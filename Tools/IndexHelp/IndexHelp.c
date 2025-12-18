/* Index: OSLib/!OsLib/Tools/IndexHelp/IndexHelp.c	20031228
**    Tony van der Hoff
**    Create an index file for a StrongHelp Chapter,
**    where DefMod has not already done so, because the chapter is drawn
**    together from a number of SWI modules.
*/

/*
      OSLib is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

      OSLib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
   along with this programme; if not, write to the Free Software
   Foundation, Inc, 675 Mass Ave, Cambridge, MA 02139, U S A.
*/

/* revision History:
** 20010130 TV    V1.01 fixes calls to *printf with null arguments (T.Baldwin)
*/

#define VERSION "V1.01"

/* Calling Syntax:
**    IndexHelp <SourceDir> <DestFile> [options [options]]
**    options: -v - verbose
*/

#ifndef TRACE
#define TRACE 0
#endif

#ifdef EXECUTE_ON_UNIX
#define DIR_SEP "/"
#else
#define DIR_SEP "."
#endif

//#define USE_STRONGHELP 0   /* set true to generate dest using StrongHelp filer calls */

/* From CLib */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
//#include <time.h>

/* From OSLib */
#include "oslib/macros.h"
#include "oslib/os.h"
#include "oslib/osfile.h"
#include "oslib/osgbpb.h"
#include "oslib/osfind.h"
#include "oslib/osargs.h"

/* From OSLibSupport */
#include "x.h"
#include "trace.h"


#define INDEX_SIZ          0x1000
#define DIR_BUF_SIZ        0x100
#define TEMP_MAX           0x100

#define ERRNO(x) ( 1<<30 | x )

#define FREE(x)   x_FREE( ( x ) ); \
                  ( x ) = 0
/* globals */
static int index_entries;
static char *man_index[ INDEX_SIZ ];
static char *src_path;
//static char *dst_path;
static osbool verbose;

/***************************************************/
/* return the size of a string in a word block
*/
static size_t str_block_size( const char* s )
{
   return (s && *s) ? (strlen(s) + 4) & ~ 3 : 0;
}



/***************************************************/
/* create a copy of string s on the heap
**  return a pointer to the heap block
*/
static char *qstrdup( const char *s )
{
   int size;
   char *dup = NULL;

   if( s && *s )
   {
      size = ( str_block_size(s) );
      dup = x_ALLOC( size );
      memset( dup, '\0', size );
      strcpy( dup, s );
   }
   return dup;
}


/****************************************************************/
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


/****************************************************************/
/* compare two strings */
static int Cmp( void const *v0, void const *v1 )
{
   int cmp = 0;

   #define s0 (*(const char**)v0)
   #define s1 (*(const char**)v1)

   cmp = stricmp( s0, s1 );

//   tracef( "cmp (\"%s\" \"%s\") = %i\n" _ s0 _ s1 _ SGN(cmp) );
   return cmp;
}


/***************************************************/
/* clear the index
*/
static void clear_index()
{
   for( ;index_entries > 0; index_entries-- )
     free( man_index[ index_entries - 1 ] );
}


/***************************************************/
/* add a manual to the index
*/
static void add_index( const char* dir, const char* file )
{
   FILE *source;
   char temp[ TEMP_MAX];
   x_exception x0;
   x_LOCAL_ERROR( Error_index_full, ERRNO( 0x301 ), "Master Index full" );

   if ( index_entries >= INDEX_SIZ )
      x_THROW( Error_index_full );

   sprintf( temp,
            "%s%s%s%s%s",
            src_path ? src_path : "",
            src_path? DIR_SEP : "",
            dir ? dir : "",
            dir ? DIR_SEP : "",
            file
          );

   tracef( "add_index: reading %s\n" _ temp );
   source = fopen( temp, "r" );
   if ( !source ) x_THROW_LAST_ERROR();

   x_TRY( &x0 )
   {
      /* get the manual name */
      if( fgets( temp, TEMP_MAX, source ) == NULL )
         x_THROW_LAST_ERROR();

      /* add to index */
      *(temp+strlen(temp)-1) = '\0';
      tracef( "add_index: %s\n" _ temp );
      man_index[ index_entries++ ] = qstrdup( temp );

      /* tidy up */
      fclose( source );
   }
   x_CATCH( &x0 )
   {
      fclose( source );
      tracef( "exception in add_index\n" );
      x_RETHROW( &x0 );
   }
}


/***************************************************/
/* write a supplementary index
*/
static int write_root( const char* name )
{
   char temp[ TEMP_MAX ];
   FILE *index_file;
   x_exception x0;
   int size = 0;

   /* sort the index */
   qsort( man_index, index_entries, sizeof( man_index[0] ), Cmp );

   /* create an index file in the source dir */
   sprintf( temp, "%s"DIR_SEP"%s", src_path, "!Root" );
   if ( ( index_file = fopen( temp, "wb" ) ) == NULL )
      x_THROW_LAST_ERROR();

   x_TRY( &x0 )
   {
      int i;

      // write the header
      if ( ( size +=
               fprintf( index_file,
                        "%s\n"
                        "#Parent OSLib:Root\n"
                        "#Wrap Off\n"
                        "#Background Wimp 1\n"
                        "#fH2:OSLib: %s\n"
                        "#Line\n"
                        "#Table Columns 1\n",
                        name,
                        name
                      )
            ) < 0
         ) x_THROW_LAST_ERROR();


      // write the index
      for( i = 0; i < index_entries; i++ )
      {
         if ( ( size += fprintf( index_file,
                                 "<%s>\n",
                                 man_index[i]
                               )
              ) < 0
            ) x_THROW_LAST_ERROR();
      }

      // write the tailer
      if ( ( size += fprintf( index_file,
                              "#EndTable\n"
                            )
           ) < 0
         ) x_THROW_LAST_ERROR();

      /* clear the index and close the file */
      fclose( index_file );
   }
   x_CATCH( &x0 )
   {
      fclose( index_file );
      x_RETHROW( &x0 );
   }
   return size;
}

/***************************************************/
/* create a supplementary index for a manual chapter which
** is generated by numerous SWI modules
*/
static void create_root( const char* name )
{
#define INFO ((osgbpb_info *)(infolist))
   int context;
   bits infolist[DIR_BUF_SIZ];
   x_exception x0;

   if( verbose )
      fprintf( stdout, "Creating index %s from %s\n", name, src_path );

   /* scan the directory, building up an index */
   x_TRY( &x0 )
   {
      context = 0;
      do
      {
         int read_count;
         context = osgbpb_dir_entries_info(  src_path,
                                             (osgbpb_info_list*)infolist,
                                             1,
                                             context,
                                             DIR_BUF_SIZ,
                                             NULL,
                                             &read_count
                                          );
	 tracef( "osgbpb found %d entries: %s\n" _ read_count _ read_count ? INFO -> name : "" );

         if( read_count != 0 )
         {
            if( INFO -> obj_type == fileswitch_IS_FILE )
            {
	       	tracef( "%s is File\n" _ INFO -> name );

	       /* index the file if it is not an earlier index */
               if( stricmp( INFO->name, "!root" ) != 0 )
               {
                  if( verbose )
                     fprintf( stdout, "Page %s\n", INFO->name );

                  add_index( NULL, INFO->name );
               }
            }
	    else
	    {
	    }
         }
         else
         {
	 }
      }
      while( context != osgbpb_NO_MORE );

      if( index_entries > 0 )
      {
         write_root( name );
         clear_index();
      }
   }
   x_CATCH( &x0 )
   {
      clear_index();
      x_RETHROW( &x0 );
   }
#undef INFO
}

/***************************************************/
/* Main Entry */
int main( int   argc,
          char *argv[]
        )
{
   x_exception x0;
//   char temp[ TEMP_MAX ];
   int rc = 0;                            /* return code */
   int i;
//   dir_list_entry* dir_list_root = NULL;  /* root of current directory list */
//   os_fw handle;                          /* image file handle */
   char *name;

   x_LOCAL_ERROR( Error_syntax, \
                  ERRNO( 0 ), \
                  "Syntax:\n" \
                  "  IndexHelp <SourceDir> <page name> [options [options]]\n" \
                  "  options: -p <pagename> - name of page\n" \
                  "           -v            - verbose\n"
                );

   x_TRY( &x0 )
   {
      if( argc < 2 )
         x_THROW( Error_syntax );

#if TRACE
      for( i = 0; i < argc; i++ )
      {
         printf( "%s ", argv[i] );
      }
      printf( "\n" );
#endif

      /* initialise globals */
      index_entries = 0;
      src_path = argv[1];
      verbose = FALSE;
      name = strrchr( src_path, DIR_SEP[0] ) + 1;
      for( i = 2; i < argc; i++ )
      {
         if( strcmp( argv[i], "-v" ) == 0 )
            verbose = TRUE;
         else if( strcmp( argv[i], "-p" ) == 0 )
         {
            name = argv[++i];
         }
         else
         {
            /* bad option found */
	    tracef( "Bad Syntax\n" );
            x_THROW( Error_syntax );
         }
      }

      if (verbose)
         fprintf( stdout, "IndexHelp " VERSION " " __DATE__ " © Tony van der Hoff\n" );

      /* create supplementary index */
      create_root( name );

      if( verbose )
         fprintf( stdout, "IndexHelp Complete\n" );
   }
   x_CATCH( &x0 )
   {
      fprintf( stderr,
               "\nIndexHelp: %08x - %s\n\n",
               x0.error -> errnum,
               x0.error -> errmess
             );
      rc = 1;
   }
   return rc;
}
