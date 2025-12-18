/*
 * TEXTURED.LDG: a userdef library for windom
 * Copyright (c) 2005 Arnaud Bercegeay
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
 * $Source: /cvsroot/windom/windom/userdef/textured/string.c,v $
 * WinDom header file
 *  
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2005/11/03 22:18:05 $
 *   $Revision: 1.4 $
 */


#include <stdlib.h>
#include "global.h"

char * _xobjc_string( OBJECT *tree, long idx, char *put, APPENV *env)
{
	static char p[]="";
	USERBLK *user;
#ifdef MODIF_XEDIT
	W_XEDIT *xedit;
#endif

	switch ( (tree[idx].ob_type & 0xFF00)>>8) {
		case DCRBUTTON:
		case TITLEBOX:
		case UNDERLINE:
		case CIRCLEBUT:
		case ONGLET:
		case KPOPUPSTRG: /*  TODO: check this */
		case MENUTITLE:		
		case XFREESTR:
			user = tree[idx].ob_spec.userblk;
			if( put)
				(char*)((W_PARM*)user->ub_parm)->wp_spec = put;
			return ( (char*)((W_PARM*)user->ub_parm)->wp_spec);

#ifdef MODIF_XEDIT

		case XEDIT:
			xedit = (W_XEDIT *)tree[idx].ob_spec.userblk->ub_parm;
			if( put) {
				size_t size = (int) _xobjc_realloc_xedit( xedit->size, strlen(put)+1);
				if( size) {
					char *mem = (char*) APP_MALLOC(size);
					if (mem == 0L) return NULL;
					APP_FREE(xedit->line);
					xedit->size = size;
					xedit->line = mem;
				}
				strcpy( xedit->line, put);
			}
			xedit->flags = 0;
			xedit->curs = xedit->pos = 0;
			return xedit->line;

#endif

		case XTEDINFO:
			user = tree[idx].ob_spec.userblk;
			((TEDINFO *)(((W_PARM*)user->ub_parm)->wp_spec))->te_fontid = 0; /* TODO: why this ??? */		
			if( put)
				((TEDINFO *)(((W_PARM*)user->ub_parm)->wp_spec))->te_ptext = put;
			return ((TEDINFO *)(((W_PARM*)user->ub_parm)->wp_spec))->te_ptext;

	}
	
	return (p);

}
