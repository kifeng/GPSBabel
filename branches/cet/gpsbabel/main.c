/*
    Copyright (C) 2002-2005 Robert Lipe, robertlipe@usa.net

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


#include "defs.h"
#include "cet.h"
#include "cet_util.h"
#include <ctype.h>

global_options global_opts;
const char gpsbabel_version[] = VERSION;

static void
usage(const char *pname, int shorter)
{
	printf("GPSBabel Version %s.  http://www.gpsbabel.org\n\n",
			VERSION );
	printf(
"Usage:\n"
"    %s [options] -i INTYPE -f INFILE -o OUTTYPE -F OUTFILE\n"
"    %s [options] -i INTYPE -o OUTTYPE INFILE [OUTFILE]\n"
"\n"
"    Converts GPS route and waypoint data from one format type to another.\n"
"    The input type and filename are specified with the -i INTYPE\n"
"    and -f INFILE options. The output type and filename are specified\n"
"    with the -o OUTTYPE and -F OUTFILE options.\n"
"    If '-' is used for INFILE or OUTFILE, stdin or stdout will be used.\n"
"\n"
"    In the second form of the command, INFILE and OUTFILE are the\n"
"    first and second positional (non-option) arguments.\n"
"\n"
"    INTYPE and OUTTYPE must be one of the file types listed below, and\n"
"    may include options valid for that file type.  For example:\n"
"      'gpx', 'gpx,snlen=10' and 'ozi,snlen=10,snwhite=1'\n"
"    (without the quotes) are all valid file type specifications.\n"
"\n"
"Options:\n"
"    -s               Synthesize shortnames\n"
"    -r               Process route information\n"
"    -t               Process track information\n"
"    -w               Process waypoint information [default]\n"
"    -c               Character set for next operation\n"
"    -N               No smart icons on output\n"
"    -x filtername    Invoke filter (place between inputs and output) \n"
"    -D level         Set debug level [%d]\n"
"    -l               Print GPSBabel builtin character sets and exit\n"
"    -h, -?           Print detailed help and exit\n"
"    -V               Print GPSBabel version and exit\n"
"\n"
	, pname
	, pname
	, global_opts.debug_level
	);
	if ( shorter ) {
		printf( "\n\n[Press enter]" );
		fgetc(stdin);
	}
	else {
	        printf("File Types (-i and -o options):\n");
		disp_vecs();
		printf("\nSupported data filters:\n");
		disp_filter_vecs();
	}
}



int
main(int argc, char *argv[])
{
	int c;
	int argn;
	ff_vecs_t *ivecs = NULL;
	ff_vecs_t *ovecs = NULL;
	filter_vecs_t *fvecs = NULL;
	char *fname = NULL;
	char *ofname = NULL;
	char *ivec_opts = NULL;
	char *ovec_opts = NULL;
	char *fvec_opts = NULL;
	int opt_version = 0;
	int did_something = 0;
	const char *prog_name = argv[0]; /* argv is modified during processing */
	queue *wpt_head_bak, *rte_head_bak, *trk_head_bak;	/* #ifdef UTF8_SUPPORT */
	unsigned int wpt_ct_bak, rte_ct_bak, trk_ct_bak;	/* #ifdef UTF8_SUPPORT */

	global_opts.objective = wptdata;
	global_opts.masked_objective = NOTHINGMASK;	/* this makes the default mask behaviour slightly different */
	global_opts.charset = NULL;			/* #ifdef UTF8_SUPPORT */
	global_opts.charset_name = NULL;			/* #ifdef UTF8_SUPPORT */

#ifdef DEBUG_MEM
	debug_mem_open();
	debug_mem_output( "command line: " );
	for ( argn = 1; argn < argc; argn++ ) {
		debug_mem_output( "%s ", argv[argn] );
	}
	debug_mem_output( "\n" );
#endif
	
	cet_register();
	waypt_init();
	route_init();

	if ( argc < 2 ) {
		usage(argv[0],1);
		exit(0);
	}
	
	/*
	 * Open-code getopts since POSIX-impaired OSes don't have one.
	 */
	for (argn = 1; argn < argc; argn++) {
		char *optarg;

		if (argv[argn][0] != '-') {
			break;
		}
		if (argv[argn][1] == '-') {
			break;
		}
		
		if (argv[argn][1] == 'V' ) {
			 printf("\nGPSBabel Version %s\n\n", VERSION );
			 exit(0);
		}

		if (argv[argn][1] == '?' || argv[argn][1] == 'h') {
			usage(argv[0],0);
			exit(0);
		}

		c = argv[argn][1];

		if (argv[argn][2]) {
			opt_version = atoi(&argv[argn][2]);
		}

		switch (c) {
			case 'c':
				optarg = argv[argn][2] ? argv[argn]+2 : argv[++argn];
				cet_convert_init(optarg, 1);
				break;
			case 'i': 
				optarg = argv[argn][2]
					? argv[argn]+2 : argv[++argn];
				ivecs = find_vec(optarg, &ivec_opts);
				break;
			case 'o':
				optarg = argv[argn][2]
					? argv[argn]+2 : argv[++argn];
				ovecs = find_vec(optarg, &ovec_opts);
				if (ovecs == NULL) {
					fatal ("Output type '%s' not recognized\n", optarg);
				}
				break;
			case 'f':
				optarg = argv[argn][2]
					? argv[argn]+2 : argv[++argn];
				fname = optarg;
				if (ivecs == NULL) {
					fatal ("No valid input type specified\n");
				}
				if (ivecs->rd_init == NULL) {
					fatal ("Format does not support reading.\n");
				}
				/* simulates the default behaviour of waypoints */
				if (doing_nothing) global_opts.masked_objective |= WPTDATAMASK;
			
				cet_convert_init(ivecs->encode, ivecs->fixed_encode);	/* init by module vec */

				ivecs->rd_init(fname);
				ivecs->read();
				ivecs->rd_deinit();
				
				cet_convert_strings(global_opts.charset, NULL, NULL);
				cet_convert_deinit();
				
				did_something = 1;
				break;
			case 'F':
				optarg = argv[argn][2]
					? argv[argn]+2 : argv[++argn];
				ofname = optarg;
				if (ovecs) {
					/* simulates the default behaviour of waypoints */
					if (doing_nothing) 
						global_opts.masked_objective |= WPTDATAMASK;
					if (ovecs->wr_init == NULL) {
						fatal ("Format does not support writing.\n");
					}
					
					cet_convert_init(ovecs->encode, ovecs->fixed_encode);

					wpt_ct_bak = -1;
					rte_ct_bak = -1;
					trk_ct_bak = -1;

					if (global_opts.charset != &cet_cs_vec_utf8)
					{
					    waypt_backup(&wpt_ct_bak, &wpt_head_bak);
					    route_backup(&rte_ct_bak, &rte_head_bak);
					    track_backup(&trk_ct_bak, &trk_head_bak);
					    
					    cet_convert_strings(NULL, global_opts.charset, NULL);
					}

					ovecs->wr_init(ofname);
					ovecs->write();
					ovecs->wr_deinit();
					
					cet_convert_deinit();
					
					if (wpt_ct_bak != -1) waypt_restore(wpt_ct_bak, wpt_head_bak);
					if (rte_ct_bak != -1) route_restore(rte_ct_bak, rte_head_bak);
					if (trk_ct_bak != -1) track_restore(trk_ct_bak, trk_head_bak);
				}
				break;
			case 's':
				global_opts.synthesize_shortnames = 1;
				break;
			case 't':
				global_opts.objective = trkdata;
				global_opts.masked_objective |= TRKDATAMASK;
				break;
			case 'w':
				global_opts.objective = wptdata;
				global_opts.masked_objective |= WPTDATAMASK;
				break;
			case 'r':
				global_opts.objective = rtedata;
				global_opts.masked_objective |= RTEDATAMASK;
				break;
			case 'N':
				switch(argv[argn][2]) {
					case 'i':
						global_opts.no_smart_icons = 1;
						break;
					case 'n':
						global_opts.no_smart_names = 1;
						break;
					default:
						global_opts.no_smart_names = 1;
						global_opts.no_smart_icons = 1;
						break;
				}
				
				break;
 			case 'x':
				optarg = argv[argn][2]
					? argv[argn]+2 : argv[++argn];
 				fvecs = find_filter_vec(optarg, &fvec_opts);

 				if (fvecs) {
 					if (fvecs->f_init) fvecs->f_init(fvec_opts);
 					fvecs->f_process();
 					if (fvecs->f_deinit) fvecs->f_deinit();
					free_filter_vec(fvecs);
 				}  else {
					fatal("Unknown filter '%s'\n",optarg);
				}
 				break;
			case 'D':
				optarg = argv[argn][2]
					? argv[argn]+2 : argv[++argn];
				global_opts.debug_level = atoi(optarg);
				/*
				 * When debugging, announce version.
				 */
				if (global_opts.debug_level > 0)  {
					warning("GPSBabel Version: " VERSION "\n" );
				}

				break;
				/*
				 * Undocumented '-vs' option for GUI wrappers.
				 */
			case 'v':
				switch(argv[argn][2]) {
				case 's': global_opts.verbose_status = 1; break;
				case 'S': global_opts.verbose_status = 2; break;
				}
				break;

				/*
				 * DOS-derived systems will need to escape
				 * this as -^^.
				 */
			case '^':
				disp_formats(opt_version);
				exit(0);
 			case '%':
 				disp_filters(opt_version);
  				exit(0);
			case 'h':
			case '?':
				usage(argv[0],0);
				exit(0);
			case 'l':
				cet_disp_character_set_names(stdout);
				exit(0);

		}
	}

	/*
	 * Allow input and output files to be specified positionally
	 * as well.  This is the typical command line format.
	 */
	argc -= argn;
	argv += argn;
	if (argc > 2) {
		fatal ("Extra arguments on command line\n");
	}
	else if (argc && ivecs) {
		did_something = 1;
		/* simulates the default behaviour of waypoints */
		if (doing_nothing) global_opts.masked_objective |= WPTDATAMASK;
					
		cet_convert_init(ivecs->encode, 1);

		ivecs->rd_init(argv[0]);
		ivecs->read();
		ivecs->rd_deinit();
		
		cet_convert_strings(global_opts.charset, NULL, NULL);
		cet_convert_deinit();
		
		if (argc == 2 && ovecs) 
		{
			cet_convert_init(ovecs->encode, 1);
			cet_convert_strings(NULL, global_opts.charset, NULL);
			
			ovecs->wr_init(argv[1]);
			ovecs->write();
			ovecs->wr_deinit();
			
			cet_convert_deinit();
		}
	}
	else if (argc) {
		usage(prog_name,0);
		exit(0);
	}
	if (ovecs == NULL)
	{
		cet_convert_init(CET_CHARSET_ASCII, 1);
		cet_convert_strings(NULL, global_opts.charset, NULL);
		waypt_disp_all(waypt_disp);
	}

	if (!did_something)
		fatal ("Nothing to do!  Use '%s -h' for command-line options.\n", prog_name);

	cet_deregister();
	waypt_flush_all();
	route_flush_all();
	exit_vecs();
	exit_filter_vecs();

#ifdef DEBUG_MEM
	debug_mem_close();
#endif
	exit(0);
}