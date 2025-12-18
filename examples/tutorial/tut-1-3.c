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
 * $Source: /cvsroot/windom/windom/examples/tutorial/tut-1-3.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:39:25 $
 *   $Revision: 1.5 $
 */


#include <stdlib.h>
#include <windom.h>

struct circle {
  short color;
  short interior;
  short style;
};

/**
 * Callback function called when the window is destroyed, i.e.
 * when \b WM_DESTROY message is catched by EvntWindom().
 */

static
void __CDECL evnt_destroy_circle( WINDOW *win, short buff[8], void *C) {
        free( win->info);
        free( C);  /* free memory */
}

/**
 * Callback function called when the window content should be drawn.
 */

static
void __CDECL evnt_draw_circle( WINDOW *win, short buff[8], void *data) {
	short x, y, w, h;
	struct circle *circ = (struct circle *) data;

	/* Get the workspace coordinates */
	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);

	/* Set some VDI attributes */
	vsf_color( win->graf->handle, circ->color);
	vsf_interior( win->graf->handle, circ->interior);
	vsf_style( win->graf->handle, circ->style);

	/* Drawing the circle */
	v_circle( win->graf->handle, x+w/2-1, y+h/2-1, MIN( w, h)/3);
}

static void update_info( WINDOW *win, struct circle *circ) {
	sprintf( win->info, "color:%d interior:%d style:%d", circ->color, circ->interior, circ->style);
	WindSetStr( win, WF_INFO, win->info);
}

/**
 * Callback function for keyboard event
 */

static
void __CDECL evnt_keybd_circle( WINDOW *win, short buff[8], void *data) {
	/* Get the real ascii code of a key using its scancode */
	int key = keybd2ascii( evnt.keybd, evnt.mkstate & (K_LSHIFT|K_RSHIFT));
	struct circle *circ = (struct circle *) data;
	switch( key) {
	case 'q':
	case 'Q':
	     /* Quit application */
	     if( FormAlert( 1, FA_QUESTION "[Really quit ?][Yes|No]") == 1)
	         ApplWrite( _AESapid, AP_TERM, 0, 0, 0, 0, 0);
	     break;
	case 'u':
	case 'U':
	     /* close window */
	     ApplWrite( _AESapid, WM_CLOSED, win->handle, 0, 0, 0, 0);
	     break;
	case 'w':
	case 'W':
	     /* cycle window */
	     WindSet( win, WF_BOTTOM, win->handle,0,0,0);
	     break;
	     /* change color */
	case '-':
	     circ->color = (--circ->color+16)%16;
	     update_info(win,circ);
	     snd_rdw( win);
	     break;
	case '+':
	     circ->color = ++circ->color%16;
	     update_info(win,circ);
	     snd_rdw( win);
	     break;
	}
}

/**
 * This function creates a 'Circle' window.
 */

WINDOW *WindCircle( int attr, int color, int interior, int style) {
	struct circle *C;
	WINDOW *win;
	char *info;

	/* Create a circle data */
	C = (struct circle *) malloc( sizeof( struct circle));
	C->color = color;
	C->interior = interior;
	C->style = style;

	/* Create the window */
	win = WindCreate( WAT_ALL, app.x, app.y, app.w, app.h);

 	/* Declare new event message */
	EvntDataAdd( win, WM_REDRAW, evnt_draw_circle, C, EV_BOT);
 	EvntDataAdd( win, WM_DESTROY, evnt_destroy_circle, C, EV_TOP);
 	EvntDataAttach( win, WM_XKEYBD, evnt_keybd_circle, C);

	WindSetStr( win, WF_NAME, "WinDom tutorial 1-3 : drawing a circle" );
	info = (char*)malloc( 128);
	strcpy( info, "---");
	WindSetStr( win, WF_INFO, info);
	update_info(win,C);
	
	return win;
}

/**
 * Global events
 */

static
void __CDECL evnt_apterm( ) {
	while( wglb.first) {
		ApplWrite( _AESapid, WM_DESTROY, wglb.first->handle, 0, 0, 0, 0);
		EvntWindom( MU_MESAG);
	}
}

int main( void) {
	WINDOW *win;

	/* Init WinDom */
	ApplInit();

	FormAlert( 1, FA_INFO "["
		   "q: quit, u: close window|"
		   "w: cycle window|"
		   "+/-: cycle circle color][OK]");

	/* Display a red circle ... */
	win = WindCircle( WAT_ALL, RED, FIS_SOLID, 0);
	WindOpen( win, app.x, app.y, app.w/2, app.h/2);

 	/* ... and a yellow one with a nice pattern */
	win = WindCircle( WAT_ALL, YELLOW, FIS_PATTERN, 9);
	WindOpen( win, app.x+10, app.y+10, app.w/2, app.h/2);

	/* Attach a global event */
	EvntAttach( NULL, AP_TERM, evnt_apterm);

	/* Handle GEM event */
	while( wglb.first) EvntWindom( MU_MESAG|MU_KEYBD);

	/* ok, it's finish */
	ApplExit();
	return EXIT_SUCCESS;;
}

