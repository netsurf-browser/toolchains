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
 * $Source: /cvsroot/windom/windom/src/get_appname.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stdlib.h>
#include <string.h>
#include "globals.h"

/*
 * @todo when a GEM application is launched from bash (so, probably
 *  Pexec()-launched), shel_read() returns nothing.
 */

void get_appname( APPvar *app, char *name) {
	char path[128], dum[128], *p;
	
	mt_shel_read( path, dum, app->aes_global);
	
	/* Verify if the name is OK */
	if( *path == 0 || *path == '\\') {
	  /* xaAES returns '\' then others AES return empty string */
	  mt_ApplName( app, path, app->aes_global[2]);
	  path[9] = '\0';
	  p = strchr(path, ' ');
	  if( p) *p = '\0';
	}
	/* Format name */
	p = strrchr( path, '.');
	if( p) *p = '\0';
	p = strrchr ( path, '\\');
	p = p?(p+1):path;
	strupr( p);

	/* WinDom configuration format */
	strcpy( name, "[");
	strcat( name, p);
	strcat( name, "]");	
}
