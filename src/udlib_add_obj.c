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
 * $Source: /cvsroot/windom/windom/src/udlib_add_obj.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.4 $
 */

#include <stdlib.h>

#include "globals.h"

int udlib_add_obj( UDLIB_LIB *udlib, OBJECT *tree, int ob, int ob_type) 
{
	UDLIB_OBJ *ulobj, *c=NULL;
	
	ulobj = (UDLIB_OBJ*) malloc (sizeof(UDLIB_OBJ));
	if (ulobj == NULL) return -1;
	
	ulobj->tree = tree;
	ulobj->ob = ob;
	ulobj->ob_type = ob_type;
	
	listForEach( UDLIB_OBJ*, c, &udlib->ob_list) {
		if ((unsigned long)c->tree > (unsigned long)tree)
			break;
		if ((c->tree==tree) && (c->ob >= ob))
			break;
	}
	
	/* this object has already been transformed to a userdef... this should
	 * never happen, but in this case, it's safer to remove this object
	 */
	while ( c != (UDLIB_OBJ*)listEnd(&udlib->ob_list) && c->tree==tree && c->ob==ob) {
		UDLIB_OBJ *trash = c;
		listRemove( &c->lnk );
		c = (UDLIB_OBJ*)c->lnk.next;
		free(trash);
	}
	
	listInsert( &c->lnk, &ulobj->lnk);
	
	return 0;
}

