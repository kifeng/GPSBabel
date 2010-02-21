/*
    Perform various operations on waypoints.

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

#include <stdio.h>
#include "defs.h"

queue waypt_head;
static unsigned int waypt_ct;
static void *mkshort_handle;

void
waypt_init(void)
{
	mkshort_handle = mkshort_new_handle();
	QUEUE_INIT(&waypt_head);
}

waypoint *
waypt_dupe(waypoint *wpt) 
{
	waypoint * tmp;
	tmp = xcalloc(sizeof *wpt, 1);
	memcpy(tmp, wpt, sizeof(waypoint));

	if (wpt->shortname)
		tmp->shortname = xstrdup(wpt->shortname);
	if (wpt->description)
		tmp->description = xstrdup(wpt->description);
	if (wpt->notes)
		tmp->notes = xstrdup(wpt->notes);
	if (wpt->url)
		tmp->url = xstrdup(wpt->url);
	if (wpt->url_link_text)
		tmp->url_link_text = xstrdup(wpt->url_link_text);

	return tmp;
}
void
waypt_add(waypoint *wpt)
{
	ENQUEUE_TAIL(&waypt_head, &wpt->Q);

	/*
	 * Some input may not have one or more of these types so we
	 * try to be sure that we have these fields even if just by
	 * copying them from elsewhere.
	 */
	if (wpt->shortname == NULL) {
		if (wpt->description) {
			wpt->shortname = xstrdup(wpt->description);
		} else {
			if (wpt->notes) {
				wpt->shortname = xstrdup(wpt->notes);
			}
		}
	}
	if (wpt->description == NULL || strlen(wpt->description) == 0) {
		if (wpt->description)
			xfree(wpt->description);
		if (wpt->notes != NULL) {
			wpt->description = xstrdup(wpt->notes);
		} else  {
			if (wpt->shortname != NULL) {
				wpt->description = xstrdup(wpt->shortname);
			}
		}
	}
	waypt_ct++;
}

void
waypt_del(waypoint *wpt)
{
	dequeue(&wpt->Q);
	waypt_ct--;
}

unsigned int
waypt_count(void)
{
	return waypt_ct;
}

void
waypt_disp(const waypoint *wpt)
{
	if (wpt->creation_time) {
		printf("%s ", ctime(&wpt->creation_time));
	}
	printposn(&wpt->position.latitude,1);
	printposn(&wpt->position.longitude,0);
	printf("%s/%s", 
		global_opts.synthesize_shortnames ? 
			mkshort(mkshort_handle, wpt->description) : 
				wpt->shortname, 
				wpt->description);
	if (wpt->position.altitude.altitude_meters != unknown_alt)
		printf(" %f", wpt->position.altitude.altitude_meters);
	printf("\n");
}

void
waypt_disp_all(waypt_cb cb)
{
	queue *elem, *tmp;
	waypoint *waypointp;

	QUEUE_FOR_EACH(&waypt_head, elem, tmp) {
		waypointp = (waypoint *) elem;
		(*cb) (waypointp);
	}
}

waypoint *
find_waypt_by_name(const char *name)
{
	queue *elem, *tmp;
	waypoint *waypointp;

	QUEUE_FOR_EACH(&waypt_head, elem, tmp) {
		waypointp = (waypoint *) elem;
		if (0 == strcmp(waypointp->shortname, name)) {
			return waypointp;
		}
	}

	return NULL;
}

void 
waypt_free( waypoint *wpt )
{
	if (wpt->shortname) {
		xfree(wpt->shortname);
	}
	if (wpt->description) {
		xfree(wpt->description);
	}
	if (wpt->notes) {
		xfree(wpt->notes);
	}
	if (wpt->url) {
		xfree(wpt->url);
	}
	if (wpt->url_link_text) {
		xfree(wpt->url_link_text);
	}
	if (wpt->icon_descr && wpt->icon_descr_is_dynamic) {
		xfree((char *)(void *)wpt->icon_descr);
	}
	if (wpt->gpx_extras) {
		free_gpx_extras(wpt->gpx_extras);
	}
	xfree(wpt);	
}

void 
waypt_flush( queue *head )
{
	queue *elem, *tmp;
	waypoint *last = NULL;
			
	QUEUE_FOR_EACH(head, elem, tmp) {
		if ( last ) {
			waypt_free(last);
		}
		last = (waypoint *)elem;
        }
	
	if ( last ) {
		waypt_free(last);
	}
	
	QUEUE_INIT(head);
}
void
waypt_flush_all()
{
	if ( mkshort_handle ) {
		mkshort_del_handle( mkshort_handle );
	}
	waypt_flush(&waypt_head);
}