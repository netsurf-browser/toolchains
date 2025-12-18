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
 * $Source: /cvsroot/windom/windom/src/vst_loadfonts.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include <stdio.h>
#include "globals.h"

/** emulate vst_load_fonts() when GDOS is not detected
 *
 *  @param app application descriptor
 *  @param res same as \a res paramter of VDI function vst_load_fonts()
 *  @return same as return value of VDI function vst_load_fonts()
 *
 */

int mt_VstLoadFonts( APPvar *app, int res) {
	if ( ! vq_gdos() ) {
		int r = load_fontid(app);
		return (r < 0) ? 0 : r;
	}
	return vst_load_fonts( app->graf.handle, res);
} 

