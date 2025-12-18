/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * Malloc,Free definition - This file is derived from the MintLib
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
 * $Id: malloc.c 120 2015-05-30 09:04:25Z landemarre $
 */

/* from the TOS GCC library */
/* malloc, free, realloc: dynamic memory allocation */
/* ERS: added mlalloc, relalloc, etc. for 16 bit compilers. Changed
   argument of malloc, etc.,  to size_t (per ANSI draft). */
/* 5/2/92 sb -- modified for Heat-n-Serve C to accomodate its 16-bit size_t */
/* 5/5/92 sb -- split off realloc() & calloc() to reduce library drag */

#include <compiler.h>
#include <stddef.h>	/* for size_t */
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "globals.h"

extern int ldg_context;
extern void *_heapbase[];
extern short _split_mem[];
extern long _stksize[];
extern struct mem_chunk _mchunk_free_list[];
extern int _ZeroMallocs[];

/* minimum chunk to ask OS for */
static size_t MINHUNK =	4096L;	/* default */
static size_t MAXHUNK = 32*1024L; /* max. default */

void*	my_sbrk		( long);				/* SBRK.C */
int 	getpagesize	( void);				/* GETPAGES.C */
void 	_bzero2		( void *, unsigned long);
void 	OS_Free		( void *);


	/* CAUTION: use _mallocChunkSize() to tailor to your environment,
		    do not make the default too large, as the compiler
		    gets screwed on a 1M machine otherwise (stack/heap clash)
	 */


void * my_malloc( unsigned long n) {
	struct mem_chunk *p, *q;
	long sz;

	/* add a mem_chunk to required size and round up */
	n = n + sizeof(struct mem_chunk);
	n = (7 + n) & ~7;

	/* look for first block big enough in free list */
	p = &_mchunk_free_list[ldg_context];
	q = _mchunk_free_list[ldg_context].next;

	while ((q != NULL) && (q->size < n || q->valid == VAL_BORDER)) {
		p = q;
		q = q->next;
	}

	/* if not enough memory, get more from the system */
	if (q == NULL) {
		if ( ((!_split_mem[ldg_context]) && (_heapbase[ldg_context] != NULL))
	    	 || n + BORDER_EXTRA > MINHUNK) {
			sz = n;
			if (_heapbase[ldg_context] == NULL)
			  sz += BORDER_EXTRA;
		} else {
			sz = MINHUNK;
			if (MINHUNK < MAXHUNK)
				MINHUNK *= 2;
		}
		if (_split_mem[ldg_context] || _heapbase[ldg_context] == NULL) {
			static int page_size = 0;

			if (!page_size)
				page_size = getpagesize ();
	    	sz = (sz + page_size - 1) & -page_size;
		}
		q = (struct mem_chunk * ) my_sbrk(sz);

		if (((long)q) == -1) 		/* can't alloc any more? */
			return(NULL);

		/* Note: q may be below the highest allocated chunk */
		p = &_mchunk_free_list[ldg_context];
		while (p->next != NULL && q > p->next)
			p = p->next;
		if (_heapbase[ldg_context] == NULL) {
	 		q->size = BORDER_EXTRA;
	    	sz -= BORDER_EXTRA;
	    	q->valid = VAL_BORDER;
	    	ALLOC_SIZE (q) = sz;
	    	q->next = (struct mem_chunk *) ((long) q + BORDER_EXTRA);
	    	q->next->next = p->next;
	    	p = p->next = q;
	    	q = q->next;
		} else {
	    	q->next = p->next;
	    	p->next = q;
		}
		q->size = sz;
		q->valid = VAL_FREE;
	}
		
  	if (q->size > n + sizeof(struct mem_chunk)) {	
		/* split, leave part of free list */
		q->size -= n;
		q = (struct mem_chunk * )(((long) q) + q->size);
		q->size = n;
		q->valid = VAL_ALLOC;
	} else {
		/* just unlink it */
		p->next = q->next;
		q->valid = VAL_ALLOC;
	}

	q->next = NULL;	
 	q++;	/* hand back ptr to after chunk desc */
	if(_ZeroMallocs[ldg_context] != 0)
    	_bzero2((void *)q, (long)(n - sizeof(struct mem_chunk)));
  
	return((void * )q);
}

void my_free( void *param) {
	struct mem_chunk *o, *p, *q, *s;
	struct mem_chunk *r = (struct mem_chunk *) param;

	/* free(NULL) should do nothing */
	if (r == 0)
		return;

	/* move back to uncover the mem_chunk */
	r--;			/* there it is! */

	if (r->valid != VAL_ALLOC)
		return;

 	r->valid = VAL_FREE;

	/* stick it into free list, preserving ascending address order */
	o = NULL;
	p = &_mchunk_free_list[ldg_context];
	q = _mchunk_free_list[ldg_context].next;
	while (q != NULL && q < r) {
		o = p;
		p = q;
		q = q->next;
	}

	/* merge after if possible */
	s = (struct mem_chunk * )(((long) r) + r->size);
 	if (q != NULL && s >= q && q->valid != VAL_BORDER) {
		assert(s == q);
		r->size += q->size;
		q = q->next;
		s->size = 0;
		s->next = NULL;
	}
	r->next = q;
	
	/* merge before if possible, otherwise link it in */
	s = (struct mem_chunk * )(((long) p) + p->size);
 	if (s >= r && p != &_mchunk_free_list[ldg_context]) {
	    /* remember: r may be below &_mchunk_free_list[ldg_context] in memory */
		assert(s == r);
		if (p->valid == VAL_BORDER) {
	    	if (ALLOC_SIZE (p) == r->size) {
				o->next = r->next;
				OS_Free (p);
	      	} else
	    		p->next = r;
	  		return;
	  	}
		p->size += r->size;
		p->next = r->next;
		r->size = 0;
		r->next = NULL;
		s = (struct mem_chunk * )(((long) p) + p->size);
		if ((!_split_mem[ldg_context]) && _heapbase[ldg_context] != NULL &&
		    s >= (struct mem_chunk *) _heapbase[ldg_context] &&
	    	s < (struct mem_chunk *) ((char *)_heapbase[ldg_context] + _stksize[ldg_context])) {
			assert(s == (struct mem_chunk *) _heapbase[ldg_context]);
			_heapbase[ldg_context] = (void *) p;
			_stksize[ldg_context] += p->size;
			o->next = p->next;	/* o is always != NULL here */
		} else if (o->valid == VAL_BORDER && ALLOC_SIZE (o) == p->size) {
			q = &_mchunk_free_list[ldg_context];
			s = q->next;
			while (s != NULL && s < o) {
				q = s;
				s = q->next;
	  		}
	    	if (s) {
				assert (s == o);
				q->next = p->next;
				OS_Free (o);
	    	}
		}
	} else {
		s = (struct mem_chunk * )(((long) r) + r->size);
		if ((!_split_mem[ldg_context]) && _heapbase[ldg_context] != NULL &&
	    	s >= (struct mem_chunk *) _heapbase[ldg_context] &&
	    	s < (struct mem_chunk *) ((char *)_heapbase[ldg_context] + _stksize[ldg_context])) {
			assert(s == (struct mem_chunk *) _heapbase[ldg_context]);
			_heapbase[ldg_context] = (void *) r;
			_stksize[ldg_context] += r->size;
			p->next = r->next;
		} else
			p->next = r;
	}
}

void *my_calloc( unsigned long n, unsigned long sz) {
	void *r;
	unsigned long total;

	total = n * sz;
	if ((r = my_malloc(total)) != NULL)
		_bzero2(r, total);
  	return(r);
}

/*
 * Set zero block after malloc flag
 */

void _malloczero( int yes) {
    _ZeroMallocs[ldg_context] = yes;
}

/*
 * tune chunk size
 */
void _mallocChunkSize ( size_t siz) {
    MAXHUNK = MINHUNK = siz;
}
