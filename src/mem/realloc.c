/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * Realloc definition - This file is derived from the MintLib
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
 * $Id: realloc.c 120 2015-05-30 09:04:25Z landemarre $
 */

/* from the TOS GCC library */
/* malloc, free, realloc: dynamic memory allocation */
/* 5/2/92 sb -- modified for Heat-n-Serve C to accomodate its 16-bit size_t */
/* 5/5/92 sb -- realloc() gets its own file to reduce library drag */

#include <stddef.h>	/* for size_t */
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "globals.h"

extern int ldg_context;
extern struct mem_chunk _mchunk_free_list[];
void _bzero2	   ( void *, unsigned long);
void _bcopy2	   ( void *, void *, unsigned long);
void *my_malloc( unsigned long);

void *my_realloc( void *_r, unsigned long n) {
	struct mem_chunk *p, *q, *r = (struct mem_chunk *) _r;
	long sz;

/* obscure features: realloc(NULL,n) is the same as malloc(n)
 *  		         realloc(p, 0) is the same as free(p)
 */
	if (!r)
		return my_malloc(n);
	if (n == 0) {
		free(_r);
		return NULL;
	}
	p = r - 1;
	sz = (n + sizeof(struct mem_chunk) + 7) & ~7;

	if (p->size > sz + ((sizeof (struct mem_chunk) + 7) & ~7)) {
		/* block too big, split in two */
		q = (struct mem_chunk * )(((long) p) + sz);
		q->size = p->size - sz;
        q->valid = VAL_ALLOC;
		free(q + 1);
		p->size = sz;
	} else  if (p->size < sz) {
	/* block too small, get new one */
    	struct mem_chunk *s, *t;
    
    	q = &_mchunk_free_list[ldg_context];
	    t = _mchunk_free_list[ldg_context].next;
    	while (t != NULL && t < p) {
			q = t;
			t = t->next;
     	}

		/* merge after if possible */
		s = (struct mem_chunk * )(((long) p) + p->size);
		if (t != NULL && s >= t && p->size + t->size >= sz
			&& t->valid != VAL_BORDER) {
			assert(s == t);
			p->size += t->size;
			q->next = t->next;
			t->size = 0;
			t->next = NULL;
      	} else {
			q = (struct mem_chunk * )my_malloc(n);
			if (q != NULL) {
				n = p->size - sizeof(struct mem_chunk);
				_bcopy2(r, q, n);
				/* memcpy( q, r, n); */
				free(r);	/* free r only if we got a new block */
			}
			/* else we could try to mlalloc the rest and hope that we can merge */
			r = q;
		}
	}
  	/* else current block will do just fine */
	return((void * )r);
}
