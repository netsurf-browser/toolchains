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
 * $Source: /cvsroot/windom/windom/src/set_gadget_pos.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.7 $
 */


#include <stdlib.h>
#include "globals.h"
#include "sliders.h"

#define ISNT_HIDE(a,b)	(((a)[(b)].ob_flags & HIDETREE)?0:1)

/* TODO: use constant names for "mode" */

/* adjust position and draw frame widgets
   r1   current area of the frame
   mode 0x0001	redraw
   		0x0002	redraw usign ObjcWindDraw
   		0x0004	unselect widgets
   		0x0008	mode for WindSet()
   		0x0010	idem
   		0x0020  idem
   		0x0040	select widgets. If the window is topped, this
		        is the mode for WindSet()
*/

void set_gadget_pos( APPvar *app, WINDOW *frame, GRECT *r1, int mode) {
	OBJECT *tree;
	WINDOW *root;
	FRAMEINFO *finfo = GETFINFO(app,frame);

	tree = get_frame_slider(app);
	
	if (tree==NULL)
		return;
	
	/* select the widgets (depend on attributes) */

	tree[M_INFO].ob_spec.tedinfo->te_ptext = frame->info;

	SET_BIT( tree[M_INFO].ob_flags, HIDETREE, !(frame->attrib & INFO));

	SET_BIT( tree[M_V_PG].ob_flags, HIDETREE, !(frame->attrib & VSLIDE));
	SET_BIT( tree[M_V_UP].ob_flags, HIDETREE, !(frame->attrib & UPARROW));
	SET_BIT( tree[M_V_DW].ob_flags, HIDETREE, !(frame->attrib & DNARROW));
	SET_BIT( tree[M_V_BG].ob_flags, HIDETREE, !(frame->attrib & (SIZER+DNARROW+UPARROW+VSLIDE)));

	SET_BIT( tree[M_H_PG].ob_flags, HIDETREE, !(frame->attrib & HSLIDE));
	SET_BIT( tree[M_H_LF].ob_flags, HIDETREE, !(frame->attrib & LFARROW));
	SET_BIT( tree[M_H_RT].ob_flags, HIDETREE, !(frame->attrib & RTARROW));
	SET_BIT( tree[M_H_BG].ob_flags, HIDETREE, !(frame->attrib & (LFARROW+RTARROW+HSLIDE)));

	if( frame->attrib & SIZER ||
		(!(tree[M_H_BG].ob_flags & HIDETREE) &&
		 !(tree[M_V_BG].ob_flags & HIDETREE)) )
		tree[M_SZ].ob_flags &= ~HIDETREE;
	else
		tree[M_SZ].ob_flags |= HIDETREE;

	/* widgets size and position */

	/* INFO barre */
	tree[M_INFO].ob_x =	r1->g_x;
	tree[M_INFO].ob_y = r1->g_y;
	tree[M_INFO].ob_width = r1->g_w;
	
	/* size of slider background : depend on the existance of SIZER and INFO barre */
	tree[M_H_BG].ob_width  = r1 -> g_w + 3 - ISNT_HIDE(tree,M_SZ)*(tree[M_SZ].ob_width);
	tree[M_V_BG].ob_height = r1 -> g_h + 3 - ISNT_HIDE(tree,M_SZ)*(tree[M_SZ].ob_height)
							 - ISNT_HIDE(tree,M_INFO)*(tree[M_INFO].ob_height+1);
	
	/* position of slider background */
	tree[M_V_BG].ob_x =	r1->g_x + r1->g_w - tree[M_V_BG].ob_width + 1;
	tree[M_V_BG].ob_y = r1->g_y + ISNT_HIDE(tree,M_INFO)*(tree[M_INFO].ob_height+1) - 1;

	tree[M_H_BG].ob_x = r1->g_x - 1;
	tree[M_H_BG].ob_y = r1->g_y + r1->g_h - tree[M_H_BG].ob_height + 1;
	
	/* pagers (PAGE UP/DOWN) - depend on existance of sliders */
	tree[M_V_PG].ob_y = (tree[M_V_UP].ob_height-1)*ISNT_HIDE(tree,M_V_UP);
	tree[M_V_PG].ob_height = tree[M_V_BG].ob_height 
							 - ISNT_HIDE(tree,M_V_DW)*(tree[M_V_DW].ob_height-1)
							 - ISNT_HIDE(tree,M_V_UP)*(tree[M_V_UP].ob_height-1);

	tree[M_H_PG].ob_x = (tree[M_H_LF].ob_height-1)*ISNT_HIDE(tree,M_H_LF);

	tree[M_H_PG].ob_width  = tree[M_H_BG].ob_width + 2 
							 - ISNT_HIDE(tree,M_H_RT)*(tree[M_H_RT].ob_width)
							 - ISNT_HIDE(tree,M_H_LF)*(tree[M_H_LF].ob_width);
	/* sliders */
	tree[M_V_SL].ob_height = (int)((long)finfo->vsldsize * tree[M_V_PG].ob_height/1000L);
	tree[M_V_SL].ob_height = MAX( tree[M_V_SL].ob_height, tree[M_V_SL].ob_width);
	tree[M_V_SL].ob_height = MIN( tree[M_V_SL].ob_height, tree[M_V_PG].ob_height);
	tree[M_V_SL].ob_y = (int)( (long)finfo->vsldpos*(long)(tree[M_V_PG].ob_height-tree[M_V_SL].ob_height)/1000L);	
	tree[M_V_SL].ob_y = MIN( tree[M_V_SL].ob_y, tree[M_V_PG].ob_height-tree[M_V_SL].ob_height);
	
	tree[M_H_SL].ob_width  = (int)((long)finfo->hsldsize*(long)tree[M_H_PG].ob_width/1000L);
	tree[M_H_SL].ob_width = MAX( tree[M_H_SL].ob_width, tree[M_H_SL].ob_height);
	tree[M_H_SL].ob_x = (int)((long)finfo->hsldpos*(long)(tree[M_H_PG].ob_width-tree[M_H_SL].ob_width)/1000L);
	tree[M_H_SL].ob_x = MIN( tree[M_H_SL].ob_x, tree[M_H_PG].ob_width-tree[M_H_SL].ob_width);

	/* scrollers */
	tree[M_H_RT].ob_x = tree[M_H_BG].ob_width - tree[M_H_RT].ob_width ;
	tree[M_V_DW].ob_y = tree[M_V_BG].ob_height - tree[M_V_DW].ob_height ;

	/* sizer */
	tree[M_SZ].ob_x = r1->g_x + r1->g_w - tree[M_V_BG].ob_width  + 1;
	tree[M_SZ].ob_y = r1->g_y + r1->g_h - tree[M_H_BG].ob_height + 1;
	mt_ObjcChar(app,tree,M_SZ,(frame->attrib & SIZER)?'':' ');
	
	tree[M_INFO].ob_state &= ~DISABLED;
	tree[M_H_PG].ob_state &= ~DISABLED;
	tree[M_V_PG].ob_state &= ~DISABLED;
	tree[M_V_UP].ob_state &= ~DISABLED;
	tree[M_V_DW].ob_state &= ~DISABLED;
	tree[M_H_LF].ob_state &= ~DISABLED;
	tree[M_H_RT].ob_state &= ~DISABLED;
	tree[M_SZ  ].ob_state &= ~DISABLED;

	root = mt_WindHandle( app, frame->handle);
	
	if( mode & 0x4 ||
		(mode & 0x40 && root != app->wglb.front) ) {
		tree[M_INFO].ob_state |= DISABLED;
		tree[M_H_PG].ob_state |= DISABLED;
		tree[M_V_PG].ob_state |= DISABLED;
		tree[M_V_UP].ob_state |= DISABLED;
		tree[M_V_DW].ob_state |= DISABLED;
		tree[M_H_LF].ob_state |= DISABLED;
		tree[M_H_RT].ob_state |= DISABLED;
		tree[M_SZ  ].ob_state |= DISABLED;
	} 

	/* draw */
	if( mode & 0x1 || mode & 0x8 || mode & 0x10 || mode & 0x20) {
		if( mode & 0x2 || mode & 0x8 || mode & 0x10 || mode & 0x20) {
			INT16 x, y, w, h;
		
			mt_wind_get( root -> handle, WF_CURRXYWH, &x, &y, &w, &h, app->aes_global);
			mt_WindCalc( app, WC_WORK, root, x, y, w, h, &x, &y, &w, &h);
			
			if( mode & 0x2) {
				mt_ObjcWindDraw( app, root, tree, M_INFO, 0, x, y, w, h);
				mt_ObjcWindDraw( app, root, tree, M_H_BG, 2, x, y, w, h);
				mt_ObjcWindDraw( app, root, tree, M_V_BG, 2, x, y, w, h);
				mt_ObjcWindDraw( app, root, tree, M_SZ  , 0, x, y, w, h);
			}
			if( mode & 0x8)
				mt_ObjcWindDraw( app, root, tree, M_H_PG, 1, x, y, w, h);
			if( mode & 0x10)
				mt_ObjcWindDraw( app, root, tree, M_V_PG, 1, x, y, w, h);
			if( mode & 0x20)
				mt_ObjcWindDraw( app, root, tree, M_INFO, 0, x, y, w, h);
		} else {
			mt_objc_draw_grect( tree, M_INFO, 0, &frame->graf->clip, app->aes_global);
			mt_objc_draw_grect( tree, M_V_BG, 2, &frame->graf->clip, app->aes_global);
			mt_objc_draw_grect( tree, M_H_BG, 2, &frame->graf->clip, app->aes_global);
			mt_objc_draw_grect( tree, M_SZ  , 0, &frame->graf->clip, app->aes_global);	
		}
	}		
}




