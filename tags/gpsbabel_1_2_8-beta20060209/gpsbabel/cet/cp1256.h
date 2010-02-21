/*

    Data automatically generated from recode output:

        'recode -lf "CP1256" 2>/dev/null'


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

#ifndef cp1256_h
#define cp1256_h

#define cet_cs_name_cp1256 "CP1256"

const char *cet_cs_alias_cp1256[] = 
{
	"CP1256/CR-LF", "1256/CR-LF", "ms-arab", "windows-1256", "WIN-CP1256",
	NULL
};

#define cet_ucs4_ofs_cp1256 128
#define cet_ucs4_cnt_cp1256 128

const int cet_ucs4_map_cp1256[cet_ucs4_cnt_cp1256] =
{
	0x20ac, 0x0660, 0x201a, 0x0661, 0x201e, 0x2026, 0x2020, 0x2021, 
	0x0662, 0x0663, 0x0664, 0x2039, 0x0665, 0x0666, 0x0667, 0x0668, 
	0x0669, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014, 
	0x061b, 0x2122, 0x061f, 0x203a, 0x0621, 0x0622, 0x0623, 0x0178, 
	0x00a0, 0x0624, 0x0625, 0x00a3, 0x00a4, 0x0626, 0x00a6, 0x00a7, 
	0x0627, 0x00a9, 0x0628, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x067e, 
	0x00b0, 0x00b1, 0x0629, 0x062a, 0x062b, 0x00b5, 0x00b6, 0x00b7, 
	0x062c, 0x0686, 0x062d, 0x00bb, 0x062e, 0x062f, 0x0630, 0x0631, 
	0x00c0, 0x0632, 0x00c2, 0x0698, 0x0633, 0x0634, 0x0635, 0x00c7, 
	0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x0636, 0x0637, 0x00ce, 0x00cf, 
	0x3113, 0x0639, 0x063a, 0x0640, 0x00d4, 0x0641, 0x0642, 0x00d7, 
	0x0643, 0x00d9, 0x06af, 0x00db, 0x00dc, 0x0644, 0x0645, 0x0646, 
	0x00e0, 0x0647, 0x00e2, 0x0681, 0x0648, 0x0649, 0x064a, 0x00e7, 
	0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x064b, 0x064c, 0x00ee, 0x00ef, 
	0x064d, 0x064e, 0x064f, 0x0650, 0x00f4, 0x0651, 0x0652, 0x00f7, 
	    -1, 0x00f9,     -1, 0x00fb, 0x00fc, 0x200e, 0x200f, 0x00ff
};

#define cet_ucs4_to_cp1256_ct 81

const cet_ucs4_link_t cet_ucs4_to_cp1256_links[cet_ucs4_to_cp1256_ct] =
{
	 {0x0178, 0x9f} /* capital letter y with diaeresis */,
	 {0x061b, 0x98} /* semicolon */,
	 {0x061f, 0x9a} /* question mark */,
	 {0x0621, 0x9c} /* letter hamza */,
	 {0x0622, 0x9d} /* letter alef with madda above */,
	 {0x0623, 0x9e} /* letter alef with hamza above */,
	 {0x0624, 0xa1} /* letter waw with hamza above */,
	 {0x0625, 0xa2} /* letter alef with hamza below */,
	 {0x0626, 0xa5} /* letter yeh with hamza above */,
	 {0x0627, 0xa8} /* letter alef */,
	 {0x0628, 0xaa} /* letter beh */,
	 {0x0629, 0xb2} /* letter teh marbuta */,
	 {0x062a, 0xb3} /* letter teh */,
	 {0x062b, 0xb4} /* letter theh */,
	 {0x062c, 0xb8} /* letter jeem */,
	 {0x062d, 0xba} /* letter hah */,
	 {0x062e, 0xbc} /* letter khah */,
	 {0x062f, 0xbd} /* letter dal */,
	 {0x0630, 0xbe} /* letter thal */,
	 {0x0631, 0xbf} /* letter reh */,
	 {0x0632, 0xc1} /* letter zain */,
	 {0x0633, 0xc4} /* letter seen */,
	 {0x0634, 0xc5} /* letter sheen */,
	 {0x0635, 0xc6} /* letter sad */,
	 {0x0636, 0xcc} /* letter dad */,
	 {0x0637, 0xcd} /* letter tah */,
	 {0x0639, 0xd1} /* letter ain */,
	 {0x063a, 0xd2} /* letter ghain */,
	 {0x0640, 0xd3} /* tatweel */,
	 {0x0641, 0xd5} /* letter feh */,
	 {0x0642, 0xd6} /* letter qaf */,
	 {0x0643, 0xd8} /* letter kaf */,
	 {0x0644, 0xdd} /* letter lam */,
	 {0x0645, 0xde} /* letter meem */,
	 {0x0646, 0xdf} /* letter noon */,
	 {0x0647, 0xe1} /* letter heh */,
	 {0x0648, 0xe4} /* letter waw */,
	 {0x0649, 0xe5} /* letter alef maksura */,
	 {0x064a, 0xe6} /* letter yeh */,
	 {0x064b, 0xec} /* fathatan */,
	 {0x064c, 0xed} /* dammatan */,
	 {0x064d, 0xf0} /* kasratan */,
	 {0x064e, 0xf1} /* fatha */,
	 {0x064f, 0xf2} /* damma */,
	 {0x0650, 0xf3} /* kasra */,
	 {0x0651, 0xf5} /* shadda */,
	 {0x0652, 0xf6} /* sukun */,
	 {0x0660, 0x81} /* arabic-indic digit zero */,
	 {0x0661, 0x83} /* arabic-indic digit one */,
	 {0x0662, 0x88} /* arabic-indic digit two */,
	 {0x0663, 0x89} /* arabic-indic digit three */,
	 {0x0664, 0x8a} /* arabic-indic digit four */,
	 {0x0665, 0x8c} /* arabic-indic digit five */,
	 {0x0666, 0x8d} /* arabic-indic digit six */,
	 {0x0667, 0x8e} /* arabic-indic digit seven */,
	 {0x0668, 0x8f} /* arabic-indic digit eight */,
	 {0x0669, 0x90} /* arabic-indic digit nine */,
	 {0x067e, 0xaf} /* letter peh */,
	 {0x0681, 0xe3} /* arabe ha' hamza en chef */,
	 {0x0686, 0xb9} /* arabe tchim' */,
	 {0x0698, 0xc3} /* arabe ja' */,
	 {0x06af, 0xda} /* letter gaf */,
	 {0x200e, 0xfd} /* gauche-�-droite */,
	 {0x200f, 0xfe} /* droite-�-gauche */,
	 {0x2013, 0x96} /* dash */,
	 {0x2014, 0x97} /* dash */,
	 {0x2018, 0x91} /* single quotation mark */,
	 {0x2019, 0x92} /* single quotation mark */,
	 {0x201a, 0x82} /* low-9 quotation mark */,
	 {0x201c, 0x93} /* double quotation mark */,
	 {0x201d, 0x94} /* double quotation mark */,
	 {0x201e, 0x84} /* low-9 quotation mark */,
	 {0x2020, 0x86} /* dagger */,
	 {0x2021, 0x87} /* dagger */,
	 {0x2022, 0x95} /* puce */,
	 {0x2026, 0x85} /* horizontal ellipsis */,
	 {0x2039, 0x8b} /* left-pointing angle quotation mark */,
	 {0x203a, 0x9b} /* right-pointing angle quotation mark */,
	 {0x20ac, 0x80} /* euro */,
	 {0x2122, 0x99} /* mark sign */,
	 {0x3113, 0xd0} /* letter zh */
};

/*
#define cet_ucs4_to_cp1256_extra_ct 0
const cet_ucs4_link_t cet_ucs4_to_cp1256_extra[cet_ucs4_to_cp1256_extra_ct] = {};
*/

cet_cs_vec_t cet_cs_vec_cp1256 =	/* defined in cet.h */
{	
	cet_cs_name_cp1256,		/* name of character set	*/
	cet_cs_alias_cp1256,		/* alias table			*/

	NULL,				/* ... to UCS-4 converter (multi-byte) */
	NULL,				/* UCS-4 to ... converter (multi-byte) */

	cet_ucs4_map_cp1256,		/* char to UCS-4 value table	*/
	cet_ucs4_ofs_cp1256,		/* first non standard character	*/
	cet_ucs4_cnt_cp1256,		/* number of values in table	*/

	cet_ucs4_to_cp1256_links,	/* UCS-4 to char links		*/
	cet_ucs4_to_cp1256_ct,		/* number of links		*/

	NULL,				/* hand made UCS-4 links	*/
	0,				/* number of extra links	*/

	NULL	/* for internal use */
};


/*
const int cp1256_ucs4_full_map[] =
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
	0x20ac, 0x0660, 0x201a, 0x0661, 0x201e, 0x2026, 0x2020, 0x2021, 
	0x0662, 0x0663, 0x0664, 0x2039, 0x0665, 0x0666, 0x0667, 0x0668, 
	0x0669, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014, 
	0x061b, 0x2122, 0x061f, 0x203a, 0x0621, 0x0622, 0x0623, 0x0178, 
	0x00a0, 0x0624, 0x0625, 0x00a3, 0x00a4, 0x0626, 0x00a6, 0x00a7, 
	0x0627, 0x00a9, 0x0628, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x067e, 
	0x00b0, 0x00b1, 0x0629, 0x062a, 0x062b, 0x00b5, 0x00b6, 0x00b7, 
	0x062c, 0x0686, 0x062d, 0x00bb, 0x062e, 0x062f, 0x0630, 0x0631, 
	0x00c0, 0x0632, 0x00c2, 0x0698, 0x0633, 0x0634, 0x0635, 0x00c7, 
	0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x0636, 0x0637, 0x00ce, 0x00cf, 
	0x3113, 0x0639, 0x063a, 0x0640, 0x00d4, 0x0641, 0x0642, 0x00d7, 
	0x0643, 0x00d9, 0x06af, 0x00db, 0x00dc, 0x0644, 0x0645, 0x0646, 
	0x00e0, 0x0647, 0x00e2, 0x0681, 0x0648, 0x0649, 0x064a, 0x00e7, 
	0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x064b, 0x064c, 0x00ee, 0x00ef, 
	0x064d, 0x064e, 0x064f, 0x0650, 0x00f4, 0x0651, 0x0652, 0x00f7, 
	    -1, 0x00f9,     -1, 0x00fb, 0x00fc, 0x200e, 0x200f, 0x00ff
};
*/

#endif