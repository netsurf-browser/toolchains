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
 * $Source: /cvsroot/windom/windom/src/form_restore.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stdlib.h>
#include "globals.h"

/**
 * @brief Restore state of object in windowed formular
 *
 * @param app application descriptor,
 * @param win window descriptor,
 * @param mode type of formular : \b OC_FORM or \b 0C_TOOLBAR
 *
 * This function restores the objects state of a formular or a toolbar
 * previously saved by mt_FormSave().
 *
 * @sa mt_FormSave()
 */

int mt_FormRestore( APPvar *app, WINDOW *win, int mode)	{
	W_FORM *wform;
	int co;
	
	if( mode == OC_FORM )
		wform =  mt_DataSearch( app, win, WD_WFRM);
	else
		wform = &win->tool;

	if( !wform->save)
		return -1;
	for( co=0; co<wform->nb_ob; co++)
		wform->root[co].ob_state = wform->save[ co];
	return 0;
}
