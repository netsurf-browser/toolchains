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
 * $Source: /cvsroot/windom/windom/src/udlib_exit.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>

#include "globals.h"
#include "cicon.h"

/** free all userdef libraries
 *
 *  @param app
 *
 *  @return 0.
 *
 *  This function will free all UDLIB_OBJ objects still allocated
 *  (properly, thru udlib_extfree()). Then the garbage is called to
 *  free all the userdef libraries (because they are now all unused).
 */

int udlib_exit (APPvar *app)
{
	UDLIB_LIB *udlib;
	UDLIB_OBJ *ulobj;
	
	while ( (udlib = (UDLIB_LIB*) listFirst(&app->priv->udef_lib)) != NULL ) {

#if 1
		if ( ! listIsEmpty(&udlib->ob_list))
			mt_form_alert( 1, "[1][udlib_exit:|some extended object|were not free-ed!][continue]",app->aes_global);
#endif

		while (  (ulobj = (UDLIB_OBJ*)listFirst(&udlib->ob_list)) != NULL )
			udlib_extfree(app,ulobj->tree,ulobj->ob);
		
		udlib_garbage(app);
	}
	
	return 0;
}
