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
 * $Source: /cvsroot/windom/windom/src/mnscroll.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/02/25 03:25:22 $
 *   $Revision: 1.13 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

#include "mnscroll.h"
#include "mnscroll.rh"
#include "mnscroll.rsh"

void init_scroll_menu( APPvar *app) {
	int dum;
	static int init=0;
	
	if (init==0) {
		for( dum=0; dum<NUM_OBS; mt_rsrc_obfix( rs_object, dum++, app->aes_global))
			;
		init = 1;
	}
	
	app->priv->mnscroll = mt_ObjcDup( app, rs_trindex[MNSCROLL], NULL);
	objc_extended( app, RSRC_XALL, app->priv->mnscroll);
}

void exit_scroll_menu( APPvar *app) {
	if (app->priv->mnscroll) {
		objc_extfree(app,app->priv->mnscroll);
		mt_ObjcFree(app,app->priv->mnscroll);
	}
}

void menu_draw_grect( APPvar *app, WINDOW *win, OBJECT *menu, GRECT *work, GRECT *clip) {
	OBJECT *tree = app->priv->mnscroll;
	
	menu->ob_x = work->g_x;
	menu->ob_y = work->g_y;
	menu[1].ob_width  = work->g_w;

	if( CONF(app)->flag & FLG_MNSCRL) {
		INT16 i;
		/*	shift the menu */
		for( i = 0; i < win->menu.scroll; i++)
			menu->ob_x -= menu[3+i].ob_width;
	
		/* This should go in WindSet (TODO, FIXME?) */
		menu[0].ob_width = 10000;
		menu[1].ob_width = 10000;
		menu[2].ob_width = 10000;
		/* --------------------- */
		
		mt_objc_draw_grect( menu, 1, 2, clip, app->aes_global);
		
		tree[MENULF].ob_flags =  menu[1].ob_flags;
		tree[MENURT].ob_flags =  menu[1].ob_flags;
		tree->ob_height =  menu[1].ob_height;

		/* draw scrolling arrows */
		if( win->menu.scroll > 0)
			tree[MENULF].ob_state &= ~DISABLED;
		else
			tree[MENULF].ob_state |= DISABLED;
		
		if( menu[3+win->menu.scroll].ob_next != 2 )
			tree[MENURT].ob_state &= ~DISABLED;
		else
			tree[MENURT].ob_state |= DISABLED;

		tree->ob_y = work->g_y;
		tree->ob_x = work->g_x;
		mt_objc_draw_grect( tree, 0, 1, clip, app->aes_global);
	} else
		mt_objc_draw_grect( menu, 1, 2, clip, app->aes_global);

	{ /* dividing line under the GEM menu */
		INT16 xy[4];

		vsl_color( app->graf.handle, BLACK);
		vsl_width( app->graf.handle, 1);
		vsl_type( app->graf.handle, 1);
		xy[0] = clip->g_x; xy[1] = clip->g_y;
		xy[2] = clip->g_x + clip->g_w - 1;
		xy[3] = clip->g_y + clip->g_h - 1;
		vs_clip( app->graf.handle, 1, xy);
		mt_objc_offset( menu, 1, &xy[0], &xy[1], app->aes_global);
		xy[1] += menu[1].ob_height;		/* +1 for the line */
		xy[2]  = xy[0] + menu[1].ob_width - 1;
		xy[3]  = xy[1];
		v_pline( app->graf.handle, 2, xy);
		vs_clip( app->graf.handle, 0, xy);
	}
}

/*  Draw the menubar within a window
 *  this function is automatically called by EventWinDom when a REDRAW
 *  message is received for this window
 */
void wind_menu_draw( APPvar *app, WINDOW *win, GRECT *clip) {
	GRECT  work;
	mt_wind_get_grect( win->handle, WF_WORKXYWH, &work, app->aes_global);
	menu_draw_grect( app, win, win->menu.root, &work, clip);
}

/*  Manage menu in window
 *  This function is automatically called when a mouse button event
 *  on this window is received.
 *  Return -1 if no title has been selected.
 */

int frm_menu( APPvar *app, WINDOW *win) {
	int title;
	int item;
	int index, box;
	OBJECT *menu;
	INT16 x, y, w, h, xob, yob;
	INT16 mx, my, mbut, mkey;
	INT16 msg[8], ev;
	INT16 dum;
	MFDB fond;
	OBJECT *tree = app->priv->mnscroll;

	menu = win -> menu.root;
	mt_wind_get( win->handle, WF_WORKXYWH, &x, &y, &w, &h, app->aes_global);

	if( CONF(app)->flag & FLG_MNSCRL) {
		tree -> ob_x = x;
		tree -> ob_y = y;

		title = mt_objc_find( tree, 0, MAX_DEPTH, app->evnt.mx, app->evnt.my, app->aes_global);
		switch( title) {
		case MENURT:
			mt_MenuScroll( app, win, 0);
			return -1;
		case MENULF:
			mt_MenuScroll( app, win, 1);
			return -1;
		}
	}

	while( !mt_wind_update( BEG_MCTRL, app->aes_global));

	/* wait for the mouse button released */
	
	do
		mt_graf_mkstate( &app->evnt.mx, &app->evnt.my, &app->evnt.mbut, &app->evnt.mkstate, app->aes_global);
	while( app->evnt.mbut);


	title = mt_objc_find( menu, 0, MAX_DEPTH, app->evnt.mx, app->evnt.my, app->aes_global);
	if( title == -1 || !ob_istype(app, menu,title,G_TITLE)) {
		mt_wind_update( END_MCTRL, app->aes_global);
		return -1;
	}

	item = -1;
	title = -1;
	box = 0;
	do {
		ev = 
		mt_evnt_multi( MU_TIMER|MU_BUTTON, 1, 1, 1,
					0,0,0,0,0,
					0,0,0,0,0,
					msg,
					0L,
					&mx, &my, &mbut, &mkey,
					&dum, &dum, app->aes_global);
		
		/***********************************************/
				
		index = mt_objc_find( menu, 0, MAX_DEPTH, mx, my, app->aes_global);
		if( index == -1 || ob_istype(app,menu,index,G_BOX))
			continue;

		if( ob_istype(app, menu, index,G_TITLE)
			&& !(menu[ index].ob_state & DISABLED)
			&& index != title) 	/* change of menu title selected */
		{
			if( mx<x || mx>x+w-1 || my <y || my>y+h-1)
				continue;
			if( mt_wind_find( mx, my, app->aes_global) != win->handle)
				continue;
			if( title !=-1)	/* is a menu title already selected ? */
			{				/* if so, we close the previous one. */
				if( item != -1)
					mt_objc_change( menu, item, 0, x, y, w, h, NORMAL, 0, app->aes_global);
				box = mt_GetIndexMenu( app, menu, title);
				mt_objc_offset( menu, box, &xob, &yob, app->aes_global);
				w_put_bkgr( app, xob-10, yob-10, menu[box].ob_width+20, menu[box].ob_height+20, &fond);

				mt_MenuTnormal(app,win,title,1);
			}
			title = index;
			mt_MenuTnormal(app,win,title,0);

			box = mt_GetIndexMenu( app, menu, title);
			mt_objc_offset( menu, box, &xob, &yob, app->aes_global);
			w_get_bkgr( app, xob-10, yob-10, menu[box].ob_width+20, menu[box].ob_height+20, &fond);
			mt_objc_draw( menu, box, MAX_DEPTH, xob-10, yob-10, 
					   menu[box].ob_width+20, menu[box].ob_height+20, app->aes_global);
			item = -1;
			if( win->menu.hilight) 
				(*win->menu.hilight)( app, win, title, -1);
		} else { 
			
			/* change of entry in the menu */
			
			index = mt_objc_find( menu, box, MAX_DEPTH, mx, my, app->aes_global);
			if (index == box) index = item;
			if( item != -1 && item != index 
				&& !(menu[item].ob_state & DISABLED)) {
				menu[item].ob_state &= ~SELECTED;
				mt_objc_draw( menu, box, 2, xob, yob, 
					   menu[box].ob_width, menu[box].ob_height, app->aes_global);
				
			}
			if( index != -1 && index != item
				&& !(menu[index].ob_state & DISABLED)) {
				menu[index].ob_state |= SELECTED;
				mt_objc_draw( menu, box, 2, xob, yob, 
					   menu[box].ob_width, menu[box].ob_height, app->aes_global);
				
				if( win->menu.hilight)
					(*win->menu.hilight)( app, win, title, index);
			}
			item = index;

		}
	} while( ! (ev & MU_BUTTON));

	if( item != -1 && !(menu[item].ob_state & DISABLED))
		for( dum=0; dum < CONF(app)->menu_effect; dum++)
		{
			mt_evnt_timer( 50L, app->aes_global);
			mt_objc_change( menu, item, 0, xob, yob,menu[box].ob_width, 
					 	 menu[box].ob_height, NORMAL, 1, app->aes_global);
			mt_evnt_timer( 50L, app->aes_global);
			mt_objc_change( menu, item, 0, xob, yob,menu[box].ob_width, 
					 	 menu[box].ob_height, SELECTED, 1, app->aes_global);
		}
	
	if(box)
		w_put_bkgr( app, xob-10, yob-10, menu[box].ob_width+20, menu[box].ob_height+20, &fond);

	do
		mt_graf_mkstate( &mx, &my, &mbut, &mkey, app->aes_global);
	while( mbut);

	mt_wind_update( END_MCTRL, app->aes_global);
	
	if( item != -1 && !(menu[item].ob_state & DISABLED) ) {
		mt_objc_change( menu, item, 0, x, y, w, h, NORMAL, 0, app->aes_global);
	} else 
		item = -1;
	
	if (item != -1) {
		/* result converted to standard AES message for this application */
		mt_ApplWrite( app, mt_AppId(app), MN_SELECTED, title, item, ADR(menu), obj_root(menu,item));
	} else {
		if (title != -1) {
			mt_MenuTnormal(app,win,title,1);
		}
	}
	
	return 0;
	
}


#ifdef BETAMENU
#define WMNU 678

static void click_menu( WINDOW *win) {

}

static WINDOW *open_menu( WINDOW *root, int index) {
	WINDOW *win;
	GRECT coord;
	OBJECT *tree;

	tree = MENU(root);
	coord.g_x = tree[index].ob_x;
	coord.g_y = tree[index].ob_y;
	coord.g_w = tree[index].ob_width;
	coord.g_h = tree[index].ob_height;
	win = FormCreate( &tree[index], 0, NULL, "", &coord, 0, 0);
	DataAttach( win, WMNU, root);
	EvntAttach( win, WM_XBUTTON, click_menu);
	win->type = WMNU;
	return win;
}

static void close_menu( WINDOW *win) {
	DataDelete( win, WMNU);
	EvntExec( win, WM_DESTROY);
}

int frm_menu_beta( APPvar *app, WINDOW *win) {
	int title, item;
	int index, box, ev;
	OBJECT *menu;
	INT16 x, y, w, h;
	EVNTvar old;
	WINDOW *wbox = NULL;
	OBJECT *tree = app->priv->mnscroll;
	int EvntWindom( int);
	int ObjcChange( int, void*, int, int, int);
		
	menu = win -> menu.root;
	wind_get( win->handle, WF_WORKXYWH, &x, &y, &w, &h);


	while( !wind_update( BEG_MCTRL));
	index = objc_find( menu, 0, MAX_DEPTH, evnt.mx, evnt.my);

	if( CONF(app)->flag & FLG_MNSCRL) {
		tree -> ob_x = x;
		tree -> ob_y = y;

		switch( index) {
		case MENURT:
			MenuScroll( win, 0);
			return -1;
		case MENULF:
			MenuScroll( win, 1);
			return -1;
		}
	}


	/* wait for mouse-button unpressed */	
	do
		graf_mkstate( &evnt.mx, &evnt.my, &evnt.mbut, &evnt.mkstate);
	while( evnt.mbut);


	if( index == -1 || !ob_istype( app, menu, index, G_TITLE)) {
		if( menu->ac_title != -1) {
			/* invalider le menu */
			menu->ac_title = -1
		}
		
		wind_update( END_MCTRL);
		return -1;
	}

	item = -1;
	title = -1;
	box = 0;
	old = evnt;

	do {

		ev = EvntWindom( MU_MESAG | MU_BUTTON);
		
		if( ev & MU_BUTTON) {
		
			index = objc_find( menu, 0, MAX_DEPTH, evnt.mx, evnt.my);
			if( index == -1)	continue;
		
			/* change of title menu */
			if( ob_istype(app, menu, index, G_TITLE)
				&& !(menu[ index].ob_state & DISABLED)
				&& index != title) {
				if( evnt.mx<x || evnt.mx>x+w-1 || evnt.my<y || evnt.my>y+h-1)
					continue;
				if( wind_find( evnt.mx, evnt.my) != win->handle)
					continue;
			
				/* is a menu title already selected ? */
				if( title != -1)	{
					/* close the previous one */
					if( item != -1)
						objc_change( menu, item, 0, x, y, w, h, NORMAL, 0);
					box = mt_GetIndexMenu( app, menu, title);
					close_menu( wbox);
					/* FIXME! Cannot use the ObjcWindChange as the menu[title] is
					 * not in the WM_WORKXYWH... */
					ObjcWindChange( win, menu, title, x, y, w, h, NORMAL);
				}
				ObjcWindChange( win, menu, index, x, y, w, h, SELECTED);
				title = index;
				box = mt_GetIndexMenu( app, menu, title);
				wbox = open_menu( menu, box);
			} else { 
			
				/* change of menu entry in the menu */
			
				index = objc_find( menu, box, MAX_DEPTH, evnt.mx, evnt.my);
				if( item != -1 && item != index 
					&& !(menu[item].ob_state & DISABLED))
					ObjcChange( OC_FORM, wbox, item, NORMAL, 1);
				if( index != -1 && index != item
					&& !(menu[index].ob_state & DISABLED))
					ObjcChange( OC_FORM, wbox, index, SELECTED, 1);
				item = index;
			}
		}
		graf_mkstate( &evnt.mx, &evnt.my, &evnt.mbut, &evnt.mkstate);
	} while( !evnt.mbut);

	/* flashing effect */
	if( item != -1 && !(menu[item].ob_state & DISABLED)) {
		int dum;

		for( dum=0; dum < CONF(app)->menu_effect; dum++) {
			evnt_timer( 50L);
			ObjcChange( OC_FORM, wbox, item, NORMAL, 1);
			evnt_timer( 50L);
			ObjcChange( OC_FORM, wbox, item, SELECTED, 1);
		}
	}
	if(box)
		close_menu( wbox);
	do
		graf_mkstate( &evnt.mx, &evnt.my, &evnt.mbut, &evnt.mkstate);
	while( evnt.mbut);

	wind_update( END_MCTRL);
	evnt = old;
	
	if( item != -1 && !(menu[item].ob_state & DISABLED) )
		objc_change( menu, item, 0, x, y, w, h, NORMAL, 0);
	else 
		item = -1;

	/* result converted to standard AES message for this application */
	ApplWrite( app.id, MN_SELECTED, title, item, ADR(menu), obj_root(menu,item));
	return 0;
}
#endif
