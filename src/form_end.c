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
 * $Source: /cvsroot/windom/windom/src/form_end.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stdlib.h>
#include "globals.h"

/**
 * @brief Close a classic formular.
 *
 * @param app application descriptor,
 * @param tree formular objects tree,
 * @param bckgrnd pointer to a valid memory structure or \b NULL.
 *
 * This function closes a formular previously created with
 * mt_FormBegin(). A \b NULL value for \e bckgrnd parameter
 * sent a \b WM_REDRAW event to the desktop. A correct
 * value of \e bckgrnd restores the screen area hidden
 * by the formular and memory allocated by mt_FormBegin()
 * is freed.
 *
 * @sa mt_FormEnd(), mt_FormDo()
 */

void mt_FormEnd( APPvar *app, OBJECT *tree, MFDB *bckgrnd) {
	INT16 x, y, w, h;
	
	mt_GrectCenter( app, tree->ob_width, tree->ob_height, &x, &y);
	w = tree->ob_width;
	h = tree->ob_height;
	
	if( bckgrnd)
		w_put_bkgr( app, x-6, y-6, w+11, h+11, bckgrnd);
	else
		mt_form_dial( FMD_FINISH, 0, 0, 0, 0, x-6, y-6, w+11, h+11, app->aes_global);
	if( CONF(app)->weffect == TRUE || CONF(app)->weffect == DEFVAL)
		mt_form_dial( FMD_SHRINK, 0, 0, 0, 0, x, y, w, h, app->aes_global);
	mt_wind_update( END_MCTRL, app->aes_global);
}
