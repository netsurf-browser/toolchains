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
 * $Source: /cvsroot/windom/windom/src/form_attach.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include "globals.h"

/**
 * @brief Attach a formular to a window.
 * 
 * @param app application descriptor,
 * @param win window descriptor,
 * @param tree formular object tree,
 * @param proc callback function handling user selection or \b NULL,
 * @return a window descriptor.
 * 
 * This function is a sub function of mt_FormCreate(). It should be used the case
 * where mt_FormCreate() cannot be used. If a \b NULL value is given to
 * parameter \e tree, mt_FormAttach() removes the formalar attached to
 * the window (the formular should be prevously attached by mt_FormAttach()).
 *
 * mt_FormAttach() does not duplicated the object tree. If you create
 * multiple window formular from an unique object tree with mt_FormAttach()
 * you should duplicate the object tree with mt_ObjcDup().
 *
 * @sa mt_FormCreate(), mt_FormResize(), mt_ObjcDup().
 */

int mt_FormAttach( APPvar *app, WINDOW *win, OBJECT *tree, func_evnt proc) {
	W_FORM *form;

	/* Attach a form */
	if( tree != NULL) {
		int outlined=0;
		INT16 dum, x, y;

		form = (W_FORM *)malloc( sizeof(W_FORM));
		if( !form) return ENSMEM;
		
		/* A form was already attached */
		if( win->status & WS_FORM)	
			mt_FormAttach( app, win, NULL, (func_evnt)0L);
	
		if( tree -> ob_state & OUTLINED)
			outlined = OUTLINED_WIDTH;

		mt_EvntAttach( app, win, WM_REDRAW,  frm_drw);
		mt_EvntAttach( app, win, WM_FORM,    proc);
		mt_EvntAttach( app, win, WM_MOVED,   frm_mvd);
		mt_EvntAttach( app, win, WM_TOPPED,  frm_tpd);
		mt_EvntAttach( app, win, WM_FULLED,  frm_fld);
		mt_EvntAttach( app, win, WM_XBUTTON, frm_click);
		mt_EvntAttach( app, win, WM_XKEYBD,  frm_keyhd);
		mt_EvntAdd	 ( app, win, WM_DESTROY, frm_dstry, EV_TOP);
		
		mt_DataAttach( app, win, WD_WFRM, form);
		win->status |= WS_FORM;

		form->root = tree;
		form->real = tree;
		form->save = NULL;
		form->bind = NULL;
		form->edit = obj_fd_flag( tree, 0, EDITABLE);
		
		/* position of the text cursor (objc_edit) */
		form->cursor = 0;		  
		
		/* number of OBJECTs that this tree contains */
		form->nb_ob = obj_nb( tree);
		
		/* align tree position with the work area of the window */
		mt_WindGet( app, win, WF_WORKXYWH, &x, &y, &dum, &dum);
		form->root->ob_x = x - (int)win->xpos * win->w_u + outlined;
		form->root->ob_y = y - (int)win->ypos * win->h_u + outlined;
		
		/* sliders stuff */
		mt_graf_handle( &win -> w_u, &win -> h_u, &dum, &dum, app->aes_global);
		win -> xpos = win -> ypos = 0;
		win -> xpos_max = (tree[0].ob_width+2*outlined)/win -> w_u;
		win -> ypos_max = (tree[0].ob_height+2*outlined)/win -> h_u;

		/* maximum size */
		mt_WindCalc( app, WC_BORDER, win, 0, 0, tree[0].ob_width+2*outlined, 
				  tree[0].ob_height+2*outlined, &x, &x, &win->w_max, &win->h_max);
	} else {
		/* free the form of that window */	
		form = mt_DataSearch( app, win, WD_WFRM);

		mt_RsrcUserFree( app, form->root);
		if( form->bind) free( form->bind);
		if( form->save) free( form->save);
		if( win->status & WS_FORMDUP) mt_ObjcFree( app, form->root);
		mt_DataDelete( app, win, WD_WFRM);
		free( form);

		win -> status &= ~WS_FORM;

		mt_EvntAttach( app, win, WM_REDRAW, mt_stdWindClear);
		mt_EvntRemove( app, win, WM_DESTROY, frm_dstry);
		mt_EvntDelete( app, win, WM_FORM);
		mt_EvntAttach( app, win, WM_MOVED, std_mvd);
		mt_EvntAttach( app, win, WM_TOPPED, std_tpd);
		mt_EvntDelete( app, win, WM_XKEYBD);

		win -> xpos = win -> ypos = 0;
		win -> xpos_max = win -> ypos_max = 1;
	}
	return 0;
}
