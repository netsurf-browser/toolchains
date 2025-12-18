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
 * $Source: /cvsroot/windom/windom/src/mouse_work.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.7 $
 */

#include "globals.h"
#include "mouse.h"
#include "mouse.rh"
#include "mouse.rsh"

/** display an animated buzy mouse
 *
 *  @param app application descritor
 *
 *  windom has some built-in mouse sprites to show the user that
 *  the application is buzy. 
 *
 *  This function will transform the mouse cursor to one of these
 *  built-in sprite. The next call will display the following sprite,
 *  and so on... If you call mt_MouseWork() every 200ms, then you'll
 *  see an animation on the mouse cursor, with a refresh of one frame
 *  every 200ms.
 *
 *  This may be used when your application is blocked doing a "long" work
 *  without interaction with the user while the work is not done. That way,
 *  the user will see that the mouse cursor is animated and so, the application
 *  is not dead.
 *
 */

void mt_MouseWork( APPvar *app) {
	app->priv->mouse = (app->priv->mouse+1)%8 ;
	mt_MouseSprite( app, rs_trindex[MOUSE], app->priv->mouse+1);
}
