/*
 * OS Lib very limited port
 */

//#define DEBUG

#ifdef DEBUG
# undef DEBUG
# define DEBUG 1
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#if defined(WIN32) && !defined(_WIN32_WCE)
  #include "direct.h"
  #define mkdir _mkdir
#elif defined (UNIX)
  #include <sys/stat.h>		/* for mkdir */
#endif

#include "support/trace.h"
#include "support/riscos.h"

#include "oslib/osfile.h"
#include "oslib/os.h"

/* unix_name -- in osfind.c */
extern const char *unix_name(const char *file_name, bits load_addr, bits exec_addr);

/* os_set_lasterr -- in oslib/unix/os.c */
extern os_error *os_set_lasterr( const os_error* );

/* --- gloabl data ------------------------------------------------------- */

//extern os_error oserr;

/* ------------------------------------------------------------------------
 * Function:      osfile_set_type()
 *
 * Description:   Writes the file type for an object
 *
 * Input:         file_name - value of R1 on entry
 *                file_type - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x8 with R0 = 0x12.
 */

extern os_error *xosfile_set_type (char const *file_name,
      bits file_type)
{
    /* do nothing */
    return NULL;
}

extern void osfile_set_type (char const *file_name,
      bits file_type)
{
    os_generate_error( xosfile_set_type (file_name, file_type) );
}

/* ------------------------------------------------------------------------
 * Function:      osfile_create_dir()
 *
 * Description:   Creates a directory
 *
 * Input:         dir_name - value of R1 on entry
 *                entry_count - value of R4 on entry
 *
 * Other notes:   Calls SWI 0x8 with R0 = 0x8.
 */

extern os_error *xosfile_create_dir (char const *dir_name, int entry_count)
{
    NOT_USED(entry_count);
    os_error oserr;
    os_error* err = 0;

    tracef( "xosfile_create_dir \"%s\"\n" _ dir_name );

#if defined (UNIX)
    if ( mkdir( dir_name, S_IRUSR | S_IWUSR | S_IXUSR | S_IROTH | S_IWOTH | S_IXOTH )
          !=0 && errno!=17 )
#else
    if(mkdir(unix_name(dir_name,0,0))!=0 && errno!=17)
#endif
    {
        oserr.errnum = 0x10001;
        sprintf(oserr.errmess, "osfile_create_dir [%d]: %s", errno, strerror(errno));
        err = &oserr;
    }
    return OS_ERROR(err);
}

extern void osfile_create_dir (char const *dir_name, int entry_count)
{
    os_generate_error( xosfile_create_dir (dir_name, entry_count) );
}

/* ------------------------------------------------------------------------
 * Function:      osfile_create()
 *
 * Description:   Creates an empty untyped file - prefer
 *                OSFile_CreateStamped
 *
 * Input:         file_name - value of R1 on entry
 *                load_addr - value of R2 on entry
 *                exec_addr - value of R3 on entry
 *                size - value of R5 on entry
 *
 * Other notes:   Calls SWI 0x8 with R0 = 0x7, R4 = 0x0.
 */

extern os_error *xosfile_create (char const *file_name,
      bits load_addr,
      bits exec_addr,
      int size)
{
    os_error oserr;
    os_error *err;

    /* dont use load & exec or open file wont find it */
    FILE *f = fopen(unix_name(file_name, 0, 0), "w");

    if(f==NULL)
    {
        oserr.errnum=0x10001;
        strcpy(oserr.errmess, "Unable to create file");
        err = &oserr;
    }
    else
    {
    	fseek(f, size-1, SEEK_SET);
    	fputc(0, f);
    	fclose(f);
    }

    return OS_ERROR(err);
}

extern void osfile_create (char const *file_name,
      bits load_addr,
      bits exec_addr,
      int size)
{
    os_generate_error( xosfile_create (file_name, load_addr, exec_addr, size) );
}


/* ------------------------------------------------------------------------
 * Function:      osfile_read_stamped()
 *
 * Description:   Reads catalogue information and file type for an object
 *                using the directory list in File$Path
 *
 * Input:         file_name - value of R1 on entry
 *
 * Output:        obj_type - value of R0 on exit (X version only)
 *                load_addr - value of R2 on exit
 *                exec_addr - value of R3 on exit
 *                size - value of R4 on exit
 *                attr - value of R5 on exit
 *                file_type - value of R6 on exit
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x8 with R0 = 0x14.
 */

os_error *xosfile_read_stamped (char const *file_name,
      fileswitch_object_type *obj_type_,
      bits *load_addr,
      bits *exec_addr,
      int *size_,
      fileswitch_attr *attr_,
      bits *file_type_)
{
    os_error *err = NULL;
    size_t size;
    fileswitch_object_type obj_type = osfile_NOT_FOUND;
    bits file_type = osfile_TYPE_UNTYPED;
    struct stat st_buf;
    fileswitch_attr attr = 0;

    if( stat( unix_name( file_name, 0, 0 ), &st_buf ) == 0 )
    {
    	// set the attributes
	attr |= (st_buf.st_mode & S_IRUSR) ? fileswitch_ATTR_OWNER_READ  : 0;
	attr |= (st_buf.st_mode & S_IWUSR) ? fileswitch_ATTR_OWNER_WRITE : 0;
	attr |= (st_buf.st_mode & S_IROTH) ? fileswitch_ATTR_WORLD_READ  : 0;
	attr |= (st_buf.st_mode & S_IWOTH) ? fileswitch_ATTR_WORLD_WRITE : 0;

	// test for directory
	if ( S_ISDIR( st_buf.st_mode ) )
	{
    		obj_type = osfile_IS_DIR;
		if ( file_name[0] == '!' )
			file_type = osfile_TYPE_APPLICATION;
		else
			file_type = osfile_TYPE_DIRECTORY;
	}
	// test for regular file
	else if ( S_ISREG( st_buf.st_mode ) )
	{
		obj_type  = osfile_IS_FILE;

		char *s = strrchr(file_name, ',');
		if ( s!=NULL )
    		{
         		sscanf(s, ",%x", &file_type);

			if ( file_type_ ) *file_type_ = file_type;

        		if (file_type==osfile_TYPE_ABSOLUTE)
        		{
            			if (load_addr) *load_addr = 0x8000;
            			if (exec_addr) *exec_addr = 0x8000;
        		}
        		else if (file_type==0xfd3)
        		{
            			if (load_addr) *load_addr = 0x8000;
            			if (exec_addr) *exec_addr = 0x8000;
        		}
	        	else
        		{
            			if (load_addr) *load_addr = 0;
            			if (exec_addr) *exec_addr = 0;
        		}
		}

		size = st_buf.st_size;
		if (size_) *size_ = size;
	}
    }
    if ( file_type_ ) *file_type_ = file_type;
    if ( obj_type_ ) *obj_type_ = obj_type;
    if ( attr_ ) *attr_ = attr;

    return OS_ERROR( err );
}

fileswitch_object_type osfile_read_stamped (char const *file_name,
      bits *load_addr,
      bits *exec_addr,
      int *size,
      fileswitch_attr *attr,
      bits *file_type)
{
    fileswitch_object_type obj_type;
    os_generate_error( xosfile_read_stamped (	file_name,
    						&obj_type,
						load_addr,
						exec_addr,
						size, attr,
						file_type
						) );
    return obj_type;
}

/* ------------------------------------------------------------------------
 * Function:      osfile_load_stamped()
 *
 * Description:   Loads a file using the directory list in File$Path
 *
 * Input:         file_name - value of R1 on entry
 *                addr - value of R2 on entry
 *
 * Output:        obj_type - value of R0 on exit (X version only)
 *                load_addr - value of R2 on exit
 *                exec_addr - value of R3 on exit
 *                size - value of R4 on exit
 *                attr - value of R5 on exit
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x8 with R0 = 0xFF, R3 = 0x0.
 */

os_error *xosfile_load_stamped(
	char const *file_name,
      	byte *addr,
      	fileswitch_object_type *obj_type_,
      	bits *load_addr_,
      	bits *exec_addr_,
      	int *size_,
      	fileswitch_attr *attr_ )
{
    	os_error *err = NULL;
    	fileswitch_object_type obj_type;
	bits load_addr;
	bits exec_addr;
    	int size;
	fileswitch_attr attr;

	err = xosfile_read_stamped (	file_name,
    					&obj_type,
					&load_addr,
					&exec_addr,
					&size,
					&attr,
					NULL
				);

	if (obj_type == osfile_IS_FILE )
	{
    		FILE *f =  fopen(unix_name(file_name,0,0), "rb");
    		if ( f == NULL )
    		{
    			tracef( "xosfile_load_stamped: file \"%s\" not found\n" \
				_ file_name );
    			obj_type  = osfile_NOT_FOUND;
    		}
    		else
    		{
    			if ( fread( addr, (size_t)size, 1, f ) != 1 )
    			{
    				tracef( "xosfile_load_stamped: read error: %s\n" \
					_ strerror(errno) );
    				err = riscos_errno();
    			}
			else
			{
				if (size_) *size_ = size;
				if ( attr_ ) *attr_ = attr;
				if ( load_addr_ ) *load_addr_ = load_addr;
				if ( exec_addr_ ) *exec_addr_ = exec_addr;
			}
    			fclose(f);
    		}
	}
    	if ( obj_type_ ) *obj_type_ = obj_type;
	if ( attr_ ) *attr_ = attr;

    return OS_ERROR( err );
}

fileswitch_object_type osfile_load_stamped (char const *file_name,
      byte *addr,
      bits *load_addr,
      bits *exec_addr,
      int *size,
      fileswitch_attr *attr)
{
    fileswitch_object_type obj_type;
    os_generate_error( xosfile_load_stamped(	file_name,
    						addr,
						&obj_type,
						load_addr,
						exec_addr,
						size,
						attr
						) );
    return obj_type;
}


#if 0
/* ------------------------------------------------------------------------
 * Function:      osfile_load_stamped_path()
 *
 * Description:   Calls OS_File 12 to load a file using a specified
 *                directory list
 *
 * Input:         file_name - value of R1 on entry
 *                addr - value of R2 on entry
 *                path - value of R4 on entry
 *
 * Output:        obj_type - value of R0 on exit (X version only)
 *                load_addr - value of R2 on exit
 *                exec_addr - value of R3 on exit
 *                size - value of R4 on exit
 *                attr - value of R5 on exit
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x8 with R0 = 0xC, R3 = 0x0.
 */

extern os_error *xosfile_load_stamped_path (char const *file_name,
      byte *addr,
      char const *path,
      fileswitch_object_type *obj_type,
      bits *load_addr,
      bits *exec_addr,
      int *size,
      fileswitch_attr *attr);

extern fileswitch_object_type osfile_load_stamped_path (char const *file_name,
      byte *addr,
      char const *path,
      bits *load_addr,
      bits *exec_addr,
      int *size,
      fileswitch_attr *attr);

/* ------------------------------------------------------------------------
 * Function:      osfile_load_stamped_path_var()
 *
 * Description:   Calls OS_File 14 to load a file using the directory list
 *                in a specified variable
 *
 * Input:         file_name - value of R1 on entry
 *                addr - value of R2 on entry
 *                var - value of R4 on entry
 *
 * Output:        obj_type - value of R0 on exit (X version only)
 *                load_addr - value of R2 on exit
 *                exec_addr - value of R3 on exit
 *                size - value of R4 on exit
 *                attr - value of R5 on exit
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x8 with R0 = 0xE, R3 = 0x0.
 */

extern os_error *xosfile_load_stamped_path_var (char const *file_name,
      byte *addr,
      char const *var,
      fileswitch_object_type *obj_type,
      bits *load_addr,
      bits *exec_addr,
      int *size,
      fileswitch_attr *attr);
extern fileswitch_object_type osfile_load_stamped_path_var (char const *file_name,
      byte *addr,
      char const *var,
      bits *load_addr,
      bits *exec_addr,
      int *size,
      fileswitch_attr *attr);
#endif

/* ------------------------------------------------------------------------
 * Function:      osfile_load_stamped_no_path()
 *
 * Description:   Calls OS_File 16 to load a file
 *
 * Input:         file_name - value of R1 on entry
 *                addr - value of R2 on entry
 *
 * Output:        obj_type - value of R0 on exit (X version only)
 *                load_addr - value of R2 on exit
 *                exec_addr - value of R3 on exit
 *                size - value of R4 on exit
 *                attr - value of R5 on exit
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x8 with R0 = 0x10, R3 = 0x0.
 */

extern os_error *xosfile_load_stamped_no_path (char const *file_name,
      byte *addr,
      fileswitch_object_type *obj_type,
      bits *load_addr,
      bits *exec_addr,
      int *size,
      fileswitch_attr *attr)
{
	return xosfile_load_stamped( file_name, addr, obj_type, load_addr, exec_addr, size, attr );
}

extern fileswitch_object_type osfile_load_stamped_no_path (char const *file_name,
      byte *addr,
      bits *load_addr,
      bits *exec_addr,
      int *size,
      fileswitch_attr *attr)
{
	return osfile_load_stamped( file_name, addr, load_addr, exec_addr, size, attr );
}

/* ------------------------------------------------------------------------
 * Function:      osfile_delete()
 *
 * Description:   Calls OS_File 6 to delete an object
 *
 * Input:         file_name - value of R1 on entry
 *
 * Output:        obj_type - value of R0 on exit (X version only)
 *                load_addr - value of R2 on exit
 *                exec_addr - value of R3 on exit
 *                size - value of R4 on exit
 *                attr - value of R5 on exit
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x8 with R0 = 0x6.
 */

extern os_error *xosfile_delete (char const *file_name,
      fileswitch_object_type *obj_type_,
      bits *load_addr_,
      bits *exec_addr_,
      int *size_,
      fileswitch_attr *attr_)
{
	fileswitch_object_type obj_type;
	bits load_addr;
	bits exec_addr;
	int size;
	fileswitch_attr attr;

	char const *name = unix_name( file_name, 0, 0 );

	tracef( "Reading directory entry for \"%s\"\n" _ name );

	os_error *err = xosfile_read_stamped( 	name,
						&obj_type,
						&load_addr,
						&exec_addr,
						&size,
						&attr,
						0
						);

	if (!err)
	{
		if ( remove( file_name ) != 0 )
		{
			// failed
			err = riscos_errno();
		}
		else
		{
			if (obj_type_) *obj_type_ = obj_type;
			if (load_addr_) *load_addr_ = load_addr;
			if (exec_addr_) *exec_addr_ = exec_addr;
			if (size_) *size_ = size;
			if (attr_) *attr_ = attr;
		}
	}

	return OS_ERROR( err );
}

extern fileswitch_object_type osfile_delete(	char const *file_name,
      						bits *load_addr,
      						bits *exec_addr,
      						int *size,
      						fileswitch_attr *attr
					   )
{
	fileswitch_object_type type;
	os_error *err = xosfile_delete( file_name, &type, load_addr, exec_addr, size, attr );
	if ( err )
	{
		tracef( "osfile_delete: error 0x%p\n" _ err );
		os_generate_error ( err );
	}

	return type;
}

/* ------------------------------------------------------------------------
 * Function:      osfile_save()
 *
 * Description:   Saves a block of memory as an untyped file - prefer
 *                OSFile_SaveStamped
 *
 * Input:         file_name - value of R1 on entry
 *                load_addr - value of R2 on entry
 *                exec_addr - value of R3 on entry
 *                data - value of R4 on entry
 *                end - value of R5 on entry
 *
 * Other notes:   Calls SWI 0x8 with R0 = 0x0.
 */

extern os_error *xosfile_save (char const *file_name,
      bits load_addr,
      bits exec_addr,
      byte const *data,
      byte const *end)
{
    os_error oserr;
    os_error *err = 0;

    FILE *f = fopen(unix_name(file_name,load_addr,exec_addr), "w");

    if(f==NULL)
    {
// FIXME: when f is NULL the fwrite will fail + attempt to return a ptr to data on runtime stack !
        oserr.errnum=0x10001;
        strcpy(oserr.errmess, "Unable to save file");
        err = &oserr;
    }

    fwrite(data, (end-data), 1, f);
    fclose(f);

    return err;
}

extern void osfile_save (char const *file_name,
      bits load_addr,
      bits exec_addr,
      byte const *data,
      byte const *end)
{
    os_generate_error( xosfile_save (file_name, load_addr, exec_addr, data, end) );
}
