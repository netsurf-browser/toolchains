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
 * $Source: /cvsroot/windom/windom/src/bubble_evnt.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include "globals.h"

/**
 * @brief display the bubble help defined by BubbleAttach().
 *
 * @param app application descriptor
 * @retval 0 if bubble is not found
 * @retval WS_FORM if bubble is found in a dialog box
 * @retval MW_TOOLBAR if bubble is found in a toolbar
 *
 * This function finds the object pointed by the mouse sprite. If 
 * an object is found and if a bubble help is linked to this object
 * (with mt_BubbleAttach()) the bubble help is displayed. mt_BubbleEvnt()
 * works only with window formular or toolbar.
 * 
 * @code
#include <windom.h>

void RightButton( void) {
	if( evnt.mbut & 0x2)
		BubbleEvnt();
}

int main(void) {
	OBJECT *tree;
	
	ApplInit();
	RsrcLoad( "TEST.RSC");
	rsrc_gaddr( 0, FORM1, &tree);
	
	// Link the bubble help to objects ...
	BubbleAttach( tree,  0, "Formular background");
	BubbleAttach( tree, OK, "An exit button"); 
	
	// Create form
	FormCreate( tree, MOVER|NAME, NULL, "test", NULL, 1, 0);

	// Handle globally the MU_BUTTON event
	EvntAttach( NULL, WM_XBUTTON, RightButton);

	do
		EvntWindom( MU_MESAG|MU_BUTTON);
	while( wglb.first);

	BubbleFree();  // release the bubbles
	RsrcFree();
	ApplExit();
	return 0;
}
@endcode
 * 
 *
 * @sa mt_BubbleAttach(), mt_BubbleFree(), mt_BubbleFind(), W_FORM
 */
	

int mt_BubbleEvnt( APPvar *app) {
	WINDOW *win;
	int res;
	char *help;
	W_FORM *form;
	
	win = mt_WindHandle( app, mt_wind_find(app->evnt.mx,app->evnt.my,app->aes_global));
	if( win && win->status & WS_FORM) {
		form = mt_DataSearch(app,win,WD_WFRM);
		res = mt_objc_find( form->root, 0, 8, app->evnt.mx, app->evnt.my, app->aes_global);
		if( res != -1 && mt_BubbleFind( app, form->real, res, &help)) {
			mt_BubbleCall( app, help, app->evnt.mx, app->evnt.my);
			return WS_FORM;
		}
	}
	if( win && win->status & WS_TOOLBAR) {
		res = mt_objc_find( TOOL(win), 0, 8, app->evnt.mx, app->evnt.my, app->aes_global);
		if( res != -1 && mt_BubbleFind( app, win->tool.real, res, &help)) {
			mt_BubbleCall( app, help, app->evnt.mx, app->evnt.my);
			return WS_TOOLBAR;
		}
	}
	return 0;
}
