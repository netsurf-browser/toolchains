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
 * $Source: /cvsroot/windom/windom/src/snd_msg.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.5 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"


/**  send a message to itself.
 *
 *  @param app application descriptor
 *  @param win target of the message (used for element buff[1] of the message)
 *  @param msg id buff[0] of the message
 *  @param par1,par2,par3,par4 are buff[4..7] of the message.
 *
 */

void mt_snd_msg( APPvar *app, WINDOW *win, int msg, int par1, int par2, int par3, int par4) {
	mt_ApplWrite( app, mt_AppId(app), msg, win?win->handle:0, par1, par2, par3, par4);
}

