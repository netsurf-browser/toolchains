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
 * $Source: /cvsroot/windom/windom/src/frame_calc.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.7 $
 */


#include <stdlib.h>
#include "globals.h"
#include "sliders.h"

/** get the framed window work area
 *
 *  @param app application descriptor
 *  @param win framed window descriptor
 *  @param mode global area (1) or work area (0)
 *  @param _x,_y,_w,_h position and size of the work area,
 *
 *  @return 0 (success) or a negative error code.
 *
 *	mt_FrameCalc() computes the coordinates and size of the work
 *	area of a framed window. Widgets are take in consideration. This
 *	function is a sub-function of mt_WindGet(). In fact, mt_WindGet(WF_WORKXYWH)
 *	works correctly with framed window. The \b WF_CURRXYWH mode is
 *	supported too. It better to use mt_WindGet(), specially in event
 *	functions because these windows can be used as normal window or as
 *	framed window.
 *
 *	There is an important remark to do here. When you use mt_WindSet() and
 *	mt_WindGet() with the mode \b WF_WORKXYWH on a framed window descriptor,
 *	these function compute the frame work area size and not the window
 *	area size. Thus, mt_WindGet( ..., WF_WORKXYWH, ...) is identical to
 *	mt_FrameCalc(). This feature allows us to use directly event function
 *	initially written for window. So, prefined window, such as dialog box,
 *	can be framed.
 */
	
int mt_FrameCalc( APPvar *app, WINDOW *win, int mode, INT16 *_x, INT16 *_y, INT16 *_w, INT16 *_h) {
	WINDOW *root = mt_WindHandle( app, win->handle), *scan;
	int line;
	INT16 xc, yc, wc, hc;
	INT16 x, y, w, h;
	GRECT r1,r2;
	FRAME *frame = mt_DataSearch( app, root, WD_WFRA);
	FRAMEINFO *finfo=NULL; /* initialized to quiet gcc warning.... FIXME! */

	/* if the frame is closed */
	if( !(win->status & WS_OPEN)) {
		*_x = *_y = *_w = *_h = 0;
		return 0;
	}
	
	/* Get the size of the root window */
	mt_wind_get( root -> handle, WF_CURRXYWH, &xc, &yc, &wc, &hc, app->aes_global);
	mt_WindCalc( app, WC_WORK, root, xc, yc, wc, hc, &x, &y, &w, &h);
	rc_set( &r1, x, y, w, h);
	scan = frame->list;
	line = GETFINFO(app,scan)->line;
	hc = 0;
	for(; scan;scan=scan->next) {
		/* new line */
		finfo = GETFINFO(app,scan);
		if( finfo->line > line) {
			line = finfo->line;
			y += hc + frame->border;
			hc = 0;
		} 
 		hc = MAX( hc, PROPOR( finfo->flags & FRAME_HSCALE, h, finfo->h));
		if( scan == win) break;
		/* we've got the right line. Now, go to next column */
		if( finfo->line == GETFINFO(app,win)->line) 
			x += PROPOR( finfo->flags & FRAME_WSCALE, w, finfo->w) 
				  + frame->border;
	}
	
	/* TODO : Warning, here finfo may be uninitialised */
	w = PROPOR( finfo->flags & FRAME_WSCALE, w, GETFINFO(app,win)->w);
	h = PROPOR( finfo->flags & FRAME_HSCALE, h, GETFINFO(app,win)->h);
	
	/* Intersection between root area and frame area */
	rc_set( &r2, x, y, w, h);
	rc_intersect( &r1, &r2);
	x = r2.g_x; y = r2.g_y; w = r2.g_w; h = r2.g_h;
	
	/* the widgets */
	if( mode == 0 && !(win->status & WS_ICONIFY)) {
		OBJECT *tree = get_frame_slider(app);
		
		if( scan->attrib & (UPARROW|DNARROW|VSLIDE|SIZER))
			w -= tree[M_V_BG].ob_width - 1;
		if( scan->attrib & (LFARROW|RTARROW|HSLIDE))
			h -= tree[M_H_BG].ob_height - 1;
		if( scan->attrib & INFO) {
			y += tree[M_INFO].ob_height+1;
			h -= tree[M_INFO].ob_height+1;
		}
	}
	*_x = x;
	*_y = y;
	*_w = w;
	*_h = h;
	return 0; 
}
