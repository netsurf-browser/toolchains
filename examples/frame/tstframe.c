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
 * $Source: /cvsroot/windom/windom/examples/frame/tstframe.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/23 19:23:47 $
 *   $Revision: 1.8 $
 */

#include <stdio.h>
#include "windom.h"

/* Create a false border */

static WINDOW *FrameBorder( WINDOW *win, int line, int col, int w, int h, int mode) {
	WINDOW *frame;
	
	frame = WindCreate( 0, app.x, app.y, app.w, app.h);
	EvntDelete( frame, WM_REDRAW);
	FrameAttach( win, frame, line, col, w, h, mode);
	FrameSet( frame, FRAME_FLAGS, FRAME_NOBORDER, TRUE);
	return frame;
}

static void test_click( WINDOW *win, short wbuff[8]){
	if( wbuff[6] & K_CTRL) {
		if( win->status & WS_FRAME) {
			WINDOW *frame = WindHandle( win->handle);
			WINDOW *another;
			
			another= FrameRemove( frame, win, 0, 0);
			if( another) {
				WindOpen ( another, 200, 200, 100, 100);
				snd_rdw( frame);
			}
		}
	} else {
		int l, c;
		FrameGet( win, FRAME_CELL, &l, &c);
		FormAlert( 1, "[1][frame (%d,%d) selected][OK]", l, c);
	}
}

static void draw_blue_square( WINDOW *win, short wbuff[8]) {
	INT16 x, y, w, h, xy[4];
	
	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);
	xy[0]=x; xy[1]=y;
	xy[2]=x+w-1; xy[3]=y+h-1;
	vsf_color( win->graf->handle, BLUE);
	v_bar( win->graf->handle, xy);
}

static void draw_black_square( WINDOW *win, short wbuff[8]) {
	INT16 x, y, w, h, xy[4];
	
	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);
	xy[0]=x; xy[1]=y;
	xy[2]=x+w-1; xy[3]=y+h-1;
	vsf_color( win->graf->handle, BLACK);
	v_bar( win->graf->handle, xy);
}

/* A pseudo WinVdi */

static void w_circle( WINDOW *win, int x, int y, int r) {
	INT16 wx, wy, ww, wh;
	WindGet( win, WF_WORKXYWH, &wx, &wy, &ww, &wh);
	v_circle( win->graf->handle, x + wx - (int)win->xpos*win->w_u,
								y + wy - (int)win->ypos*win->h_u, r);
}

static void w_gtext( WINDOW *win, int x, int y, char *text) {
	INT16 wx, wy, ww, wh;
	WindGet( win, WF_WORKXYWH, &wx, &wy, &ww, &wh);
	v_gtext ( win->graf->handle, x + wx - (int)win->xpos*win->w_u,
								y + wy - (int)win->ypos*win->h_u, text);
}

static WINDOW *VdiCreate( int attrib, int wwork, int hwork) {
	WINDOW *win = WindCreate( attrib, app.x, app.y, app.w, app.h);
	win->xpos_max = wwork;
	win->ypos_max = hwork;
	win->w_u = win->h_u = 10;
	return  win;
}

/***         ***/

static void draw_vdi1( WINDOW *win, short wbuff[8]) {	
	WindClear( win);
	w_gtext( win, 10, 50, "blablabla");
}

static void draw_vdi2( WINDOW *win, short wbuff[8]) {	
	vsf_color( win->graf->handle, LRED);
	w_circle( win, 50, 50, 40);
}


int main( void) {
	WINDOW *win, *frame;
	
	ApplInit();
	FrameInit();

	FormAlert(1,"[1][This example uses the|old FRAME API|to manage its frames][continue]");
			
	win = FrameCreate( MOVER|NAME|CLOSER|SMALLER|FULLER);
	FrameSet( win, FRAME_BORDER, 6);
	FrameSet( win, FRAME_COLOR, LWHITE);
		
	/* upper-left frame */
	frame = VdiCreate( WAT_NOINFO-SIZER, 500, 400);
	EvntAttach( frame, WM_XBUTTON, test_click);
	EvntAttach( frame, WM_REDRAW, draw_vdi1);
	FrameAttach( win, frame, 0, 0, 60, 100, FRAME_WSCALE|FRAME_SELECT);
	
	/* upper-right frame */
	frame = VdiCreate( WAT_NOINFO-0x20, 500, 400);
	EvntAttach( frame, WM_XBUTTON, test_click);
	EvntAdd( frame, WM_REDRAW, draw_vdi2, EV_BOT);
	FrameAttach( win, frame, 0, 1, 40, 100, FRAME_WSCALE|FRAME_SELECT);	

	/* a central frame as border */
	FrameBorder( win, 1, 0, 100, 30, FRAME_WSCALE);
	
	/* lower-left frame */	
	frame = WindCreate( 0, app.x, app.y, app.w, app.h);
	EvntAttach( frame, WM_REDRAW, draw_blue_square);
	EvntAttach( frame, WM_XBUTTON, test_click);
	FrameAttach( win, frame, 2, 0, 100, 100, FRAME_HSCALE);

	/* lower-right frame */
	frame = WindCreate( SIZER, app.x, app.y, app.w, app.h);
	EvntAttach( frame, WM_REDRAW, draw_black_square);
	EvntAttach( frame, WM_XBUTTON, test_click);
	FrameAttach( win, frame, 2, 1, 100, 100, FRAME_WSCALE|FRAME_HSCALE|FRAME_SELECT);

	WindOpen( win, 50, 50, 300, 200);
	WindSetStr( win, WF_NAME, "Example of framed windows");	
	
	while( wglb.first)
		EvntWindom( MU_MESAG|MU_BUTTON);

	FrameExit();
	ApplExit(); 
	return 0;
}
