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
 * $Source: /cvsroot/windom/windom/src/clip_on.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include "globals.h"

/** set clipping area for drawing
 *
 *  @param app application descriptor
 *  @param graf grafport which contain the VDI handle
 *  @param r the area to clip
 *
 *  This function will set a clipping for the \a area
 *  for the VDI workstation of \a graf.
 *
 *  This function is useless for most applications.
 *  Most of the time, application draw stuff to the screen
 *  only after a \b WM_REDRAW message, and in that case
 *  windom has already set the clipping area before inoking
 *  the callback function attached to the WM_REDRAW message.
 */
  
void mt_ClipOn( APPvar *app, W_GRAFPORT *graf, GRECT *r) {
	PXY pxy[2];

	pxy[0].p_x = MAX( app->x, r->g_x);
	pxy[0].p_y = MAX( app->y, r->g_y);
	pxy[1].p_x = MIN( app->x + app->w - 1, r->g_x + r->g_w - 1);
	pxy[1].p_y = MIN( app->y + app->h - 1, r->g_y + r->g_h - 1);

	rc_set( &graf->clip, pxy[0].p_x, pxy[0].p_y,
		pxy[1].p_x - pxy[0].p_x + 1, pxy[1].p_y - pxy[0].p_y + 1);
	vs_clip_pxy( graf->handle, pxy);
}
