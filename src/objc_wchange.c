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
 * $Source: /cvsroot/windom/windom/src/objc_wchange.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/03/06 20:04:15 $
 *   $Revision: 1.7 $
 */


#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/**
 * @brief Change state of any object in any window.
 *
 * @param app application descriptor,
 * @param win targeted window descriptor,
 * @param tree objects tree containing the object to draw,
 * @param index index of object to draw in \e tree,
 * @param x, y, w, h clipping aera coordinates,
 * @param state new object state,
 * @return 0 or a negative code error.
 * @pre Do not use this function in a callback function.
 * 
 * This function is a sub-function of my_ObjcChange(). It allows you to
 * chage an object state and redraw it in a window with respect to the 
 * window workspace.
 * 
 * Usually, you don't need to call this function, only mt_ObjcChange().
 *
 * @sa mt_ObjcChange(), mt_ObjcWindDraw().
 */

int mt_ObjcWindChange( APPvar *app, WINDOW *win, OBJECT *tree, int index, int x, int y, int w, int h, int state) {
	int prev_state = tree[index].ob_state;
	int res=0;
	GRECT r1, rdraw;
	rc_set( &rdraw, x, y, w, h);

	mt_graf_mouse( M_OFF, 0L, app->aes_global);
	mt_WindXGetGrect( app, win, WF_FIRSTAREAXYWH, &rdraw, &r1);
	while( r1.g_w && r1.g_h) {
		if ( rc_intersect( &rdraw, &r1)) {
			tree[index].ob_state = prev_state;
			res = mt_objc_change( tree, index, 0, r1.g_x, r1.g_y, r1.g_w, r1.g_h, state, TRUE, app->aes_global);
		}
		mt_WindGetGrect( app, win, WF_NEXTXYWH, &r1);
	}
	mt_graf_mouse( M_ON, 0L, app->aes_global);
	
	/* if the object is not on the clipping area, then mt_objc_change() is never called
	 * and tree[index].ob_state is not set to the new state. We have to do it by hand
	 */
	tree[index].ob_state = state;
	
	return res;
}

