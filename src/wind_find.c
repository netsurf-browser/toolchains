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
 * $Source: /cvsroot/windom/windom/src/wind_find.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.6 $
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"


/**
 * @brief Find a window.
 *
 * @param app application descriptor,
 * @param mode indicates how find the window. Possible values are :
 *   - \b WDF_NAME :  find a window by name, a string parameter is attented,	
 *   - \b WDF_INFO: find a window by info string, a string parameter is attented,
 *   - \b WDF_ID: find a window by GEM handle, an integer value is attented,
 *   - \b WDF_MENU: find a window by its OBJECT menu tree, a pointer is attented,
 *   - \b WDF_TOOL: find a window by its OBJECT toolbar tree, a pointer is attented,
 *   - \b WDF_DATA: find a window by data (using mt_DataSearch()), a pointer is attented,
 * @param ... parameter type depend on previous values of \e mode
 * @return window descriptor found or \b NULL otherwise.
 */

WINDOW *mt_WindFind( APPvar *app, int mode, ...) {
	int id=0;
	void *adr=NULL;
	long magic=0L;
	va_list args;
	WINDOW *win;
	
	va_start( args, mode);
	if( mode == WDF_ID)
		id = va_arg( args, int);
	else if( mode == WDF_DATA)
		magic = va_arg( args, long);
	else
		adr = va_arg( args, void*);
		
	va_end( args);
	for( win = app->wglb.first; win; win = win->next) {
		switch ( mode) {
		case WDF_NAME:
			if( win->name && !strcmp(win->name, (char *)adr) )
				return win;
			break;
		case WDF_INFO:
			if( win->info && !strcmp(win->info, (char *)adr) )
				return win;
			break;
		case WDF_ID:
			if( win->handle == id)
				return win;
			break;
		case WDF_MENU:
			if( win->menu.real == adr)
				return win;
			break;
		case WDF_FORM:
			
			break;
		case WDF_TOOL:
			if( win->tool.real == adr)
				return win;
			break;
			
		case WDF_DATA:
			if( mt_DataSearch( app, win, magic))
				return win;
			break;
		}
		
	}
	return NULL;
}
