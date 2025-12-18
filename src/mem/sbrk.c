/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * Emulation of unix call sbrk() - This file is derived from the MintLib
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
 * $Id: sbrk.c 9 2005-07-25 19:37:25Z bercegeay $
 */

/* sbrk: emulate Unix sbrk call */
/* by ERS */
/* jrb: added support for allocation from _heapbase when _stksize == -1 
	thanks to Piet van Oostrum & Atze Dijkstra for this idea and
        their diffs. */

/* WARNING: sbrk may not allocate space in continguous memory, i.e.
   two calls to sbrk may not return consecutive memory. This is
   unlike Unix.
*/

/* Further WARNING: in a split_mem model the memory addresses will NOT
   be monotonous. sigh!  (i hate these mem models as much as the next
   person. as usual, people at atari are totally oblivious to such
   brain damage, even when pointed out to them. sigh!)
*/

/*
 * support heat and serve C -- whose author continues to be adamant about
 * size_t -- big sigh!
 */

#include <stddef.h>

void *OS_Malloc( long size);
void *my_sbrk( long n);

extern int errno;			/* for stdlib */
extern void *_heapbase[];
extern long _stksize[];
extern short _split_mem[];
extern int ldg_context;

static void * HeapAlloc( unsigned long sz ) {
    char slush [64];
    register void *sp;
    
    sp = (void *)slush;
    sz = (sz + 7) & ~((unsigned long)7L); /* round up request size next octet */

    if ( sp < (void *)((char *)_heapbase[ldg_context] + sz) )
		return NULL;
    sp = _heapbase[ldg_context];
    _heapbase[ldg_context] = (void *)((char *)_heapbase[ldg_context] + sz);
    _stksize[ldg_context] -= (long)sz;
    
    return( sp );
}

/* provided for compilers with sizeof(int) == 2 */

void *my_sbrk( long n) {
	void *rval;

	if((!_split_mem[ldg_context]) && (_heapbase[ldg_context] != NULL)) {
		if(n) rval = HeapAlloc(n);
		else  rval = _heapbase[ldg_context];
	} else
		rval = OS_Malloc(n);
		
	if (rval == NULL) {
		if(_split_mem[ldg_context]) {
      		/* now switch over to own heap for further requests, including this one */
	  		_split_mem[ldg_context] = 0;
	  		return my_sbrk(n);
      	}
      	errno = 39 /*ENOMEM */;
      	rval = (void *)(-1L);
  	}
  	return rval;
}

