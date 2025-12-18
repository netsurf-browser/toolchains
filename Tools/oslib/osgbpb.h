#ifndef osgbpb_H
#define osgbpb_H

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

#define OSGBPB_WriteAt 0x1
#define OSGBPB_Write 0x2
#define OSGBPB_ReadAt 0x3
#define OSGBPB_Read 0x4
#define OSGBPB_ReadDiscName 0x5
#define OSGBPB_ReadCSDName 0x6
#define OSGBPB_ReadLibName 0x7
#define OSGBPB_CSDEntries 0x8
#define OSGBPB_DirEntries 0x9
#define OSGBPB_DirEntriesBriefInfo 0xA
#define OSGBPB_DirEntriesInternalInfo 0xB
#define OSGBPB_DirEntriesFullInfo 0xC

typedef struct osgbpb_name 			osgbpb_name;
typedef struct osgbpb_disc_name 		osgbpb_disc_name;
typedef struct osgbpb_dir_name 		osgbpb_dir_name;
typedef struct osgbpb_name_list 		osgbpb_name_list;
typedef struct osgbpb_info 			osgbpb_info;
typedef struct osgbpb_info_base		osgbpb_info_base;
typedef struct osgbpb_brief_info		osgbpb_brief_info;
typedef struct osgbpb_internal_info 		osgbpb_internal_info;
typedef struct osgbpb_full_info 		osgbpb_full_info;
typedef struct osgbpb_string_list 		osgbpb_string_list;
typedef struct osgbpb_brief_info_list 		osgbpb_brief_info_list;
typedef struct osgbpb_internal_info_list 	osgbpb_internal_info_list;
typedef struct osgbpb_full_info_list 		osgbpb_full_info_list;
typedef struct osgbpb_info_list 		osgbpb_info_list;
typedef struct osgbpb_system_info_list          osgbpb_system_info_list;
typedef struct osgbpb_system_info_list_base     osgbpb_system_info_list_base;
typedef struct osgbpb_system_info               osgbpb_system_info;
typedef struct osgbpb_system_info_base          osgbpb_system_info_base;
typedef struct osgbpb_info_stamped              osgbpb_info_stamped;
typedef struct osgbpb_info_stamped_base         osgbpb_info_stamped_base;
typedef struct osgbpb_info_stamped_list         osgbpb_info_stamped_list;
typedef struct osgbpb_info_stamped_list_base    osgbpb_info_stamped_list_base;


/*Type definitions*/
struct osgbpb_name
   {  byte count;
      char (c) [UNKNOWN];
   };

#define osgbpb_NAME(N) \
      struct \
         {  byte count; \
            char (c) [N]; \
         }

#define osgbpb_SIZEOF_NAME(N) \
      (offsetof (osgbpb_name, c) + (N)*sizeof ((osgbpb_name *) NULL)->c)

struct osgbpb_disc_name
   {  osgbpb_name name;
      byte boot_option;
   };

struct osgbpb_dir_name
   {  byte reserved;
      osgbpb_name name;
      byte privilege;
   };

struct osgbpb_name_list
   {  osgbpb_name (names) [UNKNOWN];
   };

struct osgbpb_brief_info
   {  int load_addr;
      int exec_addr;
      int size;
      int attr;
      int obj_type;
      char (name) [UNKNOWN];
   };

#define osgbpb_INFO_MEMBERS \
   bits load_addr; \
   bits exec_addr; \
   int size; \
   fileswitch_attr attr; \
   fileswitch_object_type obj_type;

/* Base osgbpb_info structure without variable part */
struct osgbpb_info_base
   {  osgbpb_INFO_MEMBERS
   };

/* legacy structure */
struct osgbpb_info
   {  osgbpb_INFO_MEMBERS
      char (name) [UNKNOWN];
   };

#define osgbpb_BRIEF_INFO(N) \
      struct \
         {  int load_addr; \
            int exec_addr; \
            int size; \
            int attr; \
            int obj_type; \
            char (name) [N]; \
         }

#define osgbpb_SIZEOF_BRIEF_INFO(N) \
      (offsetof (osgbpb_brief_info, name) + (N)*sizeof ((osgbpb_brief_info *) NULL)->name)

struct osgbpb_internal_info
   {  int load_addr;
      int exec_addr;
      int size;
      int attr;
      int obj_type;
      int sin;
      os_t stamp;
      char (name) [UNKNOWN];
   };

#define osgbpb_INTERNAL_INFO(N) \
      struct \
         {  int load_addr; \
            int exec_addr; \
            int size; \
            int attr; \
            int obj_type; \
            int sin; \
            os_t stamp; \
            char (name) [N]; \
         }

#define osgbpb_SIZEOF_INTERNAL_INFO(N) \
      (offsetof (osgbpb_internal_info, name) + (N)*sizeof ((osgbpb_internal_info *) NULL)->name)

struct osgbpb_full_info
   {  int load_addr;
      int exec_addr;
      int size;
      int attr;
      int obj_type;
      int file_type;
      char (name) [UNKNOWN];
   };

#define osgbpb_FULL_INFO(N) \
      struct \
         {  int load_addr; \
            int exec_addr; \
            int size; \
            int attr; \
            int obj_type; \
            int file_type; \
            char (name) [N]; \
         }

#define osgbpb_SIZEOF_FULL_INFO(N) \
      (offsetof (osgbpb_full_info, name) + (N)*sizeof ((osgbpb_full_info *) NULL)->name)

#define osgbpb_SYSTEM_INFO_MEMBERS \
   bits load_addr; \
   bits exec_addr; \
   int size; \
   fileswitch_attr attr; \
   fileswitch_object_type obj_type; \
   int sin; \
   os_date_and_time stamp;

/* Base osgbpb_system_info structure without variable part */
struct osgbpb_system_info_base
   {  osgbpb_SYSTEM_INFO_MEMBERS
   };

/* legacy structure */
struct osgbpb_system_info
   {  osgbpb_SYSTEM_INFO_MEMBERS
      char name [UNKNOWN];
   };

#define osgbpb_SYSTEM_INFO(N) \
   struct \
      {  osgbpb_SYSTEM_INFO_MEMBERS \
         char name [N]; \
      }

#define osgbpb_SIZEOF_SYSTEM_INFO(N) \
   (offsetof (osgbpb_system_info, name) + \
         (N)*sizeof ((osgbpb_system_info *) NULL)->name)

#define osgbpb_INFO_STAMPED_MEMBERS \
   bits load_addr; \
   bits exec_addr; \
   int size; \
   fileswitch_attr attr; \
   fileswitch_object_type obj_type; \
   bits file_type;

/* Base osgbpb_info_stamped structure without variable part */
struct osgbpb_info_stamped_base
   {  osgbpb_INFO_STAMPED_MEMBERS
   };

/* legacy structure */
struct osgbpb_info_stamped
   {  osgbpb_INFO_STAMPED_MEMBERS
      char name [UNKNOWN];
   };

#define osgbpb_INFO_STAMPED(N) \
   struct \
      {  osgbpb_INFO_STAMPED_MEMBERS \
         char name [N]; \
      }

#define osgbpb_SIZEOF_INFO_STAMPED(N) \
   (offsetof (osgbpb_info_stamped, name) + \
         (N)*sizeof ((osgbpb_info_stamped *) NULL)->name)

struct osgbpb_string_list
   {  char (names) [UNKNOWN];
   };

struct osgbpb_info_list
   {  osgbpb_info (info) [UNKNOWN];
   };

struct osgbpb_brief_info_list
   {  osgbpb_brief_info (info) [UNKNOWN];
   };

struct osgbpb_internal_info_list
   {  osgbpb_internal_info (info) [UNKNOWN];
   };

struct osgbpb_full_info_list
   {  osgbpb_full_info (info) [UNKNOWN];
   };

struct osgbpb_system_info_list
   {  osgbpb_system_info info [UNKNOWN];
   };

struct osgbpb_info_stamped_list
   {  osgbpb_info_stamped info [UNKNOWN];
   };

#define osgbpb_PRIVILEGE_OWNER ((byte) 0)
#define osgbpb_PRIVILEGE_PUBLIC ((byte) 255)
#define osgbpb_NO_MORE (-1)

#if defined EXECUTE_ON_UNIX
#ifdef __cplusplus
   extern "C" {
#endif

/* ------------------------------------------------------------------------
 * Function:      osgbpb_write_at()
 *
 * Description:   Writes bytes to an open file at the specified file
 *                pointer. Prefer OSGBPB_WriteAtW
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

extern os_error *xosgbpb_write_at (os_f file,
      byte const *data,
      int size,
      int ptr,
      int *unwritten);
extern int osgbpb_write_at (os_f file,
      byte const *data,
      int size,
      int ptr);

/* ------------------------------------------------------------------------
 * Function:      osgbpb_write_atw()
 *
 * Description:   Writes bytes to an open file at the specified file
 *                pointer. Uses 32-bit file handle.
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
      int *unwritten);
extern int osgbpb_write_atw (os_fw file,
      byte const *data,
      int size,
      int ptr);

/* ------------------------------------------------------------------------
 * Function:      osgbpb_write()
 *
 * Description:   Writes bytes to an open file. Prefer OSGBPB_WriteW
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

extern os_error *xosgbpb_write (os_f file,
      byte const *data,
      int size,
      int *unwritten);
extern int osgbpb_write (os_f file,
      byte const *data,
      int size);

/* ------------------------------------------------------------------------
 * Function:      osgbpb_writew()
 *
 * Description:   Writes bytes to an open file. Uses 32-bit file handle.
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
      int *unwritten);
extern int osgbpb_writew (os_fw file,
      byte const *data,
      int size);

/* ------------------------------------------------------------------------
 * Function:      osgbpb_read_at()
 *
 * Description:   Reads bytes from an open file at the specified file
 *                pointer. Prefer OSGBPB_ReadAtW
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

extern os_error *xosgbpb_read_at (os_f file,
      byte *buffer,
      int size,
      int ptr,
      int *unread);
extern int osgbpb_read_at (os_f file,
      byte *buffer,
      int size,
      int ptr);

/* ------------------------------------------------------------------------
 * Function:      osgbpb_read_atw()
 *
 * Description:   Reads bytes from an open file at the specified file
 *                pointer. Uses 32-bit file handle.
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

extern os_error *xosgbpb_read_atw (os_fw file,
      byte *buffer,
      int size,
      int ptr,
      int *unread);
extern int osgbpb_read_atw (os_fw file,
      byte *buffer,
      int size,
      int ptr);

/* ------------------------------------------------------------------------
 * Function:      osgbpb_read()
 *
 * Description:   Reads bytes from an open file. Prefer OSGBPB_ReadW
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

extern os_error *xosgbpb_read (os_f file,
      byte *buffer,
      int size,
      int *unread);
extern int osgbpb_read (os_f file,
      byte *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      osgbpb_readw()
 *
 * Description:   Reads bytes from an open file. Uses 32-bit file handle.
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

extern os_error *xosgbpb_readw (os_fw file,
      byte *buffer,
      int size,
      int *unread);
extern int osgbpb_readw (os_fw file,
      byte *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      osgbpb_read_disc_name()
 *
 * Description:   Reads the name and boot option of the disc which contains
 *                the CSD - prefer OSFSControl_CanonicalisePath and/or
 *                OSFSControl_ReadBootOption
 *
 * Input:         disc_name - value of R2 on entry
 *
 * Other notes:   Calls SWI 0xC with R0 = 0x5.
 */

extern os_error *xosgbpb_read_disc_name (osgbpb_disc_name *disc_name);
extern void osgbpb_read_disc_name (osgbpb_disc_name *disc_name);

/* ------------------------------------------------------------------------
 * Function:      osgbpb_read_csd_name()
 *
 * Description:   Reads the name and privilege status of the CSD - prefer
 *                OSFSControl_CanonicalisePath
 *
 * Input:         csd_name - value of R2 on entry
 *
 * Other notes:   Calls SWI 0xC with R0 = 0x6.
 */

extern os_error *xosgbpb_read_csd_name (osgbpb_dir_name *csd_name);
extern void osgbpb_read_csd_name (osgbpb_dir_name *csd_name);

/* ------------------------------------------------------------------------
 * Function:      osgbpb_read_lib_name()
 *
 * Description:   Reads the name and privilege status of the library -
 *                prefer OSFSControl_CanonicalisePath
 *
 * Input:         lib_name - value of R2 on entry
 *
 * Other notes:   Calls SWI 0xC with R0 = 0x7.
 */

extern os_error *xosgbpb_read_lib_name (osgbpb_dir_name *lib_name);
extern void osgbpb_read_lib_name (osgbpb_dir_name *lib_name);

/* ------------------------------------------------------------------------
 * Function:      osgbpb_csd_entries()
 *
 * Description:   Reads entries from the CSD - prefer OSGBPB_DirEntries
 *
 * Input:         name_list - value of R2 on entry
 *                count - value of R3 on entry
 *                context - value of R4 on entry
 *
 * Output:        unread - value of R3 on exit
 *                context_out - value of R4 on exit (X version only)
 *
 * Returns:       R4 (non-X version only)
 *
 * Other notes:   Calls SWI 0xC with R0 = 0x8.
 */

extern os_error *xosgbpb_csd_entries (osgbpb_name_list *name_list,
      int count,
      int context,
      int *unread,
      int *context_out);
extern int osgbpb_csd_entries (osgbpb_name_list *name_list,
      int count,
      int context,
      int *unread);

/* ------------------------------------------------------------------------
 * Function:      osgbpb_dir_entries()
 *
 * Description:   Reads entries from a specified directory
 *
 * Input:         dir_name - value of R1 on entry
 *                name_list - value of R2 on entry
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
 * Other notes:   Calls SWI 0xC with R0 = 0x9.
 */

extern os_error *xosgbpb_dir_entries (char const *dir_name,
      osgbpb_string_list *name_list,
      int count,
      int context,
      int size,
      char const *entries,
      int *read_count,
      int *context_out);
extern int osgbpb_dir_entries (char const *dir_name,
      osgbpb_string_list *name_list,
      int count,
      int context,
      int size,
      char const *entries,
      int *read_count);

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

extern os_error *xosgbpb_dir_entries_info (char const *dir_name,
      osgbpb_info_list *info_list,
      int count,
      int context,
      int size,
      char const *entries,
      int *read_count,
      int *context_out);
extern int osgbpb_dir_entries_info (char const *dir_name,
      osgbpb_info_list *info_list,
      int count,
      int context,
      int size,
      char const *entries,
      int *read_count);

/* ------------------------------------------------------------------------
 * Function:      osgbpb_dir_entries_system_info()
 *
 * Description:   Reads entries and file information (including SIN) from a
 *                specified directory
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
 * Other notes:   Calls SWI 0xC with R0 = 0xB.
 */

extern os_error *xosgbpb_dir_entries_system_info (char const *dir_name,
      osgbpb_system_info_list *info_list,
      int count,
      int context,
      int size,
      char const *entries,
      int *read_count,
      int *context_out);
extern int osgbpb_dir_entries_system_info (char const *dir_name,
      osgbpb_system_info_list *info_list,
      int count,
      int context,
      int size,
      char const *entries,
      int *read_count);

/* ------------------------------------------------------------------------
 * Function:      osgbpb_dir_entries_info_stamped()
 *
 * Description:   Reads entries and file information (including file type)
 *                from a specified directory
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
 * Other notes:   Calls SWI 0xC with R0 = 0xC.
 */

extern os_error *xosgbpb_dir_entries_info_stamped (char const *dir_name,
      osgbpb_info_stamped_list *info_list,
      int count,
      int context,
      int size,
      char const *entries,
      int *read_count,
      int *context_out);
extern int osgbpb_dir_entries_info_stamped (char const *dir_name,
      osgbpb_info_stamped_list *info_list,
      int count,
      int context,
      int size,
      char const *entries,
      int *read_count);

#ifdef __cplusplus
   }
#endif

#ifndef osgbpb32_H
#include "oslib/osgbpb32.h"
#endif

#else /* EXECUTE_ON_UNIX */

#define xosgbpb_write(file, data, size, un) \
   (os_error *)_swix (OS_GBPB, _IN (0) | _IN (1) | _IN (2) | _IN (3), \
         OSGBPB_Write, (os_f) (file), (byte *) (data), (int) (size))

#define xosgbpb_writew(file, data, size, un) \
   (os_error *)_swix (OS_GBPB, _IN (0) | _IN (1) | _IN (2) | _IN (3), \
         OSGBPB_Write, (os_fw) (file), (byte *) (data), (int) (size))

#define xosgbpb_write_atw(file, data, size, ptr, un) \
   (os_error *)_swix (OS_GBPB, _IN (0) | _IN (1) | _IN (2) | _IN (3) | _IN (4), \
         OSGBPB_Write, (os_fw) (file), (byte *) (data), (int) (ptr), (int) (size))

#define osgbpb_write_atw(file, data, size, ptr) \
   _swi (OS_GBPB, _IN (0) | _IN (1) | _IN (2) | _IN (3) | _IN (4) | _RETURN (3), \
         OSGBPB_Write, (os_fw) (file), (byte *) (data), (int) (ptr))

#define xosgbpb_dir_entries(dir_name, name_list, count, context, size, entries, read_count, context_out) \
   (os_error *)_swix (OS_GBPB,  _IN (0) | _IN (1) | _IN (2) | _IN (3) | _IN (4) | _IN (5) | _IN (6) | _OUT (3) | _OUT (4), \
         OSGBPB_DirEntries, (char *) (dir_name), (osgbpb_string_list *) (name_list), (int) (count), \
         (int) (context), (int) (size), (char *) (entries), (int *) (read_count), (int *) (context_out))

#define xosgbpb_dir_entriesw(dir_name, name_list, count, context, size, \
		entries, read_count, context_out) \
   (os_error *)_swix (OS_GBPB,  _IN (0) | _IN (1) | _IN (2) | _IN (3) | _IN (4) | \
   		_IN (5) | _IN (6) | _OUT (3) | _OUT (4), \
         OSGBPB_DirEntries, (char *) (dir_name), (osgbpb_string_list *) (name_list), \
         (int) (count), (int) (context), (int) (size), (char *) (entries), \
         (int *) (read_count), (int *) (context_out))

#define xosgbpb_dir_entries_info( dir_name, info_list, count, context, size, \
	entries, read_count, context_out) \
   (os_error *)_swix (OS_GBPB,  _IN (0) | _IN (1) | _IN (2) | _IN (3) | _IN (4) | \
   		_IN (5) | _IN (6) | _OUT (3) | _OUT (4), \
         OSGBPB_DirEntries, (char *) (dir_name), (osgbpb_info_list *) (info_list), \
         (int) (count), (int) (context), (int) (size), (char *) (entries), \
         (int *) (read_count), (int *) (context_out))


#define osgbpb_dir_entries_info(dir_name, info_list, count, context, size, \
      entries, read_count) \
   _swi (OS_GBPB,  _IN (0) | _IN (1) | _IN (2) | _IN (3) | _IN (4) | \
   		_IN (5) | _IN (6) | _OUT (3) | _RETURN (4), \
         OSGBPB_DirEntries, (char *) (dir_name), (osgbpb_info_list *) (info_list), \
         (int) (count), (int) (context), (int) (size), (char *) (entries), \
         (int *) (read_count))

#endif /* EXECUTE_ON_UNIX */

#endif
