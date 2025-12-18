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
 * $Source: /cvsroot/windom/windom/src/conf_getline.c,v $
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

/**  read acces by line in the configuration file
 *
 *  @param app application descriptor
 *  @param string pointer to a buffer to store the line
 *  @retval 0 no more lines to read
 *  @retval >0 number of the line read
 *
 *	mt_ConfGetLine() provides a line acces to read the configuration file.
 *	It can only read a line with the following format: 
 *  @code variable = value @endcode
 *  If the \a string parameter is NULL, the line pointer inside
 *	the file is set to the begin of the application area.
 *
 *  @note
 *  \a string should pointer to a sufficially big buffer to store
 *
 *  @sa mt_ConfInquire(), mt_ConfRead(), mt_ConfWrite()
 */

int mt_ConfGetLine( APPvar *app, char *string) {
	char *line = NULL;  /* modif Arnaud 02/2003 */
	int mode = -1;
	
	if( app->priv->__confapp_buffer) {
		line = app->priv->__confapp_buffer;
		mode = 1;
	} else if( app->priv->__confdef_buffer) {
		line = app->priv->__confdef_buffer;
		mode = 0;
	}

	/* reinit */
	if( line == NULL || string == NULL) {
		app->priv->__confgetline_pos = 0;
		app->priv->__confdef_line_pos = app->priv->__confdef_line_top;
		app->priv->__confapp_line_pos = app->priv->__confapp_line_top;
		return 0;
	}
	line += app->priv->__confgetline_pos;
	if( *line == '\0')
		return 0;	/* plus rien … lire */
	while( *line != '\n' && *line != '\0') {
		*string++=*line++;
		app->priv->__confgetline_pos++;
	}
	*string='\0';
	if( *line == '\n')
		app->priv->__confgetline_pos++;

	if( mode == -1) return 0;
	return (mode ? (app->priv->__confapp_line_pos++) : (app->priv->__confdef_line_pos++));
}


