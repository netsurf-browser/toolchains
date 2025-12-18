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
 * $Source: /cvsroot/windom/windom/src/av_server.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include <string.h>
#include "globals.h"

/**
 * @brief returns the name of the AV server 
 * 
 * @param app application descriptor,
 * @return name of the AV server (in AES format).
 *
 * This function returns the name of the AV server if the
 * AV session was succesfully opened with the mt_AvInit()
 * function. The AV server GEM identifier is given by
 * the variable \e app->avid.
 *
 * \sa mt_AvInit(), mt_AvStatus()
 */

char *mt_AvServer( APPvar *app) {
	return app->priv->__av_server;
}
