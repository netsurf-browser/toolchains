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
 * $Source: /cvsroot/windom/windom/src/globals.c,v $
 * Module : global variables
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.11 $
 */


#include "globals.h"

#define VERSION ((__WINDOM_MAJOR__ / 10) << 12) | ((__WINDOM_MAJOR__ % 10) << 8) | \
                ((__WINDOM_MINOR__ / 10) << 4 ) |  (__WINDOM_MINOR__ % 10)

#define RELEASE  __WINDOM_REVISION__

struct w_version  WinDom = 
#ifdef __PUREC__
			{ VERSION, RELEASE, __DATE__, __TIME__, "Pure C", __PUREC__};
#else
#ifdef __GNUC__
			{ VERSION, RELEASE, __DATE__, __TIME__, "Gnu C", __GNUC__};
#else
#ifdef __SOZOBONX__ 
			{ VERSION, RELEASE, __DATE__, __TIME__, "Sozobon X", __SOZOBONX__};

#endif
#endif
#endif

/* EOF */
