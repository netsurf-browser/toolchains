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
 * $Source: /cvsroot/windom/windom/src/udlib_list.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include <string.h>
#include <ldg.h>

#include "globals.h"

static short zzlen( const char * zzstring)
{
	short retlen = 0;
	short slen;
	
	if (zzstring == NULL)
		return 0;
	
	while (*zzstring) {
		slen = strlen(zzstring);
		retlen += slen + 1;
		zzstring += slen + 1;
	}
	
	return retlen + 1;
}

static char * zznext( char *zzstring)
{
	if (zzstring == NULL)
		return NULL;
	
	zzstring += strlen(zzstring) + 1;
	
	if (*zzstring == '\0')
		return NULL;
	
	return zzstring;
}

int udlib_ConfSetLib( APPvar *app, const char *libname)
{
	if (CONF(app)->udlib_filenames) {
		free(CONF(app)->udlib_filenames);
		CONF(app)->udlib_filenames = NULL;
	}
	
	return udlib_ConfAddLib( app, libname, 1);
}

int udlib_ConfAddLib( APPvar *app, const char *libname, short head)
{
	short len;
	short total_len;
	char *zzstring;
	
	len = strlen(libname) + 1;
	total_len = zzlen( CONF(app)->udlib_filenames );
	if ( total_len > 0)
		total_len--;  /* do not count the very last nul character at the end of the zzstring */
	
	zzstring = (char *) malloc (total_len + len + 1);
	if (zzstring == NULL)
		return -1;
	
	if (head) {
		memcpy( zzstring, libname, len);
		memcpy( zzstring + len, CONF(app)->udlib_filenames, total_len);
	} else {
		memcpy( zzstring, CONF(app)->udlib_filenames, total_len);
		memcpy( zzstring + total_len, libname, len);
	}
	zzstring[total_len + len] = 0;
	
	free (CONF(app)->udlib_filenames);
	CONF(app)->udlib_filenames = zzstring;
	
	return 0;
}

char * udlib_ConfGetFirstLib( APPvar *app)
{
	return CONF(app)->udlib_filenames;
}

char * udlib_ConfGetNextLib( char *prev)
{
	return zznext(prev);
}
