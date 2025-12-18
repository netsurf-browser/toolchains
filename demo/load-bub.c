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
 * $Source: /cvsroot/windom/windom/demo/load-bub.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/02/14 20:57:15 $
 *   $Revision: 1.3 $
 */

#include <windom.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "global.h"

#define MAX_SIZE	255		/* Longueur maximale d'un texte de bulle d'aide */

static char *bubble_adr=NULL;

int LoadBubble( char *filename) {
	FILE *fp;
	char buf[MAX_SIZE],*p,*q;
	size_t fsize;
	int pere, obj;
	OBJECT *tree;
	
	strcpy( buf, filename);
	shel_find( buf);
	
	fp = fopen( buf, "r");
	if( fp == NULL) {
		return -33;
	}
	fseek( fp, 0, 2);
	fsize = ftell( fp);
	rewind( fp);
	
	/* reserver de la m‚moire */
	bubble_adr = (char*)malloc(sizeof(char)*fsize);
	if( bubble_adr == NULL) {
		form_alert( 1, "[1][Plus de m‚moire disponible|pour les bulles d'aides!][Ok]");
		fclose( fp);
	}
	q = bubble_adr;
	while( !feof(fp)) {
		fgets( buf, MAX_SIZE, fp);
		if (*buf == '\0') break;
		if( buf[strlen(buf)-1] == '\n')
			buf[strlen(buf)-1] = '\0';
		sscanf( buf, "%d %d", &pere, &obj);
		tree = get_tree( pere);

		if( (p = strchr( buf, '@')) != NULL) {	/* Alias */
			int pere_a, obj_a;
			OBJECT *tree_a;
			
			sscanf( p, "@ %d %d", &pere_a, &obj_a);
			tree_a = get_tree( pere_a);
			BubbleFind( tree_a, obj_a, &p);
			BubbleAttach( tree, obj, p);
		} else {
			p = strchr( buf, ' ')+1;
			p = strchr( p, ' ')+1;
			strcpy( q, p);
			BubbleAttach( tree, obj, q);
			q += strlen( p)+1;
		}
	} 
	fclose( fp);
	return 0;
}

void UnLoadBubble( void) {
	if( bubble_adr) {
		BubbleFree();
		free( bubble_adr);
	}
}
