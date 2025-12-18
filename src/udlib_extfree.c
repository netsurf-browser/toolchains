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
 * $Source: /cvsroot/windom/windom/src/udlib_extfree.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>

#include "globals.h"

int udlib_extfree( APPvar *app, OBJECT *tree, int ob)
{
	UDLIB_LIB *udlib;
	long retval = 0;
	
	listForEach( UDLIB_LIB*, udlib, &(app->priv->udef_lib)) {
		retval = udlib_rm_obj(udlib,tree,ob);
		if (retval > 0) {
			(*udlib->objc_extfree)(tree,ob,udlib->env);
			if ( listIsEmpty(&udlib->ob_list))		/* the lib is no more used       */
				app->priv->udlib_garbage = -1;	/* try a garbage in a near futur */
			return 1;
		}
	}
	
	return 0;
}
