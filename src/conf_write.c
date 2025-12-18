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
 * $Source: /cvsroot/windom/windom/src/conf_write.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.7 $
 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"

#ifndef FA_SUBDIR
#define FA_SUBDIR 0x10
#endif

static char *find_word( char *buffer, char *find) {
	char *p, *q = buffer;
	do {
		p = strstr( q, find);
		if( p && (q == p || *(p-1) == '\n'))
			return p;
		q = p;
	} while( p);
	return 0L;
}

static int is_dir( APPvar *app, char *path, char *dir) {
	char buf[255];
	int res;

	strcpy( buf, path);
	add_slash( buf);
	strcat( buf, dir);
	res = Fattrib( buf, 0, 0);
	/* TODO: replace -33 by a constant from errno.h */
	if( res == -33) return 0;
	/* BUG? FIXME! Fattrib may return another negative value (error)
	 * in which case the function has to return now */
	if( res & FA_SUBDIR) {
		strcpy( app->priv->__confpath, buf);
		add_slash( app->priv->__confpath);
		return 1;
	} else
		return 0;
}

/*  Write to the config file
 */

int mt_vConfWrite( APPvar *app, char *name, char *fmt, va_list args) {
	FILE *fp;
	char *buf, save, *p, *q, *s;
	size_t size;
	char nameprg[20];
	int found = 1;
	
	get_appname( app, nameprg);	
	fp = fopen( app->priv->__confpath, "r");
	if( fp == NULL) {
		/* find the right name for the config file 
		 * and create the config file 
		 */
		mt_shel_envrn( &p, "HOME=", app->aes_global);
		if( p && is_dir( app, p, "Defaults"))
			;
		else if( p && is_dir( app, p, ""))
			;
		else if( is_dir( app, ".", ""))
			;
		if( vq_extfs(app->priv->__confpath))
			strcat( app->priv->__confpath, ".windomrc");
		else
			strcat( app->priv->__confpath, "windom.cnf");
		
		fp = fopen( app->priv->__confpath, "w");

		fprintf( fp, "# WinDom configuration file\n%s\n", nameprg);
		if( fmt) {
			fprintf( fp, "%s = ", name);
			vfprintf( fp, fmt, args);
			fprintf( fp, "\n");
		}
		fprintf( fp, "[end]\n");
		fclose( fp);
		return 0;
	}

	/* load the file,
	 * modify,
	 * and save.
	 */

	fseek( fp, 0L, 2);
	size = ftell( fp);
	fseek( fp, 0L, 0);
	buf = (char *)malloc(sizeof(char)*size);
	if( !buf) {
		fclose( fp);
		return 0;
	}
	size = fread( buf, sizeof(char), size, fp);
	buf[size] = '\0';

	fclose( fp);
	
	/* look for the right [label] */
	
	p = find_word( buf, nameprg);

	/* Try the default setting */
	if( p == NULL) p = find_word( buf, "[Default Settings]");
	
	if( p == NULL) {
		/* add the [label] corresponding to this application
		 * at the end of the config file
		 */
		fp = fopen( app->priv->__confpath, "a");
		fprintf( fp, "\n%s\n", nameprg);
		fprintf( fp, "%s = ", name);
		if( fmt) {
			vfprintf( fp, fmt, args);
			fprintf( fp, "\n");
		}
		fprintf( fp, "[end]\n");
		fclose( fp);
		return 0;
	} else {
		/* the [label] has been found,
		 * now, we'll have to look for the variable name
		 */

		/* to stay in the right area */
		s = find_word( p, "[end]");
		*s = '\0';
		q = find_word( p, name);
		*s = '[';
		
		if( q == NULL) {
			/* the variable doesn't exist: just add it */
			q = find_word( p, "[end]");
			found = 0;
		}
		p = q;
		
		/* write the beginning */
		save = *p;
		*p = '\0';
		fp = fopen( app->priv->__confpath, "w");
		fwrite( buf, sizeof( char), strlen( buf), fp);
		*p = save;

		if( fmt) {
			/* write the variable */
			fprintf( fp, "%s = ", name);
			vfprintf( fp, fmt, args);
			fprintf( fp, "\n");
		}

		/* write the end */	
		if( found) p = strchr( p, '\n')+1;
		size = strlen( p)+1;
		fwrite( p, sizeof( char), size, fp);
		fclose( fp);
	}
	return 0;
}


/** write the windom config file
 *
 *  @param app application descriptior
 *  @param name name of variable to write
 *  @param fmt format of value (similar to printf)
 *  @param ... variables (%%d or %%s etc... of \a fmt)
 *
 *  @retval 0 success
 *  @retval <0 error
 *
 *  This function will add a "variable = value" string in the
 *  configuration file. If the config file is not found, a new
 *  one will be created.
 *
 *  @sa mt_ConfGetLine() mt_ConfInquire() mt_ConfRead()
 *
 */

int mt_ConfWrite( APPvar *app, char *name, char *fmt, ...) {
	va_list args;
	int res;
	
	va_start( args, fmt);
	res = mt_vConfWrite( app, name, fmt, args);
	va_end( args);	

	return res;
}
