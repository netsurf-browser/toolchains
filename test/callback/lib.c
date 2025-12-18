/*
 *  test program for ldg_callback
 *
 *  (c)2005 Arnaud BERCEGEAY
 *
 *  $Id: lib.c 40 2005-11-02 21:15:29Z bercegeay $
 */

#include <stdio.h>
#include <compiler.h>  /* for purec __CDECL */
#include <ldg.h>
#include "interface.h"

#define APP_PRINT(s)  ldg_callback(appstuff->print2stdout,s)

void __CDECL libfunction ( long a, long b, APP_STUFF *appstuff) {
	long result;
	char buff[128];
	
	
	APP_PRINT("This is from inside the LDG library\n");
#ifdef __SOZOBONC__
	APP_PRINT("compiled by SOZOBONC on");
#endif
#ifdef __PUREC__
	APP_PRINT("compiled by PUREC on");
#endif
#ifdef __GNUC__
	APP_PRINT("compiled by GCC on");
#endif
	APP_PRINT( " " __DATE__  " " __TIME__ "\n");
	
	sprintf(buff,"paramters: a=%ld b=%ld\n",a,b);
	APP_PRINT(buff);
	
	APP_PRINT("local computation of a2+b2+2ab gives : ");
	result = a*a + b*b + 2*a*b;
	sprintf(buff,"%ld\n",result);
	APP_PRINT(buff);
	
	APP_PRINT("APPLI computation of a2+b2+2ab gives : ");
	result = ldg_callback(appstuff->compute_a2plusb2plus2ab,a,b);
	sprintf(buff,"%ld\n",result);
	APP_PRINT(buff);
	
	APP_PRINT("end of the LDG function.\n");
}


PROC LibFunc[] = { 
	{"libfunction",
	"/* test function */\n"
	"void libfunction( long, long, APP_STUFF*);\n",
	libfunction}
};

LDGLIB LibLdg[]={{ 
				  0x0100,					/* library version */
				  1,						/* count of functions in library */
				  LibFunc,					/* function addresses */
				  "test of ldg_callback.",  /* Library information string */
				  0 						/* Library flags */
				}};


int main( void) {
	ldg_init( LibLdg);
	return 0;
}
