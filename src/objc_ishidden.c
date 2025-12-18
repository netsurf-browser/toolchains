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
 * $Source: /cvsroot/windom/windom/src/objc_ishidden.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/**
 * @brief tell if the object or one of its parent is hidden.
 *
 * @param app application descriptor,
 * @param mode type of target :
 *   - \b OC_FORM if the formular is a window,
 *   - \b OC_TOOLBAR if the formular is a toolbar,
 *   - \b OC_OBJC if the formular is a classic formular,
 * @param win window descriptor or an object tree if mode is set to \b OC_OBJC,
 * @param index object index to draw,
 * @return 1 if the object is hidden, 0 if the objet is not hidden, or a negative
 *         value in case of error.
 *
 */

int mt_ObjcIsHidden( APPvar *app, int mode, void *win, int index) {
	OBJECT *tree;
	
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
	
	while (index >= 0) {
		if ( tree[index].ob_flags & HIDETREE)
			return 1;
		index = obj_root( tree, index);
	}
	
	return 0;
}
