/*

    Data automatically generated from recode output:

        'recode -lf "IBM850" 2>/dev/null'


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

#ifndef ibm850_h
#define ibm850_h

#define cet_cs_name_ibm850 "IBM850"

const char *cet_cs_alias_ibm850[] = 
{
	"IBM850/CR-LF", "850/CR-LF", "CP850/CR-LF", "csPC850Multilingual",
	NULL
};

#define cet_ucs4_ofs_ibm850 128
#define cet_ucs4_cnt_ibm850 128

const int cet_ucs4_map_ibm850[cet_ucs4_cnt_ibm850] =
{
	0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x00e0, 0x00e5, 0x00e7, 
	0x00ea, 0x00eb, 0x00e8, 0x00ef, 0x00ee, 0x00ec, 0x00c4, 0x00c5, 
	0x00c9, 0x00e6, 0x00c6, 0x00f4, 0x00f6, 0x00f2, 0x00fb, 0x00f9, 
	0x00ff, 0x00d6, 0x00dc, 0x00f8, 0x00a3, 0x00d8, 0x00d7, 0x0192, 
	0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x00f1, 0x00d1, 0x00aa, 0x00ba, 
	0x00bf, 0x00ae, 0x00ac, 0x00bd, 0x00bc, 0x00a1, 0x00ab, 0x00bb, 
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x00c1, 0x00c2, 0x00c0, 
	0x00a9, 0x2563, 0x2551, 0x2557, 0x255d, 0x00a2, 0x00a5, 0x2510, 
	0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x00e3, 0x00c3, 
	0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x00a4, 
	0x00f0, 0x00d0, 0x00ca, 0x00cb, 0x00c8, 0x0131, 0x00cd, 0x00ce, 
	0x00cf, 0x2518, 0x250c, 0x2588, 0x2584, 0x00a6, 0x00cc, 0x2580, 
	0x00d3, 0x00df, 0x00d4, 0x00d2, 0x00f5, 0x00d5, 0x00b5, 0x00fe, 
	0x00de, 0x00da, 0x00db, 0x00d9, 0x00fd, 0x00dd, 0x00af, 0x00b4, 
	0x00ad, 0x00b1, 0x2017, 0x00be, 0x00b6, 0x00a7, 0x00f7, 0x00b8, 
	0x00b0, 0x00a8, 0x00b7, 0x00b9, 0x00b3, 0x00b2, 0x25a0, 0x00a0
};

#define cet_ucs4_to_ibm850_ct 128

const cet_ucs4_link_t cet_ucs4_to_ibm850_links[cet_ucs4_to_ibm850_ct] =
{
	 {0x00a0, 0xff} /* space */,
	 {0x00a1, 0xad} /* exclamation mark */,
	 {0x00a2, 0xbd} /* sign */,
	 {0x00a3, 0x9c} /* sign */,
	 {0x00a4, 0xcf} /* sign */,
	 {0x00a5, 0xbe} /* sign */,
	 {0x00a6, 0xdd} /* bar */,
	 {0x00a7, 0xf5} /* sign */,
	 {0x00a8, 0xf9} /* diaeresis */,
	 {0x00a9, 0xb8} /* sign */,
	 {0x00aa, 0xa6} /* ordinal indicator */,
	 {0x00ab, 0xae} /* double angle quotation mark */,
	 {0x00ac, 0xaa} /* sign */,
	 {0x00ad, 0xf0} /* hyphen */,
	 {0x00ae, 0xa9} /* sign */,
	 {0x00af, 0xee} /* macron */,
	 {0x00b0, 0xf8} /* sign */,
	 {0x00b1, 0xf1} /* sign */,
	 {0x00b2, 0xfd} /* two */,
	 {0x00b3, 0xfc} /* three */,
	 {0x00b4, 0xef} /* accent */,
	 {0x00b5, 0xe6} /* sign */,
	 {0x00b6, 0xf4} /* sign */,
	 {0x00b7, 0xfa} /* dot */,
	 {0x00b8, 0xf7} /* cedilla */,
	 {0x00b9, 0xfb} /* one */,
	 {0x00ba, 0xa7} /* ordinal indicator */,
	 {0x00bb, 0xaf} /* double angle quotation mark */,
	 {0x00bc, 0xac} /* fraction one quarter */,
	 {0x00bd, 0xab} /* fraction one half */,
	 {0x00be, 0xf3} /* fraction three quarters */,
	 {0x00bf, 0xa8} /* question mark */,
	 {0x00c0, 0xb7} /* capital letter a with grave */,
	 {0x00c1, 0xb5} /* capital letter a with acute */,
	 {0x00c2, 0xb6} /* capital letter a with circumflex */,
	 {0x00c3, 0xc7} /* capital letter a with tilde */,
	 {0x00c4, 0x8e} /* capital letter a with diaeresis */,
	 {0x00c5, 0x8f} /* capital letter a with ring above */,
	 {0x00c6, 0x92} /* capital letter ae */,
	 {0x00c7, 0x80} /* capital letter c with cedilla */,
	 {0x00c8, 0xd4} /* capital letter e with grave */,
	 {0x00c9, 0x90} /* capital letter e with acute */,
	 {0x00ca, 0xd2} /* capital letter e with circumflex */,
	 {0x00cb, 0xd3} /* capital letter e with diaeresis */,
	 {0x00cc, 0xde} /* capital letter i with grave */,
	 {0x00cd, 0xd6} /* capital letter i with acute */,
	 {0x00ce, 0xd7} /* capital letter i with circumflex */,
	 {0x00cf, 0xd8} /* capital letter i with diaeresis */,
	 {0x00d0, 0xd1} /* capital letter eth (icelandic) */,
	 {0x00d1, 0xa5} /* capital letter n with tilde */,
	 {0x00d2, 0xe3} /* capital letter o with grave */,
	 {0x00d3, 0xe0} /* capital letter o with acute */,
	 {0x00d4, 0xe2} /* capital letter o with circumflex */,
	 {0x00d5, 0xe5} /* capital letter o with tilde */,
	 {0x00d6, 0x99} /* capital letter o with diaeresis */,
	 {0x00d7, 0x9e} /* sign */,
	 {0x00d8, 0x9d} /* capital letter o with stroke */,
	 {0x00d9, 0xeb} /* capital letter u with grave */,
	 {0x00da, 0xe9} /* capital letter u with acute */,
	 {0x00db, 0xea} /* capital letter u with circumflex */,
	 {0x00dc, 0x9a} /* capital letter u with diaeresis */,
	 {0x00dd, 0xed} /* capital letter y with acute */,
	 {0x00de, 0xe8} /* capital letter thorn (icelandic) */,
	 {0x00df, 0xe1} /* small letter sharp s (german) */,
	 {0x00e0, 0x85} /* small letter a with grave */,
	 {0x00e1, 0xa0} /* small letter a with acute */,
	 {0x00e2, 0x83} /* small letter a with circumflex */,
	 {0x00e3, 0xc6} /* small letter a with tilde */,
	 {0x00e4, 0x84} /* small letter a with diaeresis */,
	 {0x00e5, 0x86} /* small letter a with ring above */,
	 {0x00e6, 0x91} /* small letter ae */,
	 {0x00e7, 0x87} /* small letter c with cedilla */,
	 {0x00e8, 0x8a} /* small letter e with grave */,
	 {0x00e9, 0x82} /* small letter e with acute */,
	 {0x00ea, 0x88} /* small letter e with circumflex */,
	 {0x00eb, 0x89} /* small letter e with diaeresis */,
	 {0x00ec, 0x8d} /* small letter i with grave */,
	 {0x00ed, 0xa1} /* small letter i with acute */,
	 {0x00ee, 0x8c} /* small letter i with circumflex */,
	 {0x00ef, 0x8b} /* small letter i with diaeresis */,
	 {0x00f0, 0xd0} /* small letter eth (icelandic) */,
	 {0x00f1, 0xa4} /* small letter n with tilde */,
	 {0x00f2, 0x95} /* small letter o with grave */,
	 {0x00f3, 0xa2} /* small letter o with acute */,
	 {0x00f4, 0x93} /* small letter o with circumflex */,
	 {0x00f5, 0xe4} /* small letter o with tilde */,
	 {0x00f6, 0x94} /* small letter o with diaeresis */,
	 {0x00f7, 0xf6} /* sign */,
	 {0x00f8, 0x9b} /* small letter o with stroke */,
	 {0x00f9, 0x97} /* small letter u with grave */,
	 {0x00fa, 0xa3} /* small letter u with acute */,
	 {0x00fb, 0x96} /* small letter u with circumflex */,
	 {0x00fc, 0x81} /* small letter u with diaeresis */,
	 {0x00fd, 0xec} /* small letter y with acute */,
	 {0x00fe, 0xe7} /* small letter thorn (icelandic) */,
	 {0x00ff, 0x98} /* small letter y with diaeresis */,
	 {0x0131, 0xd5} /* small letter i dotless */,
	 {0x0192, 0x9f} /* minuscule latine f hame�on */,
	 {0x2017, 0xf2} /* low line */,
	 {0x2500, 0xc4} /* drawings light horizontal */,
	 {0x2502, 0xb3} /* drawings light vertical */,
	 {0x250c, 0xda} /* drawings light down and right */,
	 {0x2510, 0xbf} /* drawings light down and left */,
	 {0x2514, 0xc0} /* drawings light up and right */,
	 {0x2518, 0xd9} /* drawings light up and left */,
	 {0x251c, 0xc3} /* drawings light vertical and right */,
	 {0x2524, 0xb4} /* drawings light vertical and left */,
	 {0x252c, 0xc2} /* drawings light down and horizontal */,
	 {0x2534, 0xc1} /* drawings light up and horizontal */,
	 {0x253c, 0xc5} /* drawings light vertical and horizontal */,
	 {0x2550, 0xcd} /* drawings heavy horizontal */,
	 {0x2551, 0xba} /* drawings heavy vertical */,
	 {0x2554, 0xc9} /* drawings heavy down and right */,
	 {0x2557, 0xbb} /* drawings heavy down and left */,
	 {0x255a, 0xc8} /* drawings heavy up and right */,
	 {0x255d, 0xbc} /* drawings heavy up and left */,
	 {0x2560, 0xcc} /* drawings heavy vertical and right */,
	 {0x2563, 0xb9} /* drawings heavy vertical and left */,
	 {0x2566, 0xcb} /* drawings heavy down and horizontal */,
	 {0x2569, 0xca} /* drawings heavy up and horizontal */,
	 {0x256c, 0xce} /* drawings heavy vertical and horizontal */,
	 {0x2580, 0xdf} /* half block */,
	 {0x2584, 0xdc} /* half block */,
	 {0x2588, 0xdb} /* block */,
	 {0x2591, 0xb0} /* shade */,
	 {0x2592, 0xb1} /* shade */,
	 {0x2593, 0xb2} /* shade */,
	 {0x25a0, 0xfe} /* square */
};

/*
#define cet_ucs4_to_ibm850_extra_ct 0
const cet_ucs4_link_t cet_ucs4_to_ibm850_extra[cet_ucs4_to_ibm850_extra_ct] = {};
*/

cet_cs_vec_t cet_cs_vec_ibm850 =	/* defined in cet.h */
{	
	cet_cs_name_ibm850,		/* name of character set	*/
	cet_cs_alias_ibm850,		/* alias table			*/

	NULL,				/* ... to UCS-4 converter (multi-byte) */
	NULL,				/* UCS-4 to ... converter (multi-byte) */

	cet_ucs4_map_ibm850,		/* char to UCS-4 value table	*/
	cet_ucs4_ofs_ibm850,		/* first non standard character	*/
	cet_ucs4_cnt_ibm850,		/* number of values in table	*/

	cet_ucs4_to_ibm850_links,	/* UCS-4 to char links		*/
	cet_ucs4_to_ibm850_ct,		/* number of links		*/

	NULL,				/* hand made UCS-4 links	*/
	0,				/* number of extra links	*/

	NULL	/* for internal use */
};


/*
const int ibm850_ucs4_full_map[] =
{
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 
	0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f, 
	0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 
	0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f, 
	0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 
	0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f, 
	0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 
	0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f, 
	0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 
	0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 
	0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f, 
	0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 
	0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f, 
	0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 
	0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x007f, 
	0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x00e0, 0x00e5, 0x00e7, 
	0x00ea, 0x00eb, 0x00e8, 0x00ef, 0x00ee, 0x00ec, 0x00c4, 0x00c5, 
	0x00c9, 0x00e6, 0x00c6, 0x00f4, 0x00f6, 0x00f2, 0x00fb, 0x00f9, 
	0x00ff, 0x00d6, 0x00dc, 0x00f8, 0x00a3, 0x00d8, 0x00d7, 0x0192, 
	0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x00f1, 0x00d1, 0x00aa, 0x00ba, 
	0x00bf, 0x00ae, 0x00ac, 0x00bd, 0x00bc, 0x00a1, 0x00ab, 0x00bb, 
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x00c1, 0x00c2, 0x00c0, 
	0x00a9, 0x2563, 0x2551, 0x2557, 0x255d, 0x00a2, 0x00a5, 0x2510, 
	0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x00e3, 0x00c3, 
	0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x00a4, 
	0x00f0, 0x00d0, 0x00ca, 0x00cb, 0x00c8, 0x0131, 0x00cd, 0x00ce, 
	0x00cf, 0x2518, 0x250c, 0x2588, 0x2584, 0x00a6, 0x00cc, 0x2580, 
	0x00d3, 0x00df, 0x00d4, 0x00d2, 0x00f5, 0x00d5, 0x00b5, 0x00fe, 
	0x00de, 0x00da, 0x00db, 0x00d9, 0x00fd, 0x00dd, 0x00af, 0x00b4, 
	0x00ad, 0x00b1, 0x2017, 0x00be, 0x00b6, 0x00a7, 0x00f7, 0x00b8, 
	0x00b0, 0x00a8, 0x00b7, 0x00b9, 0x00b3, 0x00b2, 0x25a0, 0x00a0
};
*/

#endif
