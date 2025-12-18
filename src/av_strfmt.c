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
 * $Source: /cvsroot/windom/windom/src/av_strfmt.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stdlib.h>
#include <string.h>
#include "globals.h"

/**  
 * @brief Quote or unquote filenames (utility for VA-protocol, Drag 'n drop, ...)
 *
 * @param mode action to perform :
 *             - 1 to UNQUOTE the filenames (if needed),
 *             - 0 to QUOTE the filename (if needed),
 * @param src string that contains the filename(s). This string will not
 *            be modified.
 * @return a string that contains \e src with filenames unquoted, or filemane
 *         quoted (depending of \e mode value). This string is allocated using 
 *         the standard malloc() function and should be released after use.
 *
 * With some modern filesystems, the filename can contain space
 * characters. In this case, the AV protocol ask to ''quote'' a
 * filename, i.e. quote characters are added at the beginning and
 * the end of the filename. So, it is very important to use
 * this function all the time when you send or received AV request
 * with filename parameters. Please note, some desktops use the quoting 
 * filename with drag'n drop and argv protocol.
 *
 *  QUOTING: if "src" contains at least one space character " ", quotes will be add
 *  at the begin and at the end of the string.
 * <pre>
 *  ex:  [filename\\0]   --> [filename\\0]  (no changed)
 *  ex:  [filename with spaces\\0] --> ['filename with spaces'\\0]
 * </pre>
 *
 *  UNQUOTING: the returned string will contain the filenames without quotes. The NULL
 *  character is used to separate filenames. 2 successives NULL-char are used to end 
 *  the list
 * <pre>
 *  ex:  [filename1 'file name 2' filename3] --> [filename1\\0file name 2\\0filename3\\0\\0]
 * </pre>
 *
 */

char *AvStrfmt( int mode, char *src) {
	char *dst, *r;
	
	r = dst = (char*)malloc(sizeof(char)*strlen(src)+3);
	if( mode) {
		char *p, *q;

		p = src;
		do {
			q = NULL;
			while( *p == ' ') p++;
			if( *p == '\'')
				q = strchr( ++p, '\'');
			else  
				q = strchr( p, ' ');
			if( q) *q++ = '\0';
			strcpy( dst, p);
			dst += strlen( dst)+1;
			p = q;
		} while (q);
		*dst = '\0';
	} else if( strchr( src, ' ')) {
		*dst = '\'';
		strcpy( dst+1, src);
		strcat( dst, "\'");
	}
	return r;
}
