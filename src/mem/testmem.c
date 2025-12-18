#include <stdio.h>
#include <stdlib.h>

#ifdef __PUREC__
#include <ldg\mem.h>
#else
#include <ldg/mem.h>
#endif

int ldg_mem_init( char *lib, int vers, LDG **mem);

long value = 1000;

int main( int argc, char *argv[]) {
	int i;
	void *buf;
	LDG *mem;
	char **list;
	
	appl_init();
	ldg_mem_init( "..\\..\\gemsys\\ldg\\mem.ldg", 0, &mem);

	printf( "loading MEM version %x.%02x\r\n", mem->vers>>8,mem->vers&0xFF);

	ldg_allocmode( 1);
	
	if( argc > 1) value = atol( argv[1]);
	if( argc > 2 && atoi( argv[2]) == 2) {
		printf( "Allocating a list of %ld items ...\r\n", value);
		list = (char **)ldg_malloc( value*sizeof(char*));
		for( i=0; i<value; i++)
			list[i] = ldg_strdup("Hello world");

		system( "..\\..\\bin\\ldginfo -t");
		system( "ls -l U:\\shm");
		evnt_timer( 10000);
		
		for( i=0; i<value; i++)
			ldg_free( list[i]);
		ldg_free(list);	

	} else {
		printf( "Allocating %ld bytes ...\r\n", value);	
		buf = (void *)ldg_malloc( value);
	
		system( "..\\..\\bin\\ldginfo -t");
		system( "ls -l U:\\shm");
		evnt_timer( 10000);
	
		ldg_free( buf);
	}
	
	ldg_close( mem, ldg_global);
	appl_exit();
	return 0;
}

