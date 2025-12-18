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
 * $Source: /cvsroot/windom/windom/src/comp_window.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/02/25 03:12:08 $
 *   $Revision: 1.20 $
 */

#include <limits.h>

#include "globals.h"
#include "sliders.h"

static
void __CDECL comp_window_dstry( COMPONENT *c, long buff[8], APPvar *app ) {
	short wbuff[8] = { WM_DESTROY };
	mt_EvntExec(app, (WINDOW*)mt_CompDataSearch(app, c, WD_CCWD), wbuff);
}

static
void __CDECL comp_window_rdwpass( COMPONENT *c, long buff[8], APPvar *app ) {
	short wbuff[8];

	/* copy the message buff (long) to the wbuff (short) */
	long  *lp = buff;
	INT16 *sp = wbuff;
	*sp++ = *lp++; *sp++ = *lp++; *sp++ = *lp++; *sp++ = *lp++;
	*sp++ = *lp++; *sp++ = *lp++; *sp++ = *lp++; *sp++ = *lp++;

	mt_EvntExec(app, (WINDOW*)mt_CompDataSearch(app, c, WD_CCWD), wbuff);
}

static
void __CDECL comp_window_focuschange( COMPONENT *c, long buff[8], APPvar *app ) {
	WINDOW *w = (WINDOW*)mt_CompDataSearch(app, c, WD_CCWD);
	assert(w);

	/* notify WIDGET component */
	{
		COMPONENT *wgtc = mt_DataSearch( app, w, WD_CWGT );
		if ( wgtc ) mt_CompEvntExec( app, wgtc, buff);
	}

	/* emulate AES' WM_TOPPED/BOTTOM for the WINDOW */
	{
		short wbuff[8];
		wbuff[0] = ( buff[0] == CM_GETFOCUS ? WM_TOPPED : WM_BOTTOM );
		mt_EvntExec( app, w, wbuff);
	}
}

static
void __CDECL comp_window_redraw( COMPONENT *c, long buff[8], APPvar *app ) {
	short wbuff[8] = { WM_REDRAW };

	/* clip component by the redraw rectangle */
	{
		LGRECT lwork;
		mt_CompGetLGrect(app, c, WF_WORKXYWH, &lwork);

		if ( ! rc_lintersect( (LGRECT*)&buff[4], &lwork ) ) return;

		/* everything is on screen -> GRECT scope */
		rc_lgrect2grect( (GRECT*)&wbuff[4], &lwork );
	}

	/* intersect with the owner window */
	{
		GRECT work;
		WINDOW *win = mt_CompGetPtr( app, c, CF_WINDOW);

		mt_WindGet(app, win, WF_WORKXYWH, &work.g_x, &work.g_y, &work.g_w, &work.g_h );
		if ( ! rc_intersect( &work, (GRECT*)&wbuff[4] ) ) return;
	}

	/* set clipping in the wrapped window (clipping rectangles are per WINDOW instance) */
	{
		/* get the component held window */
		WINDOW *w = (WINDOW*)mt_CompDataSearch(app, c, WD_CCWD);
		GRECT clip = w->graf->clip;
		mt_ClipOn( app, w->graf, (GRECT*)&wbuff[4]);
		mt_EvntExec( app, w, wbuff );
		mt_ClipOn( app, w->graf, &clip);
	}
}

static
void __CDECL comp_window_rpsd( COMPONENT *c, long buff[8], APPvar *app ) {
	short wbuff[8] = { WM_REPOSED };

	/* LGRECT -> GRECT conversion */
	rc_lgrect2grect( (GRECT*)&wbuff[4], (LGRECT*)&buff[4] );
	mt_EvntExec(app, (WINDOW*)mt_CompDataSearch(app, c, WD_CCWD), wbuff );
}


/**
 * Returns the wrapping AES WINDOW instance for the
 * given WINDOW. To be used in the WS_CUSTOM
 * get_root_handle() handler implementation.
 */
static
WINDOW *comp_window_getaesroot( APPvar* app, WINDOW *win ) {
	COMPONENT *r = mt_DataSearch( app, win, WD_CCWD );
	if ( r ) {
		/* get the root component */
		while ( r->parent ) r = r->parent;
		/* and return the AES WINDOW to which r is hooked to */
		return (WINDOW*)mt_CompDataSearch(app, r, WD_CCRT);
	}
	if ( win->handle < 0 ) return NULL;
	return win;
}

static
int __CDECL comp_wind_get_handle(  APPvar *app, WINDOW *win) {
	WINDOW *root = comp_window_getaesroot( app, win );
	if (root)
	       return root->handle;
	return -1;
}

static
int __CDECL comp_wind_widget_set(  APPvar *app, WINDOW *win, int mode, int par1, int par2, int par3, int par4) {
	COMPONENT *wgtc = mt_DataSearch( app, win, WD_CWGT );
	if (! wgtc) return E_OK;

	return comp_widget_set( app, win, wgtc, mode, par1, par2, par3, par4);
}

static
int __CDECL comp_wind_get(  APPvar *app, WINDOW *win, int mode, GRECT *clip, INT16 *x, INT16 *y, INT16* w, INT16 *h ) {
	COMPONENT *c = mt_DataSearch( app, win, WD_CCWD );
	if (c) {
		int res = E_OK;
		LGRECT lrect;
		COMPONENT *s;

		switch( mode) {
			case WF_FIRSTXYWH:
			case WF_FIRSTAREAXYWH:
			case WF_NEXTXYWH: 
				if ( win->status & WS_OPEN ) {
					WINDOW *root = comp_window_getaesroot(app, win);
					if ( root )
						/* as the FIRST/NEXT XYWH are always intersected with the WORKXYWH
						 * further there is no need to do more here */
						return wind_aes_get( app, root, mode, clip, x, y, w, h);
				}

				/* we don't have the root WINDOW (we are not assigned to any AES WINDOW) */
				*w = *h = 0;
				break;
			case WF_SLIDXYWH:
				/* fallthrough */
			case WF_WORKXYWH:
				res = mt_CompGetLGrect( app, c, WF_WORKCLIPXYWH, &lrect );
				*x = lrect.g_x;
				*y = lrect.g_y;
				*w = lrect.g_w;
				*h = lrect.g_h;
				break;
			case WF_CURRXYWH:
				{
				       	/* WINDOW WIDGET component */
					COMPONENT *wgtc = mt_DataSearch( app, win, WD_CWGT );
					if ( wgtc ) c = wgtc;
				}
				res = mt_CompGetLGrect( app, c, WF_WORKXYWH, &lrect );
				*x = lrect.g_x;
				*y = lrect.g_y;
				*w = lrect.g_w;
				*h = lrect.g_h;
				break;

			/* FIXME? Perhaps we should wrap these into comp_slider_get() and do not
			 *        publish the CDT_* constants at all? That would however require
			 *        to create a sort of slider handlers just like for windows.
			 *
			 *        It seems OK to define the interface as the CDT_SLIDER_* settings
			 *        + CM_REFLOW to update at the time.
			 **/
			case WF_VSLIDE:
			       	s = (COMPONENT*)mt_CompDataSearch(app, c, CDT_VSLID);
				*x = s ? (int)mt_CompDataSearch(app, s, CDT_SLIDER_POS) : 0;
				break;
			case WF_VSLSIZE:
			       	s = (COMPONENT*)mt_CompDataSearch(app, c, CDT_VSLID);
				*x = s ? (int)mt_CompDataSearch(app, s, CDT_SLIDER_SIZE) : 0;
				break;
			case WF_HSLIDE:
			       	s = (COMPONENT*)mt_CompDataSearch(app, c, CDT_HSLID);
				*x = s ? (int)mt_CompDataSearch(app, s, CDT_SLIDER_POS) : 0;
				break;
			case WF_HSLSIZE:
			       	s = (COMPONENT*)mt_CompDataSearch(app, c, CDT_HSLID);
				*x = s ? (int)mt_CompDataSearch(app, s, CDT_SLIDER_SIZE) : 0;
				break;
		}

		/* standard AES WINDOW instance */
		return res; /* FIXME? perhaps later wind_aes_get( app, win, mode, clip, x, y, w, h);*/
	}

	/* no wrapping COMPONENT? */
	return ERROR;
}

static
int __CDECL comp_wind_set(  APPvar *app, WINDOW *win, int mode, int par1, int par2, int par3, int par4) {
	COMPONENT *c = mt_DataSearch( app, win, WD_CCWD );
	if (c) {
		COMPONENT *s;

		switch( mode) {
			case WF_WORKXYWH:
			case WF_CURRXYWH:
				/* FIXME: ? set size is not supported on COMPONENTs */
				return E_OK;

			case WF_VSLIDE:
			case WF_VSLSIZE:
			case WF_HSLIDE:
			case WF_HSLSIZE:
				/* This would logically be handled by the widget component but
				 * sliders are an exception so that it is possible to have sliders
				 * attached to any component with no need of the widget one. */
				{
					INT16 prev, dum;

					/* first check the slider for presence */
					s = (COMPONENT*)mt_CompDataSearch(app, c,
							(mode==WF_HSLIDE || mode==WF_HSLSIZE)
							? CDT_HSLID : CDT_VSLID);
					if ( !s ) return ERROR;

					/* go on only if changed */
					comp_wind_get( app, win, mode, NULL, &prev, &dum, &dum, &dum);
					if ( par1 != prev ) {
						WINDOW *root = comp_window_getaesroot(app, win);
						return comp_slider_set( app, (root ? root : win), s, mode, par1, par2, par3, par4);
					}
				}
				return E_OK;

			case WF_INFO:
				win->info = (void*) (long) ( ((long)par1 << 16 ) | ((long)par2 & 0xFFFFL) );
				return comp_wind_widget_set( app, win, mode, par1, par2, par3, par4);

			case WF_MENU:
				{
					int res = wind_menu_set( app, win, mode,
							(OBJECT*) (long) ( ((long)par1 << 16 ) | ((long)par2 & 0xFFFFL) ),
							(func_evnt) (long) ( ((long)par3 << 16 ) | ((long)par4 & 0xFFFFL) ) );
					if ( res)
						return res;
					return comp_wind_widget_set( app, win, mode, par1, par2, par3, par4);
				}
		}

		/* standard AES WINDOW handling */
		return E_OK; /* FIXME? perhaps later wind_aes_set( app, win, mode, par1, par2, par3, par4); */
	}

	/* no wrapping COMPONENT? */
	return ERROR;
}

static
int __CDECL comp_wind_calc( APPvar *app, WINDOW *win, int type, int x, int y, int w, int h,
						   INT16 *xout, INT16 *yout, INT16 *wout, INT16 *hout) {
	/* FIXME: TODO: ask the WIDGET component for the deltas */
	/* FIXME: REMOVE: for now copy the input to the output rectangle */
	*xout = x;
	*yout = y;
	*wout = w;
	*hout = h;
	return E_OK;
}

static
int __CDECL comp_wind_remove( APPvar *app, WINDOW *win ) {
	/* FIXME: TODO: nothing to do? */
	return E_OK;
}

static
WIND_HANDLERS comp_wind_handlers = {
	comp_wind_get_handle,
	comp_wind_get,
	comp_wind_set,
	comp_wind_calc,
	comp_wind_remove
};
 

COMPONENT *comp_wind_create( APPvar *app, WINDOW *w, short type, long size, short flex ) {
	COMPONENT *c = mt_CompCreate( app, type, size, flex );
	
	/* c{WD_CCWD} <-> w{WD_CCWD} link */
	mt_CompDataAttach( app, c, WD_CCWD, w );
	mt_DataAttach( app, w, WD_CCWD, c );

	/* event handlers */
	mt_CompEvntAttach( app, c, WM_DESTROY, comp_window_dstry);
	mt_CompEvntAttach( app, c, WM_REDRAW,  comp_window_redraw);

	mt_CompEvntAttach( app, c, CM_GETFOCUS, comp_window_focuschange);
	mt_CompEvntAttach( app, c, CM_LOSEFOCUS,comp_window_focuschange);
	mt_CompEvntAttach( app, c, CM_REPOSED, comp_window_rpsd);

	mt_CompEvntAttach( app, c, MN_SELECTED, comp_window_rdwpass);

	mt_CompEvntAttach( app, c, WM_ARROWED, comp_window_rdwpass);
	mt_CompEvntAttach( app, c, WM_VSLID,   comp_window_rdwpass);
	mt_CompEvntAttach( app, c, WM_HSLID,   comp_window_rdwpass);

	mt_CompEvntAttach( app, c, WM_RTLINE,  comp_window_rdwpass);
	mt_CompEvntAttach( app, c, WM_LFLINE,  comp_window_rdwpass);
	mt_CompEvntAttach( app, c, WM_UPLINE,  comp_window_rdwpass);
	mt_CompEvntAttach( app, c, WM_DNLINE,  comp_window_rdwpass);
	mt_CompEvntAttach( app, c, WM_RTPAGE,  comp_window_rdwpass);
	mt_CompEvntAttach( app, c, WM_LFPAGE,  comp_window_rdwpass);
	mt_CompEvntAttach( app, c, WM_UPPAGE,  comp_window_rdwpass);
	mt_CompEvntAttach( app, c, WM_DNPAGE,  comp_window_rdwpass);

	mt_CompEvntAttach( app, c, WM_XKEYBD,  comp_window_rdwpass);
	mt_CompEvntAttach( app, c, WM_XBUTTON, comp_window_rdwpass);
	mt_CompEvntAttach( app, c, WM_XTIMER,  comp_window_rdwpass);

	/* restrict the size to WINDOW capabilities */
	c->bounds.max_width  = SHRT_MAX;
	c->bounds.max_height = SHRT_MAX;
	
	if( ! (w->status & WS_CUSTOM) ) {
		/* remove the AES window instance now */
		if( w->status & WS_OPEN) 
			mt_WindClose( app, w);
		mt_wind_delete( w->handle, app->aes_global);
		mt_RemoveWindow( app, w);
		/* e.g. MagiC still returns valid FIRST/NEXTXYWH for deleted windows */
		w->handle = -1;

		/* WindSet/Get behaviour */
		mt_DataAttach( app, w, WD_WHLR, &comp_wind_handlers ); /* attach the window handlers */
		w->status |= WS_CUSTOM; /* make the WindGet/WindSet/WindCalc use the handlers */
	}

	mt_WindSlider( app, w, VSLIDER|HSLIDER ); /* set the slider size and position */

	/* FIXME:
	 * IMHO this should happen only when the real
	 * AES window opens (all WS_CUSTOM should change to WS_OPEN)
	 * -> would need a new WM_OPENED to be defined or something.
	 */
	w->status |= WS_OPEN;

	return c;
}


