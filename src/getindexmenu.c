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
 * $Source: /cvsroot/windom/windom/src/getindexmenu.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/*  return the index (?) of the title of the given menubar.
 *  This is an utility function for frm_menu().
 */

int mt_GetIndexMenu( APPvar *app, OBJECT *menu, int title) {
	register int count=0;
	int check=1, link=0;

	while( !(menu[count++].ob_flags & LASTOB) ) {
		if( count != title && ob_istype(app,menu,count,G_TITLE) )
			check ++;
		if( count == title)
			break;
	}
	while( !(menu[count++].ob_flags & LASTOB) ) {
		if( ob_istype(app,menu,count,G_BOX))
			link ++;
		if( link == check)
			return count;
	}
	return -1;
}
