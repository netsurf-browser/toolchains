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
 * $Source: /cvsroot/windom/windom/src/objc_str3d.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"
#include "cookie.h"

/**
 * @brief Transform 3D FTEXT MagiC objects.
 *
 * @param app application descriptor,
 * @param tree objects tree to transform.
 *
 * The function transform FTEXT objects having a 15-bit state 
 * (this state is STATE3D supported by MagiC) in order to give
 * us a 3D look like in MagiC.
 *
 */

void mt_ObjcStr3d( APPvar *app, OBJECT *tree) {
	int res;
		
	if( !get_cookie( MAGX_COOKIE, NULL)) {
		res = 0;
		while( !(tree[res].ob_flags & LASTOB)) {
			if( tree[res].ob_type == G_FTEXT &&
				!(tree[res].ob_state & (1<<15))) {
				OBJC_COLORWORD *color;
			
				tree[res].ob_state |= SELECTED;
				tree[res].ob_flags |= (1<<9);
				tree[res].ob_flags |= (1<<10);
				color = (OBJC_COLORWORD*)&tree[res].ob_spec.tedinfo->te_color;
				color -> fillc = 8;
			}
			res ++;
		}
	} else {
		/*  TODO */
		/* Editable FLAG10 ~FLAG9 */
		
	}
}
