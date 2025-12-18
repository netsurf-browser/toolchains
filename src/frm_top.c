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
 * $Source: /cvsroot/windom/windom/src/frm_top.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include "globals.h"

/** standard callback function to manage window-topped event on form
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to forms for
 *  the \b WM_TOPPED event, registered by  mt_FormCreate()
 */
 
void __CDECL frm_tpd( WINDOW *win, short buff[8], APPvar *app) {
	W_FORM *dial = mt_DataSearch( app, win, WD_WFRM);

	mt_WindSet( app, win, WF_TOP, win->handle, 0, 0, 0);
	app->wglb.front = win;  /* TODO: this line shows a bad management of app->wglb.front 
	                         * this variable doesn't have to be modified by user callback functions
							 * but shall be internally computed by windom in WindSet(WF_TOP) */
	mt_ObjcEdit( app, OC_FORM, win, dial->edit, 0, &dial->cursor, ED_INIT);
}
