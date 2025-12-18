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
 * $Source: /cvsroot/windom/windom/src/frm_buttn_ev.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/06/19 21:09:21 $
 *   $Revision: 1.10 $
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"


static void select_ob( BIND* bind, OBJECT *tree, int ob, int selected) {
	
	/* set/unset SELECTED state */
	
	if (selected)
		tree[ob].ob_state |=  SELECTED;
	else
		tree[ob].ob_state &= ~SELECTED;
	
	/* update binded variable */
	
	if (bind == NULL)
		return;
	
	switch (bind[ob].type) {
	case BIND_VAR:
		if (selected) *(bind[ob].desc.var.var) = bind[ob].desc.var.value;
		else          *(bind[ob].desc.var.var) = 0;
		break;
	case BIND_BIT:
		if (selected) *(bind[ob].desc.bit.var) |=  bind[ob].desc.bit.bit;
		else          *(bind[ob].desc.bit.var) &= ~bind[ob].desc.bit.bit;
		break;
	}
}

/*  this is a form_do replacement for formular in window
 */

static int __win_form_do( APPvar *app, WINDOW *win, W_FORM *wform) {
	int res, debut; 
	OBJECT *tree = wform->root;
	INT16 mx, my, mbut, dum;
	BIND *bind = wform->bind;
	
	mx = app->evnt.mx;
	my = app->evnt.my;

	/* selected object */
	res = mt_objc_find( tree, ROOTS, MAX_DEPTH, mx, my, app->aes_global);
	if( res == -1)
		return -1;

	/* manage 'SELECTABLE' objects */
	if( tree[res].ob_flags & SELECTABLE) {
		/* case of DISABLED objects */
		if( tree[res].ob_state & DISABLED)
			return -1;
		/* case of radio buttons */
		if( tree[res].ob_flags & RBUTTON) {
			if( !(tree[res].ob_state & SELECTED)) {
				debut = dum = res;
				while ( 1) { /* unselect the radio button */
					if( dum != tree[tree[dum].ob_next].ob_tail)
						dum = tree[dum].ob_next;
					else
						dum = tree[tree[dum].ob_next].ob_head;
					if( dum == debut)
						break;
					if( (tree[dum].ob_flags & RBUTTON) && (tree[dum].ob_state & SELECTED)) {
						select_ob( bind, tree, dum, 0);
						mt_ObjcWindDrawParent( app, win, tree, dum, MAX_DEPTH, MAX_DEPTH, app->x, app->y, app->w, app->h);
						break;
					}
				}
				select_ob(bind,tree,res,1);
		    	mt_ObjcWindDraw( app, win, tree, res, MAX_DEPTH, app->x, app->y, app->w, app->h);
			}
		}

		/* case of 'EXIT' objects */
		if( tree[res].ob_flags & EXIT) {
			do {
				dum = mt_objc_find( tree, ROOTS, MAX_DEPTH, mx, my, app->aes_global);
				if( (tree[res].ob_state & SELECTED)==0 && res == dum) {
					select_ob( bind, tree, res, 1);
					mt_ObjcWindDraw( app, win, tree, res, MAX_DEPTH, app->x, app->y, app->w, app->h);
				}
				if( res != dum && (tree[res].ob_state & SELECTED)) {
					select_ob( bind, tree, res, 0);
					mt_ObjcWindDraw( app, win, tree, res, MAX_DEPTH, app->x, app->y, app->w, app->h);
				}
				mt_graf_mkstate( &mx, &my, &mbut, &dum, app->aes_global);
			} while( mbut);
			return( tree[res].ob_state & SELECTED)?res:-1;
		}
	
		/* case of button which are not 'radio', nor 'exit', nor 'disabled' */
		if( !(tree[ res].ob_flags & RBUTTON)) {
			if( tree[res].ob_state & SELECTED )
				select_ob( bind, tree, res, 0);
			else
				select_ob( bind, tree, res, 1);
		}
		mt_ObjcWindDraw( app, win, tree, res, MAX_DEPTH, app->x, app->y, app->w, app->h);

		/* case of TOUCHEXIT objects */
		if( tree[res].ob_flags & TOUCHEXIT)
			return res;
		else {
			/* case of 'non TOUCHEXIT' objects: the button shall be released before exiting */
			do
				mt_graf_mkstate( &dum, &dum, &mbut, &dum, app->aes_global);
			while( mbut);
			return -1;
		}
	}	/* end of SELECTABLE objects */
	
	/* case of 'TOUCHEXIT' objects */
	if( tree[res].ob_flags & TOUCHEXIT)
		return( res);

	if( !(tree[res].ob_state & SELECTABLE) &&
	    !(tree[res].ob_flags & TOUCHEXIT)  &&
	    win != app->wglb.front)
	    mt_ApplWrite( app, mt_AppId(app), WM_TOPPED, win->handle, 0, 0, 0, 0);

	if( (tree[res].ob_type & 0x00FF) == G_USERDEF 
		 && tree[res].ob_flags & EDITABLE) {
		
		mt_graf_mkstate( &mx, &my, &mbut, &dum, app->aes_global);

		/* unselect the bloc (if needed) */
		objc_wform_edit( app, win, wform, tree, res, 0, &dum, ED_BLC_OFF);

		/* remove the text cursor from the previous field */
		objc_wform_edit( app, win, wform, tree, wform->edit, 0, &wform->cursor, ED_END);

		/* a mouse button event sets the text cursor position */
#if 0 /*find another way to select a bloc (using objc_edit extension ?) */
		if( !mbut) { 
#endif	 /*find another way to select a bloc (using objc_edit extension ?) */
			/* set the text cursor position */
			wform->cursor = udlib_get_edpos( app, tree, res, app->evnt.mx, app->evnt.my);
			objc_wform_edit( app, win, wform, tree, res, 0, &wform->cursor, ED_CURS);

			/* show the text cursor */
			objc_wform_edit( app, win, wform, tree, res, 0, &wform->cursor, ED_INIT);		
			
			wform->edit = res;
#if 0  /*find another way to select a bloc (using objc_edit extension ?) */
		} else {
			/* a long mouse button event selects a text area */
			W_XEDIT *xedit = (W_XEDIT *)tree[res].ob_spec.userblk->ub_parm;
			int beg, inter, last;
			INT16 xpos, ypos;
			
			if( app->aes4 & AES4_XGMOUSE)
				mt_graf_mouse( M_SAVE, 0L, app->aes_global); 
			mt_graf_mouse( POINT_HAND, 0L, app->aes_global); 
			xedit -> blcbeg = beg = xedit_mouse2curs( app, tree, res, app->evnt.mx);
			xedit -> blcend = last = beg;
			mt_objc_offset( tree, res, &xpos, &ypos, app->aes_global);
			do {
				inter = xedit_mouse2curs( app, tree, res, mx);
/*				if( inter <= xedit->pos && inter >= 0) {
					xedit->pos --;
					ObjcWindDraw( win, tree, res, 0, xpos, ypos, 
							  	  tree[res].ob_width, tree[res].ob_height);
				} else
 				if( width_curs( xedit, inter) > tree[res].ob_width) {
 					xedit->pos ++;
					ObjcWindDraw( win, tree, res, 0, xpos, ypos, 
							  	  tree[res].ob_width, tree[res].ob_height);
				} */
				if( inter > last) {
					xedit -> blcbeg = last;
					xedit -> blcend = inter;
					win_xedit_bloc( app, win, wform, res);
					last = inter;
				} else if( inter < xedit -> blcbeg) {
					xedit -> blcend = last;
					xedit -> blcbeg = inter;
					win_xedit_bloc( app, win, wform, res);
					last = inter;
				}
				mt_graf_mkstate( &mx, &my, &mbut, &dum, app->aes_global);

			} while ( mbut);

			if( !(app->aes4 & AES4_XGMOUSE) || mt_graf_mouse( M_RESTORE, 0L, app->aes_global) == 0 )
				mt_graf_mouse(ARROW,NULL, app->aes_global);
			if( last > beg) {
				xedit -> blcbeg = beg;
				xedit -> blcend = last;
			} else {
				xedit -> blcend = beg;
				xedit -> blcbeg = last;			
			}
			xedit -> flags |= XEDIT_BLOC_ON;
		}
#endif /*find another way to select a bloc (using objc_edit extension ?) */
	} else if( tree[res].ob_flags & EDITABLE) {
		int offset;
		INT16 tab[8];
		INT16 x,dx,pos=0;
		char *p;
		int ind, len;
		
		/* remove the text cursor */
		objc_wform_edit( app, win, wform, tree, wform->edit, 0, &wform->cursor, ED_END);
		wform->edit = res;

		/*
		 *	set the text cursor under the mouse position
		 *  compute the text coordinates
		 */
		
		/* position of the mouse pointer from the left side of the object */
		mt_objc_offset( tree, res, &x, &dx, app->aes_global);
		x = app->evnt.mx-x;

		/* width of the field */ 		
 		p = tree[res].ob_spec.tedinfo->te_ptmplt;
		if( vq_vgdos() == _FSM) 
			vqt_f_extent( app->aeshdl, p, tab);
		else
			vqt_extent(app->aeshdl, p, tab);
		dx = tab[2] - tab[0] /* + offset? */;
		
		/* shift the text depending of alignment parameter */
		switch (tree[res].ob_spec.tedinfo->te_just) {
			case TE_LEFT:
				offset = 0;
				break;
			case TE_RIGHT:
				offset = tree[res].ob_width - dx;
				break;
			case TE_CNTR:
				offset = (tree[res].ob_width - dx)/2;
				break;
			default:
				offset = 0;
		}
		
		/* compute the text cursor position */
 		p = strdup( tree[res].ob_spec.tedinfo->te_ptmplt);
		pos = (int)strlen( p);
		do {
			if( vq_vgdos() == _FSM) 
				vqt_f_extent( app->aeshdl, p, tab);
			else
				vqt_extent(app->aeshdl, p, tab);
			if( p[pos] != '_' && x >= tab[2]-tab[0]+offset)
				break;
			p[--pos] = '\0';
		} while( pos);

		/* count non-modifiable characters at the end */
		for( ind = 0, len = 0; ind < pos; ind ++)
			if( p[ind] != '_')
				len ++;
		
		/* new position of the text cursor */
		wform->cursor = MIN( pos - len, (int)strlen(tree[res].ob_spec.tedinfo->te_ptext));
		if( wform->edit != res && wform->edit != -1)
			objc_wform_edit( app, win, wform, tree, wform->edit, 0, &pos, ED_END);
		objc_wform_edit( app, win, wform, tree, res, 0, &wform->cursor, ED_INIT);
		wform->edit = res;		
		free(p);
	}
	return( -1);
}

/*
 *	mouse button event attached to formular in windows
 */

int frm_buttn_ev( APPvar *app, WINDOW *win, int mode) {
	W_FORM *tree;
	int res;
		
	switch( mode) {
	case OC_FORM:
		tree = mt_DataSearch( app, win, WD_WFRM);
		break;
	case OC_TOOLBAR:
		tree = &win->tool;
		break;
	default:
		return -1;
	}
	
	res = __win_form_do( app, win, tree);
	
	/* Binding fonction */
	if( res != -1 ) {
		BIND * bind = tree->bind;
		func_bind cb;
		if (bind && bind[res].type == BIND_FUNC) {
			if (mode==OC_FORM)
				cb = bind[res].desc.func.f.form;
			else
				cb = bind[res].desc.func.f.tb;
			(*cb)(win, res, mode, bind[res].desc.func.data, app);
		}
	}
	
	return res;
}

