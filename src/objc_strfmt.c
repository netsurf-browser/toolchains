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
 * $Source: /cvsroot/windom/windom/src/objc_strfmt.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"

/**
 * @brief Troncate a string in order to fit in an object label.
 *
 * @param dest troncated string,
 * @param src string to troncate,
 * @param size new lenght of string
 *
 * This function troncate a string in order to fit in an object label.
 * If the string is too long (prior to \e size parameter), excedented
 * characters are removed at the middle of the string and tree points
 * '...' are inserted. This function is used to format correctly filename
 * or pathname in TEXT objects before displaying the formular.
 *
 */

void ObjcStrFmt( char *dest, char *src, int size) {
	int len = (int)strlen( src);
	int middle = size/2;
	char buf [255];
	
	if( len > size) {
		strncpy( buf, src, size);
		buf[middle-3] = '\0';
		strcat( buf, "...");
		strcat( buf, src + len - middle - size%2);
		buf[size]='\0';
		strcpy( dest, buf);
	} else
		strcpy( dest, src);
}
