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
 * $Source: /cvsroot/windom/windom/src/comp_slider.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/01/03 14:05:47 $
 *   $Revision: 1.10 $
 */

#include "globals.h"
#include "sliders.h"

/*
 * @brief AES-like slider COMPONENT.
 * 
 * The slider size and position is controlled by the \c CDT_SLIDER_SIZE
 * and \c CDT_SLIDER_POS attached data values (0-1000). In order to
 * update the component appearance according to the newly set values the
 * \c CM_REFLOW message handler is to be executed.
 *
 * Use comp_slider_link() function to establish a link between a to be
 * controlled component and the slider. This is to send the slider action
 * (user drags the slider or clicks the arrow) the \c CDT_SLIDER_CONTROL data
 * attached component's appropriate handler is executed (\c WM_[VH]SLID
 * or \c WM_ARROWED).
 *
 * The component passes through the \c CM_GETFOCUS and \c CM_LOSEFOCUS
 * messages to every child component to handle visual activation and
 * deactivation.
 */


/* No matter the tree structure shape of the component this points to the
 * root of the slider */
#define CDT_SLIDER 	0x444e4dUL

/* Is used to allow common code for horizontal and vertical sliders */
#define CDT_SL_WHICH 	0x444d4dUL

/* Value of the WM_ARROW message to be sent (for code reusability) */
#define CDT_SL_ARROWED	0x444c4dUL


static
void __CDECL comp_sldr_focuschange( COMPONENT *p, long *buff, APPvar *app ) {
	COMPONENT *c;
	/* spread it through its children COMPONENT tree */
	FOR_EACH_CHILD( c, p->children ) {
		comp_dfs( c, buff, app);
	}
}

static
void __CDECL comp_sldr_reflow( COMPONENT *c, long *buff, APPvar *app ) {
	/* FIXME: do not use c->parent in the XBUTTON handlers attach a
	 * pointer directly so that the order of the arrows does not matter
	 *
	 * Also create unified slider components for vertical or horisontal
	 * visualization. Perhaps some interesting slider may be demonstrated
	 * to prove the universality.
	 */

	COMPONENT *pgup, *slid, *pgdn;
	pgup = (COMPONENT*)listFirst(c->children);
	pgup = (COMPONENT*)listNext(pgup);

	pgup = (COMPONENT*)listFirst(pgup->children);
	slid = (COMPONENT*)listNext(pgup);
	pgdn = (COMPONENT*)listNext(slid);

	/*   | pgup u | sld s | pgdn d |
	 *   0                       1000
	 *          u+d = 1000-s
	 */

	slid->flex = (int)mt_CompDataSearch(app, c, CDT_SLIDER_SIZE);
	{
		long notvisible = 1000 - slid->flex;
		pgup->flex = ((int)mt_CompDataSearch(app, c, CDT_SLIDER_POS) * notvisible) / 1000;
		pgdn->flex = notvisible - pgup->flex;
	}

	comp_reflow( c, app);
}


static
void __CDECL comp_sldr_widget_clickaction( COMPONENT *c, long *buff, APPvar *app ) {
	COMPONENT *s = mt_CompDataSearch(app, c, CDT_SLIDER);
	COMPONENT *ctrl = s ? mt_CompDataSearch( app, s, CDT_SLIDER_CONTROL) : NULL;
	if ( !ctrl ) return;

	buff[0] = WM_ARROWED;
	buff[4] = (long)mt_CompDataSearch(app, c, CDT_SL_ARROWED);
	mt_CompEvntExec(app, ctrl, buff);
}

static
void __CDECL comp_sldr_vslid( COMPONENT *c, long *buff, APPvar *app ) {
	COMPONENT *s = mt_CompDataSearch(app, c, CDT_SLIDER);
	COMPONENT *ctrl = s ? mt_CompDataSearch( app, s, CDT_SLIDER_CONTROL) : NULL;
	LGRECT lwork, cwork;
	INT16 dum, but, cur = app->evnt.my;
       	INT16 old = cur;
	INT16 delta;

	if ( !ctrl ) return;

	if( app->aes4 & AES4_XGMOUSE) mt_graf_mouse( M_SAVE, 0L, app->aes_global);
	mt_graf_mouse( FLAT_HAND, NULL, app->aes_global);

	mt_CompGetLGrect(app, c, WF_WORKXYWH, &cwork);
	mt_CompGetLGrect(app, c->parent, WF_WORKXYWH, &lwork);
	delta = cur - cwork.g_y - 1;
	do {
		if ( cur != old ) { /* position changed */
			buff[0] = (long)mt_CompDataSearch(app, s, CDT_SL_WHICH); /* WM_VSLID or WM_HSLID; */
			buff[4] = MAX( MIN( 1000, ((cur - delta - lwork.g_y)*1000)/(lwork.g_h - cwork.g_h) ), 0);
			mt_CompEvntExec(app, ctrl, buff);
			old = cur;
		}
		mt_graf_mkstate( &dum, &cur, &but, &dum, app->aes_global);
	} while( but);

	if( app->aes4 & AES4_XGMOUSE && mt_graf_mouse( M_RESTORE, 0L, app->aes_global))
		mt_graf_mouse(ARROW,NULL,app->aes_global);
}

static
void __CDECL comp_sldr_hslid( COMPONENT *c, long *buff, APPvar *app ) {
	COMPONENT *s = mt_CompDataSearch(app, c, CDT_SLIDER);
	COMPONENT *ctrl = s ? mt_CompDataSearch( app, s, CDT_SLIDER_CONTROL) : NULL;
	LGRECT lwork, cwork;
	short dum, but, cur = app->evnt.mx;
       	short old = cur;
	short delta;

	if ( !ctrl ) return;

	if( app->aes4 & AES4_XGMOUSE) mt_graf_mouse( M_SAVE, 0L, app->aes_global);
	mt_graf_mouse( FLAT_HAND, NULL, app->aes_global);

	mt_CompGetLGrect(app, c, WF_WORKXYWH, &cwork);
	mt_CompGetLGrect(app, c->parent, WF_WORKXYWH, &lwork);
	delta = cur - cwork.g_x - 1;
	do {
		if ( cur != old ) { /* position changed */
			buff[0] = (long)mt_CompDataSearch(app, s, CDT_SL_WHICH); /* WM_HSLID or WM_VSLID; */
			buff[4] = MAX( MIN( 1000, ((cur - delta - lwork.g_x)*1000)/(lwork.g_w - cwork.g_w) ), 0);
			mt_CompEvntExec(app, ctrl, buff);
			old = cur;
		}
		mt_graf_mkstate( &cur, &dum, &but, &dum, app->aes_global);
	} while( but);

	if( app->aes4 & AES4_XGMOUSE && mt_graf_mouse( M_RESTORE, 0L, app->aes_global))
		mt_graf_mouse(ARROW,NULL,app->aes_global);
}


int comp_slider_set(  APPvar *app, WINDOW *win, COMPONENT *s, int mode, int par1, int par2, int par3, int par4) {
#if 0
	/* enable if more settings are expected */
	switch( mode) {
		case WF_VSLIDE:
		case WF_VSLSIZE:
		case WF_HSLIDE:
		case WF_HSLSIZE:
			...
	}
#endif

	/* set the value */
	mt_CompDataAttach( app, s,
			(mode==WF_VSLSIZE || mode==WF_HSLSIZE)
			? CDT_SLIDER_SIZE : CDT_SLIDER_POS,
			(void*)par1 );

	{
		/* update the slider component sizes
		 * note: do not call the comp_sldr_reflow directly */
		long buff[8] = { CM_REFLOW };
		mt_CompEvntExec( app, s, buff);
		/* redraw the slider */
		mt_CompEvntRedraw( app, s);
	}

	return 0;
}


COMPONENT *comp_slider_create( APPvar *app, int mode, int width, int sz, int flex ) {
	OBJECT *tree = get_frame_slider( app);   /* FIXME: tree may be NULL is FrameInit() has not been called */
	COMPONENT *s, *p;

	COMPONENT *lln;
	COMPONENT *mln;
	COMPONENT *lpg = mt_CompCreate( app, CLT_VERTICAL, 0, 100 );
	COMPONENT *sld = comp_objc_create( app, tree + M_V_SL, CLT_VERTICAL, 0, 500 );
	COMPONENT *mpg = mt_CompCreate( app, CLT_VERTICAL, 0, 400 );

	/* p = pgup + sld + pgdn */
	if ( mode == CS_VSLIDE ) {
		s = mt_CompCreate( app, CLT_VERTICAL, sz, flex );
		p = comp_objc_create( app, tree + M_V_PG, CLT_VERTICAL, 0, 1 );

		mt_CompEvntAttach( app, sld, WM_XBUTTON, comp_sldr_vslid );

		mt_CompDataAttach( app, lpg, CDT_SL_ARROWED, (void*)WA_UPPAGE );
		mt_CompDataAttach( app, mpg, CDT_SL_ARROWED, (void*)WA_DNPAGE );

		lln = comp_objc_create( app, tree + M_V_UP, CLT_VERTICAL, width, 0 );
		mt_CompDataAttach( app, lln, CDT_SL_ARROWED, (void*)WA_UPLINE );
		mln = comp_objc_create( app, tree + M_V_DW, CLT_VERTICAL, width, 0 );
		mt_CompDataAttach( app, mln, CDT_SL_ARROWED, (void*)WA_DNLINE );
	} else {
		s = mt_CompCreate( app, CLT_HORIZONTAL, sz, flex );
		p = comp_objc_create( app, tree + M_H_PG, CLT_HORIZONTAL, 0, 1 );

		mt_CompEvntAttach( app, sld, WM_XBUTTON, comp_sldr_hslid );

		mt_CompDataAttach( app, lpg, CDT_SL_ARROWED, (void*)WA_LFPAGE );
		mt_CompDataAttach( app, mpg, CDT_SL_ARROWED, (void*)WA_RTPAGE );

		lln = comp_objc_create( app, tree + M_H_LF, CLT_VERTICAL, width, 0 );
		mt_CompDataAttach( app, lln, CDT_SL_ARROWED, (void*)WA_LFLINE );

		mln = comp_objc_create( app, tree + M_H_RT, CLT_VERTICAL, width, 0 );
		mt_CompDataAttach( app, mln, CDT_SL_ARROWED, (void*)WA_RTLINE );
	}


	mt_CompEvntAttach( app, lln, WM_XBUTTON, comp_sldr_widget_clickaction );
	mt_CompEvntAttach( app, lpg, WM_XBUTTON, comp_sldr_widget_clickaction );
	mt_CompEvntAttach( app, mpg, WM_XBUTTON, comp_sldr_widget_clickaction );
	mt_CompEvntAttach( app, mln, WM_XBUTTON, comp_sldr_widget_clickaction );

	mt_CompDataAttach( app, lln, CDT_SLIDER, s );
	mt_CompDataAttach( app, lpg, CDT_SLIDER, s );
	mt_CompDataAttach( app, sld, CDT_SLIDER, s );
	mt_CompDataAttach( app, mpg, CDT_SLIDER, s );
	mt_CompDataAttach( app, mln, CDT_SLIDER, s );

	mt_CompAttach( app, p, lpg );
	/* FIXME: TODO:
	 * mt_CompDataAttach( app, s,   CDT_SLIDLPG, lpg ); */
	mt_CompAttach( app, p, sld );
	/* mt_CompDataAttach( app, s,   CDT_SLIDSLD, sld ); */
	mt_CompAttach( app, p, mpg );
	/* mt_CompDataAttach( app, s,   CDT_SLIDMPG, mpg ); */

	/* s = up + p + dn */
	mt_CompAttach( app, s, lln );
	mt_CompAttach( app, s, p );
	mt_CompAttach( app, s, mln );

	sld->bounds.min_height = 16;	/* FIXME: widget size config? */
	sld->bounds.min_width = 16;

	/* to behave like AES WindSet( VSLID ) updates the slider immediatelly */
	mt_CompEvntAttach( app, s, CM_REFLOW, comp_sldr_reflow );
	/* enable/disable on focus change */
	mt_CompEvntAttach( app, s, CM_GETFOCUS, comp_sldr_focuschange);
	mt_CompEvntAttach( app, s, CM_LOSEFOCUS,comp_sldr_focuschange);
	

#if DEBUG
#define CDT_DEBUGID 0x434f4cUL

	mt_CompDataAttach( app, lln, CDT_DEBUGID, "lln" );
	mt_CompDataAttach( app, mln, CDT_DEBUGID, "mln" );
	mt_CompDataAttach( app, lpg, CDT_DEBUGID, "lpg" );
	mt_CompDataAttach( app, mpg, CDT_DEBUGID, "mpg" );
	mt_CompDataAttach( app, sld, CDT_DEBUGID, "sld" );
	mt_CompDataAttach( app, p, CDT_DEBUGID, "s_p" );
	mt_CompDataAttach( app, s, CDT_DEBUGID, "sss" );
#endif

	return s;
}

/**
 * @brief Link the slider and the controlled component.
 * 
 * @param app
 * @param c     the component to control (slider messages to be sent to)
 * @param s     slider component
 * @param mode  \c CS_VSLIDE or \c CS_HSLIDE according to which direction the
 *              slider \p s is to be used in by \p c.
 **/
void comp_slider_link( APPvar *app, COMPONENT *c, COMPONENT *s, int mode ) {
	mt_CompDataAttach( app, s, CDT_SLIDER_CONTROL, c );
	mt_CompDataAttach( app, s, CDT_SL_WHICH, (void*)(mode == CS_VSLIDE ? WM_VSLID : WM_HSLID) );
	mt_CompDataAttach( app, c, mode == CS_VSLIDE ? CDT_VSLID : CDT_HSLID, s );
}


