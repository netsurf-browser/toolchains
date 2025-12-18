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
 * $Source: /cvsroot/windom/windom/src/form_resize.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stdlib.h>
#include "globals.h"

/** 
 * @brief compute the window size prior to the formular size.
 *
 * @param app application descriptor,
 * @param win window descriptor,
 * @param x,y,w,h new size and window position.
 *
 * This function computes the size of a window formular in order to host the
 * formular. This function can be used to resize a formular when it changes
 * its size. 
 *
 * If the window containing formular is already opened at screen, the window
 * is resize and parameters x, y, w and h have not signification (NULL value can be
 * used). If the window is not opened at screen, x, y, w and h parameters are
 * filled with the new size and position and can be used to call 
 * WindSet(WF_CURRXYWH) function.
 *
 * \sa mt_FormCreate(), mt_FormAttach().
 */
 
void mt_FormResize( APPvar *app, WINDOW *win, INT16 *x, INT16 *y, INT16 *w, INT16 *h) {
	OBJECT *tree = FORM(app,win);
	int outlined = 0;
	INT16 xi, yi, wi, hi, d;
	
	if( tree -> ob_state & OUTLINED)
		outlined = OUTLINED_WIDTH;

	/* sliders */
	mt_graf_handle( &win -> w_u, &win -> h_u, x, x, app->aes_global);
	win -> xpos = win -> ypos = 0;
	win -> xpos_max = (tree[0].ob_width+2*outlined)/win -> w_u;
	win -> ypos_max = (tree[0].ob_height+2*outlined)/win -> h_u;

	/* maximum size */
	mt_WindCalc( app, WC_BORDER, win, 0, 0, tree[0].ob_width+2*outlined, 
			  tree[0].ob_height+2*outlined, &xi, &xi, &wi, &hi);
	mt_GrectCenter( app, wi, hi, &xi, &yi);
	win->w_max = wi;
	win->h_max = hi;

	/* Change window widget if form too big */
	if( wi >= app->w || hi >= app->h) {
		INT16 wx, wy, ww, wh;
		mt_WindGet( app, win, WF_CURRXYWH, &wx, &wy, &ww, &wh);
		mt_WindClose( app, win);
		mt_wind_delete( win->handle, app->aes_global);
		if( wi >= app->w)
			win->attrib |= HSLIDE|LFARROW|RTARROW|SIZER;
		if( hi >= app->h)
			win->attrib |= VSLIDE|DNARROW|UPARROW|SIZER;
		win->handle = mt_wind_create( win->attrib, app->x, app->y, app->w, app->h, app->aes_global);
		mt_WindOpen( app, win, wx, wy, ww, wh);
	}

	if( win->status & WS_OPEN) {
		mt_WindGet( app, win, WF_CURRXYWH, &xi, &yi, &d, &d);
		mt_WindSet( app, win, WF_CURRXYWH, xi, yi, wi, hi);
		mt_EvntRedraw( app, win);
	} 
	*x = xi;
	*y = yi;
	*w = wi;
	*h = hi;
}
