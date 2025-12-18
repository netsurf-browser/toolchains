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
 * $Source: /cvsroot/windom/windom/src/objc_drawparent.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/**
 * @brief Draws an objet in a formular.
 *
 * @param app application descriptor,
 * @param mode type of target :
 *   - \b OC_FORM if the formular is a window,
 *   - \b OC_TOOLBAR if the formular is a toolbar,
 *   - \b OC_OBJC if the formular is a classic formular,
 * @param win window descriptor or an object tree if mode is set to \b OC_OBJC,
 * @param index object index to draw,
 * @param parent_depth the level of the parent that may be drawn. A value of 1 will draw
 *        the parent of the object, the object itself, and its children (specified by
 *        next parameter \a child_depth).
 * @param child_depth the maximum object depth to draw (a value of 1 searches only first 
 *        generation objects, a value of 2 searches up to second generation objects, 
 *        up to a maximum of 7 to search all objects), if the bit \b OC_MSG
 *        is set, a redraw event will be requested instead of a direct drawn on screen.
 * @return  0 if an error occurred or non-zero otherwise.
 *
 * @pre mt_ObjcDraw() cannot be called from a callback function used to redraw the screen
 * (that is callback functions attached to WM_REDRAW for example) because this functions
 * may lock the screen and use the AES rectangle list. To draw an object from a that kind of
 * callback function, use mt_objc_draw() (with \e win->graf.clip as clip area), or use \b OC_MSG mode.
 *
 * This function replaces the AES objc_draw() function. It is specially
 * designed to draw object in window formular (\b OC_FORM mode) or toolbar
 * (\b OC_TOOLBAR mode) but it works on classical formular 
 * (\b OC_OBJC mode) too. Using this last mode, mt_ObjcDraw() is equivalent
 * to objc_draw(). When mt_ObjcDraw() is applied on a window, the redraw is done
 * with respect to the window clipping. It works even the window formular
 * is behind an another window. 
 * 
 * \b Special \b bit \b OC_MSG
 *
 * If the \e depth parameter has its \b OC_MSG bit set to 1, objects will 
 * be drawn by sending a set of \b WM_REDRAW messages to the AES kernel 
 * (redraw will be handled by mt_EvntWindom()) instead of drawing immediatly 
 * the object.
 *
 * @sa mt_ObjcChange()
 */
int mt_ObjcDrawParent( APPvar *app, int mode, void *win, int index, int parent_depth, int child_depth) {
	int res;
	OBJECT *tree;
	GRECT rect;
	int depth;
	
	switch( mode) {
	case OC_FORM:
		tree = FORM(app,(WINDOW*)win);
		break;
	case OC_TOOLBAR:
		tree = TOOL((WINDOW*)win);
		break;
	case OC_OBJC:
		tree = (OBJECT *)win;
		break;
	case OC_MENU:
		tree = ((WINDOW*)win)->menu.root;
		break;
	default:
		return EBADRQ;
	}

#define EXTRA_WIDTH 6
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
	
	if( mode == OC_OBJC) 
		res = mt_objc_draw_grect( tree, index, depth, &rect, app->aes_global);
	else if( child_depth & OC_MSG) {
		mt_snd_msg( app, win, WM_REDRAW, rect.g_x, rect.g_y, rect.g_w, rect.g_h);
		res = E_OK;
	} else
		res = mt_ObjcWindDraw( app, win, tree, index, depth, rect.g_x, rect.g_y, 
							   rect.g_w, rect.g_h);
	
	return res;
}
