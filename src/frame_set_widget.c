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
 * $Source: /cvsroot/windom/windom/src/frame_set_widget.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */


#include <stdlib.h>
#include "globals.h"
#include "sliders.h"

void __frame_set_widget( APPvar *app, int parm1) {
	OBJECT *tree = get_frame_slider(app);
	
	if (tree==NULL)
		return;
	
	tree[M_V_UP].ob_width = parm1;
	tree[M_V_UP].ob_height = parm1;
	tree[M_V_DW].ob_width = parm1;
	tree[M_V_DW].ob_height = parm1;
	tree[M_V_SL].ob_width = parm1;
	tree[M_V_PG].ob_width = parm1;
	tree[M_V_BG].ob_width = parm1;

	tree[M_H_LF].ob_width = parm1;
	tree[M_H_LF].ob_height = parm1;
	tree[M_H_RT].ob_width = parm1;
	tree[M_H_RT].ob_height = parm1;
	tree[M_H_SL].ob_height = parm1;
	tree[M_H_PG].ob_height = parm1;
	tree[M_H_BG].ob_height = parm1;

	tree[M_SZ].ob_width = parm1;
	tree[M_SZ].ob_height = parm1;
}
