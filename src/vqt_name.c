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
 * $Source: /cvsroot/windom/windom/src/vqt_name.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"

/** emulate vqt_name when GDOS not detected
 *
 *  @param app application descriptor
 *  @param elem same as \a elem paramter of VDI function vqt_name()
 *  @param name same as \a name paramter of VDI function vqt_name()
 *  @return same as return value of VDI function vqt_name()
 * 
 *	mt_VqtName() has the same effect than vqt_name() except it works
 *  even if a font driver is not present in memory. The condition
 *	is a valid FONTID file exists in the $ETC directory.
 */
	
int mt_VqtName( APPvar *app, int elem, char *name) {
	if( vq_gdos())
		return vqt_name( app->graf.handle, elem, name);
	else {
		if( elem < app->priv->maxfontid) {
			strcpy( name, app->priv->listfontid[elem].name);
			return app->priv->listfontid[elem].id;
		}
	}
	strcpy( name, "system font");
	return 1;
}

