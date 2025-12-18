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
 * $Source: /cvsroot/windom/windom/src/wind_attach.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/02/16 03:26:09 $
 *   $Revision: 1.7 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/**
 * @brief insert an alien window in the WinDom list of windows.
 *
 * @param app application descriptor,
 * @param handle handle of the window to insert,
 * @return the window descriptor associated to this window.
 *
 * mt_WindAttach() allows you to integrate alien windows in the WinDom
 * environnement. The alien window should be created and opened before
 * call of WindAttach(). This function is mainly used to insert window
 * created by an another application inside your application. For
 * example, FselInput() uses it to integrate the MagiC file selector 
 * as a window in the application.
 *
 * The window descriptor returned by mt_WindAttach() set the bit
 * \b WS_FOREIGN to 1 in \e win->status field. To remove this
 * window from WinDom environmment, mt_WindClose() then mt_WindDelete()
 * should be called.
 *
 * @todo bind the standard std_cls() and std_dst() events to \b FOREIGN windows
 */

WINDOW *mt_WindAttach( APPvar *app, int handle) {
	WINDOW *win;
	
	/* allocate memory */
	win = (WINDOW *)calloc( 1, sizeof(WINDOW));
	if( win == NULL) return NULL;
	
	/* fill the WINDOW structure */
	win -> next   = NULL;
	win -> handle = handle;
	win -> status = (WS_OPEN|WS_FOREIGN);

	listInit( &win->binding );
	
	win -> xpos 	= win -> ypos = 0;
	win -> xpos_max = win -> ypos_max = 1;
	win -> w_u = win -> h_u = 1;
	win -> menu . root = NULL;
	win -> menu . bind = NULL;
	win -> menu . hilight = NULL;
	win -> tool . root = NULL;
	win -> tool . save = NULL;
	win -> tool . bind = NULL;
	win -> icon . name = NULL;
	listInit( &win->data );
	win->type = 0;
	win->name = win->info = NULL;

	/* add this WINDOW to the list win->next->next... */
	mt_AddWindow( app, win);
	
	/* set a VDI virtual workstation for this window */
	win->graf = &(app->graf);

	/* add this WINDOW to the list of opened window. By default, it's the top window */
	wlst_add_top( app, win);
	
	/* update wglb.* variables, and verify that "win" is actually the AES top window */
	wlst_recompute_wglb(app);

	return win;
}

