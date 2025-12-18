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
 * $Source: /cvsroot/windom/windom/src/objc_string.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"

/**
 * @brief Get and set the label of an object.
 *
 * @param app application descriptor,
 * @param tree address of an objects tree,
 * @param idx object index,
 * @param put new label or NULL, 
 * @return address of the object label.
 *
 * mt_ObjcString() provides an universal acces of the label (text) of any object.
 * The object can be a button, a string, an icon, a WinDom extended object containing
 * a label. If the object has not text,
 * the function does nothing. If a \b NULL value is given to \b put
 * parameter, mt_ObjcString() returns the address of the object text. The text
 * returned can be read or modified with respect of the buffer size : @code
 printf( "Object i : %s\n", ObjcString( tree, i, NULL), i);
 strcpy( ObjcString( tree, i, NULL), "New text");
@endcode
 * It is possible to define a new buffer for the object: @code
 char txt[120] = "New text";
 ObjcString( tree, i, txt); 
@endcode
 *
 * mt_ObjcString() should never be used to change the text of an menu item. For
 * that purpose, use mt_MenuText() instead of mt_ObjcString().
 *
 * @sa ObjcStrCpy(), ObjcStrFmt()
 */

char *mt_ObjcString( APPvar *app, OBJECT *tree, int idx, char *put) {
	static char p[]="";
	USERBLK *user;
	char *retval;

	switch ( tree[idx].ob_type & 0x00FF) {
	case G_BUTTON:
	case G_STRING:
	case G_TITLE:
		if( put)
			tree[idx].ob_spec.free_string = put;
		return( tree[idx].ob_spec.free_string);
	case G_TEXT:
	case G_BOXTEXT:
	case G_FTEXT:
	case G_FBOXTEXT:
		if( put)
			tree[idx].ob_spec.tedinfo->te_ptext = put;
		return (tree[idx].ob_spec.tedinfo->te_ptext);
	case G_ICON:
	case G_CICON:
		if( put)
			tree[idx].ob_spec.iconblk->ib_ptext = put;
		return (tree[idx].ob_spec.iconblk->ib_ptext);
	case G_USERDEF:
		/* is this USERDEF object an object from a "userdef library" ? */
		if ( udlib_string(app,tree,idx,put,&retval) == 0 ) {
			/* No, maybe it's a CICON... */
			if ( ((tree[idx].ob_type & 0xFF00)>>8) == XCICON) {
				user = tree[idx].ob_spec.userblk;
				if( put)
					((ICONBLK *)user->ub_parm)->ib_ptext = put;
				return ((ICONBLK *)user->ub_parm)->ib_ptext;
			}
		} else {
			/* Yes, and retval contains the value to return */
			return retval;
		}
		break;
	}
	return (p);
}
