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
 * $Source: /cvsroot/windom/windom/src/wind_open.c,v $
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
 * @brief Open a window at screen.
 * 
 * @param app Application descriptor,
 * @param win window to open,
 * @param x,y,w,h position and size of the window on screen. 
 *     - If x=-1, the window will be horizontally centered using GrectCenter().
 *     - If x=-1, the window will be vertically centered using GrectCenter().
 * @return 0 or a negative code error.
 *
 * mt_WindOpen() opens a window at screen previously created by mt_WindCreate().
 * In WinDom environnement, the open_wind() should never be
 * used (except some very special case).
 *
 * An opened window has the \b WS_OPEN(!b) bit set to 1 in \e win->status.
 * If the status \b WS_GROW(!b) bit is set to 1, a graphic
 * effect should be used (via graf_growbox() function).
 *
 * A window can have minimum and maximal size : just sets the following
 * variables : 
 * \e win->min_w, \e win->min_h, \e win->max_w, \e win->max_h(!i).
 *
 * @deprecated If a window has to be opened with open_wind() (for a special
 * raison), the window should be registered in the internal WinDow
 * list of opened windows with the add_windowlist() function. It is
 * important for WinDom to have a correct list because the 
 *\b WM_BOTTOMED message is simuled with old TOS version.
 *
 * @sa GrectCenter()
 */
int mt_WindOpen( APPvar *app, WINDOW *win, int x, int y , int w, int h) {
	int res = 0;
	INT16 dum;
	INT16 _x;
	INT16 _y;
	
	/* update internal __windowlist */
	wlst_add_top( app, win);
	
	/* inform the AV server that this window has been opened */
	if( app->avid != -1) mt_ApplWrite( app, app->avid, AV_ACCWINDOPEN, win->handle,0,0,0,0);
	
	/* modify position/dimensions is needed */
	if( x == -1) {
		mt_GrectCenter( app, w, h, &_x, &dum); x = _x;
	}
	if( y == -1) {
		mt_GrectCenter( app, w, h, &dum, &_y); y = _y;
	}
	w = MAX( w, win->w_min);
	w = MIN( w, win->w_max);
	h = MAX( h, win->h_min);
	h = MIN( h, win->h_max);
	
	/* special effects are always optional */
	if( win -> status & WS_GROW && CONF(app)->weffect == TRUE)
		mt_graf_growbox( 0, 0, 0, 0, x, y, w, h, app->aes_global);
	
	/* open the AES window */
	res = mt_wind_open( win->handle, x, y, w, h, app->aes_global);
#ifdef WINDOMDEBUG
	if( res == 0) mt_form_alert( 1, "[1][WINDOM warning:|Impossible to open a window.][Ok]",app->aes_global);
#endif

	/* set the WS_OPEN flag */
	win->status |= WS_OPEN;
	
	/* update wglb.* variables */
	wlst_recompute_wglb(app);
	
#ifdef WM_OPENED
	/* execute WM_OPENED
	 *
	 * note: important to _execute_ (not just put into the AES message queue)
	 *       before any mt_EvntHandle( MU_MESSAG ) is called */
	{
		GRECT rect;
		mt_wind_get_grect( win->handle, WF_CURRXYWH, &rect, app->aes_global );

		app->evnt.buff[0] = WM_OPENED;
		app->evnt.buff[3] = win->handle;
		*(GRECT*)&app->evnt.buff[4] = rect;
		mt_EvntExec( app, win, WM_OPENED);
	}
#endif

	/* update sliders */
	mt_WindSlider( app, win, HSLIDER|VSLIDER);
	
	return res;
}
