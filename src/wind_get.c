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
 * $Source: /cvsroot/windom/windom/src/wind_get.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/02/26 02:09:10 $
 *   $Revision: 1.25 $
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include "globals.h"
#include "cookie.h"

/**
 * @brief Get window attributes.
 *
 * @param app application descriptor,
 * @param win target window
 * @param mode attribute to read,
 * @param clip rectagle to optimize the result,
 * @param x,y,w,h meanning depend on value of \e mode.
 * @return 0 or a negative code error.
 *
 * This an internal WinDom function not to be used by applications.
 * They should use mt_WindGet().
 *
 * @sa mt_WindGet(), mt_WindSet()
 */
int __CDECL wind_aes_get( APPvar *app, WINDOW *win, int mode, GRECT *clip, INT16 *x, INT16 *y, INT16* w, INT16 *h ) {
	INT16 xc, yc, wc, hc;
	int res=1;
	GRECT gin, gout;
	void* proc;

	switch( mode) {
	case WF_MENU:
		*x = (WORD)( ((LONG)(win->menu.root)>>16) & 0x0000FFFF);
		*y = (WORD)( (LONG)(win->menu.root) & 0x0000FFFF);
		proc = mt_EvntFind( app, win, MN_SELECTED);
		*w = (WORD)( ((LONG)(proc)>>16) & 0x0000FFFF);
		*h = (WORD)( (LONG)(proc) & 0x0000FFFF);		
		break;
	case WF_TOOLBAR:
		*x = (WORD)( ((LONG)(win->tool.root)>>16) & 0x0000FFFF);
		*y = (WORD)( (LONG)(win->tool.root) & 0x0000FFFF);
		proc = mt_EvntFind( app, win, WM_TOOLBAR);
		*w = (WORD)( ((LONG)(proc)>>16) & 0x0000FFFF);
		*h = (WORD)( (LONG)(proc) & 0x0000FFFF);		
		break;
	case WF_HILIGHT:
		*x = (WORD)( ((LONG)(win->menu.hilight)>>16) & 0x0000FFFF);
		*y = (WORD)( (LONG)(win->menu.hilight) & 0x0000FFFF);		
		break;
	case WF_COMPONENT:
		proc = win_comp_get( app, win);
		*x = (WORD)( ((LONG)(proc)>>16) & 0x0000FFFF);
		*y = (WORD)( (LONG)(proc) & 0x0000FFFF);		
		break;
	case WF_FTOOLBAR:
	case WF_NTOOLBAR:
		res = mt_wind_get( win->handle, mode == WF_FTOOLBAR?WF_FIRSTXYWH:WF_NEXTXYWH,
				  &gin.g_x, &gin.g_y, &gin.g_w, &gin.g_h,app->aes_global);
		rc_set( &gout, win->tool.root->ob_x, win->tool.root->ob_y,
			 		win->tool.root->ob_width, win->tool.root->ob_height);
		rc_intersect( &gin, &gout);
		*x = gout.g_x;
		*y = gout.g_y;
		*w = gout.g_w;
		*h = gout.g_h;
		break;
	case WF_SLIDXYWH:
		mode = WF_WORKXYWH;
		/* fallthrough: it is the same for common AES window */
	case WF_WORKXYWH:
		res = mt_wind_get( win -> handle, mode, x, y, w, h, app->aes_global);
		
		if( win->status & WS_ICONIFY)
			break;

		if (app->aes4 & AES4_SETWORKXYWH) {
			if( win->status & WS_MENU && !( SYSMENU(app) && win->attrib & MENUBAR) ) {
				*y += (win->menu.root[1].ob_height+1);
				*h -= (win->menu.root[1].ob_height+1);
			}
			if( win->status & WS_TOOLBAR ) {
				if( win->tool.root->ob_width == app->w) {
					*y += win->tool.root->ob_height;
					*h -= win->tool.root->ob_height;
				} else {
					*x += win->tool.root->ob_width;
					*w -= win->tool.root->ob_width;
				}	
			}
		} else {  /* wind_get(WF_WORKXYWH) may be bugged, so we use WF_CURRXYWH+WindCalc() instead */
			res = mt_wind_get( win->handle, WF_CURRXYWH, &xc, &yc, &wc, &hc, app->aes_global);
			if( wc == 0 && hc == 0) {
				mt_WindCalc( app, WC_WORK, win, win->createsize.g_x, win->createsize.g_y,
								           win->createsize.g_w, win->createsize.g_h, x, y, w, h);
			} else
				mt_WindCalc( app, WC_WORK, win, xc, yc, wc, hc, x, y, w, h);
		}
		break;
	case WF_CURRXYWH:
		res = mt_wind_get( win->handle, WF_CURRXYWH, x, y, w, h, app->aes_global);
		if( *w == 0 && *h == 0) {
			*x = win->createsize.g_x;
			*y = win->createsize.g_y;
			*w = win->createsize.g_w;
			*h = win->createsize.g_h;
		}
		break;
	case WF_BEVENT:
		if( win->status & WS_UNTOPPABLE)
			*x |= BEVENT_WORK;
		else
			*x &= ~BEVENT_WORK;
		if( win->status & WS_MODAL)
			*x |= BEVENT_MODAL;
		else
			*x &= ~BEVENT_MODAL;
		break;
	case WF_ICONIFY:
		mt_wind_get( win->handle, WF_CURRXYWH, x, h, y, w, app->aes_global);
		*x=(win->status&WS_ICONIFY)?1:0;
		if( *x == 0 ) {
			*y = CONF(app)->wicon;
			*w = CONF(app)->hicon;
		}
		break;
	case WF_UNICONIFY:
		*x=win->icon.x;
		*y=win->icon.y;
		*w=win->icon.w;
		*h=win->icon.h;
		break;
	case WF_BOTTOM:
		if( !(app->aes4 & AES4_BOTTOM)) {
			WINDOW *window = wlst_get_bottom(app);
			*x = window?window->handle:-1;
		} else
			res = mt_wind_get( 0, WF_BOTTOM, x, 0, 0, 0, app->aes_global);
			
		break;
	case WF_TOP:
		/* BUG? FIXME! i think the following WM_UNTOPPED patch should be moved into
		   mt_WindSet(WF_TOP) code */
		/* Emulate WM_UNTOPPED 
		 * the previous topmost window will receive 
		 * a WM_UNTOPPED message */
		
		if( !(app->aes4 & AES4_UNTOPPED)){
			WINDOW *wtop;
			INT16 dum, dum2;
						
			mt_wind_get( win->handle, WF_TOP, &dum, &dum2, &dum2, &dum2, app->aes_global);
			wtop= mt_WindHandle( app, dum);
			if( wtop)
				mt_snd_msg( app, wtop, WM_UNTOPPED, 0, 0, 0, 0);
		}
		res = mt_wind_get( 0, WF_TOP, x, 0, 0, 0, app->aes_global);
		break;

	case WF_INFO:
		*x = (WORD)( ((LONG)(win->info)>>16) & 0x0000FFFF);
		*y = (WORD)( (LONG)(win->info) & 0x0000FFFF);
		break;
	case WF_NAME:
		*x = (WORD)( ((LONG)(win->name)>>16) & 0x0000FFFF);
		*y = (WORD)( (LONG)(win->name) & 0x0000FFFF);
		break;
	case WF_ICONTITLE:
		*x = (WORD)( ((LONG)(win->icon.name)>>16) & 0x0000FFFF);
		*y = (WORD)( (LONG)(win->icon.name) & 0x0000FFFF);
		break;
	case WF_ICONDRAW:
		proc = mt_EvntFind( app, win, WM_ICONDRAW);
		*x = (WORD)( ((LONG)(proc)>>16) & 0x0000FFFF);
		*y = (WORD)( (LONG) (proc) & 0x0000FFFF);
		break;
	case WF_FIRSTXYWH:
	case WF_FIRSTAREAXYWH:
		/* optimize if possible */
		if (clip && app->aes4 & AES4_FIRSTAREAXYWH) {
			res = mt_wind_xget_grect( win->handle, WF_FIRSTAREAXYWH, clip, &gout, app->aes_global);
			*x = gout.g_x;
			*y = gout.g_y;
			*w = gout.g_w;
			*h = gout.g_h;
		} else
			res = mt_wind_get( win->handle, WF_FIRSTXYWH, x, y, w, h, app->aes_global);
		break;		
	default:
		if (clip) {
			res = mt_wind_xget_grect( win->handle, mode, clip, &gout, app->aes_global);
			*x = gout.g_x;
			*y = gout.g_y;
			*w = gout.g_w;
			*h = gout.g_h;
		} else
			res = mt_wind_get( win->handle, mode, x, y, w, h, app->aes_global);
		break;
	}
	/* WinDom returns 0 on success (unlike AES which does the opposite) */
	return (res != 0) ? E_OK : ERROR;
}


int __CDECL wind_aes_get_root_handle(  APPvar *app, WINDOW *win) {
	return win->handle;
}


/**
 * @brief Get window attributes.
 *
 * @param app application descriptor,
 * @param win target window (or \b NULL in some case),
 * @param mode attribute to read,
 * @param clip rectagle to optimize the result,
 * @param x,y,w,h meanning depend on value of \e mode. For detail, read manual
 *  of mt_WindSet().
 *   - \b WF_FTOOLBAR : unless we use AES4 toolbar, this message is useless with
 *      WinDom toolbar system (see mt_WindSet(WF_TOOLBAR)). WinDom redraw itsel the 
 *      toolbar area. However, for AES4 comptability, mt_WindGet() returns correct values
 *      for this mode,
 *   - \b WF_NTOOLBAR : same remark than for \b F_FTOOLBAR.
 * @return 0 or a negative code error.
 *
 * Use this function instead of mt_wind_get(). 
 *
 * @sa mt_WindSet()
 */
int mt_WindXGet( APPvar *app, WINDOW *win, int mode, GRECT *clip, INT16 *x, INT16 *y, INT16* w, INT16 *h ) {
	/* win == NULL means the desktop is targetted - handle = 0 */
	if ( !win ) win = app->priv->desk;

	/* the window is embeded in the COMPONENT or FRAME */
	if ( win->status & WS_CUSTOM ) {
		WIND_HANDLERS *whandlers = mt_DataSearch( app, win, WD_WHLR );
		if (whandlers) {
		       if (whandlers->get)
			       return whandlers->get( app, win, mode, clip, x, y, w, h);
		       return 0;
		}
	}

	/* standard AES WINDOW instance */
	return wind_aes_get( app, win, mode, clip, x, y, w, h);
}


/**
 * @brief Get window attributes.
 *
 * @param app application descriptor,
 * @param win target window (or \b NULL in some case),
 * @param mode attribute to read,
 * @param x,y,w,h meanning depend on value of \e mode. For detail, read manual
 *  of mt_WindSet().
 *   - \b WF_FTOOLBAR : unless we use AES4 toolbar, this message is useless with
 *      WinDom toolbar system (see mt_WindSet(WF_TOOLBAR)). WinDom redraw itsel the 
 *      toolbar area. However, for AES4 comptability, mt_WindGet() returns correct values
 *      for this mode,
 *   - \b WF_NTOOLBAR : same remark than for \b F_FTOOLBAR.
 * @return 0 or a negative code error.
 *
 * Use this function instead of mt_wind_get(). 
 *
 * @sa mt_WindSet()
 */
int mt_WindGet( APPvar *app, WINDOW *win, int mode, INT16 *x, INT16 *y, INT16* w, INT16 *h ) {
	return mt_WindXGet( app, win, mode, NULL, x, y, w, h);
}


/**
 * @brief Get window pointer attribute.
 *
 * @param app application descriptor,
 * @param win target window (or \b NULL in some case),
 * @param mode attribute to read,
 * @return mode dependent pointer value (can be \b NULL).
 **/
void* mt_WindGetPtr( APPvar *app, WINDOW *win, int mode) {
	INT16 par1, par2, par3, par4;
	if ( ! mt_WindGet( app, win, mode, &par1, &par2, &par3, &par4) )
		return (void*) (long) ( ((long)par1 << 16 ) | ((long)par2 & 0xFFFFL) ) ;
	return NULL;
}


/**
 * @brief Returns the WINDOW's real AES handle.
 *
 * To be used when sending a message to a WINDOW instance
 * containing AES window handle (like e.g. snd_rdw()).
 *
 * @param app application descriptor,
 * @param win target window (or \b NULL for application scope),
 * @return WINDOW's real AES handle. It traverses up to the top-level
 *         WINDOW instance so that it returns valid AES value.
 */
int w_get_hndl( APPvar* app, WINDOW *win ) {
	/* desktop window */
	if ( !win ) win = app->priv->desk;

	/* the window is embeded in the COMPONENT or FRAME */
	if ( win->status & WS_CUSTOM ) {
		WIND_HANDLERS *whandlers = mt_DataSearch( app, win, WD_WHLR );
		if (whandlers) {
			if (whandlers->get_root_handle)
				return whandlers->get_root_handle( app, win);
			/* we can always return the WINDOW.handle -> fallthrough... */
		}
	}
	
	/* AES WINDOW handle */
	return wind_aes_get_root_handle( app, win);
}
