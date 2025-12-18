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
 * $Source: /cvsroot/windom/windom/src/menu_bar.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/02/22 16:15:25 $
 *   $Revision: 1.5 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"


/**
 * @brief Install the desktop menu.
 *
 * @param app application descriptor,
 * @param menu objects tree standing for the menu,
 * @param mode possible values are :
 *   - 1 for installing the menu,
 *   - 0 for removing the menu,
 * @return 0 or a negative code error.
 *
 * @pre Desktop menu should not contain any G_USERDEF objects (or extended
 * object types) because some AES does not support correctly userdefs.
 *
 * mt_MenuBar() remplaces the AES mt_menu_bar() function which
 * should never be used in the WinDom environment.
 *
 * @sa mt_MenuTnorma(), mt_MenuIcheck(), mt_MenuText().
 */

int mt_MenuBar( APPvar *app, OBJECT *menu, int mode) {
	short res;
	
	if( mode) {
		/* set menu (mode != 0) */
		app->menu = mt_ObjcDup( app, menu, NULL);
		objc_extfree(app,app->menu);
		res = mt_menu_bar( app->menu, mode, app->aes_global);

		app->priv->desk->menu.scroll = 0;
	} else {	
		/* remove menu (mode == 0) */
		res = mt_menu_bar( app->menu, mode, app->aes_global);
		mt_ObjcFree( app, app->menu);
		app->menu = menu = NULL;
	}

	/* copy the data to the app->priv->desk WINDOW* structure */
	app->priv->desk->menu.root = app->menu;
	app->priv->desk->menu.real = menu;
	return res;
}

