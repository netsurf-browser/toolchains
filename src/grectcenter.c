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
 * $Source: /cvsroot/windom/windom/src/grectcenter.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */


#include <stdlib.h>
#include "globals.h"

/**
 * @brief Centering an area on screen.
 *
 * @param app application descriptor,
 * @param w,h size of area to center,
 * @param x,y at return, contain coordinate of the centered area.
 *
 * mt_GrectCenter() computes the coordinates of an area in order to centering
 * it on the screen (more precisely, on destop workspace). This function is 
 * used by mt_WindOpen(), window formular
 * and classic formular functions to centered the window or the formular
 * at screen. mt_GrectCenter() replaces the AES function form_center().
 *
 * User can change the way mt_GrectCenter() centers the areas by editing
 * the variable windom.window.center in the WinDom configuration file.
 * Windows and formulars can be centered at screen or in a corner, 
 * centered on the mouse sprite, one can leave the OS decide  ...
 *
 * @sa mt_WindOpen(), mt_FormCreate(), mt_WindFormBegin(), mt_FormBegin().
 */
 
void mt_GrectCenter( APPvar *app, int w, int h, INT16 *x, INT16 *y) {
	INT16 dum;
	
	switch( CONF(app)->wcenter) {
	case CENTER:
		if( app->w > w)
			*x = (app->w - w)/2;
		else
			*x = 0;
		if( app->h > h)
			*y = (app->h - h)/2;
		else
			*y = 0;
		break;
	case WMOUSE:
		/* center on the mouse pointer */
		mt_graf_mkstate( x, y, &dum, &dum, app->aes_global);
		*x -= w/2;
		*y -= h/2;
		break;
	case UP_LEFT:
		*x = app->x;
		*y = app->y;
		break;
	case UP_RIGHT:
		*x = app->w - w;
		*y = app->y;
		break;
	case DN_LEFT:
		*x = app->x;
		*y = app->h - h;
		break;
	case DN_RIGHT:
		*x = app->w - w;
		*y = app->h - h;
		break;
	/* use form_center */
	case FCENTER:
		{
			OBJECT form;
			form.ob_next = -1;
			form.ob_head = -1;
			form.ob_tail = -1;
			form.ob_type = G_BOX;
			form.ob_flags = LASTOB;
			form.ob_state = NORMAL;
			form.ob_spec.index =  0x00FF1100L;
			form.ob_x = 0;
			form.ob_y = 0;
			form.ob_width = w;
			form.ob_height = h;
			mt_form_center( &form, x, y, &dum, &dum, app->aes_global);
		}
		break;
	}
	/* stay in the screen area */
	if( *x + w > app->x + app->w)
		*x =  app->x + app->w - w;
	if( *y + h > app->y + app->h)
		*y =  app->y + app->h - h;
	*x = MAX( *x, app->x);
	*y = MAX( *y, app->y);

}

