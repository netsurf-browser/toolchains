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
 * $Source: /cvsroot/windom/windom/src/wind_handle.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.4 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/**
 * @brief Find a window descriptor by its AES handle.
 *
 * @param app application descriptor,
 * @param handle AES window handle,
 * @return pointer to the matching WINDOW descriptor or \n NULL 
 *         if descriptor not found.
 *
 * This function converts a GEM window handle in a WinDom window
 * descriptor. If the window is not found, mt_WindHandle() returns
 * a \b NULL value (the window does not exist or belongs to an another
 * application). This function is often used to analyse a GEM
 * message or with the AES mt_wind_find() function.
 *
 * @sa mt_wind_find(), mt_WindFind()
 */

WINDOW* mt_WindHandle( APPvar *app, int handle)
{
	register WINDOW *win;

	for(win = app->wglb.first; win; win = win->next)
		if (win->handle == handle)
			break;
	return win;
}
