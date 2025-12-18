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
 * $Source: /cvsroot/windom/windom/src/frm_mvd.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include "globals.h"

/** standard callback function to manage window-moved event on form
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to forms for
 *  the \b WM_MOVED event, registered by  mt_FormCreate()
 */
 
void __CDECL frm_mvd( WINDOW *win, short buff[8], APPvar *app) {
	INT16 x, y, w, h;
	int outlined = 0;
	W_FORM *dial = mt_DataSearch( app, win, WD_WFRM);
	 	
 	x = buff[4];
	y = buff[5];
	w = buff[6];
	h = buff[7];
	mt_wind_set( buff[3], WF_CURRXYWH, x, y, w, h, app->aes_global);
	win->status &= ~WS_FULLSIZE;
	mt_WindGet( app, win, WF_WORKXYWH, &x, &y, &w, &h);
	if( dial->root->ob_state & OUTLINED )
		outlined = OUTLINED_WIDTH;
	dial->root[ROOT].ob_x = x + outlined - (int)win->xpos*win->w_u;
	dial->root[ROOT].ob_y = y + outlined - (int)win->ypos*win->h_u;
}

