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
 * $Source: /cvsroot/windom/windom/src/do_update.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.14 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/*  update the window content (internal function)
 */

void _do_update( APPvar *app, WINDOW *win, short buff[8]) {
	GRECT rdraw,  /* zone to redraw */
	      rlist,  /* current rectangle in AES list */
	      rwork,  /* work aera of window */
	      clip = win->graf->clip;
	INT16 x, y, w, h;

	msg2grect( buff, &rdraw);

	/* check the graf clipping rectangle */
	if ( !rc_intersect( &clip, &rdraw) ) return;

	/* Dominic Test : execute only one function when one
	 * receives a WM_REDRAW message */
	buff[0] = WM_PREREDRAW;
	mt_EvntExec( app, win, buff);
	
	while( !mt_wind_update(BEG_UPDATE, app->aes_global));
	mt_graf_mouse( M_OFF, 0L, app->aes_global);
	
	buff[0] = ( win->status & WS_ICONIFY ) ? WM_ICONDRAW : WM_REDRAW;
	
	mt_WindGet( app, win, WF_WORKXYWH, &rwork.g_x, &rwork.g_y, &rwork.g_w, &rwork.g_h);
	mt_wind_get( win->handle, WF_WORKXYWH, &x, &y, &w, &h, app->aes_global);
	if (app->aes4 & AES4_FIRSTAREAXYWH)
		mt_wind_xget_grect( win->handle, WF_FIRSTAREAXYWH, &rdraw, &rlist, app->aes_global);
	else
		mt_wind_get_grect( win->handle, WF_FIRSTXYWH, &rlist, app->aes_global);
	while (rlist.g_w && rlist.g_h) {
		if ( rc_intersect( &rdraw, &rlist) ) {
			int dy=0;

			mt_ClipOn( app, win->graf, &rlist);
			if( win -> status & WS_MENU
			   && !( SYSMENU(app) && win->attrib & MENUBAR)
				 && !(win->status & WS_ICONIFY)) {
				wind_menu_draw( app, win, &rlist);
				dy = win->menu.root[1].ob_height+1;
			}
			if( win -> status & WS_TOOLBAR && !(win->status & WS_ICONIFY)) {				
				win->tool.root->ob_x = x;
				win->tool.root->ob_y = y + dy;
				mt_objc_draw_grect( win->tool.root, 0, MAX_DEPTH, &rlist, app->aes_global);
			}
			if( win->status & WS_ICONIFY ) {
				grect2msg(&rlist,buff);
				mt_EvntExec( app, win, buff);
			} else if( rc_intersect( &rwork, &rlist)) {
				mt_ClipOn( app, win->graf, &rlist);
				grect2msg(&rlist,buff);
				mt_EvntExec( app, win, buff);
			}
		}
		mt_wind_get_grect( win->handle, WF_NEXTXYWH, &rlist, app->aes_global);
	}
	mt_ClipOn( app, win->graf, &clip);
	mt_graf_mouse( M_ON, 0L, app->aes_global);
	mt_wind_update(END_UPDATE, app->aes_global);
}

