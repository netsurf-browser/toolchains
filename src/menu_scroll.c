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
 * $Source: /cvsroot/windom/windom/src/menu_scroll.c,v $
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
 * @brief Scroll the titles of a window menu.
 *
 * @param app application descriptor,
 * @param win window descriptor having an menu,
 * @param dir scroll direction :
 *   - 1 means scroll in left direction,
 *   - 0 means scroll in right direction.
 *
 * If the variable \e windom.menu.scroll
 * in the WinDom configuration file is set to TRUE, a scroller
 * widget appears in the window menu bar: the user can
 * himself scrolls the menu titles. This function can scroll
 * menu titles.
 */

void mt_MenuScroll( APPvar *app, WINDOW *win, int dir) {
	INT16 x, y, w, h;
	OBJECT *menu = win->menu.root;

	mt_wind_get( win->handle, WF_WORKXYWH, &x, &y, &w, &h, app->aes_global);
	if( dir == 0) { /* RIGTH */
		if( menu[3+win->menu.scroll].ob_next != 2 ) {
			menu[3+win->menu.scroll].ob_flags |= HIDETREE;
			menu[3+win->menu.scroll].ob_state |= DISABLED;
			win -> menu.scroll ++;
			mt_snd_msg( app, win, WM_REDRAW, x, y, w, menu[1].ob_height);
		}
	} else {
		if( win->menu.scroll > 0) {
			win -> menu.scroll --;
			menu[3+win->menu.scroll].ob_flags &= ~HIDETREE;
			menu[3+win->menu.scroll].ob_state &= ~DISABLED;
			mt_snd_msg( app, win, WM_REDRAW, x, y, w, menu[1].ob_height);
		}	
	}
}
