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
 * $Source: /cvsroot/windom/windom/src/objc_wdrawparent.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.3 $
 */


#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/**
 * @brief Draw any object (and its parents) in any window.
 *
 * @param app application descriptor,
 * @param win targeted window descriptor,
 * @param tree objects tree containing the object to draw,
 * @param index index of object to draw in \e tree,
 * @param child_depth level of redraw in children hierarchy,
 * @param parent_depth level of redraw in parent hierarchy,
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

int mt_ObjcWindDrawParent( APPvar *app, WINDOW *win, OBJECT *tree, 
						   int index, int parent_depth, int child_depth,
						   int xclip, int yclip, int wclip, int hclip) {
	GRECT rect;
	GRECT clip;
	int depth;
	
#define EXTRA_WIDTH 0
	mt_objc_offset( tree, index, &rect.g_x, &rect.g_y, app->aes_global);
	rect.g_x -= EXTRA_WIDTH;
	rect.g_y -= EXTRA_WIDTH;
	rect.g_w  = tree[index].ob_width + 2 * EXTRA_WIDTH;
	rect.g_h  = tree[index].ob_height + 2 * EXTRA_WIDTH;
	
	for (depth = parent_depth; depth > 0 && index > 0; depth--) {
		index = obj_root( tree, index);
	}
	
	/* compute depth from parent object */
	depth = (parent_depth - depth) + child_depth;
	if (depth > MAX_DEPTH) depth = MAX_DEPTH;
	
	clip.g_x = xclip;
	clip.g_y = yclip;
	clip.g_w = wclip;
	clip.g_h = hclip;
	
	if ( rc_intersect( &rect, &clip))
		return mt_ObjcWindDraw( app, win, tree, index, depth, clip.g_x, clip.g_y, clip.g_w, clip.g_h);
	
	return 0;
}
