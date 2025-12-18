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
 * $Source: /cvsroot/windom/windom/src/ob_istype.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stdio.h>

#include "globals.h"

int ob_istype( APPvar *app, OBJECT *tree, int idx, int type) 
{
	if (  (tree[idx].ob_type            & 0x00FF) == type )
		return 1;
	
	if (  (tree[idx].ob_type            & 0x00FF) == G_USERDEF
      &&  (udlib_get_type(app,tree,idx) & 0x00FF) == type   )
		return 1;
	
	/* in other cases, returns 0 */
	return 0;
}

