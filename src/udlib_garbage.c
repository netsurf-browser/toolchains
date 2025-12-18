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
 * $Source: /cvsroot/windom/windom/src/udlib_garbage.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include <ldg.h>

#include "globals.h"

/** unload a userdef library
 *
 *  @param app
 *  @param udlib
 *
 */

static void unload_lib( APPvar *app, UDLIB_LIB *udlib)
{	
	if (udlib->libexit)
		(* udlib->libexit)( udlib->env);

	if (udlib->type == udlib_ldg)
		ldg_close( (LDG*) udlib->udlib_descriptor, app->aes_global);

	if (udlib->env->fixcicon.fix) {
		mt_RsrcFreeCicon( app, (XRSRCFIX*)udlib->env->fixcicon.fix);
		free(udlib->env->fixcicon.fix);
	}

	free(udlib->env);
	free(udlib);
}

/** free unused userdef libraries
 *
 *  @param app
 *  @return the number of libraries free-ed.
 *
 */

int udlib_garbage( APPvar *app)
{
	UDLIB_LIB *udlib, *trash;
	int nlib = 0;
	
	udlib = (UDLIB_LIB*) listFirst( &(app->priv->udef_lib) );
	while (udlib) {
		trash = udlib;
		udlib = (UDLIB_LIB*)listNext(&udlib->lnk);
		
		if ( listIsEmpty( &trash->ob_list) ) {
			listRemove(&trash->lnk);
			unload_lib(app,trash);
			nlib++;
		}
	}
		
	return nlib;

}
