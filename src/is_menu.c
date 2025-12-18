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
 * $Source: /cvsroot/windom/windom/src/is_menu.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/* check if the mouse pointer is on a window's menubar
 * (internal)
 */

int is_menu( APPvar *app, WINDOW *win) {
	INT16 x, y;

	mt_objc_offset( win -> menu.root, 1, &x, &y, app->aes_global);
	if( (app->evnt.mx > x) && (app->evnt.mx < x + (win->menu.root)[1].ob_width - 1)
		&& (app->evnt.my > y) && (app->evnt.my < y + (win->menu.root)[1].ob_height - 1))
		return 1;
	return 0;
}

