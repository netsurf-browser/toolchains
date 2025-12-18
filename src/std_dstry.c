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
 * $Source: /cvsroot/windom/windom/src/std_dstry.c,v $
 * Module : standard event window callback
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.6 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/** standard callback function to manage window-destroyed event
 *
 *  @param win window descriptor
 *  @param buff AES message
 *  @param app application descriptor
 *
 *  This is the standard callback function attached to windows for
 *  the \b WM_DESTROYED event, registered by  mt_WindCreate()
 *  
 *  This function will close and delete the window, and then terminate
 *  the application (by sending a \b AP_TERM message) if there is no more
 *  opened window left. 
 */

void __CDECL std_dstry( WINDOW *win, short buff[8], APPvar *app) {
	mt_WindClose( app, win);
	mt_WindDelete( app, win);
	if( !app->wglb.first && !app->menu)
		mt_snd_msg( app, NULL, AP_TERM,0,0,0,0);
}

