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
 * $Source: /cvsroot/windom/windom/demo/win-tool.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/03/22 20:58:31 $
 *   $Revision: 1.11 $
 */

#include <stddef.h>
#include <string.h>
#include <windom.h>
#include "global.h"
#include "demo.h"


/* *********************************
 *	A simple window with a toolbar
 * ********************************* */
 
static void toolbar_popup( WINDOW *win, short next) {
	char *plist[] = { "item 1 ", "item 2 ", "item 3 ", "item 4 " };
	short x, y;
	/* remember the previous choice in popup */
	static int prev = 1;
	
	if (next) {
		x = prev + 1;
		if (x>4) x=1;
	} else {
		objc_offset( win->tool.root, TL1_POPUP, &x, &y);
		x = MenuPopUp( plist,
					   x, y, 
					   4, -1, prev, 
					   P_LIST | P_WNDW);
	}
	
	if( x != -1) {
		prev = x;
		strcpy( ObjcString(TOOL(win), TL1_POPUP, NULL), "  ");
		strcat( ObjcString(TOOL(win), TL1_POPUP, NULL), plist[x-1]);
		ObjcDraw( OC_TOOLBAR, win, TL1_POPUP, 1);
	}
}

/* The toolbar event function */

static void toolbarTOOL1( WINDOW *win, short buff[8]) {
	int obj = buff[4];	/* Selected object */
	
	switch( obj) {
	case TL1_FERM:
		/* Button closing the window */
		ApplWrite( _AESapid, WM_CLOSED, win->handle, 0, 0, 0, 0);
		break;
	case TL1_POPUP:
		/* Call of a list-popup  */
		toolbar_popup(win,0);
		break;
	case TL1_CYCLE:
		/* chose next item of the popup */
		toolbar_popup(win,1);
		break;
	}
	
	/* object state to normal and redraw object */
	ObjcChange( OC_TOOLBAR, win, obj, NORMAL, /*1*/ OC_MSG);
}


/* Create the window with the toolbar */

void create_windowtoolbar( void) {
	/* Create the window */
	WINDOW *win = WindCreate( NAME|CLOSER|MOVER|SIZER|FULLER|SMALLER, 
					  		  app.x, app.y, app.w, app.h);
	if (win==NULL) {
		FormAlert(1, FA_ERROR "[The creation of the AES window|has failed][continue]");
		return;
	}
	/* Set the window title */
	WindSetStr( win, WF_NAME, get_string( TOOLBAR_TITLE));
	/* Set the window toolbar */
	WindSetPtr( win, WF_TOOLBAR, get_tree( TOOL1), toolbarTOOL1);
	/* bind some events */
	EvntAdd( win, WM_DESTROY, stdDESTROY, EV_TOP);
	/* Open the window, centered at screen */
	WindOpen( win, -1, -1, app.w/3, app.h/2);
}

/* ****************************************************
 * A second example of window with a toolbar and a menu
 * **************************************************** */

static void toolbar2_popup( WINDOW *win, short next) {
	char *plist[] = { "choice1 ", "choice2 ", "choice3 ", "choice4 " };
	short x, y;
	/* remember the previous choice in popup */
	static int prev = 1;
	
	if (next) {
		x = prev + 1;
		if (x>4) x=1;
	} else {
		objc_offset( win->tool.root, TL2_POPUP, &x, &y);
		x = MenuPopUp( plist,
					   x, y, 
					   4, -1, prev, 
					   P_LIST | P_WNDW);
	}
	
	if( x != -1) {
		prev = x;
		strcpy( ObjcString(TOOL(win), TL2_POPUP, NULL), "  ");
		strcat( ObjcString(TOOL(win), TL2_POPUP, NULL), plist[x-1]);
		ObjcDraw( OC_TOOLBAR, win, TL2_POPUP, 1);
	}
}

/* The toolbar event function */

static void toolbarTOOL2( WINDOW *win, short buff[8]) {
	int obj = buff[4];	/* Selected object */
	
	switch( obj) {
	case TL2_FERM:
		/* Button closing the window */
		ApplWrite( _AESapid, WM_CLOSED, win->handle, 0, 0, 0, 0);
		break;
	case TL2_POPUP:
		/* Call of a list-popup  */
		toolbar2_popup(win,0);
		break;
	case TL2_CYCLE:
		/* chose next item of the popup */
		toolbar2_popup(win,1);
		break;
	}
	
	/* object state to normal and redraw object */
	ObjcChange( OC_TOOLBAR, win, obj, NORMAL, /*1*/ OC_MSG);
}

static void simpleMENU( WINDOW *win, short buff[8]) {
	MenuTnormal( win, buff[3], 1);
}

void create_windowtoolmenu( void) {
	/* Create the window */
	WINDOW *win = WindCreate( NAME|CLOSER|MOVER|SMALLER|MENUBAR, app.x, app.y, app.w, app.h);
	if (win==NULL) {
		FormAlert(1, FA_ERROR "[The creation of the AES window|has failed][continue]");
		return;
	}
	/* set the menu and the toolbar */
	WindSetPtr( win, WF_MENU, 	 get_tree( MENU2), simpleMENU);
	WindSetPtr( win, WF_TOOLBAR, get_tree( TOOL2), toolbarTOOL2);
	/* set the window title */
	WindSetStr( win, WF_NAME, get_string( MENUTOOL_TITLE));
	/* bind of events */
	EvntAdd( win, WM_DESTROY, stdDESTROY, EV_TOP);
	/* open the window */
	WindOpen( win, app.x+50, app.y+50, app.x+350, app.x+200);
}
