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
 * $Source: /cvsroot/windom/windom/src/frame_create.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/03/14 19:40:35 $
 *   $Revision: 1.17 $
 */


#include <stdlib.h>
#include "globals.h"
#include "sliders.h"

#define TOPPED_ACTIV	0x0004	/* internal */  /* TODO: move this constant in global.h */

static void set_frame( INT16 xy[10], int x, int y, int w, int h) {
	xy[8]=xy[0]=x;
	xy[9]=xy[1]=y;
	xy[2]=x+w-1;
	xy[3]=y;
	xy[4]=xy[2];
	xy[5]=y+h-1;
	xy[6]=x;
	xy[7]=xy[5];
}

static void update_scrolls( APPvar *app, WINDOW *win) {
	INT16 x,y,w,h;
	INT16 xc,yc,wc,hc;
	
	mt_wind_get( win -> handle, WF_CURRXYWH, &xc, &yc, &wc, &hc, app->aes_global); /* instead of WindGet() -- see BUG below */
	mt_WindCalc( app, WC_WORK, win, xc, yc, wc, hc, &x, &y, &w, &h);

	if( w >= win -> xpos_max * win -> w_u)
		win -> xpos = 0;
	else
		win -> xpos = MIN( win -> xpos, win -> xpos_max - w / win -> w_u);

	if( h >= win -> ypos_max * win -> h_u)
		win -> ypos = 0;
	else
		win -> ypos = MIN( win -> ypos, win -> ypos_max - h / win -> h_u);
	mt_WindSlider(app,win,HSLIDER|VSLIDER);
}

/* redraw subroutine */
/* KNOW BUG: recursive call of WindGet doesn't seems to work properly,
   maybe a VA_ARG issue...
   => that's why we use wind_get instead of WindGet */

static void __CDECL frame_draw( WINDOW *win, short buff[8], APPvar *app) {
	FRAME *frame = mt_DataSearch( app, win, WD_WFRA);
	WINDOW *scan = frame->list;
	INT16 xy[10];
	INT16 xc, yc, wc, hc, i;
	GRECT r1;
	short vdih = win->graf->handle;
	GRECT clip = win->graf->clip;
	
	mt_wind_get( win -> handle, WF_CURRXYWH, &xc, &yc, &wc, &hc, app->aes_global); /* instead of WindGet() */
	mt_WindCalc( app, WC_WORK, win, xc, yc, wc, hc, &r1.g_x, &r1.g_y, &r1.g_w, &r1.g_h);

	/* background */
	if( !(frame->flags & NO_BACKGROUND) ){
		xy[0]=r1.g_x;xy[1]=r1.g_y;
		xy[2]=r1.g_x+r1.g_w-1;xy[3]=r1.g_y+r1.g_h-1;
		vsf_color( vdih, (app->color>=16)?frame->color:WHITE);
		v_bar( vdih, xy);
	}
	
	/* draw frames */
	
	while( scan) {
		if( scan->status & WS_OPEN) {
			GRECT c = scan->graf->clip;

			mt_FrameCalc( app, scan, 1, &r1.g_x, &r1.g_y, &r1.g_w, &r1.g_h);
			/* Drawing borders */
			if( frame->border > 0 && !(GETFINFO(app,scan)->flags & FRAME_NOBORDER)) {
				if( frame->flags & NO_BACKGROUND) {
					vsl_color( vdih, frame->color);
					for( i=2; i<frame->border/2+1; i++) {
						set_frame( xy, r1.g_x-i, r1.g_y-i, r1.g_w+2*i, r1.g_h+2*i);
						v_pline( vdih, 5, xy);
					}
				}
				set_frame( xy, r1.g_x-1, r1.g_y-1, r1.g_w+2, r1.g_h+2);
				vsl_color( vdih, BLACK);
				v_pline( vdih, 5, xy);
				if( app->color >= 16) {
					set_frame( xy, r1.g_x-2, r1.g_y-2, r1.g_w+4, r1.g_h+4);
					vsl_color( vdih, LBLACK);
					v_pline( vdih, 5, xy);

					vsl_color( vdih, WHITE);
					v_pline( vdih, 3, xy+2);
				}
			}
			/* Drawing frame widgets */
			mt_ClipOn( app, scan->graf, &clip);
			if( frame->front == scan)
				set_gadget_pos( app, scan, &r1, 0x1);
			else
				set_gadget_pos( app, scan, &r1, 0x1|0x4);
			
			/* redraw the frame content (work area) */
			/* get the 1st clipping area */
			mt_FrameCalc( app, scan, 0, &r1.g_x, &r1.g_y, &r1.g_w, &r1.g_h);
			if( rc_intersect( &clip, &r1)) {
				mt_ClipOn( app, scan->graf, &r1);
				grect2msg(&r1,buff);
				mt_EvntExec( app, scan, buff);
			} 

			mt_ClipOn( app, scan->graf, &c);
		}
		scan = scan->next;
	}
}

static void __CDECL frame_destroy( WINDOW *win, short buff[8], APPvar *app) {
	FRAME *frame = mt_DataSearch( app, win, WD_WFRA);
	WINDOW *scan = frame->list, *next;
	
	while( scan) {
		next = scan->next;
		mt_EvntExec( app, scan, buff);
		scan = next;
	}
	free( frame);
/*	WindClose( win);
	WindDelete( win); */
}

/* scrolling functions (use the widget field) */

static void __CDECL frame_vsld( WINDOW *win, short buff[8], APPvar *app) {
	WINDOW *frame = ((FRAME*)mt_DataSearch(app,win,WD_WFRA))->widget;
	
	if( frame && ( frame->status & WS_OPEN))
		mt_EvntExec( app, frame, buff);
}

static void __CDECL frame_hsld( WINDOW *win, short buff[8], APPvar *app) {
	WINDOW *frame = ((FRAME*)mt_DataSearch(app,win,WD_WFRA))->widget;
	
	if( frame && ( frame->status & WS_OPEN))
		mt_EvntExec( app, frame, buff);
}

static void __CDECL frame_arw( WINDOW *win, short buff[8], APPvar *app) {
	WINDOW *frame = ((FRAME*)mt_DataSearch(app,win,WD_WFRA))->widget;
	INT16 button, dum;
	
	if( frame && ( frame->status & WS_OPEN))
		do {
			mt_EvntExec( app, frame, buff);
			mt_graf_mkstate( &dum, &dum, &button, &dum, app->aes_global);
		} while( button & 0x1 && win != app->wglb.front);  /* TODO: replace 0x1 by constant name */
}

static void __CDECL frame_tpd( WINDOW *win, short buff[8], APPvar *app) {
	WINDOW *child = mt_FrameFind( app, win, app->evnt.mx, app->evnt.my);
	INT16 x,y,w,h;
	
	if( child == NULL) {
		if( app->wglb.front == win)
			mt_WindSet( app, win, WF_BOTTOM, win->handle, 0, 0, 0);
		else
			mt_WindSet( app, win, WF_TOP, win->handle, 0, 0, 0);
		return;
	}
		
	mt_FrameCalc( app, child, 0, &x, &y, &w, &h);
	
	if( IS_IN(app->evnt.mx, app->evnt.my, x, y, w, h)) {
		mt_EvntExec( app, child, buff);
		mt_wind_set( win->handle, WF_TOP, win->handle, 0, 0, 0, app->aes_global);
		mt_FrameSet( app, win, FRAME_ACTIV, child);
	} else {
		short localbuff[8];
		localbuff[0] = WM_XBUTTON;
		localbuff[1] = mt_AppAESapid(app);
		localbuff[2] = 0;
		localbuff[3] = app->evnt.mx;
		localbuff[4] = app->evnt.my;
		localbuff[5] = app->evnt.mbut;
		localbuff[6] = app->evnt.mkstate;
		localbuff[7] = app->evnt.nb_click;
		mt_EvntExec( app, win, localbuff);
	}
}

static void __CDECL frame_untpd( WINDOW *win, short buff[8], APPvar *app) {
	GRECT r;
	FRAME *frame = mt_DataSearch( app, win, WD_WFRA);
	
	if( frame->front && !(win->status & WS_ICONIFY)) {
		mt_FrameCalc( app, frame->front, 1, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
		set_gadget_pos( app, frame->front, &r, 0x1|0x2|0x4);  /* TODO: replace 0x? by constant name */
		frame->front = NULL;
	}
}

static void __CDECL frame_szd( WINDOW *win, short buff[8], APPvar *app) {
	int w, h;
	WINDOW *child = ((FRAME *)mt_DataSearch( app, win, WD_WFRA))->list;
	
	w = MAX( buff[6], win -> w_min);
	h = MAX( buff[7], win -> h_min);
	w = MIN( w      , win -> w_max);
	h = MIN( h      , win -> h_max);
	mt_wind_set( buff[3], WF_CURRXYWH, buff[4], buff[5], w, h, app->aes_global);
	mt_snd_rdw( app, win);
	win->status &= ~WS_FULLSIZE;
	while( child) {
		if( child->status & WS_OPEN)
			update_scrolls( app, child);
		child = child->next;
	}
}

static void __CDECL frame_fld( WINDOW *win, short buff[8], APPvar *app) {
	INT16 x, y, w, h;
	WINDOW *child = ((FRAME *)mt_DataSearch( app, win, WD_WFRA))->list;
	
	if( win->status & WS_FULLSIZE) {
		mt_wind_get( buff[3], WF_PREVXYWH, &x, &y, &w, &h, app->aes_global);
		mt_wind_set( buff[3], WF_CURRXYWH, x, y,  w, h, app->aes_global);
	} else
		mt_wind_set( buff[3], WF_CURRXYWH, app->x, app->y, win->w_max, win->h_max, app->aes_global);
	
	mt_snd_rdw( app, win);
	win->status ^= WS_FULLSIZE;
	/* update scrolls */
	while( child) {
		if( child->status & WS_OPEN)
			update_scrolls( app, child);
		child = child->next;
	}
}


/* events */

/*  called by frame_click
 *	find the frame column/line at coordinates (x,y) 
 *	return 0 (line) or 
 *             1 (column)	
 *	r will contain the outside coordinates (for movebox effects)
 *
 *  Nota: bottom and the right borders (frontier) of the frame at
 *       (line/column) are part of the (line/column) frame.
 */


static int get_board( APPvar *app, WINDOW *win, int x, int y, int *line, int *col, GRECT *r) {
	INT16 xw, yw, ww, hw;
	WINDOW *scan, *begin;
	int max_h, line_h, dum=0;
	FRAME *frame = mt_DataSearch( app, win, WD_WFRA);
	FRAMEINFO *finfo;
		
	/* area of the parent window */
	
	mt_wind_get( win -> handle, WF_CURRXYWH, &xw, &yw, &ww, &hw, app->aes_global);
	mt_WindCalc( app, WC_WORK, win, xw, yw, ww, hw, &xw, &yw, &ww, &hw);
	max_h = 0;
	line_h = 0;
	rc_set( r, xw, yw, ww, hw);
	begin = scan = frame->list;
	*line = GETFINFO(app,scan)->line;
	for( ; scan; scan=scan->next) {
		finfo = GETFINFO(app,scan);
		if( *line != finfo->line) {	/* new line */
			line_h += max_h;
			*line = finfo->line;
			if( y >= yw + line_h && y < yw + line_h + frame->border) {
				r->g_y += line_h;
				r->g_h = frame->border;
				return 0;	/* line */
			} 
			if( y < yw + line_h) {
				r->g_y += dum;
				r->g_h = max_h;
				break; /* column */
			}
			line_h += frame->border;
			dum = line_h;
			begin = scan;
			max_h = 0;
		}
		
		/* compute the max height of the cell line */
		max_h = MAX( max_h, PROPOR( finfo->flags & FRAME_HSCALE, hw, finfo->h));
	}
	/* search for the column */
	finfo = GETFINFO(app,begin);
	if( scan != NULL) *line = finfo->line;
	line_h = max_h = 0;
	*col = finfo->col;
	for( scan=begin; scan; scan=scan->next) {
		finfo = GETFINFO(app,scan);
		if( *col != finfo->col) {	/* new column */
			line_h += max_h;
			max_h = 0;
			*col = finfo->col;
			if( x >= xw + line_h && x < xw + line_h + frame->border) {
				r->g_x += line_h;
				r->g_w = frame->border;
				return 1;	/* ok */
			}
			line_h += frame->border;
		}
		max_h = MAX( max_h, PROPOR( finfo->flags & FRAME_WSCALE, ww, finfo->w));
	}
	return 1;
}

static void __CDECL frame_click( WINDOW *win, short buff[8], APPvar *app) {
	WINDOW *child = mt_FrameFind( app, win, app->evnt.mx, app->evnt.my), *scan;
	FRAME *frame = mt_DataSearch( app, win, WD_WFRA);
	FRAMEINFO *finfo;
	INT16 x, y, w, h;
	INT16 xr, yr, wr, hr;
	INT16 x1, y1, res, dum1, dum2;
	int line, col;
	GRECT r;
	OBJECT *tree;
	
	if( child) {
		if( child->status & WS_OPEN) {
			/*
	 		 * manage frame gadget
	 		 */
			finfo = GETFINFO(app,child);
			if( finfo->flags & FRAME_SELECT) {
				/* Gestion des gadgets */
				tree = get_frame_slider(app);
				if( frame->front == child || win != app->wglb.front) {
					mt_FrameCalc( app, child, 1, &r.g_x, &r.g_y, &r.g_w, &r.g_h);
					set_gadget_pos( app, child, &r, 0);
				} else
					mt_FrameSet( app, win, FRAME_ACTIV, child);
				frame->widget = child;
				res = mt_objc_find( tree, M_H_BG, 2, app->evnt.mx, app->evnt.my, app->aes_global);
				if( res != -1) {
					mt_WindGet( app, win, WF_WORKXYWH, &x, &y, &w, &h);
					switch ( res) {
					case M_H_RT:
						tree[M_H_RT].ob_state |= SELECTED;
						mt_ObjcWindDraw( app, win, tree, M_H_RT, 1, x, y, w, h);
						do {
							mt_snd_arrw( app, win, WA_RTLINE);
							mt_EvntWindom( app, MU_MESAG);
							mt_graf_mkstate( &dum1, &dum1, &res, &dum1, app->aes_global);
							
						} while( res);
						tree[M_H_RT].ob_state &= ~SELECTED;
						mt_ObjcWindDraw( app, win, tree, M_H_RT, 1, x, y, w, h);
						break;
					case M_H_LF:
						tree[M_H_LF].ob_state |= SELECTED;
						mt_ObjcWindDraw( app, win, tree, M_H_LF, 1, x, y, w, h);
						do {
							
							mt_snd_arrw( app, win, WA_LFLINE);
							mt_EvntWindom( app, MU_MESAG);
							mt_graf_mkstate( &dum1, &dum1, &res, &dum1, app->aes_global);
							
						} while( res);
						tree[M_H_LF].ob_state &= ~SELECTED;
						mt_ObjcWindDraw( app, win, tree, M_H_LF, 1, x, y, w, h);
						break;
					case M_H_SL:
						if( app->aes4 & AES4_XGMOUSE) mt_graf_mouse( M_SAVE, 0L, app->aes_global);
						mt_graf_mouse( FLAT_HAND, NULL, app->aes_global);
						while( !mt_wind_update( BEG_MCTRL, app->aes_global));
						x = mt_graf_slidebox( tree, M_H_PG, M_H_SL, 0, app->aes_global);
						mt_wind_update( END_MCTRL, app->aes_global);
						mt_snd_msg( app, win, WM_HSLID, x, 0, 0, 0);
						if( !(app->aes4 & AES4_XGMOUSE) || mt_graf_mouse( M_RESTORE, 0L, app->aes_global) == 0 )
							mt_graf_mouse(ARROW,NULL,app->aes_global);
						break;

					case M_H_PG:						
						mt_objc_offset( tree, M_H_SL, &x, &y, app->aes_global);
						dum2 = (app->evnt.mx < x)?WA_LFPAGE:WA_RTPAGE;
						x1   = (app->evnt.mx < x)?M_H_LF:M_H_RT;
						
						while( !mt_wind_update( BEG_MCTRL, app->aes_global));
						tree[x1].ob_state |= SELECTED;
						mt_ObjcWindDraw( app, win, tree, x1, 1, app->x, app->y, app->w, app->h);
						do {
							mt_snd_arrw( app, win, dum2);
							mt_EvntWindom( app, MU_MESAG);
							mt_graf_mkstate( &dum1, &dum1, &res, &dum1, app->aes_global);
							
						} while( res);
						tree[x1].ob_state &= ~SELECTED;
						mt_ObjcWindDraw( app, win, tree, x1, 1, app->x, app->y, app->w, app->h);
						mt_wind_update( END_MCTRL, app->aes_global);
						
						break;
					}
					return;
				}
				res = mt_objc_find( tree, M_V_BG, 2, app->evnt.mx, app->evnt.my, app->aes_global);
				if( res != -1) {
					mt_WindGet( app, win, WF_WORKXYWH, &x, &y, &w, &h);
					switch ( res) {
					case M_V_UP:
						tree[M_V_UP].ob_state |= SELECTED;
						mt_ObjcWindDraw( app, win, tree, M_V_UP, 1, x, y, w, h);
						do {
							mt_snd_arrw( app, win, WA_UPLINE);
							mt_EvntWindom( app, MU_MESAG);
							mt_graf_mkstate( &dum1, &dum1, &res, &dum1, app->aes_global);
							
						} while( res);
						tree[M_V_UP].ob_state &= ~SELECTED;
						mt_ObjcWindDraw( app, win, tree, M_V_UP, 1, x, y, w, h);
						break;

					case M_V_DW:
						tree[M_V_DW].ob_state |= SELECTED;
						mt_ObjcWindDraw( app, win, tree, M_V_DW, 1, x, y, w, h);
						do {
							
							mt_snd_arrw( app, win, WA_DNLINE);
							mt_EvntWindom( app, MU_MESAG);
							mt_graf_mkstate( &dum1, &dum1, &res, &dum1, app->aes_global);
							
						} while( res);
						tree[M_V_DW].ob_state &= ~SELECTED;
						mt_ObjcWindDraw( app, win, tree, M_V_DW, 1, x, y, w, h);
						break;

					case M_V_SL:
						if( app->aes4 & AES4_XGMOUSE) mt_graf_mouse( M_SAVE, 0L, app->aes_global);
						mt_graf_mouse( FLAT_HAND, NULL, app->aes_global);
						while( !mt_wind_update( BEG_MCTRL, app->aes_global));
						x = mt_graf_slidebox( tree, M_V_PG, M_V_SL, 1, app->aes_global);
						mt_wind_update( END_MCTRL, app->aes_global);
						mt_snd_msg( app, win, WM_VSLID, x, 0, 0, 0);
						if( !(app->aes4 & AES4_XGMOUSE) || mt_graf_mouse( M_RESTORE, 0L, app->aes_global) == 0 )
							mt_graf_mouse(ARROW,NULL, app->aes_global);
						break;

					case M_V_PG:
						mt_objc_offset( tree, M_V_SL, &x, &y, app->aes_global);
						dum2 = (app->evnt.my < y)?WA_UPPAGE:WA_DNPAGE;
						x1   = (app->evnt.my < y)?M_V_UP:M_V_DW;

						while( !mt_wind_update( BEG_MCTRL, app->aes_global));
						tree[x1].ob_state |= SELECTED;
						mt_ObjcWindDraw( app, win, tree, x1, 1, app->x, app->y, app->w, app->h);
						do {
							mt_snd_arrw( app, win, dum2);
							mt_EvntWindom( app, MU_MESAG);
							mt_graf_mkstate( &dum1, &dum1, &res, &dum1, app->aes_global);
						} while( res);
						tree[x1].ob_state &= ~SELECTED;
						mt_ObjcWindDraw( app, win, tree, x1, 1, app->x, app->y, app->w, app->h);
						mt_wind_update( END_MCTRL, app->aes_global);
						break;
					}
					return;
				}
				/* Sizer */
				res = mt_objc_find( tree, M_SZ, 0, app->evnt.mx, app->evnt.my, app->aes_global);
				if( res != -1 && child->attrib & SIZER) {
					mt_wind_get( win->handle, WF_CURRXYWH, &x, &y, &w, &h, app->aes_global);
					while( !mt_wind_update( BEG_MCTRL, app->aes_global));
					mt_graf_mouse( 4, NULL, app->aes_global);
					mt_graf_rubberbox( x, y, win->w_min, win->h_min, &w, &h, app->aes_global);
					mt_graf_mouse( 0, NULL, app->aes_global);
					mt_wind_update( END_MCTRL, app->aes_global);
					mt_snd_msg( app, win, WM_SIZED, x, y, w, h);
					return;
				}
			}
			
			/* callback the right subroutine */
			
			if( !(child->status & WS_UNTOPPABLE) &&  win != app->wglb.front) {
				short localbuff[8] = {WM_TOPPED};
				mt_EvntExec( app, child, localbuff);
				if( frame->flags & TOPPED_ACTIV)
					mt_FrameSet( app, win, FRAME_ACTIV, child);
			} else {
				short localbuff[8];
				localbuff[0] = WM_XBUTTON;
				localbuff[1] = mt_AppAESapid(app);
				localbuff[2] = 0;
				localbuff[3] = app->evnt.mx;
				localbuff[4] = app->evnt.my;
				localbuff[5] = app->evnt.mbut;
				localbuff[6] = app->evnt.mkstate;
				localbuff[7] = app->evnt.nb_click;
				mt_EvntExec( app, child, localbuff);
			}
		}
	} else {
		/*
		 * manage frame borders
		 */		

		mt_wind_get( win -> handle, WF_CURRXYWH, &x, &y, &w, &h, app->aes_global);
		mt_WindCalc( app, WC_WORK, win, x, y, w, h, &xr, &yr, &wr, &hr);
		res = get_board( app, win, app->evnt.mx, app->evnt.my, &line, &col, &r);
		if( res == -1)
			return;	/* error - not yet handled */
		dum1 = dum2 = 0;
		for( scan = frame->list; scan; scan = scan->next) {
			/* check that frames may be resized and
			 * maximum size possible (line)
			 */
			finfo = GETFINFO(app,scan);
			if( res == 0 && finfo->line == line) {
			 	if( finfo->flags & FRAME_HFIX) 
					return;
				dum1 = MAX( dum1, PROPOR( finfo->flags & FRAME_HSCALE, hr, finfo->h));
			}
			if( res == 0 && finfo->line == line-1) {
				if( finfo->flags & FRAME_HFIX)
					return;
				dum2 = MAX( dum2, PROPOR( finfo->flags & FRAME_HSCALE, hr, finfo->h));
			}
			if( res == 1 && finfo->col == col && finfo->line == line) {
			 	if( finfo->flags & FRAME_WFIX) 
					return;
				dum2 = PROPOR( finfo->flags & FRAME_WSCALE, wr, finfo->w);
				dum2 = dum2-scan->w_min;
			}
			if( res == 1 && finfo->col == col-1 && finfo->line == line) {
				if( finfo->flags & FRAME_WFIX)
					return;
				dum1 = PROPOR( finfo->flags & FRAME_WSCALE, wr, finfo->w);
				dum1 = dum1-scan->w_min;
			}
		}
		if( res == 0) { /* line */
			mt_graf_dragbox( wr+1, frame->border, 
						  xr, r.g_y, xr, 
						  r.g_y-dum2, wr, MIN(r.g_y+dum1, yr+hr)-r.g_y+dum2,
						  &x1, &y1,
						  app->aes_global);
			/* resize : TODO */
		} else {
			FRAMEINFO *finfo_n;

			mt_graf_dragbox( frame->border, r.g_h,
						  r.g_x, r.g_y, 
						  r.g_x-dum1, r.g_y, r.g_w+dum1+dum2, r.g_h,
						  &x1, &y1,
						  app->aes_global);

			/* BUG: la taille de la derniere frame doit correspondre a la fin
			        de l'‚cran ou sa vrai taille, ca merde surement a cause des
			        erreurs d'arrondis */
			if( col == 0)	return;
			/* Resizons mes frŠres: deux frames col et col-1*/
			scan = mt_FrameSearch( app, win, line, col-1);
			finfo = GETFINFO( app,scan);
			finfo_n = GETFINFO( app,scan->next);
			mt_FrameCalc( app, scan, 1, &x, &y, &w, &h);
			dum1 = (int)((finfo->flags & FRAME_WSCALE)?(((long)(x1-x)*100L)/(long)wr):(x1-x));
			
			mt_FrameCalc( app, scan->next, 1, &x, &y, &w, &h);
			finfo_n->w = (int)((finfo_n->flags & FRAME_WSCALE)?(((long)(w-x1+x)*100L)/(long)wr):(w-x1+x));
			if( (scan->next->next == NULL
				|| finfo_n->line != GETFINFO(app,scan->next->next)->line)
				&& finfo_n->flags & FRAME_WSCALE)
				finfo->w ++;
			finfo->w = dum1;
			
			update_scrolls( app, scan);
			update_scrolls( app, scan->next);
			mt_snd_rdw(app,win);
		}
	}
}

static void __CDECL frame_keybd( WINDOW *win, short buff[8], APPvar *app) {
	FRAME *frame = mt_DataSearch( app, win, WD_WFRA);
	WINDOW *child;
	
	if( frame->flags & FLG_KEYMOUSE)
	 	child = mt_FrameFind( app, win, app->evnt.mx, app->evnt.my);
	else
		child = frame->front;
	
	if( child && (child->status & WS_OPEN)) {
		frame->widget = child;
		mt_EvntExec( app, child, buff);
	}
}

static void __CDECL frame_timer( WINDOW *win, short buff[8], APPvar *app) {
	WINDOW *child = ((FRAME *)mt_DataSearch( app, win, WD_WFRA))->list;

	while( child) {
		if( child->status & WS_OPEN)
			mt_EvntExec( app, child, buff);
		child = child->next;
	}
}

/** Create a frame window
 *
 *  @param app application descriptor
 *  @param attrib GEM widget of the window (frame)
 *
 *  @return window descriptor (NULL if failed)
 *
 *	mt_FrameCreate() just creates a window descriptor whose will host
 *	framed windows. The window created is not opened at screen as for
 *	mt_WindCreate(). Optional parameters can be set with mt_WindSet() for
 *	general window options and mt_FrameSet() for specific frame options.
 *
 *	The window created by mt_FrameCreate() has the \b WS_FRAME_ROOT bit
 *	of status window descriptor field set to 1. A 
 *	FRAME structure is attached as data to the window with a \b WD_WFRA
 *	reserved magic number.
 *
 *	The method used to handle frame window is very simple. A frame window
 *	is seen like a set of framed windows. A framed window is not a real GEM
 *	window but is identified by a window descriptor. The frame window, i.e. the
 *	root window holding the list of framed window, and the standard event functions
 *	of the frame window use the standard event function of the framed window.
 *	Some events are applied to the active frame, such as a button event. The
 *	active frame may be defined by mt_FrameSet().
 *
 *  @bug
 *  sliders of framed window are not correctly initialized
 *
 *  @bug
 *  the vertical frame resizing widget are not supported
 *
 *  @sa mt_FrameAttach(), mt_FrameSet().
 *
 *  @deprecated To be replaced by COMPONENTs
 */

WINDOW *mt_FrameCreate( APPvar *app, int attrib) {
	WINDOW *win;
	FRAME *frame;
	
	frame = (FRAME *)malloc( sizeof(FRAME));
	if( !frame) return NULL;
	frame -> list = frame -> front = NULL;
	frame -> border = 1;
	frame -> color = LWHITE;
	frame -> flags = 0;
	win = mt_WindCreate( app, attrib, app->x, app->y, app->w, app->h);
	if (win==NULL) {
		free(frame);
		return NULL;
	}
	mt_EvntAttach( app, win, WM_REDRAW, frame_draw);
	mt_EvntAttach( app, win, WM_ICONDRAW, mt_stdWindClear);
	mt_EvntAdd   ( app, win, WM_DESTROY, frame_destroy, EV_TOP);
	mt_EvntAttach( app, win, WM_VSLID  , frame_vsld);
	mt_EvntAttach( app, win, WM_HSLID  , frame_hsld);
	mt_EvntAttach( app, win, WM_ARROWED, frame_arw);
	mt_EvntAttach( app, win, WM_TOPPED , frame_tpd);
	mt_EvntAttach( app, win, WM_UNTOPPED, frame_untpd);
	mt_EvntAttach( app, win, WM_SIZED  , frame_szd);
	mt_EvntAttach( app, win, WM_FULLED , frame_fld);

	mt_EvntAttach( app, win, WM_XBUTTON , frame_click);
	mt_EvntAttach( app, win, WM_XKEYBD , frame_keybd);
	mt_EvntAttach( app, win, WM_XTIMER , frame_timer);
	
	mt_DataAttach( app, win, WD_WFRA, frame);
	
	win -> status |= WS_FRAME_ROOT;
	frame->col = frame->line =0;

	mt_WindSet( app, win, WF_BEVENT, BEVENT_WORK, 0, 0, 0);
	return win;
}

