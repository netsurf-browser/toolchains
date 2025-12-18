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
 * $Source: /cvsroot/windom/windom/src/vq_extfs.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "cookie.h"

/** test of long filename
 *
 *  @param path complet path of a file or folder
 *  @return 1 if the file system supports long file name, 0 else.
 *
 *  vq_extfs() tests if the file system supports long file name. MiNT and
 *  MagiC are concerned.
 */

int vq_extfs( char *path) {
	long n;
	
	if(	vq_magx() >= 0x500 ||
		vq_mint() >= 0x90L) {

		if( path[0] && path[1] != ':') {
			/* get the current path */
			char newpath[255];
			
			newpath[0] = Dgetdrv() + 'A';
			newpath[1] = ':';
			Dgetpath( newpath+2, 0);
			strcat( newpath, "\\");
			strcat( newpath, path);
			n = Dpathconf( newpath, 3 /*DP_NAMEMAX*/);
		} else
			n = Dpathconf( path, 3 /*DP_NAMEMAX*/);
		if( n>=31) return 1;
	}
	return 0;
}
