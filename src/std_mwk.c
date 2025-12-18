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
 * $Source: /cvsroot/windom/windom/src/std_mwk.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.5 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/*  "standard" MoveWorK function
 *  this function is used to move the work area of a window
 *  according t the mouse cursor movement while a mouse button
 *  is pressed.
 */

void __CDECL std_mwk( APPvar *app, WINDOW *win) {
	INT16 k, dum;
	INT16 x0,y0,x,y;
	INT16 w,h;
	int dx, dy;
	long xpos_bornesup;
	long ypos_bornesup;

	mt_WindGet( app, win, WF_WORKXYWH, &dum, &dum, &w, &h);
	xpos_bornesup = win -> xpos_max - (long)w / (long)win -> w_u;
	if (xpos_bornesup < 0) xpos_bornesup = 0;
	ypos_bornesup = win -> ypos_max - (long)h / (long)win -> h_u;
	if (ypos_bornesup < 0) ypos_bornesup = 0;
	
	if( app->aes4 & AES4_XGMOUSE)
		mt_graf_mouse( M_SAVE, 0L, app->aes_global);
	mt_graf_mouse( FLAT_HAND,NULL, app->aes_global);
	mt_wind_update( BEG_MCTRL, app->aes_global);
	
	mt_graf_mkstate(&x0,&y0,&k,&dum, app->aes_global);
	x = x0;
	y = y0;
	
	while (k) {
		/* compute "dx" deplacement */
		dx = (x0 - x) / win->w_u; /* unit = like "pos" */
		if (dx < 0) {  /* direction = right ==> slider to left */
			if ( - dx > win->xpos) dx = - (int) win->xpos;
		} else {       /* direction = left ==> slider to right */
			if ( win->xpos + dx > xpos_bornesup) dx = (int)(xpos_bornesup - win->xpos);
		}
		win->xpos += dx;
		dx *= win->w_u; /* unit = pixel */
		x0 -= dx;

		/* compute "dy" deplacement */
		dy = (y0 - y) / win->h_u; /* unit = like "pos" */
		if (dy < 0) {  /* direction = down ==> slider to up */
			if ( - dy > win->ypos) dy = - (int) win->ypos;
		} else {       /* direction = up ==> slider to down */
			if ( win->ypos + dy > ypos_bornesup) dy = (int)(ypos_bornesup - win->ypos);
		}
		win->ypos += dy;
		dy *= win->h_u; /* unit = pixel */
		y0 -= dy;
		
		/* perform the move */
		if (dx || dy) move_work(app, win,dx,dy);
	
		mt_graf_mkstate(&x,&y,&k,&dum,app->aes_global);
	}

	mt_wind_update(END_MCTRL,app->aes_global);
	if( !(app->aes4 & AES4_XGMOUSE) || mt_graf_mouse( M_RESTORE, 0L, app->aes_global) == 0 ) {
		/* cannot restore the mouse ? set mouse to default value (arrow) */
		mt_graf_mouse(ARROW,NULL,app->aes_global);
	}
}
