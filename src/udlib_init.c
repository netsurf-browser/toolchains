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
 * $Source: /cvsroot/windom/windom/src/udlib_init.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.14 $
 */

#include <stdlib.h>
#include <string.h>
#include <ldg.h>

#include "globals.h"
#include "cicon.h"
#include "xobjc_interface.h"

static void * __CDECL app_malloc ( long size) {
#ifdef __SOZOBONX__
	return lalloc(size);
#else
	return malloc(size);
#endif
}

static void __CDECL app_free( void *addr) {
	free(addr);
}

static UDLIB_LIB *udlib_libfind( APPvar *app, const char *filename)
{
	UDLIB_LIB *udlib;
	
	listForEach( UDLIB_LIB*, udlib, &app->priv->udef_lib) {
		if (strcmp(udlib->filename,filename) == 0 )
			return udlib;
	}
	
	return NULL;
}

static UDLIB_LIB *load_lib( APPvar *app, const char *filename)
{
	UDLIB_LIB *udlib;
	LDG *ldg = NULL;
	long ret;
	int is_builtin = 0;
	
	if (strcmp(filename,"__builtin__")==0) {
		is_builtin = 1;
	} else {
/*		ldg = ldg_open( filename, app->aes_global);*/ /* FIXME: LDG disabled, add check of USERDEF.LDG API here */
		if (ldg == NULL)
			return NULL;
	}
	
	udlib = (UDLIB_LIB*) malloc (sizeof(UDLIB_LIB));
	if (udlib==NULL) {
		if (! is_builtin)
			ldg_close( ldg, app->aes_global);
		return NULL;
	}
	
	udlib->env = (APPENV*) malloc (sizeof(APPENV));
	if (udlib->env==NULL) {
		if (! is_builtin)
			ldg_close( ldg, app->aes_global);
		free(udlib);
		return NULL;
	}
	
	udlib->env->aes_global = app->aes_global;
	udlib->env->vdih = app->priv->udlib_vdih;
	udlib->env->nplanes = app->nplanes;
	udlib->env->app_malloc = app_malloc;
	udlib->env->app_free = app_free;
	udlib->env->udlib_data = NULL;
	
	if (is_builtin) udlib->type = udlib_internal;
	else            udlib->type = udlib_ldg;
	
	strncpy( udlib->filename, filename, 256);
	udlib->udlib_descriptor = ldg;
	
	if (is_builtin) {
		udlib->libinit 			= xobjc_libinit;
		udlib->udconfig 		= xobjc_udconfig;
		udlib->libexit 			= xobjc_libexit;
		udlib->objc_extended 	= xobjc_extended;
		udlib->objc_extfree 	= xobjc_extfree;
		udlib->objc_string 		= xobjc_string;
		udlib->objc_char 		= xobjc_char;
		udlib->objc_edit 		= xobjc_edit;
		udlib->objc_edpos 		= xobjc_get_edpos;
		udlib->objc_unextnd 	= xobjc_get_unextended;
	} else {
		udlib->libinit 			= ldg_find("libinit",ldg);
		udlib->udconfig 		= ldg_find("udconfig",ldg);
		udlib->libexit 			= ldg_find("libexit",ldg);
		udlib->objc_extended 	= ldg_find("extended",ldg);
		udlib->objc_extfree 	= ldg_find("extfree",ldg);
		udlib->objc_string 		= ldg_find("string",ldg);
		udlib->objc_char 		= ldg_find("char",ldg);
		udlib->objc_edit 		= ldg_find("edit",ldg);
		udlib->objc_edpos 		= ldg_find("get_edpos",ldg);
		udlib->objc_unextnd 	= ldg_find("get_unextended",ldg);
	}
	
	/* check for the existance of mandatory functions */
	if (udlib->libinit==NULL || udlib->libexit==NULL || udlib->objc_extended==NULL
	    || udlib->objc_extfree==NULL || udlib->objc_unextnd==NULL)
	{
		if (! is_builtin)
			ldg_close( ldg, app->aes_global);
		free(udlib);
		return NULL;
	}
	
	listInit(&udlib->ob_list);
	
	udlib->env->fixcicon.rs_object  = NULL;
	udlib->env->fixcicon.fix        = NULL;
	udlib->env->fixcicon.draw_cicon = cdecl_draw_cicon;
	
	ret = udlib->libinit(udlib->env);
	switch (ret) {
	case 1 : udlib->mode = udlib_simple; break;
	case 2 : udlib->mode = udlib_freedom; break;
	default: if (! is_builtin) ldg_close( ldg, app->aes_global);
	         free(udlib->env);
			 free(udlib);
			 return NULL;
	}
	
	if (udlib->env->fixcicon.rs_object) {
		udlib->env->fixcicon.fix = malloc( sizeof(XRSRCFIX));
		if (udlib->env->fixcicon.fix)
			mt_RsrcFixCicon( app, udlib->env->fixcicon.rs_object, udlib->env->fixcicon.num_obs,
			                 udlib->env->fixcicon.num_cib, udlib->env->fixcicon.rgb_palette,
							 (XRSRCFIX*)udlib->env->fixcicon.fix);
		else {
			if (! is_builtin)
				ldg_close( ldg, app->aes_global);
			free(udlib->env);
			free(udlib);
			return NULL;
		}
	}
	
	return udlib;
}

int udlib_reinit ( APPvar *app)
{
	UDLIB_LIB *udlib, *c;
	long nlib_loaded = 0;
	char *filename;
	
	c = (UDLIB_LIB*) listRewind( &app->priv->udef_lib);
	filename = udlib_ConfGetFirstLib(app);
	
	while ( filename) {
		if ( (udlib = udlib_libfind(app,filename)) == NULL ) {
			/* library not loaded: load it now */
			udlib = load_lib(app,filename);
			if (udlib) {
				listAppend( &c->lnk, &udlib->lnk);
				c = udlib;
				nlib_loaded++;
			}
		} else {
			c = udlib;
		}
		filename = udlib_ConfGetNextLib( filename);
	}
	
	return nlib_loaded;	
}
