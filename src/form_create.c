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
 * $Source: /cvsroot/windom/windom/src/form_create.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.8 $
 */

#include <stdlib.h>
#include "globals.h"

/**
 * @brief Create and handle a formular in a window
 *
 * @param app application descriptor,
 * @param tree formular object tree ,
 * @param attrib widget attribute,
 * @param proc callback function handling user selection or \b NULL,
 * @param name window title,
 * @param gpos window position and size or \b NULL,
 * @param grow if TRUE, actives the visual graphic effects when window is opened and closed,
 * @param dup if TRUE, duplicates formular object tree in memory,
 * @return a window descriptor.
 *
 * mt_FormCreate() creates a window formular and displays it to the screen.
 * If a window formular with the same object tree already exists, the
 * function returns the window descriptor associated to this formular. 
 * If the window formular is closed, the window is re-open, if the
 * window is iconified, the window is uniconified. The window is
 * eventually topped. 
 *
 * The formular is centered at screen (by using mt_GrectCenter()) if a
 * \b NULL value is given to \e gpos parameter. A non 
 * \b NULL value of this parameter allows you to define the
 * position in the desktop od the window (\e gpos->g_x and \e gpos->g_y)
 * and the size of the window (\e gpos->g_w and \e gpos->g_h). If a
 * formular is bigger than the window, mt_FormCreate() creates sliders
 * wigdet to make scroll the window. 
 *
 * The \e name parameter specifies the window name (equivalent to a 
 * mt_WindSet( WF_NAME) call).
 *
 * It is possible to create several windows with a same formular by
 * setting the \e dup parameter of mt_FormAttach() to 1 : the object
 * tree is duplicated in memory (see mt_ObjcDup()) and this copy is used as
 * formular. A such tree is always unique. The raison of using duplicated
 * objects is that each formular has to have their own coordinates, flags
 * and state. When an object tree is duplicated, the bit \b WS_FORMDUP
 * of \e win->status of the window descriptor is set to 1.
 *
 * \b Event \b mesages \b handled \b by  \b default
 *
 * In addition to events handled by mt_WindCreate(), 
 * mt_FormCreate() defines the following events :
 *  - \b WM_REDRAW : handled by frm_drw(),
 *  - \b WM_DESTROYED : handled by frm_dstry(),
 *  - \b WM_MOVED : handled by frm_mvd(),
 *  - \b WM_FULLED : handled by frm_fld(),
 *  - \b WM_TOPPED : handled by frm_tpd(),
 *  - \b MU_BUTTON : handled by frm_click(),
 *  - \b MU_KEYBD : handled by frm_keyhd().
 *
 * \b Handling \b a \b window \b formular
 *
 * There are three ways in WinDom to handle user selection in
 * a windowed formular :
 *  - handle directly the event message #WM_FORM. This message is emited when
 *    user actes on a formular.
 *  - use the callback function specified by mt_FormCreate(). When a event message 
 *    \b WM_FORM is emit, this function is invoked by window kernel. The AES
 *    event buffer message needs to be read.
 *  - for each selectable object in the formular, attaches a callback function
 *    (or a variable). For that purpose, see mt_ObjcAttach().
 *
 * These methods are illustrated in examples provided in WinDom package.
 *
 * \sa mt_FormAttach(), mt_FormResize().
 */

WINDOW *mt_FormCreate( APPvar *app, OBJECT *tree, int attrib, func_evnt proc, char *name, 
		       GRECT *gpos, int grow, int dup) {
	register WINDOW *win = app->wglb.first;
	GRECT coord;
	INT16 x, y, w, h, dum;
	int outlined = 0;
	W_FORM *form;
	
	/* Case the window is already created */
	if( !dup)
	while( win) {
		if( win -> status & WS_FORM) {
			form = mt_DataSearch( app, win, WD_WFRM);
			if( form->root == tree) {
				if( win -> status & WS_ICONIFY) {
					mt_WindGet( app, win, WF_UNICONIFY, &x, &y, &w, &h);
					mt_snd_msg( app, win, WM_UNICONIFY, x, y, w, h);
				} else if( win -> status & WS_OPEN) 
					/* window opened */
					mt_wind_set( win -> handle, WF_TOP, 0, 0, 0, 0, app->aes_global);
				else {
					mt_wind_get( win->handle, WF_PREVXYWH, &x, &y, &w, &h, app->aes_global);
					mt_WindOpen( app, win, x, y, w, h);
				}
				return win;
			}
		}
		win = win -> next;
	}
	
	for( dum = 0; dum < 10; app->work_in[dum++] = 1);
	app->work_in[10] = 2;
	win = mt_WindCreate( app, attrib, app->x, app->y, app->w, app->h);
	if( win == NULL) return NULL;

	if( tree -> ob_state & OUTLINED)
		outlined = OUTLINED_WIDTH;

	if( !gpos) {
		coord.g_w = tree->ob_width + 2*outlined;
		coord.g_h = tree->ob_height + 2*outlined;
		mt_WindCalc( app, WC_BORDER, win, 0, 0, coord.g_w, coord.g_h, &x, &y, &coord.g_w, &coord.g_h);
		mt_GrectCenter( app, coord.g_w, coord.g_h, &coord.g_x, &coord.g_y);

		/* Si le formulaire est trop grand, on force les
		 * assenceurs 
		 */

		if( coord.g_w >= app->w || coord.g_h >= app->h) {
			mt_WindDelete( app, win);
			if( coord.g_w >= app->w)
				attrib |= HSLIDE|LFARROW|RTARROW|SIZER;
			if( coord.g_h >= app->h)
				attrib |= VSLIDE|DNARROW|UPARROW|SIZER;
			win = mt_WindCreate( app, attrib, app->x, app->y, app->w, app->h);
		}
	} else
		coord = *gpos;
	
	win -> name = name;
	if( grow) win -> status |= WS_GROW;
	mt_WindOpen( app, win, coord.g_x, coord.g_y, coord.g_w, coord.g_h);
	mt_FormAttach( app, win, tree, proc);
	form = mt_DataSearch( app, win, WD_WFRM);

	/* Form have to be duplicated */
	if( dup) {
		win -> status |= WS_FORMDUP;
		form->root = mt_ObjcDup( app, tree, win);
		if( form->root == NULL) {
			mt_WindDelete( app, win);
			return 0;
		}	
	}

	mt_WindSetStr( app, win, WF_NAME, name);
	mt_WindSet( app, win, WF_BEVENT, BEVENT_WORK, 0, 0, 0);
	mt_WindSlider( app, win, HSLIDER|VSLIDER);
	return( win);
}
