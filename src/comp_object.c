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
 * $Source: /cvsroot/windom/windom/src/comp_object.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/01/03 14:05:47 $
 *   $Revision: 1.7 $
 */

#include "globals.h"

static
void __CDECL comp_aes_object_redraw( COMPONENT *c, long *buff, APPvar *app ) {
	OBJECT *tree = (OBJECT*)mt_CompDataSearch(app, c, CDT_OBJECT);
	LGRECT work;

	/* get the work rectangle... */
	mt_CompGetLGrect(app, c, WF_WORKXYWH, &work);

	/* place the OBJECT (to the original COMPONENT's coordinates) */
	tree->ob_x = work.g_x;
	tree->ob_y = work.g_y;
	tree->ob_width = work.g_w;
	tree->ob_height = work.g_h;

	/* here we can finaly clip by the WM_REDRAW message grect */
	if ( !rc_lintersect( (LGRECT*)&buff[4], &work ) ) return;

	mt_objc_draw( tree, 0, 8, work.g_x, work.g_y, work.g_w, work.g_h, app->aes_global );
}
 
static
void __CDECL comp_aes_object_dstry( COMPONENT *c, long *buff, APPvar *app ) {
	mt_ObjcFree( app, (OBJECT*)mt_CompDataSearch(app, c, CDT_OBJECT) );
}


static
void __CDECL comp_aes_object_enable( COMPONENT *c, long *buff, APPvar *app ) {
	((OBJECT*)mt_CompDataSearch(app, c, CDT_OBJECT))->ob_state &= ~OS_DISABLED;
}

static
void __CDECL comp_aes_object_disable( COMPONENT *c, long *buff, APPvar *app ) {
	((OBJECT*)mt_CompDataSearch(app, c, CDT_OBJECT))->ob_state |= OS_DISABLED;
}

COMPONENT *comp_objc_create( APPvar *app, OBJECT *o, short type, long size, short flex ) {
	COMPONENT *c = mt_CompCreate( app, type, size, flex );

	/* TOS 4.04 have problems working with a RSC subtree like if it was
	 * the root of that tree (perhaps ob_next links causes that?)
	 *
	 * -> copy the single OBJECT we need */
	OBJECT *oc = mt_ObjcNDup( app, o, NULL, 1);
	oc->ob_next = oc->ob_head = oc->ob_tail = -1;

	mt_CompDataAttach( app, c, CDT_OBJECT, oc );
	mt_CompEvntAttach( app, c, WM_DESTROY, comp_aes_object_dstry);
	mt_CompEvntAttach( app, c, WM_REDRAW, comp_aes_object_redraw );

	mt_CompEvntAttach( app, c, CM_GETFOCUS, comp_aes_object_enable );
	mt_CompEvntAttach( app, c, CM_LOSEFOCUS, comp_aes_object_disable );
	return c;
}

