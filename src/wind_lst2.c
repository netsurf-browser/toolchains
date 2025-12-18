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
 * $Source: /cvsroot/windom/windom/src/wind_lst2.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.5 $
 */


/*
 *  theses functions manage the __windowlist.
 *
 *  Others files must not directly read/write the __windowlist variable. This must
 *  be done thru the following functions
 */


#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/* return the bottom window of __windowlist */

WINDOW *wlst_get_bottom( APPvar *app) {
	WINDOWLIST *wl = app->priv->__windowlist;
	if (wl) return wl->win;
	return NULL;
}


/* get next window in __windowlist */

WINDOW *wlst_get_next( APPvar *app, WINDOW *win) {
	WINDOWLIST *wl = app->priv->__windowlist;
	while (wl && wl->win != win) wl=wl->next;
	if (wl && wl->next) return wl->next->win;
	return NULL;
}


/* get last window in __windowlist */

WINDOW *wlst_get_top( APPvar *app) {
	WINDOWLIST *wl = app->priv->__windowlist;
	if (wl == NULL) return NULL;
	while (wl->next) wl = wl->next;
	return wl->win;
}


/* remove the window from __windowlist */

void wlst_remove( APPvar *app, WINDOW *win) {
	WINDOWLIST *wl = app->priv->__windowlist;
	WINDOWLIST *trash;
	
	if (wl==NULL) {
		/* the list is empty ! */
	} else if (wl->win == win) {
		/* remove the 1st element of the list */
		app->priv->__windowlist = wl->next;
		free(wl);
	} else {
		/* look for "win" */
		while (wl->next && wl->next->win!=win) wl=wl->next;
		if (wl->next) { 
			/* "win" has been found ! */
			trash = wl->next;
			wl->next = trash->next;
			free(trash);
		} else {
			/* "win" is not in __windowlist */
		}
	}
}


/* add "win" as 1st element of __windowlist */

void wlst_add_bottom( APPvar *app, WINDOW *win) {
	WINDOWLIST *wl = (WINDOWLIST*)malloc(sizeof(WINDOWLIST));
	if (wl) {
		wl->win = win;
		wl->next = app->priv->__windowlist;
		app->priv->__windowlist = wl;
	}
}


/* add "win" as last element of __windowlist */

void wlst_add_top( APPvar *app, WINDOW *win) {
	WINDOWLIST *wl = (WINDOWLIST*)malloc(sizeof(WINDOWLIST));
	WINDOWLIST *l = app->priv->__windowlist;
	if (wl) {
		wl->win = win;
		wl->next = NULL;
		if (l == NULL) {
			app->priv->__windowlist = wl;
		} else {
			while (l->next) l = l->next;
			l->next = wl;
		}
	}
}


/* recompute app->wglb.* variables, and fix __windowlist if needed */

void wlst_recompute_wglb( APPvar *app) {
	INT16 htop;
	mt_wind_get( 0, WF_TOP, &htop, NULL, NULL, NULL, app->aes_global);
	app->wglb.front = mt_WindHandle( app, htop);
	app->wglb.appfront = wlst_get_top(app);
	/* check the result */
	if (app->wglb.front && app->wglb.front != app->wglb.appfront) {
		/* our __windowlist is wrong ! We have to move the real
		  top window at the end of __windowlist */
		wlst_remove(app,app->wglb.front);
		wlst_add_top(app,app->wglb.front);
		app->wglb.appfront = app->wglb.front;
	}
}
