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
 * $Source: /cvsroot/windom/windom/src/form_wdo.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.7 $
 */

#include <stdlib.h>
#include "globals.h"

/** handle a modal window formular
 * 
 * @param app,Evnt same as for mt_FormWindDo()
 * @param buff the buffer to use for AES messages
 * @return same as mt_FormWindDo()
 *
 * This function is similar to mt_FormWindDo() -- in fact this
 * is what mt_FormWindDo() invoke to do the whole job.
 *
 *  The main difference is the parameter \a buff[8].
 *
 *  This parameter allows to read the full content of the AES
 *  message that occurs (if the function returns MU_MESAG).
 *
 */

int mt_FormWindDoBuff( APPvar *app, int Evnt, short buff[8]) {
	int evnt_res;

	while( 1) {
		evnt_res = mt_EvntWindomBuff( app, Evnt, buff);
		
		if( (evnt_res & MU_MESAG)) {
			switch( buff[ 0]) {

			case WM_FORM:
				return( buff[ 4]);
			case WM_DESTROY:
				return( -1);
			}
		}
		/* else  */
		if( Evnt & FORM_EVNT)
			return (evnt_res|FORM_EVNT);
	}
}

/**
 * @brief handle a modal window formular
 *   
 * @param app application descriptor,
 * @param Evnt bit field of event to catch (see mt_EvntWindom()) +
 *  the special value \b FORM_EVNT,
 * @return depend on user interaction :
 *    - object index if a \b WM_FORM event occurs,
 *    - a combinaison of \b FORM_EVNT flag and event returned
 *      by evnt_multi() if \e Evnt parameter has the \b FORM_EVNT flag set.
 *    - (-1) if \b WM_DESTROY event occurs.
 *
 * This function is used in order handle a formular opened by mt_FormWindBegin().
 * A \b MU_MESAG is always required to handle correctly the formular. 
 *
 * User interaction or event can be traited directly from the output
 * of the function. In the case where \b FORM_EVNT flag is used, a while()
 * structure is required because event imply the output of mt_FormWindDo().
 *
 * Another method to trait event and user selection is to catch up event
 * with mt_EvntAttach() and object selection with mt_ObjcAttach().
 * 
 * mt_FormWindDo() and mt_FormWindEnd() does not need a window paramater :
 * window formular are modals and there is no ambiguity because a modal
 * window is always the top window application. Several modal window
 * can be imbricated.
 *
 * @note This function does not allow to read the whole AES message that
 * occured (if the function returns MU_MESAG). If you need to read the full
 * AES message when this function returns, then you should use mt_FormWindDoBuff().
 *  
 * @sa mt_FormWindBegin(), mt_FormWindEnd().
 */

int mt_FormWindDo( APPvar *app, int Evnt) {
	short buff[8];
	return mt_FormWindDoBuff( app, Evnt, buff);
}
