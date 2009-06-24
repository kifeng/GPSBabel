/*
    Utilities for parsing Character Separated Value files (CSV)

    Copyright (C) 2002 Alex Mottram (geo_alexm at cox-internet.com)
    Copyright (C) 2002-2005 Robert Lipe

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111 USA

 */

#include <ctype.h>
#include "defs.h"
#include "csv_util.h"
#include "grtcirc.h"
#include "strptime.h"

#define MYNAME "CSV_UTIL"

/* macros */
#define LAT_DIR(a) a < 0.0 ? 'S' : 'N'
#define LON_DIR(a) a < 0.0 ? 'W' : 'E'
#define NONULL(a) a ? a : ""
#define ISWHITESPACE(a) ((a == ' ') || (a == '\t'))

/* convert excel time (days since 1900) to time_t and back again */
#define EXCEL_TO_TIMET(a) ((a - 25569.0) * 86400.0)
#define TIMET_TO_EXCEL(a) ((a / 86400.0) + 25569.0)

extern char *xcsv_urlbase;
extern char *prefer_shortnames;

extern const char *gs_get_container(geocache_container t);
extern geocache_container gs_mktype(const char *t);
extern geocache_container gs_mkcont(const char *t);

static double pathdist = 0;
static double oldlon = 999;
static double oldlat = 999;
    
static int waypt_out_count;

/*********************************************************************/
/* csv_stringclean() - remove any unwanted characters from string.   */
/*                     returns copy of string.                       */
/*     usage: p = csv_stringclean(stringtoclean, "&,\"")             */
/*            (strip out ampersands, commas, and quotes.             */
/*********************************************************************/
char *
#ifdef DEBUG_MEM
CSV_STRINGCLEAN(const char *string, const char *chararray, DEBUG_PARAMS)
#else
csv_stringclean(const char *string, const char *chararray)
#endif	
{
    char * p1;
    char * p2;
    const char * cp;
    char * tmp = xxstrdup(string,file,line);

    if ((! string) || (! chararray)) {
        return (tmp);
    }

    /* p2 - end of the original string */
    p2 = tmp + strlen(tmp);
    
    cp = chararray;

    while (*cp) {
        p1 = tmp;
        while (*p1) {
            if (*cp == *p1) {
                /* we don't want this character! */
		memmove(p1, p1 + 1, (p2 - p1));
                p1[p2 - p1] = '\0';
            }
            p1++;
        }
        cp++;
    }
    return (tmp);
}

/***********************************************************************************/
/* csv_stringtrim() - trim whitespace and leading and trailing enclosures (quotes) */
/*                    returns a copy of the modified string                        */
/*    usage: p = csv_stringtrim(string, "\"", 0)                                   */
/***********************************************************************************/
char *
#ifdef DEBUG_MEM
CSV_STRINGTRIM(const char *string, const char *enclosure, int strip_max, DEBUG_PARAMS)
#else
csv_stringtrim(const char *string, const char *enclosure, int strip_max)
#endif
{
    static const char *p1 = NULL;
    char *p2 = NULL;
    char * tmp = xxstrdup(string,file,line);
    size_t elen;
    int stripped = 0;

    if (!strlen(string)) {
	return (tmp);
    }

    if (!enclosure) {
	elen = 0;
    } else {
	elen = strlen(enclosure);
    }

    p2 = tmp + strlen(tmp) - 1;
    p1 = tmp;

    /* trim off trailing whitespace */
    while ((p2 > p1) && isspace(*p2)) {
	p2--;
    }

    /* advance p1 past any leading whitespace */
    while ((p1 < p2) && (isspace(*p1))) {
	p1++;
    }

    /* if no maximum strippage, assign a reasonable value to max */
    strip_max = strip_max ? strip_max : 9999;

    /* if we have enclosures, skip past them in pairs */
    if (elen) {
	while (
	       (stripped < strip_max) &&
	       ((size_t) (p2 - p1) >= elen) &&
	       (strncmp(p1, enclosure, elen) == 0) &&
	       (strncmp((p2 - elen + 1), enclosure, elen) == 0)) {
	    p2 -= elen;
            p1 += elen;
            stripped++;
	}
    }

    /* copy what's left over back into tmp. */
    memmove(tmp, p1, (p2 - p1) + 1);

    tmp[(p2 - p1) + 1] = '\0';

    return (tmp);
}

/*****************************************************************************/
/* csv_lineparse() - extract data fields from a delimited string. designed   */
/*                   to handle quoted and delimited data within quotes.      */
/*                   returns temporary COPY of delimited data field (use it  */
/*                   or lose it on the next call).                           */
/*    usage: p = csv_lineparse(string, ",", "\"", line)  [initial call]      */
/*           p = csv_lineparse(NULL, ",", "\"", line)    [subsequent calls]  */
/*****************************************************************************/
char *
csv_lineparse(const char *stringstart, const char *delimited_by, 
		const char *enclosed_in, const int line_no)
{
    const char *sp;
    static const char *p = NULL;
    static char *tmp = NULL;
    size_t dlen = 0, elen = 0;
    int enclosedepth = 0;
    short int dfound;
    short int hyper_whitespace_delimiter = 0;

    if (tmp) {
	xfree(tmp);
	tmp = NULL;
    }
    
    if (strcmp(delimited_by, "\\w") == 0)
        hyper_whitespace_delimiter = 1;

    /*
     * This is tacky.  Our "csv" format is actually "commaspace" format.
     * Changing that causes unwanted churn, but it also makes "real" 
     * comma separated data (such as likely to be produced by Excel, etc.) 
     * unreadable.   So we silently change it here on a read and let the
     * whitespace eater consume the space.
     */
    if (strcmp(delimited_by, ", ") == 0) {
	delimited_by = ",";
    }

    if (!p) {
	/* first pass thru */
	p =  stringstart;

	if (!p) {
	    /* last pass out */
	    return (NULL);
	}
    }

    /* the beginning of the string we start with (this pass) */
    sp = p;

    /* length of delimiters and enclosures */
    if ((delimited_by) && (!hyper_whitespace_delimiter))
        dlen = strlen(delimited_by);
    if (enclosed_in)
        elen = strlen(enclosed_in);
    
    dfound = 0;

    while ((*p) && (!dfound)) {
        if ((elen) && (strncmp(p, enclosed_in, elen) == 0)) {
	    if (enclosedepth)
		enclosedepth--;
	    else
		enclosedepth++;
	}

	if (!enclosedepth) {
	    if ((dlen) && (strncmp(p, delimited_by, dlen) == 0)) {
	        dfound = 1;
	    } else if ((hyper_whitespace_delimiter) && (ISWHITESPACE(*p))) {
	        dfound = 1;
	        while (ISWHITESPACE(*p))
	            p++;
	    } else {
	        p++;
	    }
	}
    }
    
    /* allocate enough space for this data field */
    tmp = xcalloc((p - sp) + 1, sizeof(char));

    strncpy(tmp, sp, (p - sp));
    tmp[p - sp] = '\0'; 

    if (dfound) {
	/* skip over the delimited_by */
	p += dlen;
    } else {
	/* end of the line */
	p = NULL;
    }

    if (enclosedepth != 0) {
	warning(MYNAME 
		": Warning- Unbalanced Field Enclosures (%s) on line %d\n",
	        enclosed_in, line_no);
    }

    return (tmp);
}

/*****************************************************************************/
/* dec_to_intdeg() - convert decimal degrees to integer degreees             */
/*    usage: i = dec_to_intdeg(31.1234, 1);                                  */
/*           i = dec_to_intdeg(91.1234, 0);                                  */
/*****************************************************************************/
static int
dec_to_intdeg(const double d, const int islat) 
{
    int ideg = 0;
    
    if (islat) {
        ideg = (2147483647) - (d * 8388608);
    } else {
        ideg = (2147483647) - (fabs(d) * 8388608) + 1; 
    }

    return(ideg);
}

/*****************************************************************************/
/* intdeg_to_dec() - convert integer degrees to decimal degreees             */
/*    usage: lat = dec_to_intdeg(ilat, 1);                                   */
/*           lon = dec_to_intdeg(ilon, 0);                                   */
/*****************************************************************************/
static double
intdeg_to_dec(const int ideg, const int islat) 
{
    double d;
    
    if (islat) {
        d = ((2147483647) - ideg) / (double)8388608;
    } else {
        d = ((-2147483647-1) + ideg) / (double)8388608;
    }

    return(d);
}

/*****************************************************************************/
/* decdir_to_dec() - convert a decimal/direction value into pure decimal.    */
/* usage: lat = decdir_to_dec("W90.1234");                                   */
/*        lat = decdir_to_dec("30.1234N");                                  */
/*****************************************************************************/
static double 
decdir_to_dec(const char * decdir) 
{
    char *p;
    const char *cp;
    double rval;
    int sign = 0;
    
    cp = &decdir[0];
    
    if ((*cp == 'W') || (*cp == 'S')) 
        sign = -1;
    else 
    if ((*cp == 'N') || (*cp == 'E')) 
        sign = 1;
    
    rval = sign ? strtod(&decdir[1], &p) : strtod(&decdir[0], &p);
    
    if (sign == 0) {
        if ((*p == 'W') || (*p == 'S')) 
            sign = -1;
        else 
        if ((*p == 'N') || (*p == 'E')) 
            sign = 1;
    }
    
    return(rval * sign);
}


/*****************************************************************************
 * human_to_dec() - convert a "human-readable" lat and/or lon to decimal
 * usage: human_to_dec( "N 41� 09.12' W 085� 09.36'", &lat, &lon );
 *        human_to_dec( "41 9 5.652 N", &lat, &lon );
 *        
 *        which: 0-no preference    1-prefer lat    2-prefer lon
 *****************************************************************************/

static void
human_to_dec( const char *instr, double *outlat, double *outlon, int which )
{
    double unk[3] = {999,999,999};
    double lat[3] = {999,999,999};
    double lon[3] = {999,999,999};
    int    latsign = 0;
    int    lonsign = 0;
    int    unksign = 1;
   
    const char *cur;
    double *numres = unk;
    int numind = 0;
    
    cur = instr;
       
    while ( cur && *cur ) {
	switch (*cur) {
	    case 'n': case 's': case 'N': case 'S':
		if ( unk[0] != 999 ) {
		    numind = 0;
		    numres = unk;
		    lat[0] = unk[0];
		    lat[1] = unk[1];
		    lat[2] = unk[2];
		    unk[0] = unk[1] = unk[2] = 999;
		}
		else {
		    numres = lat;
		    numind = 0;
		    lat[0] = lat[1] = lat[2] = 999;
		}
		
		if ( *cur == 'n' || *cur == 'N' ) 
		    latsign = 1;
		else
		    latsign = -1;
		cur++;
		break;
	    case 'w': case 'e': case 'W': case 'E':
		if ( unk[0] != 999 ) {
		    numind = 0;
		    numres = unk;
		    lon[0] = unk[0];
		    lon[1] = unk[1];
		    lon[2] = unk[2];
		    unk[0] = unk[1] = unk[2] = 999;
		}
		else {
		    numres = lon;
		    numind = 0;
		    lon[0] = lon[1] = lon[2] = 999;
		}
		
		if ( *cur == 'e' || *cur == 'E' ) 
		    lonsign = 1;
		else
		    lonsign = -1;
		cur++;
		break;
	    case '1': case '2': case '3': case '4': case '5':
	    case '6': case '7': case '8': case '9': case '0': case '.':
		numres[numind] = atof(cur);
		while (cur && *cur && strchr("1234567890.",*cur)) cur++;
		break;
	    case '-':
		unksign = -1;
		cur++;
		break;
	    default:
		if (numres[numind] != 999) {
    		    numind++;
		    if ( numind > 2 ) {
		        numres = unk;
		        numind = 0;
		    }
		}
		cur++;
		break;      
	}
    }
    
    if ( lat[0] == 999 && lon[0] == 999 ) {
	if ( which == 1 ) {
	    lat[0] = unk[0]; lat[1] = unk[1]; lat[2] = unk[2];
	    latsign = unksign;
	}
	else if ( which == 2 ) {
	    lon[0] = unk[0]; lon[1] = unk[1]; lon[2] = unk[2];
	    lonsign = unksign;
	}
    }
    
    if ( outlat ) {
	if ( lat[0] != 999 ) *outlat = lat[0];
	if ( lat[1] != 999 ) *outlat += lat[1]/60.0;
	if ( lat[2] != 999 ) *outlat += lat[2]/3600.0;
	if ( latsign ) *outlat *= latsign;
    }
    if ( outlon ) {
	if ( lon[0] != 999 ) *outlon = lon[0];
	if ( lon[1] != 999 ) *outlon += lon[1]/60.0;
	if ( lon[2] != 999 ) *outlon += lon[2]/3600.0;
	if ( lonsign ) *outlon *= lonsign;
    }
}

/*
 * dec_to_human - convert decimal degrees to human readable
 */

void
dec_to_human( char *buff, const char *format, const char *dirs, double val )
{
    char *subformat = NULL;
    const char *formatptr = NULL;
    char *percent = NULL;
    char *type = NULL;
    
    int  index = 0; 
    int  intvals[3] = {0,0,0};
    double  dblvals[3] = {0,0,0};
    int  sign = 0;
    
    sign = (val < 0) ? 0 : 1;
    
    dblvals[0] = fabs(val);
    intvals[0] = (int)dblvals[0];
    dblvals[1] = 60*(dblvals[0]-intvals[0]);
    intvals[1] = (int)dblvals[1];
    dblvals[2] = 60*(dblvals[1]-intvals[1]);
    intvals[2] = (int)dblvals[2];
    
    subformat = xmalloc( strlen(format)+2);
    formatptr = format;
    
    buff[0] = '\0';
    
    while ( formatptr && *formatptr ) {
	strcpy( subformat, formatptr );
	percent = strchr( subformat, '%' );
	if ( percent ) {
	    type = percent+1+strcspn( percent+1, "cdiouxXeEfgG%" );
	    *(type+1) = '\0';
	    switch( *type ) {
		case 'c':
		    sprintf( buff+strlen(buff), subformat, dirs[sign] );
		    break;
		case 'd':
		case 'i':
		case 'o':
		case 'u':
		case 'x':
		case 'X':
		    if (index>2) fatal(MYNAME ": too many format specifiers\n");
		    sprintf( buff+strlen(buff), subformat, intvals[index]);
		    index++;
		    break;
		case 'e':
		case 'E':
		case 'f':
		case 'g':
		case 'G':
		    if (index>2) fatal(MYNAME ": too many format specifiers\n");
		    sprintf( buff+strlen(buff), subformat, dblvals[index]);
		    index++;
		    break;
		case '%':
		    sprintf( buff+strlen(buff), subformat );
		    break;
		default:
		    fatal(MYNAME ": invalid format specifier\n");
		    break;

	    }
	}
	else {
	    sprintf( buff+strlen(buff), subformat );
	}
	formatptr += strlen(subformat);
    } 
    
    xfree(subformat);
}	

/*****************************************************************************/
/* xcsv_file_init() - prepare xcsv_file for first use.                       */
/*****************************************************************************/
void
xcsv_file_init(void)
{
    memset(&xcsv_file, '\0', sizeof(xcsv_file_t));

    QUEUE_INIT(&xcsv_file.prologue);
    QUEUE_INIT(&xcsv_file.epilogue);

    QUEUE_INIT(&xcsv_file.ifield);
    /* ofield is alloced to allow pointing back at ifields
     * where applicable.
     */
    xcsv_file.ofield = xcalloc(sizeof(queue), 1);
    QUEUE_INIT(xcsv_file.ofield);
    /*
     * Provide a sane default for CSV _files_.
     */
    xcsv_file.type = ff_type_file;

    xcsv_file.mkshort_handle = mkshort_new_handle();
}

/*****************************************************************************/
/* xcsv_ifield_add() - add input field to ifield queue.                      */
/* usage: xcsv_ifield_add("DESCRIPTION", "", "%s")                           */
/*****************************************************************************/
void
xcsv_ifield_add(char *key, char *val, char *pfc)
{
    field_map_t *fmp = xcalloc(sizeof(*fmp), 1);
    
    fmp->key = key;
    fmp->val = val;
    fmp->printfc = pfc;
    
    ENQUEUE_TAIL(&xcsv_file.ifield, &fmp->Q);
    xcsv_file.ifield_ct++;
}

/*****************************************************************************/
/* xcsv_ofield_add() - add output field to ofield queue.                     */
/* usage: xcsv_ofield_add("LAT_DECIMAL", "", "%08.5lf")                      */
/*****************************************************************************/
void
xcsv_ofield_add(char *key, char *val, char *pfc)
{
    field_map_t *fmp = xcalloc(sizeof(*fmp), 1);
    
    fmp->key = key;
    fmp->val = val;
    fmp->printfc = pfc;
    
    ENQUEUE_TAIL(xcsv_file.ofield, &fmp->Q);
    xcsv_file.ofield_ct++;
}

/*****************************************************************************/
/* xcsv_prologue_add() - add prologue line to prologue queue                 */
/* usage: xcsv_prologue_add("Four score and seven years ago today,")         */
/*****************************************************************************/
void
xcsv_prologue_add(char *prologue)
{
    ogue_t* ogp = xcalloc(sizeof(*ogp), 1);

    ogp->val = prologue;
    ENQUEUE_TAIL(&xcsv_file.prologue, &ogp->Q);
    xcsv_file.prologue_lines++;
}

/*****************************************************************************/
/* xcsv_epilogue_add() - add epilogue line to epilogue queue                 */
/* usage: xcsv_epilogue_add("shall not perish from the earth.")              */
/*****************************************************************************/
void
xcsv_epilogue_add(char *epilogue)
{
    ogue_t * ogp = xcalloc(sizeof(*ogp), 1);

    ogp->val = epilogue;
    ENQUEUE_TAIL(&xcsv_file.epilogue, &ogp->Q);
    xcsv_file.epilogue_lines++;
}

static
time_t
yyyymmdd_to_time(const char *s)
{
	int t = atol(s);
	struct tm tm;

	memset(&tm, 0, sizeof(tm));

	tm.tm_mday = t % 100;
	t = t / 100;
	tm.tm_mon = t % 100 - 1;
	t = t / 100;
	tm.tm_year = t - 1900;
	return mktime(&tm);
}



/*
 * sscanftime - Parse a date buffer using strftime format
 */
static
time_t
sscanftime( const char *s, const char *format, const int gmt )
{
	struct tm stm = {0,0,0,0,0,0,0,0,0};
	if ( strptime( s, format, &stm ) )
	{
		stm.tm_isdst = -1;
		if (gmt)
			return mkgmtime(&stm);
		else
			return mktime(&stm);		
	}
	
	return -1;
}
static
time_t
addhms( const char *s, const char *format )
{
	time_t tt =0;
	int  hour =0;
	int  min  =0;
	int  sec  =0;
	char * ampm = NULL;
	
	ampm = xmalloc( strlen(s) );
	if (sscanf(s, format, &hour, &min, &sec, ampm))
	   tt = ((tolower(ampm[0])=='P')?43200:0)+3600*hour+60*min+sec;
    	xfree(ampm);
    	
	return tt;
}

static 
int 
writetime(char * buff, size_t bufsize, const char * format, time_t t, int gmt )
{
	static struct tm * stmp;

	if (gmt)
		stmp = gmtime(&t);
	else
		stmp = localtime(&t);

	return strftime(buff, bufsize, format, stmp );
}

static 
int 
writeisotime(char * buff, size_t bufsize, const char * format, time_t t)
{
	static struct tm * stmp;
	char * ibuff = NULL;
	int i;
	
	ibuff = xmalloc(bufsize);
	stmp = gmtime(&t);
	strftime(ibuff, bufsize, format, stmp );
	i = snprintf(buff, bufsize, format, ibuff );
	xfree(ibuff);
	return i;
}


static 
int 
writehms(char * buff, size_t bufsize, const char * format, time_t t, int gmt )
{
	static struct tm * stmp;

	if (gmt)
		stmp = gmtime(&t);
	else
		stmp = localtime(&t);

	return snprintf(buff, bufsize, format, 
		stmp->tm_hour, stmp->tm_min, stmp->tm_sec, 
		(stmp->tm_hour>=12?"PM":"AM") );
}

static 
long 
time_to_yyyymmdd(time_t t)
{
	long b;
	struct tm *tm = gmtime(&t);

	b = (1900 + tm->tm_year) * 10000 + 
		(1 + tm->tm_mon) * 100 + 
		tm->tm_mday;

	return b;
}

/*****************************************************************************/
/* xcsv_parse_val() - parse incoming data into the waypt structure.          */
/* usage: xcsv_parse_val("-123.34", *waypt, *field_map)                      */
/*****************************************************************************/
static void
xcsv_parse_val(const char *s, waypoint *wpt, const field_map_t *fmp)
{
    if (strcmp(fmp->key, "IGNORE") == 0) {
       /* IGNORE -- Categorically ignore this... */
    } else
    if (strcmp(fmp->key, "CONSTANT") == 0) {
       /* CONSTANT -- Ignore on Input... */
    } else
    if (strcmp(fmp->key, "ANYNAME") == 0) {
       /* ANYNAME -- Ignore -- this is output magic. */
    } else
    if (strcmp(fmp->key, "INDEX") == 0) {
       /* IGNORE -- Calculated Sequence # For Ouput*/
    } else
    if (strcmp(fmp->key, "SHORTNAME") == 0) {
       wpt->shortname = csv_stringtrim(s, "", 0);
    } else
    if (strcmp(fmp->key, "DESCRIPTION") == 0) {
       wpt->description = csv_stringtrim(s, "", 0);
    } else
    if (strcmp(fmp->key, "NOTES") == 0) {
       wpt->notes = csv_stringtrim(s, "", 0);
    } else
    if (strcmp(fmp->key, "URL") == 0) {
       wpt->url = csv_stringtrim(s, "", 0);
    } else
    if (strcmp(fmp->key, "URL_LINK_TEXT") == 0) {
       wpt->url_link_text = csv_stringtrim(s, "", 0);
    } else
    if (strcmp(fmp->key, "ICON_DESCR") == 0) {
       wpt->icon_descr = csv_stringtrim(s, "", 0);
       wpt->wpt_flags.icon_descr_is_dynamic = 1;
    } else

    /* LATITUDE CONVERSIONS**************************************************/
    if (strcmp(fmp->key, "LAT_DECIMAL") == 0) {
       /* latitude as a pure decimal value */
       wpt->latitude = atof(s);
    } else
    if ((strcmp(fmp->key, "LAT_DECIMALDIR") == 0) ||
        (strcmp(fmp->key, "LAT_DIRDECIMAL") == 0)) {
       /* latitude as a decimal with N/S in it. */
       wpt->latitude = decdir_to_dec(s);
    } else
    if (strcmp(fmp->key, "LAT_INT32DEG") == 0) {
       /* latitude as a 32 bit integer offset */
       wpt->latitude = intdeg_to_dec((int) atof(s), 1);
    } else
    if ( strcmp(fmp->key, "LAT_HUMAN_READABLE") == 0) {
       human_to_dec( s, &wpt->latitude, &wpt->longitude, 1 );
    } else
    if ( strcmp(fmp->key, "LAT_NMEA") == 0) {
	wpt->latitude = ddmm2degrees(wpt->latitude);
    } else
    /* LONGITUDE CONVERSIONS ***********************************************/
    if (strcmp(fmp->key, "LON_DECIMAL") == 0) {
       /* longitude as a pure decimal value */
       wpt->longitude = atof(s);
    } else
    if ((strcmp(fmp->key, "LON_DECIMALDIR") == 0) ||
        (strcmp(fmp->key, "LON_DIRDECIMAL") == 0)) {
       /* longitude as a decimal with N/S in it. */
       wpt->longitude = decdir_to_dec(s);
    } else
    if (strcmp(fmp->key, "LON_INT32DEG") == 0) {
       /* longitude as a 32 bit integer offset  */
       wpt->longitude = intdeg_to_dec((int) atof(s), 0);
    } else
    if ( strcmp(fmp->key, "LON_HUMAN_READABLE") == 0) {
       human_to_dec( s, &wpt->latitude, &wpt->longitude, 2 );
    } else
    if ( strcmp(fmp->key, "LON_NMEA") == 0) {
	wpt->latitude = ddmm2degrees(wpt->longitude);
    } else
    /* LAT AND LON CONVERSIONS ********************************************/
    if ( strcmp(fmp->key, "LATLON_HUMAN_READABLE") == 0) {
       human_to_dec( s, &wpt->latitude, &wpt->longitude, 0 );
    } else
    /* DIRECTIONS **********************************************************/
    if (strcmp(fmp->key, "LAT_DIR") == 0) {
       /* latitude N/S.  Ignore on input for now */
    } else
    if (strcmp(fmp->key, "LON_DIR") == 0) {
       /* longitude E/W. Ingore on input for now */
    } else

    /* ALTITUDE CONVERSIONS ************************************************/
    if (strcmp(fmp->key, "ALT_FEET") == 0) {
       /* altitude in feet as a decimal value */
       wpt->altitude = atof(s) * .3048;
    } else
    if (strcmp(fmp->key, "ALT_METERS") == 0) {
       /* altitude in meters as a decimal value */
       wpt->altitude = atof(s);
    } else
    
    /* PATH CONVERSIONS ************************************************/
    if (strcmp(fmp->key, "PATH_SPEED") == 0) {
	wpt->speed = atof(s);
    } else
    if (strcmp(fmp->key, "PATH_COURSE") == 0) {
	wpt->course = atof(s);
    } else

    /* TIME CONVERSIONS ***************************************************/
    if (strcmp(fmp->key, "EXCEL_TIME") == 0) {
       /* Time as Excel Time  */
       wpt->creation_time = EXCEL_TO_TIMET(atof(s));
    } else
    if (strcmp(fmp->key, "TIMET_TIME") == 0) {
       /* Time as time_t */
       wpt->creation_time = atol(s);
     } else
    if (strcmp(fmp->key, "YYYYMMDD_TIME") == 0) {
	wpt->creation_time = yyyymmdd_to_time(s);
    } else
    if (strcmp(fmp->key, "GMT_TIME") == 0) {
	wpt->creation_time = sscanftime(s, fmp->printfc, 1);
    } else
    if (strcmp(fmp->key, "LOCAL_TIME") == 0) {
	wpt->creation_time = sscanftime(s, fmp->printfc, 0);
    } else
    /* Useful when time and date are in separate fields 
    	GMT / Local offset is handled by the two cases above */
    if ((strcmp(fmp->key, "HMSG_TIME") == 0)||
    	(strcmp(fmp->key, "HMSL_TIME") == 0) ) {
	wpt->creation_time += addhms(s, fmp->printfc);
    } else
    if (strcmp(fmp->key, "ISO_TIME") == 0) {
	wpt->creation_time = xml_parse_time(s);
    } else
	if (strcmp(fmp->key, "GEOCACHE_LAST_FOUND") == 0) {
	wpt->gc_data.last_found = yyyymmdd_to_time(s);
    } else

    /* GEOCACHING STUFF ***************************************************/
    if (strcmp(fmp->key, "GEOCACHE_DIFF") == 0) {
       /* Geocache Difficulty as an int */
       wpt->gc_data.diff = atof(s) * 10; 
    } else
    if (strcmp(fmp->key, "GEOCACHE_TERR") == 0) {
       /* Geocache Terrain as an int */
       wpt->gc_data.terr = atof(s) * 10;
    } else
    if (strcmp(fmp->key, "GEOCACHE_TYPE") == 0) {
       /* Geocache Type */
       wpt->gc_data.type = gs_mktype(s);
    } else
    if (strcmp(fmp->key, "GEOCACHE_CONTAINER") == 0) {
       wpt->gc_data.container = gs_mkcont(s);
    } else
    if (strcmp(fmp->key, "GEOCACHE_HINT") == 0) {
       wpt->gc_data.hint = csv_stringtrim(s, "", 0);
    } else
    if (strcmp(fmp->key, "GEOCACHE_PLACER") == 0) {
       wpt->gc_data.placer = csv_stringtrim(s, "", 0);
    } else
	
    /* GPS STUFF *******************************************************/
    if (strcmp(fmp->key, "GPS_HDOP") == 0) {
	 wpt->hdop = atof(s);
    } else
    if (strcmp(fmp->key, "GPS_VDOP") == 0) {
	wpt->vdop = atof(s);
    } else
    if (strcmp(fmp->key, "GPS_PDOP") == 0) {
        wpt->pdop = atof(s);
    } else
    if (strcmp(fmp->key, "GPS_SAT") == 0) {
	wpt->sat = atoi(s);
    } else
    if (strcmp(fmp->key, "GPS_FIX") == 0) {
	wpt->fix = atoi(s)-1;
	if ( wpt->fix < fix_2d) {
	if (!strcasecmp(s, "none"))
		wpt->fix = fix_none;
	else if (!strcasecmp(s, "dgps"))
		wpt->fix = fix_dgps;
	else if (!strcasecmp(s, "pps"))
		wpt->fix = fix_pps;
	else
		wpt->fix = fix_unknown;
	}
    } else
	
    /* OTHER STUFF ***************************************************/
    if ( strcmp( fmp->key, "PATH_DISTANCE_MILES") == 0) {
       /* Ignored on input */
    } else
    if ( strcmp( fmp->key, "PATH_DISTANCE_KM") == 0 ) {
       /* Ignored on input */
    } else {
       warning( MYNAME ": Unknown style directive: %s\n", fmp->key);
    }
}

/*****************************************************************************/
/* xcsv_data_read() - read input file, parsing lines, fields and handling    */
/*                   any data conversion (the input meat)                    */
/*****************************************************************************/
void
xcsv_data_read(void)
{
    char buff[8192];
    char *s;
    waypoint *wpt_tmp;
    int linecount = 0;
    queue *elem, *tmp;
    field_map_t *fmp;
    ogue_t *ogp;

    do {
        linecount++;
        memset(buff, '\0', sizeof(buff));
        fgets(buff, sizeof(buff), xcsv_file.xcsvfp);

        rtrim(buff);

        /* skip over x many lines on the top for the prologue... */
        if ((xcsv_file.prologue_lines) && ((linecount - 1) <
          xcsv_file.prologue_lines)) {
            continue;
        }

        /* We should skip over epilogue lines also.  Since we don't want to 
         * pre-read the file to know how many data lines we should be seeing, 
         * we take this cheap shot at the data and cross our fingers.
         */

        QUEUE_FOR_EACH(&xcsv_file.epilogue, elem, tmp) {
            ogp = (ogue_t *) elem;
            if (strncmp(buff, ogp->val, strlen(ogp->val)) == 0) {
                buff[0] = '\0';
                break;
            }
        }

        if (strlen(buff)) {
            wpt_tmp = waypt_new();

            s = buff;
            s = csv_lineparse(s, xcsv_file.field_delimiter, "", linecount);

            /* reset the ifield queue */
            elem = QUEUE_FIRST(&xcsv_file.ifield);

            /* now rip the line apart, advancing the queue for each tear
             * off the beginning of buff since there's no index into queue.
             */
            while (s) {
                fmp = (field_map_t *) elem;
                xcsv_parse_val(s, wpt_tmp, fmp);

                elem = QUEUE_NEXT(elem);
                
                if (elem == &xcsv_file.ifield) {
                    /* we've wrapped the queue. so stop parsing! */
		    while (s) {
			    s=csv_lineparse(NULL, "\xff","",linecount);
		    }
                    break;
                }

                s = csv_lineparse(NULL, xcsv_file.field_delimiter, "",
                  linecount);
            }
            waypt_add(wpt_tmp);
        }

    } while (!feof(xcsv_file.xcsvfp));
}

static void
xcsv_resetpathlen(const route_head *head)
{
    pathdist = 0;
    oldlat = 999;
    oldlon = 999;
}

/*****************************************************************************/
/* xcsv_waypt_pr() - write output file, handling output conversions          */
/*                  (the output meat)                                        */
/*****************************************************************************/
static void
xcsv_waypt_pr(const waypoint *wpt)
{
    char buff[1024];
    char *shortname = NULL;
    char *description = NULL;
    char * anyname = NULL;
    char * write_delimiter;
    int i;
    field_map_t *fmp;
    queue *elem, *tmp;
    
    if ( oldlon < 900 ) {
	pathdist += tomiles(gcdist(oldlat*M_PI/180,oldlon*M_PI/180,
			wpt->latitude*M_PI/180,wpt->longitude*M_PI/180));
    }
    oldlon = wpt->longitude;
    oldlat = wpt->latitude;

    if (xcsv_file.field_delimiter && strcmp(xcsv_file.field_delimiter, "\\w") == 0)
        write_delimiter = " ";
    else
        write_delimiter = xcsv_file.field_delimiter;
    
    if ((! wpt->shortname) || (global_opts.synthesize_shortnames)) {
        if (wpt->description) {
            if (global_opts.synthesize_shortnames) 
		shortname = mkshort_from_wpt(xcsv_file.mkshort_handle, wpt);
            else
                shortname = csv_stringclean(wpt->description, xcsv_file.badchars);
        } else {
            /* no shortname available -- let shortname default on output */
        }
    } else{
        shortname = csv_stringclean(wpt->shortname, xcsv_file.badchars);
    }

    if (! wpt->description) {
        if (shortname) {
            description = csv_stringclean(shortname, xcsv_file.badchars);
        } else {
            /* no description -- let description default on output */
        }
    } else {
        description = csv_stringclean(wpt->description, xcsv_file.badchars);
    }

    if (prefer_shortnames) {
	    if (description) {
		    xfree(description);
	    }
 	    description = shortname;
    } else if (description) {
	    char *odesc = description;
	    description = str_utf8_to_ascii(odesc);
	    xfree(odesc);
    }

    i = 0;
    QUEUE_FOR_EACH(xcsv_file.ofield, elem, tmp) {
	char *obuff;
        fmp = (field_map_t *) elem;

        if (i != 0) 
            fprintf (xcsv_file.xcsvfp, write_delimiter);

        i++;
#define writebuff(b, fmt, data) snprintf(b, sizeof(b), fmt, data)
        if (strcmp(fmp->key, "IGNORE") == 0) {
            /* IGNORE -- Write the char printf conversion */
            writebuff(buff, fmp->printfc, "");
        } else
        if (strcmp(fmp->key, "INDEX") == 0) {
            writebuff(buff, fmp->printfc, waypt_out_count + atoi(fmp->val));
        } else
        if (strcmp(fmp->key, "CONSTANT") == 0) {
	    const char *cp = xcsv_get_char_from_constant_table(fmp->val);
	    if (cp) {
                writebuff(buff, fmp->printfc, cp);
	    } else {
		writebuff(buff, fmp->printfc, fmp->val);
	    }
        } else
        if (strcmp(fmp->key, "SHORTNAME") == 0) {
            writebuff(buff, fmp->printfc, 
                (shortname && *shortname) ? shortname : fmp->val);
        } else
        if (strcmp(fmp->key, "ANYNAME") == 0) {
            if (wpt->shortname) {
                anyname = xstrdup(wpt->shortname);
            } else
            if (wpt->description) {
                anyname = mkshort(xcsv_file.mkshort_handle, wpt->description);
            } else
            if (wpt->notes) {
                anyname = xstrdup(wpt->notes);
            } else
                anyname = xstrdup(fmp->val);

            if ((anyname) && (global_opts.synthesize_shortnames)) {
                anyname = xstrdup(shortname);
	    }

            writebuff(buff, fmp->printfc, anyname);
            
            xfree(anyname);
        } else
        if (strcmp(fmp->key, "DESCRIPTION") == 0) {
            writebuff(buff, fmp->printfc, 
                (description && *description) ? description : fmp->val);
        } else
        if (strcmp(fmp->key, "NOTES") == 0) {
	    writebuff(buff, fmp->printfc, 
	        (wpt->notes && *wpt->notes) ? wpt->notes : fmp->val);
        } else
        if (strcmp(fmp->key, "URL") == 0) {
	    int off = 0;
	    if (xcsv_urlbase) {
		strcpy(buff, xcsv_urlbase);
		off = strlen(xcsv_urlbase);
	    }
	    if (wpt->url)
		snprintf(buff + off, sizeof(buff) - off, fmp->printfc, wpt->url);
	    else
		strcpy(buff, (fmp->val && *fmp->val) ? fmp->val : "\"\"");
        } else
        if (strcmp(fmp->key, "URL_LINK_TEXT") == 0) {
            snprintf(buff, sizeof(buff), fmp->printfc, 
                (wpt->url_link_text && *wpt->url_link_text) ? wpt->url_link_text : fmp->val);
        } else
        if (strcmp(fmp->key, "ICON_DESCR") == 0) {
            writebuff(buff, fmp->printfc, 
                (wpt->icon_descr && *wpt->icon_descr) ? 
                wpt->icon_descr : fmp->val);
        } else

        /* LATITUDE CONVERSION***********************************************/
        if (strcmp(fmp->key, "LAT_DECIMAL") == 0) {
            /* latitude as a pure decimal value */
            writebuff(buff, fmp->printfc, wpt->latitude);
        } else
        if (strcmp(fmp->key, "LAT_DECIMALDIR") == 0) {
            /* latitude as a decimal value with N/S after it */
            snprintf(buff, sizeof(buff), fmp->printfc, fabs(wpt->latitude), 
              LAT_DIR(wpt->latitude));
        } else
        if (strcmp(fmp->key, "LAT_DIRDECIMAL") == 0) {
            /* latitude as a decimal value with N/S before it */
            snprintf(buff, sizeof(buff), fmp->printfc, 
              LAT_DIR(wpt->latitude),
              fabs(wpt->latitude));
        } else
        if (strcmp(fmp->key, "LAT_INT32DEG") == 0) {
            /* latitude as an integer offset from 0 degrees */
            writebuff(buff, fmp->printfc,
              dec_to_intdeg(wpt->latitude, 1));
        } else
	if (strcmp(fmp->key, "LAT_HUMAN_READABLE") == 0) {
	    dec_to_human( buff, fmp->printfc, "SN", wpt->latitude );
	} else
	if (strcmp(fmp->key, "LAT_NMEA") == 0) {
		writebuff(buff, fmp->printfc, degrees2ddmm(wpt->latitude));
	} else

        /* LONGITUDE CONVERSIONS*********************************************/
        if (strcmp(fmp->key, "LON_DECIMAL") == 0) {
            /* longitude as a pure decimal value */
            writebuff(buff, fmp->printfc, wpt->longitude);
        } else
        if (strcmp(fmp->key, "LON_DECIMALDIR") == 0) {
            /* latitude as a decimal value with N/S after it */
            snprintf(buff, sizeof(buff),  fmp->printfc,
              fabs(wpt->longitude), 
              LON_DIR(wpt->longitude));
        } else
        if (strcmp(fmp->key, "LON_DIRDECIMAL") == 0) {
            /* latitude as a decimal value with N/S before it */
            snprintf(buff, sizeof(buff), fmp->printfc,
              LON_DIR(wpt->longitude),
              fabs(wpt->longitude));
        } else
        if (strcmp(fmp->key, "LON_INT32DEG") == 0) {
            /* longitudee as an integer offset from 0 degrees */
            writebuff(buff, fmp->printfc,
              dec_to_intdeg(wpt->longitude, 0));
        } else
	if (strcmp(fmp->key, "LON_HUMAN_READABLE") == 0) {
	    dec_to_human( buff, fmp->printfc, "WE", wpt->longitude );
	} else
	if (strcmp(fmp->key, "LATLON_HUMAN_READABLE") == 0) {
	    dec_to_human( buff, fmp->printfc, "SN", wpt->latitude );
	    if ( !isspace(buff[strlen(buff)])) strcat( buff, " " );
	    dec_to_human( buff+strlen(buff), fmp->printfc, "WE", 
			    wpt->longitude );
	} else
	if (strcmp(fmp->key, "LON_NMEA") == 0) {
		writebuff(buff, fmp->printfc, degrees2ddmm(wpt->longitude));
	} else

        /* DIRECTIONS *******************************************************/
        if (strcmp(fmp->key, "LAT_DIR") == 0) {
            /* latitude N/S as a char */
            writebuff(buff, fmp->printfc,
            LAT_DIR(wpt->latitude));
        } else
        if (strcmp(fmp->key, "LON_DIR") == 0) {
            /* longitude E/W as a char */
            writebuff(buff, fmp->printfc,
              LON_DIR(wpt->longitude));
        } else

        /* ALTITUDE CONVERSIONS**********************************************/
        if (strcmp(fmp->key, "ALT_FEET") == 0) {
            /* altitude in feet as a decimal value */
            writebuff(buff, fmp->printfc,
              (wpt->altitude * 3.2808));
        } else
        if (strcmp(fmp->key, "ALT_METERS") == 0) {
            /* altitude in meters as a decimal value */
            writebuff(buff, fmp->printfc,
              wpt->altitude);
        } else
		
        /* DISTANCE CONVERSIONS**********************************************/
	if (strcmp(fmp->key, "PATH_DISTANCE_MILES") == 0) {
            /* path (route/track) distance in miles */
            writebuff( buff, fmp->printfc, pathdist );
	} else
	if (strcmp(fmp->key, "PATH_DISTANCE_KM") == 0) {
            /* path (route/track) distance in  */
            writebuff( buff, fmp->printfc, pathdist * 5280*12*2.54/100/1000 );
	} else
	if (strcmp(fmp->key, "PATH_SPEED") == 0) {
            writebuff( buff, fmp->printfc, wpt->speed );
	} else
	if (strcmp(fmp->key, "PATH_COURSE") == 0) {
            writebuff( buff, fmp->printfc, wpt->course );
	} else

        /* TIME CONVERSIONS**************************************************/
        if (strcmp(fmp->key, "EXCEL_TIME") == 0) {
            /* creation time as an excel (double) time */
            writebuff(buff, fmp->printfc, TIMET_TO_EXCEL(wpt->creation_time));
        } else
        if (strcmp(fmp->key, "TIMET_TIME") == 0) {
            /* time as a time_t variable */
            writebuff(buff, fmp->printfc, wpt->creation_time);
        } else
        if (strcmp(fmp->key, "YYYYMMDD_TIME") == 0) {
	    writebuff(buff, fmp->printfc, time_to_yyyymmdd(wpt->creation_time));
	} else
	if (strcmp(fmp->key, "GMT_TIME") == 0) {
	    writetime(buff, sizeof buff, fmp->printfc, wpt->creation_time, 1 );
	} else
        if (strcmp(fmp->key, "LOCAL_TIME") == 0) {
            writetime(buff, sizeof buff, fmp->printfc, wpt->creation_time, 0 );
	} else
        if (strcmp(fmp->key, "HMSG_TIME") == 0) {
            writehms(buff, sizeof buff, fmp->printfc, wpt->creation_time, 1 );
	} else
        if (strcmp(fmp->key, "HMSL_TIME") == 0) {
            writehms(buff, sizeof buff, fmp->printfc, wpt->creation_time, 0 );
	} else
	if (strcmp(fmp->key, "ISO_TIME") == 0) {
            writetime(buff, sizeof buff, "%Y-%m-%dT%H:%M:%SZ", wpt->creation_time, 1 );
	} else
        if (strcmp(fmp->key, "GEOCACHE_LAST_FOUND") == 0) {
	    writebuff(buff, fmp->printfc, time_to_yyyymmdd(wpt->gc_data.last_found));
	} else

        /* GEOCACHE STUFF **************************************************/
        if (strcmp(fmp->key, "GEOCACHE_DIFF") == 0) {
            /* Geocache Difficulty as a double */
            writebuff(buff, fmp->printfc, wpt->gc_data.diff / 10.0);
        } else
        if (strcmp(fmp->key, "GEOCACHE_TERR") == 0) {
            /* Geocache Terrain as a double */
            writebuff(buff, fmp->printfc, wpt->gc_data.terr / 10.0);
        } else
        if (strcmp(fmp->key, "GEOCACHE_CONTAINER") == 0) {
            /* Geocache Container */
            writebuff(buff, fmp->printfc, gs_get_container(wpt->gc_data.container));
	} else
	if (strcmp(fmp->key, "GEOCACHE_TYPE") == 0) {
            /* Geocache Type */
            writebuff(buff, fmp->printfc, gs_get_cachetype(wpt->gc_data.type));
        } else 
	if (strcmp(fmp->key, "GEOCACHE_HINT") == 0) {
	    writebuff(buff, fmp->printfc, NONULL(wpt->gc_data.hint));
        } else 
	if (strcmp(fmp->key, "GEOCACHE_PLACER") == 0) {
	    writebuff(buff, fmp->printfc, NONULL(wpt->gc_data.placer));
        } else
	
	/* GPS STUFF *******************************************************/
	if (strcmp(fmp->key, "GPS_HDOP") == 0) {
            writebuff(buff, fmp->printfc, wpt->hdop);
        } else
	if (strcmp(fmp->key, "GPS_VDOP") == 0) {
            writebuff(buff, fmp->printfc, wpt->vdop);
        } else
	if (strcmp(fmp->key, "GPS_PDOP") == 0) {
            writebuff(buff, fmp->printfc, wpt->pdop);
        } else
	if (strcmp(fmp->key, "GPS_SAT") == 0) {
            writebuff(buff, fmp->printfc, wpt->sat);
        } else
	if (strcmp(fmp->key, "GPS_FIX") == 0) {
		char *fix = NULL;
		switch (wpt->fix) {
			case fix_unknown:
				fix = "Unknown";
				break;
			case fix_none:
				fix = "None";
				break;
			case fix_2d:
				fix = "2d";
				break;
			case fix_3d:
				fix = "3d";
				break;
			case fix_dgps:
				fix = "dgps";
				break;
			case fix_pps:
				fix = "pps";
				break;
		}
		writebuff(buff, fmp->printfc, fix);
        } else

	{
		/* this should probably never happen */
        }
	

        obuff = csv_stringclean(buff, xcsv_file.badchars);
        fprintf (xcsv_file.xcsvfp, "%s", obuff);
	xfree(obuff);
    }

    fprintf (xcsv_file.xcsvfp, "%s", xcsv_file.record_delimiter);

    if (description && description != shortname)
        xfree(description);

    if (shortname)
        xfree(shortname);

    /* increment the index counter */
    waypt_out_count++;
}

static void
xcsv_noop(const route_head *wp)
{
	        /* no-op */
}

/*****************************************************************************/
/* xcsv_data_write(void) - write prologues, spawn the output loop, and write */
/*                         epilogues.                                        */
/*****************************************************************************/
void
xcsv_data_write(void)
{
    queue *elem, *tmp;
    ogue_t *ogp;

    /* reset the index counter */
    waypt_out_count = 0;
    
    /* output prologue lines, if any. */
    QUEUE_FOR_EACH(&xcsv_file.prologue, elem, tmp) {
       char *ol;
       ogp = (ogue_t *) elem;

       ol = strsub(ogp->val, "__FILE__", xcsv_file.fname);

       if (ol) {
               fprintf(xcsv_file.xcsvfp, "%s", ol);
               xfree(ol);
       } else {
               fprintf(xcsv_file.xcsvfp, "%s", ogp->val);
       }
       fprintf(xcsv_file.xcsvfp, "%s", xcsv_file.record_delimiter);
    }

    waypt_disp_all(xcsv_waypt_pr);
    route_disp_all(xcsv_resetpathlen,xcsv_noop,xcsv_waypt_pr);
    track_disp_all(xcsv_resetpathlen,xcsv_noop,xcsv_waypt_pr);

    /* output epilogue lines, if any. */
    QUEUE_FOR_EACH(&xcsv_file.epilogue, elem, tmp) {
        ogp = (ogue_t *) elem;
        fprintf (xcsv_file.xcsvfp, "%s%s", ogp->val, xcsv_file.record_delimiter);
    }
}
