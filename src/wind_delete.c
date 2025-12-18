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
 * $Source: /cvsroot/windom/windom/src/wind_delete.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.12 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

int __CDECL wind_aes_remove( APPvar *app, WINDOW *win) {
	int res=0;

	/* protection against wind_delete: handle could be set to -1 */
	if( win->handle != -1 && !(win->status & WS_FOREIGN))
		res = mt_wind_delete( win -> handle, app->aes_global);
	mt_RemoveWindow( app, win);

	return res;
}


/**
 * @brief Delete a window descriptor created by mt_WindCreate().
 *
 * @param app application descriptor,
 * @param win window descriptor,
 * @return 0 or a negative code error.
 *
 * mt_WindDelete() replaces mt_wind_delete(). The window descriptor is
 * removed from the list of windows then deleted.
 *
 * \sa mt_WindCreate().
 */

int mt_WindDelete( APPvar *app, WINDOW *win) {
	WIND_HANDLERS *whandlers = NULL;
	int res=0;
	
	if( win->status & WS_OPEN)
		res = mt_WindClose( app, win);
	
	if (win->status & WS_CUSTOM)
		whandlers = mt_DataSearch( app, win, WD_WHLR );
	if (whandlers) {
		if (whandlers->remove)
			res = whandlers->remove( app, win);
	} else {
		res = wind_aes_remove( app, win);
	} 
	
	if( win -> status & WS_TOOLBAR) {
		mt_RsrcUserFree( app, win -> tool.root);
		mt_ObjcFree( app, win -> tool.root);
		if( win -> tool.save) free( win -> tool.save);
		if( win -> tool.bind) free( win -> tool.bind);
	}
	if( win -> status & WS_MENU)
		mt_ObjcFree( app, win -> menu.root);
	mt_EvntClear( app, win);
	
	if (win->graf != &(app->graf)) {
		v_clsvwk( win->graf->handle);
		free(win->graf);
	}
	
	/* if the win structure is in use (for example because this function is called by a callback
	 * function which pointer is stored in the list of event, stored in the win structure), then
	 * win shall not be free because the list of callback function should remain there.
	 * see mt_EvntExec() and evnt_cleanup() for details
	 */
	if (win->status & WS_WIN_IN_USE)
		win->status |= WS_DESTROYED;
	else {
		mt_DataClear( app, win);
		free( win);
	}
	
	return res;
}

