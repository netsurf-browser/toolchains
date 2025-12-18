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
 * $Source: /cvsroot/windom/windom/examples/tutorial/tut-2-1.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:39:25 $
 *   $Revision: 1.4 $
 */

/**
 * \file tut-2-1.c
 * Tutorial WinDom 
 * \version 1.0.0
 * \author Dominique B&eacute;r&eacute;ziat
 * \brief Create, open and handle a non-proportional window.
 *
 * Explain how sliders are handled in WinDom.
 *
 * $Id: tut-2-1.c,v 1.4 2006/01/03 14:39:25 bercegeay Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windom.h>

#include "text.h"

/**
 * Event function drawing the window workarea
 */

static 
void __CDECL evnt_draw_text( WINDOW *win, short buff[8], void *data) {
  short x, y, w, h;
  short hcell, hcar;
  short i, attr[10];
  short y_max, h_offset;
  TEXT *ptext = (TEXT *) data;

  /* Get window workarea coordinate */
  WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);
  y_max = MIN( y + h, win->graf->clip.g_y + win->graf->clip.g_h) - 1;

  /* Get height of cellule and character */
  vqt_attributes( win->graf->handle, attr);
  hcell = attr[9];
  hcar  = attr[7];
  
  /* Foreground */
  vswr_mode( win->graf->handle, MD_TRANS);

  /* vertical offset for a nice text drawing */
  h_offset = hcell - hcar; 
  
  /* from the first line visible to the end ...
   * Convention: we have always 0 <= win->ypos < win->yposmax */
  for( i=win->ypos; i<win->ypos_max ; i++)  {
    y += hcell;

    /* If the line is upper than the clipped zone? */
    if( y < win->graf->clip.g_y)
      continue;
    
    /* line inside the window ? */
    if( strlen( ptext->line[ i]) > win->xpos) 
      v_gtext(win->graf->handle, x, y - h_offset, ptext->line[i] + win->xpos);
    
    /* End if the line is downer than the clipped zone */
    if (y > y_max) 
      break;
  }
}

/**
 * Event function called when the window is destroyed.
 */

static
void __CDECL evnt_destroy_text( WINDOW *win, short buff[8], void *text) {
	free_text( text);
}

/**
 * Open a window, setup event functions and slider parameters.
 */

WINDOW *OpenText( TEXT *text) {
	WINDOW *win;
	short attrib[10];
     
	win = WindCreate( WAT_NOINFO, app.x, app.y, app.w, app.h);
	EvntDataAdd( win, WM_REDRAW, evnt_draw_text, text, EV_BOT);
	EvntDataAdd( win, WM_DESTROY, evnt_destroy_text, text, EV_TOP);

	/* Maximal lenght of a line */
	win -> xpos_max = 255;
	/* Number of line */
	win -> ypos_max = text->maxline;
	vqt_attributes( app.graf.handle, attrib);
	/* Height of a cell character */
	win -> w_u = attrib[8];  
	/* Width of a cell character */
	win -> h_u = attrib[9];  
	/* Height of a character */
	text -> wchar = attrib[6];
	/* Width of a character */
	text-> hchar = attrib[7];
	/* Open the window */
	WindOpen( win, app.x, app.y, app.w, app.h);
	/* Update the size and position of sliders */
	WindSlider( win, HSLIDER|VSLIDER);
	return win;
}

/**
 * Global events
 */

static
void __CDECL evnt_apterm( ) {
	while( wglb.first) {
		ApplWrite( _AESapid, WM_DESTROY, wglb.first->handle, 0, 0, 0, 0);
		EvntWindom( MU_MESAG);
	}
}

/**
 * Main Part
 */

int main() {
	TEXT *text;
	WINDOW *win;

	ApplInit();

	text = load_text( __FILE__);
	EvntAttach( NULL, AP_TERM, evnt_apterm);

	if( text == NULL)
		FormAlert( 1, FA_ERROR "[Error: file not found !][End]");
	else { 
		win = OpenText( text);
		WindSetStr( win, WF_NAME, __FILE__ ": display a text file");
		while( wglb.first) EvntWindom( MU_MESAG);
	}
	ApplExit();
	return EXIT_SUCCESS;
}
