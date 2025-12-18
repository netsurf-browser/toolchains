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
 * $Source: /cvsroot/windom/windom/src/strdup.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.4 $
 */


#include <stdlib.h>
#include <string.h>

/*
 * strdup: return a duplicate of a string
 * Written by Eric R. Smith and placed in the public domain.
 *
 * This function replace the (very) buggy function provided
 * by sozobonx/xdlib.
 *
 * this module is only compiled and linked for sozobonx target
 */

char * strdup(s)
	const char *s;
{
	char *dup;

	dup = (char *) malloc(strlen(s)+1);
	if (dup)
		strcpy(dup, s);
	return dup;
}
