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
 * $Source: /cvsroot/windom/windom/src/menu_disable.c,v $
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
 * @brief Disable the desktop menu.
 *
 * @param app application descriptor.
 * @return 0 or a negative code error.
 *
 * mt_MenuDisable() disables the desktop menu: the user cannot select items except
 * the desktop accessories. This function is used by mt_FormWindBegin(),
 * the file and font selectors.
 *
 * @sa mt_MenuEnable()
 */

int mt_MenuDisable( APPvar *app) {
	if( !app->menu) return 1;
	if( app->priv->_menu_disabled == 0) {
	int index = 3;
	
		while( ob_istype( app, app->menu, index++, G_TITLE))
			app->menu[index].ob_state = DISABLED;
		while( app->menu[ index].ob_type != G_STRING)
			index++;
		app->menu[index].ob_state = DISABLED;
		mt_menu_bar( app->menu, 1, app->aes_global);
	}
	app->priv->_menu_disabled ++;
	return 0;
}
