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
 * $Source: /cvsroot/windom/windom/src/rsrc_userdraw.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.7 $
 */

#include <stdlib.h>
#include "globals.h"

#define MASK_STATE		(DISABLED|CROSSED|CHECKED|OUTLINED|SHADOWED)

/*
 *  transform any object to a userdef object, and attach the given 
 *  userdraw subroutine to this object.
 */

static WORD  __CDECL user_draw( PARMBLK *pblk) {
	USER_DRAW *udraw;
	func_userdraw f;
	GRECT r1, r2;

	/* get the user subroutine */
	udraw = (USER_DRAW *)pblk->pb_parm;
	f = udraw -> draw;
	/* set the clipping area */
	rc_set( &r1, pblk -> pb_xc, pblk -> pb_yc, pblk -> pb_wc, pblk -> pb_hc);
	rc_set( &r2, pblk -> pb_x, pblk -> pb_y, pblk -> pb_w, pblk -> pb_h);
	if( rc_intersect( &r1, &r2)) {
		GRECT c = udraw->win->graf->clip;
		mt_ClipOn( udraw->app, udraw->win->graf, &r2);
		(*f)( udraw->win, pblk, udraw->data, udraw->app);
		mt_ClipOn( udraw->app, udraw->win->graf, &c );
	}
	return (pblk->pb_currstate & MASK_STATE);
}

/**
 * @brief associate a drawing function to an objet formular
 *
 * @param app application descriptor,
 * @param mode \b OC_FORM or \b OC_TOOLBAR,
 * @param win window descriptor,
 * @param index object index,
 * @param draw address of drawing function,
 * @param data address of an user data,
 * @return a negative code error or 0 otherwise.
 *
 * mt_RsrcUserDraw() attaches a drawing function to an object. AES will call
 * the function \e draw() to draw the object. 
 *
 *  Refer to the documentation of the #func_userdraw type for a description of
 *  the userdef callback function interface.
 *
 * @sa mt_RsrcXtype(), func_userdraw.
 */

int mt_RsrcUserDraw( APPvar *app, int mode, WINDOW *win, int index, func_userdraw draw, void *data) {
    USERBLK *user;
    USER_DRAW *udraw;
    OBJECT *tree = (mode == OC_FORM)?FORM(app,win):TOOL(win);

    user = (USERBLK *)malloc( sizeof(USERBLK));
    if( user == NULL) return ENSMEM;
    udraw = (USER_DRAW *)malloc(sizeof(USER_DRAW));
    if( udraw == NULL) {
        free( user);
        return ENSMEM;
    }
    udraw -> win  = win;
    udraw -> draw = draw;
    udraw -> data = data;
	udraw -> app  = app;
    user->ub_code = user_draw;
    user->ub_parm = (long)udraw;
    tree[index].ob_type = G_USERDEF | (USERDRAW << 8);
    tree[index].ob_spec.userblk = user;
    return 0;
}
