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
 * $Source: /cvsroot/windom/windom/src/slid_setfunc.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include "globals.h"

/**
 * @brief define a slider event function.
 *
 * @param app application descriptor,
 * @param sld a slider structure,
 * @param func pointer to a slider event function,
 * @param data pointer to an user data.
 *
 * This function attaches a slider event function to a slider.
 * When the slider value is modified, this function is called with
 * the following prototype :
 * @code
       void doslid( WINDOW *win, int mode, short value, void *data, APPvar *ap);
@endcode
 *
 * \e win and \e mode are respectively the window descriptor host and
 * the formular type (\b OC_FORM or \b OC_TOOLBAR). Parameter \e value
 * is the new value of the slider. \e data is an optional pointer to a user data
 * specified by mt_SlidSetFunc().
 *
 * @sa mt_SlidCreate(), mt_SlidAttach()
 */

void mt_SlidSetFunc( APPvar *app, void *sld, func_doslid func, void *data) {
	SLIDER *slid = sld;
	slid->doslid = func;
	slid->data = data;
}
