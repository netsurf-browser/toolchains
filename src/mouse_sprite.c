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
 * $Source: /cvsroot/windom/windom/src/mouse_sprite.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */

#include "globals.h"

/** set a ICON as the new mouse cursor form
 *
 *  @param app application descriptor
 *  @param tree address of a form
 *  @param index object of the form 
 *
 *  This function gets the \a index-th object of the \a tree.
 *  This object must be a G_ICON object. This doesn't work
 *  for color icon. Same for userdef object (warning: a userdef
 *  library may have transformed your G_ICON object to a userdef
 *  object).
 *
 *  Then, the icon data of this object is used as the new form
 *  of the mouse.
 *
 */

void mt_MouseSprite( APPvar *app, OBJECT *tree, int index) {
	MFORM mform;
	int dum;
	
	if ((tree[index].ob_type & 0xFF) != G_ICON)
		return;
	
	dum = tree[index].ob_spec.iconblk->ib_char;
	mform . mf_nplanes = 1;
	mform . mf_fg = (dum>>8)&0x0F;
	mform . mf_bg = dum>>12;	
	mform . mf_xhot = 0; /* to prevent the mform to "jump" on the */
	mform . mf_yhot = 0; /* screen (zebulon rules!) */

	for( dum = 0; dum<16; dum ++) {
		mform . mf_mask[dum] = tree[index].ob_spec.iconblk->ib_pmask[dum];
		mform . mf_data[dum] = tree[index].ob_spec.iconblk->ib_pdata[dum];
	}
	mt_graf_mouse( USER_DEF, &mform, app->aes_global);
}

