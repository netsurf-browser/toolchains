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
 * $Source: /cvsroot/windom/windom/src/slid_setsize.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stdlib.h>
#include "globals.h"

/**
 * @brief Change the size of the slider cursor widget.
 *
 * @param app application descriptor,
 * @param sld slider structure,
 * @param size new slider size, a value between 0 and 1000.
 *
 * This function changes the size (width for an horizontal slider and
 * height for a vertical slider) of the slider cursor widget. A
 * value between 0 and 1000 is requested, 1000 means the largest possible
 * size (it is the size of the cursor parent widget).
 * The cursor and its root element are redrawn.
 *
 * Generally, a cursor have a fixed size. But in the case of a slider
 * associated to a window displaying a list for elements for example, this
 * size can be used to symbolize the number of elements displayed in
 * the window compare to the total of elements. In this case : 
 * @code
      size = MIN( (elements_seen / elements_total)  * 1000, 1000); @endcode
 *
 *
 */

void mt_SlidSetSize( APPvar *app, void *sld, int size) {
	SLIDER *slid = sld;
	OBJECT *tree = mt_ObjcTree( app, slid->mode, slid->win);
	unsigned long ratio = size;
	
	if( slid->ori == SLD_HORI) {
		ratio *= tree[slid->bsld].ob_width-2*SLID_OFFSET;
		ratio /= 1000UL;
		tree[slid->sld].ob_width = (INT16) ratio;
	} else {
		ratio *= tree[slid->bsld].ob_height-2*SLID_OFFSET;
		ratio /= 1000UL;
		tree[slid->sld].ob_height = (INT16) ratio;
	}
	if( mt_slid_newpos( app, slid)) 
		mt_ObjcDraw( app, slid->mode, slid->win, slid->bsld, MAX_DEPTH);			
}
