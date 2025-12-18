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
 * $Source: /cvsroot/windom/windom/demo/global.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/02/14 20:57:15 $
 *   $Revision: 1.3 $
 */

#include <stddef.h> /* for NULL definition */
#include <windom.h>

#ifdef __SOZOBON__
long _STKSIZ = 32000;
#endif

struct _param {
	OBJECT *desk;
	WINDOW *winuser;
	int setup;
	int	FrameWidth;
	int FrameColor;
} param = { NULL, NULL, 0, 0, LWHITE};

void *rsc = NULL;
OBJECT **rsc_trindex = NULL;
short rsc_ntree = 0;
