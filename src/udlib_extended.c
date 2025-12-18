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
 * $Source: /cvsroot/windom/windom/src/udlib_extended.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>

#include "globals.h"

int udlib_extended( APPvar *app, OBJECT *tree, int ob, int mode)
{
	UDLIB_LIB *udlib;
	long retval = 0;
	short ob_type = tree[ob].ob_type;
	
	if (app->priv->udlib_garbage <= 0) {
		/* force the loading of all the userdef libraries */
		udlib_reinit(app);
		/* set a flag to remember windom to garbage the unused libraries */
		app->priv->udlib_garbage = 1 ;
	}
	
	listForEach( UDLIB_LIB*, udlib, &(app->priv->udef_lib)) {
		retval = (*udlib->objc_extended)(tree,ob,mode,udlib->env);
		
		if (retval > 0) {
			/* TODO: enhancement of the USERDEF_LIB API: if the object is
			 * not a G_USERDEF but the return value is >0, then the object
			 * has been "transformed" by the lib and shall not go thru other libs.
			 * This may be the case when a lib transform "windom radio buttons" to
			 * "magic" radio buttons (if magic is running), or doesn't transform
			 * anything is the AES supports all windom extended objects
			 */
			retval = udlib_add_obj(udlib,tree,ob,ob_type);
			if (retval < 0) {
				(*udlib->objc_extfree)(tree,ob,udlib->env);
				return retval;
			}
			return 1;
		}
	}
	
	return 0;
}
