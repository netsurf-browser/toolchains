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
 * $Source: /cvsroot/windom/windom/src/rsrc_load.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stddef.h>
#include <stdlib.h>
#ifdef __PUREC__
#include <tos.h>
#endif
#include <string.h>
#include "globals.h"

#ifndef FO_READ
#define FO_READ 0
#endif

/**
 * @brief  Load a resource file in memory.
 *
 * @param app application descriptor,
 * @param name GEM resource file name to load.
 * @return 0 if an error occured, 1 otherwise.
 *
 * mt_RsrcLoad() replaces the AES mt_rsrc_load() function. The 
 * resource filename can be in a TOS format (e.g. with 
 * backslash characters) or MiNT format (e.g. with slash
 * characters). mt_RsrcLoad() uses conv_path() to make the
 * convertion. mt_RsrcLoad() fills in the \e app->ntree
 * variable with the number of tree contained in the
 * resource file.
 *
 * Trick : mt_RsrcLoad() use \b PATH environment variable to
 *         locate resource file.
 *
 * @sa mt_RsrcFree(), mt_RsrcXload()
 */

int mt_RsrcLoad( APPvar *app, const char *name) {
	int res;
	RSHDR RSCheader;
	int fd;
	char path[255];
	
	strcpy( path, name);
	res = mt_rsrc_load( path, app->aes_global);
	if( !res) {
		/* try other format */
		conv_path(path);
		res = mt_rsrc_load( path, app->aes_global);
	}
	if( !res) return 0;
	fd = (int)Fopen( path, FO_READ);
	if( fd < 0 && mt_shel_find( path, app->aes_global))
		fd = (int)Fopen( path, FO_READ);
	if( fd < 0) {
		/* should not occured */
		mt_rsrc_free( app->aes_global);
		return 0;
	}
	Fread( fd, sizeof( RSHDR), &RSCheader);
	app->ntree = (signed int)RSCheader .  rsh_ntree;
	Fclose( fd);
	return 1 ;
}
