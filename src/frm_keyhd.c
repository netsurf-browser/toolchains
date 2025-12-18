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
 * $Source: /cvsroot/windom/windom/src/frm_keyhd.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include "globals.h"
#include "scancode.h"

/*
 *	manage the keyboard events
 */
 
static int _obj_fd_xtype( APPvar *app, OBJECT *tree, int type) {
	int ob = 0;
	int udtype;
	
	while( 1) {
		if (   ((tree[ob].ob_type & 0x00FF) == G_USERDEF) 
		    && ((udtype=udlib_get_type(app,tree,ob)) != 0)   )
		{
			if ((udtype>>8) == type)
				return( ob);
		} 
		else if ((tree[ob].ob_type>>8) == type)
			return( ob);
		ob = get_next_obj( tree, ob);
		if (ob == -1)
			return -1;
	}
}

/** standard callback function to manage keyboard event on form
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to forms for
 *  the \b WM_XKEYBD event, registered by  mt_FormCreate()
 */
 
void __CDECL frm_keyhd( WINDOW *win, short buff[8], APPvar *app) {
	W_FORM *form = mt_DataSearch( app, win, WD_WFRM);
	OBJECT *dial;
	char scancode;
	int res, x2, y2;
	INT16 x, y;

	scancode = app->evnt.keybd >> 8;
	dial = form->root;
	
	/* key shortcut */
	
	if( (res = frm_keybd_ev( app, dial)) != -1)
		;
	else if ( scancode == SC_RETURN || scancode == SC_ENTER) 
		res = obj_fd_flag( dial, ROOTS, DEFAULT);
	else if( scancode == SC_UNDO) {
		res = _obj_fd_xtype( app, dial, UNDOBUT);
		if( res == -1)
			res = obj_fd_flag( dial, res, OF_FLAG11);
	}
	else if( scancode == SC_HELP)
		res = _obj_fd_xtype( app, dial, HELPBUT);
	else if (form->edit != -1) { 
		
		/* manage EDITABLE fields */
		
		/* go to next field */
		if ( scancode == SC_DWARW && app->evnt.mkstate == 0) {
			for( res=(form->edit+1)%form->nb_ob; (res % form->nb_ob) != form->edit; res++)
				if( (dial[res%form->nb_ob].ob_flags & EDITABLE) &&
					!(dial[res%form->nb_ob].ob_flags & HIDETREE))
					break;

			if ( res%form->nb_ob != form->edit) {
				mt_ObjcEdit( app, OC_FORM, win, form->edit, 0, &form->cursor, ED_END);
				form->edit = res%form->nb_ob;
				form->cursor = 0;
				mt_ObjcEdit( app, OC_FORM, win, form->edit, 0, &form->cursor, ED_INIT);
			}
		}
		/* go to previous field */
		else if ( scancode == SC_UPARW && app->evnt.mkstate == 0) {
			res = form->edit-1;			
			while( res != form->edit) {
				if( res<0)
					res = form->nb_ob-1;
				if( (dial[res].ob_flags & EDITABLE) && !(dial[res].ob_flags & HIDETREE))
					break;
				res --;
			}
			if ( res != form->edit) {
				mt_ObjcEdit( app, OC_FORM, win, form->edit, 0, &form->cursor, ED_END);
				form->edit = res;
				form->cursor = 0;
				mt_ObjcEdit( app, OC_FORM, win, form->edit, 0, &form->cursor, ED_INIT);
			}
		}
		/* otherwise: insert character */
		else 
			mt_ObjcEdit( app, OC_FORM, win, form->edit, app->evnt.keybd, &form->cursor, ED_CHAR);
		res = -1;
	}

	if( res != -1) {
		mt_objc_offset( dial, res, &x, &y, app->aes_global);
		x2 = app->evnt.mx; y2 = app->evnt.my;
		app->evnt.mx = x + dial[res].ob_width/2-1;
		app->evnt.my = y + dial[res].ob_height/2-1;
		res = frm_buttn_ev( app, win, OC_FORM);
		if( res == -1)
			return ;
		app->evnt.mx = x2; app->evnt.my = y2;
		mt_snd_msg( app, win, WM_FORM, res, app->evnt.mkstate, 0, 0);
	}
}
