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
 * $Source: /cvsroot/windom/windom/src/snd_rdw.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.6 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/**  send a WM_REDRAW message for the work area of this window
 *  
 *  @param app application descriptor
 *  @param win window to redraw
 *
 *  This will send a WM_REDRAW for the whole work area of \a win.
 */

void mt_snd_rdw( APPvar *app, WINDOW *win) {
	INT16 x, y, w, h;
	
	mt_WindGet( app, win, WF_WORKXYWH, &x, &y, &w, &h);
	mt_ApplWrite( app, mt_AppId(app), WM_REDRAW, w_get_hndl(app, win), x, y, w, h);
}

