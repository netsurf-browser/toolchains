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
 * $Source: /cvsroot/windom/windom/src/get_next_obj.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */


#include <stddef.h>
#include <stdlib.h>
#include "globals.h"


int get_next_obj( OBJECT *dial, int racine) {
	if(dial[racine].ob_head != -1 && !(dial[racine].ob_flags & HIDETREE))
			return( dial[racine].ob_head);
	else {
		if(dial[dial[racine].ob_next].ob_tail != racine)
			return( dial[racine].ob_next);
		else {
			while(dial[dial[racine].ob_next].ob_tail == racine) {
				racine = dial[racine].ob_next;
				if(dial[racine].ob_next == -1)
					return( -1);
			}
			return( dial[racine].ob_next);
		}
	}
}
