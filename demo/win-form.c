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
 * $Source: /cvsroot/windom/windom/demo/win-form.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/03/22 20:58:31 $
 *   $Revision: 1.10 $
 */

#include <stddef.h>
#include <string.h>  /* needed for NULL definition */
#include <stdlib.h>
#include <windom.h>
#include "global.h"
#include "demo.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/*
 *  ABOUT WINDOM FORM:
 *  ------------------
 *
 * A Basic example, without real user action
 */

void create_info_form( void) {
	/* A simple informative formular */
	FormCreate( get_tree( DIAL1), 
					  		  WAT_FORM,
					  		  (func_evnt)0L,
					  		  get_string( INFO_TITLE), 
					  		  NULL, 
					  		  TRUE, FALSE);
}


/*  
 *  COMPLETE FORM WITH TAB AND OTHER STUFF
 *  --------------------------------------
 *
 *  Should be opened
 *  - normally
 *  - duplicated
 *  - modal form (duplicated to keep the original form unchanged)
 *
 */
 
static void __CDECL ftest( WINDOW *win, int obj, int mode, void *data) {
	short *vtest = data;
	
	if (vtest == NULL) return;
	
	/* reverse value of vtest */
	if (*vtest) *vtest=0; else *vtest=1;
	
	/* update object string */
	ObjcStrCpy( FORM(win), obj, *vtest?"yes":"no ");
	
	/* Here, we used the OC_MSG flags because 3D objects
	 * are drawn in transparency. This flags sends a
	 * redraw message around the object, then foreground
	 * objects are redrawn too. */
	ObjcDraw( OC_FORM, win, obj, OC_MSG);
}

/* Here an example of USERDRAW function */

static void __CDECL user( WINDOW *win, PARMBLK *pblk, void *data) {
	short vdih = win->graf->handle;
	vsf_color( vdih, LRED);
	v_circle( vdih, 
			  pblk->pb_x+pblk->pb_w/2, 
			  pblk->pb_y+pblk->pb_h/2,
			  20);
	vsf_color( vdih, LGREEN);
	v_circle( vdih, 
			  pblk->pb_x+pblk->pb_w/2, 
			  pblk->pb_y+10,
			  20);
	vsf_color( vdih, LBLUE);
	v_circle( vdih, 
			  pblk->pb_x+10, 
			  pblk->pb_y+pblk->pb_h/2,
			  20);	
}

/*
 * A formular showing all extended objet available with
 * WinDom.
 */

static void __CDECL widget_close( WINDOW *win, int obj, int mode, void *data) {
	ObjcChange( OC_FORM, win, obj, NORMAL, FALSE);
	ApplWrite( _AESapid, WM_CLOSED, win->handle, 0, 0, 0, 0);
}

static void __CDECL widget_busy( WINDOW *win, int obj, int mode, void *data) {
	int i;
	wind_update( BEG_MCTRL);
	for( i=0; i<50; i++) {
		MouseWork();
		evnt_timer( 100L);
	}
	graf_mouse( ARROW, 0L);
	wind_update( END_MCTRL);
	ObjcChange( OC_FORM, win, obj, NORMAL, TRUE);
}

static void __CDECL vtest_dstry( WINDOW *win, short buff[8], void *data) {
	UNUSED( buff);
	if (data) free(data);
}

static WINDOW *create_tab_form( int attr, char *title, int dup) {
	int but[] = {ONG1,ONG2,ONG3,ONG4};
	int thb[] = {FOND1,FOND2,FOND3,FOND4};
	int *vtest;
	
	/* So, create the formular ... */
	WINDOW *win = FormCreate( get_tree( DIAL2), attr, (func_evnt)0L, title, NULL, TRUE, dup);
	FormSave( win, OC_FORM);
	WindSetStr( win, WF_ICONTITLE, get_string( APP_NAME));
	WindSetPtr( win, WF_ICONDRAW, draw_color_icon, NULL);

	/* Handle the thumb indexes */
 	FormThumb( win, thb, but, 4);	
	
	/* Variable TEST data (vtest) */
	vtest = (int*)malloc( sizeof(int));
	EvntDataAdd( win, WM_DESTROY, vtest_dstry, vtest, EV_BOT);

	/* Bind some function to object */
	ObjcAttachFormFunc( win, DIAL2_OK, widget_close, NULL);
	ObjcAttachFormFunc( win, DIAL2_ANNUL, widget_close, NULL);
	ObjcAttachFormFunc( win, DIAL2_BUSY, widget_busy, NULL);

	/* Bind function that work on vtest variable */
	ObjcAttachFormFunc( win, DIAL2_TEST, ftest, vtest);
	
	/* An USERDRAW function */
	RsrcUserDraw( OC_FORM, win, USERDIAL, user, NULL);
	
	return win;
}

/*
void widget_dstry( WINDOW *win) {
	FormThbFree( win);
}
*/

void create_widget_form( int dup) {
	create_tab_form( WAT_NOINFO, get_string( FORM_TITLE), dup);
}


/* Because our modal window disables the desktop
 * menu, it has a special destroy function */

static void __CDECL close_modal( WINDOW *win, short buff[8]) {
	/* Enable the desktop menu	*/
	MenuEnable();
}

void create_modal_form( void) {
	WINDOW *win;
	
	/* create the form in window */
	win = create_tab_form(WAT_MFORM, get_string( MFORM_TITLE), 1);
	/* Set the window modal */
	WindSet( win, WF_BEVENT, BEVENT_MODAL, 0, 0, 0);
	/* New closer event function */
	EvntAdd( win, WM_CLOSED, close_modal, EV_TOP);
	/* Disable the desktop menu */
	MenuDisable();
}


/*
 *  QUIT: YES/NO FORM
 *  -----------------
 *
 *  Using a standard modal form.
 */

static void __CDECL quit_yes( WINDOW *win, int obj, int mode, void *data) {
	ObjcChange( OC_FORM, win, obj, NORMAL, FALSE);
	ApplWrite( _AESapid, WM_CLOSED, win->handle, 0, 0, 0, 0);
	ApplWrite( _AESapid, AP_TERM, 0, 0, 0, 0, 0);
}

static void __CDECL quit_no( WINDOW *win, int obj, int mode, void *data) {
	ObjcChange( OC_FORM, win, obj, NORMAL, FALSE);
	ApplWrite( _AESapid, WM_CLOSED, win->handle, 0, 0, 0, 0);
}

void create_quit_form( void) {
	WINDOW *win;
	
	win = FormCreate( get_tree( ALR_QUIT), WAT_MFORM, (func_evnt)0L, get_string( APP_NAME), NULL, TRUE, 0);
	ObjcAttachFormFunc( win, ALR1_OUI, quit_yes, NULL);
	ObjcAttachFormFunc( win, ALR1_NON, quit_no, NULL);
	
	/* Set the window modal */
	WindSet( win, WF_BEVENT, BEVENT_MODAL, 0, 0, 0);
	/* New closer event function */
	EvntAdd( win, WM_CLOSED, close_modal, EV_TOP);
	/* Disable the desktop menu */
	MenuDisable();
	MouseObjc( FORM(win), ALR1_OUI);
}


/* End of file */
