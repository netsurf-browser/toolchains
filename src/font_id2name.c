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
 * $Source: /cvsroot/windom/windom/src/font_id2name.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"

/** convert an id-font in a font name
 *  
 *  @param app application descriptor
 *  @param id font identificator
 *  @param name name font associated to \a id, it should be a 64-byte buffer.
 *  
 *  @return 0 (success) or -1 if the font is not found
 *  
 *  The identificator font is an integer value which identify in a
 *  unique way a font. This fonction finds the font name  associated to
 *  a font identificator. The fonction needs a font manager to work correctly
 *  
 *  @sa mt_FontName2Id()
 */

int mt_FontId2Name( APPvar *app, int id, char *name) {
	int  i, scan;
	
	for( i=0; i<app->gdos; i++) {
		scan = mt_VqtName(app,i,name);
		if( id == scan)
			return 0; /* OK, name contains the font name */
	}
	return -1;	/* not found */
}

