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
 * $Source: /cvsroot/windom/windom/src/udlib_objfind.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.4 $
 */

#include <stdlib.h>

#include "globals.h"

UDLIB_OBJ * udlib_objfind( UDLIB_LIB *udlib, OBJECT *tree, int ob) 
{
	UDLIB_OBJ *c;
	
	listForEach( UDLIB_OBJ*, c, &udlib->ob_list) {
		if ((unsigned long)c->tree > (unsigned long)tree)
			return NULL; /* not found */
		if ((c->tree==tree) && (c->ob == ob))
			return (UDLIB_OBJ*) c;
	}
	
	return NULL;
}
		
