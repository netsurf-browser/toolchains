/*
 * OS Lib very limited port
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "oslib/osfind.h"
#include "oslib/os.h"

#include "support/trace.h"
#include "support/riscos.h"

/* --- gloabl functoin --------------------------------------------------- */

/*
 * Function:      unix_name
 *			renamed from uname to avoid linking with posix uname
 *
 * Input:         file_name - modified on exit
 *                load_addr
 *                exec_addr
 *
 * Output:        static output buffer
 *
 * Description:   Detects and converts a RISC OS filename to UNIX equivelent
 *                Adds NFS style filetype extension from load & exec addresses
 *
 * Other notes:   NOTE extremely primative implementation
 */

const char *unix_name(const char *file_name, bits load_addr, bits exec_addr)
{
	static char buffer[1024];
	char       *s = strchr(file_name, '$');
	char       *d = buffer;

	if(!s)
	{
		return file_name;
	}

	while(*s)
	{
		switch(*s)
		{
			case '$':	/* ignore */			break;
			case '.':	*(d++) = '/';			break;
			case '/':	*(d++) = '.';			break;
			case '^':	*(d++) = '.';	*(d++) = '.';	break;
			default:	*(d++) = *s;			break;
		}
		s++;
	}
	//*d = 0;

	NOT_USED(exec_addr)

	if((load_addr&0xFFF00000)==0xFFF00000)
		sprintf(d, ",%03X", (load_addr>>8) & 0xFFF);

	return buffer;
}

/* ------------------------------------------------------------------------
 * Function:      osfind_openinw()
 *
 * Description:   Opens an existing file with read access only
 *
 * Input:         flags - value of R0 on entry
 *                file_name - value of R1 on entry
 *                path - value of R2 on entry
 *
 * Output:        file - value of R0 on exit (X version only)
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0xD with R0 |= 0x40.
 */

os_error *xosfind_openinw (osfind_flags flags,
      char const *file_name,
      char const *path,
      os_fw *file_)
{
	os_fw file;

	NOT_USED(flags);
	NOT_USED(path);
	file = (os_fw)fopen(unix_name(file_name,0,0), "rb");

	if (file_) *file_ = file;
	return NULL;
}

os_fw osfind_openinw (osfind_flags flags,
      char const *file_name,
      char const *path)
{
	os_fw file;
	os_generate_error( xosfind_openinw(flags, file_name, path, &file) );
	return file;
}

/* ------------------------------------------------------------------------
 * Function:      osfind_openoutw()
 *
 * Description:   Creates a new file with read/write access
 *
 * Input:         flags - value of R0 on entry
 *                file_name - value of R1 on entry
 *                path - value of R2 on entry
 *
 * Output:        file - value of R0 on exit (X version only)
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0xD with R0 |= 0x80.
 */

extern os_error *xosfind_openoutw(	osfind_flags flags,
					char const *file_name,
      					char const *path,
      					os_fw *file_
				  )
{
	os_fw file = 0;
	os_error *err = 0;
        char const *name = unix_name(file_name,0,0);

	NOT_USED(flags);
	NOT_USED(path);

	tracef( "opening \"%s\"\n" _ name );
	file = (os_fw)fopen( name, "w+b");

	if( file )
	{
		tracef( "success; handle = 0x%x\n" _ file );
		if (file_) *file_ = file;
	}
	else
	{
		err = riscos_errno();
	}
	return err;
}

extern os_fw osfind_openoutw(	osfind_flags flags,
				char const *file_name,
				char const *path
			     )
{
	os_fw file;
	os_generate_error( xosfind_openoutw(flags, file_name, path, &file) );
	return file;
}

/* ------------------------------------------------------------------------
 * Function:      osfind_openupw()
 *
 * Description:   Opens an existing file with read/write access
 *
 * Input:         flags - value of R0 on entry
 *                file_name - value of R1 on entry
 *                path - value of R2 on entry
 *
 * Output:        file - value of R0 on exit (X version only)
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0xD with R0 |= 0xC0.
 */

extern os_error *xosfind_openupw (osfind_flags flags,
      char const *file_name,
      char const *path,
      os_fw *file_)
{
	os_fw file;

	NOT_USED(flags);
	NOT_USED(path);

	file = (os_fw)fopen(unix_name(file_name,0,0), "r+b");

	if (file_) *file_ = file;
	return NULL;
}

extern os_fw osfind_openupw (osfind_flags flags,
      char const *file_name,
      char const *path)
{
	os_fw file;
	os_generate_error( xosfind_openupw(flags, file_name, path, &file) );
	return file;
}

/* ------------------------------------------------------------------------
 * Function:      osfind_closew()
 *
 * Description:   Closes a file or files
 *
 * Input:         file - value of R1 on entry
 *
 * Other notes:   Calls SWI 0xD with R0 = 0x0.
 */

os_error *xosfind_closew (os_fw file)
{
	fclose((FILE*)file);
	return NULL;
}

void osfind_closew (os_fw file)
{
	os_generate_error( xosfind_closew (file) );
}

