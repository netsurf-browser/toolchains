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
 * $Source: /cvsroot/windom/windom/src/move_work.c,v $
 * Module : standard event window callback
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/16 22:29:33 $
 *   $Revision: 1.11 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

void move_screen( APPvar *app, int vhandle, GRECT *screen, int dx, int dy) {
	INT16 xy[ 8];
	long dum = 0L;
	GRECT g;

	/* get intersection with screen area */
	mt_wind_get_grect( 0, WF_CURRXYWH, &g, app->aes_global);
	rc_intersect( &g, screen);
	xy[ 0] = screen -> g_x;
	xy[ 1] = screen -> g_y;
	xy[ 2] = xy[ 0] + screen -> g_w - 1;
	xy[ 3] = xy[ 1] + screen -> g_h - 1;
	xy[ 4] = xy[ 0] + dx;
	xy[ 5] = xy[ 1] + dy;
	xy[ 6] = xy[ 2] + dx;
	xy[ 7] = xy[ 3] + dy;
	vro_cpyfm( vhandle, S_ONLY, xy, (MFDB *)&dum, (MFDB *)&dum);
}

void move_work_rect( APPvar *app, WINDOW *win, GRECT *rect, int dx, int dy) {
	GRECT r1, r2;
	short buff[8] = {WM_REDRAW};

	while( !mt_wind_update(BEG_UPDATE, app->aes_global));
	mt_graf_mouse( M_OFF, 0L, app->aes_global);

	mt_WindXGet( app, win, WF_FIRSTAREAXYWH, rect, &r1.g_x, &r1.g_y, &r1.g_w, &r1.g_h);
	while (r1.g_w && r1.g_h) {
		if( !rc_intersect( rect, &r1) ) {
			mt_WindGet( app, win, WF_NEXTXYWH, &r1.g_x, &r1.g_y, &r1.g_w, &r1.g_h);
			continue;
		}

		if ( (abs(dx) < r1.g_w) && (abs(dy) < r1.g_h) )
		{
			GRECT c = win->graf->clip;
			mt_ClipOn( app, win->graf, &r1);

			r2 = r1; /* save */
			if (dy > 0) {
				r1.g_y += dy;
				r1.g_h -= dy;
			} else {
				r1.g_h += dy;
			}
			if (dx > 0) {
				r1.g_x += dx;
				r1.g_w -= dx;
			} else {
				r1.g_w += dx;
			}
			move_screen( app, win->graf->handle, &r1, - dx, - dy);
			if (dx)
			{
				r1 = r2 ;  /* restore */
				if (dx > 0) {
					r1.g_x += r1.g_w - dx;
					r1.g_w = dx;
				} else {
					r1.g_w = - dx;
				}
				if (dy > 0) {
					r1.g_h -= dy;
				} else {
					r1.g_y -= dy;
					r1.g_h += dy;
				}
				mt_ClipOn( app, win->graf, &r1);
				grect2msg( &r1, buff);
				mt_EvntExec( app, win, buff);
			}
			if (dy)
			{
				r1 = r2 ;  /* restore */
				if (dy > 0) {
					r1.g_y += r1.g_h - dy;
					r1.g_h = dy;
				} else {
					r1.g_h = - dy;
				}
				mt_ClipOn( app, win->graf, &r1);
				grect2msg( &r1, buff);
				mt_EvntExec( app, win, buff);
			}
			mt_ClipOn( app, win->graf, &c);
		}
		else
		{
			GRECT c = win->graf->clip;
			mt_ClipOn( app, win->graf, &r1);
			grect2msg( &r1, buff);
			mt_EvntExec( app, win, buff);
			mt_ClipOn( app, win->graf, &c);
		}

		mt_WindGet( app, win, WF_NEXTXYWH, &r1.g_x, &r1.g_y, &r1.g_w, &r1.g_h);
	}

	mt_graf_mouse( M_ON, 0L, app->aes_global);
	mt_wind_update(END_UPDATE, app->aes_global);
}

void move_work( APPvar *app, WINDOW *win, int dx, int dy) {
	short buff[8];
	buff[0] = WM_SLIDEXY;
	buff[4] = dx;
	buff[5] = dy;
	mt_EvntExec( app, win, buff);
}
