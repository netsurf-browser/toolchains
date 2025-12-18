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
 * $Source: /cvsroot/windom/windom/src/frame_get.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */


#include <stdarg.h>
#include <stdlib.h>
#include "globals.h"

/** get frame related informations
 *
 *  @param app application descriptor
 *  @param win window descriptor (of a root frame window of a framed window),
 *  @param mode parameter to get
 *  @param ... new values depending on \a mode
 *
 *	From WinDom version 1.20, frame sub structures in window descriptor
 *	have been hidden to improve portability with futur extensions.
 *	To compense the loss of information, the function mt_FrameGet()
 *	has been introduced. Currently, only one mode is available.
 *
 *  <table>
 *  <tr>
 *    <th>Mode<th>Type window<th>Description/Parameters
 *  <tr>
 *    <td>FRAME_CELL
 *    <td>framed window
 *    <td>returns cellule reference:
 *        - par1 is the line position (int value)
 *        - par2 is the col position (int value)
 *  </table>
 */  

void mt_FrameGet( APPvar *app, WINDOW *win, int mode, ...) {
	va_list args;
	int *par1, *par2;
	
	va_start( args, mode);
	par1 = va_arg( args, int*);
	par2 = va_arg( args, int*);
	va_end( args);
	if( win->status & WS_FRAME) {
		FRAMEINFO *finfo = GETFINFO(app,win);

		switch( mode) {
		case FRAME_CELL:
			*par1 = finfo -> line;
			*par2 = finfo -> col;
			break;
		}
	}
}

