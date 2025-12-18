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
 * $Source: /cvsroot/windom/windom/demo/util.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/03/22 20:58:31 $
 *   $Revision: 1.8 $
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <windom.h>
#include "global.h"
#include "demo.h"

/* Return a string from resource file */

char *get_string( int idx) {
	char *txt;	
	RsrcGaddr( rsc, R_STRING, idx,  &txt);
	return txt;
}

OBJECT *get_tree( int idx) {
  OBJECT *tree;
  RsrcGaddr( rsc, R_TREE, idx, &tree);
  return tree;
}


/*
 *	Display informations about a window 
 */

void display_info( WINDOW *win, int handle) {
	OBJECT *tree;

	tree = get_tree( DIAL3);
	if( win != NULL) {
		sprintf( ObjcString( tree, DL3_GEM, NULL), "%d", win -> handle);
		if( win -> status & WS_FORM)
			strcpy( ObjcString( tree, DL3_TYPE, NULL), get_string( TYPE_FORM));
		else
			strcpy( ObjcString( tree, DL3_TYPE, NULL), get_string( TYPE_ANY));
		if( win -> attrib & SMALLER)
			strcpy( ObjcString( tree, DL3_ICON, NULL), get_string( ANS_YES));
		else
			strcpy( ObjcString( tree, DL3_ICON, NULL), get_string( ANS_NO));
		if( win -> status & WS_TOOLBAR)
			strcpy( ObjcString( tree, DL3_TOOL, NULL), get_string( ANS_YES));
		else
			strcpy( ObjcString( tree, DL3_TOOL, NULL), get_string( ANS_NO));
		if( win -> status & WS_MENU)
			strcpy( ObjcString( tree, DL3_MENU, NULL), get_string( TYPE_FORM));
		else
			strcpy( ObjcString( tree, DL3_MENU, NULL), get_string( TYPE_FORM));
	} else {
		sprintf( ObjcString( tree, DL3_GEM, NULL), "%d", handle);
		strcpy( ObjcString( tree, DL3_TYPE, NULL), get_string( TYPE_ALIEN));
		strcpy( ObjcString( tree, DL3_ICON, NULL), "-");
		strcpy( ObjcString( tree, DL3_TOOL, NULL), "-");
		strcpy( ObjcString( tree, DL3_MENU, NULL), "-");
	}
	FormWindBegin( tree, get_string( WINDNAME));
	FormWindDo(MU_MESAG|MU_KEYBD|MU_TIMER);
	FormWindEnd();
	ObjcChange( OC_OBJC, tree, DL3_OUI, NORMAL, 0);
}

/*
 * Example of window icon drawing function
 */

void draw_color_icon( WINDOW *win) {
	OBJECT *tree = get_tree( COLOR_ICON);
	INT16 w, h;
	
	WindGet( win, WF_WORKXYWH, &tree->ob_x, &tree->ob_y, &w, &h);
	objc_draw_grect( tree, 0, 2, &win->graf->clip);
}

void draw_mono_icon( WINDOW *win) {
	OBJECT *tree = get_tree( MONO_ICON);
	INT16 w, h;
	
	WindGet( win, WF_WORKXYWH, &tree->ob_x, &tree->ob_y, &w, &h);
	objc_draw_grect( tree, 0, 2, &win->graf->clip);
}


/*
 *	User closer event function
 */

void __CDECL stdDESTROY( WINDOW *win, short buff[8]) {
	UNUSED( buff);
}
