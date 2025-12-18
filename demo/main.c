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
 * $Source: /cvsroot/windom/windom/demo/main.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/02/14 20:57:15 $
 *   $Revision: 1.9 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __PUREC__
#include "..\include\windom.h"
#else
#include "../include/windom.h"
#endif

#include "av.h"
#include "scancode.h"

#include "demo.h"
#include "global.h"

/* debug */

#ifdef ENABLE_DEMO_LOG
static char *list[] = {
	"",
	"WM_XTIMER", 	/* 1 */
	"WM_XKEYBD",
	"WM_XBUTTON",
	"WM_XM1",
	"WM_XM2",
	"","","","",
	"MN_SELECTED",	/* 10 */
	"","","","","","","","","",""
	"WM_REDRAW",	/* 20 */
	"WM_TOPPED",
	"WM_CLOSED",
	"WM_FULLED",
	"WM_ARROWED",
	"WM_HSLID",
	"WM_VSLID",
	"WM_SIZED",
	"WM_MOVED",
	"WM_NEWTOP",
	"WM_UNTOPPED",	/* 30 */
	"WM_ONTOP",
	"",
	"WM_BOTTOM",
	"WM_ICONIFY",
	"WM_UNICONIFY",
	"WM_ALLICONIFY",
	"WM_TOOLBAR",
	"", "",
	"AC_OPEN",		/* 40 */
	"AC_CLOSE", 
	"","","","","","","","",""
	"AP_TERM",		/* 50 */
	"AP_TFAIL"
	};


static char *nom_message( int msg) {
	static char nom[50];
	
	*nom = '\0';
	if( msg < 52) {
		strcpy( nom, list[msg]);
	} else switch( msg) {
		case WM_DESTROY:
			strcpy( nom, "WM_DESTROY");
			break;
		case WM_MNSELECTED:
			strcpy( nom, "WM_MNSELECTED");
			break;
		case WM_FORM:
			strcpy( nom, "WM_FORM");
			break;
		case AP_LOADCONF:
			strcpy( nom, "AP_LOADCONF");
			break;
		case AP_BUTTON:
			strcpy( nom, "AP_BUTTON");
			break;
		case AP_KEYBD:
			strcpy( nom, "AP_KEYBD");
			break;
		}
	return nom;
}
#endif

static void __CDECL keybd( WINDOW *w, short buff[8]) {
#ifdef ENABLE_DEMO_LOG
	WINDOW *win;
	FILE *fp;
	void *ev;
	
	if( (evnt.keybd >> 8) == SC_F10) {
		fp = fopen( "demo.log", "w");
		if( fp) {
			fprintf( fp, "global events:\n");
			for( ev = app.binding; ev; ev = ev->next) 
				fprintf( fp, "\t%d (%s) 0x%X %ld\n", ev->msg, 
					nom_message( ev->msg), ev->flags, ev->proc);
			for( win=wglb.first; win; win = win->next) {
				fprintf( fp, "window %d events:\n", win->handle);
				for( ev = win->binding; ev; ev = ev->next) 
					fprintf( fp, "\t%d (%s) 0x%X %ld\n", ev->msg, 
							nom_message( ev->msg), ev->flags, ev->proc);
			}
			fclose( fp);
		}
	}
#endif
}

int main( void) {
	OBJECT *menu;
	
	/* GEM-LIB dependant init */
	ApplInit();

	/* Load the application resource */
	rsc = RsrcXload( "demo.rsc");
	
	/* in case of failure, try to locate the RSC file */
	if( rsc==NULL && form_alert(1,"[1][resource file not found !][Locate|End]") == 1 ) {
		char name[256];
		char path[256];
		strcpy(name,"demo.rsc");
		path[0] = 0;
		if (FselInput(path,name,"*.rsc","locate windom demo RSC file",NULL,NULL) == 1) {
			strcat(path,name);
			rsc = RsrcXload( path);
		}
	}
	
	/* No RSC loaded => exit */
	if (rsc==NULL) {
		ApplExit();
		exit( 0);
	}
	
	rsc_trindex = RsrcGhdr(rsc) -> trindex;
	rsc_ntree   = RsrcGhdr(rsc) -> ntree;
	
	sprintf( ObjcString(get_tree(DIAL1), DIAL1_COMPILER, NULL),
		 "RELEASE %d.%d.%d" __WINDOM_BETATAG__, __WINDOM_MAJOR__, __WINDOM_MINOR__, __WINDOM_REVISION__);
	menu = get_tree( MENU1);

	/* Init frame environment */
	FrameInit();

	
	/* Init AVserver environnement */
	if( AvInit( "DEMO    ", A_START|A_QUOTE, 1000 ) < 0) {
		menu[MN1_AVSENDFILE].ob_state |= DISABLED;
		menu[MN1_AVOPENDIR].ob_state |= DISABLED;
		menu[MN1_AVSENDKEY].ob_state |= DISABLED;
		menu[MN1_AVVIEW].ob_state |= DISABLED;
	} else {
		INT16 *status = AvStatus();
		
		if( (status[0] & V_SENDKEY) == 0)
			menu[MN1_AVSENDKEY].ob_state |= DISABLED;	
		if( (status[1] & V_VIEW) == 0)
		{	FormAlert( 1, "[1][houpla][ok]");
		 menu[MN1_AVVIEW].ob_state |= DISABLED;	}
		if( (status[0] & V_STARTPROG) == 0)
			menu[MN1_AVSENDFILE].ob_state |= DISABLED;	
		if( (status[0] & V_OPENWIND) == 0)
			menu[MN1_AVOPENDIR].ob_state |= DISABLED;	
	}
	
	/* Install extended type for objects */
	RsrcXtype( RSRC_XALL, rsc_trindex, rsc_ntree);

	/* Install menu desktop */
	MenuBar( menu, 1);

	/* Register application name in menu if possible */
	if( _AESnumapps == -1)
		menu_register( _AESapid, "  WinDom demo ");

	param.desk = get_tree( FORM11);
	setup_read();

	/* Install the desktop */
	if( param.setup & SETUP_INSTALL_DESKTOP) {
		param.desk->ob_x 	  = app.x;
		param.desk->ob_y 	  = app.y;
		param.desk->ob_width  = app.w;
		param.desk->ob_height = app.h;
		WindSetPtr( NULL, WF_NEWDESK, param.desk, NULL);
		/* Redraw the desktop */
		form_dial (FMD_FINISH, 0, 0, 0, 0, app.x, app.y, app.w, app.h);
	}
	
	/* BubbleGEM */
	LoadBubble( "demo.bub");

	/* initialise timer value */
	evnt.timer = 1000L;
	
	/* Bind global events */
	EvntAttach( NULL, AP_TERM,      ap_term);
	EvntAttach( NULL, AV_SENDCLICK, av_sendclick);
	EvntAttach( NULL, AV_SENDKEY, 	av_sendkey);
	EvntAttach( NULL, MN_SELECTED,  menu_desktop);
	EvntAttach( NULL, WM_XBUTTON,   mu_button);
	EvntAttach( NULL, WM_XKEYBD,    keybd);
	menu_binding();

	/* Events handling */
	for(;;) EvntWindom( MU_MESAG | MU_TIMER | MU_KEYBD | MU_BUTTON);
	
	return 0;
}
