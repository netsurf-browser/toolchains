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
 * $Source: /cvsroot/windom/windom/examples/tutorial/text.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:39:25 $
 *   $Revision: 1.2 $
 */

/**
 * \file text.c
 * \brief Simple functions to load a text file into memory.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "text.h"

/**
 * This function loads into memory a text file.
 */

TEXT *load_text( char *filename) {
	FILE* fp;
	int dum;
	long size, nbread;
	char *p;
	TEXT *text;

	text = (TEXT *) malloc(sizeof(TEXT));
	assert( text);
	strcpy( text->filename, filename);

	fp = fopen( filename, "r");
	if( !fp) return NULL;

	/* read the file and place it into text->buffer */
	fseek( fp, 0, 2);
	size = ftell( fp);
	fseek( fp, 0, 0);
	text->buffer = (char*)malloc(sizeof(char)*(size+1));
	assert( text->buffer);
	nbread = fread( text->buffer, sizeof(char), size, fp);
	fclose( fp);
	text->buffer[nbread] = '\0';

	/* counting lines */

	for( p=text->buffer,text->maxline=1; *p; p++)
		if( *p == '\n') text->maxline ++;

	/* set pointer to each line */
	text->line = (char**)malloc(sizeof(char*)*text->maxline);
	assert( text->line);
	text->line[0]=text->buffer;
	
	for( p=text->buffer, dum=0; *p; p++)
		if( *p == '\n') {
			text->line[++dum] = p+1;
			*p = '\0';
		}
	return text;
}

/**
 * This function release from memory a text loaded by load_text().
 */
void free_text( TEXT *text) {
	free( text->line);
	free( text->buffer);
	free( text);
}

/* EOF */
