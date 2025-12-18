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
 * $Source: /cvsroot/windom/windom/src/menu_enable.c,v $
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
 * @brief Enable the desktop menu.
 *
 * @param app application descriptor.
 * @return O or a negativ code error.
 *
 * mt_MenuEnable() enables the desktop menu previously disabled by
 * mt_MenuDisable(). This function is used by mt_FormWindEnd(), the
 * file and font selectors.
 * @sa mt_MenuDisable()
 */

int mt_MenuEnable( APPvar *app) {
	if( !app->menu) return 1;
	if( app->priv->_menu_disabled == 1)
	{
	int index = 3;
	
		while( ob_istype( app, app->menu, index++, G_TITLE))
			app->menu[index].ob_state = NORMAL;
		while( app->menu[ index].ob_type != G_STRING)
			index++;
		app->menu[index].ob_state = NORMAL;
		mt_menu_bar( app->menu, 1, app->aes_global);
	}
	if( app->priv->_menu_disabled>0)
		app->priv->_menu_disabled --;
	return 0;
}

