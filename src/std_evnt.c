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
 * $Source: /cvsroot/windom/windom/src/std_evnt.c,v $
 * Module : standard event window callback
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/02/26 03:15:22 $
 *   $Revision: 1.15 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/** standard callback function to manage application-terminated event
 *
 *  @param win window descriptor (NULL)
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to application for
 *  the \b AP_TERM event, registered by  mt_ApplInit()
 *
 *  this function will try to terminate cleanly the application by:
 *  - sending a WM_DESTROY message to all windows
 *  - unsubscribe from an AV server if any
 *  - terminate frame support
 *  - and calling mt_ApplExit().
 *
 *  It's recommanded to write to own apterm callback function, in order
 *  to free all the resources your application may have reserved (RSC file,
 *  dynamical libraries, etc...)
 *  
 */

void __CDECL std_apterm( WINDOW *win, short buff[8], APPvar *app ) {
	/* close all window */
	while( app->wglb.first) {
		mt_snd_msg( app, app->wglb.first, WM_DESTROY, 0, 0, 0, 0);
		mt_EvntWindom( app, MU_MESAG);
	}

	/* unsubscribe from an AV server if any */
	if ( app->avid != -1 ) {
		mt_AvExit( app );
	}

	/* frame support terminate */
	mt_FrameExit( app );

	/* terminate */
	mt_ApplExit( app );
	exit( 0);
}

/** standard callback function to manage window-closed event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to windows for
 *  the \b WM_CLOSED event, registered by  mt_WindCreate()
 *
 *  The function just sends a \b WM_DESTROY message.
 */

void __CDECL std_cls( WINDOW *win, short buff[8], APPvar *app) {	
	mt_snd_msg( app, win, WM_DESTROY,0,0,0,0);
}

/** standard callback function to manage window-moved event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to windows for
 *  the \b WM_MOVED event, registered by  mt_WindCreate()
 *
 *  this function will set the new window coordinates (given in \a buff)
 *  and unset the WS_FULLSIZE flag.
 */
 
void __CDECL std_mvd( WINDOW *win, short buff[8], APPvar *app) {	
	mt_wind_set( buff[3], WF_CURRXYWH, buff[4], buff[5], buff[6], buff[7], app->aes_global);
	win->status &= ~WS_FULLSIZE;
}

/** standard callback function to manage window-topped event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to windows for
 *  the \b WM_TOPPED event, registered by  mt_WindCreate()
 */
 
void __CDECL std_tpd( WINDOW *win, short buff[8], APPvar *app) {
	mt_WindSet( app, win, WF_TOP, win->handle, 0, 0, 0);
}

/** standard callback function to manage window-sized event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to windows for
 *  the \b WM_SIZED and \b WM_REPOSED event, registered by  mt_WindCreate()
 *
 *  This function will set the new size of the window (given in \a buff)
 *  after saturation by w_min, w_max, h_min and h_max, recompute the
 *  sliders, send to itself a redraw message if the content of the window
 *  has to be redrawed, and unset the WS_FULLSIZE flag
 */
 
void __CDECL std_szd( WINDOW *win, short buff[8], APPvar *app) {
	INT16 x, y, w, h;
	int rdw = 0;
	unsigned long oldx, oldy;
	
	w = MAX( buff[6], win -> w_min);
	h = MAX( buff[7], win -> h_min);
	w = MIN( w		, win -> w_max);
	h = MIN( h		, win -> h_max);

	mt_WindSet( app, win, WF_CURRXYWH, buff[4], buff[5], w, h);
	
	/* TODO: better optimisation of the redraw forcing */
	
	/* case of "proportional content" : we force the redraw */
	if( win->xpos == 0 && win->xpos_max == 1 &&
		win->ypos == 0 && win->ypos_max == 1)
		rdw = 1;
		
	/* ajust xpos and ypos if the window has been elarged */
	mt_WindGet( app, win, WF_SLIDXYWH, &x, &y, &w, &h);
	oldx = win -> xpos;
	oldy = win -> ypos;
	if( (unsigned int)w > win -> xpos_max * win -> w_u)
		win -> xpos = 0;
	else
		win -> xpos = MIN( win -> xpos, win -> xpos_max - w / win -> w_u);

	if( (unsigned int)h > win -> ypos_max * win -> h_u)
		win -> ypos = 0;
	else
		win -> ypos = MIN( win -> ypos, win -> ypos_max - h / win -> h_u);

	if( oldx != win -> xpos || oldy != win -> ypos)
		rdw = 1;
	mt_WindSlider( app, win, HSLIDER|VSLIDER);
	win->status &= ~WS_FULLSIZE;
	if( rdw) mt_snd_rdw( app, win);
}

/** standard callback function to manage window-fulled event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to windows for
 *  the \b WM_FULLED event, registered by  mt_WindCreate()
 *
 *  This function will 
 *  - restore the previous window position and size
 *    if the WS_FULLSIZE flag is set, or will set the window position
 *    and size to its maximum size otherwise.
 *  - changed the flag WS_FULLSIZE
 *  - send to itself a redraw message if needed
 *  - recompute the sliders
 */
 
void __CDECL std_fld( WINDOW *win, short buff[8], APPvar *app) {
	INT16 x, y, w, h;

	if( win->status & WS_FULLSIZE) {
		mt_wind_get( buff[3], WF_PREVXYWH, &x, &y, &w, &h, app->aes_global);
		mt_wind_set( buff[3], WF_CURRXYWH, x, y,  w, h, app->aes_global);
	} else
		mt_wind_set( buff[3], WF_CURRXYWH, app->x, app->y, win->w_max, win->h_max, app->aes_global);
	
	win->status ^= WS_FULLSIZE;

	/* case of "proportional content" : we force the redraw */
	if( win->xpos == 0 && win->xpos_max == 1 &&
		win->ypos == 0 && win->ypos_max == 1)
		mt_snd_rdw( app, win);
		
	/* adjust xpos and ypos if the window has been enlarged */
	mt_WindGet( app, win, WF_SLIDXYWH, &x, &y, &w, &h);
	if( (unsigned int)w > win -> xpos_max * win -> w_u)
		win -> xpos = 0;
	else
		win -> xpos = MIN( win -> xpos, win -> xpos_max - w / win -> w_u);

	if( (unsigned int)h > win -> ypos_max * win -> h_u)
		win -> ypos = 0;
	else
		win -> ypos = MIN( win -> ypos, win -> ypos_max - h / win -> h_u);

	mt_WindSlider( app, win, HSLIDER|VSLIDER);
}

/** standard callback function to manage window-iconify event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to windows for
 *  the \b WM_ICONIFY event, registered by  mt_WindCreate()
 *
 *  This function will :
 *  - iconify the window (WF_ICONIFY),
 *  - put it at the bottom of the screen (WF_BOTTOM)
 *  - change the window name (WF_NAME) if a particular name has been specified
 *    for the window when iconified (\a win->icon.name)
 */

void __CDECL std_icn( WINDOW *win, short buff[8], APPvar *app) {
	mt_WindSet( app, win, WF_ICONIFY, buff[4], buff[5], buff[6], buff[7]);
	mt_WindSet( app, win, WF_BOTTOM, win->handle, 0, 0, 0);
	if( win->icon.name)
		mt_wind_set_str( win->handle, WF_NAME, win->icon.name, app->aes_global);
}

/** standard callback function to manage window-uniconify event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to windows for
 *  the \b WM_UNICONIFY event, registered by  mt_WindCreate()
 *
 *  If the flag WS_ALLICNF is set (this is the unified iconified window),
 *  then all the window that has been grouped in this iconified window are
 *  re-opened.
 *
 *  Then, the window is uniconified (WF_UNICONIFY) and set to the top (WF_TOP).
 */

void __CDECL std_unicn( WINDOW *win, short buff[8], APPvar *app) {
	if( win->status & WS_ALLICNF) {
		WINDOW *scan;
		
		mt_MenuEnable(app);
		for( scan=app->wglb.first; scan; scan=scan->next) {
			if( scan != win && scan->status & WS_ALLICNF) {
				mt_WindOpen( app, scan, scan->icon.x, scan->icon.y, scan->icon.w, scan->icon.h);
				scan->status &= ~WS_ALLICNF;
			/*	EvntWindom( MU_MESAG); */
			}
		}
		win->status &= ~WS_ALLICNF;
		mt_WindSet( app, win, WF_TOP, win->handle, 0, 0, 0);
	}
	mt_WindSet( app, win, WF_UNICONIFY, buff[4], buff[5], buff[6], buff[7]);
	mt_WindSet( app, win, WF_TOP, win->handle, 0, 0, 0);
	if( win->name)
		mt_wind_set_str( win->handle, WF_NAME, win->name, app->aes_global);
}

/** standard callback function to manage window-alliconify event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to windows for
 *  the \b WM_ALLICONIFY event, registered by  mt_WindCreate()
 *
 *  This function will "group" all the windows together and iconified
 *  the unified window (in fact, all the windows except \a win are closed
 *  and \a win is iconified. All of them will have their flag WD_ALLICNF set)
 */

void __CDECL std_allicn( WINDOW *win, short buff[8], APPvar *app)
{
	WINDOW *scan;
	
	mt_MenuDisable(app);

	/* Close all the windows but this one */
	for( scan = app->wglb.first; scan; scan=scan->next) {
		if( scan->status & WS_OPEN && scan != win) {
			scan->status |= WS_ALLICNF;
			mt_WindGet( app, scan, WF_CURRXYWH, &scan->icon.x, &scan->icon.y, &scan->icon.w, &scan->icon.h);
			mt_WindClose( app, scan);
		/*	EvntWindom( MU_MESAG); */
		}
	}
	win->status |= WS_ALLICNF;
	
	/* iconify this window */
	std_icn( win, buff, app);
}


/** standard callback function to manage "move-work" actions
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to windows for
 *  the \b WM_SLIDEXY event, registered by  mt_WindCreate(). This event
 *  is defined by windom (it's not a standard message sent by all AESes).
 *
 *  This function will update the sliders, and then move the work area
 *  of the window with the shift given in \a buff[4] (dx) and \a buff[5] (dy).
 */

void __CDECL std_sldxy( WINDOW *win, short buff[8], APPvar *app) {
	int dx = buff[4];
	int dy = buff[5];
	GRECT work, screen;

	if (!dx && !dy) return;

	/* update the sliders _before_ we call redraw (which might depend on the slider possitions) */
	mt_WindSlider( app, win, (dx?HSLIDER:0) | (dy?VSLIDER:0) );

	mt_WindGetGrect( app, win, WF_WORKXYWH, &work);
	mt_wind_get_grect( 0, WF_CURRXYWH, &screen, app->aes_global);
	if( !rc_intersect( &screen, &work)) return;

	move_work_rect( app, win, &work, dx, dy);
}

/** standard callback function to manage window page-down event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard function invoked by std_arw() when
 *  the \b WM_ARROWED message received is for \b WA_DNPAGE action.
 *
 *  This function is also attached to the \b WM_DNPAGE windom
 *  message by mt_WindCreate(). This feature is unused at the moment.
 */

void __CDECL std_dnpgd( WINDOW *win, short buff[8], APPvar *app) {
	int page;
	INT16 x,y,w,h;
	long old_pos = win -> ypos;
	int dy;

	mt_WindGet( app, win, WF_SLIDXYWH, &x, &y, &w, &h);
	page = h / win -> h_u;
	if ( win -> ypos < win -> ypos_max - page) {
		win -> ypos = MIN( win->ypos_max, win->ypos) + page;
		win -> ypos = MIN( win -> ypos, win -> ypos_max - page);
		dy = (int) ((win->ypos - old_pos) * win->h_u);
		move_work(app,win,0,dy);
	}
}

/** standard callback function to manage window page-up event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard function invoked by std_arw() when
 *  the \b WM_ARROWED message received is for \b WA_UPPAGE action.
 *
 *  This function is also attached to the \b WM_UPPAGE windom
 *  message by mt_WindCreate(). This feature is unused at the moment.
 */

void __CDECL std_uppgd( WINDOW *win, short buff[8], APPvar *app) {
	long pos;
	INT16 d, h;
	int dy;

	if ( win -> ypos > 0L) {
		mt_WindGet( app, win, WF_SLIDXYWH, &d, &d, &d, &h);
		pos = MAX( 0L, win->ypos - h / win->h_u);
		dy = (int) ((pos - win->ypos) * win->h_u);
		win->ypos = pos;
		move_work(app, win,0,dy);
	}
}

/** standard callback function to manage window line-up event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard function invoked by std_arw() when
 *  the \b WM_ARROWED message received is for \b WA_UPLINE action.
 *
 *  This function is also attached to the \b WM_UPLINE windom
 *  message by mt_WindCreate(). This feature is unused at the moment.
 */

void __CDECL std_uplnd( WINDOW *win, short buff[8], APPvar *app) {
	if ( win -> ypos > 0L) {
		win->ypos --;
		move_work(app,win,0, - win->h_u);
	}
}

/** standard callback function to manage window down-up event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard function invoked by std_arw() when
 *  the \b WM_ARROWED message received is for \b WA_DNLINE action.
 *
 *  This function is also attached to the \b WM_DNLINE windom
 *  message by mt_WindCreate(). This feature is unused at the moment.
 */

void __CDECL std_dnlnd( WINDOW *win, short buff[8], APPvar *app) {
	INT16 x, y, w, h;

	mt_WindGet( app, win, WF_SLIDXYWH, &x, &y, &w, &h);
	if (( win -> ypos < win -> ypos_max - h / win -> h_u) &&
	    ( win -> ypos_max > h / win -> h_u )) {
		win -> ypos ++;
		move_work(app,win,0,win->h_u);
	}
}

/** standard callback function to manage window vertical-slider event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to windows for
 *  the \b WM_VSLID event, registered by  mt_WindCreate().
 */

void __CDECL std_vsld( WINDOW *win, short buff[8], APPvar *app) {
	long pos;
	INT16 h, dum;
	int dy;

	mt_WindGet( app, win, WF_SLIDXYWH, &dum, &dum, &dum, &h);
	pos = (long)(win->ypos_max - h/win->h_u)*(long)buff[4]/1000L;
	if (pos < 0) pos = 0;
	dy = (int)((pos - win->ypos) * win->h_u);
	win->ypos =  (int) pos;
	if( dy) move_work( app, win, 0, dy);
}

/** standard callback function to manage window horizontal-slider event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to windows for
 *  the \b WM_HSLID event, registered by  mt_WindCreate().
 */

void __CDECL std_hsld( WINDOW *win, short buff[8], APPvar *app) {
	INT16 w, dum;
	long pos;
	int dx;

	mt_WindGet( app, win, WF_SLIDXYWH, &dum, &dum, &w, &dum);
	pos = (long)(win->xpos_max - w/win->w_u)*(long)buff[4]/1000L;	
	if (pos < 0) pos = 0;
	dx = (int)((pos - win->xpos) * win->w_u );
	win->xpos =  (int) pos;
	if (dx) move_work(app,win,dx,0);
}

/** standard callback function to manage window line-left event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard function invoked by std_arw() when
 *  the \b WM_ARROWED message received is for \b WA_LFLINE action.
 *
 *  This function is also attached to the \b WM_LFLINE windom
 *  message by mt_WindCreate(). This feature is unused at the moment.
 */

void __CDECL std_lflnd( WINDOW *win, short buff[8], APPvar *app) {
	if ( win -> xpos > 0) {
		win -> xpos --;
		move_work(app,win,-win->w_u,0);
	}
}

/** standard callback function to manage window line-right event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard function invoked by std_arw() when
 *  the \b WM_ARROWED message received is for \b WA_RTLINE action.
 *
 *  This function is also attached to the \b WM_RTLINE windom
 *  message by mt_WindCreate(). This feature is unused at the moment.
 */

void __CDECL std_rtlnd( WINDOW *win, short buff[8], APPvar *app) {
	INT16 dum, w;

	mt_WindGet( app, win, WF_SLIDXYWH, &dum, &dum, &w, &dum);
	if( ( win -> xpos < win -> xpos_max - w / win -> w_u) &&
	    ( win -> xpos_max > w / win -> w_u )) {
		win -> xpos ++;
		move_work(app,win,win->w_u,0);
	}
}

/** standard callback function to manage window page-left event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard function invoked by std_arw() when
 *  the \b WM_ARROWED message received is for \b WA_LFPAGE action.
 *
 *  This function is also attached to the \b WM_LFPAGE windom
 *  message by mt_WindCreate(). This feature is unused at the moment.
 */

void __CDECL std_lfpgd( WINDOW *win, short buff[8], APPvar *app) {
	long pos;
	INT16 dum, w;
	int dx;

	if ( win -> xpos > 0L) {
		mt_WindGet( app, win, WF_SLIDXYWH, &dum, &dum, &w, &dum);
		pos = MAX( 0L, win->xpos - w / win->w_u);
		dx = (int)((pos - win->xpos) * win->w_u);
		win->xpos = pos;
		move_work(app,win,dx,0);
	}
}

/** standard callback function to manage window page-right event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard function invoked by std_arw() when
 *  the \b WM_ARROWED message received is for \b WA_RTPAGE action.
 *
 *  This function is also attached to the \b WM_RTPAGE windom
 *  message by mt_WindCreate(). This feature is unused at the moment.
 */

void __CDECL std_rtpgd( WINDOW *win, short buff[8], APPvar *app) {
	int page;
	INT16 x,y,w,h;
	long old_pos = win -> xpos;
	int dx;
	
	mt_WindGet( app, win, WF_SLIDXYWH, &x, &y, &w, &h);
	page = w / win -> w_u;
	if ( win -> xpos < win -> xpos_max - page) {
		win -> xpos = MIN( win -> xpos_max, win -> xpos) + page;
		win -> xpos = MIN( win -> xpos, win -> xpos_max - page);
		dx = (int)((win->xpos - old_pos) * win->w_u);
		move_work(app,win,dx,0);
	}
}

/** standard callback function to manage window-arrowed event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to windows for
 *  the \b WM_ARROWED event, registered by  mt_WindCreate().
 *
 *  Depending on \a buff[4], the dedicated function is invoked in order
 *  to perform the requested action :
 *  - std_uppgd() for \b WA_UPPAGE action
 *  - std_dnpgd() for \b WA_DNPAGE action
 *  - std_rtpgd() for \b WA_RTPAGE action
 *  - std_lfpgd() for \b WA_LFPAGE action
 *  - std_uplnd() for \b WA_UPLINE action
 *  - std_dnlnd() for \b WA_DNLINE action
 *  - std_rtlnd() for \b WA_RTLINE action
 *  - std_lflnd() for \b WA_LFLINE action
 */

void __CDECL std_arw( WINDOW *win, short buff[8], APPvar *app) {
	switch( buff[4]) {
	case WA_UPPAGE:
		std_uppgd( win, buff, app);
		break;
	case WA_DNPAGE:
		std_dnpgd( win, buff, app);
		break;
	case WA_RTPAGE:
		std_rtpgd( win, buff, app);
		break;
	case WA_LFPAGE:
		std_lfpgd( win, buff, app);
		break;
	case WA_UPLINE:
		std_uplnd( win, buff, app);
		break;
	case WA_DNLINE:
		std_dnlnd( win, buff, app);
		break;
	case WA_RTLINE:
		std_rtlnd( win, buff, app);
		break;
	case WA_LFLINE:
		std_lflnd( win, buff, app);
		break;
	}
}

/** standard callback function to manage window-bottomed event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to windows for
 *  the \b WM_BOTTOMED event, registered by  mt_WindCreate().
 */

void __CDECL std_btm( WINDOW *win, short buff[8], APPvar *app) {
	mt_WindSet( app, win, WF_BOTTOM, 0, 0, 0, 0);
}


/** standard callback function to manage CM_REFLOW event
 *
 *  @param c COMPONENT descriptor
 *  @param buff component message
 *  @param app application descriptor
 *
 *  This is the standard callback function that ensures the component
 *  contents is adjusted in case any of the contained parts changed
 *  its size.
 */

void __CDECL std_comp_reflow( COMPONENT *c, long buff[8], APPvar *app ) {
	comp_reflow( c, app );
}

