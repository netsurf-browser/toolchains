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
 * $Source: /cvsroot/windom/windom/src/slid_setvalue.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include "globals.h"

/**
 * @brief Change the current slider value.
 *
 * @param app application descriptor,
 * @param sld slider structure,
 * @param value new value.
 *
 * This function sets the internal slider value. If the slider is
 * attached to a formular, widgets will be updated and 
 * the slider event function (see SlidSetFunc()) will be invoked if needed.
 *
 * @sa mt_SlidGetValue()
 */

void mt_SlidSetValue( APPvar *app, void *sld, short value) {
	SLIDER *slid = sld;
	slid->value = MIN(MAX(value,slid->min),slid->max);
	if( slid->doslid && slid->win)
		(*slid->doslid)( slid->win, slid->mode, slid->value, slid->data, app);
	if( mt_slid_newpos( app, slid)) 
		mt_ObjcDraw( app, slid->mode, slid->win, slid->bsld, MAX_DEPTH);
}
