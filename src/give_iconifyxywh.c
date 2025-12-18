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
 * $Source: /cvsroot/windom/windom/src/give_iconifyxywh.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.7 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"
#include "cookie.h"

/*  this function returns position for next iconified window
 */

/** give a valid position where iconify a window
 *
 *  @param app application descriptor
 *  @param x,y,w,h position and size of the icon window
 *
 *	 This function can be used if one wants iconify artificially
 *	 a window. The function give valid parameters for mt_WindSet()
 *	 with the \b WF_ICONIFY mode :
@code
	{
		short x,y,w,h;
		give_iconify( win, &x, &y, &w, &h);
		WindSet( win, WF_ICONFY, x, y, w, h);
	}
@endcode
 *   
 *	Depending the OS, the way to compute the iconify position
 *	is different:
 *	- with AES older then 4.1, WinDom uses its own icon
 *	  position. Icon placement is local to the application.
 *	- with system handling iconify (AES >= 4.1, MagiC),
 *	  give_iconifyxywh()  returns special values (-1 for
 *	  each component). These values, given to
 *	  WindSet(WF_ICONIFY), forces the screen manager to place itself
 *	  the icon.
 *
 *	With PlainTOS, it is not necessary to have an iconify mechanism
 *	because mt_EvntWindom() offers to the user to iconify a window when
 *	the smaller widget is not present (by shift-clicking the closer
 *	widget).
 */

	
void mt_give_iconifyxywh( APPvar *app, INT16 *x, INT16 *y, INT16 *w, INT16 *h) {
	int pos=0, pmax;
	
	if( app->aes4 & AES4_ICONIFY) {
		*x = *y = *w = *h = -1;
	} else {
		pmax = app->w/CONF(app)->wicon;
		for(; app->priv->__iconlist[ pos]; pos ++);
		*x = app->x + (pos % pmax)*CONF(app)->wicon;
		*y = app->y + app->h -(pos / pmax + 1)*CONF(app)->hicon;
		*w = CONF(app)->wicon;
		*h = CONF(app)->hicon;
	}
}

