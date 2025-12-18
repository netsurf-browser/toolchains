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
 * $Source: /cvsroot/windom/windom/examples/tutorial/tut-1-2.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:39:25 $
 *   $Revision: 1.4 $
 */

/**
 * @file
 * 
 * $Id: tut-1-2.c,v 1.4 2006/01/03 14:39:25 bercegeay Exp $
 */

#include <stdlib.h>
#include <windom.h>

struct circle {
  short color;
  short interior;
  short style;
};

/**
 * Event function called when the window is destroyed.
 */

static
void __CDECL evnt_destroy_circle( WINDOW *win, short msg[], void *C) {
	free( C);			  /* free memory */
	WindClose( win);		  /* close the window */
	WindDelete( win);		  /* delete the window */
}

/**
 * Event function called when the window content should be drawn.
 * We draw a disc using VDI function. The disc size is related to
 * the window size.
 */

static
void __CDECL evnt_draw_circle( WINDOW *win, short msg[], void *data) {
	short x, y, w, h;
	struct circle *circ = (struct circle *)data;

	/* Get the workspace coordinates */
	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);

	/* Clear the background */
	WindClear( win);

	/* Set VDI attributes */
	vsf_color( win->graf->handle, circ->color);
	vsf_interior( win->graf->handle, circ->interior);
	vsf_style( win->graf->handle, circ->style);

	/* Drawing the circle */
	v_circle( win->graf->handle, x+w/2-1, y+h/2-1, MIN( w, h)/3);
}

int main( void) {
	WINDOW *win;
	struct circle *C;

	/* Init WinDom */
	ApplInit();

	/* Create a circle data */
	C = (struct circle *) malloc( sizeof( struct circle));
	C->color = BLACK;
	C->interior = FIS_SOLID;
	C->style = 0;

	/* Create the window */
	win = WindCreate( WAT_ALL, app.x, app.y, app.w, app.h);

 	/* Declare new event message */
	EvntDataAttach( win, WM_REDRAW, evnt_draw_circle, C);
 	EvntDataAttach( win, WM_DESTROY, evnt_destroy_circle, C);

	/* Open the window */
	WindOpen( win, app.x, app.y, app.w/2, app.h/2);
	WindSetStr( win, WF_NAME, "WinDom tutorial 1-2 : drawing a circle" );

	/* Handle GEM event */
	while( wglb.first) EvntWindom( MU_MESAG+MU_KEYBD);

	ApplExit();
	return EXIT_SUCCESS;
}

