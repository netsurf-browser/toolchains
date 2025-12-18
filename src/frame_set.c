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
 * $Source: /cvsroot/windom/windom/src/frame_set.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.7 $
 */

#include <stdarg.h>
#include <stdlib.h>
#include "globals.h"

/** Frame settings
 *  
 *  @param app application descriptor
 *  @param win window descriptor (of a root frame window of a framed window),
 *  @param mode parameter to set
 *  @param ... new values depending on \a mode
 *  
 *  This function sets the special features of frame windows. The table below
 *  lists and comments the different modes of the function.
 *  
 *  <table>
 *  <tr>
 *    <th>Mode<th>Type window<th>"..." Parameters
 *  <tr>
 *    <td>FRAME_BORDER
 *    <td>root window
 *    <td>border size of frames (int value). Values in pixel unit
 *  <tr>
 *    <td>FRAME_COLOR
 *    <td>root window
 *    <td>color of border of frames (int value). Color is the VDI color index
 *  <tr>
 *    <td>FRAME_KEYBD
 *    <td>root window
 *    <td>dispatching of keyboard event. may be one of the following (int) values:
 *        - \b MOUSE_WINDOW : event sent to the frame pointed by the mouse
 *        - \b FRONT_WINDOW : event sent to the active frame
 *  <tr>
 *    <td>FRAME_ACTIVE
 *    <td>root window
 *    <td>the frame (WINDOW* value) to set as the new active frame
 *  <tr>
 *    <td>FRAME_TOPPED_ACTIV
 *    <td>root window
 *    <td>TRUE (int value) if topped frame should become active, or FALSE otherwise.
 *  <tr>
 *    <td>FRAME_SIZE
 *    <td>framed window
 *    <td>width and height (int values) of the new framed window size. These values
 *        depends on FRAME_FLAGS values
 *  <tr>
 *    <td>FRAME_FLAGS
 *    <td>framed window
 *    <td>type of flag (int value) and value TRUE/FALSE of the flag (int value).
 *
 *        Here is the list of the flags:
 *        - \b FRAME_HSCALE : The framed window height is defined proportionaly to the window root
 *          work area height (0..100).
 *        - \b FRAME_WSCALE : The framed window width is defined proportionaly to the window root
 *          work area width (0..100).
 *        - \b FRAME_HFIX : The framed window height is an absolute value (in pixel).
 *        - \b FRAME_WFIX : The framed window width is an absolute value (in pixel).
 *        - \b FRAME_NOBORDER : The framed window has no borders
 *        - \b FRAME_SELECT : The framed windows is activable
 *  </table>
 *  
 */  

void mt_FrameSet( APPvar *app, WINDOW *win, int mode, ...) {
	va_list args;
	int parm1=0, parm2=0;
	WINDOW *wparm=NULL;
	FRAME *frame;
	
	va_start( args, mode);
	if( mode == FRAME_ACTIV)
		wparm = va_arg( args, WINDOW*);
	else {
		parm1 = va_arg( args, int);
		parm2 = va_arg( args, int);
	}

	if( win->status & WS_FRAME_ROOT) {
		frame = mt_DataSearch( app, win, WD_WFRA);
		switch( mode) {
		case FRAME_BORDER:
			frame->border = parm1;
			break;
		case FRAME_COLOR:
			frame->color = parm1;
			break;
		case FRAME_KEYBD:
			SET_BIT( frame->flags, FLG_KEYMOUSE, parm1);
			break;
		case FRAME_NOBG:
			SET_BIT( frame->flags, NO_BACKGROUND, parm1);
			break;
		case FRAME_ACTIV:
			/* set the given frame active, and enable the gadget */
			if( !(GETFINFO(app,wparm)->flags & FRAME_SELECT))
				break;
			if( win->status & WS_OPEN) {
				GRECT r;
			
				if( frame->front &&
					frame->front != wparm ) {
					mt_FrameCalc( app, frame->front, 1, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
					set_gadget_pos( app, frame->front, &r, 0x1|0x2|0x4);  /* TODO: replace 0x* by constant names */
				}
				
				mt_FrameCalc( app, wparm, 1, &r.g_x, &r.g_y, &r.g_w, &r.g_h); 		
				set_gadget_pos( app, wparm, &r, 0x1|0x2); /* TODO: replace 0x* by constant names */
			}
			frame->front = wparm;
			frame->widget = wparm;
			break;
		case FRAME_TOPPED_ACTIV:
			SET_BIT( frame->flags, FRAME_ACTIV, parm1);
			break;
		}
	}

	if( win->status & WS_FRAME) {
		FRAMEINFO *finfo = GETFINFO(app,win);
		switch( mode) {
		case FRAME_SIZE:
			finfo->w = parm1;
			finfo->h = parm2;
			break;
		case FRAME_FLAGS:
			/* set or unset specificied bit */
			SET_BIT( finfo->flags, parm1, parm2);
			break;
		}
	}
	va_end( args);
}
