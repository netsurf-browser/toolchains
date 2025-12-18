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
 * $Source: /cvsroot/windom/windom/src/form_thumb.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include "globals.h"


/*  This private function will be bound to thumb buttons.
 *  this function will manage a mouse click on a thumb button
 *  (hide the actual thumb, show the new thumb, get the 
 *   button of the new thumb to foreground)
 */

static void __CDECL thumb_do( WINDOW *win, int obj, int dummy, void *data, APPvar *app) {
	W_FORM *form = mt_DataSearch( app, win, WD_WFRM);
	THUMB *thumb = data;
	int i, idx;

	/* index of the selected button in thumb->idxbut[] */
	idx = -1;
	for( i=0; i<thumb->nbbut; i++) {
		if( thumb->idxbut[i] == obj) {
			idx = i;
			break;
		}
	}
	
	/* button found, and another button has been selected */
	if( idx != -1 && idx != thumb -> selbut) {
		/* show the new thumb */
		form->root[thumb->idxthb[idx]].ob_flags &= ~HIDETREE;
		/* hide the old thumb */
		form->root[thumb->idxthb[thumb->selbut]].ob_flags |= HIDETREE;
		/* active button */
		thumb->selbut = idx;
		form->edit = -1; 	/* WinDom bug */
		/* draw the thumb */
		mt_ObjcDraw( app, OC_FORM, win, thumb->idxthb[idx], MAX_DEPTH);
		/* draw the button */
		mt_ObjcDraw( app, OC_FORM, win, thumb->idxbut[idx], 0);
	}
}

/* free all the thumb stuff attached to this window.
 * this callback function is attached to the DESTROY event. 
 */

static void __CDECL FormThbFree( WINDOW *win, short buff[8], void *data, APPvar *app) {
	THUMB *thumb = data;
	UNUSED (win);
	free( thumb->idxthb);
	free( thumb->idxbut);
	free( thumb);
}

/** 
 * @brief handle a thumb object in a windowed formular.
 *
 * @param app application descriptor,
 * @param win window descriptor,
 * @param idxthb list of thumb index,
 * @param idxbut list of button index to link,
 * @param nb number of thumb object,
 * @return a pointer to a thumb structure or \b NULL if an error occurs.
 * 	
 * The function declares a thumb, i.e. a multiple subdialog, inside
 * a window dialog. After this call, thumb buttons are automatically
 * handled. Parameters \e idxthb and \e idxbut describe
 * links between sub dialogs and thumb buttons. There are arrays
 * of \e nb items. For each item \c i of these arrays,
 * \c idxthb[i] is linked to \c idxbut[i].
 *
 * The fonction returns a pointer which be used by functions 
 * mt_FormThbSet() and mt_FormThbGet().
 *
 * \b Example:
 * @code// Example of thumb indexes with three elements
int but [] = {BUT1, BUT2, BUT3};
int sub [] = {SUB1, SUB2, SUB3};

win = FormCreate( ...);
FormThumb( win, sub, but, 3); @endcode
 *
 * @sa mt_FormThbSet(), mt_FormThbGet().
 */

void *mt_FormThumb( APPvar *app, WINDOW *win, int *idxthb, int *idxbut, int nb) {
	W_FORM *form = mt_DataSearch( app, win, WD_WFRM);
	THUMB *thumb;
	int i;
	
	thumb = (THUMB*)malloc( sizeof( THUMB));
	if( !thumb)	{
		app->error = ENSMEM;
		return NULL;
	}
	thumb -> idxthb = malloc( sizeof(int)*nb);
	if( !thumb -> idxthb) {
		free( thumb);
		app->error = ENSMEM;
		return NULL;
	}
	thumb -> idxbut = malloc( sizeof(int)*nb);
	if( !thumb -> idxbut) {
		free( thumb->idxthb);
		free( thumb);
		app->error = ENSMEM;
		return NULL;
	}
	for( i=0; i<nb; i++) {
		thumb -> idxthb[i] = idxthb[i];
		thumb -> idxbut[i] = idxbut[i];
		mt_ObjcAttachFormFunc( app, win, idxbut[i], thumb_do, thumb);
	}
	thumb -> nbbut = nb;
	thumb -> selbut = 0;
	mt_EvntDataAdd( app, win, WM_DESTROY, FormThbFree, thumb, EV_TOP);

	if( form) {
		for(i=0; i<nb; i++) {
			form->root[idxthb[i]].ob_flags |= HIDETREE;
			form->root[idxbut[i]].ob_state &= ~SELECTED;
			form->root[idxbut[i]].ob_flags |= RBUTTON;
		}
		form->root[idxthb[0]].ob_flags &= ~HIDETREE;
		form->root[idxbut[0]].ob_state |= SELECTED;
	}
	
	return thumb;
}
