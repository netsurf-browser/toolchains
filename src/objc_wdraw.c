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
 * $Source: /cvsroot/windom/windom/src/objc_wdraw.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/03/06 20:04:15 $
 *   $Revision: 1.8 $
 */


#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/**
 * @brief Draw any object in any window.
 *
 * @param app application descriptor,
 * @param win targeted window descriptor,
 * @param tree objects tree containing the object to draw,
 * @param index index of object to draw in \e tree,
 * @param depth level of redraw in children hierarchy,
 * @param xclip, yclip, wclip, hclip clipping aera coordinates,
 * @return 0 or a negative code error.
 * @pre Do not use this function in a callback function.
 * 
 * This function is a sub-function of my_ObjcDraw(). It allows you to
 * draw an object tree in a window with respect to the window workspace.
 *
 * Usually, you don't need to call this function, only mt_ObjcDraw().
 *
 * @sa mt_ObjcDraw(), mt_ObjcWindChange().
 */

int mt_ObjcWindDraw( APPvar *app, WINDOW *win,
					 OBJECT *tree, int index, int depth,
					 int xclip, int yclip, int wclip, int hclip) {
	int res=0;
	GRECT r1, rdraw;
	rc_set( &rdraw, xclip, yclip, wclip, hclip);

	mt_graf_mouse( M_OFF, 0L, app->aes_global);
	mt_WindXGetGrect( app, win, WF_FIRSTAREAXYWH, &rdraw, &r1);
	while (r1.g_w && r1.g_h) {
		if ( rc_intersect( &rdraw, &r1)) 
			res = mt_objc_draw_grect( tree, index, depth, &r1, app->aes_global);
		mt_WindGetGrect( app, win, WF_NEXTXYWH, &r1);
	}
	mt_graf_mouse( M_ON, 0L, app->aes_global);
	return res;
}

