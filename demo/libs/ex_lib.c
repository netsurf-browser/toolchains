/*
 * Library example : handle float parameters
 */

#include <gem.h> /* for __CDECL definition */
#include <ldg.h>
#include <math.h>
#include <stdlib.h>

/*
 * double are not standard between different
 * compilers. We encapsule the double with a
 * long : each compiler makes the conversion.
 */

typedef union real {
		long dummy;
		double value;
	} Real;

/* prototypes */

char*  __CDECL information( void);
double __CDECL sinus( Real*);
double __CDECL cosinus( Real*);

/*
 * tableau de taille … modifier pour mettre toutes 
 * les addresses des proc‚dures … pr‚senter 
 */

PROC LibFunc[] = { 
"information", "/* Return a string information on the library */\n"
			   "char *information( VOID);\n", information,
"sinus", 	   "/* The function sinus */\n"
			   "double sinus( double *x);\n", sinus,
"cosinus", "/* The function cosinus */\n"
			   "double cosinus( double *x);\n", cosinus };

LDGLIB LibLdg[]={ 
				  0x0101,	/* library version */
				  3,		/* count of functions in library */
				  LibFunc,	/* function addresses */
				  "Example of library with floating parameters.",	
				  			/* Library information string */
				  0    		/* Library flags */
				};

char* __CDECL information( void) {
	return("[1][Example of math library!!!][Ok]");
}

double __CDECL sinus( Real *val) {
	return( sin(val->value));
}

double __CDECL cosinus( Real *val) {
	return( cos(val->value));
}

/*
 * Main part : communication with LDG-system
 */

int main( void) {
	if( ldg_init( LibLdg) == -1) {
		appl_init();
		form_alert( 1, "[1][This program is a LDG library][End]");
		appl_exit();
	}
	return 0;
}

