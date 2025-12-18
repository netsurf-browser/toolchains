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
 * $Source: /cvsroot/windom/windom/src/rsrc_userfree.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stdlib.h>
#include "globals.h"

/* This function is automatically called by WindDelete() */

void mt_RsrcUserFree( APPvar *app, OBJECT *tree) {
	int index = 0;
	do {
		if( (tree[index].ob_type >> 8) == USERDRAW ) {
			free( (void*)tree[index].ob_spec.userblk->ub_parm);
			free( tree[index].ob_spec.userblk);
			tree[index].ob_type = G_IBOX;
			tree[index].ob_spec.index = 0L;
		}
	} while( !(tree[index++].ob_flags & LASTOB));
}
