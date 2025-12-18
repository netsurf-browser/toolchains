/* ToLower
 * Change the contents of a system variable to lower case
 * Tony van der Hoff 20031218
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <kernel.h>
#include <swis.h>

#ifndef DEBUG
#define DEBUG 0
#endif

int main( int argc, char *argv [])
{
   int rc = EXIT_FAILURE;

   if( argc == 2 )
   {
      int size=0, next=0, type=0;

      // get the size
#if 0
      _swix( OS_ReadVarVal,
                  _IN(0)  | _IN(1)  | _IN(2)  | _IN(3)  | _IN(4)  |
                  _OUT(2) | _OUT(3) | _OUT(4),
                  argv[1], 0, -1, 0, 0,
                  &size, &next, &type
                 );
#else
      {
         _kernel_swi_regs r;
         r.r[0] = (int)argv[1];
         r.r[1] = 0;
         r.r[2] = -1;
         r.r[3] = 0;
         r.r[4] = 0;
         _kernel_swi( OS_ReadVarVal, &r, &r );
         size = r.r[2];
         next = r.r[3];
         type = r.r[4];
      }
#endif

      if( size < 0 )
      {
         int i;

         // allocate a buffer
         size = (~size);

#if DEBUG
         printf( "size = %d\n", size);
#endif

         char* var = malloc( size + 1 );

         // get the current value
         _swi( OS_ReadVarVal,
                  _IN(0)  | _IN(1)  | _IN(2)  | _IN(3)  | _IN(4)  |
                  _OUT(2) | _OUT(3) | _OUT(4),
                  argv[1], var, size, 0, 0,
                  &size, &next, &type
             );


         // terminate the string
         *(var + size ) = '\0';

#if DEBUG
         printf( "initial var = %s\n", var );
#endif

         // convert to lower case
         for ( i = 0; i < strlen( var ); i++ )
            *(var+i) = tolower( *(var + i) );

#if DEBUG
         printf( "final var = %s\n", var );
#endif

         // re-write the var
         _swi( OS_SetVarVal,
                  _IN(0)  | _IN(1)  | _IN(2)  | _IN(3)  | _IN(4)  |
                  _OUT(3) | _OUT(4),
                  argv[1], var, size, 0, 0,
                  &next, &type
             );

         free( var );
         rc = EXIT_SUCCESS;
      }
   }

   return rc;
}
