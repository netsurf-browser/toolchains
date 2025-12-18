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
 * $Source: /cvsroot/windom/windom/src/menu_attach.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"

/*
 * Attach a submenu. The goal was to offer to all TOS this AES 3.4 feature.
 * Currently this function is not finished and does not work.
 * Any ideas are welcome.
 */

int mt_MenuAttach( APPvar *app, int flag, OBJECT *tree, short item, MENU *mdata) {
	char *txt;
	
	if( tree[item].ob_type == G_STRING && flag == ME_ATTACH) {
		SMENU *sub;
		USERBLK *user;
		
		/* attach the submenu:
		 * the entry point (G_STRING) is replaced by a userdef
		 * with the XSMENU flag
		 */
		tree[item].ob_type = G_USERDEF | (XSMENU<<8);

		sub = (SMENU *)malloc( sizeof(SMENU));
		txt = tree[item].ob_spec.free_string;
		txt[ strlen(txt)-2] = '';
		sub->string = txt;
		sub->mn_tree = mdata->mn_tree;
		sub->mn_menu = mdata->mn_menu;
		sub->mn_item = mdata->mn_item;
		sub->mn_scroll = 0 /*mdata->mn_scroll*/;
		sub->app = app;

		user = (USERBLK *)malloc( sizeof(USERBLK));
		user->ub_code = draw_submenu;
		user->ub_parm =(long)sub;
		
		tree[item].ob_spec.userblk = user;
		
	} else if( tree[item].ob_type == (G_USERDEF|(XSMENU<<8) && udlib_get_type(app,tree,item) == 0) ) {
		if (flag == ME_REMOVE) {		
			txt = ((SMENU *)tree[item].ob_spec.userblk->ub_parm)->string;
			tree[item].ob_type = G_STRING;
			free( (void*)tree[item].ob_spec.userblk->ub_parm);
			free( tree[item].ob_spec.userblk);
			txt[ strlen(txt)-2] = ' ';
			tree[item].ob_spec.free_string = txt;
		} else if( flag == ME_INQUIRE) {
			/* TODO */
		}
	}
	return 0;
}

