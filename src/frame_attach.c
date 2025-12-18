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
 * $Source: /cvsroot/windom/windom/src/frame_attach.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/01/23 05:35:09 $
 *   $Revision: 1.13 $
 */


#include <stdlib.h>
#include "globals.h"
 

static
int __CDECL frame_wind_get(  APPvar *app, WINDOW *win, int mode, GRECT *clip, INT16 *x, INT16 *y, INT16* w, INT16 *h ) {
	int res = E_OK;
	switch( mode) {
	case WF_WORKXYWH:
		res = mt_FrameCalc( app, win, 0, x, y, w, h);
		break;
	case WF_CURRXYWH:
		res = mt_FrameCalc( app, win, 1, x, y, w, h);
		break;

	/* Sliders and frame */
	case WF_HSLIDE:
		*x=GETFINFO(app,win)->hsldpos;
		break;
	case WF_VSLIDE:
		*x=GETFINFO(app,win)->vsldpos;
		break;
	case WF_HSLSIZE:
		*x=GETFINFO(app,win)->hsldsize;
		break;
	case WF_VSLSIZE:
		*x=GETFINFO(app,win)->vsldsize;
		break;
	default:
		/* standard AES WINDOW handling for others */
		return wind_aes_get( app, win, mode, clip, x, y, w, h);
	}
	return res;
}

static
int __CDECL frame_wind_set(  APPvar *app, WINDOW *win, int mode, int par1, int par2, int par3, int par4) {
	int res = E_OK;
	GRECT r;
	INT16 x = par1;
	void  *v1 = (void*) (long) ( ((long)par1 << 16 ) | ((long)par2 & 0xFFFFL) ) ;

	switch( mode) {
	case WF_INFO:
		win->info = v1;	
		res = mt_FrameCalc( app, win, 1, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
		set_gadget_pos( app, win, &r, 0x20);
		break;

	/* Sliders and frame */
	case WF_HSLIDE:
		GETFINFO(app,win)->hsldpos= x;
		res = mt_FrameCalc( app, win, 1, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
		set_gadget_pos( app, win, &r, 0x8|0x40);
		break;
	case WF_VSLIDE:
		GETFINFO(app,win)->vsldpos = x;
		res = mt_FrameCalc( app, win, 1, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
		set_gadget_pos( app, win, &r, 0x10|0x40);
		break;
	case WF_HSLSIZE:
		GETFINFO(app,win)->hsldsize = x;
		res = mt_FrameCalc( app, win, 1, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
		set_gadget_pos( app, win, &r, 0x8|0x40);
		break;
	case WF_VSLSIZE:
		GETFINFO(app,win)->vsldsize = x;
		res = mt_FrameCalc( app, win, 1, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
		set_gadget_pos( app, win, &r, 0x10|0x40);
		break;
	default:
		/* standard AES WINDOW handling for others */
		return wind_aes_set( app, win, mode, par1, par2, par3, par4);
	}
	return res;
}

static
int __CDECL frame_wind_calc( APPvar *app, WINDOW *win, int type, int x, int y, int w, int h,
							INT16 *xout, INT16 *yout, INT16 *wout, INT16 *hout) {
	return mt_FrameCalc( app, win, type == WC_BORDER ? 1 : 0, xout, yout, wout, hout);
}

static
int __CDECL frame_wind_remove( APPvar *app, WINDOW *win ) {
	remove_frame_from_list( app, mt_WindHandle( app, win->handle), win);
	return E_OK;
}


static
WIND_HANDLERS frame_wind_handlers = {
	wind_aes_get_root_handle,
	frame_wind_get,
	frame_wind_set,
	frame_wind_calc,
	frame_wind_remove
};

/** attach a framed window in a window
 *  
 *  @param app application descriptor
 *  @param win window descriptor of the root window
 *  @param src window descriptor of the window to attach
 *  @param line,col  cell which will contain the frame
 *  @param w,h size of the frame
 *  @param flags bit field of special frame features (see mt_FramSet()).
 *
 *  This function attaches a window in a root window. The attached window
 *  becomes a framed window. The framed window is created like a standard
 *  window. It can be a formular or any custom user window. The window
 *  descriptor of the framed window is removed from the list of window
 *  and the \b WS_FRAME bit of the window descriptor \a status
 *  field is set to 1.
 *
 *  Frames inside a window are organized by line of cells. Each line
 *  can contain variable cells. The \a line and \a col parameters
 *  defines the order of the frame in the window. The real coordinates of
 *  a frame inside the work area of the root window depend on the frame sizes.
 *  And these sizes can be eventually changed by the user. The \a w and
 *  \a h parameters define the initial size of the frame.
 *
 *  The \a flags parameter defines special feature of the frame. See
 *  mt_FrameSet() for a complet description of this parameter.
 *
 *  The frame uses the GEM widget of the window. Possible attributs for
 *  a frame are \b INFO, \b SIZER, \b HSLIDE, \b VSLIDE,
 *  \b UPARROW, \b DNARROW, \b LFARROW and \b RTARROW.
 *  Other widgets are ignored.
 *
 *  @sa mt_FrameCreate(), mt_FrameSet(), mt_FrameRemove().
 */  
 
void mt_FrameAttach( APPvar *app, WINDOW *win, WINDOW *src, int line, int col, int w, int h, int flags) {
	FRAME *frame = mt_DataSearch( app, win, WD_WFRA);
	WINDOW *scan = frame->list, *prev;
	GRECT r;
	FRAMEINFO *finfo;

	finfo = (FRAMEINFO *)malloc( sizeof(FRAMEINFO));
	if( finfo == NULL) {
		app->error = ENSMEM;
		return;
	}
	mt_DataAttach( app, src, WD_WFRI, finfo);
	
	/* set the sliders value */
	mt_WindSlider( app, src, VSLIDER|HSLIDER);
	
	if( src -> status & WS_OPEN) 
		mt_WindClose( app, src);
	mt_wind_delete( src->handle, app->aes_global);
	mt_RemoveWindow( app, src);

	/* WindSet/Get behaviour */
	mt_DataAttach( app, src, WD_WHLR, &frame_wind_handlers );
       	/* attach the window handlers */
	src -> status |= WS_CUSTOM; /* make the WindGet/WindSet/WindCalc use the handlers */
	
	src -> next = NULL;
	src -> status |= WS_FRAME;
	src -> status |= WS_OPEN;

	finfo -> col = col;
	finfo -> line = line;
	finfo -> w = w;
	finfo -> h = h;
	finfo -> flags = flags;

	frame -> col  = MAX( frame->col, col);
	frame -> line = MAX( frame->line, line);
	frame -> front = src;

	/* we keep some global characteristics of the root window */
	src -> handle = win -> handle;
	src -> icon = win -> icon;

	/* Add to the (sorting) list */
	prev = NULL;

	if( scan == NULL) {
		frame->list = src;
	} else {
		while( scan) {
			finfo = GETFINFO(app,scan);
			if( finfo->line > line ||
				(finfo->line == line &&
				 finfo->col > col)) { /* inserer */
					src->next = scan;
					if( prev)
						prev -> next = src;
					else
						frame->list = src;
					break;
				}
			prev = scan;
			scan = scan->next;
		}
		if( scan == NULL)
			prev->next = src;
	}
	
	mt_FrameCalc( app, src, 1, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
	set_gadget_pos( app, src, &r, 0);
}
