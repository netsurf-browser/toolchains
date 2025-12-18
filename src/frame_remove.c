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
 * $Source: /cvsroot/windom/windom/src/frame_remove.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */


#include <stdlib.h>
#include "globals.h"

/** Remove a framed window from a root window
 *  
 *  @param app application descriptor
 *  @param win root window descriptor
 *  @param child framed window descriptor or NULL
 *  @param line,col cell coordinate of the framed window
 *
 *  @return window descriptor of the frame removed
 *  
 *  mt_FrameRemove() removes a framed window from a root window. The removed
 *  frame becomes a normal window. If the \a frame parameter has a
 *  \b NULL value, the \a line and \a col cell coordinates are used to locate
 *  the framed window to remove.
 *  
 *  @sa mt_FrameAttach()
 */ 

WINDOW *mt_FrameRemove( APPvar *app, WINDOW *win, WINDOW *child, int line, int col) {
	if( child == NULL)
		child = mt_FrameSearch( app, win, line, col);
	if( remove_frame_from_list( app, win, child)) {
			child->handle = mt_wind_create( child->attrib, 
										 child->createsize.g_x, child->createsize.g_y,
										 child->createsize.g_w, child->createsize.g_h,
										 app->aes_global);
			mt_AddWindow( app, child);
			child -> status &= ~WS_FRAME;
			return child;
	}
	return NULL;
}

