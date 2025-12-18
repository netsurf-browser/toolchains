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
 * $Source: /cvsroot/windom/windom/examples/slider/slider.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:39:25 $
 *   $Revision: 1.6 $
 */

#include <stdio.h>
#include <stdlib.h>
#include "windom.h"

#include "slider.h"

void *slid1, *slid2;

static void __CDECL ApTerm( WINDOW *win, short buff[8]) {
	while( wglb.first) {
		ApplWrite( _AESapid, WM_DESTROY, wglb.first->handle, 0,0,0,0);
		EvntWindom( MU_MESAG);
	}
	free( slid1);
	RsrcXtype( 0, NULL, 0);
	RsrcFree();
	ApplExit();	
	exit( 0);
}

static void F1Close( WINDOW *win, int index, int mode, void *data) {
	ApplWrite( _AESapid, AP_TERM, 0, 0,0,0,0);
	ObjcChange( OC_FORM, win, index, NORMAL, 1);
}

static void __CDECL doslid( WINDOW *win, int mode, short value, void *data) {
	sprintf( ObjcString( FORM(win), F1_FIELD, NULL), "%04ld", (long)value);
	ObjcDraw( mode, win, F1_FIELD, 1);
}

static void __CDECL doslid2( WINDOW *win, int mode, short value, void *data) {
	evnt_timer( 100);
	sprintf( ObjcString( FORM(win), C_VAL, NULL), "%2d", value);
	ObjcDraw( mode, win, C_VAL, TRUE);
}

static void F1tr( WINDOW *win, int ind, int mode, void *slid) {
	static int tr = 0;
	tr = 1 - tr;
	if( tr) SlidSetUpdat( slid, SLD_IMME);
	else	SlidSetUpdat( slid, SLD_DIFF);
}

int main( void) {
	OBJECT *tree;
	WINDOW *win;
	
	ApplInit();
	RsrcLoad( "slider.rsc");
	RsrcXtype( RSRC_XTYPE, NULL, 0);
	
	rsrc_gaddr( 0, FORM1, &tree);
	win = FormCreate( tree, NAME|MOVER|CLOSER, NULL, "Slider & WinDom", NULL, 1, 0);

	slid1 = SlidCreate( 0, 1000, 1, 10, 0, SLD_VERT, SLD_DIFF);
	SlidAttach( slid1, OC_FORM, win, S_UP, S_PG, S_SL, S_DN);
	SlidSetFunc( slid1, doslid, NULL);
	SlidSetSize( slid1, 300);
	ObjcAttachFormFunc( win, F1_TR, F1tr, slid1);

	slid2 = SlidCreate( 1, 99, 1, 10, 0, SLD_VERT, SLD_IMME);
	SlidAttach( slid2, OC_FORM, win, C_UP, -1, -1, C_DN);
	SlidSetFunc( slid2, doslid2, NULL);
	SlidSetValue( slid2, 10);
	
	ObjcAttachFormFunc( win, F1_CLOSE, F1Close, NULL);

	EvntAttach( NULL, AP_TERM, ApTerm);
	/* Boucle Principale */
	for( ;;) EvntWindom( MU_MESAG);
	
	/* to make gcc happy */
	return 0;
}
