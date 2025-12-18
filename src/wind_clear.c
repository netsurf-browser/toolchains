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
 * $Source: /cvsroot/windom/windom/src/wind_clear.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.8 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/**
 * @brief clear a window workarea.
 *
 * @param app application descriptor,
 * @param win window descriptor,
 *
 * This function draws the window background i.e. a bar (typically with
 * a white color) in the workarea of a window. Color, pattern and style
 * can be parametrised via internal WinDom configuration (see mt_AppSet()).
 *
 *  This is the function invoked by the callback function mt_stdWindClear().
 *
 */

void mt_WindClear( APPvar *app, WINDOW *win) {
	short x, y, w, h;
	short xy[4];
	short vdih = win->graf->handle;

	mt_WindGet( app, win, WF_WORKXYWH, &x, &y, &w, &h);
	xy[0] = x;xy[1] = y;xy[2] = x+w-1;xy[3] = y+h-1;
	vsf_interior( vdih, CONF(app)->bgpatt);
	vsf_style( vdih, CONF(app)->bgstyle);
	if( CONF(app)->bgcolor < app->color)
		vsf_color( vdih, CONF(app)->bgcolor);
	else
		vsf_color( vdih, WHITE);
	vswr_mode( vdih, MD_REPLACE);
		
	v_bar( vdih, xy);
}

/** callback function to clear the window content
 *
 *  @param win window descriptor
 *  @param buff content of the AES message received
 *  @param app application descriptor
 *
 *  This callback function invoke mt_WindClear()
 *
 *  By default, mt_WindCreate() binds mt_stdWindClear() to \b WM_REDRAW event.
 *
 */

void __CDECL mt_stdWindClear( WINDOW *win, short buff[8], APPvar *app) {
	mt_WindClear(app,win);
}

