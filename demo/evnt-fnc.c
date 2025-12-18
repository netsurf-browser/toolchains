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
 * $Source: /cvsroot/windom/windom/demo/evnt-fnc.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/03/22 20:58:30 $
 *   $Revision: 1.13 $
 */

#include <stddef.h>
#include <stdlib.h>
#include <windom.h>
#include <av.h>
#include "global.h"
#include "demo.h"

#ifndef FALSE
#define TRUE 1
#define FALSE 0
#endif

/*
 * AP_TERM event function : quit properly the application
 */

void __CDECL ap_term( WINDOW *w, short buff[8]) {
	WINDOW *last_closed=NULL;
	
    /* Save the Set up */
    if( param.setup & SETUP_AUTOSAVE)
    	setup_save();

	/* Close all windows */
	while( wglb.first) {
		if (last_closed != wglb.first) {
			ApplWrite( _AESapid, WM_CLOSED, wglb.first->handle, 0, 0, 0, 0);
			last_closed = wglb.first; /* to prevent sending toons of WM_CLOSED messages to each window */
		}
		if ( EvntWindom( MU_MESAG | MU_TIMER) & MU_TIMER)  /* MU_TIMER event catched ? */
			last_closed = NULL; /* then WM_CLOSED message has been lost ! it should be resent */
	}
	
	if( param.setup & SETUP_XTYPE) RsrcXtype( 0, rsc_trindex, rsc_ntree);
 	if( param.setup & SETUP_INSTALL_DESKTOP) WindSet( NULL, WF_NEWDESK, 0, 0, 0, 0);
 	ApplWrite( buff[1], (buff[5]==AP_TERM)?SHUT_COMPLETED:RESCHG_COMPLETED, 1,0,0,0,0);
 	UnLoadBubble();
 	FrameExit();
 	AvExit();
 	RsrcXtype( 0, rsc_trindex, rsc_ntree);
 	RsrcXfree(rsc);
	ApplExit();
	exit(0);
}

/*
 * AV_SENDCLICK event function :
 * this event is tranformed as a MU_BUTTON message
 */

void __CDECL av_sendclick( WINDOW *w, short buff[8]) {
	ApplWrite( _AESapid, AP_BUTTON, buff[3], buff[4], buff[5], buff[6], buff[7]);
}

/*
 * AV_SENDKEY event function :
 * this event is tranformed as a MU_KEYBD message
 * If you sends AV_SENDKEY messages to the application, it is possible
 * remote command the application (if functions have a key shortcut)
 */

void __CDECL av_sendkey( WINDOW *w, short buff[8]) {
	ApplWrite( _AESapid, AP_KEYBD, buff[3], buff[4], buff[5], buff[6], buff[7]);
}

/*
 * MU_BUTTON global event : Bubble GEM handling
 */

void __CDECL mu_button( WINDOW *w, short buff[8]) {
	if( buff[5] & 0x02)
		BubbleEvnt();
}

/*
 *	desktop menu event 
 */

/*
 * handle the menu by catching a specific menu item
 * (using ObjcAttachMenuFunc) ...
 */

static void __CDECL my_create_info_form( WINDOW *win, int item, int title, void *data) {
	create_info_form();
}
static void __CDECL my_create_simple( WINDOW *win, int item, int title, void *data) {
	create_simple();
}
static void __CDECL my_create_windowtoolbar( WINDOW *win, int item, int title, void *data) {
	create_windowtoolbar();
}
static void __CDECL my_create_custom( WINDOW *win, int item, int title, void *data) {
	create_custom();
}
static void __CDECL my_create_windowtoolmenu( WINDOW *win, int item, int title, void *data) {
	create_windowtoolmenu();
}
static void __CDECL my_create_modal_form( WINDOW *win, int item, int title, void *data) {
	create_modal_form();
}
static void __CDECL my_call_fontselector( WINDOW *win, int item, int title, void *data) {
	call_fontselector();
}
static void __CDECL my_call_fileselector( WINDOW *win, int item, int title, void *data) {
	call_fileselector();
}
static void __CDECL my_create_two_frame( WINDOW *win, int item, int title, void *data) {
	create_two_frame();
}
static void __CDECL my_create_three_frame( WINDOW *win, int item, int title, void *data) {
	create_three_frame();
}
static void __CDECL my_create_quit_form( WINDOW *win, int item, int title, void *data) {
	create_quit_form();
}
static void __CDECL my_av_status( WINDOW *win, int item, int title, void *data) {
	av_status();
}
static void __CDECL my_do_av_opendir( WINDOW *win, int item, int title, void *data) {
	do_av_opendir();
}
static void __CDECL my_do_av_sendfile( WINDOW *win, int item, int title, void *data) {
	do_av_sendfile();
}
static void __CDECL my_do_av_sendkey( WINDOW *win, int item, int title, void *data) {
	do_av_sendkey();
}
static void __CDECL my_do_av_view( WINDOW *win, int item, int title, void *data) {
	do_av_view();
}

void menu_binding( void) {
	ObjcAttachMenuFunc( NULL, MN1_INFO, my_create_info_form, NULL);
	ObjcAttachMenuFunc( NULL, MN1_FS, my_create_simple, NULL);
	ObjcAttachMenuFunc( NULL, MN1_FT, my_create_windowtoolbar, NULL);
	ObjcAttachMenuFunc( NULL, MN1_FM, my_create_custom, NULL);
	ObjcAttachMenuFunc( NULL, MN1_FTM, my_create_windowtoolmenu, NULL);
	ObjcAttachMenuFunc( NULL, MN1_FMODAL, my_create_modal_form, NULL);

	ObjcAttachMenuFunc( NULL, MN1_FONTSEL, my_call_fontselector, NULL);
	ObjcAttachMenuFunc( NULL, MN1_FILESEL, my_call_fileselector, NULL);
	ObjcAttachMenuFunc( NULL, MN1_FRAME1, my_create_two_frame, NULL);
	ObjcAttachMenuFunc( NULL, MN1_FRAME2, my_create_three_frame, NULL);
	ObjcAttachMenuFunc( NULL, MN1_QUIT, my_create_quit_form, NULL);  

	ObjcAttachMenuFunc( NULL, MN1_AVSTATUS, my_av_status, NULL);
	ObjcAttachMenuFunc( NULL, MN1_AVOPENDIR, my_do_av_opendir, NULL);
	ObjcAttachMenuFunc( NULL, MN1_AVSENDFILE, my_do_av_sendfile, NULL);
	ObjcAttachMenuFunc( NULL, MN1_AVSENDKEY, my_do_av_sendkey, NULL);
	ObjcAttachMenuFunc( NULL, MN1_AVVIEW, my_do_av_view, NULL);

}

/*
 * ... or by catching the MN_SELECTED event.
 */

void __CDECL menu_desktop( WINDOW *__w, short buff[8]) {
	WINDOW *win;
	int title = buff[ 3];
	INT16 x,y;
	char *str;
	
	switch( buff[4]) {
	case MN1_FD:
		create_widget_form( FALSE);
		break;
	case MN1_DUP:
		create_widget_form( TRUE);
		break;

	/* Menu Action */
	case MN1_CLOSE:
		if( wglb.front != NULL)
			ApplWrite( _AESapid, WM_CLOSED, wglb.front->handle, 0, 0, 0, 0);
		break;

	case MN1_ICONIFY:
		if( wglb.front != NULL) {
			INT16 w, h;
			give_iconifyxywh( &x, &y, &w, &h);
			/*graf_dragbox( app.wicon, app.hicon, x,y, )*/
			WindSet( wglb.front, WF_ICONIFY, x, y, w, h);
		}
		break;

	case MN1_MAXIMISE:
		if( wglb.front != NULL) 
			ApplWrite( _AESapid, WM_FULLED, wglb.front->handle, 0, 0, 0, 0);
		break;

	case MN1_CYCLE:
		{
			int top = -1;
			
			if( !wglb.front && !wglb.appfront && wglb.first) 
				top = wglb.first->handle;
			else if( wglb.front)
				top = wglb.front->next?wglb.front->next->handle:wglb.first->handle;
			else if( wglb.appfront)
				top = wglb.appfront->next?wglb.appfront->next->handle:wglb.first->handle;
				
			if( top != -1) 
				ApplWrite( _AESapid, WM_TOPPED, top, 0, 0, 0, 0);
		}
		break;
		
	case MN1_REDRAW:
		if( wglb.front != NULL)
			snd_rdw( wglb.front);
		break;

	case MN1_HIDE:
		ApplControl( _AESapid, APC_HIDE, NULL);
		break;


	case MN1_AVOPENDIR:
/*		{
            ApplWrite( app.avid, AV_OPENWIND, prg, cmd, 0);
            AvWaitfor( VA_PROGSTARTED, evnt.buf, 1000);
        } */
		break;
		
	case MN1_EXT:
		param.setup ^= SETUP_XTYPE;
		RsrcXtype( (param.setup & SETUP_XTYPE)?RSRC_XALL:0, rsc_trindex, rsc_ntree);
		win = wglb.first;
		while( win != NULL) {
			if( win->status & WS_TOOLBAR || win->status & WS_FORM)
				snd_rdw( win);
			win = win->next;
		}
		break;
	case MN1_OP2:
		create_form_conf();
		break;
	case MN1_OP3:
		str = get_string( ALRT_SAVE_PAR);
		form_alert( 1, str);
		setup_save();
		break;
	case MN1_READCONF:
		setup_read();
		form_dial( FMD_FINISH, 0, 0, 0, 0, app.x, app.y, app.w, app.h);
		break;
	case MN1_WINDOM:
		ConfRead();
		form_dial( FMD_FINISH, 0, 0, 0, 0, app.x, app.y, app.w, app.h);
		break;

	/* Configuration Windom par WinConf */
	case MN1_CONF_WINDOM:
		if( ConfWindom() < 0) FormAlert( 1, get_string( ALRT_WINCONF));
		break;

	}
	MenuTnormal( NULL, title, 1);
}
