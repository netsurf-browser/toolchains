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
 * $Source: /cvsroot/windom/windom/src/wind_new.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.6 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"


/**
 * @brief replacement of wind_new() for all AESes
 * 
 * @param app application descriptor,
 *
 * This function was never really tested and used.
 */

void mt_WindNew( APPvar *app) {
	
	while( app->wglb.first) {
		mt_snd_msg( app, app->wglb.first, WM_DESTROY, 0, 0, 0, 0);
		mt_EvntWindom( app, MU_MESAG);
	}
	if( app->aes_global[0] >= 0x0140)
		mt_wind_new(app->aes_global);
	else {
		/* mouse reset */
		v_show_c( app->graf.handle, 0);
		
		mt_wind_update( END_MCTRL, app->aes_global);
		mt_wind_update( END_UPDATE, app->aes_global);
	}
}
