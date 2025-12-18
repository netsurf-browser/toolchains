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
 * $Source: /cvsroot/windom/windom/src/load_fontid.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "globals.h"

int load_fontid( APPvar *app) {
	FILE *fp;
	char *p, *q;
	int i;
	char path[255];

	fp = fopen( "fontid", "r");
	if( !fp) {
		mt_shel_envrn( &p, "ETCDIR=", app->aes_global);
		if( p) {
			strcpy( path, p);
			strcat( path, "\\fontid");
			fp = fopen( path, "r");
		}
	}
	if( !fp) {
		mt_shel_envrn( &p, "HOME=", app->aes_global);
		if( p) {
			strcpy( path, p);
			strcat( path, "Defaults\\fontid");
			fp = fopen( path, "r");
		}
	}
	if( !fp) {
		mt_shel_envrn( &p, "FONTDIR=", app->aes_global);
		if( p) {
			strcpy( path, p);
			strcat( path, "\\fontid");
			fp = fopen( path, "r");
		}
	}
	if( !fp)
		fp = fopen( "C:\\gemsys\\fontid", "r");

	if( fp) {
		char buf[LINE_SIZE];
		int count = 0;
		
		/* count the entries */
		while( fgets( buf, LINE_SIZE, fp)) {
			if( *buf == '#' ||
				*buf == '\n')
				continue;
			count ++;
		}
		rewind( fp);
		app->priv->maxfontid = count;
		app->priv->listfontid = (struct fontid *)malloc( sizeof(struct fontid)*count);
		if( !app->priv->listfontid) {
			fclose( fp);
			return -69;	/* memory error */  /* TODO : replace by constant name */
		}

		/* parse the file */
		while( fgets( buf, LINE_SIZE, fp)) {
			if( *buf == '#' ||
				*buf == '\n')
				continue;

			/* line format : num "Name" id flags */

			sscanf( buf, "%d", &i);
			p = strchr( buf, '"');
			if( p && i < app->priv->maxfontid) {
				q = strchr( p+1, '"');
				if( q) {
					*q = '\0';
					strcpy( app->priv->listfontid[i].name, p+1);
					sscanf( q+1, "%d %x", &app->priv->listfontid[i].id, &app->priv->listfontid[i].flags);
				}
			}
				/* ELSE : Erreur de format */
		}
		
		return app->priv->maxfontid - 1;
	} else {
		/* a system font is always available */
		app->priv->listfontid = (struct fontid *)malloc( sizeof(struct fontid));
		if( !app->priv->listfontid) {
			app->priv->maxfontid = 2;
			strcpy( app->priv->listfontid[0].name, "system font");
			app->priv->listfontid[0].id = 1;
			app->priv->listfontid[0].flags = FNT_MONO;
		}
		
		return -33;	/* file not found */ /* TODO : replace by constant name */
	}
}
