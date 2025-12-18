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
 * $Source: /cvsroot/windom/windom/src/w_setpal.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include "globals.h"

/** restore a screen color palette
 *  
 *  @param app application descriptor
 *  @param win window descriptor or \b NULL
 *
 *  apply the set of color to the virtual workstation of the 
 *  window (if not NULL) or application (otherwise).
 *
 *  @sa w_getpal()
 */

void w_setpal( APPvar *app, WINDOW *win) {
	int index;
	short handle;
	W_COLOR *palette;
	
	if (win) {
		palette = win->graf->palette;
		handle  = win->graf->handle;
	} else {
		palette = app->graf.palette;
		handle  = app->graf.handle;
	}

	if( !palette) return;
	
	for ( index = 0; index < app->color; index++)
		vs_color( handle, index, palette[index]);
}
