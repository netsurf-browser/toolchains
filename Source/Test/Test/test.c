/* test a new OSLib build by #includeing all headers
// ensure this stays up-to-date
*/

/* test the bool feature in types.h
** if BOOL_DEFINED, then bool == unsigned char
** else bool == osbool
*/
#undef TEST_BOOL

//  typedef unsigned char bool; /* should generate an error */

//#define BOOL_DEFINED

#if defined BOOL_DEFINED
  typedef unsigned char bool; /* should be allowed */
#endif

/* Load all OSLib C headers as one big header to catch potentional clashes.  */
#include "all_oslib.h"

#define BUF_SIZ 64

extern void EnumCSD(void);

int main (void)
{
#if defined TEST_BOOL
   bool kirk;
   osbool spock;
   char buf[BUF_SIZ];
#endif

#if defined __CC_NORCROFT
   os_write0( "Hello World from Norcroft\n\r" );
#elif defined __GNUC__
   os_write0( "Hello World from GCC\n\r" );
#else
   os_write0( "Hello world from Unidentifiecd Compiler\n\r" );
#endif

#if defined TEST_BOOL
   os_writec( '\n' );
   os_write0( "BOOL_DEFINED is " );
#  if !defined BOOL_DEFINED
   os_write0( "not " );
#  endif
   os_write0( "defined\r\n" );

#  if defined BOOL_IS_OSBOOL
   os_write0( "bool is osbool\r\n" );
#  endif

   os_binary_to_decimal( sizeof( kirk ), buf, BUF_SIZ-1 );
   os_write0( "size of   bool is " );
   os_write0( buf );
   os_writec( '\n' );
   os_writec( '\r' );

   os_binary_to_decimal( sizeof( spock ), buf, BUF_SIZ-1 );
   os_write0( "size of osbool is " );
   os_write0( buf );
   os_writec( '\n' );
   os_writec( '\r' );
#endif

   EnumCSD();

   return 0;
}
