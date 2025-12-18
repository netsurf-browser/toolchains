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
 * $Source: /cvsroot/windom/windom/examples/frame/txtframe.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/23 19:23:48 $
 *   $Revision: 1.9 $
 */

#include <stdlib.h>
#include "mt_wndm.h"
#include "viewtext.h"	/* A WinDom wrapper to display
						 * simple ascii file in a window */


/* FIXME: there should exist some factories instead of using those forward declarations
 *        for WinDom internal functions. Or we can rename and make them public.
 */

/* from src/comp_widget.c */
COMPONENT *comp_widget_create( APPvar *app, WINDOW *win, int size, int flex );

/* from src/comp_split.c */
COMPONENT *comp_split_create( APPvar *app, int mode, int sz );


/* temporary demonstration define */
#define CDT_COLOR 0x434f4cUL  /* 'COL' */

static
void __CDECL color_box_redraw( COMPONENT *c, long buff[8], APPvar *app ) {
	INT16 xy[4];
	LGRECT work;

	// get the work rectangle...
	mt_CompGetLGrect(app, c, WF_WORKXYWH, &work);

	// fill in the array for v_bar (leave clipping to upper layer)
	xy[0] = work.g_x;
	xy[1] = work.g_y;
	xy[2] = xy[0] + work.g_w - 1;
	xy[3] = xy[1] + work.g_h - 1;

	// set the color
	vsf_color( c->graf->handle, (long)mt_CompDataSearch(app, c, CDT_COLOR) );

	// draw the contents
	v_bar( c->graf->handle, xy);
}


static
void ApTerm( WINDOW *win, short wbuff[8], APPvar *app) {
	while( app->wglb.first) {
		mt_ApplWrite( app, WM_CLOSED, app->wglb.first->handle,0,0,0,0,0);
		mt_EvntWindom( app, MU_MESAG);
	}
	mt_FrameExit(app);	
	mt_ApplExit(app);
	exit( 0);
}

int main( void) {
	WINDOW *win, *frame;
	COMPONENT *c, *u, *p;
	
	APPvar*	app = mt_ApplInit();
	if (!app)
		return -1;

	mt_FrameInit(app);
	
	mt_FormAlert(app,1,"[1][This example uses the|new experimental COMPONENT API|to manage its frames][continue]");

       	/* component container */
	p = mt_CompCreate( app, CLT_VERTICAL, 0, 1 );
	
	/* upper horizontal component */
	u = mt_CompCreate( app, CLT_HORIZONTAL, 0, 1 );
	mt_CompAttach( app, p, u );
	
	{
		frame = mt_ViewTextCreate( app, "viewtext.c", WAT_ALL-SIZER-VSLIDE);
		mt_WindSetStr( app, frame, WF_INFO, "Component Window Info" );
		c = comp_widget_create( app, frame, 100, 0 );
		mt_CompAttach( app, u, c );
	       	/* make the component sizable in width 100-200 */
		c->bounds.max_width = 200;

		c = comp_split_create( app, CS_HSLIDE, 3 );
		mt_CompEvntAttach( app, c, WM_REDRAW, color_box_redraw );
		mt_CompDataAttach( app, c, CDT_COLOR, (void*)LBLACK );
		mt_CompAttach( app, u, c );

		frame = mt_ViewTextCreate( app, "viewtext.c", WAT_ALL-SIZER-VSLIDE);
		mt_WindSetStr( app, frame, WF_INFO, "Component Window Info" );
		mt_CompAttach( app, u, comp_widget_create( app, frame, 40, 1 ) );

		c = comp_split_create( app, CS_HSLIDE, 3 );
		mt_CompEvntAttach( app, c, WM_REDRAW, color_box_redraw );
		mt_CompDataAttach( app, c, CDT_COLOR, (void*)LBLACK );
		mt_CompAttach( app, u, c );

		frame = mt_ViewTextCreate( app, "viewtext.c", WAT_ALL-SIZER-HSLIDE);
		mt_WindSetStr( app, frame, WF_INFO, "Component Window Info" );
		mt_CompAttach( app, u, comp_widget_create( app, frame, 40, 1 ) );
	}

	c = comp_split_create( app, CS_HSLIDE, 3 );
	mt_CompEvntAttach( app, c, WM_REDRAW, color_box_redraw );
	mt_CompDataAttach( app, c, CDT_COLOR, (void*)LBLACK );
	mt_CompAttach( app, p, c );

	/* lower frame component */
	frame = mt_ViewTextCreate( app, "viewtext.c", WAT_ALL-INFO);
	mt_CompAttach( app, p, comp_widget_create( app, frame, 0, 1 ) );
	mt_WindSetStr( app, frame, WF_INFO, "Component Window Info" );


	/* now create the real AES WINDOW (top-level) */
	win = mt_WindCreate( app, NAME|MOVER|INFO|FULLER|SMALLER|CLOSER,
			app->x, app->y, app->w, app->h);
	/* link the COMPONENT container */
	mt_WindSetPtr( app, win, WF_COMPONENT, p, NULL);

	mt_WindSetStr( app, win, WF_INFO, "Real AES Window Info" );
	mt_WindSetStr( app, win, WF_NAME, "Component layout test");

	mt_WindOpen( app, win, 100, 100, 300, 300);

	{
		short wbuff[8];
		/* recompute the nested component sizes and positions */
		mt_WindGetGrect( app, win, WF_CURRXYWH, (GRECT*)&wbuff[4] );
		wbuff[0] = CM_REFLOW;
		mt_EvntExec(app, win, wbuff );
		/* FIXME: I don't like this at all here... should be inside WINDOM */
	}

	mt_EvntAttach( app, NULL, AP_TERM, ApTerm);
	for( ;; ) mt_EvntWindom( app, MU_MESAG|MU_BUTTON|MU_KEYBD|MU_TIMER);

	return 0;
}

