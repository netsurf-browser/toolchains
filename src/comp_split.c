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
 * $Source: /cvsroot/windom/windom/src/comp_split.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/03/27 18:13:41 $
 *   $Revision: 1.5 $
 */

#include "globals.h"


static long comp_split_hmvd( COMPONENT *c, long delta, APPvar *app ) {
#define AXIS_SIZE(rect)  (rect.g_w)
#define AXIS_MAXBOUND(bounds)  (bounds.max_width)
#define AXIS_MINBOUND(bounds)  (bounds.g_w)

	COMPONENT *p = (COMPONENT *)listPrev( (LINKABLE*)c );
	COMPONENT *n = (COMPONENT *)listNext( (LINKABLE*)c );

	LGRECT pwork, nwork, psz, nsz;
	mt_CompGetLGrect(app, p, WF_WORKXYWH, &pwork);
	mt_CompGetLGrect(app, n, WF_WORKXYWH, &nwork);
	mt_CompGetLGrect(app, p, CF_MAXSIZE, &psz);
	mt_CompGetLGrect(app, n, CF_MAXSIZE, &nsz);

	{
		long pw = MAX( AXIS_SIZE(pwork) + delta, 0);
		long nw = MAX( AXIS_SIZE(nwork) - delta, 0);
		long pdw =   MIN( MAX( pw, AXIS_MINBOUND(psz) ), AXIS_MAXBOUND(p->bounds) ) - AXIS_SIZE(pwork);
		long ndw = - MIN( MAX( nw, AXIS_MINBOUND(nsz) ), AXIS_MAXBOUND(n->bounds) ) + AXIS_SIZE(nwork);
		delta = delta < 0 ? MAX( pdw, ndw ) : MIN( pdw, ndw );

		if ( delta ) {
			/* Note that the split doesn't really need to move when
			 * the l->size and r->size changes. This may be caused
			 * by unappropriate c->flex setting of other components
			 * around.
			 *
			 * One can always wrap those affected components
			 * by a one more to get the appropriate effect.
			 */
			p->size += delta;
			n->size -= delta;
			comp_reflow( c->parent, app);

			return delta;
		}

		return 0;
	}
}

static long comp_split_vmvd( COMPONENT *c, long delta, APPvar *app ) {
#undef AXIS_SIZE
#undef AXIS_MINBOUND
#undef AXIS_MAXBOUND
#define AXIS_SIZE(rect)  (rect.g_h)
#define AXIS_MAXBOUND(bounds)  (bounds.max_height)
#define AXIS_MINBOUND(bounds)  (bounds.g_h)

	COMPONENT *p = (COMPONENT *)listPrev( (LINKABLE*)c );
	COMPONENT *n = (COMPONENT *)listNext( (LINKABLE*)c );

	LGRECT pwork, nwork, psz, nsz;
	mt_CompGetLGrect(app, p, WF_WORKXYWH, &pwork);
	mt_CompGetLGrect(app, n, WF_WORKXYWH, &nwork);
	mt_CompGetLGrect(app, p, CF_MAXSIZE, &psz);
	mt_CompGetLGrect(app, n, CF_MAXSIZE, &nsz);

	{
		long pw = MAX( AXIS_SIZE(pwork) + delta, 0);
		long nw = MAX( AXIS_SIZE(nwork) - delta, 0);
		long pdw =   MIN( MAX( pw, AXIS_MINBOUND(psz) ), AXIS_MAXBOUND(p->bounds) ) - AXIS_SIZE(pwork);
		long ndw = - MIN( MAX( nw, AXIS_MINBOUND(nsz) ), AXIS_MAXBOUND(n->bounds) ) + AXIS_SIZE(nwork);
		delta = delta < 0 ? MAX( pdw, ndw ) : MIN( pdw, ndw );

		if ( delta ) {
			/* Note that the split doesn't really need to move when
			 * the l->size and r->size changes. This may be caused
			 * by unappropriate c->flex setting of other components
			 * around.
			 *
			 * One can always wrap those affected components
			 * by a one more to get the appropriate effect.
			 */
			p->size += delta;
			n->size -= delta;
			comp_reflow( c->parent, app);

			return delta;
		}

		return 0;
	}
}

static void __CDECL comp_split_click( COMPONENT *c, long *buff, APPvar *app ) {
	COMPONENT *p = (COMPONENT *)listPrev( (LINKABLE*)c );
	COMPONENT *n = (COMPONENT *)listNext( (LINKABLE*)c );
	short but, dum;

	if ( !c->parent || !p || !n ) return;

	mt_wind_update( BEG_MCTRL, app->aes_global);
	if( app->aes4 & AES4_XGMOUSE) mt_graf_mouse( M_SAVE, 0L, app->aes_global);
	mt_graf_mouse( FLAT_HAND, NULL, app->aes_global);

	if ( c->parent->type == CLT_HORIZONTAL ) {
		short cur, old = app->evnt.mx;
		do {
			mt_graf_mkstate( &cur, &dum, &but, &dum, app->aes_global);
			if ( cur != old ) {
				long delta = cur - old;
				delta = comp_split_hmvd( c, delta, app );
				if ( !delta ) continue; /* didn't move -> continue */

				old += delta;
				mt_CompEvntRedraw( app, c->parent);
			}
		} while( but);
	} else {
		short cur, old = app->evnt.my;
		do {
			mt_graf_mkstate( &dum, &cur, &but, &dum, app->aes_global);
			if ( cur != old ) {
				long delta = cur - old;
				delta = comp_split_vmvd( c, delta, app );
				if ( !delta ) continue; /* didn't move -> continue */

				old += delta;
				mt_CompEvntRedraw( app, c->parent);
			}
		} while( but);
	}

	if( app->aes4 & AES4_XGMOUSE && mt_graf_mouse( M_RESTORE, 0L, app->aes_global))
		mt_graf_mouse(ARROW,NULL,app->aes_global);
	mt_wind_update( END_MCTRL, app->aes_global);
}


COMPONENT *comp_split_create( APPvar *app, int mode, int sz ) {
	COMPONENT *c = mt_CompCreate( app, CLT_VERTICAL, sz, 0 );
	mt_CompEvntAttach( app, c, WM_XBUTTON, comp_split_click);
	return c;
}

