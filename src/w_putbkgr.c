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
 * $Source: /cvsroot/windom/windom/src/w_putbkgr.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include "globals.h"

/** Restore a saved screen area
 *
 *  @param app application descriptor
 *  @param of_x,of_y,of_w,of_h coordinate and size of the area to restore
 *  @param img a valid MFDB structure containing the screen area saved.
 *
 *	It is a sub function of mt_FormEnd(). The screen area to restore should be
 *	previously saved by w_get_bkgr(). After the call, the memory is released, so
 *	the function can be called only one time with the same MFDB structure.
 *
 *  @sa w_get_bkgr()
 */
	
void w_put_bkgr(APPvar * app, int of_x, int of_y, int of_w, int of_h, MFDB *img) {
	INT16 pxy[8];
	MFDB ecr = {0};	/* logical screen */

	pxy[0] = 0;		
	pxy[1] = 0;
	pxy[2] = of_w - 1;
	pxy[3] = of_h - 1;
	pxy[4] = of_x;
	pxy[5] = of_y;
	pxy[6] = of_x + of_w - 1;
	pxy[7] = of_y + of_h - 1;
	v_hide_c ( app->graf.handle);
	vro_cpyfm ( app->graf.handle, S_ONLY, pxy, img, &ecr);	/* copy the image */
	v_show_c ( app->graf.handle, 1);
	free ( img->fd_addr);		/* free memory */
}

