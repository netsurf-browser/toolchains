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
 * $Source: /cvsroot/windom/windom/src/objc_free.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.11 $
 */


#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

#define PARM( ldata)	(((W_PARM *)(ldata))->wp_spec)

/* free the ob_spec field previously duplicated by mt_ObjcDup()
 * WARNING: tree[ob] shall not be a G_USERDEF object trasnformed by udlib
 */
static void free_obspec( APPvar *app, OBJECT *tree, int ob) {
	switch( tree[ob].ob_type & 0x00FF) {
	case G_TEXT:
	case G_FTEXT:
	case G_BOXTEXT:
	case G_FBOXTEXT:
		__free_obspec( app, &tree[ob]);
		free( tree[ob].ob_spec.tedinfo);
		break;
		
	case G_STRING:
	case G_BUTTON:
	case G_TITLE:
		__free_obspec( app, &tree[ob]);
		break;
	
	case G_IMAGE:
#if 0 /* FIXME: temporary disabled because there's a bug somewhere in mt_ObjcDup() */
		__free_obspec( app, &tree[ob]);
#endif
		break;
	
	case G_ICON:
#if 0 /* FIXME: temporary disabled because there's a bug somewhere in mt_ObjcDup() */
		__free_obspec( app, &tree[ob]);
#endif
		break;
	
	case G_CICON:
#if 0 /* FIXME: temporary disabled because there's a bug somewhere in mt_ObjcDup() */
		__free_obspec( app, &tree[ob]);
#endif
		break;
		
	case G_USERDEF:
		switch ( tree[ob].ob_type >> 8 ) {
		case  XCICON:
			/* TODO FIXME : nothing (cf ObjcDup) */
			break;
		case USERDRAW:
			free( (void*)tree[ob].ob_spec.userblk->ub_parm);
			free( tree[ob].ob_spec.userblk);
			break;
		}
	}
}

/**
 * @brief Release from memory a duplicated objects tree.
 *
 * @param app application descriptor,
 * @param cpy duplicated objects tree to release.
 *
 * This function releases an object tree explicitly duplicated by
 * mt_ObjcDup().
 *
 * @sa mt_ObjcDup(), mt_ObjcNDup()
 */

void mt_ObjcFree( APPvar *app, OBJECT *cpy) {
	int nb = obj_nb( cpy);
	mt_ObjcNFreeAtAddr( app, cpy, nb);
	free( cpy);
}

/**
 * @brief Release from memory a duplicated objects tree.
 *
 * @param app application descriptor,
 * @param cpy duplicated objects tree to release.
 * @param nb number of OBJECTs of the tree \a cpy to release.
 *
 * This function releases an object tree explicitly duplicated by
 * mt_ObjcNDupAtAddr().
 *
 * @sa mt_ObjcNDupAtAddr()
 */

void mt_ObjcNFreeAtAddr( APPvar *app, OBJECT *cpy, int nb) {
	int i;
	
	for( i=0; i<nb; i++) {
		if ( (cpy[i].ob_type & 0X00FF) == G_USERDEF 
		   &&( udlib_get_type(app,cpy,i) != 0 ) )
		{
			/* this is a userdefined object that has been transformed
			 * by udlib
			 */
			udlib_extfree(app,cpy,i);
			free_obspec(app,cpy,i);
		} else {
			free_obspec(app,cpy,i);
		}
	}
}
