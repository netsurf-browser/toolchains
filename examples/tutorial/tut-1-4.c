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
 * $Source: /cvsroot/windom/windom/examples/tutorial/tut-1-4.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:39:25 $
 *   $Revision: 1.2 $
 */

/**
 * \file tut-1-4.c
 * Tutorial WinDom 
 * \version 1.0.0
 * \author Dominique B&eacute;r&eacute;ziat
 *
 * 
 *
 * $Id: tut-1-4.c,v 1.2 2006/01/03 14:39:25 bercegeay Exp $
 */

#include <stdlib.h>
#include <windom.h>

struct circle {
  short color;
  short interior;
  short style;
  int   xc, yc, radius;
};

/**
 * Event function called when the window is destroyed.
 */

static
void __CDECL evnt_destroy_circle( WINDOW *win, struct circle *C) {
  free( C);			  /* free memory */
}

/**
 * Event function called when the window content should be drawn.
 * We draw a disc using VDI function. The disc size is related to
 * the window size.
 */

static
void __CDECL evnt_draw_circle( WINDOW *win, struct circle *circ) {
	short x, y, w, h;

	/* Get the workspace coordinates */
	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);

	/* Clear the background */
	WindClear( win);

	/* Set VDI attributes */
	vsf_color( win->graf.handle, circ->color);
	vsf_interior( win->graf.handle, circ->interior);
	vsf_style( win->graf.handle, circ->style);

	/* Drawing the circle */
	v_circle( win->graf.handle, x+circ->xc-win->xpos*win->w_u, 
		  y+circ->yc-win->ypos*win->h_u, circ->radius);
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
	C->radius = 100;
	C->xc = 150;
	C->yc = 150;

	/* Create the window */
	win = WindCreate( WAT_ALL, app.x, app.y, app.w, app.h);

	/* Set slider parameters */
	win->w_u = 10;
	win->h_u = 10;
	win->xpos_max = 40;
	win->ypos_max = 40;


 	/* Declare new event message */
	EvntDataAttach( win, WM_REDRAW, evnt_draw_circle, C);
 	EvntDataAdd( win, WM_DESTROY, evnt_destroy_circle, C, EV_TOP);

	/* Open the window */
	WindOpen( win, app.x, app.y, app.w/2, app.h/2);
	WindSetStr( win, WF_NAME, "WinDom tutorial 1-4 : drawing a circle" );

	/* Handle GEM event */
	while( wglb.first) EvntWindom( MU_MESAG+MU_KEYBD);

	ApplExit();
	return EXIT_SUCCESS;
}

