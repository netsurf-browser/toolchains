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
 * $Source: /cvsroot/windom/windom/src/conf_windom.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/04/18 17:50:24 $
 *   $Revision: 1.8 $
 */

#include <stdlib.h>
#include <string.h>
#include "globals.h"

/** load winconf (windom configurator)
 *
 *  @param app application descriptor
 *  @return -33 if winconf.app is not found, or the result of 
 *          the loading done by mt_ShelWrite()
 *	
 */

int mt_ConfWindom( APPvar *app) {
	char *p=NULL;
	char prgname[255], prgcmd[255];
	int locate = FALSE;


	/* Nom du programme */
	mt_shel_read( prgname, prgcmd, app->aes_global);

	/* Localisation de WinConf */
	/* warning: mt_shel_envrn() always returns 1 -- so "p" has to be checked */
	if( mt_shel_envrn( &p, "WINCONF=", app->aes_global) && p)
		locate = TRUE;
	else {
		strcpy( prgcmd, "winconf.app");
		locate = mt_shel_find( prgcmd, app->aes_global);
		p = prgcmd;
	}

	if( locate)
		return mt_ShelWrite( app, p, prgname, NULL, TRUE, FALSE);
	else
		return -33;  /* TODO: replace -33 by a constant from errno.h */

	
/* Future Extension :

#define WC_SECTION	0x5743	

	ApplWrite( id, WC_SECTION, section);
	return id;
*/
	
}

