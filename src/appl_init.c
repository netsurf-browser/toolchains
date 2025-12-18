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
 * $Source: /cvsroot/windom/windom/src/appl_init.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/02/25 03:14:53 $
 *   $Revision: 1.29 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "globals.h"
#include "toserror.h"
#include "fsel.h"

/* default configuration
 * 
 *  IMPORTANT:
 *  if you change this, don't forget to update the "default value" documented
 *  for the _config struct in globals.h
 */
static CONFIG default_config = {  
	0,								/* flags */
	72, 72, 						/* icon size */
	WHITE,FIS_SOLID, 8,				/* window background style */
	LTMFLY_COLOR|BLACK,				/* keyshort color */
	
	/* sizes are in point units (not pixel) */
	/* font, size, color, [small size] */
	{SYSFONT, 13, BLACK},			/* string */
	{SYSFONT, 13, BLACK},			/* button */
	{SYSFONT, 13, BLACK},			/* exit */
	{SYSFONT, 13, BLACK},			/* title */
	{SYSFONT, 13, DEFVAL},			/* xedit texte */
	{SYSFONT, 13, DEFVAL},			/* xedit label */
	BLACK,							/* xedit cursor color */
	{SYSFONT, 10, DEFVAL}, 8,		/* xlongedit */
	{SYSFONT, 10, DEFVAL}, 8,		/* xtedinfo */
	0,								/* Mono */
	LWHITE,							/* Color */
	3,								/* Menu effect */
	{BLACK, 13, 1},					/* Bubble GEM */
	WHITE, 2, BLACK, 0x100, DEFVAL,	/* Popup */
	CENTER, DEFVAL,
	NAME|MOVER,
	-1,								/* Frame Widget Size, -1 (default) */
	NULL							/* list of default userdef library */
	};

static EVNTvar default_evnt = {
	0L, 258, 3, 0,
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0 };

static void __CDECL std_ConfRead( WINDOW *win, short buff[8], APPvar *app) {
	mt_ConfRead(app);
}


static
int __CDECL app_desk_get( APPvar *app, WINDOW *win, int mode, GRECT *clip, INT16 *x, INT16 *y, INT16* w, INT16 *h ) {
	int res;
	if (clip) {
		GRECT gout;
		res = mt_wind_xget_grect( 0, mode, clip, &gout, app->aes_global);
		*x = gout.g_x;
		*y = gout.g_y;
		*w = gout.g_w;
		*h = gout.g_h;
	} else
		res = mt_wind_get( 0, mode, x, y, w, h, app->aes_global);
	return (res != 0) ? E_OK : ERROR;
}

static
int __CDECL app_desk_set( APPvar *app, WINDOW *win, int mode, int par1, int par2, int par3, int par4) {
	int res;

	void *v1 = (void*) (long) ( ((long)par1 << 16 ) | ((long)par2 & 0xFFFFL) ) ;
	func_evnt v2 = (func_evnt) (long) ( ((long)par3 << 16 ) | ((long)par4 & 0xFFFFL) ) ;

	/* app wide modes */
	switch( mode){
		case WF_MENU:
			/* the win->menu is updated in the mt_MenuBar() */
			if( v1 == NULL) {
				mt_MenuBar( app, app->menu, 0);
				mt_EvntDelete( app, NULL, MN_SELECTED);
			} else {
				mt_MenuBar( app, v1, 1);
				mt_EvntAttach( app, NULL, MN_SELECTED, v2);
			}

			return E_OK;

		case WF_MENUTNORMAL:
			return mt_menu_tnormal( app->menu, par1, par2, app->aes_global);

		case WF_HILIGHT:
			return wind_menu_set( app, app->priv->desk, mode, v1, v2);

		default:
			/* other desktop window modes */
			res = mt_wind_set( 0, mode, par1,par2,par3,par4, app->aes_global);
	}

	return (res != 0) ? E_OK : ERROR;
}

static
int __CDECL app_desk_calc( APPvar *app, WINDOW *win, int type, int x, int y, int w, int h,
		INT16 *xout, INT16 *yout, INT16 *wout, INT16 *hout) {
	/* FIXME: TODO: ask the WIDGET component for the deltas */
	/* FIXME: REMOVE: for now copy the input to the output rectangle */
	*xout = x;
	*yout = y;
	*wout = w;
	*hout = h;
	return E_OK;
}

static
int __CDECL app_desk_remove( APPvar *app, WINDOW *win ) {
	return E_OK;
}

static
int __CDECL app_desk_get_handle(  APPvar *app, WINDOW *win) {
	return win->handle;
}

static
WIND_HANDLERS app_desk_handlers = {
	app_desk_get_handle,
	app_desk_get,
	app_desk_set,
	app_desk_calc,
	app_desk_remove
};



static void
ApplReInit( APPvar * app) {
	INT16 dum;
	INT16 parm1, parm3, parm4;
	
	/* initialise private data */
	app->priv->pos = 0 ;					/* conf_getline.c */
	app->priv->first= 1;					/* debug_windom.c */
	app->priv->init_fontsel = 0;			/* font_sel.c */
	app->priv->slct= NULL;				/* fsel.c */
	app->priv->fsel= NULL;				/* fsel.c */
	app->priv->type_mxalloc= -1;			/* galloc.c */
	app->priv->__bubble_quit = -1;		/* global.c */
	app->priv->_menu_disabled = 0;		/* global.c */
	app->priv->bubble_index=NULL; 		/* global.c */
	app->priv->maxfontid=0;				/* global.c */
	app->priv->__confapp_buffer = NULL;	/* global.c */
	app->priv->__confdef_buffer = NULL; 	/* global.c */
	app->priv->__confgetline_pos = 0;
	app->priv->__av_status[0]=0;			/* global.c */
	app->priv->__av_status[1]=0;			/* global.c */
	app->priv->__av_status[2]=0;			/* global.c */
	app->priv->mouse = 0;				/* mouse_work.c: */
	app->priv->__list_obspec_dup = NULL;	/* obspec_mem.c */
	app->priv->xrsrc_init= 0;			/* xrsrc.c: */
	app->priv->frame_slider = NULL;     /* frame_init.c */
	app->priv->config = default_config;

	/* initialise public data */

	app->evnt = default_evnt;
	app->wglb.first = NULL;
	app->wglb.front = NULL;
	app->wglb.appfront = NULL;
	app->menu = NULL;
	
	app->aeshdl = mt_graf_handle( NULL, NULL, NULL, NULL, app->aes_global);
	vq_extnd( app->aeshdl, 0, app->work_out);
	app->color  = app->work_out[ 13];
	vq_extnd( app->aeshdl, 1, app->work_out);
	app->nplanes = app->work_out[ 4];

	/* No color palette with true color modes */
	if( app->nplanes > 8) {
		mt_ApplSet( app, APS_FLAG, FLG_NOPAL, 1, 0, 0);
		app->graf.palette = NULL;
	} else {
		app->graf.palette = (W_COLOR *) malloc( app->color * sizeof( W_COLOR));
		w_getpal( app, NULL);
	}
	mt_wind_get( 0, WF_WORKXYWH, &app->x, &app->y, &app->w, &app->h, app->aes_global);
	mt_graf_mouse( ARROW, 0L, app->aes_global);
	app->work_in[0] = Getrez() + 2;
	for( dum = 1; dum < 10; app->work_in[ dum ++] = 1);
	app->work_in[10] = 2;
	app->graf.handle = app->aeshdl;
	v_opnvwk( app->work_in, &app->graf.handle, app->work_out);
	mt_ClipOn( app, &app->graf, (GRECT*)&app->x );
	app->priv->_res = ( app->y+app->h-1 >=240)?1:0;
	CONF(app)->hicon = app->priv->_res?72:36;
	
	/* default filenames of userdef libraries */
	if ( mt_AppAESnumapps(app) == 1 )  /* singletasking OS */
		udlib_ConfAddLib(app,"windom\\userdef\\tos\\userdef.ldg",1);
	else /* mutlitasking OS */
		udlib_ConfAddLib(app,"windom\\userdef\\userdef.ldg",1);
	/* and last, the built-in userdef, usefull if no userdef.ldg is installed */
	udlib_ConfAddLib(app,"__builtin__",0);
	
	/* __windowlist is initialy empty */
	app->priv->__windowlist = NULL;
	
	/* __iconlist is initialy zero-ed (empty) */
	for( dum = 0; dum<MAX_ICON; app->priv->__iconlist[dum++]=0)
		;
	
	/* set AES4 flags depending on the system 
	 * the AES function mt_appl_getinfo() returns 1 when the information is available,
	 * and 0 otherwise
	 */
	app->aes4 = 0;

	if( mt_appl_getinfo( AES_MESSAGE, &parm1, &dum, &parm3, &dum, app->aes_global) == 1) {
		if( parm1 & 0x0002 )
			app->aes4 |= AES4_UNTOPPED;
		if( parm1 & 0x0040 )
			app->aes4 |= AES4_BOTTOM;
		if( (parm1 & 0x0080) && (parm1 & 0x0100) && (parm1 & 0x0200)) {
			app->aes4 |= AES4_ICONIFY;
			app->aes4 |= AES4_TOOLBAR;
		}
		if( parm3 & 0x0001 )	/* support des coordonn‚es */
			app->aes4 |= AES4_ICONIFYXYWH;
	}

	if( mt_appl_getinfo( AES_WINDOW, &parm1, &dum, &parm3, &dum, app->aes_global) == 1) {
		if( parm3 & 0x0001 )
			app->aes4 |= AES4_SMALLER;
		if( parm3 & 0x0002 )
			app->aes4 |= AES4_BOTTOMER;
		if( parm1 & 0x0020)
			app->aes4 |= AES4_BEVENT;
		if( parm1 & 0x0400)
			app->aes4 |= AES4_FIRSTAREAXYWH;
		if( parm1 & 0x1000)
			app->aes4 |= AES4_MENUBAR;
		if( parm1 & 0x2000)
			app->aes4 |= AES4_SETWORKXYWH;
	}
	if( mt_appl_getinfo( AES_PROCESS, &dum, &dum, &parm3, &dum, app->aes_global) == 1) {
		if( parm3 == 1) app->aes4 |= AES4_APPSEARCH;
	}
	/* Les fonctions fslx_() sont-elles dispos ? */
	if( mt_appl_getinfo( 7 /* AES_EXTENSION */, &parm1, &dum, &dum, &dum, app->aes_global) == 1) {
		if( parm1 & 0x0008) app->aes4 |= AES4_FSLX;
	}
	/* Mode extendues de graf_mouse dispo ? */
	if( mt_appl_getinfo( AES_MOUSE, &parm1, &dum, &dum, &dum, app->aes_global) == 1) {
		if( parm1 == 1) app->aes4 |= AES4_XGMOUSE;
	}
	/* Menu extensions */
	if( mt_appl_getinfo( AES_MENU, &dum, &dum, &dum, &parm4, app->aes_global) == 1) {
		if( parm4 == 1) app->aes4 |= AES4_MNSELECTED;
	}

	if( app->aes_global[0] >= 0x0340) { /* AES Falcon */
		app->aes4 |= AES4_UNTOPPED;
		app->aes4 |= AES4_BEVENT;
	}

	if( (mt_appl_getinfo( AES_NAES, &parm1, &dum, &dum, &dum, app->aes_global) == 1 && parm1) || vq_magx() >= 0x0310) 
		app->aes4 |= AES4_APPLCONTROL;
	
	app->gdos = vq_gdos() ? vst_load_fonts( app->graf.handle, 0)+app->work_out[10] : 0;
	app->avid = -1;
	app->ntree = -1;
#ifdef WINDOMDEBUG
	{
		FILE *fp;
		fp = fopen( LOGFILE, "w");
		if( fp) fclose( fp);
	}
#endif
	mt_ConfRead(app);

	/* create WINDOW* for handle 0 */
	app->priv->desk = mt_WindAttach( app, 0);
	/* remove the desktop WINDOW* from the window list */
	mt_RemoveWindow( app, app->priv->desk);
	/* WindSet/Get behaviour */
	mt_DataAttach( app, app->priv->desk, WD_WHLR, &app_desk_handlers ); /* attach the window handlers */
	app->priv->desk->status |= WS_CUSTOM; /* make the WindGet/WindSet/WindCalc use the handlers */

	/* Default binding */
	mt_EvntAttach( app, NULL, AP_TERM, std_apterm);
	mt_EvntAttach( app, NULL, AP_LOADCONF, std_ConfRead);
	mt_EvntAttach( app, NULL, FNT_CHANGED, std_fntchg);

	app->pipe = Galloc( PIPE_SIZE);
	
	/* userdef library */
	listInit( &app->priv->udef_lib);
	app->priv->udlib_garbage = 0;
	app->work_in[0] = Getrez() + 2;
	for( dum = 1; dum < 10; app->work_in[ dum ++] = 1);
	app->work_in[10] = 2;
	app->priv->udlib_vdih = app->aeshdl;
	v_opnvwk( app->work_in, &app->priv->udlib_vdih, app->work_out);
	
	/* initialisations that use objc_extended should go there,
	 * after udlib stuff are initialised.
	 */ 
	init_scroll_menu(app);
}

APPvar * __mt_ApplInit( short *global) {
	APPvar * app;
	
	app = (APPvar*)malloc(sizeof(APPvar));
	if (app == NULL) return NULL;
	
	app->aes_global = global;
	
	app->priv = (APPprivate*)malloc(sizeof(APPprivate));
	if (!app->priv) {
		free(app->aes_global);
		free(app);
		return NULL;
	}

	if ( mt_appl_init(app->aes_global) == -1) {
		free(app->aes_global);
		free(app->priv);
		free(app);
		return NULL;
	}

	ApplReInit( app);
	
	return app;
}

/**
 * @brief AES and WinDom initialization.
 *
 * @return an application descriptor. This variable is always
 *     the first parameter of all others WinDom functions.
 *
 * This function replaces the appl_init() AES function and should
 * be the first call of WinDom library. The AES
 * and WinDom environements are iniatilized. The windom configuration
 * file is read in order to setup the global windom variables.
 *
 * @sa my_ApplExit()
 */

APPvar * mt_ApplInit( void) {
	APPvar *app;
	short *global;
	
	global = (short*)malloc(15*sizeof(short));
	
	if (global==NULL)
		return NULL;
	
	app = __mt_ApplInit( global);
	
	if (app == NULL) {
		free( global);
		return NULL;
	}
	
	return app;
}
