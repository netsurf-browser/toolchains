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
 * $Source: /cvsroot/windom/windom/src/wind_lst.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.5 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/*
 * Add a WINDOW structure to the list
 */
 
void mt_AddWindow( APPvar *app, WINDOW* win) {
	WINDOW* winptr = app->wglb.first;
	
	if (winptr == NULL)
		app->wglb.first = win;
	else {
		while(winptr->next)
			winptr = winptr->next;
		winptr->next = win;
	}
}

/*
 * Remove a WINDOW from the list
 */

void mt_RemoveWindow( APPvar *app, WINDOW* win) {
	WINDOW* winptr = app->wglb.first;
	
	if (winptr == win)  	/* case win = wglb.first (1st element of the list) */
		app->wglb.first = winptr->next;
	else {
		while (winptr->next) /* look for "win" in the list */
			if (winptr->next == win)
				break;
			else
				winptr = winptr->next;
				
		if (winptr->next == win)
			winptr->next = winptr->next->next;
	}
}
