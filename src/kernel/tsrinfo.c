/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * Just a GEM dialog showing the installation of the LDG kernel
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
 * $Id: tsrinfo.c 9 2005-07-25 19:37:25Z bercegeay $
 */

#define __GEMLIB_OLDNAMES	/* Activer les vieux noms GEMLIB */

#include <string.h>
#include <gem.h>
#include "tsrinfo.rsh"
#include "tsrinfo.rh"

#define dialog rs_trindex[TSRINFO]
static short obx, oby, obw, obh;

void message( char *msg) {
	strcpy( dialog[TSRMESAG].ob_spec.free_string, msg);
	objc_draw( dialog, 0, 1, obx, oby, obw, obh);
	evnt_timer( 1500);
}

void start_info( char *version) {
	int i;
	appl_init();
	for( i=0; i<NUM_OBS; i++)
		rsrc_obfix(& rs_object[i],0);
	strcpy( dialog[TSRVERSION].ob_spec.free_string, version);
	form_center( dialog, &obx, &oby, &obw, &obh);
	form_dial( FMD_START, 0, 0, 0, 0, obx, oby, obw, obh);
	/* 	form_dial( FMD_GROW, 0, 0, 0, 0, x, y, w, h); */
	while( !wind_update( BEG_MCTRL));
	objc_draw( dialog, 0, MAX_DEPTH, obx, oby, obw, obh);
}

void end_info( void) {
	form_dial( FMD_FINISH, 0, 0, 0, 0, obx, oby, obw, obh);
	wind_update( END_MCTRL);
	appl_exit();
}

