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
 * $Source: /cvsroot/windom/windom/src/bubble_do.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include "globals.h"

/**
 * @brief alternative FormDo() function for BubbleGEM
 *
 * @param app,tree,edit see mt_FormDo().
 *
 * see mt_BubbleModal() for details
 *
 * @sa mt_BubbleModal().
 */

int mt_BubbleDo( APPvar *app, OBJECT *tree, int edit) {
	INT16 msg[8], mx, my, mbut;
	INT16 keybd, click, obj;
	INT16 ev;
	INT16 pos = 0;
	char *help;

	if( edit != -1) mt_objc_edit( tree, edit, 0, &ev, ED_INIT, app->aes_global);
	
	while( 1) {
		ev = mt_evnt_multi( MU_BUTTON|MU_KEYBD, 
					258, 3, 0, 
     				0, 0, 0, 0, 0,
     				0, 0, 0, 0, 0, 
     				msg, 
					0L,
          			&mx, &my, &mbut,  
   					NULL, &keybd, &click,
					app->aes_global);
	
		if( ev & MU_BUTTON) {
			obj = mt_objc_find( tree, 0, 7, mx, my, app->aes_global);
			if( obj != -1 && mbut & 0x1) {
				if( !mt_form_button( tree, obj, click, NULL, app->aes_global))
					return obj;
				if( tree[obj].ob_flags & EDITABLE && obj != edit) {
					mt_objc_edit( tree, edit, 0, &pos, ED_END, app->aes_global);
					edit = obj;
					mt_objc_edit( tree, edit, 0, &pos, ED_INIT, app->aes_global);			
				}
			} else if( obj != -1 && mbut & 0x2) {
				if(	mt_BubbleFind( app, tree, obj, &help))
					mt_BubbleModal( app, help, mx, my);
			}
		}
		
		if( ev & MU_KEYBD) {
			INT16 key;
			
			if( !mt_form_keybd( tree, (edit==-1)?0:edit, 1, keybd, &obj, &key, app->aes_global))
				return obj;
			if( key) 
				mt_objc_edit( tree, edit, key, &pos, ED_CHAR, app->aes_global);
			else {
				mt_objc_edit( tree, edit, 0, &pos, ED_END, app->aes_global);
				edit = obj;
				mt_objc_edit( tree, edit, 0, &pos, ED_INIT, app->aes_global);
			}	
		}
	}
}

