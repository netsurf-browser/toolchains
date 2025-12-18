/*
 *	Librairie Math‚matique (exemple pour LDG)
 *	fichier en tete a inclure par le client
 */

#include <ldg.h>

typedef union real {
		long dummy;
		double value;
	} Real;

extern char*  __CDECL (*mathinfo)( void);
extern double __CDECL (*sinus)( Real *);
extern double __CDECL (*cosinus)( Real *);

void init_ldg_math( LDG* ldg_math);
