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
 * $Source: /cvsroot/windom/windom/src/frm_keybd_ev.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "globals.h"

/*
 *	find the object which shortcut corresponds to the key pressed
 */

int frm_keybd_ev( APPvar *app, OBJECT *tree) {
	int ob=0;
	char *p, rac[2];

	if( !(app->evnt.mkstate & K_ALT))
		return -1;
	
	GemCode2Ascii( app->evnt.keybd, app->evnt.mkstate, rac);

	/* look for the command */
	while(1) {
		
		if (tree[ob].ob_flags & SELECTABLE && !(tree[ob].ob_state & DISABLED)
		    && !(tree[ob].ob_flags & HIDETREE))
		{
			p = mt_ObjcString(app,tree,ob,NULL);
			
			if (p) 
				p = strchr( p, '[');
				
			if (p && toupper( *(p+1)) == rac[0])
				return ob;
		}
		
		ob = get_next_obj( tree, ob);
		if (ob == -1)
			return -1;
	}
}
