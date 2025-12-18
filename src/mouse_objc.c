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
 * $Source: /cvsroot/windom/windom/src/mouse_objc.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */

#include "globals.h"

/** set the mouse cursor over the center of an object
 *
 *  @param app application decriptor
 *  @param tree address of a form
 *  @param index object of the form on which the mouse cursor
 *         should be centered
 *
 *
 *  This function will set the position of the mouse cursor
 *  on the middle of the object.
 *
 *  This function internally use mt_appl_tplay() to change
 *  the mouse position. This function may not work on some
 *  AESes which lack mt_appl_tplay.
 *
 */

void mt_MouseObjc( APPvar *app, OBJECT *tree, int index) {
	INT16 x, y;
	long rec[2];
		
	mt_objc_offset( tree, index, &x, &y, app->aes_global);
	x += tree[index].ob_width/2-1;
	y += tree[index].ob_height/2-1;

	rec[0] = 2;
	rec[1] = (long)y + ((long)x << 16);
	mt_appl_tplay( rec, 1, 100, app->aes_global);
}


