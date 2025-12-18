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
 * $Source: /cvsroot/windom/windom/src/comp_create.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/02/22 18:26:36 $
 *   $Revision: 1.26 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#include "globals.h"

#define SZDDEBUG 0
#if SZDDEBUG
#define CDT_DEBUGID 0x434f4cUL  /* 'COL' */
#define D(x) x
#else
#define D(x)
#endif


/* Focus component pointer ('CFCS') */
#define CDT_CPNT_FOCUS        0x43464353L


static
void comp_setfocus( APPvar *app, COMPONENT *p, COMPONENT *f );


/**
 * @brief Create a COMPONENT.
 *
 * @param app application descriptor,
 * @param type Component layout type :
 *   - \b CLT_VERTICAL : a vertical layout,
 *   - \b CLT_HORIZONTAL : an horizontal layout,
 *   - \b CLT_STACK : a stack layout,
 * @param size size of compoment, avalaible with \b CLT_VERTICAL and \b CLT_HORIZONTAL, 
 * @param flex flexibility of the component. 0 means fixed size.
 *
 * @sa mt_CompDelete().
 *
 * @note If \a flex > 0 then the component status is set to CS_PROPORTIONAL.
 */

COMPONENT *mt_CompCreate( APPvar *app, short type, long size, short flex ) {
	COMPONENT *c = malloc( sizeof(COMPONENT) );
	if ( !c) return NULL;

	/* linkable NULLs */
	c->item.prev = NULL;
	c->item.next = NULL;

	/* basic properties */
	c->type = type;
	c->status = flex ? CS_PROPORTIONAL : 0;
	c->flex = flex;
	c->size = size;
	c->graf = &app->graf;

	/* relationship */
	c->parent = NULL;
	c->children = NULL;

	/* binding & data list initialize */
	listInit(&c->binding);
	listInit(&c->data);

	/* Set the top level component min/max sizes to let the
	 * resizing work properly. It is to be adjusted properly
	 * in the CompAttach. */
	c->bounds.min_width = 0;
	c->bounds.max_width = CPNT_MAXSIZE;
	c->bounds.min_height = 0;
	c->bounds.max_height = CPNT_MAXSIZE;

	return c;
}

/**
 * @brief Delete a COMPONENT created by mt_CompCreate().
 *
 * This issues the WM_DESTROY event on the particular component and
 * calls itself recursively on the children. After that it deletes
 * and COMPONENT internal data and finally the \p p instance itself.
 *
 * @param app application descriptor,
 * @param p component to delete.
 *
 */

int mt_CompDelete( APPvar *app, COMPONENT *p ) {
	/* remove possible CF_FOCUS reference */
	if ( p->status & CS_FOCUSED ) {
		COMPONENT *c = p;
		while ( c->parent ) c = c->parent;
		/* ask the top-level component whether the focus is 'p' */
		if ( mt_CompDataSearch( app, c, CDT_CPNT_FOCUS) == p )
			mt_CompDataDelete( app, c, CDT_CPNT_FOCUS);

		p->status &= ~CS_FOCUSED;
	}

	/* postpone the deletion if this is called from
	 * within a component handler now */
	if (p->status & CS_IN_USE) {
		p->status |= CS_DESTROYED;
		return E_OK;
	}

	/* WM_DESTROY handler call */
	{
		long buff[8] = { WM_DESTROY };
		mt_CompEvntExec(app, p, buff );
	}

	/* delete children (if any) */
	if ( p->children ) {
		/* get the first child */
		COMPONENT *c = (COMPONENT*)listFirst(p->children);
		while ( c ) {
			COMPONENT *trash = c;
			c = (COMPONENT*)listNext(c); /* get the next list item */
			mt_CompDelete( app, trash);
		}

		/* delete the children LIST */
		free(p->children);
	}

	/* remove the sliderinfos if any */
	if ( p->status & CS_HSLIDE ) {
		free(mt_CompDataSearch(app, p, WD_CPNT_HSLIDE_INFO));
	}
	if ( p->status & CS_VSLIDE ) {
		free(mt_CompDataSearch(app, p, WD_CPNT_VSLIDE_INFO));
	}

	/* clear data entry list */
	mt_CompDataClear( app, p );

	/* event handlers clear */
	mt_CompEvntClear( app, p );

	/* dispose the COMPONENT structure itself */
	free(p);

	return E_OK;
}


int mt_CompSet( APPvar *app, COMPONENT *c, short mode, long par1, long par2, long par3, long par4) {
	C_SLIDERINFO *slider;
	switch( mode) {
		case CF_FOCUS:
			{
				void *f = (void*) (long) ( ((long)par1 << 16 ) | ((long)par2 & 0xFFFFL) );
				comp_setfocus( app, c, f);
				return E_OK;
			}

		case CF_VSLIDE:
		case CF_HSLIDE:
			slider = mt_CompDataSearch(app, c, mode==CF_VSLIDE ? WD_CPNT_VSLIDE_INFO : WD_CPNT_HSLIDE_INFO);
			if ( !slider ) return E_OK;
			slider->pos  = par1;
			slider->max  = par2;
			slider->unit = par3;
			return E_OK;
	}
	return ENOSYS;
}

int mt_CompSetPtr( APPvar *app, COMPONENT *c, short mode, void *par1, void *par2) {
	return mt_CompSet( app, c, mode, ADR(par1), ADR(par2));
}
	

/**
 * @brief Define a COMPONENT to become slidable.
 * @param app application descriptor,
 * @param p targeted component, 
 * @param type type of slider to define :
 *   - \b CS_HSLIDE defines an horizontal slider,
 *   - \b CS_VSLIDE defines an vertical slider,
 * @return O or a negative code error otherwise.
 */

int mt_CompSliderAttach(APPvar *app, COMPONENT *p, short type) {
	C_SLIDERINFO *slider = malloc(sizeof(C_SLIDERINFO));
	if( !slider) return ENSMEM;

	slider->pos = 0;
	slider->max = 0;
	slider->unit = 1;

	switch ( type ) {
		case CS_HSLIDE:
			mt_CompDataAttach(app, p, WD_CPNT_HSLIDE_INFO, slider);
			break;
		case CS_VSLIDE:
			mt_CompDataAttach(app, p, WD_CPNT_VSLIDE_INFO, slider);
			break;
		default:
			/* unsupported type */
			free(slider);
			return ENOSYS;
	}

	p->status |= type;
	return E_OK;
}


/**
 * @brief Attach a COMPONENT as children of an another COMPONENT.
 *
 * @param app application descriptor,
 * @param p parent component,
 * @param c children component.
 *
 * As one can see, COMPONENTs are hierarchical in a similar way to 
 * AES OBJECTS.
 *
 */

void mt_CompAttach( APPvar *app, COMPONENT *p, COMPONENT *c) {
	/* append the child */
	if ( !p->children ) p->children = createList();
	c->parent = p;
	listInsert( &p->children->tail, (LINKABLE*)c );

	/* project c->size to c->min/max_size H/V */
	switch ( p->type ) {
		case CLT_HORIZONTAL:
			if ( !c->flex ) c->bounds.min_width  = MAX( c->bounds.min_width, c->size );
			c->bounds.max_width  = MIN( c->bounds.max_width, c->flex ? CPNT_MAXSIZE : c->size );
			break;
		case CLT_VERTICAL:
			if ( !c->flex ) c->bounds.min_height  = MAX( c->bounds.min_height, c->size );
			c->bounds.max_height  = MIN( c->bounds.max_height, c->flex ? CPNT_MAXSIZE : c->size );
			break;
		case CLT_STACK:
			/* fallthrough: stack children can be of arbitrary size (defined in c->bounds) */
		default:
			break;
	}
}


static
void comp_get_minmaxsize( COMPONENT *p, LGRECT *rect ) {
	rect->g_w = 0;
	rect->g_h = 0;

	if ( p->children ) {
		COMPONENT *c;

		switch ( p->type ) {
			case CLT_HORIZONTAL:
				FOR_EACH_CHILD( c, p->children ) {
					LGRECT crect;
					comp_get_minmaxsize( c, &crect );

					if ( crect.g_w != CPNT_MAXSIZE )
						rect->g_w += crect.g_w;
					if ( crect.g_h != CPNT_MAXSIZE )
						rect->g_h = MAX( rect->g_h, crect.g_h );
				}
				break;
			case CLT_VERTICAL:
				FOR_EACH_CHILD( c, p->children ) {
					LGRECT crect;
					comp_get_minmaxsize( c, &crect );

					if ( crect.g_w != CPNT_MAXSIZE )
						rect->g_w = MAX( rect->g_w, crect.g_w );
					if ( crect.g_h != CPNT_MAXSIZE )
						rect->g_h += crect.g_h;
				}
				break;
			case CLT_STACK:
				/* get the size from the rect of the component */
				rect->g_w = p->rect.g_w;
				rect->g_h = p->rect.g_h;
				break;
		}
	}

	rect->g_w = MIN( MAX( rect->g_w, p->bounds.min_width ), p->bounds.max_width );
	rect->g_h = MIN( MAX( rect->g_h, p->bounds.min_height), p->bounds.max_height );

	D(printf( " MXS: c = %03s: size=[%3ld,%3ld] c=[%3ld,%3ld]\n",
			  (char*)mt_CompDataSearch(NULL, p, CDT_DEBUGID),
			  rect->g_w, rect->g_h, p->bounds.max_width, p->bounds.max_height ));
}


int mt_CompGetLGrect( APPvar *app, COMPONENT *c, short type, LGRECT *rect) {
	int res = E_OK;
	COMPONENT *p = c;

	switch ( type ) {
		case WF_WORKCLIPXYWH:
			res = mt_CompGetLGrect( app, p, WF_WORKXYWH, rect);

			/* clip by the nearest CS_CLIP parent (if any) */
			while ( p->parent ) {
				p = p->parent;
				if ( p->status & CS_CLIP ) {
					LGRECT clip;
					res = mt_CompGetLGrect( app, p, WF_WORKCLIPXYWH, &clip);
					rc_lintersect( &clip, rect );
					return res;
				}
			}
			break;

		case WF_WORKXYWH:
			rect->g_x = 0;
			rect->g_y = 0;
			rect->g_w = p->rect.g_w;
			rect->g_h = p->rect.g_h;

			for(;;) {
				rect->g_x += p->rect.g_x;
				rect->g_y += p->rect.g_y;

				p = p->parent;
				if ( ! p ) break;

				/* slider position (only affects components' children) */
				if ( p->status & CS_HSLIDE ) {
					C_SLIDERINFO *slider = mt_CompDataSearch(app, p, WD_CPNT_HSLIDE_INFO);
					rect->g_x -= slider->pos * slider->unit;
				}
				if ( p->status & CS_VSLIDE ) {
					C_SLIDERINFO *slider = mt_CompDataSearch(app, p, WD_CPNT_VSLIDE_INFO);
					rect->g_y -= slider->pos * slider->unit;
				}
			}
			break;

		case CF_MAXSIZE:
			rect->g_x = 0;
			rect->g_y = 0;
			comp_get_minmaxsize( c, rect );
			break;

		default:
			return ENOSYS;
	}

	return res;
}

void* mt_CompGetPtr( APPvar *app, COMPONENT *c, short type) {
	switch ( type ) {
		case CF_WINDOW:
			while ( c ) {
				WINDOW *win = mt_CompDataSearch(app,c,WD_CCRT);
				if (win) return win;
				c = c->parent;
			}
			break;

		case CF_FOCUS:
			return mt_CompDataSearch( app, c, CDT_CPNT_FOCUS);
	}
	return NULL;
}


COMPONENT *mt_CompFind( APPvar *app, COMPONENT *p, short mx, short my) {
	COMPONENT *c;
	LGRECT work;

	/* get the work rectangle... */
	mt_CompGetLGrect(app, p, WF_WORKXYWH, &work);

	/* not in our rectangle -> fail */
	if ( mx < work.g_x || mx > work.g_x + work.g_w - 1 ||
		 my < work.g_y || my > work.g_y + work.g_h - 1 )
		return NULL;

	D(printf( " FND: c = %03s: delta=[%3d,%3d] OK\n",
			  (char*)mt_CompDataSearch(app, p, CDT_DEBUGID),
			  mx, my ));

	/* if it has no children then finished -> ok */
	if ( !p->children )
		return p;

	/* search among children */
	FOR_EACH_CHILD( c, p->children ) {
		COMPONENT *f = mt_CompFind( app, c, mx, my );
		if ( f ) return f;
	}

	return p;
}


/**
 * COMPONENT handlers.
 **/

/* executes the message handler by each component in the tree (DFS order) */
void comp_dfs( COMPONENT *p, long *buff, APPvar *app ) {
	COMPONENT *c;

	/* fire the event for each COMPONENT */
	mt_CompEvntExec(app, p, buff);

	/* call children recursively */
	FOR_EACH_CHILD( c, p->children ) {
		comp_dfs( c, buff, app);
	}
}


static
void comp_redraw( COMPONENT *p, LGRECT *lrect, APPvar *app ) {
	LGRECT lwork;
	mt_CompGetLGrect(app, p, WF_WORKXYWH, &lwork);

	if ( rc_lintersect( lrect, &lwork ) ) {
		short clipping = p->status & CS_CLIP;
		COMPONENT *c;
		GRECT clip;
		GRECT work;

		/* convert to GRECT and set as a clip rectangle */
		rc_lgrect2grect( &work, &lwork );

		if ( clipping ) {
			clip = p->graf->clip;
			mt_ClipOn( app, p->graf, &work );
		}

		/* fire the event for the COMPONENT */
		{
			long buff[8];

			buff[0] = WM_REDRAW;
			*(LGRECT*)&buff[4] = lwork;
			mt_CompEvntExec(app, p, buff);
		}

		/* call children recursively */
		FOR_EACH_CHILD( c, p->children ) {
			comp_redraw( c, &lwork, app);
		}

		if ( clipping ) {
			mt_ClipOn( app, p->graf, &clip );
		}
	}
}


static
void comp_setfocus( APPvar *app, COMPONENT *p, COMPONENT *f ) {
	COMPONENT *of = mt_CompGetPtr( app, p, CF_FOCUS);
	if ( f != of ) {
		long buff[8];

		if ( of ) {
			buff[0] = CM_LOSEFOCUS;
			mt_CompEvntExec(app, of, buff);
			of->status &= ~CS_FOCUSED;
		}

		if ( !f ) {
			mt_CompDataDelete( app, p, CDT_CPNT_FOCUS);
			return;
		}

		mt_CompDataAttach( app, p, CDT_CPNT_FOCUS, f);
		f->status |= CS_FOCUSED;

		buff[0] = CM_GETFOCUS;
		mt_CompEvntExec(app, f, buff);
	}
}


/*
 * COMPONENT containing WINDOW handlers.
 */
#define W_COMP(app,win) mt_DataSearch(app,win,WD_CCRT)

static
void evntbuff2compbuff( long *buff, short *wbuff )
{
	long  *lb = buff;
	INT16 *sb = wbuff;
	/* copy the message buffer contents */
	*lb++ = *sb++; *lb++ = *sb++; *lb++ = *sb++; *lb++ = *sb++;
	*lb++ = *sb++; *lb++ = *sb++; *lb++ = *sb++; *lb++ = *sb++;
}

static
void __CDECL container_pass( WINDOW *win, short wbuff[8], APPvar *app ) {
	long buff[8];
	evntbuff2compbuff( buff, wbuff );
	/* pass the message to the component as is */
	mt_CompEvntExec(app, W_COMP(app,win), buff );
}

static
void __CDECL container_timer( WINDOW *win, short wbuff[8], APPvar *app ) {
	long buff[8];
	evntbuff2compbuff( buff, wbuff );

	/* pass to every child (in deep) */
	buff[0] = WM_XTIMER;
	comp_dfs( W_COMP(app,win), buff, app );
}

static
void __CDECL container_redraw( WINDOW *win, short wbuff[8], APPvar *app ) {
	long buff[8];
	evntbuff2compbuff( buff, wbuff );

	/* call component's WM_REDRAW handlers
	 * (no matter which message this handler was called for) */
	buff[0] = WM_REDRAW;

	/* note: basicaly a call to comp_dfs() would be sufficient here
	 * but we need something more efficient to not to call all the
	 * draw functions for the components that are not about to be
	 * displayed.
	 */
	comp_redraw( W_COMP(app,win), (LGRECT*)&buff[4], app );
}

static
void __CDECL container_mvd_comp( WINDOW *win, COMPONENT *p, short wbuff[8], APPvar *app ) {
	GRECT work;

	/* setup the component rectangle */
	mt_WindCalcGrect( app, WC_WORK, win, (GRECT*)&wbuff[4], &work);
	rc_grect2lgrect( &p->rect, &work );
}

static
void __CDECL container_reflow_comp( WINDOW *win, COMPONENT *p, short wbuff[8], APPvar *app ) {
	/* move the container to the place */
	container_mvd_comp( win, p, wbuff, app );

	/* recompute the size of the attached COMPONENT container */
	comp_reflow( p, app );
}

static
void __CDECL container_szd_comp( WINDOW *win, COMPONENT *p, short wbuff[8], APPvar *app ) {
	/* move the component and adjust the children sizes */
	container_reflow_comp( win, p, wbuff, app );

	/* if the REFLOW caused the top level component
	   to reach the min/max size ->
	   adjust the WM_SIZED message to that size */
	{
		/* get the top-level component size and compute
		   the border as if it was the window work rectangle size */
		LGRECT lrect;
		GRECT rect, border;
		rc_lset( &lrect, app->x, app->y, app->w, app->h );
		rc_lintersect( &p->rect, &lrect );
		rc_lgrect2grect( &rect, &lrect );
		mt_WindCalcGrect( app, WC_BORDER, win, &rect, &border);

		/* adjust the WM_SIZED width or height if larger than the
		   component may stretch */
		if ( wbuff[6] > border.g_w )
			wbuff[6] = border.g_w;
		if ( wbuff[7] > border.g_h )
			wbuff[7] = border.g_h;
	}
}


static
void __CDECL container_reflow( WINDOW *win, short wbuff[8], APPvar *app ) {
	container_reflow_comp( win, W_COMP(app,win), wbuff, app );
}

static
void __CDECL container_mvd( WINDOW *win, short wbuff[8], APPvar *app ) {
	COMPONENT *p = W_COMP(app,win);

	/* move the container to the place */
	container_mvd_comp( win, p, wbuff, app );

	std_mvd( win, wbuff, app );
}

static
void __CDECL container_dstry( WINDOW *win, short wbuff[8], APPvar *app ) {
	/* delete the attached component */
	mt_CompDelete( app, W_COMP(app,win));
}


static
void container_grect_redraw( WINDOW *win, COMPONENT *p, GRECT *oldwork, APPvar *app ) {
	GRECT clip = win->graf->clip;
	mt_ClipOn( app, win->graf, oldwork);
	mt_CompEvntRedraw(app, p);
	mt_ClipOn( app, win->graf, &clip);
}


static
void __CDECL container_szd( WINDOW *win, short wbuff[8], APPvar *app ) {
	COMPONENT *p = W_COMP(app,win);
	GRECT oldwork; /* work rect before resize */
	mt_WindGetGrect( app, win, WF_WORKXYWH, &oldwork);

	/* szd */
	container_szd_comp( win, p, wbuff, app);

	/* standard szd */
	std_szd( win, wbuff, app);

	/* redraw the oldwork area if proportional */
	if ( p->status & CS_PROPORTIONAL )
		container_grect_redraw( win, p, &oldwork, app);
	/* area extensions will be handled by subsequent AES WM_REDRAW */
}

static
void __CDECL container_fld( WINDOW *win, short wbuff[8], APPvar *app ) {
	COMPONENT *p = W_COMP(app,win);
	GRECT oldwork; /* work rect before resize */
	mt_WindGetGrect( app, win, WF_WORKXYWH, &oldwork);

	/* get the fulled rectangle */
	if( win->status & WS_FULLSIZE) {
		mt_WindGetGrect( app, win, WF_PREVXYWH, (GRECT*)&wbuff[4] );
	} else
		rc_set( (GRECT*)&wbuff[4], app->x, app->y, win->w_max, win->h_max );

	/* ensure the contents resized appropriately */
	container_szd_comp( win, p, wbuff, app);

	/* can call the std_fld() directly here */
	std_fld( win, wbuff, app);

	/* redraw the oldwork area if proportional */
	if ( p->status & CS_PROPORTIONAL )
		container_grect_redraw( win, p, &oldwork, app);
	/* area extensions will be handled by subsequent AES WM_REDRAW */
}


static
void __CDECL container_keybd( WINDOW *win, short wbuff[8], APPvar *app) {
	COMPONENT *f = mt_CompGetPtr( app, W_COMP(app,win), CF_FOCUS);
	if ( f ) {
		long buff[8];
		evntbuff2compbuff( buff, wbuff );

		buff[0] = WM_XKEYBD;   /* TODO: why this overwrite ? */
		mt_CompEvntExec(app, f, buff);
	}
}

static
void __CDECL container_click( WINDOW *win, short wbuff[8], APPvar *app) {
	COMPONENT *p = W_COMP(app,win);
	COMPONENT *f = mt_CompFind( app, p,
								app->evnt.mx, app->evnt.my);
	if ( f ) {
		long buff[8];
		evntbuff2compbuff( buff, wbuff );

		/* focus transfer */
		if ( f->status & CS_INTERACTIVE ) {
			comp_setfocus( app, p, f );
		}

		/* click */
		buff[0] = WM_XBUTTON;  /* TODO: why this overwrite ? */
		mt_CompEvntExec(app, f, buff);
	}
}


COMPONENT *win_comp_get( APPvar *app, WINDOW *win) {
	return W_COMP(app,win);
}

void win_comp_attach( APPvar *app, WINDOW *win, COMPONENT *c) {
	/* attach the component container to the WINDOW */
	mt_CompDataAttach( app, c, WD_CCRT, win ); /* attach the window to the component */
	mt_DataAttach( app, win, WD_CCRT, c ); /* attach the component to the window */

	/* set the window to fixed (not proportional from the WINDOM perspective) */
	win->xpos_max = win->ypos_max = 0;

	/* cleanup */
	mt_EvntAdd( app, win, WM_DESTROY, container_dstry, EV_TOP);

	/* recompute */
	mt_EvntAttach( app, win, CM_REFLOW, container_reflow);
	mt_EvntAttach( app, win, WM_FULLED, container_fld);
	mt_EvntAttach( app, win, WM_SIZED,  container_szd);
	mt_EvntAttach( app, win, WM_MOVED,  container_mvd);

	/* redraw */
	mt_EvntAttach( app, win, WM_REDRAW, container_redraw);
	mt_EvntAttach( app, win, WM_PREREDRAW, container_pass);

	/* others */
	mt_EvntAttach( app, win, WM_XKEYBD,  container_keybd);
	mt_EvntAttach( app, win, WM_XBUTTON, container_click);
	mt_EvntAttach( app, win, WM_XTIMER , container_timer);
}
