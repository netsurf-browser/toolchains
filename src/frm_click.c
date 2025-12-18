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
 * $Source: /cvsroot/windom/windom/src/frm_click.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.9 $
 */

#include <stdlib.h>
#include "globals.h"

/** standard callback function to manage mouse click event on form
 *
 *  @param win window descriptor
 *  @param buff content of the #WM_FORM message
 *  @param app application descriptor
 *
 *  manage mouse button events (click!) which occur on forms.
 *
 *  This is the standard callback function attached to forms for
 *  the \b WM_XBUTTON event, registered by  mt_FormCreate().
 */
 
void __CDECL frm_click( WINDOW *win, short buff[8], APPvar *app) {
	int res;
	if( !(buff[5] & 0x01))
		return;
	res = frm_buttn_ev( app, win, OC_FORM);
	if( res != -1)  /* send a WM_FORM message for this MU_BUTTON event) */
		mt_snd_msg( app, win, WM_FORM, res, buff[6], 0, 0);
}
