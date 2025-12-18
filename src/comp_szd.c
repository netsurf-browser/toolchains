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
 * $Source: /cvsroot/windom/windom/src/comp_szd.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/01/03 14:05:47 $
 *   $Revision: 1.9 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#include "globals.h"

#define SZDDEBUG 0
#if SZDDEBUG
#define CDT_COMPID 0x01434f4cUL  /* 'COL' */
#define NFD(x) printf x; printf("\r");
#include <string.h>
static char indent[256];
#else
#define NFD(x)
#endif

/* recomputes the container's component sizes */
static short comp_szd_sizes( APPvar *app, COMPONENT *c );
static void  comp_szd_position( APPvar *app, COMPONENT *c );


static void comp_set_x( COMPONENT *c, long x ) {
	c->rect.g_x = x;
}
static void comp_set_y( COMPONENT *c, long y ) {
	c->rect.g_y = y;
}
static void comp_set_w( COMPONENT *c, long w ) {
	c->rect.g_w = w;
}
static void comp_set_h( COMPONENT *c, long h ) {
	c->rect.g_h = h;
}
static long comp_get_w( COMPONENT *c ) {
	return c->rect.g_w;
}
static long comp_get_h( COMPONENT *c ) {
	return c->rect.g_h;
}
static long comp_get_min_w( COMPONENT *c ) {
	return c->bounds.min_width;
}
static long comp_get_max_w( COMPONENT *c ) {
	return c->bounds.max_width;
}
static long comp_get_min_h( COMPONENT *c ) {
	return c->bounds.min_height;
}
static long comp_get_max_h( COMPONENT *c ) {
	return c->bounds.max_height;
}


static void comp_szd_prepare( COMPONENT *p ) {
	COMPONENT *c;

	/* clean the fix_flex flag */
	p->status &= ~CS_FIX_FLEX;

	/* all recursively */
	FOR_EACH_CHILD( c, p->children ) {
		comp_szd_prepare( c );
	}
}


/**
 * Calls a component size computation and returns 1 if its size
 * was changed (affected by min/max sizes if its children).
 **/
static short comp_boxes_child_size( APPvar *app, COMPONENT *c,
									long parentNaSize, long *loopNaSize,
									long (*get_nasize)(COMPONENT*) )
{
	/* resize the child and check whether it resized */
	if ( comp_szd_sizes( app, c ) ) {
		long nacsize = get_nasize(c);

		/* is the non-axis size different to the parent's one? */
		if ( nacsize != parentNaSize ) {
			NFD(( "  NA: %sc = %s:          size=%3ld != %3ld\n",
					  indent, (char*)mt_CompDataSearch(app, c, CDT_COMPID),
					  nacsize, parentNaSize ));


			/* the next pass will get this one */
			*loopNaSize = nacsize;
		}

		/* the size had changed -> return 1 */
		return 1;
	}

	return 0;
}


/**
 * Computes sizes of the horizontal or vertical container children
 * and handles the min/max sizes for them as well as for the parent.
 *
 * note: This is the most tricky piece on the component layout
 * handling. Modifications to this function affects the overall
 * behaviour of the horizontal and vertical component containers. The
 * order of lines are crucial in most places. Be extra careful to know
 * what you are doing when touching this.
 **/
static short comp_boxes_sizes( APPvar *app, COMPONENT *p, long *w, long *h)
{
	short result = 0; /* normally the component should not change its size */

	long orig_h = p->rect.g_h;
	long orig_w = p->rect.g_w;

	/* apply min/max limits */
	p->rect.g_w = MIN( MAX( p->rect.g_w, (long)p->bounds.min_width  ), (long)p->bounds.max_width  );
	p->rect.g_h = MIN( MAX( p->rect.g_h, (long)p->bounds.min_height ), (long)p->bounds.max_height );

	if ( !p->children ) {
		if ( orig_w == p->rect.g_w && orig_h == p->rect.g_h ) {
			/* no size change */
			return 0;
		}

		/* the proposed size doesn't fit into mix/max limits */
		return 1;
	}

	{ /* childred reflow */

	COMPONENT *c;

	long flexTiles;
	long nonAxisSize;
	long loopNaSize;
	long remainingSize;

	short passes = 0;
	short again = 1;  /* go through at least once */

	/* axis and non-axis setter/getter function setup */
	long *size;
	long *nasize;
	long (*get_size)(COMPONENT*);
	long (*get_nasize)(COMPONENT*);
	long (*get_min_size)(COMPONENT*);
	long (*get_max_size)(COMPONENT*);
	void (*set_size)(COMPONENT*,long);

	switch ( p->type ) {
		case CLT_HORIZONTAL:
			size = w;
			set_size = comp_set_w;
			get_size = comp_get_w;
			get_min_size = comp_get_min_w;
			get_max_size = comp_get_max_w;
			nasize = h;
			get_nasize = comp_get_h;
			break;
		case CLT_VERTICAL:
			size = h;
			set_size = comp_set_h;
			get_size = comp_get_h;
			get_min_size = comp_get_min_h;
			get_max_size = comp_get_max_h;
			nasize = w;
			get_nasize = comp_get_w;
			break;
	}

	/* Children are reflow according to the complete COMPONENT's size
	 * including the SLIDERINFO.max and .unit values as well (if present)
	 */
	orig_h = *h;
	orig_w = *w;

	loopNaSize = *nasize;

#if SZDDEBUG
	strcat(indent, "  ");
#endif

	/* if the child changed its size during recursive computation then
	 * we need to go through all this until no such child exists.
	 */
	for (;;) {
	theloop:
		*nasize = loopNaSize; /* update the non-axis size */

		/* no child changed its size in the previous pass */
		if ( !again ) {
			/* apply axis children size affections */
			*size -= remainingSize;

			if ( orig_w == *w && orig_h == *h ) {
				/* no size change? -> finished: break the for(;;) loop */
				break;
			}

			NFD(( "RES : %sc = %s: delta=[%3ld,%3ld]\n",
						indent, (char*)mt_CompDataSearch(app, p, CDT_COMPID),
						*w - orig_w, *h - orig_h ));

			/* the component changed it size */
			result = 1;

			orig_h = *h;
			orig_w = *w;

			/* all the children sizes need to be recomputed again
			   after parent component size change */
			comp_szd_prepare( p );

			/* but the parent is already of a corect size */
			p->status |= CS_FIX_FLEX;
		}

		/* sanity check: not to block the whole application. if this
		   is loop is issued more than some limit then something is
		   seriously wrong... -> break the loop */
		if ( ++passes > 10 ) {
			NFD(( "!   : %sc = %s:          size=[%3ld,%3ld]\n",
					  indent, (char*)mt_CompDataSearch(app, p, CDT_COMPID),
					  *w, *h ));
			break;
		}

		/* setup loop variables */
		again = 0; /* no more pass needed by default */
		flexTiles = 0; /* number of flex parts to divide the remainingSize */
		remainingSize = *size; /* to be subtracted by the children sizes */

		nonAxisSize = *nasize; /* non-axis size value */
		loopNaSize = nonAxisSize; /* loop non-axis size value */

		NFD(( "P%d  : %sc = %s:          size=[%3ld,%3ld]\n",
				  passes,indent, 
				  (char*)mt_CompDataSearch(app, p, CDT_COMPID),
				  *w, *h ));

		FOR_EACH_CHILD( c, p->children ) {
			/* check previous pass fixed flexibles... */
			if ( c->status & CS_FIX_FLEX ) {
				/* assure the opposite axis size is set properly: */

				/* backup the size */
				long csize = get_size(c);

				/* default size (parent's one) */
				c->rect.g_w = *w;
				c->rect.g_h = *h;
				/* set the axis size back */
				set_size( c, csize );

				NFD(( "FIXF: %sc = %s:          size=[%3ld,%3ld]\n",
						  indent, (char*)mt_CompDataSearch(app, c, CDT_COMPID),
						  c->rect.g_w, c->rect.g_h ));

				/* still need to reflow the children */
				again |= comp_boxes_child_size( app, c, nonAxisSize, &loopNaSize, get_nasize );
				/* do not care about further size changes */
				set_size( c, csize );

				/* remove this child from among flexible computation */
				remainingSize -= csize;
				continue;
			}

			/* default size (parent's one) */
			c->rect.g_w = *w;
			c->rect.g_h = *h;

			/* compute the remaining size to 'flex' around (subtracting preferred sizes) */
			remainingSize -= c->size;

			/* compute the *flex* children tilecount */
			flexTiles += c->flex;

			if ( !c->flex ) {
				/* for fixed objects we know the size rightaway */
				set_size( c, c->size );
				c->status |= CS_FIX_FLEX;

				NFD(( "FIX : %sc = %s:          size=[%3ld,%3ld]\n",
						  indent, (char*)mt_CompDataSearch(app, c, CDT_COMPID),
						  c->rect.g_w, c->rect.g_h ));

				again |= comp_boxes_child_size( app, c, nonAxisSize, &loopNaSize, get_nasize );

				/* compute the remaining size to 'flex' around */
				remainingSize += c->size;
				remainingSize -= get_size( c );
			}

		}

		/* quick finish when all children are of a fixed size */
		if ( flexTiles<=0 ) goto theloop;

		/* handle min/max by *flex* children (if any) */
		FOR_EACH_CHILD( c, p->children ) if ( !(c->status & CS_FIX_FLEX) ) {
			long csize;
			if ( flexTiles<=0 ) { /* DEBUG WARNING */
				printf("Wrong flexTiles???\r\n");
				flexTiles=1;
			}
			csize = c->size + (remainingSize * c->flex) / flexTiles;

			if ( csize < get_min_size(c) ) {
				c->status |= CS_FIX_FLEX;
				set_size( c, get_min_size(c) );

				NFD(( "SMIN: %sc = %s:          size=[%3ld,%3ld] (remSize=%3ld, proposed=%4ld)\n",
						  indent, (char*)mt_CompDataSearch(app, c, CDT_COMPID),
						  c->rect.g_w, c->rect.g_h, remainingSize, csize ));

				/* remove this child from among flexible computation */
				flexTiles -= c->flex;
				again |= comp_boxes_child_size( app, c, nonAxisSize, &loopNaSize, get_nasize );
				remainingSize += c->size;
				remainingSize -= get_size( c );

				NFD(( "SMIN/ %sc = %s:          size=[%3ld,%3ld] (remSize=%3ld) \n",
						  indent, (char*)mt_CompDataSearch(app, c, CDT_COMPID),
						  c->rect.g_w, c->rect.g_h, remainingSize ));
				continue;
			}
			if ( csize > get_max_size(c) ) {
				c->status |= CS_FIX_FLEX;
				set_size( c, get_max_size(c) );

				NFD(( "SMAX: %sc = %s:          size=[%3ld,%3ld] (proposed=%4ld)\n",
						  indent, (char*)mt_CompDataSearch(app, c, CDT_COMPID),
						  c->rect.g_w, c->rect.g_h, csize ));

				/* remove this child from among flexible computation */
				flexTiles -= c->flex;
				again |= comp_boxes_child_size( app, c, nonAxisSize, &loopNaSize, get_nasize );
				remainingSize += c->size;
				remainingSize -= get_size( c );

				NFD(( "SMAX/ %sc = %s:          size=[%3ld,%3ld]\n",
						  indent, (char*)mt_CompDataSearch(app, c, CDT_COMPID),
						  c->rect.g_w, c->rect.g_h ));
				continue;
			}
		}

		/* quick finish when all children are of a fixed size */
		if ( flexTiles<=0 ) goto theloop;

		/* compute the childrenSize and set the size of the remaining *flex* boxes */
		FOR_EACH_CHILD( c, p->children ) if ( !(c->status & CS_FIX_FLEX) ) {
			/* all the remaining non-min/max limited flex children */
			long csize;
			if ( flexTiles<=0 ) { /* DEBUG WARNING */
				printf("Wrong flexTiles???\r\n");
				flexTiles=1;
			}
			csize = c->size + (remainingSize * c->flex) / flexTiles;
			set_size( c, MAX(0,csize) ); /* size should not be negative */

			NFD(( "FLEX: %sc = %s:          size=[%3ld,%3ld] (remSize=%ld, flex=%hd, ft=%ld, flexPart=%ld)\n",
					  indent, (char*)mt_CompDataSearch(app, c, CDT_COMPID),
					  c->rect.g_w, c->rect.g_h, remainingSize, c->flex, flexTiles, (remainingSize * c->flex) / flexTiles));

			/* subtract from the remainingSize
			 *
			 * note: this is important because of otherwise we face
			 * rounding problems caused by the *flex* csize computation
			 * formula (multiplication/division combination)
			 */
			flexTiles -= c->flex;
			again |= comp_boxes_child_size( app, c, nonAxisSize, &loopNaSize, get_nasize );
			remainingSize += c->size;
			remainingSize -= get_size( c );

			NFD(( "FLEX/ %sc = %s:          size=[%3ld,%3ld] (remSize=%ld, flex=%hd, ft=%ld, again=%d)\n",
					  indent, (char*)mt_CompDataSearch(app, c, CDT_COMPID),
					  c->rect.g_w, c->rect.g_h, remainingSize, c->flex, flexTiles, again));
		}
	}; /* for(;;) */

	NFD(( "P%d/ : %sc = %s:          size=[%3ld,%3ld] resized=%d\n",
			  passes,indent, 
			  (char*)mt_CompDataSearch(app, p, CDT_COMPID),
			  *w, *h, result ));

#if SZDDEBUG
	indent[strlen(indent)-2] = '\0';
#endif
	} /* childred reflow */

	return result;
}

static short comp_szd_sizes( APPvar *app, COMPONENT *c ) {
	short res;
	long  w, h, *wptr, *hptr;
	C_SLIDERINFO *hslid = NULL;
	C_SLIDERINFO *vslid = NULL;

	/* slider positions */
	if ( c->status & CS_HSLIDE ) {
		hslid = HSLIDERINFO(app, c);
		w = hslid->max * hslid->unit;
		wptr = &w;
	} else {
		wptr = &c->rect.g_w;
	}
	if ( c->status & CS_VSLIDE ) {
		vslid = VSLIDERINFO(app, c);
		h = vslid->max * vslid->unit;
		hptr = &h;
	} else {
		hptr = &c->rect.g_h;
	}
	
	NFD(( "R   : %sc = %s:          size=[%3ld,%3ld]\n",
		indent, 
		(char*)mt_CompDataSearch(app, c, CDT_COMPID),
		*wptr, *hptr ));

	switch ( c->type ) {
		case CLT_HORIZONTAL:
		case CLT_VERTICAL:
			res = comp_boxes_sizes( app, c, wptr, hptr);
			break;
		case CLT_STACK:
			/* fallthrough, children do not move */
			/* recompute the children recursively */
			{
				COMPONENT *ch; FOR_EACH_CHILD( ch, c->children ) {
					res = comp_szd_sizes( app, ch);
				}
			}
			break;
		default:
			res = 0;
	}

	if ( res ) {
		/* update the slid->max valued in case the size changed */
		if ( hslid ) {
			long old = hslid->max;
			hslid->max = ( w + hslid->unit - 1 ) / hslid->unit;
			res |= (hslid->max != old ? 1 : 0);
		}
		if ( vslid ) {
			long old = vslid->max;
			vslid->max = ( h + vslid->unit - 1 ) / vslid->unit;
			res |= (vslid->max != old ? 1 : 0);
		}
	}

	NFD(( "R/  : %sc = %s:          size=[%3ld,%3ld]\n",
		indent, 
		(char*)mt_CompDataSearch(app, c, CDT_COMPID),
		*wptr, *hptr ));

	/* res indicates that the component changed it size by a value of 1 */
	return res;
}


static void comp_flex_position( APPvar *app, COMPONENT *p,
								 void (*set_begin)(COMPONENT*,long),
								 long (*get_size)(COMPONENT*) )
{
	COMPONENT *c;
	long headDistance=0;

	FOR_EACH_CHILD( c, p->children ) {
		long csize = get_size( c );

		NFD(( "   H: %sc = %s: pos=%4ld size=%4ld [%3ld,%3ld]\n",
				  indent, (char*)mt_CompDataSearch(app, c, CDT_COMPID),
				  headDistance, csize, c->rect.g_w, c->rect.g_h ));

		c->rect.g_x = 0;
		c->rect.g_y = 0;

		set_begin( c, headDistance );
		headDistance += csize;

		/* recompute the children recursively */
		comp_szd_position(app, c);
	}
}

static void comp_szd_position( APPvar *app, COMPONENT *p ) {
	long buff[8];

	/* fire the CM_REPOSED event for each COMPONENT */
	mt_CompGetLGrect(app, p, WF_WORKXYWH, (LGRECT*)&buff[4] );
	buff[0] = CM_REPOSED;
	/* *(LGRECT*)&buff[4] = p->rect; */
	mt_CompEvntExec(app, p, buff);

	/* reflow children */
	switch ( p->type ) {
		case CLT_HORIZONTAL:
			comp_flex_position( app, p, comp_set_x, comp_get_w );
			break;
		case CLT_VERTICAL:
			comp_flex_position( app, p, comp_set_y, comp_get_h );
			break;
		case CLT_STACK:
			/* fallthrough, nothing to do here as children are stacked
			 * (fixed position and size) */

			/* recompute the children recursively */
			{
				COMPONENT *c; FOR_EACH_CHILD( c, p->children ) {
					comp_szd_position(app, c);
				}
			}
			break;
		default:
			;
	}
}


COMPONENT *comp_reflow( COMPONENT *c, APPvar *app ) {
	COMPONENT *p;
	short res;

#if SZDDEBUG
	strcpy(indent,"");
#endif

	/* clean the fix_flex flag by every component */
	comp_szd_prepare(c);

	/* component reflow might end up resizing parents
	 * all the way up to the root COMPONENT
	 */
	do {
		p = c; /* save */

		/* compute the sizes */
		res = comp_szd_sizes(app, p);
		if ( !res ) break;
	} while ( (c = c->parent) );

	/* root COMPONENT changed its size
	 * -> one more pass */
	if ( res ) comp_szd_sizes(app, p);

	/* now the sizes are already computed
	 * reposition them all */ 
	comp_szd_position(app, p);

	/* return the top-most reflown COMPONENT instance */
	return p;
}
