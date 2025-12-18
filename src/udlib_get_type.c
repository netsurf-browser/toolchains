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
 * $Source: /cvsroot/windom/windom/src/udlib_get_type.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>

#include "globals.h"

/* return the original ob_type of the object, before transformation to
 * an userdef object, or 0 if none of the loaded userdef lib have 
 * transformed this object to a userdef object
 */
int udlib_get_type( APPvar *app, OBJECT *tree, int ob)
{
	UDLIB_LIB *udlib;
	UDLIB_OBJ *ulobj;
	
	listForEach( UDLIB_LIB*, udlib, &(app->priv->udef_lib)) {
		ulobj = udlib_objfind( udlib, tree, ob);
		if (ulobj) return ulobj->ob_type;
	}
	
	return 0;	
}
