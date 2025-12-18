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
 * $Source: /cvsroot/windom/windom/src/av_init.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "av.h"


/* seach the AV server:
 * - in "AVSERVER=" env. variable
 * - applications named AVSERVER, GEMINI, THING
 * - any application with APP_SHELL flag
 *
 * returns the AES application ID of the AV server or -1 (no AV server found).
 */
 
static int av_server_id( APPvar *app) {
	char *p, name[9];
	INT16 id=-1;
	
	mt_shel_envrn(&p, "AVSERVER=", app->aes_global);
	if( p) {
		sprintf(name, "%-8s", p);
		id = mt_appl_find( name, app->aes_global);
	}	
	if( id != -1) return id;
	id = mt_appl_find( "AVSERVER",app->aes_global);
	if( id != -1) return id;
	id = mt_appl_find( "GEMINI  ",app->aes_global);
	if( id != -1) return id;
	id = mt_appl_find( "THING   ",app->aes_global);
	if( id != -1) return id;
	if( app->aes4 & AES4_APPSEARCH) {
		INT16 type;
		if( mt_appl_search( APP_FIRST, name, &type, &id, app->aes_global) == 0)
			if( type & APP_SHELL) return 1;
		while( mt_appl_search( APP_NEXT, name, &type, &id, app->aes_global))
			if( type & APP_SHELL) return 1;
		id = -1;
	}
	return id;
}


/*  Ping the AV server (send the AV_PROTOKOLL message, and wait for the answer
 *  VA_PROTOSTATUS). 
 */

static int av_ping( APPvar *app, int status, long idle) {
	INT16 msg[8];
	
	strcpy(app->pipe,app->priv->__av_name);
	mt_ApplWrite( app, app->avid, AV_PROTOKOLL, status, 0, 0, ADR(app->pipe));
	if( mt_AvWaitfor( app, VA_PROTOSTATUS, msg, idle)) {
		app->priv->__av_status[0] = msg[3];
		app->priv->__av_status[1] = msg[4];
		app->priv->__av_status[2] = msg[5];
		strcpy( app->priv->__av_server, *(char **)&msg[6]);
		return 1;
	} else
		return 0;
}


/**
 * @brief  Initialize the AV protocol.
 *
 * @param app application descriptor
 * @param name name of the client application (normaly, our application) 
 *             in AES format (a 8-capitalized character eventually completed
 *             with space character),
 * @param status bitfield defining the supported features by the client application:
 *	      - \b A_SETSTATUS (0x1) : client supports the VA_SETSTATUS message
 *	      - \b A_START (0x2) : client supports the VA_START message
 *	      - \b A_STARTED (0x4) : client supports the AV_STARTED message
 *	      - \b A_FONTCHANGED (0x8) : client supports the VA_FONTCHANGED message
 *	      - \b A_QUOTE (0x10): filename parameter are eventually quoted
 *	      - \b A_PATH_UPDATE (0x20) : client supports the VA_PATH_UPDATE message
 * @param idle timeout for the server reply (units: ms). the value "1000" 
 *             is a good choice.
 * @return AES id of the AV server or error code :
 *            - -1 : server not found,
 *            - -2 : server doesn't not supporte AV protocol.
 *
 * This function initializes an AV session between the client
 * and the AV server and waits for the server's answer.
 * Use mt_AvStatus() function to know the actions supported
 * by the server. An AV session should be closed by calling
 * mt_AvExit() before application termination.
 *
 * To identifiate the AV server, mt_AvInit() uses the 
 * environ variable AVSERVER. If this variable is not defined,
 * AvInit() tries the following processes: 'AVSERVER', 'THING',
 * 'GEMINI' then the desktop application.
 *
 * mt_AvInit() declares to the server the actions supported by the
 * client (our application). The AV client states table displayed below gives the
 * differents possible value. Among these values, the \b A_QUOTE
 * value is very important. It means that the client supportes
 * quoted filename : when a filename contains a space character, the
 * complete string is surrounded by a quote character. For example,
 * the string "The World" is quoted: "'The World'". Use 
 * mt_AvStrfmt() function to quote or unquote the strings.
 *
 * On return, mt_AvInit() returns the AV server AES pid (or
 * negative error) and \e app->avid contains this pid.
 * 
 *
 * @sa mt_AvExit(), mt_Status(), mt_AvStrfmt()
 */
 
int mt_AvInit( APPvar *app, char *name, int status, long idle) {
	if( app->avid == -1) {
		app->avid = av_server_id(app);
		app->priv->__av_name = strdup( name);
	}

	/* error: no AV server => bye */
	if( app->avid == -1) return -1 ;

	/* error, a client cannot be its server => bye */
	if( app->avid == mt_AppId(app)) return -3 ;

	/* register the client to the AV server */
	if( av_ping( app, status, idle))
		return app->avid;
	else
		return -2;	/* The protocol is not supported => bye */
}
