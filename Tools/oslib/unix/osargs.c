/*
 * OS Lib very limited port
 */

#include <stdio.h>

#include "oslib/os.h"
#include "oslib/osargs.h"

#include "support/kernel.h"

extern os_error *os_set_lasterr( const os_error *err );

/* ------------------------------------------------------------------------
 * Function:      osargs_set_ptrw()
 *
 * Description:   Writes an open file's sequential file pointer
 *
 * Input:         file - value of R1 on entry
 *                ptr - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x9 with R0 = 0x1.
 */

extern os_error *xosargs_set_ptrw (os_fw file,
      int ptr)
{
	fseek((FILE*)file, ptr, SEEK_SET);
	return OS_ERROR( NULL );
}

extern void osargs_set_ptrw (os_fw file,
      int ptr)
{
	os_generate_error( xosargs_set_ptrw(file, ptr) );
}

/* ------------------------------------------------------------------------
 * Function:      osargs_read_extw()
 *
 * Description:   Reads an open file's extent
 *
 * Input:         file - value of R1 on entry
 *
 * Output:        ext - value of R2 on exit (X version only)
 *
 * Returns:       R2 (non-X version only)
 *
 * Other notes:   Calls SWI 0x9 with R0 = 0x2.
 */

extern os_error *xosargs_read_extw (os_fw file,
      int *ext)
{
	fseek((FILE*)file, 0, SEEK_END);
	if (ext) *ext = ftell((FILE*)file);
	return NULL;
}

extern int osargs_read_extw (os_fw file)
{
	int ext;
	os_generate_error( xosargs_read_extw(file, &ext) );
	return ext;
}

/* ------------------------------------------------------------------------
 * Function:      osargs_read_eof_statusw()
 *
 * Description:   Reads an open file's end-of-file (EOF) status
 *
 * Input:         file - value of R1 on entry
 *
 * Output:        eof_status - value of R2 on exit (X version only)
 *
 * Returns:       R2 (non-X version only)
 *
 * Other notes:   Calls SWI 0x9 with R0 = 0x5.
 */

extern os_error *xosargs_read_eof_statusw (os_fw file,
      osbool *eof_status)
{
	if (eof_status) *eof_status = feof((FILE*)file);
	return NULL;
}

extern osbool osargs_read_eof_statusw (os_fw file)
{
	osbool eof_status;
	os_generate_error( xosargs_read_eof_statusw(file, &eof_status) );
	return eof_status;
}

/* ------------------------------------------------------------------------
 * Function:      osargs_ensurew()
 *
 * Description:   Ensures data has been written to a file, or to all files
 *                on the temporary filing system. Uses 32-bit file handle.
 *
 * Input:         file - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x9 with R0 = 0xFF.
 */

extern os_error *xosargs_ensurew (os_fw file)
{
	fflush( (FILE*)file );
	return NULL;
}

extern void osargs_ensurew (os_fw file)
{
	os_generate_error( xosargs_ensurew( file ) );
}


