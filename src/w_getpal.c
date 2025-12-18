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
 * $Source: /cvsroot/windom/windom/src/w_getpal.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include "globals.h"

/** save in a buffer the current screen color palette
 *
 *  @param app application descriptor
 *  @param win window descriptor or \b NULL
 *
 *  Get the AES set of colors, and store them in \a app
 *  (if \a win is \b NULL), or in \a win (if \a win is not \b NULL)
 *  for futur use.
 *
 *  @note
 *  the palet is saved in the GRAF_PORT data of \a win or \a app.
 *  By default, every windows use the application graf port, and so
 *  w_getpal(app,win) is equivalent to w_getpal(app,NULL) in most cases.
 *
 *  @sa w_setpal()
 */
	
void w_getpal( APPvar *app, WINDOW *win) {
	int index;
	W_COLOR *palette;
	
	if (win) palette = win->graf->palette;
	else     palette = app->graf.palette;
	
	if (!palette) return;
	
	for ( index = 0; index < app->color; index ++)
		vq_color( app->aeshdl, index, 0, palette[ index]);
}
