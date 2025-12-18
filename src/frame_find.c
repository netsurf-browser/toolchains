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
 * $Source: /cvsroot/windom/windom/src/frame_find.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */


#include <stdlib.h>
#include "globals.h"

/** find a framed window
 *
 *  @param app application descriptor
 *  @param win root frame window descriptor
 *  @param x,y coordinates in desktop
 *
 *  @return framed window descriptor found or NULL
 *
 *	mt_FrameFind() finds the framed window pointed by the
 *	\a x, \a y coordinates in the \a win root frame window.
 *	A \b NULL value returned indicates the frame is not
 *	found.
 *
 *  @sa mt_FrameSearch()
 */
	
WINDOW *mt_FrameFind( APPvar *app, WINDOW *win, int x, int y) {
	FRAME *frame = mt_DataSearch( app, win, WD_WFRA);
	WINDOW *scan = frame->list;
	INT16 xf, yf, wf, hf;
	
	while( scan) {
		mt_FrameCalc( app, scan, 1, &xf, &yf, &wf, &hf);
		if( IS_IN( x, y, xf, yf, wf, hf))
			break;
		scan = scan->next;
	}
	return scan;
}
