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
 * $Source: /cvsroot/windom/windom/src/form_thbget.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stdlib.h>
#include "globals.h"

/**
 * @brief Retrieve active thumb.
 *
 * @param app application descriptor,
 * @param data pointer to the thumb structure (provide by mt_FormThumb()),
 * @param type possible values are :
 *   - 0 returns active button,
 *   - 1 returns active thumb (sub-dialog).
 * @return an object index or -1 if an error occurs.
 *
 * This function returns the active thumb (the thumb itself or button linked to
 * the active thumb).
 *
 * @sa mt_FormThumb(), mt_FormThbSet().
 */

int mt_FormThbGet( APPvar *app, void *data, int type) {
	THUMB *thumb = data;
	if( thumb) {
		if( type == 0)
			return thumb->idxbut[thumb->selbut];
		else 
			return thumb->idxthb[thumb->selbut];
	} else
		return -1;
}
