/* test librairie static */

extern int ldg_context ;

void* ldg_malloc( long size) {
	ldg_context = 0;
	return my_malloc(size);
}

void* ldg_free( void *adr) {
	ldg_context = 0;
	return my_free(adr);
}
