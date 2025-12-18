/*
 *  test program for ldg_callback
 *
 *  (c)2005 Arnaud BERCEGEAY
 *
 *  $Id: client.c 39 2005-11-02 21:11:29Z bercegeay $
 */

#include <stdio.h>
#include <gem.h>
#include <ldg.h>
#include <string.h>
#include "interface.h"

/********* interface between application and libraries ***********/

static long __CDECL a2pb2p2ab( long a, long b) {
	return (a*a + b*b + 2*a*b);
}

static void __CDECL print2stdout( const char *s) {
#ifdef __SOZOBONX__
	fputs(s, stdout);
#else
	/* fwrite() to stdout is bugged for sozobon */
	fwrite( s, strlen(s), 1, stdout);
#endif
}

static APP_STUFF my_app_stuff = { a2pb2p2ab, print2stdout };


/********* identification of the application ********************/

static void print_client_info( void) {
	printf("test of ldg_stdcall()\n");
	printf("compiled: " __DATE__ __TIME__ "\n");
#ifdef __SOZOBONC__
	printf("==> SOZOBON C version <== \n");
#endif
#ifdef __PUREC__
	printf("==> PURE C version <== \n");
#endif
#ifdef __GNUC__
	printf("==> GCC version <== \n");
#endif
}

/********* test a library ***************************************/

static void test_lib( const char *filename) {
	LDG * ldg;
	void __CDECL (*libfunction)( long, long, APP_STUFF*);
	
	printf("\nloading <%s>... ",filename);
	ldg = ldg_open( filename, ldg_global);
	if (ldg) printf("ok\n");
	else {   printf("ERROR\n"); return; }
	
	printf("finding libfunction()... ");
	libfunction = ldg_find("libfunction",ldg);
	if (libfunction) printf("ok\n");
	else {           printf("ERROR\n"); ldg_close(ldg,ldg_global); return; }
	
	libfunction(5,6,&my_app_stuff);
	
	ldg_close(ldg,ldg_global);
}




int main( void)
{
	appl_init();
	
	print_client_info();
	
	test_lib("lib-gcc.ldg");
	test_lib("lib-sox.ldg");
	test_lib("lib-pc.ldg");
	
	appl_exit();
	
	return 0;
}
