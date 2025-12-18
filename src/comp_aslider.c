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
 * $Source: /cvsroot/windom/windom/src/comp_aslider.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/01/07 04:48:13 $
 *   $Revision: 1.11 $
 */

#include "globals.h"

/**
 * Using AES slider to control COMPONENT.
 * 
 * It can be done using the mt_WindSliderLink() function. It makes the
 * AES WINDOW instance WS_CUSTOM and install handlers so that the
 * wind_set( WF_[VH]SLIDE/SIZE ) calls actually adjust the COMPONENT's
 * sliders position.
 *
 * It also replaces the WM_SLIDEXY handler for the move_work (and
 * therefore any std_'slide' hander actually affects the COMPONENT
 * instead of the whole WINDOW area.
 *
 * @note FIXME: This file may be only temporary storage for this
 *       functionality and as well as the function names is subject
 *       to change.
 */ 

static
int __CDECL container_slider_get( APPvar *app, WINDOW *win, int mode, GRECT *clip, INT16 *x, INT16 *y, INT16* w, INT16 *h ) {
	switch( mode) {
		case WF_SLIDXYWH:
		{
			COMPONENT *c;
			GRECT  work;
			LGRECT lwork;

			/* the default is the window work rectangle */
			wind_aes_get( app, win, WF_WORKXYWH, clip, &work.g_x, &work.g_y, &work.g_w, &work.g_h);
			rc_grect2lgrect( &lwork, &work );

			/* only size matters */
			*x = 0;
			*y = 0;
			*w = work.g_w;
			*h = work.g_h;

			/* horizontal slider component width */
			c = mt_DataSearch(app,win,CDT_HSLID);
			if ( c && c->status & CS_HSLIDE ) {
				LGRECT crect;
				mt_CompGetLGrect( app, c, WF_WORKCLIPXYWH, &crect);
				rc_lintersect( &lwork, &crect );
				*w = crect.g_w;
			}

			/* vertical slider component height */
			c = mt_DataSearch(app,win,CDT_VSLID);
			if ( c && c->status & CS_VSLIDE ) {
				LGRECT crect;
				mt_CompGetLGrect( app, c, WF_WORKCLIPXYWH, &crect);
				rc_lintersect( &lwork, &crect );
				*h = crect.g_h;
			}
			return 0;
		}
	}
	/* standard AES WINDOW handling for others */
	return wind_aes_get( app, win, mode, clip, x, y, w, h);
}


static
int __CDECL container_slider_set(  APPvar *app, WINDOW *win, int mode, int par1, int par2, int par3, int par4) {
	COMPONENT *p;
	switch( mode) {
	 	/* Here actually the win slider values serves to slide the assigned component */
		case WF_HSLIDE:
		case WF_HSLSIZE:
			p = mt_DataSearch(app,win,CDT_HSLID);
			if ( p && p->status & CS_HSLIDE )
				mt_CompSet( app, p, CF_HSLIDE, win->xpos, win->xpos_max, win->w_u, 0 );
			break; /* we want the AES wind_set() to be called */
		case WF_VSLIDE:
		case WF_VSLSIZE:
			p = mt_DataSearch(app,win,CDT_VSLID);
			if ( p && p->status & CS_VSLIDE )
				mt_CompSet( app, p, CF_VSLIDE, win->ypos, win->ypos_max, win->h_u, 0 );
			break; /* we want the AES wind_set() to be called */
	}
	/* standard AES WINDOW handling */
	return wind_aes_set( app, win, mode, par1, par2, par3, par4);
} 

static
WIND_HANDLERS container_slider_handlers = {
	wind_aes_get_root_handle,
	container_slider_get,
	container_slider_set,
	wind_aes_calc,
	wind_aes_remove
};


static
void __CDECL container_slider_sldxy( WINDOW *win, short wbuff[8], APPvar *app) {
	COMPONENT *hc = mt_DataSearch(app,win,CDT_HSLID);
	COMPONENT *vc = mt_DataSearch(app,win,CDT_VSLID);
	LGRECT lrect, clip;
	int dx = wbuff[4];
	int dy = wbuff[5];

	if (!dx && !dy) return;

	/* update the sliders _before_ we call redraw (it might depend on the slider possitions) */
	mt_WindSlider( app, win, (dx?HSLIDER:0) | (dy?VSLIDER:0) );

	rc_grect2lgrect( &clip, &win->graf->clip );

	/* horizontal move */ 
	if ( hc ) {
		mt_CompGetLGrect( app, hc, WF_WORKCLIPXYWH, &lrect);
		if ( rc_lintersect( &clip, &lrect) ) {
			GRECT rect;
			rc_lgrect2grect( &rect, &lrect );

			move_work_rect( app, win, &rect, dx, hc != vc ? 0 : dy);
		}
	}

	/* vertical move (if different to the horizontal) */ 
	if ( vc && vc != hc ) {
		mt_CompGetLGrect( app, vc, WF_WORKCLIPXYWH, &lrect);
		if ( rc_lintersect( &clip, &lrect) ) {
			GRECT rect;
			rc_lgrect2grect( &rect, &lrect );

			move_work_rect( app, win, &rect, 0, dy);
		}
	}
}

void mt_WindSliderLink( APPvar *app, WINDOW *win, int mode, COMPONENT *c) {
	/* link the WindSet() to the slide COMPONENT c */
	mt_DataAttach( app, win, mode == CS_VSLIDE ? CDT_VSLID : CDT_HSLID, c );
	/* override the move_work() functionality so that we would only move
	 * the appropriate component contents */
	mt_EvntAttach( app, win, WM_SLIDEXY, container_slider_sldxy );

	if ( !(win->status & WS_CUSTOM) ) {
		mt_DataAttach( app, win, WD_WHLR, &container_slider_handlers );
		win->status |= WS_CUSTOM;
	}
}
