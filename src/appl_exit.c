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
 * $Source: /cvsroot/windom/windom/src/appl_exit.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/02/22 16:15:25 $
 *   $Revision: 1.12 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "globals.h"
#include "fsel.h"

int 
__mt_ApplExit( APPvar *app) {
	int ret;
	
	/* delete the desktop WINDOW* */
	mt_WindDelete( app, app->priv->desk);

	exit_scroll_menu(app);
	
	/* userdef library */
	udlib_exit(app);
	
	/* lib conf */
	if( app->priv->__confapp_buffer)
		free( app->priv->__confapp_buffer);
	if( app->priv->__confdef_buffer)
		free( app->priv->__confdef_buffer);

	if( !(CONF(app)->flag & FLG_NOPAL))
		w_setpal( app, NULL);
	if( app->graf.palette) free( app->graf.palette);
	if( app->gdos)
		vst_unload_fonts( app->graf.handle, 0);
	if( app->priv->__bubble_quit != -1 ) {
		short buff[8];
		buff[0] = AP_TERM;
        buff[1] = mt_AppId(app);
        buff[2] = 0;
        mt_appl_write( app->priv->__bubble_quit, 16, buff, app->aes_global);
	}
	v_clsvwk( app->graf.handle);
	v_clsvwk( app->priv->udlib_vdih);
	mt_EvntClear( app, NULL);
	if( app->pipe) Mfree( app->pipe);

	__freeall_obspec(app);
	if (CONF(app)->udlib_filenames) free(CONF(app)->udlib_filenames);
	free (app->priv);
	ret = mt_appl_exit(app->aes_global);
	free(app);
	return ret;
}

/**
 * @brief Terminate a WinDom and AES session.
 *
 * @param app application descriptor.
 * @return 0 or code error.
 *
 * @sa mt_ApplInit()
 */

int 
mt_ApplExit( APPvar *app) {
	short *global;
	int ret;
	
	global = app->aes_global;
	
	ret = __mt_ApplExit( app);
	
	free(global);
	return ret;
}

