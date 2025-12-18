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
 * $Source: /cvsroot/windom/windom/src/slid_attach.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.7 $
 */

#include <stdlib.h>
#include "globals.h"

int mt_slid_newpos( APPvar *app, void *_slid) {
	SLIDER *slid = _slid;
	long large;
	int new, ret;
	OBJECT *tree = mt_ObjcTree( app, slid->mode, slid->win);

	/* No cursor in the slider, so we return 0 */	
	if( slid->bsld == -1)
		return 0;

	if (slid->ori == SLD_HORI) 
		large = (tree[slid->bsld].ob_width - tree[slid->sld].ob_width - 2*SLID_OFFSET);
	else
		large = (tree[slid->bsld].ob_height - tree[slid->sld].ob_height - 2*SLID_OFFSET);

	new = (long)((long)slid->value * large) / (long)(slid->max - slid->min) + SLID_OFFSET;
	
	if (slid->ori == SLD_HORI) {
		ret = (new != tree[slid->sld].ob_x);
		tree[slid->sld].ob_x = new;
	} else {
		ret = (new != tree[slid->sld].ob_y);
		tree[slid->sld].ob_y = new;
	}
	return ret;
}

/* Slider object calbacks */

static 
void __CDECL mt_slid_up( WINDOW *win, int index, int mode, void *_slid, APPvar *app ) {
	SLIDER *slid = _slid;
	int rdw;
	short old;
	INT16 dum, but;
	
	mt_ObjcChange( app, mode, win, index, SELECTED, TRUE);
	do {
		mt_graf_mkstate( &dum, &dum, &but, &dum, app->aes_global);
		/* new value */
		old = slid->value;
		slid -> value = MAX( slid->min, slid -> value - slid -> sinc);
		if( old == slid->value) continue;
		/* new slider position */
		rdw = mt_slid_newpos( app, slid);
		if( slid->upd == SLD_IMME && slid->doslid)	
			(*slid->doslid)( win, mode, slid->value, slid->data, app);
		/* redraw object */
		if( rdw) mt_ObjcDraw( app, mode, win, slid->bsld, MAX_DEPTH);	
	} while( but) ;
	if( slid->upd == SLD_DIFF && slid->doslid)	
		(*slid->doslid)( win, mode, slid->value, slid->data, app);
	mt_ObjcChange( app, mode, win, index, ~SELECTED, TRUE);
}

static 
void __CDECL mt_slid_dn( WINDOW *win, int index, int mode, void *_slid, APPvar *app ) {
	SLIDER *slid = _slid;
	int rdw;
	short old;
	INT16 dum, but;

	mt_ObjcChange( app, mode, win, index, SELECTED, TRUE);
	do {
		mt_graf_mkstate( &dum, &dum, &but, &dum, app->aes_global);
		old = slid->value;
		slid -> value = MIN( slid->max, slid -> value + slid -> sinc);
		if( old == slid->value) continue;
		rdw = mt_slid_newpos( app, slid);
		if( slid->upd == SLD_IMME && slid->doslid)	
			(*slid->doslid)( win, mode, slid->value, slid->data, app);
		if( rdw) mt_ObjcDraw( app, mode, win, slid->bsld, MAX_DEPTH);
	} while( but) ;
	if( slid->upd == SLD_DIFF && slid->doslid)	
		(*slid->doslid)( win, mode, slid->value, slid->data, app);
	mt_ObjcChange( app, mode, win, index, ~SELECTED, TRUE);
}

static 
void __CDECL mt_slid_bg( WINDOW *win, int index, int mode, void *_slid, APPvar *app) {
	SLIDER *slid = _slid;
	int rdw, dir, sel;
	INT16 xc, yc, xm, ym, dum, but;
	OBJECT *tree = mt_ObjcTree( app, mode, win);
	short old = -1;
	
#define DIR_UP 0
#define DIR_DN 1

	UNUSED(index);
	/* get the direction */
	mt_objc_offset( tree, slid->sld, &xc, &yc, app->aes_global);
	mt_graf_mkstate( &xm, &ym, &but, &dum, app->aes_global);
	if( slid->ori == SLD_HORI) {
		dir = (xm < xc)?DIR_UP:DIR_DN;
		sel = (xm < xc)?slid->up:slid->dn;
	} else { 
		dir = (ym < yc)?DIR_UP:DIR_DN;
		sel = (ym < yc)?slid->up:slid->dn;
	}
	if( sel != -1) mt_ObjcChange( app, mode, win, sel, SELECTED, TRUE);		
	do {
		mt_graf_mkstate( &xm, &ym, &but, &dum, app->aes_global);
		/* new value */
		if( dir == DIR_UP)
			slid -> value = MAX( slid->min, slid -> value - slid -> linc);
		else
			slid -> value = MIN( slid->max, slid -> value + slid -> linc);
		if( slid->value == old)
			continue;
		else
			old = slid->value;
		/* object position */
		rdw = mt_slid_newpos( app, slid);
		if( slid->upd == SLD_IMME && slid->doslid)	
			(*slid->doslid)( win, mode, slid->value, slid->data, app);
		/* redraw objets */
		if( rdw) mt_ObjcDraw( app, mode, win, slid->bsld, MAX_DEPTH);
	} while( but);
	if( sel != -1) mt_ObjcChange( app, mode, win, sel, ~SELECTED, TRUE);
	if( slid->upd == SLD_DIFF && slid->doslid)	
		(*slid->doslid)( win, mode, slid->value, slid->data, app);
}

static 
void __CDECL mt_slid_sld( WINDOW *win, int index, int mode, void *_slid, APPvar *app) {
	SLIDER *slid = _slid;
	int rdw;
	INT16 xm, ym, xc, yc, but, dum;
	OBJECT *tree = mt_ObjcTree( app, mode, win);
	long val;
	int dxm, dym;
	
	mt_ObjcChange( app, mode, win, index, SELECTED, TRUE);

	if( app->aes4 & AES4_XGMOUSE) mt_graf_mouse( M_SAVE, 0L, app->aes_global);
	mt_graf_mouse( FLAT_HAND, NULL, app->aes_global);
	
	mt_graf_mkstate( &xm, &ym, &but, &dum, app->aes_global);
	mt_objc_offset( tree, slid->sld, &xc, &yc, app->aes_global);
	dxm = (xm - xc);
	dym = (ym - yc);

	do {
		mt_graf_mkstate( &xm, &ym, &but, &dum, app->aes_global);

		mt_objc_offset( tree, slid->bsld, &xc, &yc, app->aes_global);
		/* Relative position with respect to bsld */
		xm -= xc+1;
		ym -= yc+1;

		if( slid->ori == SLD_HORI) {
			xm -= dxm;
			if( xm < SLID_OFFSET) xm = SLID_OFFSET;
			if( xm > tree[slid->bsld].ob_width-tree[slid->sld].ob_width-2*SLID_OFFSET) 
				xm = tree[slid->bsld].ob_width-tree[slid->sld].ob_width-2*SLID_OFFSET;
			if( xm != tree[slid->sld].ob_x) {
				
				val = (long)((long)xm * (long)(slid->max-slid->min)) / (long)(tree[slid->bsld].ob_width - tree[slid->sld].ob_width - 2);
				/* Results should be rounded to a multiple value of SINC */
				slid -> value = val;
				slid -> value = MIN( slid -> value, slid->max);
				slid -> value = MAX( slid -> value, slid->min);

				rdw = mt_slid_newpos( app, slid);
				if( slid->upd == SLD_IMME && slid->doslid)	
					(*slid->doslid)( win, mode, slid->value, slid->data, app);
				if( rdw) mt_ObjcDraw( app, mode, win, slid->bsld, MAX_DEPTH);
			}
		} else {
			ym -= dym;
			if( ym < SLID_OFFSET) ym = SLID_OFFSET;
			if( ym > tree[slid->bsld].ob_height-tree[slid->sld].ob_height-2*SLID_OFFSET) 
				ym = tree[slid->bsld].ob_height-tree[slid->sld].ob_height-2*SLID_OFFSET;
			if( ym != tree[slid->sld].ob_y) {
				val = (long)((long)ym * (long)(slid->max-slid->min)) / (long)(tree[slid->bsld].ob_height - tree[slid->sld].ob_height - 2);
				slid -> value = val;
				slid -> value = MIN( slid -> value, slid->max);
				slid -> value = MAX( slid -> value, slid->min);

				rdw = mt_slid_newpos( app, slid);	
				if( slid->upd == SLD_IMME && slid->doslid)	
					(*slid->doslid)( win, mode, slid->value, slid->data, app);
				if( rdw) mt_ObjcDraw( app, mode, win, slid->bsld, MAX_DEPTH );
			}
		}

		/* don't load the processor */
		mt_evnt_timer( 100L, app->aes_global); 
	} while( but);
	if( slid->upd == SLD_DIFF && slid->doslid)	
		(*slid->doslid)( win, mode, slid->value, slid->data, app);

	if( !(app->aes4 & AES4_XGMOUSE) || mt_graf_mouse( M_RESTORE, 0L, app->aes_global) == 0 )
		mt_graf_mouse(ARROW,NULL, app->aes_global);

	mt_ObjcChange( app, mode, win, index, ~SELECTED, TRUE);	
}

/**
 * @brief Attach a slider to an object structure in a formular.
 * 
 * @param app application descriptor,
 * @param sld slider structure,
 * @param mode \b OC_FORM or \b OC_TOOLBAR,
 * @param win window descriptor targeted,
 * @param up index of decrement widget or -1,
 * @param bsld index of slider background or -1,
 * @param curs index of cursor widget or -1,
 * @param dn index of increment widget or -1, 
 *
 * This function attaches a slider structure to slider object in a formular.
 * Only window and toolbar formular are handled. 
 *
 * Objects \e up and \e dn are generally \b TOUCHEXIT \b SELECTABLE
 * \b G_BOXCHAR object containing an up arrow and and down arrow if slider is
 * vertical, or left arrow and right arrow if slider is horizontal.
 * When user clicks on these widgets, slider value is increased or decreased
 * using the small increment step defined by \e sinc mt_SlidCreate() parameter.
 * \e up and/or \e dn can be set to -1. In this case, the slider won't use 
 * these widgets.
 * 
 * Object \e sld should always be a children of the \e bsld object.
 * When the user click on pager object (bsld), slider value is increased
 * or decreased using the large increment step defined by \e linc mt_SlidCreate() 
 * parameter. If \e sld and \e bg  are set to -1, the slider won't
 * use these widgets.
 *
 * The \b TOUCHEXIT flag is required for the four objects specified
 * in mt_SlidAttach().
 *
 * @sa mt_SlidCreate()
 */

void mt_SlidAttach( APPvar *app, void *sld, int mode, WINDOW *win, int up, int bsld, int curs, int dn) {
	SLIDER *slid = sld;

	slid -> win = win;
	slid -> mode = mode;
	slid -> up = up;
	slid -> dn = dn;
	slid -> sld = curs;
	slid -> bsld = bsld;

	mt_slid_newpos( app, slid);
	
	if (mode == OC_FORM) {
		if( up != -1)   mt_ObjcAttachFormFunc( app, win, up,   mt_slid_up,  slid);
		if( dn != -1)   mt_ObjcAttachFormFunc( app, win, dn,   mt_slid_dn,  slid);
		if( bsld != -1) mt_ObjcAttachFormFunc( app, win, bsld, mt_slid_bg,  slid);
		if( curs != -1) mt_ObjcAttachFormFunc( app, win, curs, mt_slid_sld, slid);
	} else {
		if( up != -1)   mt_ObjcAttachTBFunc( app, win, up,   mt_slid_up,  slid);
		if( dn != -1)   mt_ObjcAttachTBFunc( app, win, dn,   mt_slid_dn,  slid);
		if( bsld != -1) mt_ObjcAttachTBFunc( app, win, bsld, mt_slid_bg,  slid);
		if( curs != -1) mt_ObjcAttachTBFunc( app, win, curs, mt_slid_sld, slid);
	}
}
