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
 * $Source: /cvsroot/windom/windom/examples/popup/ex_popup.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:39:25 $
 *   $Revision: 1.4 $
 */

#include <stdlib.h>
#include "windom.h"
#include "ex_popup.h"

/* Popup prefs */
unsigned int popflags = 0;

/*
 *	Close resources and quit cleany application
 */

static void __CDECL ApTerm( WINDOW *win, short buff[8]) {
	while( wglb.first) {
		ApplWrite( _AESapid, WM_DESTROY, wglb.first->handle,0,0,0,0); 
		EvntWindom( MU_MESAG);
	}
	RsrcXtype( 0, NULL, 0);
	RsrcFree();
	ApplExit();
	exit( 0);
}

/* close the form and quit app */

static void __CDECL close_and_exit( WINDOW *win, int index, int mode, void *data) {
	ObjcChange( mode, win, index, NORMAL, TRUE);
	ApplWrite( _AESapid, AP_TERM,0,0,0,0,0);
}

/* formular Prefs */

static void __CDECL do_prefs( WINDOW *win, int index, int mode, void *data) {
	OBJECT *frm;

	rsrc_gaddr( 0, PREFS, &frm);
	
	FormWindBegin( frm, "Popups : Prefs");
	FormWindDo( MU_MESAG);
	FormWindEnd( );
	
	ObjcChange( OC_FORM, win, index, NORMAL, TRUE);
}

/*
 * Example Popup 1 : with an object tree 
 */

static void __CDECL ex_popup_1( WINDOW *win, int index, int mode, void *data) {
	OBJECT *pop;
	short x, y;
	static int last_choice = -1;
	int choice;
	
	rsrc_gaddr( 0, POP1, &pop);
	objc_offset( FORM(win), index, &x, &y);

	choice = 
	MenuPopUp ( pop, 
			   x, y, 
			   -1, -1, 			/* Not used when data is object tree */
			   last_choice,	   	/* */ 
			   P_WNDW + P_CHCK );
	last_choice = choice;
	ObjcChange( OC_FORM, win, index, NORMAL, TRUE);
}

/*
 * Example Popup 2 : with a list of items 
 */

static void ex_popup_2( WINDOW *win, int index, int mode, void *data) {

	char *items[] = { "  My fair lady",
					  "  Summertime",
					  "  My funny Valentine" };
	short x, y;
	static int last_choice = -1;
	int choice;
	
	objc_offset( FORM(win), index, &x, &y);

	choice = 
	MenuPopUp ( items, 
			    x, y, 
			    3, 
			    -1,
			    last_choice,	   	/* */ 
			    P_LIST + P_WNDW + P_CHCK );
	last_choice = choice;
	ObjcChange( OC_FORM, win, index, NORMAL, TRUE);
}

/*
 * Example Popup 3 : with a list of items and a slider 
 */

static void ex_popup_3( WINDOW *win, int index, int mode, void *data) {

	char *items[] = { "  Maman, Papa",
					  "  Le vin",
					  "  Philistins",
					  "  Le vieux l‚on",
					  "  La ronde des jurons",
					  
					  "  A l'ombre du coeur de ma mŠre",
					  "  Le pornographe",
					  "  La femme d'hector",
					  "  Bonhomme",
					  "  Le cocu",
					  
					  "  Comme une soeur",
					  "  Le pŠre no‰l et la petite fille",
					  "  Les fun‚railles d'antan",
					  "  Le bistrot",
					  "  Embrasse-les tous",
					  
					  "  P‚n‚lope",
					  "  L'orage",
					  "  Le m‚cr‚ant",
					  "  Le verger du roi Louis",
					  "  La traŒtesse" };
	short x, y;
	static int last_choice = -1;
	int choice;
	
	objc_offset( FORM(win), index, &x, &y);

	choice = 
	MenuPopUp ( items, 
			   x, y, 
			   20, 
			   8,
			   last_choice,	   	/* */ 
			   P_LIST + P_WNDW + P_CHCK );
	last_choice = choice;
	ObjcChange( OC_FORM, win, index, NORMAL, TRUE);
}

/*
 *	Preference dialog
 */

int bgcolor  = DEFVAL;
int frcolor  = DEFVAL;
int bdcolor  = DEFVAL;
int bgpatt   = DEFVAL;

/* handle APPLY button */
static void __CDECL apply_pref( WINDOW *root, int index, int mode, void *data) {	
	ApplSet( APS_POPUPWIND, popflags & 0x1, 0, 0, 0);
	ApplSet( APS_POPUPSTYLE, bgcolor, bdcolor, frcolor, bgpatt);
	ObjcChange( OC_FORM, root, index, NORMAL, TRUE);
	ApplWrite( _AESapid, WM_DESTROY, root->handle, 0,0,0,0);
}

/* Handle the popup style selection */
static void __CDECL pop_style( WINDOW *win, int index, int mode, void *data) {
	OBJECT *tree;
	short x, y;
	int res;

	rsrc_gaddr( 0, POPSTYLE, &tree);
	objc_offset( FORM( win), index, &x, &y);
	res = MenuPopUp ( tree, x, y, -1, -1,
					  FORM( win)[index].ob_spec.obspec.fillpattern+1,
					  P_WNDW );
	if( res != -1)
		bgpatt = FORM( win)[index].ob_spec.obspec.fillpattern = res-1;
	ObjcDraw( OC_FORM, win, index, 1);
}

/* Handle the popup color selection */
static void pop_color( WINDOW *win, int index, int *value) {
	OBJECT *tree;
	short x, y;
	int res;

	rsrc_gaddr( 0, POPCOLOR, &tree);
	objc_offset( FORM( win), index, &x, &y);
	res = MenuPopUp ( tree, x, y, -1, -1,
					  FORM( win)[index].ob_spec.obspec.interiorcol+1,
					  P_WNDW );
	if( res != -1)
		*value = FORM( win)[index].ob_spec.obspec.interiorcol = res-1;
	ObjcDraw( OC_FORM, win, index, 0);
}

static void __CDECL pop_bgcolor( WINDOW *win, int index, int mode, void *data) {
	pop_color( win, index, &bgcolor);
}

static void __CDECL pop_bdcolor( WINDOW *win, int index, int mode, void *data) {
	pop_color( win, index, &bdcolor);
}

static void __CDECL pop_frcolor( WINDOW *win, int index, int mode, void *data) {
	pop_color( win, index, &frcolor);
}


/* create dialog */
static void __CDECL form_pref( WINDOW *root, int index, int mode, void *data) {
	OBJECT *tree;
	WINDOW *win;

	rsrc_gaddr( 0, PREFS, &tree);
	win = FormCreate( tree, WAT_FORM, NULL, "Popup prefs", NULL, TRUE, FALSE);
	ObjcAttachFormFunc( win, PREF_APPLY,  apply_pref, NULL);
	ObjcAttachFormFunc( win, PREF_CANCEL, stdFormClose, NULL);
	ObjcAttachBit( OC_FORM, win, BUT_WINDOW, &popflags, 0x1);
	ObjcAttachBit( OC_FORM, win, BUT_3D,     &popflags, 0x2);
	ObjcAttachFormFunc( win, POP_BGCOLOR, pop_bgcolor, NULL);
	ObjcAttachFormFunc( win, POP_BDCOLOR, pop_bdcolor, NULL);
	ObjcAttachFormFunc( win, POP_FRCOLOR, pop_frcolor, NULL);
	ObjcAttachFormFunc( win, POP_BGPATT,  pop_style, NULL);
	
	ObjcChange( OC_FORM, root, index, NORMAL, TRUE);
}

/* Handle the WINCONF button */

static void __CDECL call_winconf( WINDOW *win, int index, int mode, void *data) {
	int ret;
	
	ret = ConfWindom();
	if( ret == -1) FormAlert( 1, "[1][Impossible to launch|WinConf][OK]");
	ObjcChange( OC_FORM, win, index, NORMAL, TRUE);
}


/*
 *	Main Part
 */

int main( void) {
	OBJECT *frm;
	WINDOW *wfrm;
	
	/* Some initialization */
	ApplInit();
	RsrcLoad( "ex_popup.rsc");
	RsrcXtype( 1, NULL, 0);
	
	/* Create the main form */
	rsrc_gaddr( 0, FRM1, &frm);
	wfrm = FormCreate( frm, WAT_FORM, NULL, "Example of Popup", NULL, TRUE, FALSE);
	ObjcAttachFormFunc( wfrm, BUT_QUIT, close_and_exit, NULL);
	ObjcAttachFormFunc( wfrm, BUT_POP1, ex_popup_1, NULL);
	ObjcAttachFormFunc( wfrm, BUT_POP2, ex_popup_2, NULL);
	ObjcAttachFormFunc( wfrm, BUT_POP3, ex_popup_3, NULL);
	ObjcAttachFormFunc( wfrm, BUT_PREF, form_pref, NULL);
	ObjcAttachFormFunc( wfrm, BUT_WINCONF, call_winconf, NULL);
	
	/* Bind some events */
	EvntAttach( NULL, AP_TERM, ApTerm);
	EvntAdd	  ( wfrm, WM_DESTROY, ApTerm, EV_BOT);

	/* Other inits */
	rsrc_gaddr( 0, PREFS, &frm);
	{
	short a,b,c,d;
	ApplGet( APS_POPUPSTYLE, &a, &b, &c, &d);
	frm[POP_BGCOLOR].ob_spec.obspec.interiorcol = a;
	frm[POP_BDCOLOR].ob_spec.obspec.interiorcol = b;
	frm[POP_FRCOLOR].ob_spec.obspec.interiorcol = c;
   	frm[POP_BGCOLOR].ob_spec.obspec.fillpattern = d;
   	}
	
	/* Handle GEM events */
	for( ;;)	EvntWindom( MU_MESAG);
}

/* EOF */
