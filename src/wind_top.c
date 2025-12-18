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
 * $Source: /cvsroot/windom/windom/src/wind_top.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.5 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"


/**
 * @brief Put a window in foreground.
 *
 * @param app application descriptor,
 * @param win window descriptor
 *
 * mt_WindTop() puts in foreground (tops) a window. If needed :
 *  - an iconified window will be uniconified,
 *  - a closed window will be opened (with its previous geometry and position).
 * 
 */

void mt_WindTop( APPvar *app, WINDOW *win) {
	INT16 x, y, w, h;
	
	if( win -> status & WS_ICONIFY) {
		mt_WindGet( app, win, WF_UNICONIFY, &x, &y, &w, &h);
		mt_ApplWrite( app, mt_AppId(app), WM_UNICONIFY, win->handle, x, y, w, h);
	} else if( win -> status & WS_OPEN) 
		/* window opened */
		mt_wind_set( win -> handle, WF_TOP, 0, 0, 0, 0, app->aes_global);
	else {
		mt_wind_get( win->handle, WF_PREVXYWH, &x, &y, &w, &h, app->aes_global);
		mt_WindOpen( app, win, x, y, w, h);
	}
}

