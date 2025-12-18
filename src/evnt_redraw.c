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
 * $Source: /cvsroot/windom/windom/src/evnt_redraw.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/02/22 18:28:10 $
 *   $Revision: 1.16 $
 */


#include <stdio.h>
#include <stdlib.h>
#include "globals.h"

/**
 * @brief execute functions bound to \b WM_REDRAW event.
 *
 * @param app application descriptor,
 * @param win targeted window of \b NULL (if application is targeted)
 * @param rdraw an area to redraw
 *
 * This function is similar to mt_EvntRedraw(), except that you can
 * specify the area to be redrawn (by using mt_EvntRedraw(), the whole
 * work area is redrawn).
 *
 *  @sa mt_EvntRedraw()
 */

void mt_EvntRedrawGrect( APPvar *app, WINDOW *win, GRECT *rdraw) {
	W_GRAFPORT *graf = win ? win->graf : &app->graf;

	/* FIXME? rdraw is not considered const GRECT ??? */

	/* apply the win clipping rectangle */
	if ( rc_intersect( &graf->clip, rdraw)) {
		GRECT clip = graf->clip;
		GRECT rect;
		short buff[8];

		buff[0] = WM_PREREDRAW;
		grect2msg( rdraw, buff);
		mt_EvntExec( app, win, buff);

		while( !mt_wind_update(BEG_UPDATE, app->aes_global));
		buff[0] = WM_REDRAW;
		mt_graf_mouse( M_OFF, 0L, app->aes_global);
		mt_WindXGet( app, win, WF_FIRSTAREAXYWH, rdraw, &rect.g_x, &rect.g_y, &rect.g_w, &rect.g_h);
		while( rect.g_w && rect.g_h) {
			if ( rc_intersect( rdraw, &rect) ) {
				mt_ClipOn( app, graf, &rect);
				grect2msg(&rect,buff);
				mt_EvntExec( app, win, buff);
			}
			mt_WindGet( app, win, WF_NEXTXYWH, &rect.g_x, &rect.g_y, &rect.g_w, &rect.g_h);
		}
		mt_ClipOn( app, graf, &clip);
		mt_graf_mouse( M_ON, 0L, app->aes_global);
		mt_wind_update(END_UPDATE, app->aes_global);
	}
}

/**
 * @brief execute functions bound to \b WM_REDRAW event.
 *
 * @param app application descriptor,
 * @param win targeted window of \b NULL (if application is targeted)
 *
 * \b WM_REDRAW is a special event. When this event occurs, functions
 * bound to this message are executed several times: one time for each
 * rectangle of the AES rectangle list. Off course, EvntWindow()
 * handles that and it is transparent when we write a redraw
 * event function. The consequence is a redraw function cannot
 * be executed directly with EvntExec(). It is the goal of
 * EvntRedraw().
 *
 * @sa mt_EvntExec(), mt_snd_rdw().
 *
 * @todo mt_EvntRedraw() could be executed directly by mt_EvntExec().
 */

void mt_EvntRedraw( APPvar *app, WINDOW *win) {
	GRECT work;
	mt_WindGetGrect( app, win, WF_WORKXYWH, &work);
	mt_EvntRedrawGrect( app, win, &work);
}

void mt_CompEvntRedrawLGrect( APPvar *app, COMPONENT *c, LGRECT *rdraw) {
	LGRECT lwork;
	mt_CompGetLGrect(app, c, WF_WORKCLIPXYWH, &lwork);

	/* check with the given rdraw rectangle */
	if ( rc_lintersect( rdraw, &lwork ) ) {
		GRECT work;
		rc_lgrect2grect( &work, &lwork );
		mt_EvntRedrawGrect( app, mt_CompGetPtr( app, c, CF_WINDOW), &work);
	}
}

void mt_CompEvntRedraw( APPvar *app, COMPONENT *c) {
	LGRECT lwork;
	mt_CompGetLGrect(app, c, WF_WORKXYWH, &lwork);
	mt_CompEvntRedrawLGrect( app, c, &lwork);
}

