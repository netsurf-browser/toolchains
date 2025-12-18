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
 * $Source: /cvsroot/windom/windom/examples/multifrm/multifrm.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:39:25 $
 *   $Revision: 1.5 $
 */

#include <stddef.h>
#include "windom.h"
#include "multifrm.h"

static void __CDECL closeform( WINDOW *win, int index, int unused, void *unused2) {
	ObjcChange( OC_FORM, win, index, ~SELECTED, TRUE);
	ApplWrite( _AESapid, WM_DESTROY, win->handle, 0,0,0,0);
}

int main(void) {
	OBJECT *tree;
	WINDOW *win;
	int frms[] = {FOND1, FOND2, FOND3};
	int buts[] = {BUT1, BUT2, BUT3};

	ApplInit();
	RsrcLoad( "multifrm.rsc");
	RsrcXtype( RSRC_XALL, NULL, 0);
	rsrc_gaddr( 0, FORM1, &tree);

	win = FormCreate( tree, WAT_FORM, NULL, "Tab forms", 
					  NULL, TRUE, FALSE);
	FormThumb( win, frms, buts, 3);
	ObjcAttachFormFunc( win, OK, closeform, NULL);
	ObjcAttachFormFunc( win, CANCEL, closeform, NULL);

	while( wglb.first) EvntWindom(MU_MESAG);
	RsrcXtype( 0, NULL, 0);
	RsrcFree();
	ApplExit();
	return 0;
}
