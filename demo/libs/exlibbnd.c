/* Bindings of EX_LIB.LDG
 * Compile this module and link it with the application client
 */

#include <gem.h>
#include <ldg.h>

typedef union real {
		long dummy;
		double value;
	} Real;

char*  CDECL (*mathinfo)( void);
double CDECL (*sinus)   ( Real *);
double CDECL (*cosinus) ( Real *);

void init_ldg_math( LDG* ldg_math) {
	mathinfo = ldg_find("information", ldg_math);
	cosinus  = ldg_find("cosinus", ldg_math);
	sinus    = ldg_find("sinus", ldg_math);         
}
