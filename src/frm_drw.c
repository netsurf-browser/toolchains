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
 * $Source: /cvsroot/windom/windom/src/frm_drw.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include "globals.h"

/** standard callback function to manage redraw event on form
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to forms for
 *  the \b WM_REDRAW event, registered by  mt_FormCreate()
 */
 
void __CDECL frm_drw( WINDOW *win, short buff[8], APPvar *app) {
	INT16 x, y, w, h;
	int outlined = 0;
	W_FORM	*dial = mt_DataSearch( app, win, WD_WFRM);

	mt_WindGet( app, win, WF_WORKXYWH, &x, &y, &w, &h);
	if( dial->root->ob_state & OUTLINED)
		outlined = OUTLINED_WIDTH;
	dial->root[ROOT].ob_x = x - (int)win->xpos * win->w_u + outlined;
	dial->root[ROOT].ob_y = y - (int)win->ypos * win->h_u + outlined;
	mt_objc_draw_grect( dial->root, ROOT, MAX_DEPTH, &win->graf->clip, app->aes_global);
	/* case of EDITABLE fields */
	if ( dial->edit != -1) {
		mt_wind_get( win -> handle, WF_TOP, &x, &y, &w, &h, app->aes_global);
		if ( x == win -> handle)
			mt_ObjcEdit( app, OC_FORM, win, dial->edit, 0, &dial->cursor, ED_INIT);
	}
}
