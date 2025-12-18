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
 * $Source: /cvsroot/windom/windom/src/appl_write.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/01/04 06:43:52 $
 *   $Revision: 1.7 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "globals.h"

/* A more pratical call to appl_write() */

/**
 * @brief Send a message to a GEM process.
 *
 * @param app application descriptor,
 * @param to AES id of the targeted process,
 * @param msg message number to send,
 * @param b3, b4, b5, b6, b7 5 parameters filling words
 *        3 to 7 of the AES message pipe.
 * @return the value returned by appl_write().
 *
 * This function is just a more practical call to the appl_write()
 * AES function. It replaces the obsolet mt_snd_msg() WinDom
 * function. Some AES message needs long args representing a
 * pointer value. In this case, use the macro function ADR() to
 * transform an one long argument into two short arguments.
 *
 * Example:
 * @code
void ex_vastart( APPvar *app) {
  ApplWrite( id_target, VA_START, ADR(strcpy(app->pipe,"C:\\NEWDESK.INF")), 0, 0);
} @endcode
 *
 * @note
 * the implementation with variable arguments has been removed.
 *
 */

int mt_ApplWrite( APPvar *app, int to, int msg, int b3, int b4, int b5, int b6, int b7) {
	INT16 buf[8];

	buf[0] = msg;
	buf[1] = mt_AppId(app);
	buf[2] = 0;
	buf[3] = b3;
	buf[4] = b4;
	buf[5] = b5;
	buf[6] = b6;
	buf[7] = b7;
	return mt_appl_write( to, 16, buf, app->aes_global);
}
