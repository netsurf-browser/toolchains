/*
 * WinDom: a high level GEM library
 * Copyright (c) 1997-2006 windom authors (see AUTHORS file)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * $Source: /cvsroot/windom/windom/examples/mousexy/mousexy.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:39:25 $
 *   $Revision: 1.4 $
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __PUREC__
#include <tos.h>
#else
#include <osbind.h>
#endif

#include "windom.h"

#define MOUSEPOS 0x1
#define CLOCK	 0x2

typedef struct {
	unsigned year:	7;
	unsigned month:	4;
	unsigned day:	5;
	unsigned hour:	5;
	unsigned minute:6;
	unsigned second:5;
} BIOS_TIME;

char info[200];
unsigned int conf = MOUSEPOS;

static void __CDECL timer(WINDOW *win, short buff[8]) {
	static int x=0;
	static int y=0;
	static char *prefix="-/|\\";
	static short ind_prefix=0;

	if ( 1 /* force the update every xtimer evnt */ || evnt.my != y || evnt.mx != x) {
		/* recompute the content of the info bar */
		info[0] = prefix[ind_prefix++];
		if (prefix[ind_prefix] == 0) ind_prefix=0;
		strcpy( &info[1], "  ");
		if( conf & MOUSEPOS)
			sprintf( info,   "%s(%.3d,%.3d)    ", info, evnt.mx, evnt.my);
		if( conf & CLOCK) {
			long dum;
			BIOS_TIME time;
			
			dum = Gettime();
			*(long*)&time = dum;
			sprintf( info, "%s%.2d:%.2d", info, time.hour, time.minute);
		}
		WindSetStr( win, WF_INFO, info);
		x = evnt.mx;
		y = evnt.my;
	}
}

static void __CDECL ApTerm( WINDOW *win, short buff[8]) {
	ApplExit();
	exit( 0);
}

int main(void) {
	WINDOW *win;
	int refresh=100,dum;
	int x=100,y=100;
	short xd, yd;
	short attrib = MOVER|NAME|INFO|CLOSER|SMALLER;
	
	ApplInit();
	
	/* La config */
	ConfInquire( "mousexy.timer", "%d", &refresh);
	if( ConfInquire( "mousexy.display.clock", "%b", &dum) == 1)
		SET_BIT( conf, CLOCK, dum);
	if( ConfInquire( "mousexy.display.mouse", "%b", &dum) == 1)
		SET_BIT( conf, MOUSEPOS, dum);
	ConfInquire( "mousexy.window", "%d,%d", &x, &y);
	wind_get( 0, WF_WORKXYWH, &xd, &yd, NULL ,NULL); /* NULL allowed with gemlib43 */
	x = MAX(x,xd);
	y = MAX(y,yd);

	/* La fenetre */
	win = WindCreate( attrib, app.x, app.y, app.w, app.h);
	EvntAttach( win, WM_XTIMER, timer);
	EvntAttach( NULL, AP_TERM, ApTerm);
	WindOpen(win, x, y, 120*((conf & MOUSEPOS)?1:0)+90*((conf & CLOCK)?1:0), 0);
	WindSetStr( win, WF_NAME, "MouseXY 2.00 by D.B‚r‚ziat");
	evnt.timer = refresh;

	/* les evenements */
	while( 1)
		EvntWindom( MU_MESAG|MU_TIMER);

	return 0;
}
