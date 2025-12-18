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
 * $Source: /cvsroot/windom/windom/src/objc_edit.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/06/19 18:48:01 $
 *   $Revision: 1.8 $
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "scancode.h"

/*  objc_edit-like function that works for standard editable object, and for
 *  extended editable object (XEDIT)
 */

/**
 * @brief Control texte edition of EDITABLE object in formular.
 *
 * @param app application descriptor,
 * @param mode type of target :
 *   - \b OC_FORM if the formular is a window,
 *   - \b OC_TOOLBAR if the formular is a toolbar,
 *   - \b OC_OBJC if the formular is a classic formular,
 * @param win window descriptor or an object tree if mode is set to \b OC_OBJC,
 * @param obj index of an EDITABLE object,
 * @param val parameter whose meanning depends on the \e kind parameter,
 * @param idx position of cursor,
 * @param kind type of action. Possible values are :
 *   - \b ED_INIT activate the cursor initiale position is given by \e idx,
 *   - \b ED_END desactivate the cursor,
 *   - \b ED_CHAR insert the charater 'val' at current cursor position,
 *   - \b ED_BLC_OFF desactivate a block selection,
 *   - \b ED_BLC_START set the beginning of a block selection,
 *   - \b ED_BLC_END set the end of a block selection and activate it,
 * @return O or  a negative code error if an error occurs.
 *
 * This function replaces the AES mt_objc_edit() function and works with all editable field.
 * \b ED_BLC_OFF, \b ED_BLC_START and \b ED_BLC_END are extended modes only valid with
 * \b XEDIT objects (WinDom extended editable fields). Thess modes are currently
 * under developpement.
 */

int mt_ObjcEdit( APPvar *app, int mode, WINDOW *win, int obj, int val, INT16 *idx, int kind) {
	OBJECT *tree;
	W_FORM *wform;

	if( obj < 0) 
		return ERANGE;  /* Error, bad value for obj */

	switch( mode) {
	case OC_FORM:
		wform = mt_DataSearch( app, win, WD_WFRM);
		tree = wform->root;
		break;
	case OC_TOOLBAR:
		wform = &win->tool;
		tree = TOOL((WINDOW*)win);
		break;
	case OC_OBJC:
		tree = (OBJECT *)win;
		wform = NULL;
		break;
	default:
		return EBADRQ;	/* Error, bad value for mode */
	}
	
	return objc_wform_edit( app, win, wform, tree, obj, val, idx, kind);
}

int objc_wform_edit( APPvar *app, WINDOW *win, W_FORM *wform, OBJECT *tree, int obj, int val, INT16 *idx, int kind) {
	int retval;

	if ( !(tree[obj].ob_flags & EDITABLE))
		return EBADRQ;  /* Error, bad value for obj */
		
	if ( (tree[obj].ob_type & 0x00FF) == G_USERDEF ) {
		GRECT grect;
		
		mt_objc_offset( tree, obj, &(grect.g_x), &(grect.g_y), app->aes_global);
		grect.g_w = tree[obj].ob_width;
		grect.g_h = tree[obj].ob_height;
		
		if (wform) {
			GRECT r2, screen;
			GRECT *gclip = NULL;
			int nclip = 0;

			/* FIXME: rather use the app->graf.clip or win->graf->clip */
			mt_wind_get_grect( 0, WF_CURRXYWH, &screen, app->aes_global);
			if ( !rc_intersect( &screen, &grect) )
				return 0;

			mt_WindXGet( app, win, WF_FIRSTAREAXYWH, &grect, &r2.g_x, &r2.g_y, &r2.g_w, &r2.g_h);
			while( r2.g_w && r2.g_h) {
				if( rc_intersect( &grect, &r2) ) {
					nclip++;
					gclip = realloc( gclip, nclip * sizeof(GRECT));
					gclip[nclip-1] = r2;
				}
				mt_WindGet( app, win, WF_NEXTXYWH, &r2.g_x, &r2.g_y, &r2.g_w, &r2.g_h);
			}
			retval = udlib_edit( app, tree, obj, val, idx, kind, nclip, gclip);
			if (gclip) free(gclip);
			return retval;
		}
		
		return udlib_edit( app, tree, obj, val, idx, kind, 1, &grect);
	}
		
	/* case of editable object within a window
	 * PROBLEME : there's no way to change the clipping area
	 *            for objc_edit() 
	 */
	if( wform) {
		INT16 curs = *idx;
		
		retval = mt_objc_edit( tree, obj, val, idx, kind,app->aes_global);
		
		/* recompute the cursor position */
		if( curs > 0 && kind == ED_INIT) {		
			while( *idx != curs)
				mt_objc_edit( tree, obj, 
					((*idx > curs)?SC_LFARW:SC_RTARW)<<8, idx, ED_CHAR,app->aes_global);
		}
		
		return retval;
	}

	/* case of standard formular */
	return mt_objc_edit( tree, obj, val, idx, kind,app->aes_global);
}
