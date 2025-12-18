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
 * $Source: /cvsroot/windom/windom/src/slid_create.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include "globals.h"

/**
 * @brief Initialise a slider structure.
 * 
 * @param app application descriptor,
 * @param min minimal slider value,
 * @param max maximal slider value,
 * @param sinc small incremental/decremental step
 * @param linc large incremental/decremental step 
 * @param value current slider value,
 * @param ori slider orientation :
 *        - \b SLD_HORI  horizontal slider,
 *        - \b SLD_VERT  vertical slider,
 * @param upd type of slider update :
 *        - \b SLD_IMME slider is immediatly updated,
 *        - \b SLD_DIFF slider is updated after the user action,
 * @return a new slider structure, correctly initialized. 
 * 	
 * This function creates a slider structure. It is not an AES object tree 
 * in a formular but only some variables used to handle a set of objects
 * representing and acting as a slider.
 *
 * The object slider should be creating in a resource editor (or in other
 * way). Then this object slider must to attached to the slider structure
 * created by mt_SlidCreate(). The function mt_SlidAttach() performs this link.
 * When the slider object is no more used (formular closed), the slider object
 * should be delete using mt_SlidDelete().
 *
 * \e min, \e max and \e value are respectively
 * minimal value, the maximal value and the current value of the internal
 * slider variable. To read or eventually to change an internal variable,
 * use functions mt_SlidGetValue() and mt_SlidSetValue().
 *
 * Parameters \e sinc and \e linc addresses the user interaction in the slider
 * object. \e linc represents a small incrementation or decrementation of
 * the slider value and \e linc represents a large incrementation or decrementation
 * (see mt_SlidAttach()). 
 * 
 * Parameter \e upd is usefull if you attach an update function to
 * the slider (see mt_SlidSetFunc()). This function is called when the slider
 * value is changed.
 *
 * @sa mt_SlidAttach(), mt_SlidSetValue(), mt_SlidGetValue(), mt_SlidSetFunc();
 *     mt_SlidDelete().
 */

void *mt_SlidCreate( APPvar *app, short min, short max, short sinc, short linc, short value, int ori, int upd) {
	SLIDER *slid = malloc( sizeof(SLIDER));

	if( !slid) return NULL; /* memory allocation error */
	slid -> min = min;
	slid -> max = max;
	slid -> sinc = sinc;
	slid -> linc = linc;
	slid -> ori = ori;
	slid -> upd = upd;
	slid -> value = MIN(MAX(value,min),max);
	slid -> doslid = (func_doslid)0L;
	slid -> data = NULL;
	slid -> win = NULL;
	return slid;
}
