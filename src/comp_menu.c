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
 * $Source: /cvsroot/windom/windom/src/comp_menu.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/02/26 03:15:22 $
 *   $Revision: 1.3 $
 */

#include "globals.h"

#include "mnscroll.h"
#include "mnscroll.rh"

struct MENU_HANDLE_CONTEXT{
	MFDB	fond;
	int	pane;
	int	title;
	int	item;
};


/* FIXME: move to globals.h? */
#define CDT_MENU_CONTROL	0x33330002

COMPONENT *comp_menu_create( APPvar *app, OBJECT *menu );
int comp_menu_set(  APPvar *app, WINDOW *win, COMPONENT *c, int mode, int par1, int par2, int par3, int par4);

static
void mt_objc_get_lgrect( OBJECT *tree, int index, LGRECT *rect, short *aes_global) {
	short x, y;
	mt_objc_offset( tree, index, &x, &y, aes_global);
	rect->g_x = x;
	rect->g_y = y;
	rect->g_w = tree[index].ob_width;
	rect->g_h = tree[index].ob_height;
}

static
void comp_menu_object_redraw( APPvar *app, COMPONENT *c, OBJECT *menu, int index ) {
	LGRECT lob;
	mt_objc_get_lgrect( menu, index, &lob, app->aes_global);
	mt_CompEvntRedrawLGrect( app, c, &lob);
}

static
void __CDECL comp_window_menu_redraw( COMPONENT *c, long buff[8], APPvar *app ) {
	LGRECT lwork;

	mt_CompGetLGrect(app, c, WF_WORKXYWH, &lwork);

	/* clip by the WM_REDRAW message grect */
	if ( rc_lintersect( &lwork, (LGRECT*)&buff[4] ) ) {
		GRECT work, clip;

		rc_lgrect2grect( &clip, (LGRECT*)&buff[4] );
		rc_lgrect2grect( &work, &lwork );

		menu_draw_grect( app,
				 (WINDOW*)mt_CompGetPtr(app, c, CF_WINDOW),
				 (OBJECT*)mt_CompDataSearch(app, c, CDT_OBJECT),
				 &work, &clip);
	}
}


static
void menu_handle_event( APPvar *app, COMPONENT *c, OBJECT *menu,
		short mx, short my, struct MENU_HANDLE_CONTEXT *ctx)
{
	int index = mt_objc_find( menu, 0, MAX_DEPTH, mx, my, app->aes_global);
	if( index == -1 || ob_istype(app,menu,index,G_BOX))
		return;

	/* menu title selection check */
	if( index != ctx->title
			&& ob_istype(app, menu, index, G_TITLE)
			&& !(menu[ index].ob_state & DISABLED) )
	{
		LGRECT ob;

#if 0
		if( mx<x || mx>x+w-1 || my <y || my>y+h-1)
			continue;
		if( mt_wind_find( mx, my, app->aes_global) != win->handle)
			continue;
#endif

		/* is a menu title already selected */
		if( ctx->title !=-1) {
			/* if so, we close it */
			if( ctx->item != -1)
				mt_objc_change( menu, ctx->item, 0, 0,0,0,0, NORMAL, 0, app->aes_global);

			/* restore the background */
			ctx->pane = mt_GetIndexMenu( app, menu, ctx->title);
			mt_objc_get_lgrect( menu, ctx->pane, &ob, app->aes_global);
			w_put_bkgr( app, ob.g_x-10, ob.g_y-10, ob.g_w+20, ob.g_h+20, &ctx->fond);

			/* clean the title selection */
			menu[ctx->title].ob_state &= ~SELECTED;
			comp_menu_object_redraw( app, c, menu, ctx->title );
		}

		/* select the new title object */
		ctx->title = index;
		menu[ctx->title].ob_state |= SELECTED;
		comp_menu_object_redraw( app, c, menu, ctx->title );

		/* save the new pane background */
		ctx->pane = mt_GetIndexMenu( app, menu, ctx->title);
		mt_objc_get_lgrect( menu, ctx->pane, &ob, app->aes_global);
		w_get_bkgr( app, ob.g_x-10, ob.g_y-10, ob.g_w+20, ob.g_h+20, &ctx->fond);

		/* draw the new pane object */
		mt_objc_draw( menu, ctx->pane, MAX_DEPTH, ob.g_x-10, ob.g_y-10, ob.g_w+20, ob.g_h+20, app->aes_global);
#if 0
		if( win->menu.hilight) 
			(*win->menu.hilight)( app, win, ctx->title, -1);
#endif

		ctx->item = -1;
		return;
	}

	/* change of entry in the menu */
	index = mt_objc_find( menu, ctx->pane, MAX_DEPTH, mx, my, app->aes_global);
	/* menu popup pane found -> no change: index = item */
	if (index == ctx->pane)
		index = ctx->item;

	/* item change */
	if( index != ctx->item ) {
		LGRECT lob;
		GRECT ob;
		mt_objc_get_lgrect( menu, ctx->pane, &lob, app->aes_global);
		rc_lgrect2grect( &ob, &lob );

		/* deselect the current item if any */
		if( ctx->item != -1 && !(menu[ctx->item].ob_state & DISABLED)) {
			menu[ctx->item].ob_state &= ~SELECTED;
			mt_objc_draw_grect( menu, ctx->pane, MAX_DEPTH, &ob, app->aes_global);

		}
		/* select the new item if any */
		if( index != -1 && !(menu[index].ob_state & DISABLED)) {
			menu[index].ob_state |= SELECTED;
			mt_objc_draw_grect( menu, ctx->pane, MAX_DEPTH, &ob, app->aes_global);

#if 0
			if( win->menu.hilight)
				(*win->menu.hilight)( app, win, ctx->title, index);
#endif
		}

		ctx->item = index;
	}
}


static
void menu_navigate( APPvar *app, COMPONENT *c, OBJECT *menu,
		short mx, short my, struct MENU_HANDLE_CONTEXT *ctx) { 
	INT16 msg[8], ev;
	short dum;

	ctx->pane = 0;

	menu_handle_event( app, c, menu, mx, my, ctx);
	do {
		short mbut, mkey;
		ev = mt_evnt_multi( MU_M1|MU_BUTTON, 1, 1, 1,
					MO_LEAVE,mx,my,1,1,
					0,0,0,0,0,
					msg,
					0L,
					&mx, &my, &mbut, &mkey,
					&dum, &dum, app->aes_global);

		menu_handle_event( app, c, menu, mx, my, ctx);
	} while( !(ev & MU_BUTTON));

	/* invalidate if the menu item is disabled */
	if ( ctx->item != -1 && menu[ctx->item].ob_state & DISABLED ) {
		ctx->item = -1;
	}

	if ( ctx->pane) {
		LGRECT lob;
		GRECT ob;
		mt_objc_get_lgrect( menu, ctx->pane, &lob, app->aes_global);
		rc_lgrect2grect( &ob, &lob );

		if( ctx->item != -1 ) {
			/* redraws are here not bound to any component as it is on a pane */
			for( dum=0; dum < CONF(app)->menu_effect; dum++) {
				mt_evnt_timer( 50L, app->aes_global);
				mt_objc_change( menu, ctx->item, 0, ob.g_x, ob.g_y, ob.g_w, ob.g_h, NORMAL, 1, app->aes_global);
				mt_evnt_timer( 50L, app->aes_global);
				mt_objc_change( menu, ctx->item, 0, ob.g_x, ob.g_y, ob.g_w, ob.g_h, SELECTED, 1, app->aes_global);
			}

			/* deselect the item */
			menu[ctx->item].ob_state &= ~SELECTED;
		}

		/* restore the background */
		w_put_bkgr( app, ob.g_x-10, ob.g_y-10, ob.g_w+20, ob.g_h+20, &ctx->fond);
	}

	/* note: this needs to be done _after_ the background restore */
	if( ctx->item == -1 ) {
		/* no item selected -> deselect the current title */
		if (ctx->title != -1) {
			menu[ctx->title].ob_state &= ~SELECTED;
			comp_menu_object_redraw( app, c, menu, ctx->title );
		}
	}
}


static
void __CDECL comp_window_menu_click( COMPONENT *c, long buff[8], APPvar *app ) {
	LGRECT work;
	COMPONENT *wc = (COMPONENT *)mt_CompDataSearch(app, c, CDT_MENU_CONTROL);
	OBJECT *menu = (OBJECT*)mt_CompDataSearch(app, c, CDT_OBJECT);
	struct MENU_HANDLE_CONTEXT ctx;

	short mx = buff[3];
	short my = buff[4];
	short mbut, mkey;
	int obj;

	ctx.title = -1;
	ctx.item  = -1;

	mt_CompGetLGrect(app, c, WF_WORKCLIPXYWH, &work);

#if 0
	if( CONF(app)->flag & FLG_MNSCRL) {
		OBJECT *tree = app->priv->mnscroll;
		tree->ob_x = work.g_x;
		tree->ob_y = work.g_y;

		obj = mt_objc_find( tree, 0, MAX_DEPTH, mx, my, app->aes_global);
		switch( obj) {
		case MENURT:
			mt_MenuScroll( app, win, 0); // FIXME!!!
			return;
		case MENULF:
			mt_MenuScroll( app, win, 1); // FIXME!!!
			return;
		}
	}
#endif

	/* lock and wait for the mouse button to be released */
	while( !mt_wind_update( BEG_MCTRL, app->aes_global));
	do mt_graf_mkstate( &mx, &my, &mbut, &mkey, app->aes_global); while( mbut);

	/* set to the current position */
	menu->ob_x = work.g_x;
	menu->ob_y = work.g_y;

	obj = mt_objc_find( menu, 0, MAX_DEPTH, mx, my, app->aes_global);
	if( obj == -1 || !ob_istype(app, menu, obj, G_TITLE)) {
		mt_wind_update( END_MCTRL, app->aes_global);
		return;
	}

	/* handle menu navigation */
	menu_navigate( app, c, menu, mx, my, &ctx);

	/* wait for the mouse button to be released and unlock */
	do mt_graf_mkstate( &mx, &my, &mbut, &mkey, app->aes_global); while( mbut);
	mt_wind_update( END_MCTRL, app->aes_global);

	/* do not send the message if empty */
	if (ctx.item == -1)
		return;

	/* execute MN_SELECTED */
	if ( wc ) {
		long cbuff[8];
		cbuff[0] = MN_SELECTED;
		cbuff[1] = mt_AppId(app);
		cbuff[2] = 0;
		cbuff[3] = ctx.title;
		cbuff[4] = ctx.item;
		cbuff[5] = (long)menu >> 16;
		cbuff[6] = (long)menu & 0xffffU;
		cbuff[7] = obj_root(menu, buff[4]);
		mt_CompEvntExec( app, wc, cbuff);
	}
}


int comp_menu_set(  APPvar *app, WINDOW *win, COMPONENT *c, int mode, int par1, int par2, int par3, int par4) {
	switch( mode) {
		case WF_MENU:
			{
				/* do not use win->menu.* as we should honor the par* args here*/
				OBJECT *menu = (OBJECT*) (long) ( ((long)par1 << 16 ) | ((long)par2 & 0xFFFFL) );
				
				/* set the into text to the component */
				if ( menu ) {
					mt_CompDataAttach( app, c, CDT_OBJECT, menu );
					c->size = menu[1].ob_height + 1;
				} else {
					mt_CompDataDelete( app, c, CDT_OBJECT );
					c->size = 0;
				}

				{
					/* need to reflow the WIDGET COMPONENT as the
					 * size of the MENU component changed */
					COMPONENT *wgtc = mt_DataSearch( app, win, WD_CWGT );
					long buff[8] = { CM_REFLOW };
					mt_CompEvntExec( app, wgtc, buff);
				}
				
				/* redraw */
				mt_CompEvntRedraw( app, c);
			}
			break;

		case WF_MENUTNORMAL:
			{
				OBJECT *menu = (OBJECT*)mt_CompDataSearch(app, c, CDT_OBJECT);

				/* place the menu OBJECT to the right place */
				LGRECT lob;
				mt_CompGetLGrect(app, c, WF_WORKXYWH, &lob);
				menu->ob_x = lob.g_x;
				menu->ob_y = lob.g_y;
				menu[1].ob_width  = lob.g_w;

				if( par2)
					menu[par1].ob_state &= ~SELECTED;
				else
					menu[par1].ob_state |= SELECTED;

				comp_menu_object_redraw( app, c, menu, par1 );
			}
			break;
	}

	return E_OK;
}


COMPONENT *comp_menu_create( APPvar *app, OBJECT *menu ) {
	COMPONENT *c = mt_CompCreate( app, CLT_VERTICAL, menu[1].ob_height + 1, 0 );
	mt_CompDataAttach( app, c, CDT_OBJECT, menu );
	mt_CompEvntAttach( app, c, WM_REDRAW, comp_window_menu_redraw );
	mt_CompEvntAttach( app, c, WM_XBUTTON, comp_window_menu_click );
	return c;
}
