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
 * $Source: /cvsroot/windom/windom/src/rc_set.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include "globals.h"
#include <limits.h>

/** Initialize a GRECT data 
 *
 *  @param rect the GRECT structure to initialize
 *  @param x,y,w,h values to put in \a rect
 *
 */

void rc_set( GRECT *rect, int x, int y, int w, int h) {
	rect->g_x = x;
	rect->g_y = y;
	rect->g_w = w;
	rect->g_h = h;
}

/** Initialize a LGRECT data 
 *
 *  @param rect the LGRECT structure to initialize
 *  @param x,y,w,h values to put in \a rect
 *
 */

void rc_lset( LGRECT *rect, long x, long y, long w, long h) {
	rect->g_x = x;
	rect->g_y = y;
	rect->g_w = w;
	rect->g_h = h;
}

/** Convert the GRECT values to LGRECT 
 * 
 *  @param l output rectangle
 *  @param r input rectangle
 *
 *  copy a GRECT rectangle \a r to a LGRECT rectangle \a l.
 */

void rc_grect2lgrect ( LGRECT *l, const GRECT *r) {
	l->g_x = r->g_x;
	l->g_y = r->g_y;
	l->g_w = r->g_w;
	l->g_h = r->g_h;
}

/** Convert the LGRECT values to GRECT 
 * 
 *  @param r output rectangle
 *  @param l input rectangle
 *
 *  copy a LGRECT rectangle \a l to a GRECT rectangle \a r.
 *
 *  @note
 *  coordinates and sizes in \a l are 32 bits integers. The conversion
 *  to 16 bits integers (for GRECT structure) may lead to a saturation
 *  of the data to   SHRT_MIN and SHRT_MAX.
 */

void rc_lgrect2grect ( GRECT *r, const LGRECT *l) {
	r->g_x = MIN( MAX( l->g_x, SHRT_MIN ), SHRT_MAX );
	r->g_y = MIN( MAX( l->g_y, SHRT_MIN ), SHRT_MAX );
	r->g_w = MIN( MAX( l->g_w, SHRT_MIN ), SHRT_MAX );
	r->g_h = MIN( MAX( l->g_h, SHRT_MIN ), SHRT_MAX );
}


/** Compute the interesection of two LGRECT rectangles
 *
 *  @param r1 the first rectangle
 *  @param r2 the other rectangle. The intersection area will
 *            be returned in \p r2.
 *
 *  @return 0 if theses rectangle don't have any common area, any
 *          other value value if the intersection exist. Then,
 *          \p r2 will contain the common area.
 *
 */

short rc_lintersect (const LGRECT * r1, LGRECT * r2) {
	long tx, ty, tw, th;
	short ret;

	tx = MAX (r2->g_x, r1->g_x);
	tw = MIN (r2->g_x + r2->g_w, r1->g_x + r1->g_w) - tx;

	ret = (0 < tw);
	if (ret)
	{
		ty = MAX (r2->g_y, r1->g_y);
		th = MIN (r2->g_y + r2->g_h, r1->g_y + r1->g_h) - ty;

		ret = (0 < th);
		if (ret)
		{
			r2->g_x = tx;
			r2->g_y = ty;
			r2->g_w = tw;
			r2->g_h = th;
		}
	}

	return ret;
}

