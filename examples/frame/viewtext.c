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
 * $Source: /cvsroot/windom/windom/examples/frame/viewtext.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:39:25 $
 *   $Revision: 1.7 $
 */

#include <scancode.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <mt_wndm.h>

#define DEBUG(s)	FormAlert(1,"[1][ligne %d|%s][ok]", __LINE__, s)
#define TESTATTR

#include "viewtext.h"

#define VT_UNIX 1
#define VT_DOS  2
#define VT_MAC  3

struct viewtext {
	char *buf;
	int maxline;
	char **line;
	int type;
};

#define WCAR	attr[6]
#define HCAR	attr[7]
#define WCELL	attr[8]
#define HCELL	attr[9]

#define BOLD_BEG "ÝB"
#define BOLD_END "Ýb"
#define ITAL_BEG "ÝI"
#define ITAL_END "Ýi"
#define UNDL_BEG "ÝU"
#define UNDL_END "Ýu"
#define NONE_END "Ý0"

#define BOLD 0x1
#define LIGH 0x2
#define ITAL 0x4
#define UNDL 0x8
#define OUTL 0x01

int mt_ViewTextGet( APPvar *app, WINDOW *win, int mode, int *val) {
	struct viewtext *data = mt_DataSearch( app, win, WTXT);
	if( data) {
		switch( mode) {
		case 1:
			*val = data->type;
			return 0;
		}
	}
	return -1;
}

static char* tab2spc( int tab, char *dest, char *src, int max) {
	int fill, pos = 0;
	char *beg = dest;

	max--;
	while( *src != '\0' && 
		   *src != '\n' &&
		   *src != '\r' && max) {

		if( *src == '\t') {
			
			fill = tab - (pos % tab);
			while( fill -- && max) {
				* dest++ = ' ';
				max --;
			}
			pos += tab - (pos % tab);
			src ++;
		} else {
			*dest++ = *src++;
			max--;
			pos++;
		}
	}
	*dest = '\0';
	return beg;
}

static void draw_text( WINDOW *win, short wbuff[8], APPvar *app ) {
	INT16 x, y, w, h, attr[10];
	INT16 i, hcell;
	char buf[256], *p, *q;
	struct viewtext *ptext = mt_DataSearch( app, win, WTXT);
	static int next_attr = 0;
	int xpospix, xpos;
	
	mt_WindGet( app, win, WF_WORKXYWH, &x, &y, &w, &h);
	h += y-1;

	/* On affiche le fond de la fenˆtre */
	mt_WindClear( app, win);

	/* Le texte maintenant */
	vswr_mode( win->graf->handle, MD_TRANS);
	vqt_attributes( win->graf->handle, attr);
	hcell = HCELL;
	w = HCELL - HCAR;  		/* d‚calage vertical du texte */
	for( i=(int)win->ypos; i<ptext->maxline ; i++)	/* ptext->maxline == win->ypos_max		*/
	{												/* win->ypos_max est la valeur maximale	*/
		y += hcell;									/* admissible de win->ypos */
		if( y < win->graf->clip.g_y)
			continue;  /* Ici on sort de la zone de clippage
						* donc pour optimiser le redraw on s'arrˆte ici */
		tab2spc( 4, buf, ptext->line[i], 255);

#ifdef TESTATTR
		xpospix = x;
		xpos = (int)win->xpos;
		q = p = buf;
		while( *p) {
			if( (q = strstr( p, BOLD_BEG))!=NULL) {
				next_attr |= BOLD;
				*q = '\0';
				q += strlen(BOLD_BEG);
			} else if( (q = strstr( p, BOLD_END))!=NULL) {
				next_attr &= ~BOLD;			
				*q = '\0';
				q += strlen(BOLD_END);
			} else if( (q = strstr( p, ITAL_BEG))!=NULL) {
				next_attr |= ITAL;
				*q = '\0';
				q += strlen(ITAL_BEG);
			} else if( (q = strstr( p, ITAL_END))!=NULL) {
				next_attr &= ~ITAL;			
				*q = '\0';
				q += strlen(ITAL_END);
			} else if( (q = strstr( p, UNDL_BEG))!=NULL) {
				next_attr |= UNDL;
				*q = '\0';
				q += strlen(UNDL_BEG);
			} else if( (q = strstr( p, UNDL_END))!=NULL) {
				next_attr &= ~UNDL;
				*q = '\0';
				q += strlen(UNDL_END);
			} else
				q = p + strlen(p);
			
			/* if( strlen( p) > win->xpos) */
			/* la ligne est-elle dans la fenˆtre? */
			/*	v_gtext(win->graf->handle, x, y - w, p + win->xpos); */

			if( strlen(p) > xpos) {
				v_gtext(win->graf->handle, xpospix, y - w, p + xpos);
				vqt_extent( win->graf->handle, p + xpos, attr);
				xpospix += (attr[2] - attr[0]);
			}
			vst_effects( win->graf->handle, next_attr);
			xpos -= (int)strlen(p);
			xpos = MAX(xpos,0);
			p += (q-p);
		}
#else
		if( strlen(buf) > win->xpos)
			v_gtext(win->graf->handle, x, y - w, buf + win->xpos);
#endif
		if (y > MIN(h, win->graf->clip.g_y + win->graf->clip.g_h-1)) /* sort-on de la zone clipp‚e ? */
			break;
		
	}
}

static void del_text( WINDOW *win, short wbuff[8], APPvar *app) {
	struct viewtext *text = mt_DataSearch( app, win, WTXT);
	if( text) {
		mt_DataDelete( app, win, WTXT);
		free( text->buf);
		free( text);
		free( win->info);
	}
}

/* pourrait ˆtre standard il me semble */

static void __CDECL keybd_text( WINDOW *win, short wbuff[8], APPvar *app) {
	int shifted = app->evnt.mkstate & (K_LSHIFT|K_RSHIFT); 
	wbuff[0] = 0;

	switch (app->evnt.keybd>>8) {
	case SC_HOME:
		wbuff[0] = WM_VSLID;
		wbuff[4] = shifted ? 1000 : 0;
		break;
	case SC_PGUP:
		wbuff[0] = WM_UPPAGE;
		break;		
	case SC_PGDN:
		wbuff[0] = WM_DNPAGE;
		break;		
	case SC_UPARW:
		wbuff[0] = shifted ? WM_UPPAGE : WM_UPLINE;
		break;
	case SC_DWARW:
		wbuff[0] = shifted ? WM_DNPAGE : WM_DNLINE;
		break;		
	case SC_LFARW:
		wbuff[0] = shifted ? WM_LFPAGE : WM_LFLINE;
		break;
	case SC_RTARW:
		wbuff[0] = shifted ? WM_RTPAGE : WM_RTLINE;
		break;
	default:
		switch (app->evnt.keybd) {
		case 0x1615:	/* C-U */
			mt_ApplWrite( app, mt_AppAESapid(app), WM_CLOSED, w_get_hndl(app, win), 0, 0, 0, 0);
			break;
		}
	}

	if ( wbuff[0] ) mt_EvntExec( app, win, wbuff);
}

static struct viewtext *TextBlank( void) {
	struct viewtext *text;

	text = (struct viewtext*) malloc(sizeof(struct viewtext));
	assert( text);
	text->buf = (char *) malloc( sizeof(char));
	assert( text->buf);
	text->buf[0] = '\0';
	text->maxline = 1;
	text->line = (char**) malloc(sizeof(char*)*text->maxline);
	assert( text->line);
	text->line[0] = text->buf;
	text->type = VT_UNIX;
	return text;
}

static struct viewtext *TextCreate( char *file) {
	FILE* fp;
	int dum;
	long size, nbread;
	struct viewtext *text;
	char *p;

	text = (struct viewtext*) malloc(sizeof(struct viewtext));
	assert( text);
	fp = fopen( file, "rb");
	if( !fp) return NULL;

	fseek( fp, 0, 2);
	size = ftell( fp);
	fseek( fp, 0, 0);
	text->buf = (char*)malloc(sizeof(char)*(size+1));
	assert( text->buf);
	nbread = fread( text->buf, sizeof(char), size, fp);
	fclose( fp);
	text->buf[nbread] = '\0';

	/* attribution des lignes */
	text->type = 0;
	for( p=text->buf,text->maxline=1; *p; p++) {
		if( *p == '\n') { /* unix */
			text->maxline ++;
			if( !text->type) text->type = VT_UNIX;
		} else
		if( *p == '\r' && *(p+1) == '\n') { /* dos */
			text->maxline ++;
			p++;
			if( !text->type) text->type = VT_DOS;
		} else
		if( *p == '\r') { /* mac */ 
			text->maxline ++; 
			if( !text->type) text->type = VT_MAC;
		}
	}

	text->line = (char**)malloc(sizeof(char*)*text->maxline);
	assert( text->line);
	text->line[0]=text->buf;
	
	for( p=text->buf, dum=0; *p;) {
		if( *p == '\n') { /* unix */
			text->line[++dum] = p+1;
			*p = '\0';
		} else
		if( *p == '\r' && *(p+1) == '\n') { /* dos */
			text->line[++dum] = p+2;
			*p = '\0';
			p ++;
		} else
		if( *p == '\r') { /* mac */ 
			text->line[++dum] = p+1;
			*p = '\0';
		}
		p ++;
	}
	return text;
}

WINDOW *mt_ViewTextCreate( APPvar *app, char *file, int attrib) {
	WINDOW *win;
	struct viewtext *text;
	INT16 attr[10];
	
	text = file?TextCreate( file):TextBlank();
	if( !text) return NULL;

	win = mt_WindCreate( app, attrib, app->x, app->y, app->w, app->h);
	mt_DataAttach( app, win, WTXT, text);
	mt_EvntAttach( app, win, WM_REDRAW, draw_text);
	mt_EvntAdd	  ( app, win, WM_DESTROY, del_text, EV_TOP);
	mt_EvntAttach( app, win, WM_XKEYBD, keybd_text);
	vqt_attributes( win->graf->handle, attr);
	win->w_u = WCELL;
	win->h_u = HCELL;
	win->ypos_max = text->maxline;
	win->xpos_max = 255;
	
	mt_WindSetStr( app, win, WF_NAME, strdup( file));
	mt_WindSlider( app, win, HSLIDER|VSLIDER);
	return win;
}

void mt_ViewTextReCreate( APPvar *app, WINDOW *win, char *file) {		
	struct viewtext *text;
	INT16 attr[10];

	text = file?TextCreate( file):TextBlank();
	if( text) {
		del_text( win, NULL, app);
		mt_DataAttach( app, win, WTXT, text);
		vqt_attributes( win->graf->handle, attr);
		win->xpos = win->ypos = 0;
		win->w_u = WCELL;
		win->h_u = HCELL;
		win->ypos_max = text->maxline;
		win->xpos_max = 255;
	
		mt_WindSetStr ( app, win, WF_NAME, strdup( file));
		mt_WindSlider( app, win, HSLIDER|VSLIDER);
		free( win->info);
	}
}

/*
 * x,y : -1 voir WindOpen()
 * w,h : -1 valeur minimale selon le texte
 */

void mt_ViewTextOpen( APPvar *app, WINDOW *win, int x, int y, int w, int h) {
	INT16 width, attr[10], dum;
	struct viewtext *text = mt_DataSearch( app, win, WTXT);
		
	vqt_attributes( win->graf->handle, attr);
	/* Calcul largeur & hauteur */
	if( w == -1) {
		INT16 w16;
		width = 0;
		for( dum = 0; dum<text->maxline; dum++)
			width = MAX( width, (int)strlen( text->line[dum]));
		width *= WCELL;
		width = MIN( width, app->w);
		mt_WindCalc( app, WC_BORDER, win, 0, 0, width, h, &dum, &dum, &w16, &dum);
		w = w16;
	}
	if( h == -1) {
		INT16 h16;
		width = text->maxline*HCELL;
		width = MIN( width, app->h);
		mt_WindCalc( app, WC_BORDER, win, 0, 0, w, width, &dum, &dum, &dum, &h16);
		h = h16;
	}
	w = MIN( w, app->w);
	h = MIN( h, app->h);	
	mt_WindOpen( app, win, x, y, w, h);
}

int mt_ViewTextSearch( APPvar *app, WINDOW *win, char *target, int lastpos, int capsens) {
	struct viewtext *ptext = mt_DataSearch( app, win, WTXT);
	int i;
	int found = 0;
	
	for( i = lastpos; i < ptext->maxline; i++) {
		if( capsens && !strcmp( ptext->line[i], target)) {
			found = 1;
			break;
		} else if( !stricmp( ptext->line[i], target)) {
			found = 1;
			break;
		}
	}
	if( found) {
		win->xpos = i;
		mt_snd_rdw( app, win);
		return i;
	} else
		return -1;
}

/* EOF */
