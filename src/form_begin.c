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
 * $Source: /cvsroot/windom/windom/src/form_begin.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stdlib.h>
#include "globals.h"

/**
 * @brief Open a classic formular.
 *
 * @param app application descriptor,
 * @param tree objects tree standing for the formular,
 * @param bckgrnd pointer to a valid MFDB structure or \b NULL.
 *
 * This function creates and displays a classic formular i.e. a 
 * preemptive formular blocking the AES events (not displayed
 * in a window). The formular is centered at screen (by calling
 * the mt_GrectCenter() function). If a \b NULL value is given to
 * parameter \e bckgrnd, the screen area hidden by the formular
 * is not saved in memory. A simple \b WM_REDRAW will be
 * sent when the formular will be closed (mt_FormEnd()). This mode can
 * be used when the formular hides the desktop or windows. If a 
 * valid parameter is given to \e bckgrnd, the screen area
 * hidden by the formular will stored in this buffer. This mode can
 * be used when the formular hides another classic formulars otherwise
 * hidden formulars need to be redrawn.
 *
 * A call of mt_FormDo() handles user interaction with the formular.
 * A call of mt_FormClose() close and release memory used by mt_FormBegin().
 *
 * \b Example
 * @code
 void do_dialog( int tree_index) {
   MFDB screen;
   OBJECT *tree;
   int res;
          
   rsrc_gaddr( 0, tree_index, &tree);
   FormBegin( tree, &screen);
   res = FormDo( tree, -1)
   switch(res){
   case OK:
     ...
     break;
   }
   FormEnd( tree, &screen);
 } 
@endcode
 * @sa mt_FormDo(), mt_FormEnd(), mt_GrectCenter().
 */

void mt_FormBegin( APPvar *app, OBJECT *tree, MFDB *bckgrnd) {
	INT16 x, y, w, h;
	
	mt_GrectCenter( app, tree->ob_width, tree->ob_height, &x, &y);
	tree->ob_x = x;
	tree->ob_y = y;
	w = tree->ob_width;
	h = tree->ob_height;
	
	if( bckgrnd)	/* On save the background */
		w_get_bkgr( app, x-6, y-6, w+11, h+11, bckgrnd);
	else
		mt_form_dial( FMD_START, 0, 0, 0, 0, x-6, y-6, w+11, h+11, app->aes_global);
	if( CONF(app)->weffect == TRUE || CONF(app)->weffect == DEFVAL)
		mt_form_dial( FMD_GROW, 0, 0, 0, 0, x, y, w, h, app->aes_global);
	while( !mt_wind_update( BEG_MCTRL, app->aes_global));
	mt_objc_draw( tree, 0, MAX_DEPTH, x-6, y-6, w+11, h+11, app->aes_global);
}

