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
 * $Source: /cvsroot/windom/windom/demo/setup.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/03/22 20:58:30 $
 *   $Revision: 1.7 $
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windom.h>
#include "demo.h"
#include "global.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/*
 *	Save parameters into the WinDom
 *  configuration file using ConfWrite()
 */

void setup_save( void) {
	MouseWork();
	switch( app.color) {
	case 2:
		ConfWrite( "demo.bg2", "%d,%d", param.desk->ob_spec.obspec.interiorcol, param.desk->ob_spec.obspec.fillpattern);
		break;
	case 4:
		ConfWrite( "demo.bg4", "%d,%d", param.desk->ob_spec.obspec.interiorcol, param.desk->ob_spec.obspec.fillpattern);
		break;
	case 16:
		ConfWrite( "demo.bg16", "%d,%d", param.desk->ob_spec.obspec.interiorcol, param.desk->ob_spec.obspec.fillpattern);
		break;
	default:
		ConfWrite( "demo.bg256", "%d,%d", param.desk->ob_spec.obspec.interiorcol, param.desk->ob_spec.obspec.fillpattern);
		break;
	}

	MouseWork();
	ConfWrite( "demo.options.desktop", "%s", (param.setup & SETUP_INSTALL_DESKTOP)?"true":"false");
	MouseWork();
	ConfWrite( "demo.options.autosave", "%s", (param.setup & SETUP_AUTOSAVE)?"true":"false");
	MouseWork();
	ConfWrite( "demo.options.windowsave", "%s", (param.setup & SETUP_SAVE_WINDOWS)?"true":"false");
	MouseWork();
	ConfWrite( "demo.options.frame.width", "%d", param.FrameWidth);
	MouseWork();
	ConfWrite( "demo.options.frame.color", "%d", param.FrameColor);

	graf_mouse( ARROW, 0L);
}

/*
 * Read setup in the WinDom configuration file
 * using ConfInquire().
 */

void setup_read( void) {
	char name[80], buf[128];
	int x, y, w, h, max, count;
	WINDOW *win=wglb.first;
	OBJECT *tree;

	/* fermer toutes les fenˆtres */
	
	while( wglb.first) {
		ApplWrite( _AESapid, WM_DESTROY, wglb.first->handle, 0, 0, 0, 0);
		EvntWindom( MU_MESAG);
	}
	
	ConfInquire("demo.options.desktop", "%B", &param.setup, SETUP_INSTALL_DESKTOP);	
	switch( app.color) {
	case 2:
		ConfInquire( "demo.bg2", "%d,%d",  &x, &y);
		param.desk->ob_spec.obspec.interiorcol = x;
		param.desk->ob_spec.obspec.fillpattern = y;
		break;
	case 4:
		ConfInquire( "demo.bg4", "%d,%d",  &x, &y);
		param.desk->ob_spec.obspec.interiorcol = x;
		param.desk->ob_spec.obspec.fillpattern = y;
		break;
	case 16:
		ConfInquire( "demo.bg16", "%d,%d",  &x, &y);
		param.desk->ob_spec.obspec.interiorcol = x;
		param.desk->ob_spec.obspec.fillpattern = y;
		break;
	default:
		ConfInquire( "demo.bg256", "%d,%d",  &x, &y);
		param.desk->ob_spec.obspec.interiorcol = x;
		param.desk->ob_spec.obspec.fillpattern = y;
		break;
	}
	
	ConfInquire( "demo.options.windowsave", "%B", &param.setup, SETUP_SAVE_WINDOWS);
	ConfInquire( "demo.options.autosave",   "%B", &param.setup, SETUP_AUTOSAVE);

	ConfInquire( "demo.options.frame.width", "%d", &param.FrameWidth);
	ConfInquire( "demo.options.frame.color", "%d", &param.FrameColor);
	max = 0;
	ConfInquire( "demo.window.max", "%d", &max);
	
	for( count = 1; count <= max; count++) {
		sprintf( name, "demo.window.%d.widget", count);
		ConfInquire( name, "%x", &x);
		win = WindCreate( x, app.x, app.y, app.w, app.h);
		if (win==NULL) continue;
		if( x & NAME) {
			sprintf( name, "demo.window.%d.title", count);
			ConfInquire( name, "%s", buf);
			WindSetStr( win, WF_NAME, strdup( buf));
		}
		if( x & INFO) {
			sprintf( name, "demo.window.%d.info", count);
			ConfInquire( name, "%s", buf);
			WindSetStr( win, WF_INFO, strdup( buf));
		}
		sprintf( name, "demo.window.%d.form", count);
		if( ConfInquire( name, "%d", &x) && x!=-1) {
			tree = get_tree( x);
			FormAttach( win, tree, (func_evnt)0L);
		}
		sprintf( name, "demo.window.%d.menu", count);
		if( ConfInquire( name, "%d", &x) && x!=-1) {
			tree = get_tree( x);
			WindSetPtr( win, WF_MENU, tree, NULL);
		}
		sprintf( name, "demo.window.%d.form", count);
		if( ConfInquire( name, "%d", &x) && x!=-1) {
			tree = get_tree( x);
			WindSetPtr( win, WF_TOOLBAR, tree, NULL);

		}
		sprintf( name, "demo.window.%d.effect", count);
		if( ConfInquire( name, "%b", &x) && x) {
			win -> status |= WS_GROW;
		}
		sprintf( name, "demo.window.%d.modal", count);
		if( ConfInquire( name, "%b", &x) && x) {
			win -> status |= WS_MODAL;
		}
		/* On fini par ouvrir la fenˆtre */
		sprintf( name, "demo.window.%d.xywh", count);
		ConfInquire( name, "%d,%d,%d,%d", &x, &y, &w, &h);
		WindOpen( win, x, y, w, h);
		EvntWindom( MU_MESAG);
		/* Cas iconification */
		sprintf( name, "demo.window.%d.iconified", count);
		if( ConfInquire( name, "%b", &x) && x) {
			WindSet( win,  WF_ICONIFY, -1, -1, -1, -1);
		}
	}
}


static void __CDECL conf_ok( WINDOW *, int, int, void*);
static void __CDECL conf_cancel( WINDOW *, int, int, void*);
static void __CDECL conf_apply( WINDOW *, int, int, void*);
static void __CDECL conf_popcolor( WINDOW *, int, int, void*);
static void __CDECL conf_popstyle( WINDOW *, int, int, void*);

static struct _param copypar;		/* Parameters are locally store in this */

/*
 * Create and initalize the formular of configuration.
 * This example uses the new WinDom function ObjcAttach()
 * where it is perticulary approriate because many variables
 * can be linked to objects.
 */
 
void create_form_conf( void) {
	WINDOW *conf;
	OBJECT *tree;

	/* Create the formular */
	tree = get_tree( CONFIGURE);
	conf = FormCreate( tree, 			/* formular object tree */
					   WAT_FORM,		/* window widgets */
					   (func_evnt)0L,			/* No event function ! */
					   get_string(CONF_TITLE),	/* window title */
					   NULL,			/* center at screen */
					   1,				/* graphic effects ON */
					   0);				/* no duplication */

	copypar = param;
	
	/* link objects to variables */
	ObjcAttachBit( OC_FORM, conf, CONF_INSTALL , &copypar.setup, SETUP_INSTALL_DESKTOP);
	ObjcAttachBit( OC_FORM, conf, CONF_WINDSAVE, &copypar.setup, SETUP_SAVE_WINDOWS);
	ObjcAttachBit( OC_FORM, conf, CONF_AUTOSAVE, &copypar.setup, SETUP_AUTOSAVE);
	
	/* link objects to function */
	ObjcAttachFormFunc( conf, CONF_OK,  	   conf_ok, NULL);
	ObjcAttachFormFunc( conf, CONF_CANCEL,     conf_cancel, NULL);
	ObjcAttachFormFunc( conf, CONF_APPL,	   conf_apply, NULL);
	ObjcAttachFormFunc( conf, CONF_BTRAME,     conf_popstyle, NULL);
	ObjcAttachFormFunc( conf, CONF_BTRAME_POP, conf_popstyle, NULL);
	ObjcAttachFormFunc( conf, CONF_FCOL,	   conf_popcolor, NULL);
	ObjcAttachFormFunc( conf, CONF_FCOL_POP,   conf_popcolor, NULL);
	ObjcAttachFormFunc( conf, CONF_BCOL,	   conf_popcolor, NULL);
	ObjcAttachFormFunc( conf, CONF_BCOL_POP,   conf_popcolor, NULL);
	
	/* Some object inits */
	tree[CONF_BCOL_POP].ob_spec.obspec.interiorcol 	 = param.desk->ob_spec.obspec.interiorcol;
	tree[CONF_BTRAME_POP].ob_spec.obspec.fillpattern = param.desk->ob_spec.obspec.fillpattern;
	tree[CONF_FCOL_POP].ob_spec.obspec.interiorcol   = copypar.FrameColor;
#ifdef __GNUC__
	sprintf( ObjcString( tree, CONF_FEPAI, NULL), "%d", param.FrameWidth);
#else
	itoa( param.FrameWidth, ObjcString( tree, CONF_FEPAI, NULL), 10);
#endif
}

/* Next, comes the function linked to objects */

/* sub function */

static void apply_settings(void) {
	OBJECT *tree = get_tree( CONFIGURE);
	param = copypar;
	param.desk->ob_spec.obspec.interiorcol = tree[CONF_BCOL_POP].ob_spec.obspec.interiorcol;
	param.desk->ob_spec.obspec.fillpattern = tree[CONF_BTRAME_POP].ob_spec.obspec.fillpattern;
	param.FrameWidth = atoi( ObjcString( tree, CONF_FEPAI, NULL));
	param.FrameColor = tree[CONF_FCOL_POP].ob_spec.obspec.interiorcol;
}

/* button OK */
static void __CDECL conf_ok( WINDOW *win, int obj, int mode, void *data) {
	/* Close the form */
	ApplWrite( _AESapid, WM_CLOSED, win->handle, 0, 0, 0, 0);
	/* change state to normal */
	ObjcChange( OC_FORM, win, obj, NORMAL, FALSE);
	/* apply settings */
	apply_settings();
}

/* button APPLY */
static void __CDECL conf_apply( WINDOW *win, int obj, int mode, void *data) {
	/* apply settings */
	apply_settings();
	/* change state to normal and redraw the object */
	ObjcChange( OC_FORM, win, obj, NORMAL, TRUE);	
}

/* button CANCEL */
static void __CDECL conf_cancel( WINDOW *win, int obj, int mode, void *data) {
	/* Close the form */
	ApplWrite( _AESapid, WM_CLOSED, win->handle, 0, 0, 0, 0);
	/* normal state for the button */
	ObjcChange( OC_FORM, win, obj, NORMAL, FALSE);	
}

/* color popup */
static void __CDECL conf_popcolor( WINDOW *win, int obj, int mode, void *data) {
	OBJECT *tree;
	INT16 x, y, res;
	
	tree = get_tree( COLORS);
	objc_offset( FORM( win), obj, &x, &y);
	res = MenuPopUp( tree, 
					 x, y, 
					 0, 0, 
					 FORM( win)[obj].ob_spec.obspec.interiorcol+1,
					 P_WNDW);
	
	if( res > 0 && tree[res].ob_flags & SELECTABLE) {
		FORM( win)[obj].ob_spec.obspec.interiorcol = res-1;
		ObjcDraw( OC_FORM, win, obj, 1);
	}
}

/* style popup */
static void __CDECL conf_popstyle( WINDOW *win, int obj, int mode, void *data) {
	OBJECT *tree;
	INT16 x, y, res;
	
	tree = get_tree( TRAME);
	objc_offset( FORM( win), obj, &x, &y);
	res = MenuPopUp( tree, 
					 x, y, 
					 0, 0, 
					 FORM(win)[obj].ob_spec.obspec.fillpattern+1, 
					 P_WNDW);
	if( res > 0) {
		FORM(win)[obj].ob_spec.obspec.fillpattern = tree[ res].ob_spec.obspec.fillpattern;
		FORM(win)[obj].ob_state &= ~SELECTED;
		ObjcDraw( OC_FORM, win, obj, 1);
	}
}

/* End of file */
