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
 * $Source: /cvsroot/windom/windom/src/menu_icheck.c,v $
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
 * @brief Check or uncheck an entry in a menu.
 *
 * @param app application descriptor,
 * @param win window descriptor containing a menu or \b NULL for
 *        desktop menu,
 * @param item index of menu entry,
 * @param check possible values are :
 *   - 0 : uncheck the entry,
 *   - 1 : check the entry,
 * @return 0 or a negative error.
 *
 * mt_MenuIcheck() replaces and extendes the AES mt_menu_icheck() 
 * function. It can be used on window menu or on the desktop menu 
 * (if \e win parameter is set to \b NULL). 
 *
 * Extended objects are correctly handled by this function.
 *
 */

int mt_MenuIcheck( APPvar *app, WINDOW *win, int item, int check) {
	OBJECT *menu;
	char *str;
	
	menu = ( win == NULL)? app->menu: win->menu.root;
	
	str = mt_ObjcString( app, menu, item, NULL);
	if( *str) {
		str[0] = check?'':' ';
		return 0;
	}
	return 1;
}
