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
 * $Source: /cvsroot/windom/windom/userdef/extended/xedit.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:52:21 $
 *   $Revision: 1.6 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef __PUREC__
#include <tos.h>
#else
#include <osbind.h>
#endif

#include "global.h"

#ifdef MODIF_XEDIT

#define XEDIT_REALLOC 128

/**  Check if the bloc size allocated for the XEDIT is big enough.
 *
 *  @param old size of the memory bloc
 *  @param len is the len of the text
 *  @return 0 if the bloc size if sufficient, or the new size of the memory
 *          block to reallocate.
 *
 */

size_t 
_xobjc_realloc_xedit( size_t old, size_t len) {
	size_t new;
	
	new = (len / XEDIT_REALLOC + 1) * XEDIT_REALLOC;
	if( new > old)
		return new;
	
	return 0;
}


/**  shift right the buffer 
 *  
 *  @param buf
 *
 */
 
static void 
shift_right( char *buf) 
{
	int l = (int)strlen(buf);
	
	buf += l;
	*(buf+1) = '\0';
	while( l) {
		*buf = *(buf-1);
		buf --;
		l --;
	}
}


/**  shift left the buffer 
 *  
 *  @param buf
 *
 */
 
static void 
shift_left( char *buf) 
{
	do
		*buf = *(buf+1);
	while (*buf++);
}

typedef struct {
        char *unshift;
        char *shift;
        char *capslock;
} MY_KEYTAB;

static int 
_keybd2ascii( int keybd, int shift) 
{
	MY_KEYTAB *key;
	key = (MY_KEYTAB *)Keytbl( (char*)-1, (char*)-1, (char*)-1);
	return (shift)?key->shift[keybd>>8]:key->unshift[keybd>>8];
}

static int 
scrap_txt_write( char *str, short *aes_global) 
{
	char scrpdir[255];
	long fh;
		
	if( !mt_scrp_read( scrpdir, aes_global))
		return 0;
	strcat( scrpdir, "\\scrap.txt");
	fh = Fcreate( scrpdir, 0);
	if( fh < 0) return  0;
	Fwrite( fh, strlen(str)+1, str);
	Fclose( fh);
	return 1;
}

static char *
scrap_txt_read( APPENV *env, int maxlen) 
{
	char scrpdir[255], *buf;
	long fh;
	long size;
	
	if( !mt_scrp_read( scrpdir, env->aes_global))
		return NULL;
	strcat( scrpdir, "\\scrap.txt");
	fh = Fopen( scrpdir, 0);
	if( fh < 0) return  NULL;
	size = Fseek( 0L, fh, SEEK_END);
	size ++; /* for the null char to end the string */
	if (size > maxlen) size = maxlen;
	Fseek( 0L, fh, SEEK_SET);
	buf = (char*)APP_MALLOC( size+1);
	Fread( fh, size-1, buf);
	buf[size] = 0;
	Fclose( fh);
	return buf;
}

static void 
clip_xedit_bloc( XOBJC_DATA *uldata, OBJECT *tree, int obj, int nclip, GRECT *grect_clip, GRECT *grect) 
{
	int ind;
	short tab[4];
	GRECT gclip;
	short vdih = uldata->vdih;
	
	for (ind=0; ind>nclip; ind++) {
		gclip = *grect;
		rc_intersect( &grect_clip[ind], &gclip);
		if (gclip.g_w && gclip.g_h) {
			grect_to_array( &gclip, tab);
			vs_clip( vdih, 1, tab);
			_xobjc_xedit_bloc( uldata, tree, obj, grect);
			vs_clip_off(vdih);
		}
	}
}


/*  Insert a character using the mask tedinfo->pvalid
 */
static void 
xedit_add_char( W_XEDIT *xedit, char val, APPENV *env) {
	char *p = xedit->line;
	char *pvalid = xedit->tedinfo->te_pvalid;
	int add = FALSE;
	
	switch( *pvalid) {
	case 'X':
	case 'H':		/* Hidden */
		add = TRUE;
		break;
	case 'x':
		val = toupper( val);
		add = TRUE;
		break;
	case '9':
		switch( val) {
		case '-':
		case '+':
			if( (xedit->curs && p[xedit->curs-1] == 'e')
				|| xedit->curs == 0) {
				if( p[xedit->curs] != '+' && p[xedit->curs] != '-')
					add = TRUE;
			}
			break;
		case '.':
		case 'e':
			if( strchr(p, val) == 0)
				add = TRUE;
			break;
		default:
			if( isdigit(val))
				add = TRUE;
		}
		break;

	case 'N': /* 0-9, A-Z and space */
		if( isdigit(val) || isupper(val) || val == ' ')
			add = TRUE;
		break;
	case 'n': /* 0-9, a-z and space */
		if( isdigit(val) || islower(val) || val == ' ')
			add = TRUE;		
		break;
 	case 'A': /* A-Z and space */
		if( isupper(val) || val == ' ')
			add = TRUE;
 		break;
	case 'a': /* a-z and space */
		if( islower(val) || val == ' ')
			add = TRUE;
		break;
	case 'F': /* Valid GEMDOS filename ? * */
		 if( isalnum(val) || strchr( ":-_*?", val))
		 	add = TRUE;
		 break;
	case 'P': /* Valid GEMDOS pathname ? * \ */
		 if( isalnum(val) || strchr( ":-_*?\\", val)) 
		 	add = TRUE;
		 break;
	case 'p': /* Valid GEMDOS pathame , \ */
		 if( isalnum(val) ||  strchr( ":-_\\", val)) 
		 	add = TRUE;
		 break;
	}

	if( add) {
		size_t size = _xobjc_realloc_xedit( xedit->size, strlen(p) + 2);
	
		if( size) {
			char *mem = (char*)APP_MALLOC(size);
			if (mem == 0L) return;
			memcpy( mem, xedit->line, size);
			APP_FREE(xedit->line);
			p = xedit->line = mem;
		}
				
		/* insert the char */
		shift_right( p + xedit->curs);
		p[xedit->curs] = val;
		xedit->curs ++;
	}
}

/*  objc_edit-like function that works for standard editable object, and for
 *  extended editable object (XEDIT)
 */

long 
_xobjc_edit( OBJECT *tree, long obj, long val, short *idx, long kind,
             long nclip, GRECT *grect_clip, APPENV *env ) 
{
	int res = 0;
	W_XEDIT *xedit;
	char *p;
	INT16 mod, dum;
	enum { RDRW_OFF, RDRW_ALL,RDRW_CUR, RDRW_RT} redraw = RDRW_OFF;
	GRECT grect;
	int ind;
	short vdih;
	XOBJC_DATA *uldata;
	GRECT gclip;

	xedit = (W_XEDIT *)tree[obj].ob_spec.userblk->ub_parm;
	uldata = xedit->wp_uldata;
	vdih = uldata->vdih;
	p = xedit->line;
	mt_objc_offset( tree, obj, &grect.g_x, &grect.g_y, env->aes_global);
	grect.g_w = tree[obj].ob_width;
	grect.g_h = tree[obj].ob_height;
	
	switch( kind) {
	case ED_CURS:
		/* test */
		xedit -> curs = *idx;
		break;
	case ED_BLC_OFF:
		if( xedit -> flags & XEDIT_BLOC_ON) {
			clip_xedit_bloc( uldata, tree, obj, nclip, grect_clip, &grect);
			xedit -> flags &= ~XEDIT_BLOC_ON;
		}
		break;
	case ED_BLC_START:
		xedit -> blcbeg = val;
		break;
	case ED_BLC_END:
		xedit -> flags |= XEDIT_BLOC_ON;
		xedit -> blcend = val;
		clip_xedit_bloc( uldata, tree, obj, nclip, grect_clip, &grect);
		break;
	case ED_INIT:	/* show the cursor */
		xedit -> flags |= XEDIT_CURS_ON;
		val = xedit->pos;
		/* position of the scroll */
		if( xedit->curs < xedit->pos)
			xedit->pos = xedit->curs;
		while( _xobjc_width_curs( uldata, xedit, xedit->curs) + 2 > tree[obj].ob_width)
			xedit->pos ++;
		redraw = ( val != xedit->pos)?RDRW_ALL:RDRW_CUR;
		*idx = xedit -> curs;
		break;
	case ED_END:	/* hide the cursor */
		xedit -> flags &= ~XEDIT_CURS_ON;
		redraw = RDRW_ALL;
		break;
	case ED_CHAR:	/* add a char */
		if( (xedit->flags & XEDIT_CURS_ON) ||
			(xedit->flags & XEDIT_BLOC_ON) )
		switch( val) {

		case 0x4700:	/* HOME : cursor go to the begining of the text */
			if( p[xedit->curs] != '\0') {
				xedit->curs = xedit->pos = 0;
				redraw = RDRW_ALL;
			}
			break;
		case 0x4737:	/* S-HOME : cursor go to end of the text */
			if( p[xedit->curs] != '\0') {
				redraw = RDRW_ALL;
				xedit->pos = xedit->curs = (int) strlen(p);
				xedit->pos = MAX( 0, xedit->pos-2);
			}	
		case 0x4B00:	/* LFARW : go 1 char to the left */
			if( xedit->curs > 0) {
				redraw = RDRW_CUR;
				xedit->curs --;
			}
			break;
		case 0x7300:	/* C-LFARTW : go 1 word to left */
			if( xedit->curs > 0) {
				xedit->curs --;
				/* skip escape */
				while( xedit->curs && p[xedit->curs] == ' ')
					xedit->curs --;
				/* skip word */
				while( xedit->curs && p[xedit->curs] != ' ')
					xedit->curs --;
				if( xedit->curs) xedit->curs ++;
				redraw = RDRW_ALL;
			}
			break;
		case 0x4D00:	/* RTARW : go 1 char to right */
			if( p[xedit->curs] != '\0') {
				redraw = RDRW_CUR;
				xedit->curs ++;
			}
			break;
		case 0x7400:	/* C-RTARW : go 1 word to right */
			if( p[xedit->curs] != '\0') {					
				/* skip word */
				while( p[xedit->curs] != '\0' && p[xedit->curs] != ' ')
					xedit->curs ++;
				/* skip escape */
				while( p[xedit->curs] != '\0' && p[xedit->curs] == ' ')
					xedit->curs ++;
				redraw = RDRW_ALL;
			/*	xedit->curs --; */
			}
			break;
		case 0x537F: 	/* DEL : delete the char right after the cursor */
			if( p[xedit->curs] != '\0')
				xedit->curs ++;
			/* TODO: if a bloc is selected, the bloc shall be deleted */
		case 0x0E08:	/* BACK : delete the char right before the cursor */
			if( xedit->curs > 0) {
				redraw = RDRW_RT;
				shift_left( p + xedit->curs-1);
				xedit->curs --;
				if( xedit->curs < xedit->pos) {
					redraw = RDRW_ALL;
					xedit->pos = MIN(xedit->pos-2,0);
				} 
			}
			/* TODO: if a bloc is selected, the bloc shall be deleted */
			break;

		case 0x011B:	/* ESC : empty the field, cursor at the begining of the text */
			redraw = RDRW_ALL;
			p[0] = '\0';
			xedit->curs = 0;
			xedit->pos = 0;
			break;

		default:

			/* other special keys */
			mt_graf_mkstate( &dum, &dum, &dum, &mod, env->aes_global);
			if( mod & K_CTRL) {

				switch( _keybd2ascii(val, mod & (K_LSHIFT+K_RSHIFT))) {
				case 'c':
					/* TODO: if a part of the text is selected
					 * then copy this bloc only (not the whole text)
					 */
					scrap_txt_write( p, env->aes_global);
					break;
				case 'v':
					{
						char *s, *q = scrap_txt_read(env, 127);

						if( q) {
							for( s=q; *s; s++)
								xedit_add_char( xedit, *s, env);
							APP_FREE( q);
							redraw = RDRW_ALL;
						}
					}
					break;			
				case 'k':
					p[xedit->curs] = '\0';
					redraw = RDRW_RT;
					break;
				}
			} else {
				/* insert a char */
				if( val & 0x00FF) {
					redraw = p[xedit->curs]?RDRW_RT:RDRW_CUR;
					if( xedit->flags & XEDIT_BLOC_ON ) {
						/* erase the bloc */
						clip_xedit_bloc( uldata, tree, obj, nclip, grect_clip, &grect);
						xedit->curs = xedit->blcbeg;
						xedit->flags &= ~XEDIT_BLOC_ON;
						xedit->flags |= XEDIT_CURS_ON;
						while( xedit->blcend >= xedit->blcbeg)
							shift_left( xedit->line + xedit->blcend --);
						redraw = RDRW_RT;
					}
					xedit_add_char( xedit, val, env);
				}
			}
		} /* switch( val>>8) */

		val = xedit->pos;
		/* manage the scroll */
		if( xedit->curs >= 0) {
			if( xedit->curs < xedit->pos)
				xedit->pos = xedit->curs;

			while( _xobjc_width_curs( uldata, xedit, xedit->curs) + 2 > tree[obj].ob_width)
				xedit->pos ++;
		}
		if( redraw != RDRW_OFF && val != xedit->pos)
			redraw = RDRW_ALL;

		*idx = xedit -> curs;
		break;
	} /* switch(kind) */

	/* redraw the object */
	if( redraw != RDRW_OFF) {
		switch( redraw) {
		case RDRW_ALL:
			val = _xobjc_width_curs( uldata, xedit, xedit->pos);
			grect.g_x += val - 1;
			grect.g_w -= val;
			break;
		case RDRW_CUR:
			val = _xobjc_width_curs( uldata, xedit, MAX(xedit->pos,xedit->curs-1));
			grect.g_x += val - 1;
			grect.g_w = _xobjc_width_curs( uldata, xedit, MAX(xedit->pos,xedit->curs+1)) - val + 1;
			break;
		case RDRW_RT:
			val = _xobjc_width_curs( uldata, xedit, MAX(xedit->pos,xedit->curs-1));
			grect.g_x += val - 1;
			grect.g_w -= val;
			break;
		case RDRW_OFF:
			/* should never happen */
			break;
		}
		
		for (ind=0; ind<nclip; ind++) {
			gclip = grect;
			rc_intersect( &grect_clip[ind], &gclip);
			if (gclip.g_w && gclip.g_h) {
				mt_objc_draw( tree, obj, 0, gclip.g_x, gclip.g_y, gclip.g_w, gclip.g_h, env->aes_global);
			}
		}
	}
	
	return res;
}

long  
_xobjc_get_edpos( OBJECT *tree, long idx, long x, long y, APPENV *env)
{
	XOBJC_DATA *uldata;
	W_XEDIT *xedit;
	int curs;
	
	xedit = (W_XEDIT*)tree[idx].ob_spec.userblk->ub_parm;
	uldata = xedit->wp_uldata;

	curs = (int)strlen(xedit->line);
	
	if(  _xobjc_width_curs( uldata, xedit, curs) < x) 
		return curs;
	
	curs = xedit->pos;	
	while( xedit->line[curs] && _xobjc_width_curs( uldata, xedit, curs++) < x)
		;
	
	return curs - 1 ;
}



#endif /* MODIF_XEDIT */
