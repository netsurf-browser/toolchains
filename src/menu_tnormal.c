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
 * $Source: /cvsroot/windom/windom/src/menu_tnormal.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/02/25 03:14:53 $
 *   $Revision: 1.6 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/**
 * @brief Hilight or unhilight a title in a menu.
 *
 * @param app application descriptor,
 * @param win window descriptor containing a menu or \b NULL for
 *        desktop menu,
 * @param title index of title menu,
 * @param mode possible values are :
 *   - 0 : hilights the title,
 *   - 1 : unhilights the title.
 * @return 0 or a negative error.
 *
 * mt_MenuTnormal() replaces and extendes the AES mt_menu_tnormal() 
 * function. It can be used on window menu or on the desktop menu 
 * (if \e win parameter is set to \b NULL). 
 * mt_MenuTnormal() it usually used to unhilight a menu entry
 * when a \b MN_SELECTED message occurs.
 *
 */

int mt_MenuTnormal( APPvar *app, WINDOW *win, int title, int mode) {
	return mt_WindSet( app, win, WF_MENUTNORMAL, title, mode, 0, 0);
}

