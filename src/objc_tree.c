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
 * $Source: /cvsroot/windom/windom/src/objc_tree.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stdlib.h>
#include "globals.h"

/**
 * @brief Returns the objects tree hosted by a window.
 *
 * @param app application descriptor,
 * @param mode type of host :
 *   - \b OC_FORM objects tree is a window formular,
 *   - \b OC_TOOLBAR objects tree is a window toolbar,
 *   - \b OC_MENU objects tree is a window menu,
 * @param win host window descriptor,
 * @return address of the objects tree or \b NULL if an error occures.
 */

OBJECT *mt_ObjcTree( APPvar *app, int mode, WINDOW *win) {	
	switch( mode) {
	case OC_FORM:
		return  FORM(app,win);
	case OC_TOOLBAR:
		return TOOL(win);
	case OC_MENU:
		return win->menu.root;
	}
	return NULL;
}
