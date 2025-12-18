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
 * $Source: /cvsroot/windom/windom/src/scrap_txtwrite.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"

/* Copy a string to the GEM clipboard */

int scrap_txt_write( APPvar *app, char *str) {
	char scrpdir[255];
	FILE *fp;
	
	if( !mt_scrp_read( scrpdir, app->aes_global))
		return 0;
	strcat( scrpdir, "\\scrap.txt");
	fp = fopen( scrpdir, "w");
	if( !fp) return  0;
	fwrite( str, sizeof(char), strlen(str)+1, fp);
	fclose( fp);
	return 1;
}
