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
 * $Source: /cvsroot/windom/windom/src/form_wbegin.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include "globals.h"

/*  the callback function attached to the destroy event 
 */

static void __CDECL _form_wind_close( WINDOW *__win, short buff[8], APPvar *app) {
	/* FIXME! :  IMO, win=__win is better. */
	WINDOW *win = mt_WindHandle( app, buff[3]);

	mt_FormAttach( app, win, NULL, (func_evnt)0L);
	mt_WindClose( app, win);
	mt_WindDelete( app, win);
	mt_MenuEnable(app);
}


/**
 * @brief Open a formular in a modal window.
 *
 * @param app application descriptor,
 * @param tree formular object tree,
 * @param name window title,
 * @return a window descriptor.
 *
 * mt_FormWindBegin() creates a modal window formular that is
 * a formular displayed in a modal window. A modal window disables the
 * user interaction of the application excepted in this own window. 
 * It is very similar to the classic formular except the AES is not 
 * stopped. Events
 * of the formular are handled by mt_FormWindDo(). The formular is
 * closed with mt_FormWindEnd().
 *
 * The user WinDom variable \b windom.mform.widget defines the
 * widgets of the window.	
 *
 * \sa mt_FormWindDo(), mt_FormWindEnd().
 */

WINDOW *mt_FormWindBegin( APPvar *app, OBJECT *tree, char *name) {
	WINDOW *win;

	win = mt_FormCreate( app, tree, CONF(app)->mwidget, (func_evnt)0L, name, NULL, 1, 0);
	if( win == NULL) return NULL;
	mt_EvntAttach( app, win, WM_DESTROY, _form_wind_close);
	mt_MenuDisable(app);
	mt_WindSet( app, win, WF_BEVENT, BEVENT_MODAL, 0, 0, 0);
	return win;
}
