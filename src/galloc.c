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
 * $Source: /cvsroot/windom/windom/src/galloc.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include "globals.h"
#include "cookie.h"

#define MGLOBAL	0x20

/** Allocate memory in global mode
 *
 * @param size memory size required (in byte),
 * @return pointer to allocated buffer or \b NULL if memory is exhausted.
 *
 * Galloc() reserves global memory, i.e. memory which can be shared with
 * other applications. This function is used for GEM message
 * interprocess : all data shared between application should be
 * declared in global memory (or shared memory). Only MiNT with
 * memory protection is concerned.
 * 
 * For managing GEM message event pointing on shared data, WinDom
 * reserves a buffer for this purpose (WinDom kernel can use it for
 * special message) : the variable \e app->pipe (or gl_appvar.pipe)
 * points to a 256-byte buffer allocated with Galloc().
 *
 * \attention Galloc() is not a performant memory manager as malloc(), it makes a
 * direct call to Malloc()/Mxalloc() and has the same limitation of these
 * functions.
 * 
 * \par Example
 * @code
// Safe example
#include <av.h> // For VA_START constant

void send_file_to_qed( char *file) {
  char *path = Galloc( strlen(file) + 1);
  
  strcpy( path, file);
  ApplWrite( appl_find( "QED     "), VA_START, ADR(path), 0, 0, 0);
  Mfree( path);
}
// Same example, shorter but less safe
#define SEND_FILE_TO_QED(file)  AppWrite( appl_find( "QED     "), VA_START, ADR(strcpy(app.pipe,file), 0, 0, 0)
@endcode
*
*/

void *Galloc( long size) {
	long val;
	static int type_mxalloc = -1;
	
	if (type_mxalloc < 0) {
		unsigned short v = Sversion();
		if ( (v & 0xff) || (v > 0x1900) ) { /* GEMDOS > 0.19 */
			if ( get_cookie (MAGX_COOKIE, &val) || get_cookie (MiNT_COOKIE, &val))
				type_mxalloc = 2;  /* Mxalloc with MGLOBAL */
			else
				type_mxalloc = 1;  /* Mxalloc without MGLOBAL */
		} else
			type_mxalloc = 0; /* Malloc */
	}
	
	switch (type_mxalloc) {
	case 0 : return (void*)Malloc( size);
	case 1 : return (void*)Mxalloc( size, 3);
	case 2 : return (void*)Mxalloc( size, 3|MGLOBAL);
	}
	
	/* should never happen */
	return NULL;
}
