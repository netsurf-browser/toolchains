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
 * $Source: /cvsroot/windom/windom/demo/win-divr.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/03/22 20:58:31 $
 *   $Revision: 1.7 $
 */

#include <stddef.h>   /* needed for NULL definition */
#include <windom.h>
#include "global.h"
#include "demo.h"


/*
 * A simple window
 */

static void __CDECL simpleDRAW( WINDOW *win, short buff[8]) {
	INT16 line[4];
	INT16 x, y, w, h;
	short vdih = win->graf->handle;
	UNUSED( buff);	/* prevent warning from compilation */
	
	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);
	
	vsl_width( vdih, 3);
	line[0] = x;
	line[1] = y;
	line[2] = x+w-1;
	line[3] = y+h-1;
	vsl_color( vdih, BLUE);
	v_pline( vdih, 2, line);
	
	line[0] = x+w-1;
	line[1] = y;
	line[2] = x;
	line[3] = y+h-1;
	vsl_color( vdih, RED);
	v_pline( vdih, 2, line);
}

/*
 *	Example of MU_BUTTON event function apply on a window 
 */
 
static void __CDECL simpleCLICK( WINDOW *win, short buff[8]) {
	UNUSED( win); UNUSED( buff);	/* prevent warning from compilation */
	FormAlert( 1 , get_string( ALRT_CLIC));
}

/*
 *	Example of MU_KEYBD event function apply on a window
 */

static void __CDECL simpleKEY( WINDOW *win, short buff[8]) {
	UNUSED( win); UNUSED( buff);	/* prevent warning from compilation */
	FormAlert( 1, get_string( ALRT_KEY), evnt.keybd, ((char)evnt.keybd) ? (char)evnt.keybd : ' ');
}

void create_simple( void) {
	/* Create the window */
	WINDOW *win = WindCreate( WAT_NOINFO, app.x, app.y, app.w, app.h);
	
	if (win==NULL) {
		FormAlert(1, FA_ERROR "[The creation of the AES window|has failed][continue]");
		return;
	}
	
	/* Set the window title */
	WindSetStr( win, WF_NAME, get_string( SIMPLE_TITLE));
	/* Icon settings */
	WindSetStr( win, WF_ICONTITLE, get_string( APP_NAME));
	WindSetPtr( win, WF_ICONDRAW , draw_mono_icon, NULL);
	/* Define some window events */
	EvntAttach( win, WM_XBUTTON, simpleCLICK);
	EvntAttach( win, WM_XKEYBD,  simpleKEY );
	EvntAdd	  ( win, WM_DESTROY, stdDESTROY, EV_TOP);
	EvntAdd	  ( win, WM_REDRAW,  simpleDRAW, EV_BOT);
	
	/* Open the window */
	WindOpen( win, -1, -1, app.w/2, app.h/2);

	vsf_perimeter( win->graf->handle, 0);
}



/*
 * A simple example of calling the font selector
 */
		
void call_fontselector( void) {
	char name[60];
	int id=0, size=0;
	FontSel( get_string( FONT_TITLE), NULL, 0, &id, &size, name);
}

/*
 * A simple example of calling the file selector,
 * the last folder visited and file selected are keep in memory.
 */
		
void call_fileselector( void) {
	static char path[128];
	static char name[60];

	FselInput( path, name, "*", get_string(FSEL_TITLE), NULL, NULL);
}


/* End of file */
