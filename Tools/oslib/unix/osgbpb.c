/* 	osgbpb.c
 *
 * OS Lib very limited port
 */

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "support/trace.h"

#include "oslib/os.h"
#include "oslib/osfile.h"
#include "oslib/osgbpb.h"

//extern os_error oserr;

#ifdef EXECUTE_ON_UNIX
#define DIR_SEP "/"
#else
#define DIR_SEP "."
#endif


extern os_error *os_set_lasterr( const os_error *err );

/* ------------------------------------------------------------------------
 * Function:	wild_strcoll()
 *
 * Description:	Performs strcoll using RISC OS wildcards
 *
 * Input:	s1 - pointer to search string
 *		s2 - pointer to (wildcarded) reference string
 *
 * Output:
 *
 * Returns:   	zero if s1 == s2
 *
 * Other notes:	This implementation takes no account of the current locale
 */

static int wild_strcoll( const char *s1, const char* s2 ) 
{
	int stat = 0;

	if( s1 == 0 && s2 == 0 )
		stat = 0;
	else if ( s1 == 0 && s2 != 0 )
		stat = -1;
	else if ( s1 != 0 && s2 == 0 )
	  stat = 1;
	else
	{
		do
		{
			if( *s2 == '#' )
				stat = 0;
			else if ( *s2 == '*' )
				stat = wild_strcoll( strchr( s1, *(s2+1) ), s2+1 );
			else
				stat = *s1 - *s2;
		}
		while ( (!stat) && (*++s1 != '\0') && (*++s2 != '\0') );
	}
	return stat;
}



/* ------------------------------------------------------------------------
 * Function:      osgbpb_readw()
 *
 * Description:   Reads bytes from an open file
 *
 * Input:         file - value of R1 on entry
 *                buffer - value of R2 on entry
 *                size - value of R3 on entry
 *
 * Output:        unread - value of R3 on exit (X version only)
 *
 * Returns:       R3 (non-X version only)
 *
 * Other notes:   Calls SWI 0xC with R0 = 0x4.
 */

os_error *xosgbpb_readw (os_fw file,
      byte *buffer,
      int size,
      int *unread_)
{
	int unread = size;

	if (buffer)
		unread = size - fread(buffer, 1, size, (FILE*)file);

	if (unread_) *unread_ = unread;
	return NULL;	/* should check for error */
}

int osgbpb_readw (os_fw file,
      byte *buffer,
      int size)
{
	int unread;
	xosgbpb_readw(file, buffer, size, &unread);
	return unread;
}

/* ------------------------------------------------------------------------
 * Function:      osgbpb_read_atw()
 *
 * Description:   Reads bytes from an open file at the specified file
 *                pointer
 *
 * Input:         file - value of R1 on entry
 *                buffer - value of R2 on entry
 *                size - value of R3 on entry
 *                ptr - value of R4 on entry
 *
 * Output:        unread - value of R3 on exit (X version only)
 *
 * Returns:       R3 (non-X version only)
 *
 * Other notes:   Calls SWI 0xC with R0 = 0x3.
 */

os_error *xosgbpb_read_atw (os_fw file, byte *buffer,
		int size,
		int ptr,
		int *unread)
{
	fseek((FILE*)file, ptr, SEEK_SET);
	return xosgbpb_readw(file, buffer, size, unread);
}

int osgbpb_read_atw (os_fw file,
		byte *buffer,
		int size,
		int ptr)
{
	int unread;
	xosgbpb_read_atw(file, buffer, size, ptr, &unread);
	return unread;
}

/* ------------------------------------------------------------------------
 * Function:      osgbpb_writew()
 *
 * Description:   Writes bytes to an open file
 *
 * Input:         file - value of R1 on entry
 *                data - value of R2 on entry
 *                size - value of R3 on entry
 *
 * Output:        unwritten - value of R3 on exit (X version only)
 *
 * Returns:       R3 (non-X version only)
 *
 * Other notes:   Calls SWI 0xC with R0 = 0x2.
 */

extern os_error *xosgbpb_writew (os_fw file,
      byte const *data,
      int size,
      int *unwritten_)
{
	int unwritten = size;

	unwritten = size - fwrite(data, 1, size, (FILE*)file);

	if (unwritten_) *unwritten_ = unwritten;
	return NULL;
}

extern int osgbpb_writew (os_fw file,
      byte const *data,
      int size)
{
	int unwritten;
	xosgbpb_writew(file, data, size, &unwritten);
	return unwritten;
}

/* ------------------------------------------------------------------------
 * Function:      osgbpb_write_atw()
 *
 * Description:   Writes bytes to an open file at the specified file
 *                pointer
 *
 * Input:         file - value of R1 on entry
 *                data - value of R2 on entry
 *                size - value of R3 on entry
 *                ptr - value of R4 on entry
 *
 * Output:        unwritten - value of R3 on exit (X version only)
 *
 * Returns:       R3 (non-X version only)
 *
 * Other notes:   Calls SWI 0xC with R0 = 0x1.
 */

extern os_error *xosgbpb_write_atw (os_fw file,
      byte const *data,
      int size,
      int ptr,
      int *unwritten)
{
	fseek((FILE*)file, ptr, SEEK_SET);
	return xosgbpb_writew(file, data, size, unwritten);
}

extern int osgbpb_write_atw (os_fw file,
      byte const *data,
      int size,
      int ptr) 
{
	int unwritten;
	xosgbpb_write_atw(file, data, size, ptr, &unwritten);
	return unwritten;
}



/* ------------------------------------------------------------------------
 * Function:      osgbpb_dir_entries_info()
 *
 * Description:   Reads entries and file information from a specified
 *                directory
 *
 * Input:         dir_name - value of R1 on entry
 *                info_list - value of R2 on entry
 *                count - value of R3 on entry
 *                context - value of R4 on entry
 *                size - value of R5 on entry
 *                entries - value of R6 on entry
 *
 * Output:        read_count - value of R3 on exit
 *                context_out - value of R4 on exit (X version only)
 *
 * Returns:       R4 (non-X version only)
 *
 * Other notes:   Calls SWI 0xC with R0 = 0xA.
 */

extern os_error *xosgbpb_dir_entries_info (	char const *dir_name,
      						osgbpb_info_list *info_list,
      						int count_,
      						int context_,
      						int size,
      						char const *entries,
      						int *read_count,
      						int *context_out
					)
{
	os_error oserr;
	os_error *err = 0;
	int count = 0;
	int context = 0;

	DIR *dir = opendir( dir_name );
	if (!dir)
	{
		oserr.errnum = 0x10001;
		sprintf( oserr.errmess, "%s", strerror( errno ) );
		err = &oserr;
	}
	else
	{
		struct dirent *entry;

		// iterate through the entries until start context is reached
		for( context = 0; context < context_;  context++ )
		{
			entry = readdir( dir );
			if ( !entry )
			{
				// end of list
				context = -1;
				break;
			}
			//tracef( "Ignoring entry: %s\n" _ entry -> d_name );
		}

		// now read the selected entries
		char *list_entry = (char*)info_list;
		while ((count < count_) &&
			(context >= 0) &&
			( err == 0 )
			)
		{
			entry = readdir( dir );
			if ( !entry )
			{
				// end of list
				context = -1;
				break;
			}
			else if ( strcmp( entry -> d_name, "." ) == 0 )
			{
				// ignore current and parent directories
				++context;
			}
			else if ( strcmp( entry -> d_name, ".." ) == 0 )
			{
				// ignore current and parent directories
				++context;
			}
			else
			{
				tracef( "Found entry: %s\n" _ entry -> d_name );
				if ( 	( !entries ) ||
					(*entries == '\0') ||
					( wild_strcoll( entries, entry -> d_name ) == 0)
				   )
				{
					/* this one matches spec; add it to the list */
					osgbpb_info_base info;

					/* make certain there's room in the list  */
					if ( 	list_entry
						- (char*)info_list
						+ sizeof( info )
						+ strlen( entry -> d_name )
							< size
					   )
					{
						/* allocate a working buffer
						   for the path name */
						char temp[ 	strlen( dir_name ) +
								strlen( entry -> d_name ) +
								2
							 ];

						sprintf( temp,
							 "%s"DIR_SEP"%s",
							 dir_name,
							 entry->d_name );
						err = xosfile_read_stamped(
								temp,
								&info.obj_type,
								&info.load_addr,
								&info.exec_addr,
								&info.size,
								&info.attr,
								0
									  );

						if (!err)
						{
							memcpy( list_entry,
								&info,
								sizeof( info ) );
							list_entry += sizeof( info );
							strcpy( list_entry,
								entry -> d_name );
							list_entry +=
								strlen( entry -> d_name )
								+ 1;

							++context;
							++count;
						}
					}
					else
					{
						/* list full */
						tracef( "List Full\n" );
						break;
					}
				}
			}
		}
		closedir( dir );
	}
	if (!err)
	{
		if (read_count) *read_count = count;
		if (context_out) *context_out = context;
	}

	return OS_ERROR(err);
}

extern int osgbpb_dir_entries_info (char const *dir_name,
      osgbpb_info_list *info_list,
      int count,
      int context,
      int size,
      char const *entries,
      int *read_count)
{
	int context_out;
	os_generate_error( xosgbpb_dir_entries_info(	dir_name,
							info_list,
							count,
							context,
							size,
							entries,
							read_count,
							&context_out
						) 
			);
	return context_out;
}

