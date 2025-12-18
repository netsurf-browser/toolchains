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
 * $Source: /cvsroot/windom/windom/src/wind_calc.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.9 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

int __CDECL wind_aes_calc( APPvar *app, WINDOW *wind, int type, int xin, int yin,
			  int win, int hin, INT16 *xout, INT16 *yout, 
			  INT16 *wout, INT16 *hout) {
	int res=0;

	if( wind->status & WS_ICONIFY) {
		res = mt_wind_calc( type, MOVER|NAME, xin, yin, win, hin, xout, yout, wout, hout, app->aes_global);
		return res;
	}
	else
		res = mt_wind_calc( type, wind->attrib, xin, yin, win, hin, xout, yout, wout, hout, app->aes_global);

	if( type == WC_BORDER) { /* WORK --> CURR */
		if( wind->status & WS_TOOLBAR) {
			if( wind -> tool.root -> ob_width == app->w) {
				*yout -= wind->tool.root->ob_height;
				*hout += wind->tool.root->ob_height;
			} else {
				*xout -= wind -> tool.root -> ob_width;
				*wout += wind -> tool.root -> ob_width;
			}
		}
		if( wind->status & WS_MENU &&
			!( SYSMENU(app) && wind->attrib & MENUBAR) ) {
			*yout -= (wind->menu.root[1].ob_height+1); /* "+1" for the 1 pixel width */
			*hout += (wind->menu.root[1].ob_height+1); /* line under the menu */
		}
	} else {
		if( wind->status & WS_MENU &&
			!( SYSMENU(app) && wind->attrib & MENUBAR) ) {
			*yout += (wind->menu.root[1].ob_height+1);
			*hout -= (wind->menu.root[1].ob_height+1);
		}
		if( wind -> status & WS_TOOLBAR ) {
			if( wind -> tool.root -> ob_width == app->w) {
				*yout += wind -> tool.root -> ob_height;
				*hout -= wind -> tool.root -> ob_height;
			} else {
				*xout += wind -> tool.root -> ob_width;
				*wout -= wind -> tool.root -> ob_width;
			}	
		}	
	}
	return res;
}


/**
 * @brief convert window workarea in real area and inversly
 *
 * @param app application descriptor,
 * @param type currently, two values are possible :
 *   - \b WC_BORDER : convert work area coodinate in window
 *        real coordinates,
 *   - \b WC_WORK : inverse operation of \b WC_BORDER.
 * @param wind window descriptor,
 * @param xin,yin,win,hin coordinates to convert,
 * @param xout,yout,wout,hout converted coordinates,
 * @return a null value if error.
 *
 * mt_WindCalc() take into account optional window menu and toolbar. If you
 * want compute the real AES workarea of a window, use mt_wind_calc().
 *
 * \sa mt_wind_calc()
 */

int mt_WindCalc( APPvar *app, int type, WINDOW *wind, int xin, int yin,
			  int win, int hin, INT16 *xout, INT16 *yout, 
			  INT16 *wout, INT16 *hout) {
	
	if( !wind || wind->status & WS_CUSTOM) {
		WIND_HANDLERS *whandlers = (WIND_HANDLERS*)mt_DataSearch( app, wind, WD_WHLR );
		if (whandlers) {
		       	if (whandlers->calc)
				return whandlers->calc( app, wind, type, xin, yin, win, hin, xout, yout, wout, hout );
			return 0;
		}
	}

	/* standard AES WINDOW handling */
	return wind_aes_calc( app, wind, type, xin, yin, win, hin, xout, yout, wout, hout );
}
