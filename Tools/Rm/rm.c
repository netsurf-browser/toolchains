/*rm - delete a lot of files*/

#include <kernel.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <swis.h>

//#define DEBUG

#define BUF_SIZ 256

static int rm( const char *object );

static int quiet;
static int recurse;
static int verbose;
static int not_error;

static void PrintHelp()
{
   printf(  "rm - remove a number of files\n"
            "Syntax: rm [-{hnqrv-}] file1 ... filen\n"
            "Options:\n"
            "\th - print this help text\n"
            "\tn - ignore \"not found\" errors\n"
            "\tq - be quiet\n"
            "\tr - recursively descend into subdirectories\n"
            "\tv - be verbose\n"
            "\t- - end of options; any subsequent \"-\" character is part of file name\n"
         );
}


static void GetOpt( const char* arg )
{
   if ( *arg == '-' )
   {
      while ( *(++arg) != '\0' )
      {
         if       ( *arg == 'q' ) quiet   = 1;
         else if  ( *arg == 'n' ) not_error = 1;
         else if  ( *arg == 'r' ) recurse = 1;
         else if  ( *arg == 'v' ) verbose = 1;
         else if  ( *arg == 'h' ) PrintHelp();
      }
   }
}


static void MakeError( const char *object, int obj_type )
{
   char* errmess = _swix(  OS_File,
                           _IN(0) | _IN(1) | _IN(2),
                           19, object, obj_type
                        ) -> errmess;
   fprintf( stderr, "rm: %s\n", errmess );
}


static void PrintError( const char* object, const _kernel_oserror *error )
{
   fprintf( stderr,
            "rm: %s: 0x%08x: %s\n",
            object, error -> errnum, error->errmess
          );
}


static int rm_obj( const char *object )
{
   int rc = 0;
   _kernel_oserror* error = 0;

   if ( verbose )
      printf( "rm: removing %s\n", object );

#if 1
   error = _swix( OS_File,
                  _IN(0) | _IN(1),
                  6, object
                );
#endif
   if ( error )
   {
      PrintError( object, error );
      rc = 1;
   }
   return rc;
}


static int rm_dir( const char *object )
{
   _kernel_oserror* error;
   int rc = 0;
   int i;
   int context = 0;

   /* enumerate the directory and remove each file */
   do
   {
      int items;
      char buf[ BUF_SIZ ];
      char name[ BUF_SIZ ];
      char* wildcard = "*";

      error = _swix( OS_GBPB,
                     _IN(0) | _IN(1) | _IN(2) | _IN(3) | _IN(4) | _IN(5) | _IN(6) |
                     _OUT(3) | _OUT(4),
                     9, object, buf, 100, context, BUF_SIZ, wildcard,
                     &items, &context
                   );
      if ( error )
      {
         PrintError( object, error );
         rc = 1;
      }
      else
      {
#ifdef DEBUG
         printf( "%s contains %d items:\n", object, items );
#endif
         char *p = buf;
         for ( i = 0; i < items; ++i )
         {
            sprintf( name, "%s.%s", object, p );
#ifdef DEBUG
            printf( "\t%s\n", name );
#endif
            rm( name );
            p += strlen(p) + 1;
         }
      }
   }
   while ( rc == 0 && context >= 0);

   return rc;
}


static int rm( const char *object )
{
   _kernel_oserror *error;
   int rc = 0;
   int obj_type;

   /* get catalogue information */
   error = _swix( OS_File,
                  _IN(0) | _IN(1) | _OUT(0),
                  17, object, &obj_type
                );

   if ( error != NULL)
   {
      PrintError( object, error );
      rc = 1;
   }
   else
   {
      if ( obj_type == 0 )
      {
         /* not found */
         if ( !quiet )
            MakeError( object, obj_type );

         if ( !not_error )
            rc = 1;
      }
      else if ( recurse && obj_type == 2 )
      {
         /* directory found - delete recursively */
         rc = rm_dir( object );
      }

      /* try to delete the object */
      if (rc == 0)
         rc = rm_obj( object );
   }
   return rc;
}


extern int main( int argc, char *argv [] )
{
   int i;
   int lastopt = INT_MAX;
   int rc = 0;

   if ( argc == 1 ) PrintHelp();

   /* check the options */
   for ( i = 1; (i < argc); ++i )
   {
      if (argv[i][0] == '-')
      {
         if ( argv[i][1] == '-' )
         {
            lastopt = i;
            break;
         }
         else
            GetOpt( argv[i] );
      }
   }

   /* delete the objects */
   for (i = 1; i != argc; ++i)
   {
      if ( i > lastopt || argv[i][0] != '-' )
      {
         rc |= rm( argv[i] );
      }
   }

   return rc;
}
