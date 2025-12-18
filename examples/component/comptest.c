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
 * $Source: /cvsroot/windom/windom/examples/component/comptest.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:39:25 $
 *   $Revision: 1.6 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#include "mt_wndm.h"

/* temporary demonstration defines */
#define CDT_COLOR 0x434f4cUL  /* 'COL' */

/* FIXME!!! from src/comp_szd.c */
COMPONENT *comp_reflow( COMPONENT *c, APPvar *app );


static
void mt_comp_reflow( APPvar *app, WINDOW *win, GRECT *rect ) {
	short buff[8];
	if ( !rect ) {
		mt_WindGetGrect( app, win, WF_CURRXYWH, (GRECT*)&buff[4]);
	} else {
		*(GRECT*)&buff[4] = *rect;
	}
	buff[0] = CM_REFLOW; /* recompute the nested component sizes */
	mt_EvntExec(app, win, buff );
}


static void __CDECL color_box_redraw( COMPONENT *c, long buff[8], APPvar *app ) {
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

/* blink! */
static void __CDECL color_box_click( COMPONENT *c, long *buff, APPvar *app ) {
	void *color = mt_CompDataSearch(app, c, CDT_COLOR);

	mt_CompDataAttach( app, c, CDT_COLOR, (void*)BLACK );
	mt_CompEvntRedraw( app, c);

	mt_CompDataAttach( app, c, CDT_COLOR, color );
	mt_CompEvntRedraw( app, c);

	/* let the component become hidden ;) */
	c->status |= CS_HIDDEN;
	mt_CompEvntRedraw( app, comp_reflow( c->parent, app) );
}


int main( void) {
	COMPONENT *p, *f, *w, *c;

	APPvar*	app = mt_ApplInit();
	if (!app)
		return -1;

	p = mt_CompCreate( app, CLT_VERTICAL, 0, 1 ); /* used just as a container */

	c = mt_CompCreate( app, CLT_VERTICAL, 30, 0 );
	mt_CompEvntAttach( app, c, WM_REDRAW, color_box_redraw );
	mt_CompDataAttach( app, c, CDT_COLOR, (void*)BLUE );
	mt_CompAttach( app, p, c );

	f = c = mt_CompCreate( app, CLT_HORIZONTAL, 150, 2 );
	// c->bounds.min_height = 300;
	mt_CompDataAttach( app, c, CDT_COLOR, (void*)0xf1 /*LCYAN*/ );
	mt_CompAttach( app, p, f );

	{
		c = mt_CompCreate( app, CLT_VERTICAL, 50, 0 );
		mt_CompEvntAttach( app, c, WM_REDRAW, color_box_redraw );
		mt_CompDataAttach( app, c, CDT_COLOR, (void*)LWHITE );
		mt_CompAttach( app, f, c );

		w = c = mt_CompCreate( app, CLT_VERTICAL, 0, 1 );
		c->bounds.min_width = 150;
		c->bounds.max_width = 250;
		c->bounds.min_height = 250;
		mt_CompDataAttach( app, c, CDT_COLOR, (void*)0xf2 /*LCYAN*/ );

		{
			c = mt_CompCreate( app, CLT_VERTICAL, 30, 2 );
			mt_CompEvntAttach( app, c, WM_REDRAW, color_box_redraw );
			mt_CompDataAttach( app, c, CDT_COLOR, (void*)LRED );
			c->bounds.max_height = 120;
			mt_CompAttach( app, w, c );

			c = mt_CompCreate( app, CLT_VERTICAL, 100, 1 );
			c->bounds.max_height = 150;
			mt_CompEvntAttach( app, c, WM_REDRAW, color_box_redraw );
			mt_CompDataAttach( app, c, CDT_COLOR, (void*)LCYAN );
			mt_CompAttach( app, w, c );

			c = mt_CompCreate( app, CLT_VERTICAL, 30, 0 );
			mt_CompEvntAttach( app, c, WM_REDRAW, color_box_redraw );
			mt_CompDataAttach( app, c, CDT_COLOR, (void*)BLUE );
			mt_CompAttach( app, w, c );
		}

		mt_CompAttach( app, f, w );

		c = mt_CompCreate( app, CLT_VERTICAL, 0, 1 );
		mt_CompEvntAttach( app, c, WM_REDRAW, color_box_redraw );
		mt_CompEvntAttach( app, c, WM_XBUTTON, color_box_click );
		mt_CompDataAttach( app, c, CDT_COLOR, (void*)LCYAN );
		// c->status |= CS_HIDDEN;
		mt_CompAttach( app, f, c );

		c = mt_CompCreate( app, CLT_VERTICAL, 40, 0 );
		mt_CompEvntAttach( app, c, WM_REDRAW, color_box_redraw );
		mt_CompDataAttach( app, c, CDT_COLOR, (void*)GREEN );
		mt_CompAttach( app, f, c );
	}

	c = mt_CompCreate( app, CLT_VERTICAL, 20, 1 );
	c->bounds.max_height = 140;
	mt_CompEvntAttach( app, c, WM_REDRAW, color_box_redraw );
	mt_CompEvntAttach( app, c, WM_XBUTTON, color_box_click );
	mt_CompDataAttach( app, c, CDT_COLOR, (void*)RED );
	mt_CompAttach( app, p, c );

	c = mt_CompCreate( app, CLT_VERTICAL, 40, 0 );
	mt_CompEvntAttach( app, c, WM_REDRAW, color_box_redraw );
	mt_CompDataAttach( app, c, CDT_COLOR, (void*)GREEN );
	mt_CompAttach( app, p, c );

	{
		WINDOW *win = mt_WindCreate( app, NAME|SIZER|MOVER|FULLER|SMALLER|CLOSER|HSLIDE|VSLIDE,
									 app->x, app->y, app->w, app->h);
		mt_WindSetPtr( app, win, WF_COMPONENT, p, NULL);
		mt_WindSetStr( app, win, WF_NAME, "Component layout test");
		mt_WindOpen( app, win, 50, 50, 300, 200);

		mt_comp_reflow( app, win, NULL); /* assure the contents fits the window position and size */
	}

	for(;;) mt_EvntWindom( app, MU_MESAG|MU_BUTTON);
	return 0;
}
