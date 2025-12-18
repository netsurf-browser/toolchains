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
 * $Source: /cvsroot/windom/windom/src/font_name2id.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/04/18 18:46:30 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"

/** convert a font name in a id-font
 *
 *  @param app application descriptor
 *  @param name font name
 *
 *  @return font id or -1 if font does not exist
 *
 *  The identificator font is an integer value which identify in a
 *	unique way a font. This fonction finds the id-font associated to
 *  a font name. The fonction needs a font manager to work correctly.
 *
 *  @sa mt_FontId2Name()
 */

int mt_FontName2Id( APPvar *app, char *name) {
	int id, i;
	char buf[60];
	
	for( i=0; i<app->gdos; i++) {
		id = mt_VqtName(app,i,buf);
		if( !strcmp(buf, name))
			return id;
	}
	return -1;
}

