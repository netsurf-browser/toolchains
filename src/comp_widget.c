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
 * $Source: /cvsroot/windom/windom/src/comp_widget.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/02/26 03:15:22 $
 *   $Revision: 1.14 $
 */

#include "globals.h"
#include "sliders.h"

#define CDT_TEXT		0x33330001
#define CDT_MENU_CONTROL	0x33330002

#define WDT_WDGT_INFO	0x33440001
#define WDT_WDGT_WORK	0x33440002
#define WDT_WDGT_MENU	0x33440003



/* FIXME: from the comp_menu.c */
COMPONENT *comp_menu_create( APPvar *app, OBJECT *menu );
int comp_menu_set(  APPvar *app, WINDOW *win, COMPONENT *c, int mode, int par1, int par2, int par3, int par4);



/**
 * FIXME!!!
 * There should rather exist standalone MENU and INFO COMPONENTs instead of using the
 * WM_REDRAW handlers to use the WINDOW properties. This is a hack to get this WIDGET working
 * with the WINDOM way of managing it WINDOW extensions like MENU, INFO, TOOLBAR.
 *
 * For example the WIDGET COMPONENT as it is now doesn't know which is its MENU child element.
 * It only contains 'some' COMPONENTs that can draw the WINDOW's ->info and ->menu payload.
 *
 * So the desired state is to use MENU_COMPONENT->menu.root instead of WINDOW->menu.root here.
 * All the WINDOW->COMPONENT assignments should only be done in the WidgetCreate()/WindSet()
 * code. Everywhere else the components should be carrier WINDOW fields structure free.
 **/


static
void __CDECL comp_window_info_redraw( COMPONENT *c, long buff[8], APPvar *app ) {
	OBJECT *tree;
	LGRECT work;

	mt_CompGetLGrect(app, c, WF_WORKXYWH, &work);

	/* move the OBJECT to the COMPONENT's place (before clipping!) */
       	tree = (OBJECT*)mt_CompDataSearch(app, c, CDT_OBJECT);
	tree->ob_x = work.g_x;
	tree->ob_y = work.g_y;
	tree->ob_width = work.g_w;
	/* the height should never change for INFO line */

	/* clip by the WM_REDRAW message grect */
	if ( ! rc_lintersect( (LGRECT*)&buff[4], &work ) ) return;

	/* set the info line */
	tree->ob_spec.tedinfo->te_ptext = (char*)mt_CompDataSearch(app, c, CDT_TEXT);

	mt_objc_draw( tree, 0, 8, work.g_x, work.g_y, work.g_w, work.g_h, app->aes_global );
}


static
void __CDECL comp_sizer_click( COMPONENT *c, long buff[8], APPvar *app ) {
	INT16 wh = w_get_hndl(app, mt_CompGetPtr( app, c, CF_WINDOW));
       	short mx, my, omx, omy, ox = mx = omx = app->evnt.mx, oy = my = omy = app->evnt.my;
	short dum, but;
	GRECT r1;

	mt_wind_get_grect( wh, WF_CURRXYWH, &r1, app->aes_global);

	while( !mt_wind_update( BEG_MCTRL, app->aes_global));
	if( app->aes4 & AES4_XGMOUSE) mt_graf_mouse( M_SAVE, 0L, app->aes_global);
	mt_graf_mouse( FLAT_HAND, NULL, app->aes_global);
	do {
		if ( omx != mx || omy != my ) { /* position changed */
			short wbuff[8];
			short dx = mx - ox;
			short dy = my - oy;

			omx = mx;
			omy = my;

			wbuff[0] = WM_SIZED;
			wbuff[3] = wh;
			wbuff[4] = r1.g_x;
			wbuff[5] = r1.g_y;
			wbuff[6] = r1.g_w + dx;
			wbuff[7] = r1.g_h + dy;

			mt_EvntExec(app, mt_WindHandle( app, wh), wbuff );
		}
		mt_evnt_timer( 1L, app->aes_global);
		mt_graf_mkstate( &mx, &my, &but, &dum, app->aes_global);
	} while( but);

	if( app->aes4 & AES4_XGMOUSE && mt_graf_mouse( M_RESTORE, 0L, app->aes_global))
		mt_graf_mouse(ARROW,NULL,app->aes_global);
	mt_wind_update( END_MCTRL, app->aes_global);
}


static
void __CDECL comp_wdgt_focuschange( COMPONENT *c, long buff[8], APPvar *app ) {
	COMPONENT *wwc;

	/* notify menu */
	wwc = (COMPONENT*)mt_CompDataSearch(app, c, WDT_WDGT_MENU);
	if ( wwc ) {
		mt_CompEvntExec( app, wwc, buff);
		mt_CompEvntRedraw( app, wwc);
	}

	/* notify info */
	wwc = (COMPONENT*)mt_CompDataSearch(app, c, WDT_WDGT_INFO);
	if ( wwc ) {
		mt_CompEvntExec( app, wwc, buff);
		mt_CompEvntRedraw( app, wwc);
	}

	/* notify slider components */
	wwc = (COMPONENT*)mt_CompDataSearch(app, c, WDT_WDGT_WORK);
	if ( wwc ) {
		COMPONENT *s;
		s = (COMPONENT*)mt_CompDataSearch(app, wwc, CDT_VSLID);
		if ( s ) {
			mt_CompEvntExec( app, s, buff);
			mt_CompEvntRedraw( app, s);
		}
		s = (COMPONENT*)mt_CompDataSearch(app, wwc, CDT_HSLID);
		if ( s ) {
			mt_CompEvntExec( app, s, buff);
			mt_CompEvntRedraw( app, s);
		}
	}
}


int comp_widget_set(  APPvar *app, WINDOW *win, COMPONENT *wgtc, int mode, int par1, int par2, int par3, int par4) {
	switch( mode) {
		case WF_INFO:
			{
				/* do not use win->info as we should honor the par* args here*/
				char *info = (char*) (long) ( ((long)par1 << 16 ) | ((long)par2 & 0xFFFFL) );

				/* pass to info COMPONENT and redraw */
				COMPONENT *wwc = (COMPONENT*)mt_CompDataSearch(app, wgtc, WDT_WDGT_INFO);
				if ( !wwc ) return E_OK;

				/* set the into text to the component and redraw */
				mt_CompDataAttach( app, wwc, CDT_TEXT, info );
				mt_CompEvntRedraw( app, wwc);
			}
			return E_OK;

		case WF_MENU:
		case WF_MENUTNORMAL:
			{
				/* pass to menu COMPONENT and redraw */
				COMPONENT *wwc = (COMPONENT*)mt_CompDataSearch(app, wgtc, WDT_WDGT_MENU);
				if ( !wwc ) return E_OK;

				comp_menu_set( app, win, wwc, mode, par1, par2, par3, par4 );
			}
			return E_OK;
	}
	return ERROR;
}


COMPONENT *comp_widget_create( APPvar *app, WINDOW *win, int size, int flex ) {
	int SS = 20;
	COMPONENT *wc, *vc, *menu = NULL;
	COMPONENT *wgtc = mt_CompCreate( app,  CLT_VERTICAL, size, flex );

	mt_CompEvntAttach( app, wgtc, CM_REFLOW, std_comp_reflow);
	mt_CompEvntAttach( app, wgtc, CM_GETFOCUS, comp_wdgt_focuschange);
	mt_CompEvntAttach( app, wgtc, CM_LOSEFOCUS,comp_wdgt_focuschange);

	if ( win->status & WS_MENU ) {
		menu = comp_menu_create( app, win->menu.root );
		mt_CompAttach( app, wgtc, menu);
		mt_CompDataAttach( app, wgtc, WDT_WDGT_MENU, menu);
	}

	if ( win->attrib & INFO ) {
		OBJECT *tree = get_frame_slider( app);  /* FIXME: tree may be NULL is FrameInit() has not been called */
		COMPONENT *c = comp_objc_create( app, tree + M_INFO, CLT_VERTICAL, tree[M_INFO].ob_height + 1, 0 );
		mt_CompEvntAttach( app, c, WM_REDRAW, comp_window_info_redraw );
		mt_CompAttach( app, wgtc, c );
		mt_CompDataAttach( app, wgtc, WDT_WDGT_INFO, c);

		/* set the info component data */
		comp_widget_set( app, win, wgtc, WF_INFO, (long)win->info >> 16, (long)win->info & 0xffffUL, 0, 0);
	}

	if ( win->status & WS_TOOLBAR ) {
		/* FIXME: TODO: add upper horizontal toolbar handling
		mt_CompAttach( app, wgtc, c );
		 */
	}

	vc = mt_CompCreate( app, CLT_HORIZONTAL, 0, 1 );
	mt_CompAttach( app, wgtc, vc );

	if ( win->status & WS_TOOLBAR ) {
		/* FIXME: TODO: add left vertical toolbar handling
		mt_CompAttach( app, vc, c );
		 */
	}

	/* WINDOW wrapper COMPONENT - the WORK area component */
	wc = comp_wind_create( app, win, CLT_VERTICAL, 0, 1 );
	wc->status |= CS_INTERACTIVE;
	mt_DataAttach( app, win, WD_CWGT, wgtc ); /* WIDGET COMPONENT link */
	mt_CompDataAttach( app, wgtc, WDT_WDGT_WORK, wc);
	/* set as CONTROL component for menu (if set) */
	if ( menu ) mt_CompDataAttach( app, menu, CDT_MENU_CONTROL, wc);

	mt_CompAttach( app, vc, wc );

	if ( win->attrib & HSLIDE ) {
		COMPONENT *h = mt_CompCreate( app, CLT_HORIZONTAL, SS, 0 );
		mt_CompAttach( app, wgtc, h );

		/* horizontal slider */
		{
			COMPONENT *hslid = comp_slider_create( app, CS_HSLIDE, SS, 0, 1 );
			mt_CompAttach( app, h, hslid );

			/* link the slider to the WINDOW component */
			comp_slider_link( app, wc, hslid, CS_HSLIDE );
		}

		/* sizer */
		if ( win->attrib & SIZER ) {
		     	/* FIXME: tree may be NULL if FrameInit() has not been called */
			OBJECT *tree = get_frame_slider( app);
			COMPONENT *c = comp_objc_create( app, tree + M_SZ, CLT_VERTICAL, SS, 0 );
			mt_CompEvntAttach( app, c, WM_XBUTTON, comp_sizer_click );
			mt_CompAttach( app, h, c );
		}
	}

	if ( win->attrib & VSLIDE ) {
		/* vertical slider */
		COMPONENT *vslid = comp_slider_create( app, CS_VSLIDE, SS, SS, 0 );
		mt_CompAttach( app, vc, vslid );

		/* link the slider to the WINDOW component */
		comp_slider_link( app, wc, vslid, CS_VSLIDE );
	}


#if 0
#define CDT_COLOR 0x434f4cUL  /* 'COL' */
	mt_CompDataAttach( app, wc, CDT_COLOR, (void*)0xFe );
	mt_CompDataAttach( app, wgtc, CDT_COLOR, (void*)0xFF );
#endif

	{
		/* send losefocus to ensure the component is 'disabled' (doesn't have focus) */
		long lfbuff[8] = { CM_LOSEFOCUS };
		mt_CompEvntExec( app, wc, lfbuff );
	}

	/* the complete window component set (including widget ones) */
	return wgtc;
}

