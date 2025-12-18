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
 * $Source: /cvsroot/windom/windom/src/shel_write.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.7 $
 */


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "globals.h"
#include "av.h"

/**
 * @brief Launch GEM or TOS applications.
 *
 * @param app application descriptor,
 * @param prg full or relative path of program to execute,
 * @param cmd command line or \b NULL,
 * @param env environ string or \b NULL,
 * @param av if \b TRUE, try to send a \b VA_START if possible,
 * @param single if \b TRUE, execute in single mode,
 * @return the id process of the application launched or a 
 *         negative code error (-1 : execution failed, -2: AV_STARTED
 *         message not received (only if \e av parameter is set to TRUE).
 * @pre pathname of program to execute must be in TOS format (with \ and x: for disc unit).
 *
 * mt_ShelWrite() launches application in a easy way. Applications can be GEM
 * application or TOS application. mt_ShelWrite() uses the file name extension
 * to identify the type of application (TOS programs have TOS or TTP suffix
 * and GEM programs have APP, PRG or GTP suffix).
 *
 * In a multitask environnment, applications are launched in parallel. With
 * the parameter \e single set to \b TRUE, applications are lauched in single mode. If
 * the parameter \e av is set to \b TRUE, a \b VA_START message is
 * sent to the GEM application if it is running. Otherwise, a \b FALSE value has
 * the consequent to create multiple application. Note : if an \b VA_START
 * is sent, one waits for \b AV_STARTED message, as stated by the AV-protocol and
 * the function returns -2 if this message is not received.
 *
 * The parameter \e cmd describe the command line given to the
 * application. The format is different of Pexec() or mt_shel_write() (ie the
 * first character does not contain the lenght of the string).
 *
 * @bug The ARGV protocol is not handled.
 * @bug Desktop accessories can not be launched.
 */
 
int mt_ShelWrite( APPvar *app, char *prg, char *cmd, void *env, int av, int single) {
	char file[FILE_SIZE];
	char *p;
	int res=-1, mode;
	char path[FILE_SIZE], path2[FILE_SIZE];
	int drv;

	if( av) {
		char *q;
		INT16 id_app, msg[8];
		char aesname[9];

		strcpy( app->pipe, cmd);
		/* extract program filename and convert it to AES format */
		p = strrchr( prg, '\\');
		if( !p) p = strrchr( prg, '/'); /* support MiNT format */
 		if( !p) p = prg;  else p++;
		for( q=aesname; q-aesname<8; q++) 
			*q = ( *p == '.' || *p == '\0' ) ? ' ' : toupper( *p++);
		aesname[8] = '\0';
		id_app = mt_appl_find( aesname, app->aes_global);
		if( id_app >= 0) {
			msg[0] = VA_START;
			msg[1] = mt_AppId(app);
			msg[2] = 0;
			*(char **)&msg[3] = app->pipe;
		    msg[5] = 0;
    	    msg[6] = 0;
    	    msg[7] = 0;
    	   	mt_appl_write(id_app, 16, msg, app->aes_global);
			return mt_AvWaitfor( app, AV_STARTED, msg, 500)?id_app:-2;
		}
	} 
	strcpy( file+1, cmd);
	*file = strlen(cmd);

	/* Save current directory */
	drv = Dgetdrv();
	Dgetpath( path, 0);
	
	/* Set the program path the new current directory */
	if( prg[1] == ':')
		Dsetdrv( toupper(prg[0])-'A');
	p = strchr( prg, '\\');
	strcpy( path2, p?p:prg);
	p = strrchr( path2, '\\');
	if(p) *p = '\0';
	Dsetpath( path2);
	
	if( app->aes_global[1] != 1 && single == 0) {
		p = strchr( prg, '.');
		if( p) {
			if( !stricmp(p+1, "ttp") ||
				!stricmp(p+1, "tos"))
				mode = 0;
			else
				mode = 1;
		} else
			mode = 0; /* Case Minix ?? */
		
		if( env) {
			SHELW shelenv;

			shelenv.newcmd=prg;
			shelenv.env=env;
			res = mt_shel_write(0x801,mode,100, (char *)&shelenv, file, app->aes_global);
		} else
			res = mt_shel_write(1,mode,100, prg, file, app->aes_global);
		if( !res) res = -1;
	} else {
		WINDOW *scan;
		INT16 x, y, w, h;
		
		/* Close all the windows, and change the current directory
		 * to the path of the application  */
		for( scan=app->wglb.first; scan; scan = scan->next) {
			if( scan->status & WS_OPEN) {
				mt_WindClose( app, scan);
				scan->status |= WS_PEXEC;
			}
		}
		/* remove the menubar  */
		if( app->menu) mt_menu_bar( app->menu, 0, app->aes_global);
		

		res=(int)Pexec( 0, prg, (void *) file, env);
		
		
		/* restore the menubar */
		if( app->menu) mt_menu_bar( app->menu, 1, app->aes_global);

		/* restore the windows	*/
		for( scan=app->wglb.first; scan; scan = scan->next) {
			if( scan->status & WS_PEXEC) {
				mt_WindGet( app, scan, WF_PREVXYWH, &x, &y, &w, &h);
				mt_WindOpen( app, scan, x, y, w, h);
				scan->status &= ~WS_PEXEC;
			}
		}
	}

	/* restore the current directory */
	Dsetdrv( drv);
	Dsetpath( path);

	return res;
}
