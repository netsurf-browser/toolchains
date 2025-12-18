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
 * $Source: /cvsroot/windom/windom/src/objc_char.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.2 $
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"

/**
 * @brief Get and set the character of an object.
 *
 * @param app application descriptor,
 * @param tree address of an objects tree,
 * @param idx object index,
 * @param put new character (from 0 to 255) or -1 to inquire current value, 
 * @return character of the object or a negative error code.
 *
 * mt_ObjcChar() provides an universal acces of the character of any object.
 * The object can be a charbox, an icon, a WinDom extended object containing
 * a character. If the object has not a character,
 * the function does nothing. If a \b -1 value is given to \b put
 * parameter, mt_ObjcChar() returns the character of the object.
 */

int mt_ObjcChar( APPvar *app, OBJECT *tree, int idx, int put) {
	USERBLK *user;
	int retval;

	switch ( tree[idx].ob_type & 0x00FF) {
	case G_BOXCHAR:
		if( put >= 0)
			tree[idx].ob_spec.obspec.character = put;
		return( tree[idx].ob_spec.obspec.character);
	case G_ICON:
		if( put >= 0)
			tree[idx].ob_spec.iconblk->ib_char = put;
		return( tree[idx].ob_spec.iconblk->ib_char);
	case G_CICON:
		if( put >= 0)
			tree[idx].ob_spec.ciconblk->monoblk.ib_char = put;
		return (tree[idx].ob_spec.ciconblk->monoblk.ib_char);
	case G_USERDEF:
		/* is this USERDEF object an object from a "userdef library" ? */
		if ( udlib_char(app,tree,idx,put,&retval) == 0 ) {
			/* No, maybe it's a CICON... */
			if ( ((tree[idx].ob_type & 0xFF00)>>8) == XCICON) {
				user = tree[idx].ob_spec.userblk;
				if( put >= 0)
					((ICONBLK *)user->ub_parm)->ib_char = put;
				return ((ICONBLK *)user->ub_parm)->ib_char;
			}
		} else {
			/* Yes, and retval contains the value to return */
			return retval;
		}
		break;
	}
	return -1;
}
