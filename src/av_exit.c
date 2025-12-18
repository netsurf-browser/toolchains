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
 * $Source: /cvsroot/windom/windom/src/av_exit.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "av.h"

/** 
 * @brief close an AV session opened with AvInit()
 *  
 * @param app application descriptor.
 *
 * Before terminate a client, the AV session must be absolutely 
 * closed with this function otherwise, the AV server will be
 * confused.
 *
 * \sa mt_AvInit()
 */
 
void mt_AvExit( APPvar *app) {
	if( app->priv->__av_status[0] & V_EXIT)
		mt_ApplWrite( app, app->avid, AV_EXIT, mt_AppId(app), 0, 0, 0, 0);
	free( app->priv->__av_name);
	app->avid = -1;
}
