/* test some Wide file handles
*/

//#define TEST_OS_F

// Standard C includes
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

// OSLib includes
#define os_f junk

#include "oslib/osfile.h"
#include "oslib/osfind.h"
#include "oslib/osgbpb.h"
#include "oslib/osargs.h"
#include "oslib/colourtrans.h"

//#undef os_f

// from OSLibSupport
#include "Err.h"	// error handling library
#include "x.h"	      	// exception library

#define BUFFSIZE 128
#define ERRNO( x ) ( 1<<30 | x )


// globals to persist over try/catch blocks
static int iFiles;
static os_f ahFile[512];
static os_fw hFile = 0;

#if defined TEST_OS_F
   static os_f hFile0 = 0;
#endif


////////////////////////////////////////////////
int main( int argc, char* argv[] )
{

   int iFilePos;
   int iChars;
   int iMaxHandle;
   int iMinHandle;
   int i = 0;
   int i1 = 0;

   char c;
   char acPathName[512];

   byte aucBuff[ BUFFSIZE ];

//   const char* TestFile = "<Wimp$ScrapDir>.FW_File";
//   const char* TestPath = "Test$Path";
   const char* TestData[] = "Hello World!\n";
   char* TestDir0 = "LanMan98::tony.$._tmp.test";
   char* TestDir;

   // set up some error messages
   x_LOCAL_ERROR( Error_cant_open, ERRNO( 1 ), "'Failed to open test file" );
   x_LOCAL_ERROR( Error_cant_write, ERRNO( 2 ), "OSGBPB write failed" );
   x_LOCAL_ERROR( Error_bad_ptr, ERRNO( 3 ), "Bad file pointer returned" );
   x_LOCAL_ERROR( Error_cant_read, ERRNO( 4 ), "OSGPBP Read failed" );
   x_LOCAL_ERROR( Error_bad_EOF, ERRNO( 5 ), "Expected Eof - not set" );
   x_LOCAL_ERROR( Error_bad_handle, ERRNO( 6 ), "Bad file handle returned" );
   x_LOCAL_ERROR( Error_bad_extent, ERRNO( 7 ), "File extent does not match file pointer" );
#if 0
   x_LOCAL_ERROR( Error_bad_allocation, ERRNO( 8 ), "File allocation does not match allocated size" );
#endif
   x_LOCAL_ERROR( Error_buff_inadequate, ERRNO( 9 ), "Insufficient buffer space for operation" );
   x_LOCAL_ERROR( Error_bad_char, ERRNO( 10 ), "Bad character returned" );
   x_LOCAL_ERROR( Error_bad_path, ERRNO( 11 ), "Bad path name returned" );

   x_exception x0;
   x_exception x1;

   // introduce ourselves
   printf( "OSLib wide file handle test - Tony van der Hoff "__DATE__"\n\n" );

   // get command line arguments
   if( argc > 1 )
   {
      TestDir = argv[1];
   }
   else
   {
      TestDir = TestDir0;
   }
   printf( "Test Directory = %s\n", TestDir );

   printf( "Size of os_f = %d bits\n", 8 * sizeof( os_f ) );

   x_TRY( &x0 )   // Outer exception block
   {
      // open a file for I/O
#if defined TEST_OS_F
      sprintf( acPathName, "%s.test0", TestDir );
      hFile0 = osfind_openup( osfind_NO_PATH,
                              acPathName, 0
                            );
#endif

      printf( "Opening File\n" );
      sprintf( acPathName, "%s.test", TestDir );
      hFile = osfind_openup( osfind_NO_PATH,
                              acPathName, 0
                            );
      if( hFile == 0 )
      {
         printf( "   failed - assume it needs creating\n" );
         hFile = osfind_openout( osfind_NO_PATH,
                                 acPathName, 0
                                );
      }

      // we have a File handle - tell the world
      printf( "   Output File handle: %i", hFile );
      if( hFile == 0 )
         x_THROW( Error_cant_open );
      printf( " -- OK\n\n" );

      // write to the file
      printf( " Writing %i bytes of test data: %s",
               strlen( (char*)TestData[0] ) + 1,
               (char*)TestData[0]
            );
      if( osgbpb_write( hFile,
                        (byte*)(TestData[0]),
                        strlen( TestData[0]) + 1
                      ) != 0 )
         x_THROW( Error_cant_write );
      printf( " -- OK\n\n" );

      // test the file position
      iFilePos = osargs_read_ptr( hFile );
      printf( "File position %i", iFilePos );
      if( iFilePos != strlen( TestData[0] ) + 1 )
         x_THROW( Error_bad_ptr );
      printf( " -- OK\n\n" );

      // read extent
      printf( "Testing File extent" );
      i = osargs_read_ext( hFile );
      if( i != iFilePos )
         x_THROW( Error_bad_extent );
      printf( " -- OK\n\n" );

      // read the data back
      memset( aucBuff, '\0', BUFFSIZE );
      printf( "Reading back %i bytes of test data\n", iFilePos );
      iChars = osgbpb_read_at( hFile, aucBuff, iFilePos, 0 );
      printf( "   %i Characters unread\n", iChars );
//      *(aucBuff + iFilePos - iChars) = '\0';
      printf( "   Read buffer: %s\n", (char*)aucBuff );
      if( iChars != 0
            || strcmp( (char*)aucBuff, TestData[0] ) != 0
        )
         x_THROW( Error_cant_read );
      printf( " -- OK\n\n" );

      // rewind the pointer to "world"
      #define WORLDPOS 6
      printf( "\nSetting pointer to file position %i", WORLDPOS );
      osargs_set_ptr( hFile, WORLDPOS );
      iFilePos = osargs_read_ptr( hFile );
      if( iFilePos != WORLDPOS  )
         x_THROW( Error_bad_ptr );
      printf( " -- OK\n\n" );

      // BGet this char
      printf( "Testing OS_BGet" );
      os_bget( hFile, &c );
      if( c != *(TestData[0] + WORLDPOS ) )
         x_THROW( Error_bad_char );
      printf( " -- OK\n\n" );

      // BPut in lower case
      printf( "Testing OS_BPut" );
      osargs_set_ptr( hFile, WORLDPOS );
      os_bput( tolower(c), hFile );
      printf( " -- OK\n\n" );

      // read to EOF
      printf( "Reading to EoF\n" );
      osargs_set_ptr( hFile, WORLDPOS );
      iChars = osgbpb_read( hFile, aucBuff, BUFFSIZE );
      printf( "   %i Characters read in buffer\n", BUFFSIZE - iChars );
      *(aucBuff + BUFFSIZE - iChars) = '\0';
      printf( "   Read buffer: %s\n", (char*)aucBuff );

      // undo our bput
      if( *aucBuff != *(TestData[0] + WORLDPOS) )
         *aucBuff = toupper( *aucBuff );
      else
         x_THROW( Error_bad_char );

      if( strcmp( TestData[0] + WORLDPOS, (char*)aucBuff ) != 0 )
         x_THROW( Error_cant_read );
      printf( " -- OK\n\n" );

      printf( "Testing EoF status" );
      if( !osargs_read_eof_status( hFile ) )
         x_THROW( Error_bad_EOF );
      printf( " -- OK\n\n" );

      // set extent to 0 and read it back
      printf( "Setting file extent to 0" );
      osargs_set_ext( hFile, 0 );
      if( osargs_read_ext( hFile ) != 0 )
         x_THROW( Error_bad_extent );
      printf( " -- OK\n\n" );

      // get allocation and double it
      printf( "Reading file allocation\n" );
      i = osargs_read_allocation( hFile );
      printf( "Allocation read %i; Setting to %i\n", i, i*2 );
      osargs_set_allocation( hFile, i*2 );
      i1 = osargs_read_allocation( hFile );
      printf( "Allocation read %i", i1 );
//      if( i1 != i*2  )
//         x_THROW( Error_bad_allocation );
      printf( " -- OK\n\n" );

      // get canonicalised name
      printf( "Reading Canonicalised name: " );
      if( osargs_read_path( hFile, (char*)aucBuff, BUFFSIZE ) <= 0 )
         x_THROW( Error_buff_inadequate );
      printf( "%s\n", (char*)aucBuff );
      if( strcmp( (char*)aucBuff, acPathName ) != 0 )
         x_THROW( Error_bad_path );
      printf( " -- OK\n\n" );

      // write ColourTrans calibration
      printf( "Writing ColourTrans calibration to file" );
      colourtrans_write_calibration_to_file( colourtrans_WRITE_DEFAULT_CALIBRATION, hFile );
      printf( " -- OK\n\n" );

      // ensure the file
      printf( "Ensuring file" );
      osargs_ensure( hFile );
      printf( " -- OK\n\n" );
   }
   x_CATCH( &x0 )
   {
      printf( "\n\nError %i - %s\n\n", x0.error -> errnum, x0.error -> errmess );
   }

   // close the file if open
   if( hFile != 0 )
   {
      printf( "closing handle %i\n", hFile );
      osfind_close( hFile );
      hFile = 0;
   }

   // delete it
   sprintf( (char*)aucBuff, "%s.test", TestDir );
   printf( "Deleting file %s\n\n", (char*)aucBuff );
   if( xosfile_delete( (char*)aucBuff, 0, 0, 0, 0, 0 ) != 0 )
      printf( "delete failed\n" );

#if defined TEST_OS_F
   if( hFile0 != 0 )
   {
      printf( "closing handle %i\n", hFile0 );
      osfind_close( hFile0 );
      hFile0 = 0;
   }

   // delete it
   sprintf( (char*)aucBuff, "%s.test0", TestDir );
   printf( "Deleting file %s", (char*)aucBuff );
   xosfile_delete( (char*)aucBuff, 0, 0, 0, 0, 0 );
#endif


   // attempt > 512 file opens
   for( i = 0; i < 512; i++ )
   {
      ahFile[i] = 0;
   }

  printf( "Attempting to open 512 files\n" );

  x_TRY( &x0 )   // Outer exception block
   {
      for( iFiles = 0; iFiles < 512; iFiles++ )
      {
         sprintf( (char*)aucBuff, "%s.test%03i", TestDir, iFiles );

         x_TRY( &x1 )
         {
            ahFile[iFiles] = osfind_openout( osfind_NO_PATH,
                                          (char*)aucBuff, 0
                                        );
#if 1
            printf( "\r   File %s Handle %3i", (char*)aucBuff, ahFile[iFiles] );
#else
            printf( "   File %s Handle %3i\n", (char*)aucBuff, ahFile[iFiles] );
#endif
            if( ahFile[iFiles] == 0 || ahFile[iFiles] == -1 )
               x_THROW( Error_bad_handle );
         }
         x_CATCH( &x1 )
         {
            // catch "too many open files"
            if( x1.error -> errnum == 192 )
            {
               printf( "\n   Limit reached at %i files", iFiles );
               break;
            }

            // catch bad file handle
            else if( x1.error -> errnum != ERRNO( 6 ) )
            {
               printf( "\n   %s: %i at %i", x1.error -> errmess,  ahFile[iFiles], iFiles );
               break;
            }

            else
               x_RETHROW( &x1 );
         }
      }

      printf( "\n   Successfully opened %i files\n", iFiles );
      iMaxHandle = 0;
      iMinHandle = INT_MAX;
      for( i = iFiles; i >= 0; --i )
      {
         if( ahFile[i] < iMinHandle && ahFile[i] != 0 )
            iMinHandle = ahFile[i];
         if( ahFile[i] > iMaxHandle )
            iMaxHandle = ahFile[i];
      }
      printf( "   Largest file handle = %i; Smallest = %i\n\n", iMaxHandle, iMinHandle );


   }
   x_CATCH( &x0 )
   {
      printf( "\n\nError %i - %s\n\n", x0.error -> errnum, x0.error -> errmess );
   }


   x_TRY( &x0 )   // Outer exception block
   {
      // close 'em and delete
      printf( "Closing & Deleting %i files\n ", iFiles );
      for( ; iFiles >= 0; --iFiles )
      {
         if( ahFile[iFiles] != 0 )
         {
            printf( "\r   Closing handle %3i ", ahFile[iFiles] );
            osfind_close( ahFile[ iFiles ] );
            sprintf( (char*)aucBuff, "%s.test%03i", TestDir, iFiles );
#if 1
            printf( " Deleting file %s", (char*)aucBuff );
#else
            printf( " Deleting file %s\n", (char*)aucBuff );
#endif
            osfile_delete( (char*)aucBuff, 0, 0, 0, 0 );
         }
      }
   }
   x_CATCH( &x0 )
   {
      printf( "\n\nError %i - %s\n\n", x0.error -> errnum, x0.error -> errmess );
   }


   // terminate
   printf( "\nOS_FW test complete\n" );
   return 0;
}
