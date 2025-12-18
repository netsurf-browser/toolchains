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
 * $Source: /cvsroot/windom/windom/src/menu_bind.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/02/22 16:15:25 $
 *   $Revision: 1.7 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

void mt_menu_bind( APPvar *app, WINDOW *win, int item, int title) {
	BIND *bind = win?win->menu.bind:app->priv->desk->menu.bind;
	func_bindmenu cb;
	
	if( item != -1 && bind) {
		switch (bind[item].type) {
		case BIND_VAR:  /* what should we do there ??? */
			break;
		case BIND_BIT:  /* change the state of the bit and check/unckeck the menu entry */
			*(bind[item].desc.bit.var) ^= bind[item].desc.bit.bit;
			mt_MenuIcheck( app, win, item, (*(bind[item].desc.bit.var) & bind[item].desc.bit.bit)?1:0);
			break;
		case BIND_FUNC: /* invoke the callback function */
			cb = bind[item].desc.func.f.menu;
			(*cb)( win, item, title, bind[item].desc.func.data, app);
		}
	}
}
