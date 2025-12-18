/*
 *
 */
#include  <stdio.h>
#include  "globals.h"

#ifdef __PUREC__
#define CDECL cdecl
#else
#include <compiler.h>
#define CDECL __CDECL
#endif

extern struct mem_chunk _mchunk_free_list[];

long cdecl getusedmem( long context) {
	/* D'abord du verbose */
	struct mem_chunk *q;
	
	q = &_mchunk_free_list[context];
	
	/* while ((q != NULL) && (q->size < n || q->valid == VAL_BORDER)) { */
	while( q) {
		if(  q->valid == VAL_FREE)
			printf( "Bloc VAL_FREE size %ld\n",   q->size);
		else if(  q->valid == VAL_ALLOC)
			printf( "Bloc VAL_ALLOC size %ld\n",  q->size);
		else if(  q->valid == VAL_BORDER)
			printf( "Bloc VAL_BORDER size %ld\n", q->size);
		else
			printf( "Bloc 0x%lX size %ld\n", q->valid, q->size);
		q = q->next;
	}
	return 0L;
}

