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
 * $Source: /cvsroot/windom/windom/src/frame_init.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.8 $
 */

#include <stdlib.h>
#include "globals.h"
#include "sliders.h"
#include "sliders.rh"
#include "sliders.rsh"

OBJECT * get_frame_slider ( APPvar *app) {
	return app->priv->frame_slider;
}


/** Initialization of the frame environment
 *
 *  @param app application descriptor
 *
 *	This function initializes the frame environment. 
 *  The frame environ should be released with mt_FrameExit().
 *  
 *  @sa mt_FrameExit()
 *
 */

void mt_FrameInit( APPvar *app) {
	OBJECT *tree;
	int i;
	static int init = 1;
	
	if( init) {
		for( i=0; i<NUM_OBS; mt_rsrc_obfix( rs_object, i++, app->aes_global));
		init = 0;
	}
	
	if (app->priv->frame_slider != NULL)
		mt_ObjcFree(app, app->priv->frame_slider);
	
	app->priv->frame_slider	= mt_ObjcDup( app, rs_trindex[ FRAME_SLIDER], NULL);
	tree = app->priv->frame_slider;
	
	if( CONF(app)->fwidsize != -1)
		__frame_set_widget( app, CONF(app)->fwidsize);

	/* monochrome resolution case (or less than 16 colors) */
	if( app->color < 16 ) {
		tree[M_V_PG].ob_spec.obspec.fillpattern = 1;
		tree[M_H_PG].ob_spec.obspec.fillpattern = 1;		
	}
	
	objc_extended( app, RSRC_XALL, tree);
}


/** release the frame environment
 *
 *  @param app application descriptor
 *
 *  This function release the memory used by the frame environment.
 *
 *  @sa mt_FrameInit()
 */

void mt_FrameExit( APPvar *app) {
	OBJECT *tree = app->priv->frame_slider;	
	
	if (tree) {
		objc_extfree(app,tree);
		mt_ObjcFree(app,tree);
	}
}
