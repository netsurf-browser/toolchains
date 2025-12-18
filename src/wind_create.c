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
 * $Source: /cvsroot/windom/windom/src/wind_create.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.11 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/**
 * @brief Create a window.
 *
 * @param app application descriptor,
 * @param GemAttrib GEM widget of the window. It is a bitfield of the following values :
 *   - \b CLOSER : button to close the window,
 *   - \b MOVER : bar to move the window,
 *   - \b NAME : the window title (on the mover bar),
 *   - \b INFO : the window informative bar (under the mover bar),
 *   - \b SIZER : button to resize the window, 
 *   - \b VSLIDE : vertical slider,
 *   - \b UPARROW : up arrow in vertical slider,
 *   - \b DNARROW : down arrow in vertical slider,
 *   - \b HSLIDE : horizontal slider,
 *   - \b RTARROW : right arrow in horizontal slider,
 *   - \b LFARROW : left arrow in horizontal slider,
 *   - \b FULLER : button to resize in fullscreen the window,
 *   - \b SMALLER  : button to iconify the window. Available with AES 4.0 or newer.
 *                   If AES does not support iconification, WinDom emulates it : 
 *                   if user clicks on the CLOSER widget when the shift key is
 *                   depressed, a \b WM_ICONIFY event is sent,
 *   - \b MENUBAR (xaAES only) : As WinDom can handle its own menubar, this widget
 *      could be safely ignored. However, WinDom can leave to system to handle menu.
 *   - \b WAT_ALL (WinDom constant) : all widgets (except \b MENUBAR),
 *   - \b WAT_NOINFO (WinDom constant) : all widgets (except \b MENUBAR and \b INFO),
 *   - \b WAT_FORM (WinDom constant) : widget for a formular (no \b SIZER and sliders),
 *   - \b WAT_MFORM (WinDom constant) : widget for a modal formular (as \b WAT_FORM without \b CLOSER).
 * @param x,y,w,h maximal size of the window.
 * @return a pointer to a window descriptor or \b NULL if an error occurs.
 *
 * In WinDom, you have to create a window with mt_WindCreate() and not 
 * mt_wind_create() (unless you know what you do ...). mt_WindCreate() does not display
 * window at screen. For that, use mt_WindOpen(). mt_WindCreate() performs
 * the following actions :
 *  - a WINDOW structure is created and inserted in the internal WinDom list 
 *    (see mt_AddWindows()),
 *  - the GEM window is created,
 *  - standard callback functions are bound to the window (using mt_EvntAttach()).
 *
 * This last point is very important: when a window is created by
 * mt_WindCreate(), this window is ready to live in the WinDom
 * environnement because standard functions handle most of GEM events. Of course, these 
 * standard functions are very common and basic and the developper should need to change
 * or remove some of them (for that purpose, see the \link Event Event Library\endlink).
 *
 * A useless window should be deleted by calling mt_WindDelete(). However, you probably
 * never use directly this function because standard event function make for you this
 * work.
 *
 * \b Event \b mesages \b handled \b by  \b default
 *
 *  - \b WM_REDRAW (window area should be refreshed) : handled by WindClear(),
 *  - \b WM_CLOSED (window should be closed) : handled by std_cls(),
 *  - \b WM_DESTROY (window and data related should be released) : handled by std_dstry(),
 *  - \b WM_TOPPED (window should be put in foreground) : handled by std_tpd(),
 *  - \b WM_BOTTOMED (window should be put in background) : handled by std_btm(),
 *  - \b WM_MOVED (window has moved) : handled by std_mvd(),	
 *  - \b WM_SIZED (window has been resized) : handled by std_szd(),
 *  - \b WM_REPOSED (window has been moved and resized) : handled by std_szd(),
 *  - \b WM_FULLED (window resized in fullscreen) : handled by std_fld(),
 *  - \b WM_ICONIFY (window should be iconified ) : handled by std_icn(),
 *  - \b WM_UNICONIFY (iconified window should be restored) : handled by std_unicn(),
 *  - \b WM_ALLICONIFY (all opened windows should be iconified) : std_allicn(),
 *  - \b WM_HSLID (horizontal slider has be moved) : handled by std_hsld(),
 *  - \b WM_VSLID (vertical slider has be moved) : handled by std_vsld(),
 *  - \b WM_ARROWED (slider arrows has be selected) : handled by  std_arw(),
 *  - \b WA_DNPAGE (one page down) : handled by std_dnpgd(),
 *  - \b WA_UPPAGE (one page up)   : handled by std_uppgd(),
 *  - \b WA_LFPAGE (one page left) : handled by std_lfpgd(),
 *  - \b WA_RTPAGE (one page down) : handled by std_rtpgd(),
 *  - \b WA_DNLINE (one line down) : handled by std_dnlnd(),
 *  - \b WA_UPLINE (one line up)   : handled by std_uplnd(),
 *  - \b WA_LFLINE (one line left) : handled by std_lflnd(),
 *  - \b WA_RTLINE (one line down) : handled by std_rtlnd(),
 *
 *
 * The \b WM_BOTTOMED is supported by WinDom for any TOS version.
 * With TOS unsorpting this feature, a window can be 'bottomed', i.e.
 * send to the background, by shift-clicking the mover widget.
 * 
 * @sa mt_WindOpen(), mt_WindClose(), mt_WindDelete()
 * @todo The window chained list could be used instead of WINDOWLIST.
 */
WINDOW* mt_WindCreate( APPvar *app, int GemAttrib, int x, int y, int w, int h) {
	WINDOW* win;
	INT16 handle;

	if( !(CONF(app)->flag & FLG_MNSYSTEM)) GemAttrib &= ~MENUBAR;
	handle = mt_wind_create( GemAttrib, x, y, w, h, app->aes_global);
	if( handle < 0) {
#ifdef WINDOMDEBUG
		mt_form_alert( 1, "[1][WINDOM warning:|Impossible to create a window.][Ok]", app->aes_global);
#endif
		return NULL;
	}
	win = (WINDOW *) malloc( sizeof( WINDOW));
	if( win == NULL) {
		mt_wind_delete( handle, app->aes_global);
		return NULL;
	}
	rc_set( &win->createsize, x, y, w, h);
	win->attrib = GemAttrib;
	win->w_min 	= win->h_min = 40;
	win->status = WS_GROW;
	win->w_max 	= w;
	win->h_max 	= h;
	win->graf   = &(app->graf);

	win->next    = NULL;
	win->handle  = handle;

	listInit( &win->binding );

	mt_EvntAttach( app, win, WM_REDRAW, mt_stdWindClear);
	mt_EvntAttach( app, win, WM_CLOSED, std_cls);
	mt_EvntAttach( app, win, WM_TOPPED, std_tpd);
	mt_EvntAttach( app, win, WM_BOTTOMED, std_btm);
	mt_EvntAttach( app, win, WM_MOVED , std_mvd);
	mt_EvntAttach( app, win, WM_SIZED , std_szd);
	mt_EvntAttach( app, win, WM_REPOSED, std_szd);
	mt_EvntAttach( app, win, WM_FULLED, std_fld);
	mt_EvntAttach( app, win, WM_ICONIFY, std_icn);
	mt_EvntAttach( app, win, WM_UNICONIFY, std_unicn);
	mt_EvntAttach( app, win, WM_ALLICONIFY, std_allicn);
	mt_EvntAttach( app, win, WM_ARROWED, std_arw);
	mt_EvntAttach( app, win, WM_UPPAGE, std_uppgd);
	mt_EvntAttach( app, win, WM_DNPAGE, std_dnpgd);
	mt_EvntAttach( app, win, WM_LFPAGE, std_lfpgd);
	mt_EvntAttach( app, win, WM_RTPAGE, std_rtpgd);	
	mt_EvntAttach( app, win, WM_UPLINE, std_uplnd);
	mt_EvntAttach( app, win, WM_DNLINE, std_dnlnd);
	mt_EvntAttach( app, win, WM_LFLINE, std_lflnd);
	mt_EvntAttach( app, win, WM_RTLINE, std_rtlnd);
	mt_EvntAttach( app, win, WM_VSLID , std_vsld);
	mt_EvntAttach( app, win, WM_HSLID , std_hsld);
	mt_EvntAttach( app, win, WM_SLIDEXY, std_sldxy);
	mt_EvntAttach( app, win, WM_DESTROY, std_dstry);
	

	win -> xpos 	= win -> ypos = 0;
	win -> xpos_max = win -> ypos_max = 1;
	win -> w_u = win -> h_u = 1;
	win -> menu . root = NULL;
	win -> menu . bind = NULL;
	win -> menu . hilight = NULL;
	win -> tool . root = NULL;
	win -> tool . save = NULL;
	win -> tool . bind = NULL;
	win -> icon . name = NULL;
	listInit( &win->data );
	win->type = 0;
	win->name = win->info = NULL;

	mt_AddWindow( app, win);
	return win;	
}
