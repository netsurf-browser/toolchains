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
 * $Source: /cvsroot/windom/windom/src/conf_inquire.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.8 $
 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"

/*  read a line in the buffer pointed by ptr
 */
static char *sgets( char *line, long size, char *ptr) {
	char *e=ptr;
	
	line[size-1] = '\0';			/* cas ou l'on d‚passe */
	while( *ptr != '\0' &&
		   *ptr != '\n' &&
		   ptr-e < size-1) {
		*line++ = *ptr++;
	}
	if( *ptr =='\n') {
		ptr ++;
		*line = '\0';
	}
	return ptr;
}

/* inquire a variable stored in __conf_buffer
 */
static int conf_inquire( char *buffer, char *keyword, char *fmt, va_list args) {
	char buf[LINE_SIZE], cmd[30], *p, *q, *s;
	int res = -1;
	void *ptr;
	unsigned int bit;
	char *pos = buffer;

	if( pos == NULL)
		return -33;    /* TODO: replace -33 by a constant from errno.h */
	while( *pos) {
		/* read a line */
		pos = sgets( buf, LINE_SIZE, pos);
		if(*buf == '#' || *buf=='\n')
			continue;
		sscanf( buf, "%s", cmd);
		if( !stricmp( cmd, keyword)) {
			res = 0;
			p = strchr( buf, '=');
			q = fmt;
			while( *q) {
				q = strchr( q, '%');
				if( p)
					p++;
				else
					break;
				res ++;
				if( q)
					switch( *++q) {
					case 'd':
					case 'x':
						ptr = (void*)va_arg( args, int *);
						sscanf( p, (*q=='d')?"%d":"%x", ptr);
						break;						
					case 'c':
						ptr = (void*)va_arg( args, char *);
						p = strchr( p , '\'');
						if( p) 
							sscanf( p+1, "%c", (char*)ptr);
						else	res --;
						break;
					case 's':
					case 'S':
						ptr = (void*)va_arg( args, char *);
						s = strchr( p, '"');
						if( s) {
							strcpy( ptr, s+1);
							*strchr( ptr, '"') = '\0';
							p = strchr( s+1, '"');
						} else /* res--; */ {
							sscanf( p, "%s", (char*)ptr);
							s = strchr( ptr, ',');
							if( s) *s='\0';
						}
						break;
					case 'B':
					case 'b':
						ptr = (void *)va_arg( args, unsigned int *);
						if( *q == 'B') bit = va_arg( args, unsigned int);
						else		   bit = 0x1;
						sscanf( p, "%s", cmd);
						s = strchr( cmd, ',');
						if( s) *s='\0';
						if( !stricmp( cmd, "true") ||
							!stricmp( cmd, "on")   ||
							!strcmp ( cmd, "1"))
							SET_BIT( *(unsigned int *)ptr, bit, TRUE);
						else
						if( !stricmp( cmd, "false") ||
							!stricmp( cmd, "off")   ||
							!strcmp ( cmd, "0"))
							SET_BIT( *(unsigned int *)ptr, bit, FALSE);
						else
							res --;
						break;
					default:
						res --;
					}
				p = strchr( p, ',');
			}
			break;
		} else 
			if( !stricmp( cmd, "[end]"))
				break;	/* end of the loop */
	}
	return res;
}

int mt_vConfInquire( APPvar *app, char *keyword, char *fmt, va_list args) {
	int res;
	va_list args_dup;
	
	/* duplicate args (should use __va_copy(dest,src) with gcc 3 ?) 
	 * what's the good way to check the availability of va_copy() ?
	 */
#ifdef va_copy
	va_copy( args_dup, args);
#else
	args_dup = args;
#endif
	
	res = conf_inquire( app->priv->__confapp_buffer, keyword, fmt, args);
	if( res < 0)
		res = conf_inquire( app->priv->__confdef_buffer, keyword, fmt, args_dup);
	
	return res;
}

/** read an user variable in the configuration file
 *
 *  @param app application descriptor
 *  @param keyword name of variable to read
 *  @param fmt format of value (see below)
 *  @param ... address(es) of variable(s) to fill-in.
 *
 *  @retval >=0 is the number of values read
 *  @retval -1 variable not found
 *  @retval -33 configuration file not found
 *
 *	mt_ConfInquire() read a variable from the configuration file in the
 *	application area is defined or in the Default Settings area if
 *	defined. If the variable is not found or if no configuration area
 *	addressing the application is not found, the function returns -1.
 *
 *  The syntax of variable definition in the configuration file have
 *	the following structure:
 *
 *  @code keyword = value[, value[, ...]] @endcode
 *
 *  The '[]' notation means an optional argument.
 *	The \c format parameter have a similar syntax than printf().
 *	Possible variable are :
 *  - %%d : 16-bit integer,
 *  - %%f : single real (32-bit),
 *  - %%c : a character delimited by a quote ((!U)'(!u)),
 *  - %%b : a boolean variable (true, on, 1, false, off, 0),
 *  - %%B : the boolean value in set in a specific bit of the
 *	       variable (see EXAMPLES),
 *  - %%s : a string. The string can be delimited by a double
 *		   quote charactere ((!U)"(!u)) if the string contains
 *  	   space characters.
 *  - %%S : equivalent to %s, it is an obsolet mode but kept for
 *	       higher compatibility.
 *
 *
 *  Example of configuration file:
 *  @code
appli.font.name = "Helvetica Bold"
appli.system.path = C:\APPLI\system\
appli.window.size = 400,300
appli.window.sizer = 'S'
appli.parameters.save = TRUE
appli.parameters.bubble = TRUE @endcode
 *  and here is the corresponding code (in the application) :
 *  @code
void InitAppl( void)
{
    char FontName[33], path[128];
    int width, height;
    char car;
#define PARAM_SAVE	0x1
#define PARAM_BUBBLE 0x2
    int param;

    if( ConfInquire( "appli.font.name", "%s", FontName) != 1)
        strcpy( FontName, "Times");
    if( ConfInquire( "appli.system.path", "%s", path) != 1)
        strcpy( path, "");
    if( ConfInquire( "appli.window.size", "%d,%d", &width, &height) != 2)
        width = height = 200;
    if( ConfInquire( "appli.window.sizer", "%s", &car) != 1)
        car = 'S';
    ConfInquire( "appli.parameters.save", "%B", &param,   PARAM_SAVE);
    ConfInquire( "appli.parameters.bubble", "%B", &param, PARAM_BUBBLE);       
} @endcode
 *
 *  @sa mt_ConfRead(), mt_ConfWrite(), mt_ConfGetLine()
 */

int mt_ConfInquire( APPvar *app, char *keyword, char *fmt, ...) {
	va_list args;
	int res;
	
	va_start( args, fmt);
	res = mt_vConfInquire( app, keyword, fmt, args);
	va_end( args);	

	return res;
}

