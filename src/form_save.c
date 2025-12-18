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
 * $Source: /cvsroot/windom/windom/src/form_save.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stdlib.h>
#include "globals.h"

/**
 * @brief Store objects state of a windowed formular.
 *
 * @param app application descriptor,
 * @param win window descriptor,
 * @param mode type of formular : \b OC_FORM or \b 0C_TOOLBAR
 *
 * This function stores in a special buffer of the WINDOW structure
 * value of ob_state of each object in the formular. These values can
 * be restored by mt_FormRestore() (typically, after a Cancel action).
 * 
 * Memory used by mt_FormSave() is released when window is destroyed
 * or when the formular (or toolbar) is removed from the window via
 * mt_FormAttach() (or mt_WindSet(WF_TOOLBAR)).
 *
 * Currently, this function is incomplete because editable objects
 * are not saved. It is probably most powerful to use mt_ObjcDup()
 * to create a snapshot of a formular that is make probably this
 * function deprecated.
 *
 * @sa mt_FormRestore()
 */

int mt_FormSave( APPvar *app, WINDOW *win, int mode) {
	W_FORM *wform;
	int co;
	
	if( mode == OC_FORM)
		wform = mt_DataSearch( app, win, WD_WFRM);
	else
		wform = &win->tool;
	
	if( wform->save == NULL)
		wform->save = (int *)malloc( sizeof(int)*wform->nb_ob);
	if( wform->save == NULL)
		return -39;
	for( co=0; co<wform->nb_ob; co++)
		wform->save[ co] = wform->root[co].ob_state;
	return 0;
}
