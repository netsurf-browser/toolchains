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
 * $Source: /cvsroot/windom/windom/src/objc_change.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/02/26 02:56:08 $
 *   $Revision: 1.5 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/**
 * @brief Change the object state.
 *
 * @param app application descriptor,
 * @param mode type of target :
 *   - \b OC_FORM if the formular is a window,
 *   - \b OC_TOOLBAR if the formular is a toolbar,
 *   - \b OC_OBJC if the formular is a classic formular,
 * @param win window descriptor or an object tree if mode is set to \b OC_OBJC,
 * @param index object index to change,
 * @param state new object state,
 * @param redraw bit 0 indicates if object should be redrawn,
 *        \b OC_MSG resquestes a redraw by WinDom kernel.
 * @return 0 if an error occurred or non-zero otherwise.
 *
 * @pre If a redraw is requested, mt_ObjcChange() cannot be called from a 
 *      callback function because mt_EvntWindom() uses the AES rectangle list. 
 *
 * This function replaces the AES mt_objc_change() function mainfully to
 * change the object state in window formular or toolbar. If you use
 * mt_ObjcChange() with the \b NORMAL value, the extended bits of the
 * objet state (known as extended states) will be not affected. Negative
 * states (e.g. \b ~SELECTED) have effect to unset the state. See mt_ObjcDraw() 
 * for recommandation usage.
 *
 * @sa mt_ObjcDraw()
 */

int mt_ObjcChange( APPvar *app, int mode, void *win, int index, int state, int redraw) {
	OBJECT *tree;
	int res = E_OK;
	
	switch( mode) {
	case OC_FORM:
		tree = FORM(app, (WINDOW*)win);
		break;
	case OC_TOOLBAR:
		tree = TOOL((WINDOW*)win);
		break;
	case OC_OBJC:
		tree = (OBJECT *)win;
		redraw = redraw?2:3;
		break;
	case OC_MENU:
		tree = ((WINDOW*)win)->menu.root;
		break;
	default:
		return EBADRQ;
	}

	/* filter ob_state */
	if( (state & 0xFF00) == 0xFF00)  /* case of (f.e.) '~SELECTED' */
		state &= tree[ index].ob_state;
	else
		state = (tree[ index].ob_state & 0xFFC0) | (state & 0x003F);
	
	switch( redraw) {  /* TODO: replace the constants 0..3 herafter by names from gemlib (if any) */
	case 3:
	case 2:
		res = mt_objc_change( tree, index, 0, app->x, app->y, app->w, app->h, state, (redraw==2)?1:0, app->aes_global);
		break;
	case REDRAW:
		mt_ObjcWindChange( app, win, tree, index, app->x, app->y, app->w, app->h, state);
		break;
	case OC_MSG:
		tree[ index].ob_state = state;
		res = mt_ObjcDraw( app, mode, win, index,  MAX_DEPTH | OC_MSG);
		break;
	case NO_DRAW:
		res = mt_objc_change( tree, index, 0, app->x, app->y, app->w, app->h, state, 0, app->aes_global);
		break;
	}
	return res;
}

