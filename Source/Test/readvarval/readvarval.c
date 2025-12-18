/* test.readVarVal.c  Test os_read_var_val_size interface
*/
//#define DEBUG
//#define OUTPUT
//#define SHORT

#ifndef OUTPUT
#define OUTPUT 0
#endif

#include <stdio.h>
#include <stdlib.h>
#include "oslib/os.h"

int main()
{
   	int iSize;
	os_var_type xType;
	int iStat = 0;

	os_error *Error= 0;
	int Context = 0;

	int vartype_number = 0;
	int vartype_macro = 0;
	int vartype_string = 0;

   	do
   	{
#ifdef DEBUG
		printf( "Read var val size; context = 0x%8x\n" _ Context );
#endif
		Error = xos_read_var_val_size( "*",
                                       		Context,
                                       		os_VARTYPE_STRING,
                                       		&iSize,
                                       		&Context,
                                       		&xType
                                     		);
      		if( !Error && iSize != 0 )
      		{
#ifdef SHORT
         		printf(  "Found: %s (size %04x; type %x)\n",
                  		(char*)Context,
                  		~iSize,
                  		(int) xType
               		);
#else
         if (  xType == os_VARTYPE_STRING
               || xType == os_VARTYPE_NUMBER
               || xType == os_VARTYPE_MACRO
            )
         {
            int iUsed;
            char* buf;

            iSize = (~iSize) + 1;
            buf = malloc( iSize );
            Error = xos_read_var_val( (char*)Context,
                                        buf,
                                        iSize,
                                        0, // Context,
                                        os_VARTYPE_STRING,
                                        &iUsed,
                                        0, // &Context,
                                        &xType
                                      );
            if( !Error )
            {
               switch ( xType )
               {
               	case os_VARTYPE_NUMBER:
               		++vartype_number;
               		break;

               	case os_VARTYPE_MACRO:
               		++vartype_macro;
               		break;

		case os_VARTYPE_STRING:
			++vartype_string;
			break;
               }
#if OUTPUT
               if( xType == os_VARTYPE_NUMBER )
               {
                  printf(  "%s Size %3d: %s = %d\n",
                           "Number; ",
                           iSize,
                           (char*)Context,
                           (int) *buf
                        );
               }
               else
               {
                  *(buf+iUsed) = '\0';
                  printf(  "%s Size %3d: %s: %s\n",
                            (xType == os_VARTYPE_MACRO) ?
                              "Macro;  " :
                              (xType == os_VARTYPE_STRING) ?
                                 "String; " :
                                 "Unknown;",
                           iSize,
                           (char*)Context,
                           buf
                       );
               }
#endif
            }
            free( buf );
         }
#endif
      }
   }
   while( !Error && Context );

	printf( "ReadVarVal found %d numbers; %d macros; %d strings \n",
		vartype_number, vartype_macro, vartype_string
		);

   if( Error )
   {
      printf( "\nError: %08x - %s\n", Error -> errnum, Error -> errmess );
      iStat = -1;
   }

   return iStat;
}
