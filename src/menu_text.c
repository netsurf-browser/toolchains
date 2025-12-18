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
 * $Source: /cvsroot/windom/windom/src/menu_text.c,v $
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
 * @brief Change the text of a menu entry.
 *
 * @param app application descriptor,
 * @param win window descriptor containing a menu or \b NULL for
 *        desktop menu,
 * @param item index of menu entry,
 * @param txt new entry label.
 * @return 0 or a negative error.
 *
 * mt_MenuText() is the WinDom equivalent of the AES mt_menu_text() function. It
 * is used to change the text of an entry (or a title) in a window menu or in the desktop
 * menu. mt_MenuText() should be always used instead of ObjcString().
 *
 * Extended objects are correctly handled by this function.
 *
 */

int mt_MenuText( APPvar *app, WINDOW *win, int item, char *txt) {
	OBJECT *menu;
	char *str;
	
	menu = ( win == NULL)? app->menu: win->menu.root;
	str= mt_ObjcString( app, menu, item, txt);
	return (*str?1:0);
}
