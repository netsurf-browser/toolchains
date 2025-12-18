#ifndef osfile_H
#define osfile_H

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

#include <swis.h>

#ifndef types_H
   #include "types.h"
#endif

#ifndef fileswitch_H
   #include "oslib/fileswitch.h"
#endif

#define OSFile_Load               255
#define OSFile_Save                 0
#define OSFile_WriteInfo            1
#define OSFile_WriteLoad            2
#define OSFile_WriteExec            3
#define OSFile_WriteAttr            4
#define OSFile_Read                 5
#define OSFile_Delete               6
#define OSFile_Create               7
#define OSFile_CreateDir            8
#define OSFile_SetStamp             9
#define OSFile_SaveStamp           10
#define OSFile_CreateStamp         11
#define OSFile_LoadPath            12
#define OSFile_ReadPath            13
#define OSFile_LoadPathVar         14
#define OSFile_ReadPathVar         15
#define OSFile_LoadNoPath          16
#define OSFile_ReadNoPath          17
#define OSFile_SetType             18
#define OSFile_MakeError           19
#define OSFile_ReadWithType        20
#define OSFile_ReadWithTypePath    21
#define OSFile_ReadWithTypePathVar 22
#define OSFile_ReadWithTypeNoPath  23
#define OSFile_ReadBlockSize       24

#define osfile_NOT_FOUND 0
#define osfile_IS_FILE   1
#define osfile_IS_DIR    2

#define osfile_TYPE_DRAW         0xAFF
#define osfile_TYPE_BBC_ROM      0xBBC
#define osfile_TYPE_DOS_DISC     0xFC8
#define osfile_TYPE_DEVICE       0xFCC
#define osfile_TYPE_CACHE        0xFCF
#define osfile_TYPE_TASK_EXEC    0xFD6
#define osfile_TYPE_TASK_OBEY    0xFD7
#define osfile_TYPE_DOS          0xFE4
#define osfile_TYPE_DESKTOP      0xFEA
#define osfile_TYPE_OBEY         0xFEB
#define osfile_TYPE_TEMPLATE     0xFEC
#define osfile_TYPE_PALETTE      0xFED
#define osfile_TYPE_CONFIG       0xFF2
#define osfile_TYPE_PRINTOUT     0xFF4
#define osfile_TYPE_POSTSCRIPT   0xFF5
#define osfile_TYPE_FONT         0xFF6
#define osfile_TYPE_BBC_FONT     0xFF7
#define osfile_TYPE_ABSOLUTE     0xFF8
#define osfile_TYPE_SPRITE       0xFF9
#define osfile_TYPE_MODULE       0xFFA
#define osfile_TYPE_BASIC        0xFFB
#define osfile_TYPE_UTILITY      0xFFC
#define osfile_TYPE_DATA         0xFFD
#define osfile_TYPE_COMMAND      0xFFE
#define osfile_TYPE_TEXT         0xFFF

#define osfile_TYPE_DIRECTORY   0x1000
#define osfile_TYPE_APPLICATION 0x2000
#define osfile_TYPE_UNTYPED 0xFFFFFFFF

#define osfile_TYPE(load_addr) ((load_addr & 0xFFF00000) == 0xFFF00000? \
      (load_addr) >> 8 & 0xFFF: osfile_TYPE_UNTYPED)

#ifdef EXECUTE_ON_UNIX

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
      bits file_type);

extern void osfile_set_type (char const *file_name,
      bits file_type);

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

extern os_error *xosfile_create_dir(char const *, int);
extern void      osfile_create_dir(char const *, int);

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
      int size);

extern void osfile_create (char const *file_name,
      bits load_addr,
      bits exec_addr,
      int size);

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
      fileswitch_object_type *obj_type,
      bits *load_addr,
      bits *exec_addr,
      int *size,
      fileswitch_attr *attr,
      bits *file_type);

fileswitch_object_type osfile_read_stamped (char const *file_name,
      bits *load_addr,
      bits *exec_addr,
      int *size,
      fileswitch_attr *attr,
      bits *file_type);

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

os_error *xosfile_load_stamped (char const *file_name,
      byte *addr,
      fileswitch_object_type *obj_type,
      bits *load_addr,
      bits *exec_addr,
      int *size_,
      fileswitch_attr *attr);

fileswitch_object_type osfile_load_stamped (char const *file_name,
      byte *addr,
      bits *load_addr,
      bits *exec_addr,
      int *size,
      fileswitch_attr *attr);

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
      fileswitch_attr *attr);

extern fileswitch_object_type osfile_load_stamped_no_path (char const *file_name,
      byte *addr,
      bits *load_addr,
      bits *exec_addr,
      int *size,
      fileswitch_attr *attr);

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
      fileswitch_attr *attr_);

extern fileswitch_object_type osfile_delete (char const *file_name,
      bits *load_addr,
      bits *exec_addr,
      int *size,
      fileswitch_attr *attr);

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
      byte const *end);

extern void osfile_save (char const *file_name,
      bits load_addr,
      bits exec_addr,
      byte const *data,
      byte const *end);

#if 0	/* in os.h */
extern void      os_generate_error(_kernel_oserror *);
#endif

#else /* EXECUTE_ON_UNIX */

#define xosfile_load(file_name, addr, load_addr, exec_addr, size, attr) \
   (os_error *)_swix (OS_File, _IN (0) | _IN (1) | _IN (2) | _IN (3) | _OUT (2) | \
         _OUT (3) | _OUT (4) | _OUT (5), OSFile_Load, (char *) (file_name), \
         (void *) (addr), 0, (int *) (load_addr), (int *) (exec_addr), \
         (int *) (size), (int *) (attr))

#define xosfile_save(file_name, load_addr, exec_addr, start, end) \
   (os_error *)_swix (OS_File, \
         _IN (0) | _IN (1) | _IN (2) | _IN (3) | _IN (4) | _IN (5), \
         OSFile_Save, (char *) (file_name), (int) (load_addr), \
         (int) (exec_addr), (char *) (start), (char *) (end))

#define xosfile_write_info() ...
#define xosfile_write_load() ...
#define xosfile_write_exec() ...
#define xosfile_write_attr() ...

#define xosfile_read(file_name, obj_type, load_addr, exec_addr, size, \
      attr) \
   (os_error *)_swix (OS_File, _IN (0) | _IN (1) | _OUT (0) | _OUT (2) | _OUT (3) | \
         _OUT (4) | _OUT (5), OSFile_Read, (char *) (file_name), \
         (int *) (obj_type), (int *) (load_addr), (int *) (exec_addr), \
         (int *) (size), (int *) (attr))

#define xosfile_delete(file_name, obj_type, load_addr, exec_addr, size, \
      attr) \
   (os_error *)_swix (OS_File, _IN (0) | _IN (1) | _OUT (0) | _OUT (2) | _OUT (3) | \
         _OUT (4) | _OUT (5), OSFile_Delete, (char *) (file_name), \
         (int *) (obj_type), (int *) (load_addr), (int *) (exec_addr), \
         (int *) (size), (int *) (attr))

#define osfile_delete(file_name, load_addr, exec_addr, size, attr) \
   _swi (OS_File, _IN (0) | _IN (1) | _RETURN (0) | _OUT (2) | _OUT (3) | \
         _OUT (4) | _OUT (5), OSFile_Delete, (char *) (file_name), \
         (int *) (load_addr), (int *) (exec_addr), \
         (int *) (size), (int *) (attr))

#define xosfile_create() ...

#define xosfile_create_dir(dir_name, no_entries) \
   (os_error *)_swix (OS_File, _IN (0) | _IN (1) | _IN (4), \
         OSFile_CreateDir, (char *) (dir_name), (int) (no_entries))

#define osfile_create_dir(dir_name, no_entries) \
   _swi (OS_File, _IN (0) | _IN (1) | _IN (4), \
         OSFile_CreateDir, (char *) (dir_name), (int) (no_entries))

#define xosfile_set_stamp() ...

#define xosfile_save_stamped(file_name, osfile_type, start, end) \
   (os_error *)_swix (OS_File, _IN (0) | _IN (1) | _IN (2) | _IN (4) | _IN (5), \
         OSFile_SaveStamp, (char *) (file_name), (int) (osfile_type), \
         (char *) (start), (char *) (end))

#define xosfile_create_stamp(file_name, file_type, size) \
   (os_error *)_swix (OS_File, _IN (0) | _IN (1) | _IN (2) | _IN (4) | _IN (5), \
         OSFile_CreateStamp, (char *) (file_name), (int) (file_type), \
         0, (int) (size))

#define xosfile_load_path() ...
#define xosfile_read_path() ...
#define xosfile_load_path_var() ...
#define xosfile_read_path_var() ...

#define xosfile_load_no_path(file_name, addr, load_addr, exec_addr, size, \
      attr) \
   (os_error *)_swix (OS_File, _IN (0) | _IN (1) | _IN (2) | _IN (3) | _OUT (2) | \
         _OUT (3) | _OUT (4) | _OUT (5), OSFile_LoadNoPath, \
         (char *) (file_name), (void *) (addr), 0, (int *) (load_addr), \
         (int *) (exec_addr), (int *) (size), (int *) (attr))

#define xosfile_load_stamped_no_path(file_name, addr, obj_type, \
		load_addr, exec_addr, size, attr) \
   (os_error *)_swix (OS_File, _IN (0) | _IN (1) | _IN (2) | _IN (3) | _OUT (0) | _OUT (2) | \
         _OUT (3) | _OUT (4) | _OUT (5), OSFile_LoadNoPath, \
         (char *) (file_name), (void *) (addr), 0, (fileswitch_object_type *) (obj_type), \
         (int *) (load_addr), (int *) (exec_addr), (int *) (size), (int *) (attr))

#define osfile_load_stamped_no_path(file_name, addr, \
		load_addr, exec_addr, size, attr) \
   _swi (OS_File, _IN (0) | _IN (1) | _IN (2) | _IN (3) | _RETURN (0) | _OUT (2) | \
         _OUT (3) | _OUT (4) | _OUT (5), OSFile_LoadNoPath, \
         (char *) (file_name), (void *) (addr), 0, \
         (int *) (load_addr), (int *) (exec_addr), (int *) (size), (int *) (attr))

#define xosfile_read_no_path(file_name, obj_type, load_addr, exec_addr, size, \
      attr) \
   (os_error *)_swix (OS_File, _IN (0) | _IN (1) | _OUT (0) | _OUT (2) | _OUT (3) | \
         _OUT (4) | _OUT (5), OSFile_ReadNoPath, (char *) (file_name), \
         (int *) (obj_type), (int *) (load_addr), (int *) (exec_addr), \
         (int *) (size), (int *) (attr))

#define xosfile_set_type(file_name, file_type) \
   (os_error *)_swix (OS_File, _IN (0) | _IN (1) | _IN (2), \
   	OSFile_SetType, (char *) (file_name), (int) (file_type))

#define osfile_set_type(file_name, file_type) \
   _swi (OS_File, _IN (0) | _IN (1) | _IN (2), \
	OSFile_SetType, (char *) (file_name), (int) (file_type))

#define xosfile_make_error(file_name, obj_type) \
   (os_error *)_swix (OS_File, _IN (0) | _IN (1) | _IN (2), OSFile_MakeError, \
      (char *) (file_name), (int) (obj_type))

#define xosfile_read_with_type() ...
#define xosfile_read_with_type_path() ...
#define xosfile_read_with_type_path_var() ...
#define xosfile_read_with_type_no_path() ...
#define xosfile_read_block_size() ...

#endif /* EXECUTE_ON_UNIX */

#endif
