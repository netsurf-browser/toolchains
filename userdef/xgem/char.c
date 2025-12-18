/*
 * XGEM.LDG: a userdef library for windom
 * Copyright (c) 2005 Arnaud Bercegeay
 *
 * some routines are based on Eric REBOUX's XGEM library
 * Copyright (c)1997-2003 Eric Reboux
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
 * $Source: /cvsroot/windom/windom/userdef/xgem/char.c,v $
 * WinDom header file
 *  
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2005/02/09 21:17:48 $
 *   $Revision: 1.1 $
 */

#include <stdlib.h>
#include "global.h"

long _xobjc_char( OBJECT *tree, long idx, long put, APPENV *env)
{
	USERBLK *user;
	BFOBSPEC *bfobspec;

	if ( ((tree[idx].ob_type & 0xFF00)>>8) == XBFOBSPEC) {
		user = tree[idx].ob_spec.userblk;
		bfobspec = (BFOBSPEC*) &((W_PARM*)user->ub_parm)->wp_spec;
		if( put > 0L) {
			bfobspec->character = put;
		}
		return bfobspec->character;
	}
	
	return (-1L);
}
