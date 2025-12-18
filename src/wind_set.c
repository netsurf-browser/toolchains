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
 * $Source: /cvsroot/windom/windom/src/wind_set.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/06/14 21:00:00 $
 *   $Revision: 1.29 $
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include "globals.h"
#include "cookie.h"


/**
 * Set the WINDOW menu appropriately.
 *
 * @param app application descriptor,
 * @param win window descriptor
 * @param mode WindSet() mode,
 * @param v1 pointer to a data (parameters \a p1 and \a p2 of WindSet()),
 * @param handler pointer to a callback function (parameters \a p3 and \a p4 of WindSet())
 *
 * @return 0 on success otherwise negative error code
 *
 * FIXME: MOVE to other file? perhaps comp_menu.c is the right place later on?
 */
int wind_menu_set( APPvar *app, WINDOW *win, int mode, void *v1, func_evnt handler) {
	switch( mode){
	case WF_MENU:
		/* WINDOW menu */
		if( win -> status & WS_MENU)
			mt_ObjcFree( app, win -> menu.root);

		if( v1 == NULL) { /* remove the menu */
			win -> status &= ~WS_MENU;
			win -> menu.root = NULL;
			win -> menu.real = NULL;
			mt_EvntDelete( app, win, MN_SELECTED);
		} else {
			win -> menu.root = mt_ObjcDup( app, v1, win);
			win -> menu.real = (OBJECT *)v1;
			win -> menu.scroll = 0;
#ifdef BETAMENU
			win -> menu.ac_title = win -> menu.ac_item = -1;
#endif
			win -> status |= WS_MENU;
			mt_EvntAttach( app, win, MN_SELECTED, handler);
		}
		break;

	case WF_HILIGHT:
		win->menu.hilight = v1;
		return E_OK;
	}

	/* signal error when the mode is not supported */
	return ENOSYS;
}


/**
 * @brief set/change window attributes.
 *
 * @param app application descriptor,
 * @param win window descriptor (or \b NULL in some case),
 * @param mode attribute to change,
 * @param par1
 * @param par2
 * @param par3
 * @param par4 meanning of these 4 parameters depend on \e mode value as explained elswhere.
 *
 * @return O or a negative error code.   
 *
 * This is WinDom internal function not to be used by applications.
 * They should use mt_WindSet() instead.
 *
 * @sa mt_WindSet(), t_WindGet().
 */
int __CDECL wind_aes_set( APPvar *app, WINDOW *win, int mode, int par1, int par2, int par3, int par4) {
	int res=1;
	INT16 x,y,w,h;			/* short integer type arguments */
	INT16 top, pos, dum;	/* temporary variables */
	INT16 xi, yi, wi, hi;
	void *v1;				/* pointer type arguments */
	func_evnt v2;			/* pointer type arguments */
	
	x = par1; y = par2; w = par3; h = par4;

	v1 = (void*) (long) ( ((long)par1 << 16 ) | ((long)par2 & 0xFFFFL) ) ;
	v2 = (func_evnt) (long) ( ((long)par3 << 16 ) | ((long)par4 & 0xFFFFL) ) ;

	/* and now, analyse the "mode" */		

	/* New : if the window is open
	 * the size is adjusted */

	switch( mode){
	case WF_MENU:
		hi = ( win -> status & WS_MENU) ? win -> menu.root[1].ob_height : 0;

		res = wind_menu_set( app, win, mode, v1, v2);

		if ( v1 == NULL ) {
			if( win->status & WS_OPEN && hi) {
				mt_WindGet( app, win, WF_CURRXYWH, &x, &y, &w, &h);
				h -= hi;
				mt_WindSet( app, win, WF_CURRXYWH, x, y, w, h);
				mt_ApplWrite( app, mt_AppId(app), WM_REDRAW, win->handle, x, y, w, h);
			}
			if( CONF(app)->flag & FLG_MNSYSTEM && 
					app->aes4 & AES4_MENUBAR)
				mt_wind_set_str( win->handle, WF_MENU, NULL, app->aes_global);
		} else {
			if( CONF(app)->flag & FLG_MNSYSTEM && 
					app->aes4 & AES4_MENUBAR)
				mt_wind_set_str( win->handle, WF_MENU, (char*)win -> menu.root, app->aes_global);
			if( win->status & WS_OPEN) {
				mt_WindGet( app, win, WF_CURRXYWH, &x, &y, &w, &h);
				h += (win -> menu.root[1].ob_height-hi);
				mt_WindSet( app, win, WF_CURRXYWH, x, y, w, h);
				mt_ApplWrite( app, mt_AppId(app), WM_REDRAW, win->handle, x, y, w, h);
			}
		}
		return res;
	case WF_MENUTNORMAL: /* menu_tnormal( title /par1/, mode /par2/ ) functionality */
		{
			short buff[8] = { WM_REDRAW };

			/* FIXME? perhaps set the position of the menu OBJECT */

			/* get area of the menu TITLE object */
			mt_objc_offset(win->menu.root, par1, &buff[4], &buff[5], app->aes_global);
			buff[6] = win->menu.root[par1].ob_width;
			buff[7] = win->menu.root[par1].ob_height;

			if( par2)
				win->menu.root[par1].ob_state &= ~SELECTED;
			else
				win->menu.root[par1].ob_state |= SELECTED;

			/* this goes through that internal function as it needs to update
			 * an object outside of the WinDom provided WM_WORKXYWH rectangle */
			_do_update( app, win, buff);
		}
		break;
	case WF_NAME:
		win -> name = (char*)v1;
		if( !(win -> status & WS_ICONIFY))
			res = mt_wind_set_str( win->handle, WF_NAME, (char*)v1, app->aes_global);
		break;
	case WF_ICONTITLE:
		win -> icon.name = (char*)v1;
		if( win -> status & WS_ICONIFY)
			res = mt_wind_set_str( win->handle, WF_NAME, (char*)v1, app->aes_global);
		break;
	case WF_ICONDRAW:
		mt_EvntAttach( app, win, WM_ICONDRAW, (func_evnt)v1);
		break;
	case WF_VWORK:
		/* case "open virtual workstation" */
		if ( (par1) && (win->graf == &(app->graf)) ) {
			win->graf = (W_GRAFPORT*) malloc (sizeof(W_GRAFPORT));
			if (win->graf) {
				win->graf->handle = mt_graf_handle( NULL, NULL, NULL, NULL, app->aes_global);
				v_opnvwk( app->work_in, &win->graf->handle, app->work_out);
				win->graf->palette = NULL;
			}
		} else if ( ( ! par1) && (win->graf != &(app->graf)) ) {
			v_clsvwk( win->graf->handle);
			free(win->graf);
			win->graf = &(app->graf);
		}
		break;
	case WF_INFO:
		win->info = v1;
		res = mt_wind_set_str( win->handle, WF_INFO, (char*)v1, app->aes_global);
		break;
	case WF_COMPONENT:
		win_comp_attach( app, win, v1);
		break;
	case WF_TOOLBAR:
		/* FIXME: Generalize to be handled correctly also in WS_CUSTOM windows!!! */
		if( win -> status & WS_TOOLBAR) {
			wi = ( win -> tool.root -> ob_width > win -> tool.root -> ob_height)?1:0;
			hi = wi ? win -> tool.root -> ob_height : win -> tool.root -> ob_width;
			mt_RsrcUserFree( app, win -> tool.root);
			mt_ObjcFree( app, win -> tool.root);
			if( win -> tool.save) free( win->tool.save);
		} else
			hi = 0;
		mt_wind_get( win->handle, WF_CURRXYWH, &x, &y, &w, &h, app->aes_global);
		if( win -> tool.root) mt_ObjcFree( app, win -> tool.root);
		if( win -> tool.bind) free(  win -> tool.bind);
		win -> tool.bind = NULL;
		if( v1 == NULL) {
			win -> tool.root = NULL;
			win -> tool.real = NULL;
			mt_EvntDelete( app, win, WM_TOOLBAR);
			win -> status &= ~WS_TOOLBAR;
			if( wi) h -= hi;
			else	w -= hi;
		} else {
			win -> tool.root = mt_ObjcDup( app, v1, win);
			win -> tool.real = (OBJECT *)v1;			
			mt_EvntAttach( app, win, WM_TOOLBAR, v2);
			win -> tool.save = NULL;
			win -> status |= WS_TOOLBAR;
			if( win -> tool.root -> ob_width > win -> tool.root -> ob_height) {
				win -> tool.root -> ob_width = app->w;
				h += win -> tool.root -> ob_height;
			} else {
				win -> tool.root -> ob_height = app->h;
				w += win -> tool.root -> ob_width;
			}
		}
		if( win->status & WS_OPEN) {
			mt_wind_set( win->handle, WF_CURRXYWH, x, y, w, h, app->aes_global);
			mt_ApplWrite( app, mt_AppId(app), WM_REDRAW, win->handle, x, y, w, h);
		}
		break;
	case WF_BEVENT:
		if( x & BEVENT_WORK)
			win -> status |= WS_UNTOPPABLE;
		if( (app->aes4 & AES4_BEVENT) && (x & BEVENT_WORK)) {
			x &= ~BEVENT_MODAL;
			res = mt_wind_set( win -> handle, WF_BEVENT, x, y, w, h, app->aes_global);
		}
		if( x & BEVENT_MODAL)
			win -> status |= WS_MODAL;
		else
			win -> status &= ~WS_MODAL;
		break;
	case WF_ICONIFY:
		if( win -> status & WS_ICONIFY)
			break;
		win -> status |= WS_ICONIFY;
		mt_wind_get( win->handle, WF_CURRXYWH, &win->icon.x, &win->icon.y, 
				  &win->icon.w, &win->icon.h, app->aes_global);
		if( app->aes4 & AES4_ICONIFY) {
			if( win -> status & WS_GROW)
				mt_graf_shrinkbox( x,y,w,h,win->icon.x,win->icon.y,win->icon.w,win->icon.h, app->aes_global);
			res = mt_wind_set( win->handle, WF_ICONIFY, x, y, w, h, app->aes_global);
			if( win->icon.name)
				mt_wind_set_str( win->handle, WF_NAME, win->icon.name, app->aes_global);
			break;
		}

		/* Is this the top window ? */
		mt_wind_get( 0, WF_TOP, &top, &dum, &dum, &dum, app->aes_global);
		dum=(top==win->handle)?0:1;

		if( win -> status & WS_GROW) {
			res = 1;
			win -> status &= ~ WS_GROW;
		}
		else res = 0;
		mt_WindClose( app, win);
		
		mt_wind_delete( win->handle, app->aes_global);
		win->handle = mt_wind_create( NAME|MOVER, app->x, app->y, app->w, app->h, app->aes_global);
		
		/* position of icons */
		for( pos = 0; app->priv->__iconlist[ pos]; pos ++);
		xi = app->x + (pos%(app->w/CONF(app)->wicon))*CONF(app)->wicon;
		yi = app->y + app->h -(pos/(app->w/CONF(app)->wicon)+1)*CONF(app)->hicon;
		wi = CONF(app)->wicon;
		hi = CONF(app)->hicon;
		if( xi == x && yi == y && wi == w && hi == h)
			app->priv->__iconlist[ pos] = win->handle;
		
		if( res == 1)
			mt_graf_shrinkbox( x,y,w,h,win->icon.x,win->icon.y,win->icon.w,win->icon.h, app->aes_global);

		mt_WindOpen( app, win, x, y, w, h);
		if( res == 1)
			win -> status |= WS_GROW;
		
		if( dum)
			mt_wind_set( top, WF_TOP, 0, 0, 0, 0, app->aes_global);

		if( win->icon.name)
			mt_wind_set_str( win->handle, WF_NAME, win->icon.name, app->aes_global);
		res = 1; /* emulation is always ok */
		break;

	case WF_UNICONIFY:
		if( (win -> status & WS_ICONIFY) == 0)
			break;
		win -> status &= ~WS_ICONIFY;
		mt_wind_get( win->handle, WF_CURRXYWH, &xi, &yi, &wi, &hi, app->aes_global);
		if( app->aes4 & AES4_ICONIFY)
		{
			if( win -> status & WS_GROW)
				mt_graf_growbox( xi,yi,wi,hi,x,y,w,h, app->aes_global);
			res = mt_wind_set( win->handle, WF_UNICONIFY, x, y, w, h, app->aes_global);
			if( win->name)
				mt_wind_set_str( win->handle, WF_NAME, win->name, app->aes_global);
			break;
		}

		/* free the icon positions */
		for( pos = 0; app->priv->__iconlist[ pos] != win->handle; pos ++); 
		app->priv->__iconlist[ pos] = 0;

		if( win -> status & WS_GROW) {
			res = 1;
			win -> status &= ~ WS_GROW;
		}
		else res = 0;

		mt_WindClose( app, win);
		mt_wind_delete( win->handle, app->aes_global);
		win->handle = mt_wind_create( win->attrib, win->createsize.g_x, 
								   win->createsize.g_y, win->createsize.g_w, 
								   win->createsize.g_h, app->aes_global);

		if( res == 1)
			mt_graf_growbox( xi, yi, wi, hi,x,y,w,h, app->aes_global);
		
		mt_WindOpen( app, win, x, y, w, h);
		if( res == 1)
			win -> status |= WS_GROW;
		
		if( win -> name)
			mt_wind_set_str( win->handle, WF_NAME, win->name, app->aes_global);
		if( win -> info)
			mt_wind_set_str( win->handle, WF_INFO, win->info, app->aes_global);
		res = 1; /* emulation is always ok */
		break;
	case WF_UNICONIFYXYWH:
		if( app->aes4 & AES4_ICONIFY)
			res = mt_wind_set( win->handle, WF_UNICONIFYXYWH, x, y, w, h, app->aes_global);
		else {
			win->icon.x = x;
			win->icon.y = y;
			win->icon.w = w;
			win->icon.h = h;
		}
		break;
	case WF_WORKXYWH:
		if (app->aes4 & AES4_SETWORKXYWH) {
			if( win->status & WS_TOOLBAR) {
				if( win->tool.root->ob_width == app->w) {
					y -= win->tool.root->ob_height;
					h += win->tool.root->ob_height;
				} else {
					x -= win->tool.root->ob_width;
					w += win->tool.root->ob_width;
				}
			}
			if( win->status & WS_MENU && !( SYSMENU(app) && win->attrib & MENUBAR) ) {
				y -= (win->menu.root[1].ob_height+1); /* "+1" for the 1 pixel width */
				h += (win->menu.root[1].ob_height+1); /* line under the menu */
			}
			res = mt_wind_set( win->handle, WF_WORKXYWH, x, y, w, h, app->aes_global);
		} else {
			INT16 xc,yc,wc,hc;
			
			mt_WindCalc( app, WC_BORDER, win, x, y, w, h, &xc, &yc, &wc, &hc);
			res = mt_wind_set( win->handle, WF_CURRXYWH, xc, yc, wc, hc, app->aes_global);
		}
		break;
	case WF_BOTTOM:
		/* if this is a modal window we do nothing */
		if( is_modal(app) && win == app->wglb.appfront)
			break;
		
		/* if this is already the bottom window, we do nothing */
		if (win == wlst_get_bottom(app))
			break;
		
		/* Update internal window list */
		wlst_remove(app,win);
		wlst_add_bottom(app,win);
		
		if( app->aes4 & AES4_BOTTOM)
			res = mt_wind_set( x, WF_BOTTOM, x, 0, 0, 0, app->aes_global);
		else {
			/* Here we emulates WF_BOTTOM feature */
			WINDOW *window = wlst_get_next(app,win);
			while( window) {
				mt_wind_set( window->handle, WF_TOP, 0, 0, 0, 0, app->aes_global);
				window = wlst_get_next(app,window);
			}
		} 

		break;
	case WF_TOP:
		/* if there is a modal window, the topped window will be the modal window */
		if( is_modal(app)) win = app->wglb.appfront;
		
		/* Update internal windows list */
		wlst_remove(app,win);
		wlst_add_top(app,win);
		
		/* TOP the AES window */
		res = mt_wind_set( win->handle, WF_TOP, 0, 0, 0, 0, app->aes_global);
		
		/* update wglb.* */
		app->wglb.appfront = app->wglb.front = win;
		
		break;
		
	case WF_HILIGHT:
		return wind_menu_set( app, win, mode, v1, v2);

	/* other wind_set() modes */
	default:
		res = mt_wind_set( win->handle, mode, x,y,w,h, app->aes_global);
		break;
	}

	/* WinDom returns 0 on success (unlike AES which does the opposite) */
	return (res != 0) ? E_OK : ERROR;
}


/**
 * @brief set/change window attributes.
 *
 * @param app application descriptor,
 * @param win window descriptor (or \b NULL in some case),
 * @param mode attribute to change,
 * @param par1
 * @param par2
 * @param par3
 * @param par4 meanning of these 4 parameters depend on \e mode value as explained below. For
 *        each mode value listed, we describe meanning of these parameters :
 *    - \b WF_NAME, \b WF_INFO, \b WF_ICONTITLE : see mt_WindSetStr() function,
 *    - \b WF_TOOLBAR, \b WF_MENU, \b WF_HILIGHT, \b WF_COMPONENT : see mt_WindSetPtr() function.
 *    - \b WF_WORKXYWH : set the window workspace. Additionnal toolbar and menu are taken
 *                       into account,
 *    - \b WF_TOP : Put the window \e win in foreground (par1, ..., par4 are not used).
 *                  Internal WinDom list is updated (for \b WM_BOTTOM emulation),
 *    - \b WF_BOTTOM : Put the window \e win in background (par1, ..., par4 are not used). 
 *                     This mode is supported by all TOS.
 *    - \b WF_BEVENT : Set the window event behaviour. Parameter \e par1 is a bitfield
 *                     with the following signification :
 *                       - \b BEVENT_WORK : a mouse button event on a window workspace creates a 
 *                                          \b MU_BUTTON event instead of a \b WF_TOP event.
 *                       - \b BEVENT_MODAL : a window modal is the only window active. Mouse button
 *                                addressed to other application windows are redirected to the modal window. This
 *                                mode is a specific WinDom feature.
 *    - \b WF_ICONIFY : Iconify window \e win at position specified by \e par1, \e par2, \e par3, \e par4.
 *                      Position is provided by the \b WM_ICONIFY event. This mode is supported by all TOS.
 *    - \b WF_UNICONIFY : Uniconify window \e win at position specified by \e par1, \e par2, \e par3, \e par4.
 *                      Position is provided by the \b WM_UNICONIFY event. This mode is supported by all TOS.
 *    - \b WF_UNICONIFXYWH : Specify a position (\e par1, \e par2, \e par3, \e par4) for the next window that 
 *                           will be iconified. This mode is supported by all TOS.
 *    - \b WF_HSLIDE, \b WF_VSLIDE, \b WF_HSLSIZE, \b WF_VSLSIZE : these mode are deprecated in WinDom.
 *                       mt_WindSlider() should be used instead of mt_WindSet().
 *    - \b WF_VWORK : open (or close) a virtual VDI workstation for this window. \e par1 = 1 is used
 *                    to open a virtual workstation (if a virtual workstation was not already opened
 *                    for this window), and \e par1 = 0 is used to close the virtual workstation. Other
 *                    parameters and values are reserved.
 *    - other values of \e mode make direct use of mt_wind_set() and have not specific signification in WinDom.
 * @return O or a negative error code.   
 *
 * This function is to be always used instead of mt_wind_set() as it is a way WinDom tracks window settings.
 * Some mode needs non integer parameters (e.g. \b WF_NAME, \b WF_MENU, ...). In this case, you can
 * use mt_WindSetStr() or mt_WindSetPtr() as specified in the previous list. Many modes don't
 * need to be set in a WinDom program because standard callback functions do it for you.
 *
 * @sa mt_WindSetStr(), mt_WindSetPtr(), mt_WindGet(), mt_WindSlider().
 */

int mt_WindSet( APPvar *app, WINDOW *win, int mode, int par1, int par2, int par3, int par4) {
	/* win == NULL means the desktop is targetted - handle = 0 */
	if ( !win ) win = app->priv->desk;

	/* the window is embeded in the COMPONENT or FRAME */
	if ( win->status & WS_CUSTOM ) {
		WIND_HANDLERS *whandlers = mt_DataSearch( app, win, WD_WHLR );
		if (whandlers) {
			if (whandlers->set)
				return whandlers->set( app, win, mode, par1, par2, par3, par4);
			return 0;
		}
	}

	/* standard AES WINDOW handling */
	return wind_aes_set( app, win, mode, par1, par2, par3, par4);
}


/**
 * @brief set/change window string attributes
 *
 * @param app application descriptor,
 * @param win window descriptor,
 * @param mode attribute to change,
 * @param str a pointer to a permanent string. Meanning of this parameter
 *        depend on \e mode value as explained below :
 *    - \b WF_NAME, set the window title. 
 *    - \b WF_INFO, set the window informative string.
 *    - \b WF_ICONTITLE, set the iconfiued window title.
 *    - other mode : see mt_WindSet()
 * @return O or a negative error code.   
 *
 * This function is used when window attribute is a string.
 *
 * @sa mt_WindSet()
 */

int mt_WindSetStr( APPvar *app, WINDOW *win, int mode, char *str) {
  return mt_WindSet( app, win, mode, ADR(str), 0, 0);
}

/**
 * @brief set/change window pointer attributes
 *
 * @param app application descriptor,
 * @param win window descriptor,
 * @param mode attribute to change,
 * @param par1, par2 meanning depend on \e mode value as described below :
 *     - \b WF_TOOLBAR, add a toolbar in a window. This mode is supported by all
 *        TOS. Toolbar will be displayed horizontally if its width is longer than
 *        its height and vertically otherwise. \e par1 design the pointer to the toolbar object tree
 *        and \e par2 design the callback function handling user selection in toolbar
 *        (can be \b NULL).
 *     - \b WF_MENU, add a menu in a window. This mode is supported by all
 *        TOS. \e par1 designs the pointer to the menu object tree
 *        and \e par2 designs the callback function handling user selection in menu
 *        (can be \b NULL). 
 *     - \b WF_HILIGHT, if the window (or the application desktop if parameter \b win
 *       is \b NULL), a callback function can be defined and call when a menu item
 *       is hilighted or unhilighted. \e par1 designs the callback function address.
 *       menu items. An hilight callback function have the following prototype :@code
void (*hilight)( APPvar *app, WINDOW *win, int title, int item);
// title is the menu title object index visited
// item is the menu item object index visited.
@endcode
 *     - \b WF_COMPONENT, attach a COMPONENT to a work area of the window.
 *        \e par1 pointer to the COMPONENT instance
 *        and \e par2 reserved (must be \b NULL). 
 * @return O or a negative error code.   
 *
 * This function is used when window attributes are pointers.
 *
 * @sa mt_WindSet()
 */

int mt_WindSetPtr( APPvar *app, WINDOW *win, int mode, void *par1, void *par2) {
  return mt_WindSet( app, win, mode, ADR(par1), ADR(par2));
}
