/* Index: OSLib/!OsLib/Tools/BindHelp/BindHelp.c	20031228
**    Tony van der Hoff
**    Modified by Stephen Fryatt
**    Bind a StrongHelp manual from the StrongHelp2 source files created by DefMod
**    we write the image file directly (if USE_STRONGHELP == 0)
**       - to avoid needing StrongHelp loaded,
**       - to increase speed, because StongHelp shuffles data around
**       - and to avoid having to CleanCopy the result.
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

/* Revision History:
** 20011212 TV    V1.00 Original release
** 20010130 TV    V1.01 Fixes calls to *printf with null pointers (Timothy Baldwin)
** 20040922 TV	  V1.10 fixes bug where file types were incorrectly set under UNIX
**
** 20100515 SF    V1.20 Add option to build using supplied !Root, and appended filetypes.
*/

#define VERSION "V1.20"

/* Calling Syntax:
**    BindHelp <SourceDir> <DestFile> [-f] [-r] [-v]
*/

/*
Operation is as follows:
------------------------
  First, a new, empty, image is created. This is so that if this  step fails,
we can abort without wasting time on subsequent steps. As we will be building
a new manual !Root, any such file is deleted to avoid confusion.

  Next,  the source  directory  is  scanned  to gain  information  about  its
contents, so that  a new directory structure  can be eventually built  in the
image. The information for  each object is held in a branched  linked list of
dir_list_entries. Each directory entry appears  in the next item of the list,
with any sub-directories branching off to form their own list.

  During the source scan, a root  index is built, by reading each object name
from  the first  line of  each !Root  file encountered.  Each object  name is
placed in an array.

  The index array is then sorted alphabetically, and written to a new overall
manual master !Root file in the source directory, for want of a better place,
and to keep all the source together. A new dir_list_entry is created for this
file at the head of the linked list

  The image file is then generated. First, having collected all  the relevant
file data, the linked list is scanned to calculate directory and data offsets
for  each item, which are written  to the list (that was  the hard bit to get
right).

  With this information,  the image file  is written out with  each directory
and data file at its appropriate offset. Data files are read from the source,
and written back to the image.

  Finally, if  all  went well,  the  image file  is closed  and  set to  type
StrongHelp.

  A compile-time option allows the StrongHelp filer to be used to  create the
same manual; after clean-copying it, this should be identical to the (faster)
native method.
*/

#define USE_STRONGHELP 0   /* set true to generate dest using StrongHelp filer calls */

/* From CLib */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

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
#define FILE_TYPE_STRHELP  0x3d6
#define STRONGHELP_VERSION 0x10F
#define DIRS               0x24524944u
#define DATA               0x41544144u
#define HELP               0x504c4548u

#define ERRNO(x) ( (1<<30) | x )

#define FREE(x)   do { x_FREE( x ); \
                  ( x ) = 0; } while (0)

#define RO_DIR_SEP	"."
#define UNIX_DIR_SEP	"/"

#ifdef EXECUTE_ON_UNIX
#  define DIR_SEP UNIX_DIR_SEP
#else
#  define DIR_SEP RO_DIR_SEP
#endif

/* data structures */
typedef struct
{
   unsigned int id;
   unsigned int size;
   unsigned int version;
   unsigned int freeptr;
}
file_header;

typedef struct
{
   unsigned int id;
   unsigned int size;
   unsigned int used;
}
dir_header;

typedef struct
{
   bits id;
   bits size;
}
data_header;

typedef struct
{
   bits offset;
   bits load_addr;
   bits exec_addr;
   bits length;
   bits flags;
   bits reserved;
}
dir_entry_header; /* always followed by name */


/* element in linked list describing directory structure */
typedef struct dir_list_entry_t
{
   struct dir_list_entry_t *next;   /* next entry in linked list; NULL for last */
   struct dir_list_entry_t *list;   /* root of linked list describing sub-dir; NULL for file */
   const char *name;                /* element name */
   int size;                        /* object size */
   fileswitch_attr flags;           /* object flags */
   bits stamp_h;                    /* time stamp */
   bits stamp_l;
   int offset;                      /* offset to object */
   const char* filename;            /* path to source name */
}
dir_list_entry;


/* forward references */
//static void copy_page( const char *dir, const char* page, int size );

/* globals */
static int index_entries;
static char *man_index[ INDEX_SIZ ];
static char *src_path;
static char *dst_path;
static osbool verbose;
static osbool build_root;
static osbool append_filetypes;

/***************************************************/
/* return the size of a string in a word block
*/
static size_t str_block_size( const char* s )
{
   return (s && *s) ? (strlen(s) + 4) & ~ 3 : 0;
}


/***************************************************/
/* return the size of a data entry
*/
static int data_size( const dir_list_entry *entry )
{
   return sizeof( data_header ) + entry -> size;
}


/***************************************************/
/* return the size of a data block
*/
static int data_block_size( const dir_list_entry *entry )
{
   return ( data_size( entry ) + 3 ) & ~3;
}


/***************************************************/
/* return the size of a dir block entry
*/
static int dir_entry_size( const dir_list_entry *entry )
{
   int s;
   s = sizeof( dir_entry_header) + str_block_size( entry -> name );
   tracef( "dir_entry_size: %04x; %s\n" _ s _ entry->name );
   return s;
}


/***************************************************/
/* scan the dir list, and return dir block size
*/
static int dir_size( const dir_list_entry *root )
{
   int size = 0;
   const dir_list_entry *entry;

   /* compute dir block size by adding sizes of individual dir entries */
   for( entry = root; entry; entry = entry -> next )
   {
      size += dir_entry_size( entry );
   }
   tracef( "dir_size: %04x\n" _ size );
   return size;
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

   tracef( "cmp (\"%s\" \"%s\") = %i\n" _ s0 _ s1 _ SGN(cmp) );
   return cmp;
}


/***************************************************/
/* write a block of data to the image file
**    write to ptr or end of file if ptr < 0
*/
#if !USE_STRONGHELP
static int write_block( os_fw handle, byte* buf, size_t size, int ptr )
{
   int unwritten;
   x_LOCAL_ERROR( err_write_error, 1, "write_block failed" );

   size = (size + 3 ) & ~3;

   tracef( "write_block size: %d; to %d\n" _ size _ ptr );

   if( ptr < 0 )
      ptr = osargs_read_extw( handle );

   unwritten = osgbpb_write_atw( handle, buf, size, ptr );
   if( unwritten != 0 )
      x_THROW( err_write_error );

   return size;
}
#endif


/***************************************************/
/* create a new object entry to be inserted in the list of entries
*/
static dir_list_entry *add_obj(  const char* name,
                                 int size,
                                 bits stamp_h,
                                 bits stamp_l ,
                                 fileswitch_attr flags,
                                 const char* filename
                              )
{
   dir_list_entry *entry;

   tracef( "add_obj %s\n" _ name );

   entry = x_ALLOC( sizeof( dir_list_entry ) );
   entry -> next = NULL;
   entry -> list = NULL;
   entry -> size = size;
   entry -> name = qstrdup( name );
   entry -> flags = flags;
   entry -> stamp_h = stamp_h;
   entry -> stamp_l = stamp_l;
   entry -> offset = 0;
   entry -> filename = qstrdup( filename );
   return entry;
}


/***************************************************/
/* create a new directory entry to be inserted in the list of entries
*/
static dir_list_entry *add_dir( const char *name,
                                bits stamp_h,
                                bits stamp_l,
                                fileswitch_attr flags,
                                const char* dirname
                              )
{
   int size = strlen( name ) + sizeof( dir_header );
#if !USE_STRONGHELP
   dirname = NULL;
#endif
   return add_obj( name, size, stamp_h, stamp_l, flags | 0x100, dirname );
}


/***************************************************/
/* create a new leaf entry to be inserted in the list of entries
*/
static dir_list_entry *add_file( const char *name,
                                 int size,
                                 bits stamp_h,
                                 bits stamp_l,
                                 fileswitch_attr flags,
                                 const char* filename
                               )
{
   return add_obj( name, size, stamp_h, stamp_l, flags, filename );
}


#if TRACE
/***************************************************/
/* print the directory list to stderr
*/
static void print_dir_list( const dir_list_entry* root, int level )
{
   const dir_list_entry *entry;
   char temp[64];
   *temp = '\0';
   memset( temp, ' ', level*2 );

   for( entry = root; entry != NULL; entry = entry -> next )
   {
      fprintf( stderr,
               "%sentry: size: %08x; offset: %08x; module: %s; file: %s\n",
               temp,
               entry -> size,
               entry -> offset,
               entry -> name,
               entry -> filename
            );

      if( entry -> list )
         print_dir_list( entry-> list, level+1 );
   }
}
#endif


/***************************************************/
/* write dir entry to the image
**    returns number of bytes written
*/
static int write_dir_entry( os_fw handle,
                            const dir_list_entry* entry,
                            int offset
                          )
{
   int written = 0;

#if !USE_STRONGHELP
   dir_entry_header header;

   header.offset = entry -> offset;
   header.load_addr = entry -> stamp_h;
   header.exec_addr = entry -> stamp_l;
   header.flags = entry -> flags;
   header.reserved = 0;

   if( entry -> list )
      header.length = dir_size( entry -> list )
                      + sizeof( dir_header )
                      - sizeof( unsigned int )  ;
   else
      header.length = entry -> size + sizeof( data_header );

   tracef( "write_dir_entry: offset: %08x; length: %08x; %s\n" \
            _ offset \
            _ header.length \
            _ entry -> name \
         );

   written += write_block( handle,
                           (byte*)&header,
                           sizeof( header ),
                           offset + written
                         );
   written += write_block( handle,
                           (byte*)(entry->name),
                           str_block_size( entry->name ),
                           offset + written
                         );

#else
   char temp[ TEMP_MAX ];

   offset = offset;
   handle = handle;

   if( entry -> list )
   {
      sprintf( temp, "CDir %s.%s", dst_path, entry -> filename );
      system( temp );
   }
#endif

   return written;
}


/***************************************************/
/* write data block to the image
**    returns numbr of bytes written
*/
static int write_data( os_fw handle, const dir_list_entry* entry )
{
   x_exception x0;
   int written = 0;
   static byte *buf;
   char temp[ TEMP_MAX ];
   os_error err;

#if !USE_STRONGHELP
   data_header header;
   int offset = entry -> offset;

   tracef( "write_data: offset %08x; %s\n" _ offset _ entry->filename );

   /* write the data header */
   header.id = DATA;
   header.size = data_size( entry );
   written += write_block( handle,
                           (byte*)&header,
                           sizeof( header ),
                           offset + written
                         );
#endif

   if( verbose )
      fprintf( stdout, "     page %s\n", entry -> filename );

   sprintf( temp, "%s"DIR_SEP"%s", src_path, entry -> filename );

   /* copy the data */
   buf = x_ALLOC( data_block_size( entry ) );
   x_TRY( &x0 )
   {
      int size = 0;
      tracef( "Write_data: reading %08x bytes from %s\n" \
               _ entry -> size \
               _ temp \
            );

      memset( buf, data_block_size( entry ), 0 );

      if ( osfile_load_stamped_no_path( temp, buf, NULL, NULL, &size, NULL )
      		!= osfile_IS_FILE
	 )
      {
	err.errnum = 0;
	sprintf( err.errmess,"File not found: \"%s\"", temp );
      	x_THROW( &err );
      }

      if( entry->size != size )
      {
         x_LOCAL_ERROR( err_filesize, 1, "Filesize mismatch" );
         tracef( "write_data: file size passed: %d; read %d\n" _ entry->size _ size );
         x_THROW( err_filesize );
      }

#if !USE_STRONGHELP
      written += write_block( handle, buf, size, offset + written );
      tracef( "write_data: file size %08x; written: %08x\n" _ written );
#else
      handle = handle;
      sprintf( temp, "%s"DIRSEP"%s", dst_path, entry -> filename );
      osfile_save( temp, entry -> stamp_h, entry -> stamp_l, buf, buf + size );
#endif

      FREE( buf );
   }
   x_CATCH( &x0 )
   {
	tracef( 	"write_data: Exception caught: errno %d -- \"%s\"\n" \
			_ x0.error -> errnum \
			_ x0.error -> errmess
		);
      	FREE( buf );
      	x_RETHROW( &x0 );
   }
   tracef( "write_data: returning\n" );
   return written;
}


/***************************************************/
/* On pass 0,
**    scan the dir list, and calculate offsets in file.
** On pass 1,
**    write directories and data to the image
** returns size of data
*/
static int write_dir_list( os_fw handle,
                           dir_list_entry* root,
                           int offset,
                           int pass
                         )
{
   dir_list_entry* entry;
   int size = 0;
   dir_header header;
   int dirsize =  dir_size( root ) + sizeof( header );

   /* write the dir header and entries */
   switch( pass )
   {
   case 0:
      size += dirsize;
      break;

   case 1:
      header.id = DIRS;
      header.used = header.size = dirsize;

#if !USE_STRONGHELP
      tracef( "write_dir_list: writing dir at %08x; size %08x\n" _ offset+size _ dirsize );
      size += write_block( handle,
                           (byte*)&header,
                           sizeof( header ),
                           offset + size
                         );
#endif

      /* write this level of dir entries */
      for ( entry = root;
            entry;
            entry = entry -> next
          )
      {
            size += write_dir_entry( handle, entry, offset + size );
      }
      break;
   }  /* end switch */

   /* write the objects pointed to by this level */
   for ( entry = root;
         entry;
         entry = entry -> next
       )
   {
      switch( pass )
      {
      case 0:
         entry -> offset = offset + size;
         break;
      }   /* end switch */

      if( entry -> list )
      {
         /* recurse into any lower level dir */
         tracef( "write_dir_list: recurses\n" );
         if( verbose )
            fprintf( stdout, "Directory %s\n", entry -> name );

         size += write_dir_list( handle, entry -> list, entry -> offset, pass );
      }
      else
      {
         switch( pass )
         {
         case 0:
            size += data_block_size( entry );
            break;

         case 1:
            write_data( handle, entry );
            break;
         }  /* end switch */
      }
   }

#if TRACE
   /*   print the list after pass 0 */
   switch( pass )
   {
   case 0:
      print_dir_list( root, 0 );
   }
#endif

   tracef( "write_dir_list: returns %08x\n" _ size );
   return size;
}

/***************************************************/
/* delete the directory list and associated structs
*/
static void delete_dir_list( dir_list_entry** root )
{
   dir_list_entry *entry;

   while( root && *root )
   {
      //tracef( "delete_dir_entry 0x%x\n" _ *root );

      entry = *root;
      *root = entry -> next;

      if ( entry -> list )
         delete_dir_list( &(entry -> list) );

      x_FREE( (char*)(entry -> name) );
      x_FREE( (char*)(entry -> filename) );
      x_FREE( entry );
   }
}


/***************************************************/
/* Write the image file
*/
static void write_file( os_fw handle, dir_list_entry* root )
{
   int offset = 0;
   int pass;
   int size;

#if !USE_STRONGHELP
   dir_entry_header dir;
#if 0
   char* name = "$";
#else
   char name[4] = {'$',0,0,0};
#endif
   offset =  osargs_read_extw( handle );

   /* pseudo dir above root */
   dir.offset = offset + sizeof( dir ) + str_block_size( name );
   dir.load_addr = 0xfffffD00u;
   dir.exec_addr = 0;
   dir.length = dir_size( root )
                + sizeof( dir_header )
                - 2 * sizeof( unsigned int );
   dir.flags = 0x100;
   dir.reserved = 0;

   offset += write_block( handle, (byte*)&dir, sizeof( dir ), offset );
   offset += write_block( handle, (byte*)name, str_block_size( name ), offset );
#endif

   /* first scan the directory list to calculate offsets
   ** then write write the dir structure and files to those offsets
   */
   for( pass = 0; pass <= 1; pass++ )
      size = write_dir_list( handle, root, offset, pass );

   tracef( "write_file: wrote %08x bytes\n" _ size );
}


/***************************************************/
/* create a new StrongHelp manual
**    throws exception on error
*/
static os_fw create_file( const char* name )
{
   os_fw handle = 0;

#if USE_STRONGHELP
   char temp[ TEMP_MAX];

   sprintf( temp, "StrongCreate %s", name );
   system( temp );
#else
   file_header header;
   tracef( "create_file - %s \n" _ name );

   handle = osfind_openoutw( osfind_NO_PATH,
                             name,
                             NULL
                           );

   header.id = HELP;
   header.size = sizeof( header ) + sizeof( dir_entry_header ) + 4;
   header.version = STRONGHELP_VERSION;
   header.freeptr = 0xffffffffu;

   write_block( handle, (byte*)&header, sizeof( header ), 0 );
#endif

   return handle;
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
/* sort the index and copy to file
*/
static int file_index( dir_list_entry** root )
{
   int i;
   int size = 0;
   char temp_name[ TEMP_MAX ];
   char temp[ TEMP_MAX ];
   FILE *index_file;
   x_exception x0;
   time_t xtime;
   char year[8];

   if( verbose )
      fprintf( stdout, "Writing master index\n" );

   /* sort the index */
   qsort( man_index, index_entries, sizeof( man_index[0] ), Cmp );

   /* create an index file in the source dir */
   sprintf( temp_name, "%s"DIR_SEP"%s", src_path, "!Root" );
   if ( ( index_file = fopen( temp_name, "wb" ) ) == NULL )
      x_THROW_LAST_ERROR();

   tracef( "file_index writing %s\n" _ temp_name );
   x_TRY( &x0 )
   {
      size = 0;

      // write the header
      if ( ( size +=
               fprintf( index_file,
                        "OSLib\n"
                        "#Parent StrongHelp:!Menu\n"
                        "#Background Wimp 1\n"
                        "#fH1:OSLib\n"
                        "#Line\n"
                        "#Table Columns 6\n"
                      )
            ) < 0
         ) x_THROW_LAST_ERROR();


      // write the index
      for( i = 0; i < index_entries; i++ )
      {
         if ( ( size += fprintf( index_file,
                                 "<%s=>%s_>\n", man_index[i], man_index[i]
                               )
              ) < 0
            ) x_THROW_LAST_ERROR();
      }

      // write the tailer
      time( &xtime );
      strftime( temp, TEMP_MAX, "%d %b %Y", localtime( &xtime ) );
      strftime( year, 8, "%Y", localtime( &xtime ) );
      if ( ( size += fprintf( index_file,
                              "#EndTable\n"
                              "#Line\n"
                              "#Wrap Off\n"
                              "#Align Centre\n"
                              "OSLib by {*}Jonathan Coxhead{*}\n"
                              "Copyright (c) Jonathan Coxhead and OSLib Maintainers "
			      "1995 - %s\n"
                              "StrongHelp Manual created %s by BindHelp\n"
                              "<http://ro-oslib.sourceforge.net=>#url>\n"
                              "<mailto:oslib-users@compton.nu=>#url>\n",
			      year,
                              temp
                            )
           ) < 0
         ) x_THROW_LAST_ERROR();

      /* clear the index and close the file */
      clear_index();
      fclose( index_file );

#if 0
      /* insert this at the head of the dir list */
      {
         dir_list_entry *entry;
         entry = add_file( "!Root", size, 0xFFFFFD00u, 0, 3, "!Root" );
         entry -> next = *root;
         *root = entry;
      }
#else
      /* insert this at the tail of the dir list
      **    (to match the StrongHelp output)
      */
      {
         dir_list_entry **entry;
         for( entry = root; *entry; entry = &((*entry)->next))
         {}
         *entry = add_file( "!Root", size, 0xFFFFFD00u, 0, 3, "!Root" );
      }
#endif
   }
   x_CATCH( &x0 )
   {
      fclose( index_file );
      x_RETHROW( &x0 );
   }
   return size;
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
            src_path ? DIR_SEP : "",
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
/* recursively scan the source directory
** and create the master index
*/
static void scan_dir( const char* dir,          /* source directory */
                      dir_list_entry** root     /* root of current directory list */
                    )
{
#define INFO ((osgbpb_info *)(infolist))
   char source_dir[ TEMP_MAX ];
   int context;
   bits infolist[DIR_BUF_SIZ];
   osbool std_filetype;

   sprintf( source_dir,
            "%s%s%s",
            src_path,
            (dir && dir[0] != '\0') ? DIR_SEP : "",
            dir ? dir : ""
         );

   context = 0;
   do
   {
      int read_count;

      tracef( "scan_dir: %s\n" _ source_dir );
      context = osgbpb_dir_entries_info(  source_dir,
                                          (osgbpb_info_list*)infolist,
                                          1,
                                          context,
                                          DIR_BUF_SIZ,
                                          NULL,
                                          &read_count
                                       );
      tracef( "scan_dir: context %d; read_count %d\n" _ context _ read_count );

      if( read_count != 0 )
      {
         char temp[ TEMP_MAX ];
         dir_list_entry *dir_entry;

         sprintf( temp,
                  "%s%s%s",
                  dir ? dir : "",
                  (dir && dir[0] != '\0') ? DIR_SEP : "",
                  INFO -> name
                );

         if ( INFO -> obj_type == fileswitch_IS_DIR )
         {
            /* sub-directory found - add to linked list, then recurse into it */

            tracef( "scan_dir - recursing into %s\n" _ INFO->name );

            *root = dir_entry =  add_dir( INFO->name,
                                          INFO->load_addr,
                                          INFO->exec_addr,
                                          INFO->attr,
                                          temp
                                        );
            root = &(dir_entry -> next);

            /* recurse into source dir */
            scan_dir( temp, &(dir_entry -> list) );
         }
         else if ( INFO -> obj_type == fileswitch_IS_FILE )
         {
            /* page found */
            tracef( "scan_dir - found file: %s"DIR_SEP"%s \n" _ dir _ INFO->name );

#if defined EXECUTE_ON_UNIX
	/* V1.10 */
	/* Under RISC OS, the file type information is available in the info block;
	// not so under UNIX. It is important, as searchable (using F1) files are typed
	// "text", and non-searchable files are typed "data".
	// Fortunately, in this case, an alternative distinguishing feature is that
	// the (non-searchable) index files are named "!Root", or have file names starting with "_",
	// so under Unix we use this clue to set the file type.
	//
	// V1.20
	// To facilitiate the addition of non-text files, we now have an option to
	// assume that all filetypes will be shown on the end of the filename by
	// the conventional ,xxx notation.  If not, of if the type conversion of the
	// trailing text fails, we fall back to the usual logic.  If a filetype is found,
	// then the ,xxx extension isn't copied into the archive.
	//
	*/
	    std_filetype = TRUE;

	    if ( append_filetypes && strlen( INFO->name ) > 4 )
	    {
	       char *postfix = INFO->name + strlen( INFO->name ) - 4;
	       if ( *postfix == ',' )
	       {
		  char *end;
		  unsigned int type = (unsigned int) strtol( postfix+1, &end, 16 );

		  if ( type >= 0x000u && type <= 0xfffu )
		  {
		     *postfix = '\0';

		     INFO->load_addr = 0xFFF00000u | ( type << 8 );
		     INFO->exec_addr = 0;

		     std_filetype = FALSE;
		   }
		}
	    }

	    if ( std_filetype )
	    {
              if( (INFO -> name[0] == '_') || (stricmp( INFO->name, "!Root" ) == 0) )
              {
		  INFO->load_addr = 0xFFFFFD00u;
		  INFO->exec_addr = 0;
              }
	      else
              {
		  INFO->load_addr = 0xFFFFFF00u;
		  INFO->exec_addr = 0;
              }
            }
#endif

            *root = dir_entry = add_file( INFO->name,
                                          INFO->size,
                                          INFO->load_addr,
                                          INFO->exec_addr,
                                          INFO->attr,
                                          temp
                                        );
            root = &(dir_entry -> next);

            /* if this was a !Root page, and not the manual !Root
            ** then add manual name to index */
            if( dir && stricmp( INFO->name, "!Root" ) == 0 )
            {
               add_index( dir, INFO->name );
            }
         }
      }
   }
   while( context != osgbpb_NO_MORE );

   tracef( "scan_dir - returns\n" );
#undef INFO
}


/***************************************************/
/* Main Entry */
int main( int   argc,
          char *argv[]
        )
{
   x_exception x0, x1;
   char temp[ TEMP_MAX ];
   int rc = 0;                            /* return code */
   int i;
   dir_list_entry* dir_list_root = NULL;  /* root of current directory list */
   os_fw handle;                          /* image file handle */

   x_LOCAL_ERROR( Error_syntax, \
                  ERRNO( 0 ), \
                  "Syntax:\n" \
                  "  BindHelp <SourceDir> <DestFile> [options]\n" \
                  "  options: -f - use appended filetypes\n" \
                  "           -r - use existing !Root\n" \
                  "           -v - verbose\n"
                );

   x_TRY( &x0 )
   {
      if( argc < 3 )
         x_THROW( Error_syntax );

      /* initialise globals */
      index_entries = 0;
      src_path = argv[1];
      dst_path = argv[2];
      verbose = FALSE;
      build_root = TRUE;
      append_filetypes = FALSE;
      for( i = 3; i < argc; i++ )
      {
         if( strcmp( argv[i], "-v" ) == 0 )
            verbose = TRUE;
         else if (strcmp( argv[i], "-r") == 0)
            build_root = FALSE;
         else if (strcmp( argv[i], "-f") == 0)
            append_filetypes = TRUE;
         else
         {
            /* bad option found */
            x_THROW( Error_syntax );
         }
      }

      if( verbose )
      {
      	fprintf( stdout, "BindHelp " VERSION " "__DATE__ " � Tony van der Hoff\n" );
      	if ( !build_root )
      		fprintf( stdout, "Using supplied !Root file\n" );
      	if ( append_filetypes )
      		fprintf( stdout, "Checking for appended filetypes in filenames\n" );
#if USE_STRONGHELP
      	fprintf( stdout, "Using StrongHelp calls\n" );
#endif
      }

      /* create destination manual and copy the source into it */
      tracef( "main: Creating file\n" );
      handle = create_file( dst_path );

      tracef( "main: creating manual\n" );
      x_TRY( &x1 )
      {
         /* delete any previous !Root at root level
         ** so it doesn't appear in the dir list
         **
         ** This is only done if we don't want to accept the user-supplied
         ** !Root file.
         */
         if (build_root)
         {
            sprintf( temp, "%s"DIR_SEP"!Root", src_path );
	    tracef( "deleting \"%s\"\n" _ temp );
            xosfile_delete( temp, NULL, NULL, NULL, NULL, NULL );
	 }

         /* scan the source dir to create the master index and directory list */
         if( verbose )
            fprintf( stdout, "Creating master index\n" );

         scan_dir( NULL, &dir_list_root);

         if ( build_root )
            file_index( &dir_list_root );

         write_file( handle, dir_list_root );

         /* tidy up */
         if( handle )
            osfind_closew( handle );
         delete_dir_list( &dir_list_root );

         /* set the file type */
         osfile_set_type( dst_path, FILE_TYPE_STRHELP );

         if( verbose )
            fprintf( stdout, "BindHelp Complete\n" );
      }
      x_CATCH( &x1 )
      {
	tracef( "Main: exception caught: 0x%x - %s\n" _ \
		x1.error ? x1.error -> errnum : 0 _ \
		x1.error ? x1.error -> errmess : "NULL" );

	 osfind_closew( handle );
         osfile_delete( argv[2], NULL, NULL, NULL, NULL );
         delete_dir_list( &dir_list_root );

         x_RETHROW( &x1 );
      }
   }
   x_CATCH( &x0 )
   {
      fprintf( stderr,
               "\nBindHelp: %x - %s\n\n",
               x0.error -> errnum,
               x0.error -> errmess
             );
      rc = 1;
   }
   return rc;
}
