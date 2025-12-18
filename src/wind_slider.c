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
 * $Source: /cvsroot/windom/windom/src/wind_slider.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.6 $
 */

#include <stddef.h>
#include <stdlib.h>
#include "globals.h"


/*********************************************************/
/* Manage the sliders           		 				 */
/*********************************************************/

/**
 * @brief Adjust window sliders according to the internal WinDom
 *        slider system.
 *
 * @param app Application descriptor,
 * @param win window descriptor,
 * @param mode slider to update :
 *     - \b HSLIDER : update horizontal slider,
 *     - \b VSLIDER : update vertical slider,
 * 
 * WindSlider() updates the size and position of horizontal and
 * vertical sliders according on value of the following variables : \e win->xpos, 
 * \e win->ypos, \e win->xpos_max, \e win->ypos_max, \e win->h_u, 
 * \e win->w_u . We recall signification of these variables :
 *  - \e win->xpos : horizontal relative position of data in the window workaera,
 *  - \e win->ypos : vertical relative position of data in the window workaera,
 *  - \e win->xpos_max : maximal possible value for \e win->xpos (0 <= xpos < xpos_max)
 *  - \e win->ypos_max : maximal possible value for \e win->ypos (0 <= ypos < ypos_max)
 *  - \e win->w_u : horizontal scrolling unit (in pixel) when a horizontal line scroll occurs,
 *  - \e win->h_u : vertical scrolling unit (in pixel) when a vertical line scroll occurs,
 *
 * Unlike WinDom, AES slider positions are between 0 and 1000. With WinDom,
 * values are between 0 and \c sizeof(int)=2^32 with Gcc or 2^16 with Pure C.
 * 
 * \b Example
 *
 * Let's give a typical example. We want display an ASCII text in a window. 
 * In this context, the variable \e win->ypos is exactly the index of the first 
 * line displayed in the window and the variable \e win->xpos is the first column 
 * displayed in the window (we suppose we use a non proportional font to display 
 * the text). So the variable \e win->ypos_max represents the number of lines of 
 * the text and the variable \e win->xpos_max should be the size of the largest line 
 * or a fixed number like 255, to have it easier. The variable \e win->h_u represents 
 * the height of a character cell and the variable \e win->w_u is the width of a character 
 * cell. 
 * When the window is opened, variables \e win->xpos and \e win->ypos should be zero. 
 * The following figure pictures this situation :
 * <pre>
(0,0)                              xpos_max
     ----------------Text------------------
     |                                    | <- Top of the text.
     |       ======= Window =======       |
     |Hello g|uy. Nice to meet you|<-ypos |
     |       |       _            |       |
     |       |      |c| <- h_u    |       |
     |       |       ^---- w_h    |       |
     |       |                    |       |
     |       ----------------------       |
     | xpos---^                           |
     |                                    |
  .->|                                    | <- Botton of the text
  |  --------------------------------------
 ypos_max </pre>
 *
 * These variables are set then the window created. Then, the function mt_WindSlider() 
 * sets the size and position of the sliders according to the variables previously described. 
 * Usually, the function mt_WindSlider() is used when you change the value of a slider variable.
 * In other cases, event standard functions update slider position and call mt_WindSlider(). 
 *
 * @todo In order to mask information, one could access internal slider variables via 
 *       WindSet()/WindGet(). One can use the SLIDER library functions too. Inversly
 *       WindSet(WF_(H|V)(SLSIZE|SLIDE)) should act on internal WinDom slider values.
 */

void mt_WindSlider( APPvar *app, WINDOW *win, int mode) {
	INT16 x,y,w,h;
	unsigned long size, pos;
	
	mt_WindGet( app, win, WF_SLIDXYWH, &x, &y, &w, &h);
	if( mode & VSLIDER) {
		if ( win->ypos_max < (long)h/win->h_u)
			size = 1000L;
		else
			size = MAX( 50L, (unsigned long)h*1000L/(unsigned long)(win->h_u*win->ypos_max));
		mt_WindSet( app, win, WF_VSLSIZE,(int)size , 0, 0, 0);

		if( win->ypos_max > (long)h/win->h_u) {
			pos = (unsigned long)win->ypos *1000L/(unsigned long)(win->ypos_max-h/win->h_u);
			mt_WindSet( app, win, WF_VSLIDE, (int)pos, 0, 0, 0);
		} else if (win->ypos) {
			win->ypos = 0;
			mt_WindSet( app, win, WF_VSLIDE, 0, 0, 0, 0); 
		}
	}
	if( mode & HSLIDER) {
		if ( win->xpos_max < (long)w/win->w_u)
			size = 1000L;
		else
			size = MAX( 50L, (unsigned long)w*1000L/(unsigned long)( win->w_u*win->xpos_max));
		mt_WindSet( app, win, WF_HSLSIZE, (int)size, 0, 0, 0);

		if( win->xpos_max > (long)w/win->w_u) {
			pos = (unsigned long)win->xpos*1000L/(unsigned long)(win->xpos_max-w/win->w_u);
			mt_WindSet( app, win, WF_HSLIDE, (int)pos, 0, 0, 0);
		} else if (win->xpos) {
			win->xpos = 0;
			mt_WindSet( app, win, WF_HSLIDE, 0, 0, 0, 0);
		}
	}
}
