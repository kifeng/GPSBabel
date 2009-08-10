/*
    Distance Between Points Filter(s)

    Copyright (C) 2002 Robert Lipe, robertlipe@usa.net

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
#include "grtcirc.h"

#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

extern queue waypt_head;
static route_head *cur_rte = NULL;

static double pos_dist;
static char *distopt;
static char *purge_duplicates = NULL;
static char *latopt;
static char *lonopt;
static char *exclopt;
static char *nosort;

waypoint * home_pos;

typedef struct {
	double distance;
} extra_data;

static
arglist_t position_args[] = {
	{"distance", &distopt, "Maximum positional distance",
		ARGTYPE_FLOAT | ARGTYPE_REQUIRED },
	{"all", &purge_duplicates, "Suppress all points close to other points", ARGTYPE_BOOL }, 
	{0, 0, 0, 0}
};

static
arglist_t radius_args[] = {
	{"lat", &latopt,       "Latitude for center point (D.DDDDD)",
		ARGTYPE_FLOAT | ARGTYPE_REQUIRED },
	{"lon", &lonopt,       "Longitude for center point (D.DDDDD)",
		ARGTYPE_FLOAT | ARGTYPE_REQUIRED },
	{"distance", &distopt, "Maximum distance from center",
		ARGTYPE_FLOAT | ARGTYPE_REQUIRED },
	{"exclude", &exclopt,  "Exclude points close to center",
		ARGTYPE_BOOL },
	{"nosort", &nosort,    "Inhibit sort by distance to center.",
		ARGTYPE_BOOL },
	{0, 0, 0, 0}
};

static double
gc_distance(double lat1, double lon1, double lat2, double lon2)
{
	return gcdist( 
	    (lat1 * M_PI) / 180.0,
	    (lon1 * M_PI) / 180.0,
	    (lat2 * M_PI) / 180.0,
	    (lon2 * M_PI) / 180.0
	    );
}

static int
position_comp(const void * a, const void * b)
{
	const waypoint *x1 = *(waypoint **)a;
	const waypoint *x2 = *(waypoint **)b;
	double latdiff, londiff, max;

	/*
	 * this compare makes the assumption that things will fall into
	 * order by declaring their biggest single axial difference.
	 * It is much less math than distance and bearing from some random
	 * point.
	 */

	londiff = (x1->longitude -
		   x2->longitude) * 1000000.0;
	latdiff = (x1->latitude -
		   x2->latitude) * 1000000.0;

	max = fabs(londiff) >= fabs(latdiff) ? floor(londiff) : floor(latdiff);

	if (max < 0)
		return (-1);
	else if (max > 0)
		return (1);

	return(0);
}

static int
dist_comp(const void * a, const void * b)
{
	const waypoint *x1 = *(waypoint **)a;
	const waypoint *x2 = *(waypoint **)b;
	extra_data *x1e = (extra_data *) x1->extra_data;
	extra_data *x2e = (extra_data *) x2->extra_data;

	if (x1e->distance > x2e->distance)
		return 1;
	if (x1e->distance < x2e->distance)
		return -1;
	return 0;

}

/* tear through a waypoint queue, processing points by distance */
static void 
position_runqueue(queue *q, int nelems, int qtype)
{
	queue * elem, * tmp;
	waypoint ** comp;
	double dist;
	int i, j;
	int del = 0;

	comp = (waypoint **) xcalloc(nelems, sizeof(*comp));

	i = 0;

	QUEUE_FOR_EACH(q, elem, tmp) {
		comp[i] = (waypoint *)elem;
		i++;
	}

	if (qtype == wptdata)
		qsort(comp, nelems, sizeof(waypoint *), position_comp);

	for (i = 1, j = 0 ; i < nelems ; i++) {
		dist = gc_distance(comp[j]->latitude,
				   comp[j]->longitude,
				   comp[i]->latitude,
				   comp[i]->longitude);

		/* convert radians to integer feet */
		dist = (int)(5280*tomiles(dist));
		
		if (dist <= pos_dist) {
			switch (qtype) {
				case wptdata:
					waypt_del(comp[i]);
					waypt_free(comp[i]);
					del = !!purge_duplicates;
					break;
				case trkdata:
				case rtedata:
					route_del_wpt(cur_rte, comp[i]);
					del = !!purge_duplicates;
					break;
				default:
					break;
			}
		}
		else if (del ) {
			switch (qtype) {
				case wptdata:
					waypt_del(comp[i]);
					waypt_free(comp[i]);
					del = 0;
					break;
				case trkdata:
				case rtedata:
					route_del_wpt(cur_rte, comp[i]);
					del = 0;
					break;
				default:
					break;
			}
		} else {
			j = i; /* advance last use point */
		}
	}
	if ( del ) {
		switch (qtype) {
			case wptdata:
				waypt_del(comp[nelems-1]);
				waypt_free(comp[nelems-1]);
				break;
			case trkdata:
			case rtedata:
				route_del_wpt(cur_rte, comp[i]);
				break;
			default:
				break;
		}
	}

	if (comp)
		xfree(comp);
}

static void
position_process_route(const route_head * rh) { 
    int i = rh->rte_waypt_ct;

    if (i) {
    	cur_rte = (route_head *)rh;
        position_runqueue((queue *)&rh->waypoint_list, i, rtedata);
        cur_rte = NULL;
    }

}

static void 
position_noop(){
}

void position_process() 
{
	int i = waypt_count();
	
	if (i)
		position_runqueue(&waypt_head, i, wptdata);
	
	route_disp_all(position_process_route, position_noop, position_noop);
	track_disp_all(position_process_route, position_noop, position_noop);
}

void
position_init(const char *args) {
	char *fm;

	pos_dist = 0;

	if (distopt) {
		pos_dist = strtod(distopt, &fm);

		if ((*fm == 'm') || (*fm == 'M')) {
			 /* distance is meters */
			pos_dist *= 3.2802;
		}
	}
}

void
position_deinit(void) {
}

void 
radius_process(void)
{
	queue * elem, * tmp;
	waypoint * waypointp;
	double dist;
	waypoint ** comp;
	int i, wc;
	queue temp_head;

	QUEUE_FOR_EACH(&waypt_head, elem, tmp) {
		extra_data *ed;

		waypointp = (waypoint *)elem;
		dist = gc_distance(waypointp->latitude,
				   waypointp->longitude,
				   home_pos->latitude,
				   home_pos->longitude);

		/* convert radians to float point statute miles */
		dist = tomiles(dist);

		if ((dist >= pos_dist) == (exclopt == NULL)) {
			waypt_del(waypointp);
			waypt_free(waypointp);
			continue;
		}

		ed = (extra_data *) xcalloc(1, sizeof(*ed));
		ed->distance = dist;
		waypointp->extra_data = ed;
	}

	wc = waypt_count();
	QUEUE_INIT(&temp_head);

	comp = (waypoint **) xcalloc(wc, sizeof(*comp));

	i = 0;

	/*
	 * Create an array of remaining waypoints, popping them off the
	 * master queue as we go.   This gives us something reasonable 
	 * for qsort.
	 */

	QUEUE_FOR_EACH(&waypt_head, elem, tmp) {
		waypoint *wp = (waypoint *) elem;
		comp[i] = wp;
		waypt_del(wp);
		i++;
	}

	if (!nosort) {
		qsort(comp, wc, sizeof(waypoint *), dist_comp);
	}

	/*
	 * The comp array is now sorted by distance.   As we run through it,
	 * we push them back onto the master wp list, letting us pass them
	 * on through in the modified order.
 	 */
	for (i = 0; i < wc; i++) {
		waypoint * wp = comp[i];
		waypt_add(wp);
		xfree(wp->extra_data);
	}

	xfree(comp);
}

void
radius_init(const char *args) {
	char *fm;

	pos_dist = 0;

	if (distopt) {
		pos_dist = strtod(distopt, &fm);

		if ((*fm == 'k') || (*fm == 'K')) {
			 /* distance is kilometers, convert to feet */
			pos_dist *= .6214;
		}
	}

	home_pos = (waypoint *) xcalloc(sizeof(*home_pos), 1);

	if (latopt)
		home_pos->latitude = atof(latopt);
	if (lonopt)
		home_pos->longitude = atof(lonopt);
}

void
radius_deinit(void) {
	if (home_pos)
		xfree(home_pos);
}

filter_vecs_t position_vecs = {
	position_init,
	position_process,
	position_deinit,
	position_args
};

filter_vecs_t radius_vecs = {
	radius_init,
	radius_process,
	radius_deinit,
	radius_args
};