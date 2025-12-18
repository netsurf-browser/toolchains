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
 * $Source: /cvsroot/windom/windom/src/appl_name.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "globals.h"

/**
 * @brief Identify the name of a GEM process.
 *
 * @param app application descriptor,
 * @param name name of the process (a 8-byte buffer, AES formatted)
 * @param id AES handle of the process,
 * @return 1 if process is found, 0 otherwise.
 *
 * This function gets the name of a GEM process using its
 * AES process handle. This function uses the appl_search()
 * function. If this function is not available, ApplName()
 * returns always 0. In this case the \b AES4_APPSEARCH
 * bit of the \e app->aes4 is 0.
 *
 * @sa appl_search()
 * @sa appl_find()
 */

int mt_ApplName( APPvar *app, char *name, int id) {
	INT16 i,m=0x0F;

	if( app->aes4 & AES4_APPSEARCH) {
		if( mt_appl_search(APP_FIRST, name, &m, &i, app->aes_global) == 0) {
			*name = '\0';
			return 0;
		}
		if( i == id)
			return 1;
		while( mt_appl_search(APP_NEXT, name, &m, &i, app->aes_global)) {
			if( i == id )
				return 1;
		}
	}
	*name = '\0';
	return 0;
}

