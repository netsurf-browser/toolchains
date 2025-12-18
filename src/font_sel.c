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
 * $Source: /cvsroot/windom/windom/src/font_sel.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/01/12 04:05:45 $
 *   $Revision: 1.13 $
 */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "scancode.h"
#include "globals.h"

/* TODO: a (good?) idea would be to replace this internal resource
   by some lines of DFRM code to dynamically build the OBJECT tree. */
#include "fontsel.h"
#include "fontsel.rh"
#include "fontsel.rsh"


struct fonte {
	char name[33];
	int id; 
};
	
static void fill_field( OBJECT *tree, struct fonte *list, int pos, int max) {
	int i;
	char buf[33];
	
	for( i = FONT1; i <= FONT8; i++) {
		if( i-FONT1+pos <= max) {
			if( i-FONT1+pos == 0)
				strcpy( buf, "system font");
			else
				strcpy( buf, list[i-FONT1+pos].name);
			strncpy( tree[i].ob_spec.free_string, buf, 30);
			tree[i].ob_spec.free_string[30]='\0';
		} else
			strcpy( tree[i].ob_spec.free_string, "");
	}
}

/* userdraw subroutine to draw the "example" text (for a preview of the font) */
static
void __CDECL font_draw( WINDOW *win, PARMBLK *pblk, void *data, APPvar *app) {
	v_gtext( win->graf->handle, pblk->pb_x+5,
			 pblk->pb_y+pblk->pb_h-5, data);
}

/**
 * @brief Builtin WinDom font selector
 *
 * @param app application descriptor,
 * @param winname window font selector title,
 * @param example text used to display the font or \b NULL,
 * @param flags a bit field, possible bit are :
 *   - \b VSTHEIGHT size in pixel unit (instead of point unit),
 *   - \b MONOSPACED display only non proportional fonts.
 * @param fontid identificator of the selected font,
 * @param fontsize size selected,
 * @param fontname font name filled in a 64-byte buffer,
 * @return 1 if the user selects a valid font , 0 otherwise.
 *
 * mt_FontSel() calls the internal WinDom font selector. The selector is 
 * displayed
 * in a modal window. The selector can be used even the system does not
 * support multiple fonts, in this case only the size can be changed.
 *
 * Before the call, the \e fontid and \e fontsize parameters can be
 * filled with a default font-id and size. If a null value is used, default
 * values are the system font and a size of 13 pixels. If the \e example
 * parameter is \b NULL, a default text is used to display the fonts.
 *
 * The window hosting fontselector catches the \b AP_TERM message : the 
 * function will terminate in the same manner than a user clic on the Cancel 
 * button and an \b AP_TERM message is emited again.
 *
 * \par Usage of selector
 *  - a single clic : select a font and display it,
 *  - a double clic : select a font and return this selection, the selector is closed,
 *  - field 'size' : font size are updated in real time,
 *  - up and down keys : browse the list font,
 *  - return or enter keys : shortcut of OK button,
 *  - undo key : shortcut of Cancel button.
 */
int mt_FontSel( APPvar *app, char *winname, char *example, int flags,        /* inputs  */
			                 int *fontid, int *fontsize, char *fontname) {	 /* outputs */
	OBJECT *tree;
	int res;
	INT16 x,y;
	int max, pos=0,
			 sel=0;
	int id, size;
	WINDOW *win;
	EVNTvar save;
	struct fonte *list;

	char dummy[70];
	int mono = -1, _index, i, v;
	
	short buff[8];

	static int init_fontsel = 0;

	/* internal resource */
	if( !init_fontsel) {
		for( res=0; res<NUM_OBS; mt_rsrc_obfix( rs_object, res++, app->aes_global));
		init_fontsel = 1;
	}
	
	/* Duplicate the "model" (which is a read only form) and works on the duplicata only */
	tree = mt_ObjcDup(app,rs_trindex[FONTSEL],NULL);
	
	/* Instal extended ob_type for sliders */
	objc_extended( app, RSRC_XALL, tree);

	win = mt_FormWindBegin( app, tree, winname);

	/* without Gdos, there's a font : "system font", id 0
 	 * with Gdos, the font #0 is the system font  */
	fontname[0] = '\0';
	mt_RsrcUserDraw( app, OC_FORM, win, FONT_USER, font_draw, example?example:fontname);
	
	id = *fontid;
	size = *fontsize;
	sel = -1;

	max = app->gdos; /* FIXME? mt_VstLoadFonts( app, 0) + app->work_out[10]; */
	list = (struct fonte *)malloc(sizeof(struct fonte)*(max+1));
	for( i=0; i<=max; i++) {
		if( flags & MONOSPACED) {
			mt_VqtXname( app, 0, i, dummy, &v);
			if( v & 0x2) {
				_index = ++mono;
				res = mt_VqtName( app, i, list[mono].name);
				list[mono].id = res;
			} else
				continue;
		} else {
			_index = i;
			res = mt_VqtName( app, i, list[i].name);
			list[i].id = res;
		}
		if( res == id) {
			sel = _index;
			mt_VstFont( app, id);
			strcpy( fontname, id?list[i].name:"system font");
		}
	}
	if( flags & MONOSPACED) max = mono;
	
	pos = MAX(MIN( sel, max-8),0);

	fill_field( tree, list, pos, max);
	if( (sel-pos) >= 0 && (sel-pos)<8)
		mt_ObjcChange( app, OC_FORM, win, sel-pos+FONT1, SELECTED, 0);

	if( size == 0)
		size = 10;
	sprintf( mt_ObjcString( app, tree, FONT_SIZE, NULL), "%d", size);
	if( flags & VSTHEIGHT) {
		vst_height( win->graf->handle, size, &x, &x, &x, &x);
		strcpy( tree[FONT_SIZE].ob_spec.tedinfo->te_ptmplt, " __ pxl" );
	} else
		vst_point( win->graf->handle, size, &x, &x, &x, &x);

	save = app->evnt;
	app->evnt.bclick = 2;
	app->evnt.bmask = 1;
	app->evnt.bstate = 1;
	
	do {
		res = mt_FormWindDoBuff( app, MU_MESAG|MU_KEYBD|FORM_EVNT, buff);

		/* manage events */
		if ( res & FORM_EVNT) {
			if ( res & MU_MESAG) 
			switch( buff[0]) {
			case AP_TERM:
				res = FONT_CANCEL;
				mt_snd_msg( app, NULL, AP_TERM, 0, 0, 0, 0);
				break;
			case WM_DESTROY:
				res = FONT_CANCEL;
				break;
			}
			if ( res & MU_KEYBD) {
				if ( ((app->evnt.keybd&0x00FF)>='0' &&  (app->evnt.keybd&0x00FF)<='9')
					 || (app->evnt.keybd&0x00FF) == 0x16 /*CONTROL-V*/)
					goto UPDATE;  /* TODO: i don't like so much GOTOs... */

				switch (app->evnt.keybd >> 8) {
				case SC_BACK:
				case SC_DEL:
				case SC_ESC:
UPDATE:				size = atoi( mt_ObjcString( app, tree, FONT_SIZE, NULL));
					if( flags & VSTHEIGHT)
						vst_height( win->graf->handle, size, &x, &x, &x, &x);
					else
						vst_point( win->graf->handle, size, &x, &x, &x, &x);
					mt_ObjcDraw( app, OC_FORM, win, FONT_DRAW, OC_MSG);
					break;
				case SC_UPARW:
				case SC_DWARW:
					if( (sel-pos) >= 0 && (sel-pos)<8)
						mt_ObjcChange( app, OC_FORM, win, sel-pos+FONT1, NORMAL, OC_MSG);
					sel = ((app->evnt.keybd >> 8)==SC_UPARW)?MAX(sel-1, 0):MIN(sel+1,max);
					strcpy( fontname, list[sel].name);
					id = list[sel].id;
					vst_font( win->graf->handle, id);
					if( flags & VSTHEIGHT)
						vst_height( win->graf->handle, size, &x, &x, &x, &x);
					else
						vst_point( win->graf->handle, size, &x, &x, &x, &x);
					mt_ObjcDraw( app, OC_FORM, win, FONT_DRAW, OC_MSG);
					sprintf( mt_ObjcString( app, tree, IDFONT, NULL), "%d", id);
					mt_ObjcDraw( app, OC_FORM, win, IDFONT, OC_MSG);
					if( (sel-pos) >= 0 && (sel-pos)<8)
						mt_ObjcChange( app, OC_FORM, win, sel-pos+FONT1, SELECTED, OC_MSG);
					if((app->evnt.keybd >> 8)==SC_UPARW) {
						if( sel < pos)
							res = FONT_UP;
					} else {
						if(  sel >= pos+8)
							res = FONT_DN;
					}
					break;
				}
			}
		}
		switch( res) {
		case FONT_UP:
			mt_ObjcChange( app, OC_FORM, win, res, SELECTED, 1);
			do {
				if( pos > 0) {
					if( (sel-pos) >= 0 && (sel-pos)<8)
						mt_ObjcChange( app, OC_FORM, win, sel-pos+FONT1, NORMAL, 0);
					pos --;
					if( (sel-pos) >= 0 && (sel-pos)<8)
						mt_ObjcChange( app, OC_FORM, win, sel-pos+FONT1, SELECTED, 0);
					
					fill_field( tree, list, pos, max);
					mt_ObjcDraw( app, OC_FORM, win, FONT_BACK, 2);
  				}
				mt_graf_mkstate( &x,&x,&y,&x, app->aes_global);
			} while( y);
			mt_ObjcChange( app, OC_FORM, win, res, NORMAL, 1);
			break;
		case FONT_DN:
			mt_ObjcChange( app, OC_FORM, win, res, SELECTED, 1);
			do {
				if( pos <= max-8 ) {
					if( (sel-pos) >= 0 && (sel-pos)<8)
						mt_ObjcChange( app, OC_FORM, win, sel-pos+FONT1, NORMAL, 0);
					pos ++;
					if( (sel-pos) >= 0 && (sel-pos)<8)
						mt_ObjcChange( app, OC_FORM, win, sel-pos+FONT1, SELECTED, 0);
					fill_field( tree, list, pos, max);
					mt_ObjcDraw( app, OC_FORM, win, FONT_BACK, 2);
				}
				mt_graf_mkstate( &x,&x,&y,&x, app->aes_global);
			} while( y);
			mt_ObjcChange( app, OC_FORM, win, res, NORMAL, 1);
			break;
		case FONT1:
		case FONT2:
		case FONT3:
		case FONT4:
		case FONT5:
		case FONT6:
		case FONT7:
		case FONT8:
			if( strlen( tree[res].ob_spec.free_string)) {
				if( sel != res-FONT1+pos) {
					strcpy( fontname, list[pos+res-FONT1].name);
					id = list[pos+res-FONT1].id;
					vst_font( win->graf->handle, id);
					mt_ObjcChange( app, OC_FORM, win, res, SELECTED, OC_MSG);
					if( (sel-pos) >= 0 && (sel-pos)<8)
						mt_ObjcChange( app, OC_FORM, win, sel-pos+FONT1, NORMAL, OC_MSG);
					sel = res-FONT1+pos;
					if( sel == 0)
						strcpy( fontname, "system font");
					sprintf( mt_ObjcString( app, tree, IDFONT, NULL), "%d", id);
					mt_ObjcDraw( app, OC_FORM, win, IDFONT, OC_MSG|7);
					mt_ObjcDraw( app, OC_FORM, win, FONT_DRAW, /*OC_MSG|*/7);
				}
				if( app->evnt.nb_click == 2)
					res = FONT_OK;
			}
			break;
		}
	} while ( res != FONT_OK && res != FONT_CANCEL);
	
	tree[res].ob_state &= ~SELECTED;
	if( (sel-pos) >= 0 && (sel-pos)<8)
		tree[sel-pos+FONT1].ob_state &= ~SELECTED;
	
	/* FIXME? mt_VstUnloadFonts( app, 0); */
	mt_FormWindEnd(app);
	app->evnt = save;
	
	free( list);

	/* free extended ob_types */
	objc_extfree(app, tree);
	
	/* free the duplicata of the "model" form */
	mt_ObjcFree(app,tree);
	
	*fontid = id;
	*fontsize = size;
	return (res == FONT_OK);
}
