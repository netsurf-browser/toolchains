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
 * $Source: /cvsroot/windom/windom/src/w_getbkgr.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include "globals.h"

/** save a screen area
 *
 *  @param app application descriptor
 *  @param of_x,of_y,of_w,of_h coordinate and size of the area to save
 *  @param img a valid MFDB structure will containing the screen area saved
 *
 *	It is a sub function of mt_FormBegin(). The screen area is copied in memory. A
 *	valid structure MFDB should be given to w_get_bkgr() but the memory required
 *	to save the screen area is reserved by the function. To release the memory,
 *	w_put_bkgr() should be call.
 *
 *  @sa w_put_bkgr()
 */

void w_get_bkgr( APPvar *app, int of_x, int of_y, int of_w, int of_h, MFDB *img) {
	INT16 pxy[8];
	unsigned long taille;
	MFDB ecr = {0};		/* logical screen */

	taille = ((((unsigned long)(of_w / 16) + ((of_w % 16) != 0)) * 2 * (long)app->nplanes) * (unsigned long)of_h) + 256; 
#ifdef __SOZOBONX__
	img->fd_addr = lalloc( taille);			/* memory buffer to store on screen area */
#else
	img->fd_addr = malloc( taille);			/* memory buffer to store on screen area */
#endif
	img->fd_w = of_w;
	img->fd_h = of_h;
	img->fd_wdwidth = (of_w / 16) + ((of_w % 16) != 0);
	img->fd_stand = 1;
	img->fd_nplanes = app->nplanes;

	pxy[0] = of_x;
	pxy[1] = of_y;
	pxy[2] = pxy[0] + of_w - 1;
	pxy[3] = pxy[1] + of_h - 1;
	pxy[4] = 0;
	pxy[5] = 0;
	pxy[6] = of_w - 1;
	pxy[7] = of_h - 1;
	v_hide_c ( app->graf.handle);				/* hide the mouse */
	vro_cpyfm ( app->graf.handle, S_ONLY, pxy, &ecr, img);	/* copy image */
	v_show_c ( app->graf.handle, 1);		/* restore the mouse */
}
