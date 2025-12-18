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
 * $Source: /cvsroot/windom/windom/src/debug_windom.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */


#include <stdarg.h>	
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "globals.h"

/** print formated string to debug output
 *
 *  @param app application descriptor
 *  @param format formatted string (similar to printf)
 *  @param ... paramters of format (similar to printf)
 *
 *  This function writes the string to the windom debug output.
 *  The windom debug output is defined by the \a windom.debug variable.
 *  It may be :
 *  - a standard AES form alert (if \a windom.debug is "alert")
 *  - a \b AP_DEBUG AES message sent to the "DEBUG" application 
 *    (if \a windom.debug is "debug")
 *  - a file if \a windom.debug is "log". The second value of \a windom.debug
 *    is then the path of the log file to create.
 */

void mt_DebugWindom( APPvar *app, const char *format, ...) {
	char path[255];
	/* BUG! FIXME: change the following: static variables are forbidden ! */
	static char debug[255];
	char type[120], p;
	va_list arglist;
	
	p = mt_ConfInquire( app, "windom.debug", "%s,%s", type, path);
	if( p>0) {
		/* format */
		va_start( arglist, format);
		vsprintf( debug, format, arglist);
		va_end( arglist);

		if( !stricmp( type, "alert")) {
			char alert[255];
			sprintf( alert, "[1][%s][Okay]", debug);
			mt_form_alert( 1, alert, app->aes_global);	
		} else
		if( !stricmp( type, "debug") && p == 2) {
			int id, ans = 0;
			
			if( app->aes_global[1] != 1)
				mt_ShelWrite( app, path, "", NULL, 1, 0);
			id = mt_appl_find( "DEBUG   ", app->aes_global);
			if( id > 0) {
				short buff[8];
				buff[0] = AP_DEBUG;
				buff[1] = mt_AppId(app);
				buff[2] = 0;
				buff[3] = app->wglb.front?app->wglb.front->handle:0;
				*(char **)&buff[4] = debug;
				mt_appl_write( id, 16, buff, app->aes_global);
				/* wait for the answer */
				do {
					mt_evnt_mesag( buff, app->aes_global);
					if( buff[0] == AP_TERM) {
						ans = 1;
						mt_ApplWrite( app, mt_AppId(app), AP_TERM, 0, 0, 0, 0, 0);
					}
					if( buff[0] == AP_DEBUG)
						ans = 1;
				} while( !ans);
			}
		} else
		if( !stricmp( type, "log") && p == 2) {
			FILE *fp;
			/* BUG! FIXME: change the following: static variables are forbidden ! */
			static int first = 1;
			
			fp = fopen( path, first?"w":"a");
			first = 0;
			fputs( debug, fp);
			fputs( "\n", fp);
			fclose( fp);
		}
	}
}

/* EOF */
