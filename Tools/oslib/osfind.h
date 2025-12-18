#ifndef osfind_H
#define osfind_H

/*OSLib---efficient, type-safe, transparent, extensible,\n"
   register-safe API coverage of RISC OS*/
/*Copyright (c) 1994 Jonathan Coxhead*/

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

#ifndef types_H
   #include "types.h"
#endif

#ifndef fileswitch_H
   #include "fileswitch.h"
#endif

#ifndef os_H
   #include "os.h"
#endif

/**********************************
 * SWI names and SWI reason codes *
 **********************************/
#define OSFind_Close 0x0
#define OSFind_Openin 0x40
#define OSFind_OpeninPath 0x41
#define OSFind_OpeninPathVar 0x42
#define OSFind_OpeninNoPath 0x43
#define OSFind_Openout 0x80
#define OSFind_OpenoutPath 0x81
#define OSFind_OpenoutPathVar 0x82
#define OSFind_OpenoutNoPath 0x83
#define OSFind_Openup 0xC0
#define OSFind_OpenupPath 0xC1
#define OSFind_OpenupPathVar 0xC2
#define OSFind_OpenupNoPath 0xC3

/********************
 * Type definitions *
 ********************/
typedef bits osfind_flags;

/************************
 * Constant definitions *
 ************************/
#define osfind_PATH                             0x1u
#define osfind_PATH_VAR                         0x2u
#define osfind_NO_PATH                          0x3u
#define osfind_ERROR_IF_ABSENT ((bits) 0x8u)
#define osfind_ERROR_IF_DIR ((bits) 0x4u)

#if defined EXECUTE_ON_UNIX
#ifdef __cplusplus
   extern "C" {
#endif

/* ------------------------------------------------------------------------
 * Function:      osfind_openin()
 *
 * Description:   Opens an existing file with read access only. Prefer
 *                OSFind_OpeninW
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

extern os_error *xosfind_openin (osfind_flags flags,
      char const *file_name,
      char const *path,
      os_f *file);
extern os_f osfind_openin (osfind_flags flags,
      char const *file_name,
      char const *path);

/* ------------------------------------------------------------------------
 * Function:      osfind_openinw()
 *
 * Description:   Opens an existing file with read access only. Uses 32-bit
 *                file handle.
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

extern os_error *xosfind_openinw (osfind_flags flags,
      char const *file_name,
      char const *path,
      os_fw *file);
extern os_fw osfind_openinw (osfind_flags flags,
      char const *file_name,
      char const *path);

/* ------------------------------------------------------------------------
 * Function:      osfind_openout()
 *
 * Description:   Creates a new file with read/write access. Prefer
 *                OSFind_OpenoutW
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

extern os_error *xosfind_openout (osfind_flags flags,
      char const *file_name,
      char const *path,
      os_f *file);
extern os_f osfind_openout (osfind_flags flags,
      char const *file_name,
      char const *path);

/* ------------------------------------------------------------------------
 * Function:      osfind_openoutw()
 *
 * Description:   Creates a new file with read/write access. Uses 32-bit
 *                file handle.
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

extern os_error *xosfind_openoutw (osfind_flags flags,
      char const *file_name,
      char const *path,
      os_fw *file);
extern os_fw osfind_openoutw (osfind_flags flags,
      char const *file_name,
      char const *path);

/* ------------------------------------------------------------------------
 * Function:      osfind_openup()
 *
 * Description:   Opens an existing file with read/write access. Prefer
 *                OSFind_OpenupW
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

extern os_error *xosfind_openup (osfind_flags flags,
      char const *file_name,
      char const *path,
      os_f *file);
extern os_f osfind_openup (osfind_flags flags,
      char const *file_name,
      char const *path);

/* ------------------------------------------------------------------------
 * Function:      osfind_openupw()
 *
 * Description:   Opens an existing file with read/write access. Uses
 *                32-bit file handle.
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
      os_fw *file);
extern os_fw osfind_openupw (osfind_flags flags,
      char const *file_name,
      char const *path);

/* ------------------------------------------------------------------------
 * Function:      osfind_close()
 *
 * Description:   Closes a file or files. Prefer OSFind_CloseW
 *
 * Input:         file - value of R1 on entry
 *
 * Other notes:   Calls SWI 0xD with R0 = 0x0.
 */

extern os_error *xosfind_close (os_f file);
extern void osfind_close (os_f file);

/* ------------------------------------------------------------------------
 * Function:      osfind_closew()
 *
 * Description:   Closes a file or files. Uses 32-bit file handle.
 *
 * Input:         file - value of R1 on entry
 *
 * Other notes:   Calls SWI 0xD with R0 = 0x0.
 */

extern os_error *xosfind_closew (os_fw file);
extern void osfind_closew (os_fw file);

/* ------------------------------------------------------------------------
 * Function:      upcallfind_create_openup()
 *
 * Description:   Warns your program that a file is being created and
 *                opened for update. Prefer UpCallFind_CreateOpenupW
 *
 * Input:         file_name - value of R1 on entry
 *                file - value of R2 on entry
 *                special - value of R6 on entry
 *                fs_info - value of R8 on entry
 *
 * Other notes:   Calls SWI 0x33 with R0 = 0x3, R9 = 0x101.
 */

extern os_error *xupcallfind_create_openup (char const *file_name,
      os_f file,
      char const *special,
      fileswitch_fs_info fs_info);
extern void upcallfind_create_openup (char const *file_name,
      os_f file,
      char const *special,
      fileswitch_fs_info fs_info);

/* ------------------------------------------------------------------------
 * Function:      upcallfind_create_openupw()
 *
 * Description:   Warns your program that a file is being created and
 *                opened for update. Uses 32-bit file handle.
 *
 * Input:         file_name - value of R1 on entry
 *                file - value of R2 on entry
 *                special - value of R6 on entry
 *                fs_info - value of R8 on entry
 *
 * Other notes:   Calls SWI 0x33 with R0 = 0x3, R9 = 0x101.
 */

extern os_error *xupcallfind_create_openupw (char const *file_name,
      os_fw file,
      char const *special,
      fileswitch_fs_info fs_info);
extern void upcallfind_create_openupw (char const *file_name,
      os_fw file,
      char const *special,
      fileswitch_fs_info fs_info);

/* ------------------------------------------------------------------------
 * Function:      upcallfind_openup()
 *
 * Description:   Warns your program that a file is being opened for
 *                update. Prefer UpCallFind_OpenupW
 *
 * Input:         file_name - value of R1 on entry
 *                file - value of R2 on entry
 *                special - value of R6 on entry
 *                fs_info - value of R8 on entry
 *
 * Other notes:   Calls SWI 0x33 with R0 = 0x3, R9 = 0x102.
 */

extern os_error *xupcallfind_openup (char const *file_name,
      os_f file,
      char const *special,
      fileswitch_fs_info fs_info);
extern void upcallfind_openup (char const *file_name,
      os_f file,
      char const *special,
      fileswitch_fs_info fs_info);

/* ------------------------------------------------------------------------
 * Function:      upcallfind_openupw()
 *
 * Description:   Warns your program that a file is being opened for
 *                update. Uses 32-bit file handle.
 *
 * Input:         file_name - value of R1 on entry
 *                file - value of R2 on entry
 *                special - value of R6 on entry
 *                fs_info - value of R8 on entry
 *
 * Other notes:   Calls SWI 0x33 with R0 = 0x3, R9 = 0x102.
 */

extern os_error *xupcallfind_openupw (char const *file_name,
      os_fw file,
      char const *special,
      fileswitch_fs_info fs_info);
extern void upcallfind_openupw (char const *file_name,
      os_fw file,
      char const *special,
      fileswitch_fs_info fs_info);

/* ------------------------------------------------------------------------
 * Function:      upcallfind_close()
 *
 * Description:   Warns your program that a file is being closed. Prefer
 *                UpCallFind_CloseW
 *
 * Input:         file - value of R1 on entry
 *                fs_info - value of R8 on entry
 *
 * Other notes:   Calls SWI 0x33 with R0 = 0x3, R9 = 0x103.
 */

extern os_error *xupcallfind_close (os_f file,
      fileswitch_fs_info fs_info);
extern void upcallfind_close (os_f file,
      fileswitch_fs_info fs_info);

/* ------------------------------------------------------------------------
 * Function:      upcallfind_closew()
 *
 * Description:   Warns your program that a file is being closed. Uses
 *                32-bit file handle.
 *
 * Input:         file - value of R1 on entry
 *                fs_info - value of R8 on entry
 *
 * Other notes:   Calls SWI 0x33 with R0 = 0x3, R9 = 0x103.
 */

extern os_error *xupcallfind_closew (os_fw file,
      fileswitch_fs_info fs_info);
extern void upcallfind_closew (os_fw file,
      fileswitch_fs_info fs_info);

#ifdef __cplusplus
   }
#endif

#ifndef osfind32_H
#include "oslib/osfind32.h"
#endif

#else		/* EXECUTE_ON_UNIX */

#define xosfind_close(file) (os_error *)_swix (OS_Find, _IN (0) | _IN (1), \
      OSFind_Close, (os_f) (file))

#define xosfind_closew(file) (os_error *)_swix (OS_Find, _IN (0) | _IN (1), \
      OSFind_Close, (os_fw) (file))

#define osfind_closew(file) _swi (OS_Find, _IN (0) | _IN (1), \
      OSFind_Close, (os_fw) (file))

#define xosfind_openin(flags, file_name, path, file) \
   (os_error *)_swix (OS_Find, _IN (0) | _IN (1) | _IN (2) | _OUT (0), \
         OSFind_Openin | (bits) (flags), (char *) (file_name), \
         (char *) (path), (os_f *) (file))

#define xosfind_openinw(flags, file_name, path, file) \
   (os_error *)_swix (OS_Find, _IN (0) | _IN (1) | _IN (2) | _OUT (0), \
         OSFind_Openin | (bits) (flags), (char *) (file_name), \
         (char *) (path), (os_fw *) (file))

#define xosfind_openout(flags, file_name, path, file) \
   (os_error *)_swix (OS_Find, _IN (0) | _IN (1) | _IN (2) | _OUT (0), \
         OSFind_Openout | (bits) (flags), (char *) (file_name), \
         (char *) (path), (os_f *) (file))

#define xosfind_openoutw(flags, file_name, path, file) \
   (os_error *)_swix (OS_Find, _IN (0) | _IN (1) | _IN (2) | _OUT (0), \
         OSFind_Openout | (bits) (flags), (char *) (file_name), \
         (char *) (path), (os_fw *) (file))

#define osfind_openoutw(flags, file_name, path) \
   _swi (OS_Find, _IN (0) | _IN (1) | _IN (2) | _RETURN (0), \
         OSFind_Openout | (bits) (flags), (char *) (file_name), \
         (char *) (path))

#endif		/* EXECUTE_ON_UNIX */

#endif
