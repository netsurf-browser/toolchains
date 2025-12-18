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
 * $Source: /cvsroot/windom/windom/demo/win-fram.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/03/14 19:45:41 $
 *   $Revision: 1.8 $
 */

#include <stdlib.h>
#include <windom.h>
#include "demo.h"
#include "global.h"


short atarilogo[] = 
	{ 0x0000, 0x05A0, 0x05A0, 0x05A0,
      0x05A0, 0x0DB0, 0x0DB0, 0x1DB8,
	  0x399C, 0x799E, 0x718E, 0x718E,
	  0x6186, 0x4182, 0x0000, 0x0000 };
					  
/*
 * A WM_REDRAW mevent function of a framed window :
 * it is identic to a standard window. The function
 * just draw a textured ellipse.
 */
 
static void __CDECL draw_frame( WINDOW *win, short buff[8]) {
	INT16 x,y,w,h;
	short vdih = win->graf->handle;
	
	WindClear( win);
	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);
	vsf_interior( vdih, FIS_USER);
	vsf_udpat( vdih, atarilogo, 1);
	vsf_color(vdih,3);
	v_ellipse( vdih, x+w/2, y+h/2, w/2, h/2);
}

/*
 *	A example of a frame window containing two frames
 */


static void _create_two_frame(void) {
	WINDOW *win, *frame;
	
	/* Create the root window */
	win = FrameCreate( MOVER|NAME|CLOSER|SMALLER);
	if (win==NULL) {
		FormAlert(1, FA_ERROR "[The creation of the frame|has failed][continue]");
		return;
	}
	/* give a title to the window */
	WindSetStr( win, WF_NAME, get_string( FRAME_TITLE));
	/* adjust some specific frame parameters */
	FrameSet( win, FRAME_BORDER, param.FrameWidth);
	FrameSet( win, FRAME_COLOR, param.FrameColor);
	FrameSet( win, FRAME_NOBG, 1, 0);
	 
	/* framed window are initally standard window that
	 * we create normally */
	frame = WindCreate( WAT_NOINFO-SIZER, app.x, app.y, app.w, app.h);
	if (frame) {
		EvntAttach( frame, WM_REDRAW, draw_frame);
		/* the window is attached to the root window */
		FrameAttach( win, frame, 0, 0, 50, 100, FRAME_WSCALE|FRAME_SELECT);
	}
	
	/* a second frame ... */
	frame = WindCreate( WAT_NOINFO, app.x, app.y, app.w, app.h);
	if (frame) {
		EvntAttach( frame, WM_REDRAW, draw_frame);
		FrameAttach( win, frame, 0, 1, 50, 100, FRAME_WSCALE|FRAME_SELECT);
	}

	/* set the max height of the root window */
	win -> h_max = 200;
	/* open the root window */
	WindOpen( win, -1, -1, 200, 100);
}

/*
 *	A example of a frame window containing three frames
 */

static void _create_three_frame(void) {
	WINDOW *win, *frame;
	
	win = FrameCreate( MOVER|NAME|CLOSER|SMALLER);
	if (win==NULL) {
		FormAlert(1, FA_ERROR "[The creation of the frame|has failed][continue]");
		return;
	}
	
	WindSetStr( win, WF_NAME, get_string( FRAME_TITLE));
	FrameSet( win, FRAME_BORDER, param.FrameWidth);
	FrameSet( win, FRAME_COLOR, param.FrameColor);

	frame = WindCreate( WAT_NOINFO-SIZER, app.x, app.y, app.w, app.h);
	if (frame) {
		EvntAttach( frame, WM_REDRAW, draw_frame);
		FrameAttach( win, frame, 0, 0, 50, 100, FRAME_WSCALE|FRAME_SELECT);
	}
	
	frame = WindCreate( WAT_NOINFO-SIZER, app.x, app.y, app.w, app.h);
	if (frame) {
		EvntAttach( frame, WM_REDRAW, draw_frame);
		FrameAttach( win, frame, 0, 1, 50, 100, FRAME_WSCALE|FRAME_SELECT);
	}

	frame = WindCreate( WAT_NOINFO, app.x, app.y, app.w, app.h);
	if (frame) {
		EvntAttach( frame, WM_REDRAW, draw_frame);
		FrameAttach( win, frame, 1, 0, 100, 100, FRAME_WSCALE|FRAME_SELECT);
	}

	win -> h_max = 400;
	WindOpen( win, -1, -1, 200, 200);
}

static int two_or_three = 2;
static int show_frame_msg = 1;

static void open_frame_win() {
	if (two_or_three==2) _create_two_frame();
	else _create_three_frame();
}

static void __CDECL close_modal( WINDOW *win, short buff[8]) {
	MenuEnable();
	open_frame_win();
}

static void __CDECL continue_button_clicked( WINDOW *win, int obj, int mode, void *data) {
	ObjcChange( OC_FORM, win, obj, NORMAL, FALSE);
	ApplWrite( _AESapid, WM_CLOSED, win->handle, 0, 0, 0, 0);
}

static void display_warning_message() {
	if (show_frame_msg) {
		WINDOW * win;
		win = FormCreate(get_tree( FRAME), WAT_FORM,0L,"WARNING", NULL, TRUE, FALSE);
		WindSet( win, WF_BEVENT, BEVENT_MODAL, 0, 0, 0);
		EvntAdd( win, WM_CLOSED, close_modal, EV_TOP);
		ObjcAttachVar( OC_FORM, win, FRAMEACK, &show_frame_msg, 1);
		ObjcAttachFormFunc( win, FRAMEOK, continue_button_clicked, NULL);
		MenuDisable();
	} else {
		open_frame_win();
	}
}

void create_two_frame(void) {
	two_or_three = 2;
	display_warning_message();
}

void create_three_frame(void) {
	two_or_three = 3;
	display_warning_message();
}

