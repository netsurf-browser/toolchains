/******************************************
** StripDepends.c
**    Tony van der Hoff 2001
**
** Truncates a Make file after "#dynamic dependencies"
** to start a fresh dependency list
**
** Call: StripDepends <makefile>

** 010116   V1.00 TV    Original version
**
********************************************/
#define TRACE 0

#include <stdio.h>
#include <ctype.h>

#include "oslib/osargs.h"
#include "oslib/osgbpb.h"
#include "oslib/osfind.h"

#include "trace.h"

#define VERSION "1.00"
#define BUF_SIZ 128

/////////////////////////////////////////////////////////////////
/* case insensitive strcmp */
static int stricmp( const char *String1, const char *String2 )
{
  int iStatus;

  do
  {
    iStatus = toupper( *String1 ) - toupper( *String2 );
  }
  while( iStatus == 0 && *String1++ != '\0' && *String2++ != '\0' );

  return iStatus;
}


/////////////////////////////////////////////////////////////////
// read a line from file
static void ReadLine( os_fw handle, byte* buffer, int bufsize )
{
   byte *buf;
   for( buf = buffer; buf - buffer < bufsize; buf++ )
   {
      int chars;
      chars = osgbpb_readw( handle, buf, 1 );
      if( chars != 0 || *buf == '\n' )
      {
         *buf = '\0';
         break;
      }
   }
}


/////////////////////////////////////////////////////////////////
int main( int argc, char* argv[] )
{
   fprintf( stdout, "StripDepends V%s (%s) Tony van der Hoff\n",  VERSION, __DATE__ );
   if( argc != 2 )
      fprintf( stderr, "Syntax: StripDepends <makefile>\n" );
   else
   {
      os_fw handle;
      handle = osfind_openupw( osfind_ERROR_IF_ABSENT | osfind_NO_PATH,
                               argv[1],
                               NULL
                             );
      tracef( "File handle: %d\n" _ (int)handle );
      while( !osargs_read_eof_statusw( handle ) )
      {
         byte buf[BUF_SIZ];
         ReadLine( handle, buf, BUF_SIZ );
         tracef( "ReadLine: %s\n" _ (char*)buf );

         if( stricmp( (char*)buf, "# Dynamic Dependencies:" ) == 0 )
         {
            // truncate the file at this position
            int pos;
            pos = osargs_read_ptrw( handle );
            osargs_set_extw( handle, pos );
         }
      }
      osfind_closew( handle );
   }
   return 0;
}
