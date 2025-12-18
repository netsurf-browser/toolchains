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
 * $Source: /cvsroot/windom/windom/src/frm_dstry.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include "globals.h"

/** standard callback function to manage destroy event on form
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to forms for
 *  the \b WM_DESTROY event, registered by  mt_FormCreate().
 *
 *  This function will un-attach all the form data attached to
 *  the window when mt_FormCreate() was called.
 */
 
void __CDECL frm_dstry( WINDOW *win, short buff[8], APPvar *app) {
	mt_FormAttach( app, win, NULL, (func_evnt)0L);
	mt_graf_mouse( ARROW, 0L, app->aes_global);
}
