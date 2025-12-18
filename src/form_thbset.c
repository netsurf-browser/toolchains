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
 * $Source: /cvsroot/windom/windom/src/form_thbset.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include "globals.h"

/**
 * @brief Retrieve active thumb.
 *
 * @param app application descriptor,
 * @param win window formular hosting the thumb,
 * @param but index button linked to thumb to active.
 *
 * This function changes the active thumb (without a user manipulation).
 *
 * @bug FIXME - Does not work correctly.
 * @sa mt_FormThumb(), mt_FormThbSet().
 */

void mt_FormThbSet( APPvar *app, WINDOW *win, int but) {
	short buff[8];
	buff[0] = WM_FORM;
	buff[3] = win->handle;
	buff[4] = but;    /* selected object */
	buff[5] = 0;      /* kstate */
	mt_EvntExec( app, win, buff);
}
