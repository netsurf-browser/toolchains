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
 * $Source: /cvsroot/windom/windom/examples/frame/wdiff.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/23 19:23:48 $
 *   $Revision: 1.14 $
 */

#ifdef __PUREC__
#include <tos.h>
#else
#include <osbind.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "windom.h"

#include "dragdrop.h"
#include "viewtext.h"	/* A WinDom wrapper to display
						 * simple ascii file in a window */

#define DT_ANIM 0x414e494d  /* 'ANIM' */


/* from src/comp_widget.c */
COMPONENT *comp_widget_create( APPvar *a, WINDOW *win, int size, int flex );

/* from src/comp_split.c */
COMPONENT *comp_split_create( APPvar *a, int mode, int sz );

 
/* temporary demonstration define */
#define CDT_COLOR 0x434f4cUL  /* 'COL' */

static
void __CDECL color_box_redraw( COMPONENT *c, long buff[8]) {
	INT16 xy[4];
	LGRECT work;

	// get the work rectangle...
	mt_CompGetLGrect(&app, c, WF_WORKXYWH, &work);

	// fill in the array for v_bar (leave clipping to upper layer)
	xy[0] = work.g_x;
	xy[1] = work.g_y;
	xy[2] = xy[0] + work.g_w - 1;
	xy[3] = xy[1] + work.g_h - 1;

	// set the color
	vsf_color( c->graf->handle, (long)mt_CompDataSearch(&app, c, CDT_COLOR) );

	// draw the contents
	v_bar( c->graf->handle, xy);
}


static void ApTerm( WINDOW *win, short wbuff[8]) {
	while( wglb.first) {
		ApplWrite( WM_CLOSED, wglb.first->handle,0,0,0,0,0);
		EvntWindom( MU_MESAG);
	}
	FrameExit();	
	ApplExit();
	exit( 0);
}

static void IconDraw( WINDOW *win, short wbuff[8], void *data) {
	INT16 x,y,w,h;
	WINDOW **frames = (WINDOW**)data;
	int anim = (int)DataSearch( win, DT_ANIM);
	char *p;
	
	WindClear( win);
	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);

	switch( anim) {
	case 1:
	case 2:
		vst_color(  win->graf->handle, anim == 1 ? RED : BLUE);
		p = strrchr( frames[anim-1]->info, '\\');
		v_gtext( win->graf->handle, x+1, y+h/2, p?(p+1):"No file");
		break;
	default:
		vst_color(  win->graf->handle, BLACK);
		v_gtext( win->graf->handle, x+1, y+h/2, "Frames!");
		break;
	}
}

static void IconTimer( WINDOW *win, short buff[8]) {
	if( win->status & WS_ICONIFY) {
		int anim = (int)DataSearch( win, DT_ANIM);
		anim = anim ++ % 3;
		DataAttach( win, DT_ANIM, (void*)anim);
		snd_rdw( win);
	}
}

static void OpenText( WINDOW *win, short buff[8]) {
	char *filename;
	static char path[256];
	char name[64];
	if( (buff[5] & 0x02) && FselInput( path, name, "*.*", "Select an ascii file ...", NULL, NULL)) {
		filename = malloc(320);
		strcpy( filename, path);
		strcat( filename, name);
		
		ViewTextReCreate( win, filename);
		WindSetStr( win, WF_INFO, filename);
		snd_rdw( win);
	}
}

static void DragDrop( WINDOW *win, short wbuff[8]) {	
	if( !(win->status & WS_ICONIFY)) {
		char file[128], name[ 128], *buff;
		int dd_hdl, dd_msg; /* pipe-handle */
		long size;
		char ext[32];

		/* on accepte le drag&drop */
		dd_hdl = ddopen( wbuff[7], DD_OK);
		if( dd_hdl<0)	return;	/* erreur, pipe non ouvert */
				
		/*	On envoit notre liste de prefs:
		 *	Notre liste de prefs se borne au type ARGS
		 *	qui a une signification sp‚ciale: on attends
		 *	une liste de nom de fichiers dans le pipe
		 */
		memset( ext, 0, 32);
		strcpy( ext, "ARGS");
		dd_msg = ddsexts( dd_hdl, ext);
		if( dd_msg<0)	return;	/* erreur lecture du pipe */
		
		/* Lectures des infos re‡us */	
		dd_msg = ddrtry( dd_hdl, name, file, ext, &size);
		if( !strncmp( ext, "ARGS", 4))
		{
			/* on accepte les donn‚es */
			ddreply(dd_hdl, DD_OK);
			/* On lit les "size" octets des donn‚es dans le pipe */
			buff = (char*)malloc(sizeof(char)*(size+1));
			if( Fread( dd_hdl, size, buff) == size) {
				COMPONENT *fc;
				WINDOW *frame;
				
				buff[size] = 0;
				
				/* which frame the mouse is over? */
				fc = mt_CompFind( &app, mt_WindGetPtr(&app,win,WF_COMPONENT), evnt.mx, evnt.my);
				frame = !fc ? NULL : mt_CompDataSearch(&app,fc,WD_CCWD);
				if( frame) {					
					ViewTextReCreate( frame, buff);
					WindSetStr( frame, WF_INFO, buff);
					snd_rdw( frame);

					ddclose( dd_hdl);	/* fin du protocole */			 
					return;
				}
			} else {
				; /* erreur lecture pipe */
			}
			free( buff);
		}	
		else
			ddreply(dd_hdl, DD_NAK); 
					
		ddclose( dd_hdl);	/* fin du protocole */			 
	}
}

static void freeinfo( WINDOW *win, short buff[8]) {
	free( win->info);
}

int main( int argc, char *argv[]){
	WINDOW *win;
	WINDOW *frames[2];
	COMPONENT *c, *p;
	char *infoline;
		
	ApplInit();
	FrameInit();

	FormAlert(1,"[1][This example uses the|new experimental COMPONENT API|to manage its frames][continue]");

	/* Create the main frame window */
	win = WindCreate( NAME|MOVER|FULLER|SMALLER|CLOSER, app.x, app.y, app.w, app.h);

	/* component container */

	/* FIXME: needs to be ahead the other EvntAttach() registrations
	 *        as it sets the container's own ones
	 *
	 * HOW: This can be done once the WINDOW gets the top level COMPONENT
	 *      automaticaly */
	p = mt_CompCreate( &app, CLT_VERTICAL, 0, 1 );
	mt_WindSetPtr( &app, win, WF_COMPONENT, p, NULL);
	
	/* Create now each framed window */
	frames[0] = ViewTextCreate( (argc>1)?argv[1]:NULL, WAT_ALL-SIZER);
	infoline = malloc(320);
	strcpy( infoline, (argc>1)?argv[1]:"No file");
	WindSetStr( frames[0], WF_INFO, infoline);
	EvntAttach( frames[0], WM_XBUTTON, OpenText);
	EvntAdd( frames[0], WM_DESTROY, freeinfo, EV_TOP);
	mt_CompAttach( &app, p, comp_widget_create( &app, frames[0], 0, 1 ) );

	c = comp_split_create( &app, CS_HSLIDE, 3 );
	mt_CompEvntAttach( &app, c, WM_REDRAW, color_box_redraw );
	mt_CompDataAttach( &app, c, CDT_COLOR, (void*)LBLACK );
	mt_CompAttach( &app, p, c );
	
	frames[1] = ViewTextCreate( (argc>2)?argv[2]:NULL, WAT_ALL);
	infoline = malloc(320);
	strcpy( infoline, (argc>2)?argv[2]:"No file");
	WindSetStr( frames[1], WF_INFO, infoline);
	EvntAttach( frames[1], WM_XBUTTON, OpenText);
	EvntAdd( frames[1], WM_DESTROY, freeinfo, EV_TOP);
	mt_CompAttach( &app, p, comp_widget_create( &app, frames[1], 0, 1 ) );

	WindSetStr( win, WF_NAME, "Frames are usefull, aren't they?");
	EvntAttach( win, AP_DRAGDROP, DragDrop);
	EvntAttach( win, WM_XTIMER, IconTimer); 
	WindSetStr( win, WF_ICONTITLE, "WDiff");
	EvntDataAttach( win, WM_ICONDRAW, IconDraw, frames);

	/* Open the main frame at screen */
	WindOpen( win, 100, 100, 300, 300);

	{
		short wbuff[8];
		/* recompute the nested component sizes and positions */
		mt_WindGetGrect( &app, win, WF_CURRXYWH, (GRECT*)&wbuff[4] );
		wbuff[0] = CM_REFLOW;
		mt_EvntExec(&app, win, wbuff );
		/* FIXME: I don't like this at all here... should be inside WINDOM */
	}

	evnt.timer = 1000;
	
	EvntAttach( NULL, AP_TERM, ApTerm);
	for( ;; ) EvntWindom( MU_MESAG|MU_BUTTON|MU_KEYBD|MU_TIMER);
}

