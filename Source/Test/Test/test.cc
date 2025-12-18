// Index: OSLib/!OsLib/Source/Test/Test/test.cc		20031228

/* 1. test inclusion of all OSLib headers in a C++ build
** 2. test bool features in types.h
** 3. test OSLib Namespace
*/

#if 0
   #define BOOL_DEFINED
   typedef unsigned char bool;  // accepted under CFront; faulted under g++
#endif

#ifdef __CC_NORCROFT
#  include <stdlib.h>
#  include <iostream.h>
#  define STDNAMESPACE
#else
#  include <cstdlib>
#  include <iostream>
#  define STDNAMESPACE std::
#endif

/* Load all OSLib C headers as one big header to catch potentional clashes.  */
#include "all_oslib.h"

#if defined NAMESPACE_OSLIB
   // using namespaces
#  define TEST Test::
   namespace Test
   {
     /* a function in our local namespace */
     void os_write0( const char *text )
     {
       OSLib::os_write0( "** " );
       OSLib::os_write0( text );
     }
   }

#  if 1
      // use the |using| directive
#    define OSLIB
     using namespace OSLib;
#  else
      // use qualifiers
#    define OSLIB OSLib::
#  endif
#else
   // no namespaces
#  define OSLIB
#endif

int main()
{
#if defined __CC_NORCROFT
   const    char* text = "Hello world from Norcroft via CFront";
#elif defined __GNUC__
   const    char* text = "Hello world from G++";
#else
   const    char* text = "Hello world from Unidentifiecd Compiler";
#endif

#if defined TEST_BOOL
   OSLIB os_write0( "__cplusplus is " );
#  if !defined __cplusplus
   OSLIB os_write0( "not " );
#  endif
   OSLIB os_write0( "defined\r\n" );

   OSLIB os_write0( "\nBOOL_DEFINED is " );
#  if !defined BOOL_DEFINED
   OSLIB os_write0( "not " );
#  endif
   OSLIB os_write0( "defined\r\n" );

   OSLIB os_write0( "bool is " );
#  if !defined BOOL_IS_OSBOOL
   OSLIB os_write0( "not " );
#  endif
   OSLIB os_write0( "osbool\r\n" );

   OSLIB os_write0( "NAMESPACE_OSLIB is " );
#  if !defined NAMESPACE_OSLIB
   OSLIB os_write0( "not " );
#  endif
   OSLIB os_write0( "defined\r\n" );

   bool spock = TRUE;
   OSLIB osbool kirk = TRUE;

   const int BUF_SIZ = 16;
   char buf[BUF_SIZ];
   int used;

   used = OSLIB os_binary_to_decimal( sizeof( spock ), buf, BUF_SIZ-1 );
   buf[used] = '\0';
   OSLIB os_write0( "size of   bool is " );
   OSLIB os_write0( buf );
   OSLIB os_writec( '\n' );
   OSLIB os_writec( '\r' );

   used = OSLIB os_binary_to_decimal( sizeof( kirk ), buf, BUF_SIZ-1 );
   buf[used] = '\0';
   OSLIB os_write0( "size of osbool is " );
   OSLIB os_write0( buf );
   OSLIB os_writec( '\n' );
   OSLIB os_writec( '\r' );
#endif

   // just to prove it's C++
   STDNAMESPACE cout << text << STDNAMESPACE endl;

   return EXIT_SUCCESS;
}
