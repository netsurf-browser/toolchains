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
 * $Source: /cvsroot/windom/windom/examples/tutorial/tut-1-1.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:39:25 $
 *   $Revision: 1.3 $
 */


/** MARK1 */
#include <stdlib.h>
#include <windom.h>

/** MARK2 */
int main( void) {
	/* window descriptor */
	WINDOW *win;
		  
	/* WinDom initialisation */
	ApplInit();
		  
	/* Create a window and keep its descriptor */
	win = WindCreate( NAME|MOVER|CLOSER, app.x, app.y, app.w, app.h);
/** MARK3 */

	/* Open the window */
	WindOpen( win, app.x, app.y, app.x+app.w/2, app.y+app.h/2);

	/* Give it a title */
	WindSetStr( win, WF_NAME, __FILE__ ": Simple GEM window");

	/* Handle the GEM events */
	while( wglb.first) EvntWindom( MU_MESAG);
/** MARK4 */

	FormAlert( 1, FA_INFO "[Program successfully terminated.][OK]");

	/* Terminate WinDom session */
	ApplExit();
	return EXIT_SUCCESS;
}

/** MARK5 */
