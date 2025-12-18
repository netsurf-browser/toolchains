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
 * $Source: /cvsroot/windom/windom/src/draw_submenu.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

WORD __CDECL draw_submenu( PARMBLK *pblk) {
	APPvar *app = ((SMENU *)pblk->pb_parm)->app;
	/* 1er: dessiner l'item
	 * 2nd: afficher le sub menu
	 */
	INT16 tab[10], hcar;
#if 0
	INT16 mx,my,but,dum;
	OBJECT *tree;
	MFDB scr;
#endif

	 	/* Clippage */
	tab[0]=pblk->pb_xc;
	tab[1]=pblk->pb_yc;
	tab[2]=pblk->pb_wc+tab[0]-1;
	tab[3]=pblk->pb_hc+tab[1]-1;
	vs_clip( app->graf.handle, 1, tab);

	vqt_attributes( app->graf.handle, tab);
	hcar = tab[7];
	
	tab[0]=pblk->pb_x;
	tab[1]=pblk->pb_y;
	tab[2]=pblk->pb_w+tab[0]-1;
	tab[3]=pblk->pb_h+tab[1]-1;

	vswr_mode( app->graf.handle, MD_REPLACE);
	vsf_color( app->graf.handle, WHITE);
	v_bar( app->graf.handle, tab);

	v_gtext( app->graf.handle, pblk->pb_x, pblk->pb_y + pblk->pb_h/2+hcar/2-1, 
			((SMENU *)pblk->pb_parm)->string);
	
	if( pblk->pb_currstate & SELECTED) {
		vswr_mode( app->graf.handle, MD_XOR);
		vsf_color( app->graf.handle, BLACK);
		vsf_interior( app->graf.handle, 1 /*FIS_SOLID*/ );
		tab[0] --;tab[1] --;
		tab[2] ++;tab[3] ++;
		v_bar( app->graf.handle, tab);
	}
	vs_clip( app->graf.handle, 0, tab);

	/* TODO
	 * what did the following code here (with AES calls) ? It's a userdraw subroutine,
	 * not the sub menu management ! This piece of code shall be moved somewhere else.
	 */

#if 0 /* disabled dec 2003 : see text above */
	if( /*((SMENU *)pblk->pb_parm)->mn_scroll &&*/ pblk->pb_currstate & SELECTED) {
		tree = ((SMENU *)pblk->pb_parm)->mn_tree;
		tree->ob_x = pblk->pb_x+pblk->pb_w;
		tree->ob_y = pblk->pb_y;
		w_get_bkgr( tree->ob_x-3, tree->ob_y-3, tree->ob_width-5, tree->ob_height-5, &scr);
/*		objc_draw( tree, 0, MAX_DEPTH, app.x, app.y, app.w, app.h); */
		v_show_c( app->graf.handle, 1);
		while( !wind_update (BEG_MCTRL));
		do{
			graf_mkstate( &mx, &my, &but, &dum);
			dum = objc_find(tree, 0, 7, mx, my);
		}while( dum != -1 && tree[dum].ob_type != G_TITLE);
		
		v_hide_c( app.handle);
		wind_update (END_MCTRL);
		w_put_bkgr( tree->ob_x-3, tree->ob_y-3, tree->ob_width+5, tree->ob_height+5, &scr);
		((SMENU *)pblk->pb_parm)->mn_scroll = 0;
	}
#endif

	if( pblk->pb_currstate & SELECTED)
		((SMENU *)pblk->pb_parm)->mn_scroll = 1;
	else
		((SMENU *)pblk->pb_parm)->mn_scroll = 0;

	return 0;
}
