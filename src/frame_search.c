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
 * $Source: /cvsroot/windom/windom/src/frame_search.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */


#include <stdlib.h>
#include "globals.h"

/** Find a frame by cell reference
 *
 *  @param app application descriptor
 *  @param win frame root window descriptor
 *  @param line,col framed window cell coordinates
 *
 *  @return framed window descripteur found
 *
 *	Returns the framed window descriptor of the \a line, \a col
 *	cell in the \a win root frame window.
 *	A \b NULL value returned indicates the frame is not
 *	found.
 *
 *  @sa mt_FrameFind()
 */

WINDOW *mt_FrameSearch( APPvar *app, WINDOW *win, int line, int col) {
	WINDOW *scan = ((FRAME*)mt_DataSearch(app,win,WD_WFRA))->list;
	FRAMEINFO *finfo;

	while( scan) {
		finfo = GETFINFO(app,scan);
		if( line == finfo->line && col == finfo->col)
			return scan;
		scan = scan->next;
	}
	return NULL;
}

