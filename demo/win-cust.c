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
 * $Source: /cvsroot/windom/windom/demo/win-cust.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/03/22 20:58:31 $
 *   $Revision: 1.14 $
 */

#include <stdlib.h>
#include <windom.h>
#include "demo.h"
#include "global.h"

/* First of all, we define the data related to
 * the window.
 */

typedef struct {
	int color;		/* foreground color */
	int type;		/* SQUARE, ELLIPSE or CIRCLE */
	int bckgrd;		/* background color */
} DRAW;

/* The WM_REDRAW event function. This function is
 * called when the window is drawn.
 * Notice we must not clip the redraw area (this is
 * done by EvntWindom()).
 */

static void __CDECL draw_custom( WINDOW *win, short buff[8], void *_draw) {
	INT16 x,y,w,h;
	INT16 xy[4];
	DRAW *draw = _draw;
	short vdih = win->graf->handle;
 	
	/* get the work area coordinates */
	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);

	/* Background */
	xy[0] = x;xy[1] = y;xy[2] = x+w-1;xy[3] = y+h-1;
	switch( draw->bckgrd){
	case MN2_WHITE:
		vsf_color( vdih, WHITE);
		vsf_interior( vdih, FIS_SOLID);
		break;
	case MN2_BLACK:
		vsf_color( vdih, BLACK);
		vsf_interior( vdih, FIS_SOLID);
		break;
	case MN2_GREY:
		vsf_color( vdih, BLACK);
		vsf_style( vdih, 4);
		vsf_interior( vdih, FIS_PATTERN);
		break;
	}
	v_bar( vdih, xy);

	/* style */	
	if( app.color < 16) {
		/* low color resolution */
		vsf_interior( vdih, FIS_PATTERN);
		vsf_style( vdih, draw->color);
	} else {
		/* high color resolution */
		vsf_interior( vdih, FIS_SOLID);
		vsf_color( vdih, draw->color);
	}
	
	/* Foreground */
	switch( draw->type){
	case MN2_SQUARE:
		xy[0] += w/8;
		xy[1] += h/8;
		xy[2] -= w/8;
		xy[3] -= h/8;
		v_bar( vdih, xy);
		break;
	case MN2_CIRCLE:
		v_circle( vdih, x+w/2-1,y+h/2-1, MIN(w,h)/3);
		break;
	case MN2_ELLIPSE:
		v_ellipse( vdih, x+w/2-1,y+h/2-1, w/3, h/3);
		break;
	}
}

/*
 *	MU_TIMER event function.
 *	When a timer event occurs, the function changes
 *  the foreground color.
 */

static void __CDECL timer_custom( WINDOW *win, short buff[8], void *_draw) {
	DRAW *draw = _draw;
	draw->color = ++draw->color % 16;
	snd_rdw( win);
}

/*
 * The WM_DESTROY event function.
 * The function releases the memory, closes,
 * and destroys the window.
 */

static void __CDECL destroy_custom( WINDOW *win, short buff[8], void *draw) {
	free(draw);
}

/* This callback function is called when the
 * user operates in a menu window */

static void __CDECL menu_hilight( WINDOW *win, short buff[8]) {
	MenuTnormal( win, buff[3], 1);
}

/* Other functions are callback binded on
 * specific menu item object */

static void __CDECL mn2_info( WINDOW *win, int item, int title, void *data ) {
	display_info( win, 0);	
}

static void __CDECL mn2_fast( WINDOW *win, int item, int title, void *data ) {
	if (evnt.timer > 300) evnt.timer -= 300;
}

static void __CDECL mn2_slow( WINDOW *win, int item, int title, void *data ) {
	if (evnt.timer < 10000) evnt.timer += 300;
}

static void __CDECL mn2_pattern( WINDOW *win, int index, int title, void *_draw) {
	DRAW *draw = _draw;
	draw->type = index; 
	snd_rdw( win);
}

static void __CDECL mn2_color( WINDOW *win, int index, int title, void *_draw) {
	DRAW *draw = _draw;
	draw->bckgrd = index; 
	snd_rdw( win);
}

/* Create the window with its attributes */

void create_custom( void) {
	WINDOW *win;
	OBJECT *tree;
	DRAW *draw;
	
	/* Create the window */
	win = WindCreate( NAME|CLOSER|MOVER|SMALLER|MENUBAR, app.x, app.y, app.w, app.h);
	
	if (win==NULL) {
		FormAlert(1, FA_ERROR "[The creation of the AES window|has failed][continue]");
		return;
	}
	
	/* MENUBAR is a special widget usefull for system supporting
	 * window menu (like xaAES). WinDom can support window menu but leaves the
	 * choice of menu handling (windom or system) to user (via WinDom Configuration)
	 */

	/* Create data */
	draw = (DRAW*)malloc(sizeof(DRAW));
	draw->bckgrd = MN2_GREY;
	draw->type   = MN2_SQUARE;
	draw->color = 0;
	
	/* Attach the menu */
	tree = get_tree(MENU2);
	WindSetPtr( win, WF_MENU, tree, menu_hilight);

	/* Here we bind callback function to objects */
	ObjcAttachMenuFunc( win, MN2_INFO,    mn2_info, NULL);
	ObjcAttachMenuFunc( win, MN2_FAST,    mn2_fast, NULL);
	ObjcAttachMenuFunc( win, MN2_SLOW,    mn2_slow, NULL);
	ObjcAttachMenuFunc( win, MN2_CIRCLE,  mn2_pattern, draw);
	ObjcAttachMenuFunc( win, MN2_SQUARE,  mn2_pattern, draw);
	ObjcAttachMenuFunc( win, MN2_ELLIPSE, mn2_pattern, draw);
	ObjcAttachMenuFunc( win, MN2_WHITE,   mn2_color, draw);
	ObjcAttachMenuFunc( win, MN2_BLACK,   mn2_color, draw);
	ObjcAttachMenuFunc( win, MN2_GREY,    mn2_color, draw);

	/* Give a title to window */
	WindSetStr( win, WF_NAME, get_string( MENU_TITLE));
	/* Attach event functions */
	EvntDataAttach( win, WM_REDRAW, draw_custom, draw);
	EvntDataAdd( win, WM_DESTROY, destroy_custom, draw, EV_TOP);
	EvntDataAttach( win, WM_XTIMER, timer_custom, draw);

	/* open the window */
	WindOpen( win, -1, -1, 280, 200);
}
