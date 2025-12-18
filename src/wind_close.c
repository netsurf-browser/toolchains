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
 * $Source: /cvsroot/windom/windom/src/wind_close.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.6 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"
#include "av.h"

/**
 * @brief Close a window opened by mt_WindOpen().
 *
 * @param app application descriptor,
 * @param win window descriptor,
 * @return 0 or a negative code error.
 *
 * mt_WindClose() closes a window on the screen and replaces the
 * AES wind_close function. As mt_WindOpen() a graphic effect is
 * used if the window status (parameter \e win->status) \b WS_GROW
 * bit is set to 1.
 *
 * For similar raisons explained in the mt_WindOpen() manual, the
 * wind_close() should never be used except in some special case.
 * It is important to remove the window from the internal
 * WinDom opened windows list with the remove_windowlist() function.
 *
 * @sa mt_WindOpen().
 */

int mt_WindClose( APPvar *app, WINDOW *win) {
	int res=0;

/*
	if( !(win->status & WS_OPEN))
		return -1;	*/ /* warning : window already closed */
	
	/* not a real AES window : we just have to remove the WS_OPEN flag */
	if( win -> status & WS_CUSTOM) {
		win -> status &= ~WS_OPEN;
		return 0;
	}
	
	/* close the AES window */	
	if( (win -> status & WS_OPEN) && !(win ->status & WS_FOREIGN)) {
		res = mt_wind_close( win -> handle, app->aes_global);
		/* special effects always optional */
		if( win -> status & WS_GROW && CONF(app)->weffect == TRUE) {
			INT16 x,y,w,h;
			mt_wind_get( win->handle, WF_CURRXYWH, &x, &y, &w, &h, app->aes_global);
			mt_graf_shrinkbox( 0, 0, 0, 0, x, y, w, h, app->aes_global);
		}
	}
	win -> status &= ~WS_OPEN;
	
	/* update internal __windowlist */
	wlst_remove(app,win);
	
	/* update wglb.* */
	wlst_recompute_wglb(app);
	
	/* inform the AV server that the window has been closed */
	if( app->avid != -1 && !(win ->status & WS_FOREIGN)) 
		mt_ApplWrite( app, app->avid, AV_ACCWINDCLOSED, win->handle,0,0,0,0);

	return res;
}
