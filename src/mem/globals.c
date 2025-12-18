/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * Global variables
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
 * $Id: globals.c 9 2005-07-25 19:37:25Z bercegeay $
 */

#include <stddef.h>
#include "globals.h"

void *_heapbase[LDG_MAXCLIENT];		/* "0" means: heap not yet def'd. see crtinit.c */

/* are we on a split addr mem ST */
short _split_mem [LDG_MAXCLIENT];

/*	The (new) meaning of _stksize: (thanks to allan pratt for the feedback)
 *
 *	_stksize			meaning
 *	  -1L	keep all of memory (except MINFREE at top) and do
 *		    mallocs from own heap, with heap grown upwards towards
 *		    stack, and the stack growing down towards heap,
 *		    with a minimum slush between them so that they
 *		    dont meet (only checked while malloc'ing). With
 *		    this model, further spawning is not possible, but it is
 *		    well suited for programs such as gcc-cc1 etc.
 *		    Thanks to Piet van Oostrum & Atze Dijkstra for this idea
 *
 *	0L	    keep minimum amount of memory. this is also the
 *		    case when _stksize is undefined by the user.
 *	1L	    keep 1/4 of memory, free 3/4 ( as in Alcyon GEMSTART)
 *	2L	    keep 2/4 (1/2), free rest
 *	3L	    keep 3/4, free 1/4
 *	other	    keep that many bytes
 *	-other	    keep |other| bytes, use the heap for mallocs
 */

long _stksize [LDG_MAXCLIENT];	/* "0" means keep minimum amount: see crtinit.c */

/* linked list of free blocks struct defined in lib.h */
struct mem_chunk _mchunk_free_list[LDG_MAXCLIENT];

/* flag to control zero'ing of malloc'ed chunks */
int _ZeroMallocs[LDG_MAXCLIENT];

int __mint = 0;			/* MiNT version : initalised by a get_cookie( 'MiNT', &__mint); */

int __magic = 0;

int ldg_context = 0;	/* client concern‚ */

short mxalloc[LDG_MAXCLIENT];		/* Mxalloc() mode with MiNT/MagiC */

/* LDG init function */

void init_global_vars( void) {
	int i;
	for( i=0; i<LDG_MAXCLIENT; i++) {
		_stksize[i] = 0;
		_heapbase[i] = 0;
		_split_mem[i] = 0;
		_mchunk_free_list[i].valid = VAL_FREE;
		_mchunk_free_list[i].next  = NULL;
		_mchunk_free_list[i].size  = 0L;
		_ZeroMallocs[i] = 0;
		mxalloc[i] = 0;
	}
}

