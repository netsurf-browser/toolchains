#ifndef os_H
#define os_H

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

#include <kernel.h>
#include <swis.h>

#ifndef types_H
   #include "oslib/types.h"
#endif

#if defined EXECUTE_ON_UNIX
  #define OS_ERROR(e) (os_set_lasterr((e)))
#endif

#define osbyte_IN_KEY ((byte) 129)

/*For OS_Byte 253*/
#define os_SOFT_RESET     0
#define os_POWER_ON_RESET 1
#define os_HARD_RESET     2

/*For OS_SubstituteArgs*/
#define os_NOAPPEND (1U << 31)

/*For OS_GS{Init,Read,Trans}*/
#define os_GS_TERMINATE_ON_SPACE  (1U << 29)
#define os_GS_RETAIN_STICK        (1U << 30)
#define os_GS_RETAIN_DOUBLE_QUOTE (1U << 31)

/*For OS_{Read,Set}VarVal.*/
#define os_VARTYPE_STRING   0
#define os_VARTYPE_NUMBER   1
#define os_VARTYPE_MACRO    2
#define os_VARTYPE_EXPANDED 3
#define os_VARTYPE_CODE     16

#define os_ERROR_LIMIT 252

#define os_VDU_NULL                             ((char) '\x00')
#define os_VDU_CHAR_TO_PRINTER                  ((char) '\x01')
#define os_VDU_PRINTER_ON                       ((char) '\x02')
#define os_VDU_PRINTER_OFF                      ((char) '\x03')
#define os_VDU_GRAPH_TEXT_OFF                   ((char) '\x04')
#define os_VDU_GRAPH_TEXT_ON                    ((char) '\x05')
#define os_VDU_SCREEN_ON                        ((char) '\x06')
#define os_VDU_BELL                             ((char) '\x07')
#define os_VDU_BACKSPACE                        ((char) '\x08')
#define os_VDU_TAB                              ((char) '\x09')
#define os_VDU_LINEFEED                         ((char) '\x0A')
#define os_VDU_VERTICAL_TAB                     ((char) '\x0B')
#define os_VDU_CLS                              ((char) '\x0C')
#define os_VDU_RETURN                           ((char) '\x0D')
#define os_VDU_PAGE_MODE_ON                     ((char) '\x0E')
#define os_VDU_PAGE_MODE_OFF                    ((char) '\x0F')
#define os_VDU_CLG                              ((char) '\x10')
#define os_VDU_SET_TEXT_COLOUR                  ((char) '\x11')
#define os_VDU_SET_GCOL                         ((char) '\x12')
#define os_VDU_SET_PALETTE                      ((char) '\x13')
#define os_VDU_RESET_COLOURS                    ((char) '\x14')
#define os_VDU_SCREEN_OFF                       ((char) '\x15')
#define os_VDU_MODE                             ((char) '\x16')
#define os_VDU_MISC                             ((char) '\x17')
#define os_VDU_SET_GRAPHICS_WINDOW              ((char) '\x18')
#define os_VDU_PLOT                             ((char) '\x19')
#define os_VDU_RESET_WINDOWS                    ((char) '\x1A')
#define os_VDU_ESCAPE                           ((char) '\x1B')
#define os_VDU_SET_TEXT_WINDOW                  ((char) '\x1C')
#define os_VDU_SET_GRAPHICS_ORIGIN              ((char) '\x1D')
#define os_VDU_HOME_TEXT_CURSOR                 ((char) '\x1E')
#define os_VDU_SET_TEXT_CURSOR                  ((char) '\x1F')
#define os_VDU_DELETE                           ((char) '\x7F')
#define os_MISC_INTERLACE                       ((char) '\x00')
#define os_MISC_CURSOR                          ((char) '\x01')
#define os_MISC_SET_ECF1                        ((char) '\x02')
#define os_MISC_SET_ECF2                        ((char) '\x03')
#define os_MISC_SET_ECF3                        ((char) '\x04')
#define os_MISC_SET_ECF4                        ((char) '\x05')
#define os_MISC_SET_DOT_STYLE                   ((char) '\x06')
#define os_MISC_SCROLL                          ((char) '\x07')
#define os_MISC_CLEAR_REGION                    ((char) '\x08')
#define os_MISC_SET_ON                          ((char) '\x09')
#define os_MISC_SET_OFF                         ((char) '\x0A')
#define os_MISC_SET_BBCECF                      ((char) '\x0B')
#define os_MISC_SET_UNPACKED_ECF1               ((char) '\x0C')
#define os_MISC_SET_UNPACKED_ECF2               ((char) '\x0D')
#define os_MISC_SET_UNPACKED_ECF3               ((char) '\x0E')
#define os_MISC_SET_UNPACKED_ECF4               ((char) '\x0F')
#define os_MISC_SET_CURSOR_MOVEMENT             ((char) '\x10')
#define os_MISC_MISC                            ((char) '\x11')
#define os_MISC_SET_TEXT_FG_TINT                ((char) '\x00')
#define os_MISC_SET_TEXT_BG_TINT                ((char) '\x01')
#define os_MISC_SET_GRAPHICS_FG_TINT            ((char) '\x02')
#define os_MISC_SET_GRAPHICS_BG_TINT            ((char) '\x03')
#define os_MISC_SET_ECF_INTERPRETATION          ((char) '\x04')
#define os_MISC_INVERT_TEXT                     ((char) '\x05')
#define os_MISC_SET_ECF_ORIGIN                  ((char) '\x06')
#define os_MISC_SET_CHAR_SIZE                   ((char) '\x07')

/************************************
 * Structure and union declarations *
 ************************************/
typedef struct os_mode_                         *os_mode;
      /*Mode number*/
typedef struct os_colour_pair                   os_colour_pair;
typedef struct os_palette                       os_palette;
typedef struct os_palette_base                  os_palette_base;
typedef struct os_sprite_palette                os_sprite_palette;
typedef struct os_sprite_palette_base           os_sprite_palette_base;
typedef struct os_ecf                           os_ecf;
typedef struct os_correction_table              os_correction_table;
typedef struct os_bcd_date_and_time             os_bcd_date_and_time;
typedef struct os_station_number                os_station_number;
typedef struct os_box                           os_box;
typedef struct os_factors                       os_factors;
typedef struct os_change_box                    os_change_box;
typedef struct os_error                         os_error;
typedef struct os_coord                         os_coord;
typedef struct os_trfm                          os_trfm;
typedef struct os_hom_trfm                      os_hom_trfm;
typedef struct os_pgm                           os_pgm;
typedef struct os_error_handler_buffer          os_error_handler_buffer;
typedef union  os_string_value                  os_string_value;
typedef struct os_register_block                os_register_block;
typedef struct os_vdu_var_list                  os_vdu_var_list;
typedef struct os_vdu_var_list_base             os_vdu_var_list_base;
typedef struct os_key_handler                   os_key_handler;
typedef struct os_key_handler_base              os_key_handler_base;
typedef struct os_gi                            os_gi;
typedef struct os_gs                            os_gs;
typedef struct os_gs_base                       os_gs_base;
typedef struct os_heap_sort_flags_              *os_heap_sort_flags;
typedef struct os_mem_map_request               os_mem_map_request;
typedef struct os_mem_map_request_list          os_mem_map_request_list;
typedef struct os_mem_map_request_list_base     os_mem_map_request_list_base;
typedef struct os_page_block                    os_page_block;
typedef struct pointerv_record                  pointerv_record;
typedef struct os_mode_block                    os_mode_block;
typedef struct os_mode_block_base               os_mode_block_base;
typedef struct os_mode_selector                 os_mode_selector;
typedef struct os_mode_selector_base            os_mode_selector_base;

/********************
 * Type definitions *
 ********************/
typedef int os_t;
      /*Monotonic time*/

typedef byte os_f;
      /*Deprecated 8-bit File handle. From OSLib V6.3 onwards the headers equate this name to its 32 bit equivalent, but the library retains this symbol for binary compatibility. To revert to legacy bahaviour, #define OSLIB_F8 during compilation.*/

typedef bits os_fw;
      /*Wide (32-bit) File handle*/

typedef byte os_gcol;
      /*Graphical colour, 6 or 8 bits*/

typedef byte os_tint;
      /*Graphical tint*/

typedef byte os_action;
      /*Graphical action*/

typedef bits os_colour;
      /*0xBBGGRR00 triplet, also called palette entry*/

typedef int os_colour_number;
      /*Colour number - value in screen memory*/

struct os_colour_pair
   {  os_colour on;
      os_colour off;
   };

struct os_palette
   {  os_colour entries [UNKNOWN];
   };

#define os_PALETTE(N) \
   struct \
      {  os_colour entries [N]; \
      }

#define os_SIZEOF_PALETTE(N) \
   ((N)*sizeof ((os_palette *) NULL)->entries)

struct os_sprite_palette
   {  os_colour_pair entries [UNKNOWN];
   };

#define os_SPRITE_PALETTE(N) \
   struct \
      {  os_colour_pair entries [N]; \
      }

#define os_SIZEOF_SPRITE_PALETTE(N) \
   ((N)*sizeof ((os_sprite_palette *) NULL)->entries)

struct os_ecf
   {  bits ecf [8];
   };

struct os_correction_table
   {  byte gamma [256];
   };

struct os_bcd_date_and_time
   {  byte year;
      byte month;
      byte date;
      byte weekday;
      byte hour;
      byte minute;
      byte second;
   };

struct os_station_number
   {  int station;
      int net;
   };

struct os_box
   {  int x0;
      int y0;
      int x1;
      int y1;
   };

/* ------------------------------------------------------------------------
 * Type:          os_factors
 *
 * Description:   Scaling factors for plotting
 */

struct os_factors
   {  int xmul;
      int ymul;
      int xdiv;
      int ydiv;
   };

struct os_change_box
   {  int state;
      os_box box;
   };

#if 1
struct os_error
   {  bits errnum;
      char errmess [252];
   };
#endif

typedef byte os_date_and_time [5];
      /*System date and time*/

struct os_coord
   {  int x;
      int y;
   };

/* ------------------------------------------------------------------------
 * Type:          os_trfm
 *
 * Description:   Transformation matrix
 */

struct os_trfm
   {  int entries [3] [2];
   };

/* ------------------------------------------------------------------------
 * Type:          os_hom_trfm
 *
 * Description:   Transformation matrix without translation part
 */

struct os_hom_trfm
   {  int entries [2] [2];
   };

/* ------------------------------------------------------------------------
 * Type:          os_pgm
 *
 * Description:   Parallelogram
 */

struct os_pgm
   {  os_coord coords [4];
   };

struct os_error_handler_buffer
   {  int pc;
      bits errnum;
      char errmess [248];
   };

union os_string_value
   {  int offset;
      char *pointer;
   };

typedef bits os_plot_code;

typedef int os_var_type;

typedef int os_mode_var;

typedef bits os_mode_flags;

typedef os_mode_var os_vdu_var;

typedef int os_device_type;

typedef bits osreadline_flags;

struct os_register_block
   {  int registers [16];
   };

typedef bits os_read_unsigned_flags;

typedef bits os_gs_flags;

typedef int os_dynamic_area_no;

typedef int palettev_screen_state;

struct os_vdu_var_list
   {  os_vdu_var var [UNKNOWN];
   };

#define os_VDU_VAR_LIST(N) \
   struct \
      {  os_vdu_var var [N]; \
      }

#define os_SIZEOF_VDU_VAR_LIST(N) \
   ((N)*sizeof ((os_vdu_var_list *) NULL)->var)

struct os_key_handler
   {  byte k [UNKNOWN];
   };

#define os_KEY_HANDLER(N) \
   struct \
      {  byte k [N]; \
      }

#define os_SIZEOF_KEY_HANDLER(N) \
   ((N)*sizeof ((os_key_handler *) NULL)->k)

typedef int keyv_type;

typedef int os_handler_type;

typedef bits os_substitute_args_flags;

/* ------------------------------------------------------------------------
 * Type:          os_gi
 *
 * Description:   Type to which the output from a /e directive points after OS_ReadArgs
 */

struct os_gi
   {  byte type;
      byte i [4];
   };

/* ------------------------------------------------------------------------
 * Type:          os_gs
 *
 * Description:   Type to which the output from a /g directive points after OS_ReadArgs
 */

#define os_GS_MEMBERS \
   short size;

/* Base os_gs structure without variable part */
struct os_gs_base
   {  os_GS_MEMBERS
   };

/* legacy structure */
struct os_gs
   {  os_GS_MEMBERS
      char s [UNKNOWN];
   };

#define os_GS(N) \
   struct \
      {  os_GS_MEMBERS \
         char s [N]; \
      }

#define os_SIZEOF_GS(N) \
   (offsetof (os_gs, s) + \
         (N)*sizeof ((os_gs *) NULL)->s)

typedef void *os_sort_type;

typedef bits os_area_flags;

struct os_mem_map_request
   {  int page_no;
      byte *map;
      os_area_flags access;
   };

struct os_mem_map_request_list
   {  os_mem_map_request requests [UNKNOWN];
   };

#define os_MEM_MAP_REQUEST_LIST(N) \
   struct \
      {  os_mem_map_request requests [N]; \
      }

#define os_SIZEOF_MEM_MAP_REQUEST_LIST(N) \
   ((N)*sizeof ((os_mem_map_request_list *) NULL)->requests)

struct os_page_block
   {  int page_no;
      byte *log_addr;
      byte *phys_addr;
   };

typedef int os_changed_box_state;

typedef bits os_colour_flags;

typedef int os_pointer_type;

struct pointerv_record
   {  pointerv_record *next;
      bits flags;
      os_pointer_type pointer_type;
      char name [30];
   };

typedef bits osscreenmode_cache_flags;

typedef int osscreenmode_cleaner_laziness;

typedef bits osmemory_flags;

typedef bits osmemory_type_flags;

typedef bits osmemory_controller_flags;

typedef bits os_platform_feature_flags;

typedef bits os_synchronise_code_flags;

typedef bits os_hardware_flags;

#define os_MODE_BLOCK_MEMBERS \
   int size; \
   bits flags; \
   int xres; \
   int yres; \
   int log2_bpp; \
   int frame_rate;

/* Base os_mode_block structure without variable part */
struct os_mode_block_base
   {  os_MODE_BLOCK_MEMBERS
   };

/* legacy structure */
struct os_mode_block
   {  os_MODE_BLOCK_MEMBERS
      char name [UNKNOWN];
   };

#define os_MODE_BLOCK(N) \
   struct \
      {  os_MODE_BLOCK_MEMBERS \
         char name [N]; \
      }

#define os_SIZEOF_MODE_BLOCK(N) \
   (offsetof (os_mode_block, name) + \
         (N)*sizeof ((os_mode_block *) NULL)->name)

#define os_MODE_SELECTOR_MEMBERS \
   bits flags; \
   int xres; \
   int yres; \
   int log2_bpp; \
   int frame_rate;

/* Base os_mode_selector structure without variable part */
struct os_mode_selector_base
   {  os_MODE_SELECTOR_MEMBERS
   };

/* legacy structure */
struct os_mode_selector
   {  os_MODE_SELECTOR_MEMBERS
      struct \
   {  os_mode_var var; \
      int val; \
   } \
   modevars [UNKNOWN];
   };

#define os_MODE_SELECTOR(N) \
   struct \
      {  os_MODE_SELECTOR_MEMBERS \
         struct \
   {  os_mode_var var; \
      int val; \
   } \
   modevars [N]; \
      }

#define os_SIZEOF_MODE_SELECTOR(N) \
   (offsetof (os_mode_selector, modevars) + \
         (N)*sizeof ((os_mode_selector *) NULL)->modevars)


#define os_GLOBAL_NO_ANY 9
#define os_GLOBAL_NO_MEM 49

#ifdef EXECUTE_ON_UNIX

/*************************
 * Function declarations *
 *************************/

#ifdef __cplusplus
   extern "C" {
#endif

/* ------------------------------------------------------------------------
 * Function:      os_read_var_val_size()
 *
 * Description:   Gets size of system variable or checks for its existance
 *                (PRM 1-309, 5a-661)
 *
 * Input:         var - value of R0 on entry
 *                context - value of R3 on entry
 *                var_type - value of R4 on entry
 *
 * Output:        used - 0 if not found or, if var_type_out != 3, then NOT the number of bytes required for the variable
 *                context_out - value of R3 on exit (X version only)
 *                var_type_out - value of R4 on exit
 *
 * Returns:       R3 (non-X version only)
 *
 * Other notes:   Calls SWI 0x23 with R1 = 0x0, R2 = 0x80000000.
 */

extern os_error *xos_read_var_val_size (char const *var,
      int context,
      os_var_type var_type,
      int *used,
      int *context_out,
      os_var_type *var_type_out);
extern int os_read_var_val_size (char const *var,
      int context,
      os_var_type var_type,
      int *used,
      os_var_type *var_type_out);

/* ------------------------------------------------------------------------
 * Function:      os_change_dynamic_area_no_fail()
 *
 * Description:   Alters the space allocation of a dynamic area without
 *                throwing an error if the change was only partially
 *                performed
 *
 * Input:         area_type - value of R0 on entry
 *                change - Increase required (signed integer)
 *
 * Output:        change_out - Amount changed (unsigned integer) (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x2A.
 */

extern os_error *xos_change_dynamic_area_no_fail (os_dynamic_area_no area_type,
      int change,
      int *change_out);
extern int os_change_dynamic_area_no_fail (os_dynamic_area_no area_type,
      int change);

/* ------------------------------------------------------------------------
 * Function:      os_writec()
 *
 * Description:   Writes a character to all of the active output streams
 *
 * Input:         c - value of R0 on entry
 *
 * Other notes:   Calls SWI 0x0.
 */

extern os_error *xos_writec (char c);
__swi (0x0) void os_writec (char c);

/* ------------------------------------------------------------------------
 * Function:      wrchv()
 *
 * Description:   OS_WriteC vector
 *
 * Input:         c - value of R0 on entry
 *
 * Other notes:   Calls SWI 0x34 with R9 = 0x3.
 */

extern os_error *xwrchv (char c);
extern void wrchv (char c);

/* ------------------------------------------------------------------------
 * Function:      os_write0()
 *
 * Description:   Writes a string to all of the active output streams
 *
 * Input:         s - value of R0 on entry
 *
 * Other notes:   Calls SWI 0x2.
 */

extern os_error *xos_write0 (char const *s);
__swi (0x2) void os_write0 (char const *s);

/* ------------------------------------------------------------------------
 * Function:      os_new_line()
 *
 * Description:   Writes a line feed followed by a carriage return to all
 *                of the active output streams
 *
 * Other notes:   Calls SWI 0x3.
 */

extern os_error *xos_new_line (void);
__swi (0x3) void os_new_line (void);

/* ------------------------------------------------------------------------
 * Function:      os_readc()
 *
 * Description:   Reads a character from the input stream
 *
 * Output:        c - value of R0 on exit
 *                psr - processor status register on exit (X version only)
 *
 * Returns:       psr (non-X version only)
 *
 * Other notes:   Calls SWI 0x4.
 */

extern os_error *xos_readc (char *c,
      bits *psr);
extern bits os_readc (char *c);

/* ------------------------------------------------------------------------
 * Function:      rdchv()
 *
 * Description:   OS_ReadC vector
 *
 * Output:        c - value of R0 on exit
 *                psr - processor status register on exit (X version only)
 *
 * Returns:       psr (non-X version only)
 *
 * Other notes:   Calls SWI 0x34 with R9 = 0x4.
 */

extern os_error *xrdchv (char *c,
      bits *psr);
extern bits rdchv (char *c);

/* ------------------------------------------------------------------------
 * Function:      os_cli()
 *
 * Description:   Processes a supervisor command
 *
 * Input:         command - value of R0 on entry
 *
 * Other notes:   Calls SWI 0x5.
 */

extern os_error *xos_cli (char const *command);
__swi (0x5) void os_cli (char const *command);

/* ------------------------------------------------------------------------
 * Function:      cliv()
 *
 * Description:   OS_CLI vector
 *
 * Input:         command - value of R0 on entry
 *
 * Other notes:   Calls SWI 0x34 with R9 = 0x5.
 */

extern os_error *xcliv (char const *command);
extern void cliv (char const *command);

/* ------------------------------------------------------------------------
 * Function:      os_read_line()
 *
 * Description:   Reads a line from the input stream - prefer OS_ReadLine32
 *                on RISC OS 5+
 *
 * Input:         buffer - value of R0 on entry
 *                size - value of R1 on entry
 *                min_char - value of R2 on entry
 *                max_char - value of R3 on entry
 *
 * Output:        used - value of R1 on exit
 *                psr - processor status register on exit (X version only)
 *
 * Returns:       psr (non-X version only)
 *
 * Other notes:   Calls SWI 0xE.
 */

extern os_error *xos_read_line (char *buffer,
      int size,
      char min_char,
      char max_char,
      int *used,
      bits *psr);
extern bits os_read_line (char *buffer,
      int size,
      char min_char,
      char max_char,
      int *used);

/* ------------------------------------------------------------------------
 * Function:      os_read_line_given_echo()
 *
 * Description:   Reads a line from the input stream, using the given echo
 *                character - use OS_ReadLine32 on RISC OS 5+
 *
 * Input:         buffer - value of R0 on entry
 *                size - value of R1 on entry
 *                min_char - value of R2 on entry
 *                max_char - value of R3 on entry
 *                echo - value of R4 on entry
 *
 * Output:        used - value of R1 on exit
 *                psr - processor status register on exit (X version only)
 *
 * Returns:       psr (non-X version only)
 *
 * Other notes:   Calls SWI 0xE with R0 |= 0x40000000.
 */

extern os_error *xos_read_line_given_echo (char *buffer,
      int size,
      char min_char,
      char max_char,
      char echo,
      int *used,
      bits *psr);
extern bits os_read_line_given_echo (char *buffer,
      int size,
      char min_char,
      char max_char,
      char echo,
      int *used);

/* ------------------------------------------------------------------------
 * Function:      os_read_line_suppress_invalid()
 *
 * Description:   Reads a line from the input stream, not echoing invalid
 *                characters - use OS_ReadLine32 on RISC OS 5+
 *
 * Input:         buffer - value of R0 on entry
 *                size - value of R1 on entry
 *                min_char - value of R2 on entry
 *                max_char - value of R3 on entry
 *
 * Output:        used - value of R1 on exit
 *                psr - processor status register on exit (X version only)
 *
 * Returns:       psr (non-X version only)
 *
 * Other notes:   Calls SWI 0xE with R0 |= 0x80000000.
 */

extern os_error *xos_read_line_suppress_invalid (char *buffer,
      int size,
      char min_char,
      char max_char,
      int *used,
      bits *psr);
extern bits os_read_line_suppress_invalid (char *buffer,
      int size,
      char min_char,
      char max_char,
      int *used);

/* ------------------------------------------------------------------------
 * Function:      os_read_line_given_echo_suppress_invalid()
 *
 * Description:   Reads a line from the input stream, using the given echo
 *                character and not echoing invalid characters - use
 *                OS_ReadLine3 on RISC OS 5+
 *
 * Input:         buffer - value of R0 on entry
 *                size - value of R1 on entry
 *                min_char - value of R2 on entry
 *                max_char - value of R3 on entry
 *                echo - value of R4 on entry
 *
 * Output:        used - value of R1 on exit
 *                psr - processor status register on exit (X version only)
 *
 * Returns:       psr (non-X version only)
 *
 * Other notes:   Calls SWI 0xE with R0 |= 0xC0000000.
 */

extern os_error *xos_read_line_given_echo_suppress_invalid (char *buffer,
      int size,
      char min_char,
      char max_char,
      char echo,
      int *used,
      bits *psr);
extern bits os_read_line_given_echo_suppress_invalid (char *buffer,
      int size,
      char min_char,
      char max_char,
      char echo,
      int *used);

/* ------------------------------------------------------------------------
 * Function:      os_read_line32()
 *
 * Description:   Reads a line from the input stream - RISC OS 5+
 *
 * Input:         buffer - value of R0 on entry
 *                size - value of R1 on entry
 *                min_char - value of R2 on entry
 *                max_char - value of R3 on entry
 *                flags - value of R4 on entry
 *
 * Output:        used - value of R1 on exit
 *                psr - processor status register on exit (X version only)
 *
 * Returns:       psr (non-X version only)
 *
 * Other notes:   Calls SWI 0x7D.
 */

extern os_error *xos_read_line32 (char *buffer,
      int size,
      char min_char,
      char max_char,
      osreadline_flags flags,
      int *used,
      bits *psr);
extern bits os_read_line32 (char *buffer,
      int size,
      char min_char,
      char max_char,
      osreadline_flags flags,
      int *used);

/* ------------------------------------------------------------------------
 * Function:      readlinev()
 *
 * Description:   OS_ReadLine vector - use ReadLine32V on RISC OS 5+
 *
 * Input:         buffer - value of R0 on entry
 *                size - value of R1 on entry
 *                min_char - value of R2 on entry
 *                max_char - value of R3 on entry
 *                echo - value of R4 on entry
 *
 * Output:        used - value of R1 on exit
 *                psr - processor status register on exit (X version only)
 *
 * Returns:       psr (non-X version only)
 *
 * Other notes:   Calls SWI 0x34 with R9 = 0xE.
 */

extern os_error *xreadlinev (char *buffer,
      int size,
      char min_char,
      char max_char,
      char echo,
      int *used,
      bits *psr);
extern bits readlinev (char *buffer,
      int size,
      char min_char,
      char max_char,
      char echo,
      int *used);

/* ------------------------------------------------------------------------
 * Function:      readline32v()
 *
 * Description:   OS_ReadLine vector - RISC OS 5+
 *
 * Input:         buffer - value of R0 on entry
 *                size - value of R1 on entry
 *                min_char - value of R2 on entry
 *                max_char - value of R3 on entry
 *                flags - value of R4 on entry
 *
 * Output:        used - value of R1 on exit
 *                psr - processor status register on exit (X version only)
 *
 * Returns:       psr (non-X version only)
 *
 * Other notes:   Calls SWI 0x34 with R9 = 0xE.
 */

extern os_error *xreadline32v (char *buffer,
      int size,
      char min_char,
      char max_char,
      osreadline_flags flags,
      int *used,
      bits *psr);
extern bits readline32v (char *buffer,
      int size,
      char min_char,
      char max_char,
      osreadline_flags flags,
      int *used);

/* ------------------------------------------------------------------------
 * Function:      os_control()
 *
 * Description:   Reads/writes handler addresses - prefer
 *                OS_ChangeEnvironment
 *
 * Input:         error_handler - value of R0 on entry
 *                error_buffer - value of R1 on entry
 *                escape_handler - value of R2 on entry
 *                event_handler - value of R3 on entry
 *
 * Output:        old_error_handler - value of R0 on exit
 *                old_error_buffer - value of R1 on exit
 *                old_escape_handler - value of R2 on exit
 *                old_event_handler - value of R3 on exit
 *
 * Other notes:   Calls SWI 0xF.
 */

extern os_error *xos_control (void const *error_handler,
      os_error *error_buffer,
      void const *escape_handler,
      void const *event_handler,
      void **old_error_handler,
      os_error **old_error_buffer,
      void **old_escape_handler,
      void **old_event_handler);
extern void os_control (void const *error_handler,
      os_error *error_buffer,
      void const *escape_handler,
      void const *event_handler,
      void **old_error_handler,
      os_error **old_error_buffer,
      void **old_escape_handler,
      void **old_event_handler);

/* ------------------------------------------------------------------------
 * Function:      os_get_env()
 *
 * Description:   Reads environment parameters
 *
 * Output:        command - value of R0 on exit (X version only)
 *                ram_limit - value of R1 on exit
 *                start - value of R2 on exit
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x10.
 */

extern os_error *xos_get_env (char **command,
      byte **ram_limit,
      os_date_and_time **start);
extern char *os_get_env (byte **ram_limit,
      os_date_and_time **start);

/* ------------------------------------------------------------------------
 * Function:      os_exit()
 *
 * Description:   Passes control to the most recent exit handler
 *
 * Input:         error - value of R0 on entry
 *                rc - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x11 with R1 = 0x58454241.
 */

extern os_error *xos_exit (os_error const *error,
      int rc);
extern void os_exit (os_error const *error,
      int rc);

/* ------------------------------------------------------------------------
 * Function:      os_set_env()
 *
 * Description:   Sets environment parameters - prefer OS_ChangeEnvironment
 *
 * Input:         exit_handler - value of R0 on entry
 *                ram_limit - value of R1 on entry
 *                undefined_instruction_handler - value of R4 on entry
 *                prefetch_abort_handler - value of R5 on entry
 *                data_abort_handler - value of R6 on entry
 *                address_exception_handler - value of R7 on entry
 *
 * Output:        old_exit_handler - value of R0 on exit
 *                old_ram_limit - value of R1 on exit
 *                old_undefined_instruction_handler - value of R4 on exit
 *                old_prefetch_abort_handler - value of R5 on exit
 *                old_data_abort_handler - value of R6 on exit
 *                old_address_exception_handler - value of R7 on exit
 *
 * Other notes:   Calls SWI 0x12.
 */

extern os_error *xos_set_env (void const *exit_handler,
      byte *ram_limit,
      void const *undefined_instruction_handler,
      void const *prefetch_abort_handler,
      void const *data_abort_handler,
      void const *address_exception_handler,
      void **old_exit_handler,
      byte **old_ram_limit,
      void **old_undefined_instruction_handler,
      void **old_prefetch_abort_handler,
      void **old_data_abort_handler,
      void **old_address_exception_handler);
extern void os_set_env (void const *exit_handler,
      byte *ram_limit,
      void const *undefined_instruction_handler,
      void const *prefetch_abort_handler,
      void const *data_abort_handler,
      void const *address_exception_handler,
      void **old_exit_handler,
      byte **old_ram_limit,
      void **old_undefined_instruction_handler,
      void **old_prefetch_abort_handler,
      void **old_data_abort_handler,
      void **old_address_exception_handler);

/* ------------------------------------------------------------------------
 * Function:      os_int_on()
 *
 * Description:   Enables interrupts - cannot be called via APCS26
 *
 * Other notes:   Calls SWI 0x13.
 */

extern os_error *xos_int_on (void);
__swi (0x13) void os_int_on (void);

/* ------------------------------------------------------------------------
 * Function:      os_int_off()
 *
 * Description:   Disables interrupts - cannot be called via APCS26
 *
 * Other notes:   Calls SWI 0x14.
 */

extern os_error *xos_int_off (void);
__swi (0x14) void os_int_off (void);

/* ------------------------------------------------------------------------
 * Function:      os_call_back()
 *
 * Description:   Sets up the callback handler - prefer
 *                OS_ChangeEnvironment
 *
 * Input:         register_block - value of R0 on entry
 *                call_back_handler - value of R1 on entry
 *
 * Output:        old_register_block - value of R0 on exit
 *                old_call_back_handler - value of R1 on exit
 *
 * Other notes:   Calls SWI 0x15.
 */

extern os_error *xos_call_back (os_register_block const *register_block,
      void const *call_back_handler,
      os_register_block **old_register_block,
      void **old_call_back_handler);
extern void os_call_back (os_register_block const *register_block,
      void const *call_back_handler,
      os_register_block **old_register_block,
      void **old_call_back_handler);

/* ------------------------------------------------------------------------
 * Function:      os_break_pt()
 *
 * Description:   Causes a break point trap to occur and the break point
 *                handler to be entered
 *
 * Other notes:   Calls SWI 0x17.
 */

extern os_error *xos_break_pt (void);
__swi (0x17) void os_break_pt (void);

/* ------------------------------------------------------------------------
 * Function:      os_break_ctrl()
 *
 * Description:   Sets up the break point handler - prefer
 *                OS_ChangeEnvironment
 *
 * Input:         register_block - value of R0 on entry
 *                break_pt_handler - value of R1 on entry
 *
 * Output:        old_register_block - value of R0 on exit
 *                old_break_pt_handler - value of R1 on exit
 *
 * Other notes:   Calls SWI 0x18.
 */

extern os_error *xos_break_ctrl (os_register_block *register_block,
      void const *break_pt_handler,
      os_register_block **old_register_block,
      void **old_break_pt_handler);
extern void os_break_ctrl (os_register_block *register_block,
      void const *break_pt_handler,
      os_register_block **old_register_block,
      void **old_break_pt_handler);

/* ------------------------------------------------------------------------
 * Function:      os_unused_swi()
 *
 * Description:   Sets up the handler for unused SWI's - prefer
 *                OS_ChangeEnvironment
 *
 * Input:         unused_swi_handler - value of R0 on entry
 *
 * Output:        old_unused_swi_handler - value of R0 on exit (X version only)
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x19.
 */

extern os_error *xos_unused_swi (void const *unused_swi_handler,
      void **old_unused_swi_handler);
__swi (0x19) void *os_unused_swi (void const *unused_swi_handler);

/* ------------------------------------------------------------------------
 * Function:      os_update_memc()
 *
 * Description:   Reads or alters the contents of the MEMC control register
 *
 * Input:         value - value of R0 on entry
 *                mask - value of R1 on entry
 *
 * Output:        old_value - value of R0 on exit (X version only)
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x1A.
 */

extern os_error *xos_update_memc (bits value,
      bits mask,
      bits *old_value);
__swi (0x1A) bits os_update_memc (bits value,
      bits mask);

/* ------------------------------------------------------------------------
 * Function:      os_set_call_back()
 *
 * Description:   Causes a call to the callback handler
 *
 * Other notes:   Calls SWI 0x1B.
 */

extern os_error *xos_set_call_back (void);
__swi (0x1B) void os_set_call_back (void);

/* ------------------------------------------------------------------------
 * Function:      os_mouse()
 *
 * Description:   Reads a mouse state from the buffer
 *
 * Output:        x - value of R0 on exit
 *                y - value of R1 on exit
 *                buttons - value of R2 on exit
 *                t - value of R3 on exit
 *
 * Other notes:   Calls SWI 0x1C.
 */

extern os_error *xos_mouse (int *x,
      int *y,
      bits *buttons,
      os_t *t);
extern void os_mouse (int *x,
      int *y,
      bits *buttons,
      os_t *t);

/* ------------------------------------------------------------------------
 * Function:      mousev()
 *
 * Description:   OS_Mouse vector
 *
 * Output:        x - value of R0 on exit
 *                y - value of R1 on exit
 *                buttons - value of R2 on exit
 *                t - value of R3 on exit
 *
 * Other notes:   Calls SWI 0x34 with R9 = 0x1A.
 */

extern os_error *xmousev (int *x,
      int *y,
      bits *buttons,
      os_t *t);
extern void mousev (int *x,
      int *y,
      bits *buttons,
      os_t *t);

/* ------------------------------------------------------------------------
 * Function:      os_claim()
 *
 * Description:   Adds a routine to the list of those that claim a vector
 *
 * Input:         vector - value of R0 on entry
 *                routine - value of R1 on entry
 *                handle - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x1F.
 */

extern os_error *xos_claim (int vector,
      void const *routine,
      byte *handle);
__swi (0x1F) void os_claim (int vector,
      void const *routine,
      byte *handle);

/* ------------------------------------------------------------------------
 * Function:      os_release()
 *
 * Description:   Removes a routine from the list of those that claim a
 *                vector
 *
 * Input:         vector - value of R0 on entry
 *                routine - value of R1 on entry
 *                handle - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x20.
 */

extern os_error *xos_release (int vector,
      void const *routine,
      byte *handle);
__swi (0x20) void os_release (int vector,
      void const *routine,
      byte *handle);

/* ------------------------------------------------------------------------
 * Function:      os_read_unsigned()
 *
 * Description:   Converts a string to an unsigned number
 *
 * Input:         flags - value of R0 on entry
 *                number - value of R1 on entry
 *                limit - value of R2 on entry
 *
 * Output:        end - value of R1 on exit
 *                value - value of R2 on exit (X version only)
 *
 * Returns:       R2 (non-X version only)
 *
 * Other notes:   Calls SWI 0x21.
 */

extern os_error *xos_read_unsigned (os_read_unsigned_flags flags,
      char const *number,
      bits limit,
      char **end,
      bits *value);
extern bits os_read_unsigned (os_read_unsigned_flags flags,
      char const *number,
      bits limit,
      char **end);

/* ------------------------------------------------------------------------
 * Function:      os_read_var_val()
 *
 * Description:   Reads a variable value (PRM 1-309, 5a-661) - Prefer
 *                OS_ReadVarValSize to read size of variable
 *
 * Input:         var - value of R0 on entry
 *                value - value of R1 on entry
 *                size - value of R2 on entry
 *                context - value of R3 on entry
 *                var_type - value of R4 on entry
 *
 * Output:        used - value of R2 on exit
 *                context_out - value of R3 on exit (X version only)
 *                var_type_out - value of R4 on exit
 *
 * Returns:       R3 (non-X version only)
 *
 * Other notes:   Calls SWI 0x23.
 */

extern os_error *xos_read_var_val (char const *var,
      char *value,
      int size,
      int context,
      os_var_type var_type,
      int *used,
      int *context_out,
      os_var_type *var_type_out);
extern int os_read_var_val (char const *var,
      char *value,
      int size,
      int context,
      os_var_type var_type,
      int *used,
      os_var_type *var_type_out);

/* ------------------------------------------------------------------------
 * Function:      os_set_var_val()
 *
 * Description:   Writes a variable value
 *
 * Input:         var - value of R0 on entry
 *                value - value of R1 on entry
 *                size - value of R2 on entry
 *                context - value of R3 on entry
 *                var_type - value of R4 on entry
 *
 * Output:        context_out - value of R3 on exit (X version only)
 *                var_type_out - value of R4 on exit
 *
 * Returns:       R3 (non-X version only)
 *
 * Other notes:   Calls SWI 0x24.
 */

extern os_error *xos_set_var_val (char const *var,
      byte const *value,
      int size,
      int context,
      os_var_type var_type,
      int *context_out,
      os_var_type *var_type_out);
extern int os_set_var_val (char const *var,
      byte const *value,
      int size,
      int context,
      os_var_type var_type,
      os_var_type *var_type_out);

/* ------------------------------------------------------------------------
 * Function:      os_gs_init()
 *
 * Description:   Initialises context for use by OS_GSRead
 *
 * Input:         s - value of R0 on entry
 *                flags - value of R2 on entry
 *
 * Output:        context1 - value of R0 on exit
 *                context2 - value of R2 on exit
 *
 * Other notes:   Calls SWI 0x25.
 */

extern os_error *xos_gs_init (char const *s,
      os_gs_flags flags,
      int *context1,
      int *context2);
extern void os_gs_init (char const *s,
      os_gs_flags flags,
      int *context1,
      int *context2);

/* ------------------------------------------------------------------------
 * Function:      os_gs_read()
 *
 * Description:   Returns a character from a string which has been
 *                initialised by OS_GSInit
 *
 * Input:         context1 - value of R0 on entry
 *                context2 - value of R2 on entry
 *
 * Output:        context1_out - value of R0 on exit
 *                c - value of R1 on exit
 *                context2_out - value of R2 on exit
 *                psr - processor status register on exit (X version only)
 *
 * Returns:       psr (non-X version only)
 *
 * Other notes:   Calls SWI 0x26.
 */

extern os_error *xos_gs_read (int context1,
      int context2,
      int *context1_out,
      char *c,
      int *context2_out,
      bits *psr);
extern bits os_gs_read (int context1,
      int context2,
      int *context1_out,
      char *c,
      int *context2_out);

/* ------------------------------------------------------------------------
 * Function:      os_gs_trans()
 *
 * Description:   Equivalent to a call to OS_GSInit and repeated calls to
 *                OS_GSRead
 *
 * Input:         s - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        used - value of R2 on exit
 *                psr - processor status register on exit (X version only)
 *
 * Returns:       psr (non-X version only)
 *
 * Other notes:   Calls SWI 0x27.
 */

extern os_error *xos_gs_trans (char const *s,
      char *buffer,
      int size,
      int *used,
      bits *psr);
extern bits os_gs_trans (char const *s,
      char *buffer,
      int size,
      int *used);

/* ------------------------------------------------------------------------
 * Function:      os_binary_to_decimal()
 *
 * Description:   Converts a signed number to a string
 *
 * Input:         i - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        used - value of R2 on exit (X version only)
 *
 * Returns:       R2 (non-X version only)
 *
 * Other notes:   Calls SWI 0x28.
 */

extern os_error *xos_binary_to_decimal (int i,
      char *buffer,
      int size,
      int *used);
extern int os_binary_to_decimal (int i,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_change_dynamic_area()
 *
 * Description:   Alters the space allocation of a dynamic area
 *
 * Input:         area_type - value of R0 on entry
 *                change - value of R1 on entry
 *
 * Output:        change_out - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x2A.
 */

extern os_error *xos_change_dynamic_area (os_dynamic_area_no area_type,
      int change,
      int *change_out);
extern int os_change_dynamic_area (os_dynamic_area_no area_type,
      int change);

/* ------------------------------------------------------------------------
 * Function:      os_generate_error()
 *
 * Description:   Generates an error and invokes the error handler
 *
 * Input:         error - value of R0 on entry
 *
 * Other notes:   Calls SWI 0x2B.
 */

extern os_error *xos_generate_error (os_error const *error);
__swi (0x2B) void os_generate_error (os_error const *error);

/* ------------------------------------------------------------------------
 * Function:      errorv()
 *
 * Description:   OS_GenerateError vector
 *
 * Other notes:   Calls SWI 0x34 with R9 = 0x1.
 */

extern os_error *xerrorv (void);
extern void errorv (void);

/* ------------------------------------------------------------------------
 * Function:      os_read_escape_state()
 *
 * Description:   Checks whether an escape condition has occurred
 *
 * Output:        psr - processor status register on exit (X version only)
 *
 * Returns:       psr (non-X version only)
 *
 * Other notes:   Calls SWI 0x2C.
 */

extern os_error *xos_read_escape_state (bits *psr);
extern bits os_read_escape_state (void);

/* ------------------------------------------------------------------------
 * Function:      os_evaluate_expression()
 *
 * Description:   Evaluates a string expression and returns an integer or
 *                string result
 *
 * Input:         expr - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        expr_is_str - value of R1 on exit (X version only)
 *                int_value_or_str_used - value of R2 on exit
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x2D.
 */

extern os_error *xos_evaluate_expression (char const *expr,
      char *buffer,
      int size,
      osbool *expr_is_str,
      int *int_value_or_str_used);
extern osbool os_evaluate_expression (char const *expr,
      char *buffer,
      int size,
      int *int_value_or_str_used);

/* ------------------------------------------------------------------------
 * Function:      os_read_palette()
 *
 * Description:   Reads the palette setting of a colour
 *
 * Input:         entry - value of R0 on entry
 *                colour_type - value of R1 on entry
 *
 * Output:        on - value of R2 on exit
 *                off - value of R3 on exit
 *
 * Other notes:   Calls SWI 0x2F.
 */

extern os_error *xos_read_palette (os_colour_number entry,
      int colour_type,
      os_colour *on,
      os_colour *off);
extern void os_read_palette (os_colour_number entry,
      int colour_type,
      os_colour *on,
      os_colour *off);

/* ------------------------------------------------------------------------
 * Function:      palettev_read_entry()
 *
 * Description:   Reads the palette
 *
 * Input:         entry - value of R0 on entry
 *                colour_type - value of R1 on entry
 *
 * Output:        on - value of R2 on exit
 *                off - value of R3 on exit
 *                incomplete - value of R4 on exit (X version only)
 *
 * Returns:       R4 (non-X version only)
 *
 * Other notes:   Calls SWI 0x34 with R4 = 0x1, R9 = 0x23.
 */

extern os_error *xpalettev_read_entry (os_colour_number entry,
      int colour_type,
      os_colour *on,
      os_colour *off,
      osbool *incomplete);
extern osbool palettev_read_entry (os_colour_number entry,
      int colour_type,
      os_colour *on,
      os_colour *off);

/* ------------------------------------------------------------------------
 * Function:      palettev_set_entry()
 *
 * Description:   Sets the palette
 *
 * Input:         entry - value of R0 on entry
 *                colour_type - value of R1 on entry
 *                on - value of R2 on entry
 *                off - value of R3 on entry
 *
 * Output:        incomplete - value of R4 on exit (X version only)
 *
 * Returns:       R4 (non-X version only)
 *
 * Other notes:   Calls SWI 0x34 with R4 = 0x2, R9 = 0x23.
 */

extern os_error *xpalettev_set_entry (os_colour_number entry,
      int colour_type,
      os_colour on,
      os_colour off,
      osbool *incomplete);
extern osbool palettev_set_entry (os_colour_number entry,
      int colour_type,
      os_colour on,
      os_colour off);

/* ------------------------------------------------------------------------
 * Function:      palettev_set_on()
 *
 * Description:   Sets the flash state to on
 *
 * Output:        incomplete - value of R4 on exit (X version only)
 *
 * Returns:       R4 (non-X version only)
 *
 * Other notes:   Calls SWI 0x34 with R4 = 0x3, R9 = 0x23.
 */

extern os_error *xpalettev_set_on (osbool *incomplete);
extern osbool palettev_set_on (void);

/* ------------------------------------------------------------------------
 * Function:      palettev_set_off()
 *
 * Description:   Sets the flash state to off
 *
 * Output:        incomplete - value of R4 on exit (X version only)
 *
 * Returns:       R4 (non-X version only)
 *
 * Other notes:   Calls SWI 0x34 with R4 = 0x4, R9 = 0x23.
 */

extern os_error *xpalettev_set_off (osbool *incomplete);
extern osbool palettev_set_off (void);

/* ------------------------------------------------------------------------
 * Function:      palettev_default()
 *
 * Description:   Sets the palette to the default
 *
 * Output:        incomplete - value of R4 on exit (X version only)
 *
 * Returns:       R4 (non-X version only)
 *
 * Other notes:   Calls SWI 0x34 with R4 = 0x5, R9 = 0x23.
 */

extern os_error *xpalettev_default (osbool *incomplete);
extern osbool palettev_default (void);

/* ------------------------------------------------------------------------
 * Function:      palettev_blank_screen()
 *
 * Description:   Controls screenblanking
 *
 * Input:         state - value of R0 on entry
 *
 * Output:        state_out - value of R0 on exit
 *                incomplete - value of R4 on exit (X version only)
 *
 * Returns:       R4 (non-X version only)
 *
 * Other notes:   Calls SWI 0x34 with R4 = 0x6, R9 = 0x23.
 */

extern os_error *xpalettev_blank_screen (palettev_screen_state state,
      palettev_screen_state *state_out,
      osbool *incomplete);
extern osbool palettev_blank_screen (palettev_screen_state state,
      palettev_screen_state *state_out);

/* ------------------------------------------------------------------------
 * Function:      palettev_read_entries()
 *
 * Description:   Reads palette entries
 *
 * Input:         entry_list - value of R0 on entry
 *                colour_type_and_entry_count - value of R1 on entry
 *                on - value of R2 on entry
 *                off - value of R3 on entry
 *
 * Output:        incomplete - value of R4 on exit (X version only)
 *
 * Returns:       R4 (non-X version only)
 *
 * Other notes:   Calls SWI 0x34 with R4 = 0x7, R9 = 0x23.
 */

extern os_error *xpalettev_read_entries (os_colour_number const *entry_list,
      bits colour_type_and_entry_count,
      os_palette *on,
      os_palette *off,
      osbool *incomplete);
extern osbool palettev_read_entries (os_colour_number const *entry_list,
      bits colour_type_and_entry_count,
      os_palette *on,
      os_palette *off);

/* ------------------------------------------------------------------------
 * Function:      palettev_write_entries()
 *
 * Description:   Writes palette entries
 *
 * Input:         entry_list - value of R0 on entry
 *                colour_type_and_entry_count - value of R1 on entry
 *                palette - value of R2 on entry
 *
 * Output:        incomplete - value of R4 on exit (X version only)
 *
 * Returns:       R4 (non-X version only)
 *
 * Other notes:   Calls SWI 0x34 with R4 = 0x8, R9 = 0x23.
 */

extern os_error *xpalettev_write_entries (os_colour_number const *entry_list,
      bits colour_type_and_entry_count,
      os_palette const *palette,
      osbool *incomplete);
extern osbool palettev_write_entries (os_colour_number const *entry_list,
      bits colour_type_and_entry_count,
      os_palette const *palette);

/* ------------------------------------------------------------------------
 * Function:      palettev_set_gamma_corrections()
 *
 * Description:   Sets the gamma correction tables
 *
 * Input:         red_table - value of R0 on entry
 *                green_table - value of R1 on entry
 *                blue_table - value of R2 on entry
 *
 * Output:        incomplete - value of R4 on exit (X version only)
 *
 * Returns:       R4 (non-X version only)
 *
 * Other notes:   Calls SWI 0x34 with R4 = 0x9, R9 = 0x23.
 */

extern os_error *xpalettev_set_gamma_corrections (os_correction_table const *red_table,
      os_correction_table const *green_table,
      os_correction_table const *blue_table,
      osbool *incomplete);
extern osbool palettev_set_gamma_corrections (os_correction_table const *red_table,
      os_correction_table const *green_table,
      os_correction_table const *blue_table);

/* ------------------------------------------------------------------------
 * Function:      palettev_invert_lcd()
 *
 * Description:   Inverts the LCD display
 *
 * Output:        incomplete - value of R4 on exit (X version only)
 *
 * Returns:       R4 (non-X version only)
 *
 * Other notes:   Calls SWI 0x34 with R4 = 0xA, R9 = 0x23.
 */

extern os_error *xpalettev_invert_lcd (osbool *incomplete);
extern osbool palettev_invert_lcd (void);

/* ------------------------------------------------------------------------
 * Function:      palettev_dim_screen()
 *
 * Description:   Controls screen dimming
 *
 * Input:         state - value of R0 on entry
 *
 * Output:        state_out - value of R0 on exit
 *                incomplete - value of R4 on exit (X version only)
 *
 * Returns:       R4 (non-X version only)
 *
 * Other notes:   Calls SWI 0x34 with R4 = 0xB, R9 = 0x23.
 */

extern os_error *xpalettev_dim_screen (palettev_screen_state state,
      palettev_screen_state *state_out,
      osbool *incomplete);
extern osbool palettev_dim_screen (palettev_screen_state state,
      palettev_screen_state *state_out);

/* ------------------------------------------------------------------------
 * Function:      os_read_vdu_variables()
 *
 * Description:   Reads a series of VDU variables
 *
 * Input:         var_list - value of R0 on entry
 *                value_list - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x31.
 */

extern os_error *xos_read_vdu_variables (os_vdu_var_list const *var_list,
      int *value_list);
__swi (0x31) void os_read_vdu_variables (os_vdu_var_list const *var_list,
      int *value_list);

/* ------------------------------------------------------------------------
 * Function:      os_read_point()
 *
 * Description:   Reads the colour of a point
 *
 * Input:         x - value of R0 on entry
 *                y - value of R1 on entry
 *
 * Output:        gcol - value of R2 on exit
 *                tint - value of R3 on exit
 *                off_screen - value of R4 on exit (X version only)
 *
 * Returns:       R4 (non-X version only)
 *
 * Other notes:   Calls SWI 0x32.
 */

extern os_error *xos_read_point (int x,
      int y,
      os_gcol *gcol,
      os_tint *tint,
      osbool *off_screen);
extern osbool os_read_point (int x,
      int y,
      os_gcol *gcol,
      os_tint *tint);

/* ------------------------------------------------------------------------
 * Function:      os_read_mode_variable()
 *
 * Description:   Reads information about a screen mode
 *
 * Input:         mode - value of R0 on entry
 *                var - value of R1 on entry
 *
 * Output:        var_val - value of R2 on exit
 *                psr - processor status register on exit (X version only)
 *
 * Returns:       psr (non-X version only)
 *
 * Other notes:   Calls SWI 0x35.
 */

extern os_error *xos_read_mode_variable (os_mode mode,
      os_mode_var var,
      int *var_val,
      bits *psr);
extern bits os_read_mode_variable (os_mode mode,
      os_mode_var var,
      int *var_val);

/* ------------------------------------------------------------------------
 * Function:      os_remove_cursors()
 *
 * Description:   Removes the cursors from the screen
 *
 * Other notes:   Calls SWI 0x36.
 */

extern os_error *xos_remove_cursors (void);
__swi (0x36) void os_remove_cursors (void);

/* ------------------------------------------------------------------------
 * Function:      os_restore_cursors()
 *
 * Description:   Restores the cursors to the screen
 *
 * Other notes:   Calls SWI 0x37.
 */

extern os_error *xos_restore_cursors (void);
__swi (0x37) void os_restore_cursors (void);

/* ------------------------------------------------------------------------
 * Function:      os_swi_number_to_string()
 *
 * Description:   Converts a SWI number to a string containing its name
 *
 * Input:         swi - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        used - value of R2 on exit (X version only)
 *
 * Returns:       R2 (non-X version only)
 *
 * Other notes:   Calls SWI 0x38.
 */

extern os_error *xos_swi_number_to_string (int swi,
      char *buffer,
      int size,
      int *used);
extern int os_swi_number_to_string (int swi,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_swi_number_from_string()
 *
 * Description:   Converts a string to a SWI number if valid
 *
 * Input:         swi_name - value of R1 on entry
 *
 * Output:        swi - value of R0 on exit (X version only)
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x39.
 */

extern os_error *xos_swi_number_from_string (char const *swi_name,
      int *swi);
extern int os_swi_number_from_string (char const *swi_name);

/* ------------------------------------------------------------------------
 * Function:      os_validate_address()
 *
 * Description:   Checks that a range of address are in logical RAM
 *
 * Input:         min - value of R0 on entry
 *                max - value of R1 on entry
 *
 * Output:        psr - processor status register on exit (X version only)
 *
 * Returns:       psr (non-X version only)
 *
 * Other notes:   Calls SWI 0x3A.
 */

extern os_error *xos_validate_address (byte *min,
      byte *max,
      bits *psr);
extern bits os_validate_address (byte *min,
      byte *max);

/* ------------------------------------------------------------------------
 * Function:      os_call_after()
 *
 * Description:   Calls a specified address after a delay
 *
 * Input:         delay - value of R0 on entry
 *                code - value of R1 on entry
 *                handle - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x3B.
 */

extern os_error *xos_call_after (int delay,
      void const *code,
      byte *handle);
__swi (0x3B) void os_call_after (int delay,
      void const *code,
      byte *handle);

/* ------------------------------------------------------------------------
 * Function:      os_call_every()
 *
 * Description:   Calls a specified address every time a delay elapses
 *
 * Input:         delay - value of R0 on entry
 *                code - value of R1 on entry
 *                handle - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x3C.
 */

extern os_error *xos_call_every (int delay,
      void const *code,
      byte *handle);
__swi (0x3C) void os_call_every (int delay,
      void const *code,
      byte *handle);

/* ------------------------------------------------------------------------
 * Function:      os_remove_ticker_event()
 *
 * Description:   Removes a given call address and handle value from the
 *                ticker event list
 *
 * Input:         code - value of R0 on entry
 *                handle - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x3D.
 */

extern os_error *xos_remove_ticker_event (void const *code,
      byte *handle);
__swi (0x3D) void os_remove_ticker_event (void const *code,
      byte *handle);

/* ------------------------------------------------------------------------
 * Function:      tickerv()
 *
 * Description:   Called every centisecond
 *
 * Other notes:   Calls SWI 0x34 with R9 = 0x1C.
 */

extern os_error *xtickerv (void);
extern void tickerv (void);

/* ------------------------------------------------------------------------
 * Function:      os_install_key_handler()
 *
 * Description:   Installs a key handler or reads the address of the
 *                current one
 *
 * Input:         key_handler - value of R0 on entry
 *
 * Output:        old_key_handler - value of R0 on exit (X version only)
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x3E.
 */

extern os_error *xos_install_key_handler (os_key_handler const *key_handler,
      os_key_handler **old_key_handler);
__swi (0x3E) os_key_handler *os_install_key_handler (os_key_handler const *key_handler);

/* ------------------------------------------------------------------------
 * Function:      keyv_present()
 *
 * Description:   The given keyboard is present
 *
 * Input:         keyboard_type - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x34 with R0 = 0x0, R9 = 0x13.
 */

extern os_error *xkeyv_present (keyv_type keyboard_type);
extern void keyv_present (keyv_type keyboard_type);

/* ------------------------------------------------------------------------
 * Function:      keyv_transition_up()
 *
 * Description:   Key released
 *
 * Input:         key - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x34 with R0 = 0x1, R9 = 0x13.
 */

extern os_error *xkeyv_transition_up (int key);
extern void keyv_transition_up (int key);

/* ------------------------------------------------------------------------
 * Function:      keyv_transition_down()
 *
 * Description:   Key pressed
 *
 * Input:         key - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x34 with R0 = 0x2, R9 = 0x13.
 */

extern os_error *xkeyv_transition_down (int key);
extern void keyv_transition_down (int key);

/* ------------------------------------------------------------------------
 * Function:      keyv_led_state()
 *
 * Description:   Notifies driver of LED state
 *
 * Input:         flags - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x34 with R0 = 0x3, R9 = 0x13.
 */

extern os_error *xkeyv_led_state (bits flags);
extern void keyv_led_state (bits flags);

/* ------------------------------------------------------------------------
 * Function:      keyv_enable_drivers()
 *
 * Description:   Enables keyboard device drivers
 *
 * Other notes:   Calls SWI 0x34 with R0 = 0x4, R9 = 0x13.
 */

extern os_error *xkeyv_enable_drivers (void);
extern void keyv_enable_drivers (void);

/* ------------------------------------------------------------------------
 * Function:      os_check_mode_valid()
 *
 * Description:   Checks if it is possible to change into a specified mode
 *
 * Input:         mode - value of R0 on entry
 *
 * Output:        mode_status - value of R0 on exit
 *                alternative_mode - value of R1 on exit
 *                psr - processor status register on exit (X version only)
 *
 * Returns:       psr (non-X version only)
 *
 * Other notes:   Calls SWI 0x3F.
 */

extern os_error *xos_check_mode_valid (os_mode mode,
      int *mode_status,
      os_mode *alternative_mode,
      bits *psr);
extern bits os_check_mode_valid (os_mode mode,
      int *mode_status,
      os_mode *alternative_mode);

/* ------------------------------------------------------------------------
 * Function:      os_change_environment()
 *
 * Description:   Installs a handler
 *
 * Input:         handler_type - value of R0 on entry
 *                handler - value of R1 on entry
 *                handle - value of R2 on entry
 *                buffer - value of R3 on entry
 *
 * Output:        old_handler - value of R1 on exit (X version only)
 *                old_handle - value of R2 on exit
 *                old_buffer - value of R3 on exit
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x40.
 */

extern os_error *xos_change_environment (os_handler_type handler_type,
      void const *handler,
      byte *handle,
      byte *buffer,
      void **old_handler,
      byte **old_handle,
      byte **old_buffer);
extern void *os_change_environment (os_handler_type handler_type,
      void const *handler,
      byte *handle,
      byte *buffer,
      byte **old_handle,
      byte **old_buffer);

/* ------------------------------------------------------------------------
 * Function:      changeenvironmentv()
 *
 * Description:   OS_ChangeEnvironment vector
 *
 * Input:         handler_type - value of R0 on entry
 *                handler - value of R1 on entry
 *                handle - value of R2 on entry
 *                buffer - value of R3 on entry
 *
 * Output:        old_handler - value of R1 on exit (X version only)
 *                old_handle - value of R2 on exit
 *                old_buffer - value of R3 on exit
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x34 with R9 = 0x1E.
 */

extern os_error *xchangeenvironmentv (os_handler_type handler_type,
      void const *handler,
      byte *handle,
      byte *buffer,
      void **old_handler,
      byte **old_handle,
      byte **old_buffer);
extern void *changeenvironmentv (os_handler_type handler_type,
      void const *handler,
      byte *handle,
      byte *buffer,
      byte **old_handle,
      byte **old_buffer);

/* ------------------------------------------------------------------------
 * Function:      osclaimscreenmemory_alloc()
 *
 * Description:   Claims spare screen memory - Deprecated under RISC OS
 *                3.7+
 *
 * Input:         size - value of R1 on entry
 *
 * Output:        size_out - value of R1 on exit
 *                blk - value of R2 on exit
 *                psr - processor status register on exit (X version only)
 *
 * Returns:       psr (non-X version only)
 *
 * Other notes:   Calls SWI 0x41 with R0 = 0x1.
 */

extern os_error *xosclaimscreenmemory_alloc (int size,
      int *size_out,
      void **blk,
      bits *psr);
extern bits osclaimscreenmemory_alloc (int size,
      int *size_out,
      void **blk);

/* ------------------------------------------------------------------------
 * Function:      osclaimscreenmemory_free()
 *
 * Description:   Releases spare screen memory - Deprecated under RISC OS
 *                3.7+
 *
 * Other notes:   Calls SWI 0x41 with R0 = 0x0.
 */

extern os_error *xosclaimscreenmemory_free (void);
extern void osclaimscreenmemory_free (void);

/* ------------------------------------------------------------------------
 * Function:      os_read_monotonic_time()
 *
 * Description:   Returns the number of centiseconds since the last hard
 *                reset
 *
 * Output:        t - value of R0 on exit (X version only)
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x42.
 */

extern os_error *xos_read_monotonic_time (os_t *t);
__swi (0x42) os_t os_read_monotonic_time (void);

/* ------------------------------------------------------------------------
 * Function:      os_substitute_args()
 *
 * Description:   Substitutes command line arguments
 *
 * Input:         args - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *                source - value of R3 on entry
 *                source_size - value of R4 on entry
 *
 * Output:        used - value of R2 on exit (X version only)
 *
 * Returns:       R2 (non-X version only)
 *
 * Other notes:   Calls SWI 0x43.
 */

extern os_error *xos_substitute_args (char const *args,
      char *buffer,
      int size,
      char const *source,
      int source_size,
      int *used);
extern int os_substitute_args (char const *args,
      char *buffer,
      int size,
      char const *source,
      int source_size);

/* ------------------------------------------------------------------------
 * Function:      os_substitute_args32()
 *
 * Description:   Substitutes command line arguments - RISC OS 5+
 *
 * Input:         args - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *                source - value of R3 on entry
 *                source_size - value of R4 on entry
 *                flags - value of R5 on entry
 *
 * Output:        used - value of R2 on exit (X version only)
 *
 * Returns:       R2 (non-X version only)
 *
 * Other notes:   Calls SWI 0x7E.
 */

extern os_error *xos_substitute_args32 (char const *args,
      char *buffer,
      int size,
      char const *source,
      int source_size,
      os_substitute_args_flags flags,
      int *used);
extern int os_substitute_args32 (char const *args,
      char *buffer,
      int size,
      char const *source,
      int source_size,
      os_substitute_args_flags flags);

/* ------------------------------------------------------------------------
 * Function:      os_pretty_print()
 *
 * Description:   Writes a string with some formatting to all of the active
 *                output streams
 *
 * Input:         string - value of R0 on entry
 *                dictionary - value of R1 on entry
 *                special - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x44.
 */

extern os_error *xos_pretty_print (char const *string,
      byte const *dictionary,
      char const *special);
__swi (0x44) void os_pretty_print (char const *string,
      byte const *dictionary,
      char const *special);

/* ------------------------------------------------------------------------
 * Function:      os_plot()
 *
 * Description:   Direct VDU call to general plot command
 *
 * Input:         plot_code - value of R0 on entry
 *                x - value of R1 on entry
 *                y - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x45.
 */

extern os_error *xos_plot (os_plot_code plot_code,
      int x,
      int y);
__swi (0x45) void os_plot (os_plot_code plot_code,
      int x,
      int y);

/* ------------------------------------------------------------------------
 * Function:      ukplotv()
 *
 * Description:   Called when an unknown OS_Plot command is issued
 *
 * Input:         plot_code - value of R0 on entry
 *
 * Other notes:   Calls SWI 0x34 with R9 = 0x19.
 */

extern os_error *xukplotv (os_plot_code plot_code);
extern void ukplotv (os_plot_code plot_code);

/* ------------------------------------------------------------------------
 * Function:      os_writen()
 *
 * Description:   Writes a counted string to all of the active output
 *                streams
 *
 * Input:         s - value of R0 on entry
 *                size - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x46.
 */

extern os_error *xos_writen (char const *s,
      int size);
__swi (0x46) void os_writen (char const *s,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_add_to_vector()
 *
 * Description:   Adds a routine to the list of those that claim a vector
 *
 * Input:         vector - value of R0 on entry
 *                routine - value of R1 on entry
 *                handle - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x47.
 */

extern os_error *xos_add_to_vector (int vector,
      void const *routine,
      byte *handle);
__swi (0x47) void os_add_to_vector (int vector,
      void const *routine,
      byte *handle);

/* ------------------------------------------------------------------------
 * Function:      os_write_env()
 *
 * Description:   Sets the program environment command string and start
 *                time
 *
 * Input:         command - value of R0 on entry
 *                start - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x48.
 */

extern os_error *xos_write_env (char const *command,
      os_date_and_time const *start);
__swi (0x48) void os_write_env (char const *command,
      os_date_and_time const *start);

/* ------------------------------------------------------------------------
 * Function:      os_read_args()
 *
 * Description:   Given a keyword definition, scans a command string
 *
 * Input:         keywords - value of R0 on entry
 *                input - value of R1 on entry
 *                buffer - value of R2 on entry
 *                size - value of R3 on entry
 *
 * Output:        spare - value of R3 on exit (X version only)
 *
 * Returns:       R3 (non-X version only)
 *
 * Other notes:   Calls SWI 0x49.
 */

extern os_error *xos_read_args (char const *keywords,
      char const *input,
      char *buffer,
      int size,
      int *spare);
extern int os_read_args (char const *keywords,
      char const *input,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_read_ram_fs_limits()
 *
 * Description:   Gets the current limits of the RAM filing system
 *
 * Output:        start - value of R0 on exit
 *                end - value of R1 on exit
 *
 * Other notes:   Calls SWI 0x4A.
 */

extern os_error *xos_read_ram_fs_limits (byte **start,
      char **end);
extern void os_read_ram_fs_limits (byte **start,
      char **end);

/* ------------------------------------------------------------------------
 * Function:      os_claim_device_vector()
 *
 * Description:   Claims a device vector
 *
 * Input:         device - value of R0 on entry
 *                driver - value of R1 on entry
 *                handle - value of R2 on entry
 *                status - value of R3 on entry
 *                mask - value of R4 on entry
 *
 * Other notes:   Calls SWI 0x4B.
 */

extern os_error *xos_claim_device_vector (os_device_type device,
      void const *driver,
      byte *handle,
      int *status,
      bits mask);
extern void os_claim_device_vector (os_device_type device,
      void const *driver,
      byte *handle,
      int *status,
      bits mask);

/* ------------------------------------------------------------------------
 * Function:      os_release_device_vector()
 *
 * Description:   Releases a device vector
 *
 * Input:         device - value of R0 on entry
 *                driver - value of R1 on entry
 *                handle - value of R2 on entry
 *                status - value of R3 on entry
 *                mask - value of R4 on entry
 *
 * Other notes:   Calls SWI 0x4C.
 */

extern os_error *xos_release_device_vector (os_device_type device,
      void const *driver,
      byte *handle,
      int *status,
      bits mask);
extern void os_release_device_vector (os_device_type device,
      void const *driver,
      byte *handle,
      int *status,
      bits mask);

/* ------------------------------------------------------------------------
 * Function:      os_delink_application()
 *
 * Description:   Removes any vectors that an application is using
 *
 * Input:         vector_details - value of R0 on entry
 *                size - value of R1 on entry
 *
 * Output:        used - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x4D.
 */

extern os_error *xos_delink_application (byte *vector_details,
      int size,
      int *used);
extern int os_delink_application (byte *vector_details,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_relink_application()
 *
 * Description:   Restores from a buffer any vectors that an application is
 *                using
 *
 * Input:         vector_details - value of R0 on entry
 *
 * Other notes:   Calls SWI 0x4E.
 */

extern os_error *xos_relink_application (byte const *vector_details);
__swi (0x4E) void os_relink_application (byte const *vector_details);

/* ------------------------------------------------------------------------
 * Function:      os_heap_sort()
 *
 * Description:   Heap-sorts a list of objects - prefer OS_HeapSort32 on
 *                RISC OS 5+
 *
 * Input:         count - value of R0 on entry
 *                array - value of R1 on entry
 *                compare_fn - value of R2 on entry
 *                handle - value of R3 on entry
 *                objects - value of R4 on entry
 *                object_size - value of R5 on entry
 *                workspace - value of R6 on entry
 *
 * Other notes:   Calls SWI 0x4F.
 */

extern os_error *xos_heap_sort (int count,
      int *array,
      os_sort_type compare_fn,
      byte *handle,
      byte *objects,
      int object_size,
      void *workspace);
extern void os_heap_sort (int count,
      int *array,
      os_sort_type compare_fn,
      byte *handle,
      byte *objects,
      int object_size,
      void *workspace);

/* ------------------------------------------------------------------------
 * Function:      os_heap_sort32()
 *
 * Description:   Heap-sorts a list of objects - RISC OS 5+
 *
 * Input:         count - value of R0 on entry
 *                array - value of R1 on entry
 *                compare_fn - value of R2 on entry
 *                handle - value of R3 on entry
 *                objects - value of R4 on entry
 *                object_size - value of R5 on entry
 *                workspace - value of R6 on entry
 *                flags - value of R7 on entry
 *
 * Other notes:   Calls SWI 0x7F.
 */

extern os_error *xos_heap_sort32 (int count,
      int *array,
      os_sort_type compare_fn,
      byte *handle,
      byte *objects,
      int object_size,
      void *workspace,
      os_heap_sort_flags flags);
extern void os_heap_sort32 (int count,
      int *array,
      os_sort_type compare_fn,
      byte *handle,
      byte *objects,
      int object_size,
      void *workspace,
      os_heap_sort_flags flags);

/* ------------------------------------------------------------------------
 * Function:      os_exit_and_die()
 *
 * Description:   Kills a module and passes control to the most recent exit
 *                handler
 *
 * Input:         error - value of R0 on entry
 *                rc - value of R2 on entry
 *                module_name - value of R3 on entry
 *
 * Other notes:   Calls SWI 0x50 with R1 = 0x58454241.
 */

extern os_error *xos_exit_and_die (os_error const *error,
      int rc,
      char const *module_name);
extern void os_exit_and_die (os_error const *error,
      int rc,
      char const *module_name);

/* ------------------------------------------------------------------------
 * Function:      os_read_mem_map_info()
 *
 * Description:   Reads the page size and count
 *
 * Output:        page_size - value of R0 on exit
 *                page_count - value of R1 on exit
 *
 * Other notes:   Calls SWI 0x51.
 */

extern os_error *xos_read_mem_map_info (int *page_size,
      int *page_count);
extern void os_read_mem_map_info (int *page_size,
      int *page_count);

/* ------------------------------------------------------------------------
 * Function:      os_read_mem_map_entries()
 *
 * Description:   Reads by page number the logical to physical memory
 *                mapping used by MEMC
 *
 * Input:         request_list - value of R0 on entry
 *
 * Other notes:   Calls SWI 0x52.
 */

extern os_error *xos_read_mem_map_entries (os_mem_map_request_list *request_list);
__swi (0x52) void os_read_mem_map_entries (os_mem_map_request_list *request_list);

/* ------------------------------------------------------------------------
 * Function:      os_set_mem_map_entries()
 *
 * Description:   Writes the logical to physical memory mapping used by
 *                MEMC
 *
 * Input:         request_list - value of R0 on entry
 *
 * Other notes:   Calls SWI 0x53.
 */

extern os_error *xos_set_mem_map_entries (os_mem_map_request_list const *request_list);
__swi (0x53) void os_set_mem_map_entries (os_mem_map_request_list const *request_list);

/* ------------------------------------------------------------------------
 * Function:      os_add_call_back()
 *
 * Description:   Adds a transient callback to the list
 *
 * Input:         call_back - value of R0 on entry
 *                handle - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x54.
 */

extern os_error *xos_add_call_back (void const *call_back,
      byte *handle);
__swi (0x54) void os_add_call_back (void const *call_back,
      byte *handle);

/* ------------------------------------------------------------------------
 * Function:      os_read_default_handler()
 *
 * Description:   Gets the address of the default handler
 *
 * Input:         handler_type - value of R0 on entry
 *
 * Output:        handler - value of R1 on exit (X version only)
 *                handle - value of R2 on exit
 *                buffer - value of R3 on exit
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x55.
 */

extern os_error *xos_read_default_handler (int handler_type,
      void **handler,
      byte **handle,
      byte **buffer);
extern void *os_read_default_handler (int handler_type,
      byte **handle,
      byte **buffer);

/* ------------------------------------------------------------------------
 * Function:      os_set_ecf_origin()
 *
 * Description:   Sets the origin of the ECF patterns
 *
 * Input:         x - value of R0 on entry
 *                y - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x56.
 */

extern os_error *xos_set_ecf_origin (int x,
      int y);
__swi (0x56) void os_set_ecf_origin (int x,
      int y);

/* ------------------------------------------------------------------------
 * Function:      os_confirm()
 *
 * Description:   Gets a yes or no answer
 *
 * Output:        key - value of R0 on exit
 *                psr - processor status register on exit (X version only)
 *
 * Returns:       psr (non-X version only)
 *
 * Other notes:   Calls SWI 0x59.
 */

extern os_error *xos_confirm (char *key,
      bits *psr);
extern bits os_confirm (char *key);

/* ------------------------------------------------------------------------
 * Function:      os_changed_box()
 *
 * Description:   Determines which area of the screen has changed
 *
 * Input:         state - value of R0 on entry
 *
 * Output:        enabled - value of R0 on exit (X version only)
 *                change_box - value of R1 on exit
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x5A.
 */

extern os_error *xos_changed_box (os_changed_box_state state,
      osbool *enabled,
      os_change_box **change_box);
extern osbool os_changed_box (os_changed_box_state state,
      os_change_box **change_box);

/* ------------------------------------------------------------------------
 * Function:      os_crc()
 *
 * Description:   Calculates the cyclic redundancy check for a block of
 *                data
 *
 * Input:         crc_in - value of R0 on entry
 *                block - value of R1 on entry
 *                end - value of R2 on entry
 *                stride - value of R3 on entry
 *
 * Output:        crc - value of R0 on exit (X version only)
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x5B.
 */

extern os_error *xos_crc (int crc_in,
      byte const *block,
      byte const *end,
      int stride,
      int *crc);
__swi (0x5B) int os_crc (int crc_in,
      byte const *block,
      byte const *end,
      int stride);

/* ------------------------------------------------------------------------
 * Function:      os_read_dynamic_area()
 *
 * Description:   Reads the space allocation of a dynamic area
 *
 * Input:         area - value of R0 on entry
 *
 * Output:        area_start - value of R0 on exit (X version only)
 *                size - value of R1 on exit
 *                size_limit - value of R2 on exit
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x5C.
 */

extern os_error *xos_read_dynamic_area (os_dynamic_area_no area,
      byte **area_start,
      int *size,
      int *size_limit);
extern byte *os_read_dynamic_area (os_dynamic_area_no area,
      int *size,
      int *size_limit);

/* ------------------------------------------------------------------------
 * Function:      os_print_char()
 *
 * Description:   Sends a character to the printer stream
 *
 * Input:         c - value of R0 on entry
 *
 * Other notes:   Calls SWI 0x5D.
 */

extern os_error *xos_print_char (char c);
__swi (0x5D) void os_print_char (char c);

/* ------------------------------------------------------------------------
 * Function:      os_change_redirection()
 *
 * Description:   Reads or writes OS_CLI input/output redirection handles -
 *                prefer OS_ChangeRedirectionW
 *
 * Input:         input - value of R0 on entry
 *                output - value of R1 on entry
 *
 * Output:        old_input - value of R0 on exit
 *                old_output - value of R1 on exit
 *
 * Other notes:   Calls SWI 0x5E.
 */

extern os_error *xos_change_redirection (os_f input,
      os_f output,
      os_f *old_input,
      os_f *old_output);
extern void os_change_redirection (os_f input,
      os_f output,
      os_f *old_input,
      os_f *old_output);

/* ------------------------------------------------------------------------
 * Function:      os_change_redirectionw()
 *
 * Description:   Reads or writes OS_CLI input/output redirection. Uses
 *                32-bit file handles
 *
 * Input:         input - value of R0 on entry
 *                output - value of R1 on entry
 *
 * Output:        old_input - value of R0 on exit
 *                old_output - value of R1 on exit
 *
 * Other notes:   Calls SWI 0x5E.
 */

extern os_error *xos_change_redirectionw (os_fw input,
      os_fw output,
      os_fw *old_input,
      os_fw *old_output);
extern void os_change_redirectionw (os_fw input,
      os_fw output,
      os_fw *old_input,
      os_fw *old_output);

/* ------------------------------------------------------------------------
 * Function:      os_remove_call_back()
 *
 * Description:   Removes a transient callback from the list
 *
 * Input:         call_back - value of R0 on entry
 *                handle - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x5F.
 */

extern os_error *xos_remove_call_back (void const *call_back,
      byte *handle);
__swi (0x5F) void os_remove_call_back (void const *call_back,
      byte *handle);

/* ------------------------------------------------------------------------
 * Function:      os_find_mem_map_entries()
 *
 * Description:   Reads by logical address the logical to physical memory
 *                mapping used by MEMC
 *
 * Input:         request_list - value of R0 on entry
 *
 * Other notes:   Calls SWI 0x60.
 */

extern os_error *xos_find_mem_map_entries (os_mem_map_request_list *request_list);
__swi (0x60) void os_find_mem_map_entries (os_mem_map_request_list *request_list);

/* ------------------------------------------------------------------------
 * Function:      os_set_colour()
 *
 * Description:   Sets the current colour or colour pattern
 *
 * Input:         flags - value of R0 on entry
 *                colour_number - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x61.
 */

extern os_error *xos_set_colour (os_colour_flags flags,
      os_colour_number colour_number);
__swi (0x61) void os_set_colour (os_colour_flags flags,
      os_colour_number colour_number);

/* ------------------------------------------------------------------------
 * Function:      os_read_colour()
 *
 * Description:   Reads the current colour or colour pattern - RISC OS
 *                3.5+
 *
 * Input:         flags - value of R0 on entry
 *                pattern - value of R1 on entry
 *
 * Output:        flags_out - value of R0 on exit
 *                colour_number - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x61 with R0 |= 0x80.
 */

extern os_error *xos_read_colour (os_colour_flags flags,
      os_ecf *pattern,
      os_colour_flags *flags_out,
      os_colour_number *colour_number);
extern os_colour_number os_read_colour (os_colour_flags flags,
      os_ecf *pattern,
      os_colour_flags *flags_out);

/* ------------------------------------------------------------------------
 * Function:      ospointer_get()
 *
 * Description:   Gets the currently selected pointer device type - RISC O
 *                S 3.5+
 *
 * Output:        pointer_type - value of R0 on exit (X version only)
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x64 with R0 = 0x0.
 */

extern os_error *xospointer_get (os_pointer_type *pointer_type);
extern os_pointer_type ospointer_get (void);

/* ------------------------------------------------------------------------
 * Function:      ospointer_set()
 *
 * Description:   Sets the currently selected pointer device type - RISC O
 *                S 3.5+
 *
 * Input:         pointer_type - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x64 with R0 = 0x1.
 */

extern os_error *xospointer_set (os_pointer_type pointer_type);
extern void ospointer_set (os_pointer_type pointer_type);

/* ------------------------------------------------------------------------
 * Function:      ospointer_read_alternate_position()
 *
 * Description:   Reads the position of the alternate pointing device -
 *                RISC OS 4.32+
 *
 * Output:        xposition - value of R0 on exit
 *                yposition - value of R1 on exit
 *
 * Other notes:   Calls SWI 0x64 with R0 = 0x2.
 */

extern os_error *xospointer_read_alternate_position (int *xposition,
      int *yposition);
extern void ospointer_read_alternate_position (int *xposition,
      int *yposition);

/* ------------------------------------------------------------------------
 * Function:      pointerv_status()
 *
 * Description:   Requests status of pointer device - RISC OS 3.5+
 *
 * Input:         pointer_type - value of R1 on entry
 *
 * Output:        xmove - value of R2 on exit
 *                ymove - value of R3 on exit
 *
 * Other notes:   Calls SWI 0x34 with R0 = 0x0, R9 = 0x26.
 */

extern os_error *xpointerv_status (os_pointer_type pointer_type,
      int *xmove,
      int *ymove);
extern void pointerv_status (os_pointer_type pointer_type,
      int *xmove,
      int *ymove);

/* ------------------------------------------------------------------------
 * Function:      pointerv_enumerate()
 *
 * Description:   Enumerates pointer device types - RISC OS 3.5+
 *
 * Input:         record - value of R1 on entry
 *
 * Output:        record_out - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x34 with R0 = 0x1, R9 = 0x26.
 */

extern os_error *xpointerv_enumerate (pointerv_record const *record,
      pointerv_record **record_out);
extern pointerv_record *pointerv_enumerate (pointerv_record const *record);

/* ------------------------------------------------------------------------
 * Function:      pointerv_selected()
 *
 * Description:   Pointer device type selected - RISC OS 3.5+
 *
 * Input:         pointer_type - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x34 with R0 = 0x2, R9 = 0x26.
 */

extern os_error *xpointerv_selected (os_pointer_type pointer_type);
extern void pointerv_selected (os_pointer_type pointer_type);

/* ------------------------------------------------------------------------
 * Function:      pointerv_extended_status()
 *
 * Description:   Requests extended status of pointer device - RISC OS
 *                4.32+
 *
 * Input:         pointer_type - value of R1 on entry
 *
 * Output:        xmove - value of R2 on exit
 *                ymove - value of R3 on exit
 *                xmove_alternate - value of R4 on exit
 *                ymove_alternate - value of R5 on exit
 *                buttons - value of R6 on exit
 *
 * Other notes:   Calls SWI 0x34 with R0 = 0x4, R9 = 0x26.
 */

extern os_error *xpointerv_extended_status (os_pointer_type pointer_type,
      int *xmove,
      int *ymove,
      int *xmove_alternate,
      int *ymove_alternate,
      bits *buttons);
extern void pointerv_extended_status (os_pointer_type pointer_type,
      int *xmove,
      int *ymove,
      int *xmove_alternate,
      int *ymove_alternate,
      bits *buttons);

/* ------------------------------------------------------------------------
 * Function:      osscreenmode_select()
 *
 * Description:   Selects a screen mode - RISC OS 3.5+
 *
 * Input:         mode - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x65 with R0 = 0x0.
 */

extern os_error *xosscreenmode_select (os_mode mode);
extern void osscreenmode_select (os_mode mode);

/* ------------------------------------------------------------------------
 * Function:      osscreenmode_current()
 *
 * Description:   Returns the mode specifier for the current mode - RISC O
 *                S 3.5+
 *
 * Output:        mode - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x65 with R0 = 0x1.
 */

extern os_error *xosscreenmode_current (os_mode *mode);
extern os_mode osscreenmode_current (void);

/* ------------------------------------------------------------------------
 * Function:      osscreenmode_enumerate()
 *
 * Description:   Enumerates the available screen modes - RISC OS 3.5+
 *
 * Input:         context - value of R2 on entry
 *                block - value of R6 on entry
 *                block_size - value of R7 on entry
 *
 * Output:        complete - value of R1 on exit (X version only)
 *                context_out - value of R2 on exit
 *                end - value of R6 on exit
 *                free - value of R7 on exit
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x65 with R0 = 0x2.
 */

extern os_error *xosscreenmode_enumerate (int context,
      os_mode_block *block,
      int block_size,
      osbool *complete,
      int *context_out,
      byte **end,
      int *free);
extern osbool osscreenmode_enumerate (int context,
      os_mode_block *block,
      int block_size,
      int *context_out,
      byte **end,
      int *free);

/* ------------------------------------------------------------------------
 * Function:      osscreenmode_cache_control()
 *
 * Description:   Controls cacheing of screen memory and screen cleaning -
 *                RISC OS 4+
 *
 * Input:         flags - value of R1 on entry
 *                laziness - value of R2 on entry
 *
 * Output:        current_flags - value of R1 on exit (X version only)
 *                current_laziness - value of R2 on exit
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x65 with R0 = 0x4.
 */

extern os_error *xosscreenmode_cache_control (osscreenmode_cache_flags flags,
      osscreenmode_cleaner_laziness laziness,
      osscreenmode_cache_flags *current_flags,
      osscreenmode_cleaner_laziness *current_laziness);
extern osscreenmode_cache_flags osscreenmode_cache_control (osscreenmode_cache_flags flags,
      osscreenmode_cleaner_laziness laziness,
      osscreenmode_cleaner_laziness *current_laziness);

/* ------------------------------------------------------------------------
 * Function:      osscreenmode_request_clean()
 *
 * Description:   Request a foreground screen clean - RISC OS 4+
 *
 * Other notes:   Calls SWI 0x65 with R0 = 0x5.
 */

extern os_error *xosscreenmode_request_clean (void);
extern void osscreenmode_request_clean (void);

/* ------------------------------------------------------------------------
 * Function:      osscreenmode_force_clean()
 *
 * Description:   Force a screen clean if needed - RISC OS 4+
 *
 * Other notes:   Calls SWI 0x65 with R0 = 0x6.
 */

extern os_error *xosscreenmode_force_clean (void);
extern void osscreenmode_force_clean (void);

/* ------------------------------------------------------------------------
 * Function:      osdynamicarea_create()
 *
 * Description:   Creates a new dynamic area - RISC OS 3.5+
 *
 * Input:         area - value of R1 on entry
 *                size - value of R2 on entry
 *                base_address - value of R3 on entry
 *                flags - value of R4 on entry
 *                size_limit - value of R5 on entry
 *                handler - value of R6 on entry
 *                workspace - value of R7 on entry
 *                description - value of R8 on entry
 *
 * Output:        area_out - value of R1 on exit (X version only)
 *                base_address_out - value of R3 on exit
 *                size_limit_out - value of R5 on exit
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x66 with R0 = 0x0.
 */

extern os_error *xosdynamicarea_create (os_dynamic_area_no area,
      int size,
      byte const *base_address,
      bits flags,
      int size_limit,
      void const *handler,
      void *workspace,
      char const *description,
      os_dynamic_area_no *area_out,
      byte **base_address_out,
      int *size_limit_out);
extern os_dynamic_area_no osdynamicarea_create (os_dynamic_area_no area,
      int size,
      byte const *base_address,
      bits flags,
      int size_limit,
      void const *handler,
      void *workspace,
      char const *description,
      byte **base_address_out,
      int *size_limit_out);

/* ------------------------------------------------------------------------
 * Function:      osdynamicarea_delete()
 *
 * Description:   Removes a previously created dynamic area - RISC OS 3.5+
 *
 * Input:         area - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x66 with R0 = 0x1.
 */

extern os_error *xosdynamicarea_delete (os_dynamic_area_no area);
extern void osdynamicarea_delete (os_dynamic_area_no area);

/* ------------------------------------------------------------------------
 * Function:      osdynamicarea_read()
 *
 * Description:   Returns information on a dynamic area - RISC OS 3.5+
 *
 * Input:         area - value of R1 on entry
 *
 * Output:        size - value of R2 on exit
 *                base_address - value of R3 on exit
 *                flags - value of R4 on exit
 *                size_limit - value of R5 on exit
 *                handler - value of R6 on exit
 *                workspace - value of R7 on exit
 *                description - value of R8 on exit
 *
 * Other notes:   Calls SWI 0x66 with R0 = 0x2.
 */

extern os_error *xosdynamicarea_read (os_dynamic_area_no area,
      int *size,
      byte **base_address,
      bits *flags,
      int *size_limit,
      void **handler,
      void **workspace,
      char **description);
extern void osdynamicarea_read (os_dynamic_area_no area,
      int *size,
      byte **base_address,
      bits *flags,
      int *size_limit,
      void **handler,
      void **workspace,
      char **description);

/* ------------------------------------------------------------------------
 * Function:      osdynamicarea_enumerate()
 *
 * Description:   Enumerates dynamic areas - RISC OS 3.5+
 *
 * Input:         context - value of R1 on entry
 *
 * Output:        context_out - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x66 with R0 = 0x3.
 */

extern os_error *xosdynamicarea_enumerate (int context,
      int *context_out);
extern int osdynamicarea_enumerate (int context);

/* ------------------------------------------------------------------------
 * Function:      osdynamicarea_renumber()
 *
 * Description:   Renumbers dynamic areas - RISC OS 3.5+
 *
 * Input:         old_area - value of R1 on entry
 *                new_area - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x66 with R0 = 0x4.
 */

extern os_error *xosdynamicarea_renumber (os_dynamic_area_no old_area,
      os_dynamic_area_no new_area);
extern void osdynamicarea_renumber (os_dynamic_area_no old_area,
      os_dynamic_area_no new_area);

/* ------------------------------------------------------------------------
 * Function:      osdynamicarea_free_space()
 *
 * Description:   Return total free space, allowing for shrinkable areas -
 *                RISC OS 4+
 *
 * Input:         exclude_area - value of R1 on entry
 *
 * Output:        free_space - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x66 with R0 = 0x5.
 */

extern os_error *xosdynamicarea_free_space (os_dynamic_area_no exclude_area,
      int *free_space);
extern int osdynamicarea_free_space (os_dynamic_area_no exclude_area);

/* ------------------------------------------------------------------------
 * Function:      osdynamicarea_set_clamps()
 *
 * Description:   Set clamps on maximum sizes for subsequently created
 *                dynamic areas - RISC OS 4+
 *
 * Input:         unlimited_area_clamp - value of R1 on entry
 *                limited_area_clamp - value of R2 on entry
 *                sparse_area_clamp - value of R3 on entry
 *
 * Output:        old_unlimited_area_clamp - value of R1 on exit
 *                old_limited_area_clamp - value of R2 on exit
 *                old_sparse_area_clamp - value of R3 on exit
 *
 * Other notes:   Calls SWI 0x66 with R0 = 0x8.
 */

extern os_error *xosdynamicarea_set_clamps (int unlimited_area_clamp,
      int limited_area_clamp,
      int sparse_area_clamp,
      int *old_unlimited_area_clamp,
      int *old_limited_area_clamp,
      int *old_sparse_area_clamp);
extern void osdynamicarea_set_clamps (int unlimited_area_clamp,
      int limited_area_clamp,
      int sparse_area_clamp,
      int *old_unlimited_area_clamp,
      int *old_limited_area_clamp,
      int *old_sparse_area_clamp);

/* ------------------------------------------------------------------------
 * Function:      osdynamicarea_ensure_region()
 *
 * Description:   Ensures that a region of a sparse area is mapped to valid
 *                memory - RISC OS 4+
 *
 * Input:         area - value of R1 on entry
 *                base_address - value of R2 on entry
 *                size - value of R3 on entry
 *
 * Other notes:   Calls SWI 0x66 with R0 = 0x9.
 */

extern os_error *xosdynamicarea_ensure_region (os_dynamic_area_no area,
      byte const *base_address,
      int size);
extern void osdynamicarea_ensure_region (os_dynamic_area_no area,
      byte const *base_address,
      int size);

/* ------------------------------------------------------------------------
 * Function:      osdynamicarea_release_region()
 *
 * Description:   Allows a region of a sparse area to be released as free
 *                memory - RISC OS 4+
 *
 * Input:         area - value of R1 on entry
 *                base_address - value of R2 on entry
 *                size - value of R3 on entry
 *
 * Other notes:   Calls SWI 0x66 with R0 = 0xA.
 */

extern os_error *xosdynamicarea_release_region (os_dynamic_area_no area,
      byte const *base_address,
      int size);
extern void osdynamicarea_release_region (os_dynamic_area_no area,
      byte const *base_address,
      int size);

/* ------------------------------------------------------------------------
 * Function:      osmemory_page_op()
 *
 * Description:   General page block operations - RISC OS 3.5+
 *
 * Input:         flags - value of R0 on entry
 *                page_block - value of R1 on entry
 *                page_count - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x68 with R0 |= 0x0.
 */

extern os_error *xosmemory_page_op (osmemory_flags flags,
      os_page_block const *page_block,
      int page_count);
extern void osmemory_page_op (osmemory_flags flags,
      os_page_block const *page_block,
      int page_count);

/* ------------------------------------------------------------------------
 * Function:      osmemory_read_arrangement_table_size()
 *
 * Description:   Reads the size of the physical memory arrangement table -
 *                RISC OS 3.5+
 *
 * Output:        table_size - value of R1 on exit (X version only)
 *                page_size - value of R2 on exit
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x68 with R0 = 0x6.
 */

extern os_error *xosmemory_read_arrangement_table_size (int *table_size,
      int *page_size);
extern int osmemory_read_arrangement_table_size (int *page_size);

/* ------------------------------------------------------------------------
 * Function:      osmemory_read_arrangement_table()
 *
 * Description:   Read the physical memory arrangement table - RISC OS
 *                3.5+
 *
 * Input:         arrangement_table - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x68 with R0 = 0x7.
 */

extern os_error *xosmemory_read_arrangement_table (byte *arrangement_table);
extern void osmemory_read_arrangement_table (byte *arrangement_table);

/* ------------------------------------------------------------------------
 * Function:      osmemory_read_size()
 *
 * Description:   Reads the amount of a specified type of memory available
 *                on the computer - RISC OS 3.5+
 *
 * Input:         type - value of R0 on entry
 *
 * Output:        page_count - value of R1 on exit
 *                page_size - value of R2 on exit
 *
 * Other notes:   Calls SWI 0x68 with R0 |= 0x8.
 */

extern os_error *xosmemory_read_size (osmemory_type_flags type,
      int *page_count,
      int *page_size);
extern void osmemory_read_size (osmemory_type_flags type,
      int *page_count,
      int *page_size);

/* ------------------------------------------------------------------------
 * Function:      osmemory_read_controller()
 *
 * Description:   Reads controller presence and base address - RISC OS
 *                3.5+
 *
 * Input:         controller - value of R1 on entry
 *
 * Output:        controller_addr - value of R1 on exit
 *
 * Other notes:   Calls SWI 0x68 with R0 = 0x9.
 */

extern os_error *xosmemory_read_controller (osmemory_controller_flags controller,
      void **controller_addr);
extern void osmemory_read_controller (osmemory_controller_flags controller,
      void **controller_addr);

/* ------------------------------------------------------------------------
 * Function:      osmemory_find_contiguous()
 *
 * Description:   Recommends a base page for a currently available (not
 *                locked down) region of physically contiguous RAM - RISC O
 *                S 4+
 *
 * Input:         size - value of R1 on entry
 *
 * Output:        page_number - value of R3 on exit (X version only)
 *
 * Returns:       R3 (non-X version only)
 *
 * Other notes:   Calls SWI 0x68 with R0 = 0xC.
 */

extern os_error *xosmemory_find_contiguous (int size,
      int *page_number);
extern int osmemory_find_contiguous (int size);

/* ------------------------------------------------------------------------
 * Function:      osclaimprocessorvector_alloc()
 *
 * Description:   Claims a processor vector - RISC OS 3.5+
 *
 * Input:         vector - value of R0 on entry
 *                routine - value of R1 on entry
 *
 * Output:        old_routine - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x69 with R0 |= 0x100.
 */

extern os_error *xosclaimprocessorvector_alloc (bits vector,
      void const *routine,
      void **old_routine);
extern void *osclaimprocessorvector_alloc (bits vector,
      void const *routine);

/* ------------------------------------------------------------------------
 * Function:      osclaimprocessorvector_free()
 *
 * Description:   Releases a processor vector - RISC OS 3.5+
 *
 * Input:         vector - value of R0 on entry
 *                old_routine - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x69 with R0 |= 0x0.
 */

extern os_error *xosclaimprocessorvector_free (bits vector,
      void const *old_routine);
extern void osclaimprocessorvector_free (bits vector,
      void const *old_routine);

/* ------------------------------------------------------------------------
 * Function:      os_reset()
 *
 * Description:   Performs a hard reset - RISC OS 3.5+
 *
 * Other notes:   Calls SWI 0x6A.
 */

extern os_error *xos_reset (void);
__swi (0x6A) void os_reset (void);

/* ------------------------------------------------------------------------
 * Function:      os_mmu_control()
 *
 * Description:   Modifies the ARM MMU - prefer Cache_Control; RISC OS
 *                3.5+
 *
 * Input:         eor_mask - value of R1 on entry
 *                and_mask - value of R2 on entry
 *
 * Output:        old_value - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x6B with R0 = 0x0.
 */

extern os_error *xos_mmu_control (bits eor_mask,
      bits and_mask,
      bits *old_value);
extern bits os_mmu_control (bits eor_mask,
      bits and_mask);

/* ------------------------------------------------------------------------
 * Function:      osplatformfeatures_get_features()
 *
 * Description:   Reports platform features - RISC OS 3.7+
 *
 * Output:        flags - value of R0 on exit (X version only)
 *                predisable_fn - value of R1 on exit
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x6D with R0 = 0x0.
 */

extern os_error *xosplatformfeatures_get_features (os_platform_feature_flags *flags,
      void **predisable_fn);
extern os_platform_feature_flags osplatformfeatures_get_features (void **predisable_fn);

/* ------------------------------------------------------------------------
 * Function:      os_synchronise_code_areas()
 *
 * Description:   Informs the OS that code has been newly generated or
 *                modified in memory - RISC OS 3.7+
 *
 * Input:         flags - value of R0 on entry
 *                start - value of R1 on entry
 *                end - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x6E.
 */

extern os_error *xos_synchronise_code_areas (bits flags,
      void const *start,
      void const *end);
__swi (0x6E) void os_synchronise_code_areas (bits flags,
      void const *start,
      void const *end);

/* ------------------------------------------------------------------------
 * Function:      os_enter_usr32()
 *
 * Description:   Enter 32 bit user mode - RISC OS 4+
 *
 * Other notes:   Calls SWI 0x73.
 */

extern os_error *xos_enter_usr32 (void);
__swi (0x73) void os_enter_usr32 (void);

/* ------------------------------------------------------------------------
 * Function:      os_enter_usr26()
 *
 * Description:   Enter 26 bit user mode - RISC OS 4+
 *
 * Other notes:   Calls SWI 0x74.
 */

extern os_error *xos_enter_usr26 (void);
__swi (0x74) void os_enter_usr26 (void);

/* ------------------------------------------------------------------------
 * Function:      os_hardware_call_hal()
 *
 * Description:   Calls a HAL routine - RISC OS 5+
 *
 * Input:         arg1 - value of R0 on entry
 *                arg2 - value of R1 on entry
 *                arg3 - value of R2 on entry
 *                arg4 - value of R3 on entry
 *                arg5 - value of R4 on entry
 *                arg6 - value of R5 on entry
 *                arg7 - value of R6 on entry
 *                arg8 - value of R7 on entry
 *                flags - value of R8 on entry
 *                call - value of R9 on entry
 *
 * Output:        result1 - value of R0 on exit
 *                result2 - value of R1 on exit
 *                result3 - value of R2 on exit
 *                result4 - value of R3 on exit
 *
 * Other notes:   Calls SWI 0x7A with R8 |= 0x0.
 */

extern os_error *xos_hardware_call_hal (int arg1,
      int arg2,
      int arg3,
      int arg4,
      int arg5,
      int arg6,
      int arg7,
      int arg8,
      os_hardware_flags flags,
      int call,
      int *result1,
      int *result2,
      int *result3,
      int *result4);
extern void os_hardware_call_hal (int arg1,
      int arg2,
      int arg3,
      int arg4,
      int arg5,
      int arg6,
      int arg7,
      int arg8,
      os_hardware_flags flags,
      int call,
      int *result1,
      int *result2,
      int *result3,
      int *result4);

/* ------------------------------------------------------------------------
 * Function:      os_hardware_find_hal_routine()
 *
 * Description:   Finds the address of a HAL routine - RISC OS 5+
 *
 * Input:         flags - value of R8 on entry
 *                call - value of R9 on entry
 *
 * Output:        routine - value of R0 on exit (X version only)
 *                static_base - value of R1 on exit
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x7A with R8 |= 0x1.
 */

extern os_error *xos_hardware_find_hal_routine (os_hardware_flags flags,
      int call,
      void **routine,
      int *static_base);
extern void *os_hardware_find_hal_routine (os_hardware_flags flags,
      int call,
      int *static_base);

/* ------------------------------------------------------------------------
 * Function:      os_convert_standard_date_and_time()
 *
 * Description:   Converts a 5-byte time into a string - prefer
 *                Territory_ConvertStandardDateAndTime
 *
 * Input:         date_and_time - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xC0.
 */

extern os_error *xos_convert_standard_date_and_time (os_date_and_time const *date_and_time,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_standard_date_and_time (os_date_and_time const *date_and_time,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_date_and_time()
 *
 * Description:   Converts a 5-byte time into a string using a supplied
 *                format string - prefer Territory_ConvertDateAndTime
 *
 * Input:         date_and_time - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *                format - value of R3 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xC1.
 */

extern os_error *xos_convert_date_and_time (os_date_and_time const *date_and_time,
      char *buffer,
      int size,
      char const *format,
      char **end);
extern char *os_convert_date_and_time (os_date_and_time const *date_and_time,
      char *buffer,
      int size,
      char const *format);

/* ------------------------------------------------------------------------
 * Function:      os_convert_hex1()
 *
 * Description:   Converts a number into a 1-digit hexadecimal string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xD0.
 */

extern os_error *xos_convert_hex1 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_hex1 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_hex2()
 *
 * Description:   Converts a number into a 2-digit hexadecimal string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xD1.
 */

extern os_error *xos_convert_hex2 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_hex2 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_hex4()
 *
 * Description:   Converts a number into a 4-digit hexadecimal string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xD2.
 */

extern os_error *xos_convert_hex4 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_hex4 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_hex6()
 *
 * Description:   Converts a number into a 6-digit hexadecimal string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xD3.
 */

extern os_error *xos_convert_hex6 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_hex6 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_hex8()
 *
 * Description:   Converts a number into an 8-digit hexadecimal string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xD4.
 */

extern os_error *xos_convert_hex8 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_hex8 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_cardinal1()
 *
 * Description:   Converts a 1-byte number into an unsigned decimal string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xD5.
 */

extern os_error *xos_convert_cardinal1 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_cardinal1 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_cardinal2()
 *
 * Description:   Converts a 2-byte number into an unsigned decimal string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xD6.
 */

extern os_error *xos_convert_cardinal2 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_cardinal2 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_cardinal3()
 *
 * Description:   Converts a 3-byte number into an unsigned decimal string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xD7.
 */

extern os_error *xos_convert_cardinal3 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_cardinal3 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_cardinal4()
 *
 * Description:   Converts a 4-byte number into an unsigned decimal string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xD8.
 */

extern os_error *xos_convert_cardinal4 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_cardinal4 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_integer1()
 *
 * Description:   Converts a 1-byte number into a signed decimal string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xD9.
 */

extern os_error *xos_convert_integer1 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_integer1 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_integer2()
 *
 * Description:   Converts a 2-byte number into a signed decimal string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xDA.
 */

extern os_error *xos_convert_integer2 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_integer2 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_integer3()
 *
 * Description:   Converts a 3-byte number into a signed decimal string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xDB.
 */

extern os_error *xos_convert_integer3 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_integer3 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_integer4()
 *
 * Description:   Converts a 4-byte number into a signed decimal string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xDC.
 */

extern os_error *xos_convert_integer4 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_integer4 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_binary1()
 *
 * Description:   Converts a 1-byte number into an unsigned binary string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xDD.
 */

extern os_error *xos_convert_binary1 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_binary1 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_binary2()
 *
 * Description:   Converts a 2-byte number into an unsigned binary string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xDE.
 */

extern os_error *xos_convert_binary2 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_binary2 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_binary3()
 *
 * Description:   Converts a 3-byte number into an unsigned binary string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xDF.
 */

extern os_error *xos_convert_binary3 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_binary3 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_binary4()
 *
 * Description:   Converts a 4-byte number into an unsigned binary string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xE0.
 */

extern os_error *xos_convert_binary4 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_binary4 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_spaced_cardinal1()
 *
 * Description:   Converts a 1-byte number into a spaced unsigned decimal
 *                string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xE1.
 */

extern os_error *xos_convert_spaced_cardinal1 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_spaced_cardinal1 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_spaced_cardinal2()
 *
 * Description:   Converts a 2-byte number into a spaced unsigned decimal
 *                string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xE2.
 */

extern os_error *xos_convert_spaced_cardinal2 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_spaced_cardinal2 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_spaced_cardinal3()
 *
 * Description:   Converts a 3-byte number into a spaced unsigned decimal
 *                string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xE3.
 */

extern os_error *xos_convert_spaced_cardinal3 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_spaced_cardinal3 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_spaced_cardinal4()
 *
 * Description:   Converts a 4-byte number into a spaced unsigned decimal
 *                string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xE4.
 */

extern os_error *xos_convert_spaced_cardinal4 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_spaced_cardinal4 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_spaced_integer1()
 *
 * Description:   Converts a 1-byte number into a spaced signed decimal
 *                string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xE5.
 */

extern os_error *xos_convert_spaced_integer1 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_spaced_integer1 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_spaced_integer2()
 *
 * Description:   Converts a 2-byte number into a spaced signed decimal
 *                string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xE6.
 */

extern os_error *xos_convert_spaced_integer2 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_spaced_integer2 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_spaced_integer3()
 *
 * Description:   Converts a 3-byte number into a spaced signed decimal
 *                string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xE7.
 */

extern os_error *xos_convert_spaced_integer3 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_spaced_integer3 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_spaced_integer4()
 *
 * Description:   Converts a 4-byte number into a spaced signed decimal
 *                string
 *
 * Input:         value - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xE8.
 */

extern os_error *xos_convert_spaced_integer4 (int value,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_spaced_integer4 (int value,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_fixed_net_station()
 *
 * Description:   Converts from an Econet station/network number pair to a
 *                string of fixed length
 *
 * Input:         station_number - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xE9.
 */

extern os_error *xos_convert_fixed_net_station (os_station_number const *station_number,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_fixed_net_station (os_station_number const *station_number,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_net_station()
 *
 * Description:   Converts from an Econet station/network number pair to a
 *                string
 *
 * Input:         station_number - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xEA.
 */

extern os_error *xos_convert_net_station (os_station_number const *station_number,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_net_station (os_station_number const *station_number,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_fixed_file_size()
 *
 * Description:   Converts an integer into a filesize string of fixed
 *                length
 *
 * Input:         file_size - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xEB.
 */

extern os_error *xos_convert_fixed_file_size (int file_size,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_fixed_file_size (int file_size,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_convert_file_size()
 *
 * Description:   Converts an integer into a filesize string
 *
 * Input:         file_size - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0xEC.
 */

extern os_error *xos_convert_file_size (int file_size,
      char *buffer,
      int size,
      char **end);
extern char *os_convert_file_size (int file_size,
      char *buffer,
      int size);

/* ------------------------------------------------------------------------
 * Function:      os_null()
 *
 * Description:   VDU command for nul (no operation)
 *
 * Other notes:   Calls SWI 0x100.
 */

extern os_error *xos_null (void);
__swi (0x100) void os_null (void);

/* ------------------------------------------------------------------------
 * Function:      os_char_to_printer()
 *
 * Description:   VDU command to send the next character to the printer
 *                only - must be followed by 'char'
 *
 * Other notes:   Calls SWI 0x101.
 */

extern os_error *xos_char_to_printer (void);
__swi (0x101) void os_char_to_printer (void);

/* ------------------------------------------------------------------------
 * Function:      os_printer_on()
 *
 * Description:   VDU command to enable the printer stream - prefer PDriver
 *
 * Other notes:   Calls SWI 0x102.
 */

extern os_error *xos_printer_on (void);
__swi (0x102) void os_printer_on (void);

/* ------------------------------------------------------------------------
 * Function:      os_printer_off()
 *
 * Description:   VDU command to disable the printer stream - prefer
 *                PDriver
 *
 * Other notes:   Calls SWI 0x103.
 */

extern os_error *xos_printer_off (void);
__swi (0x103) void os_printer_off (void);

/* ------------------------------------------------------------------------
 * Function:      os_split_cursors()
 *
 * Description:   VDU command to split the text and graphics cursors
 *
 * Other notes:   Calls SWI 0x104.
 */

extern os_error *xos_split_cursors (void);
__swi (0x104) void os_split_cursors (void);

/* ------------------------------------------------------------------------
 * Function:      os_join_cursors()
 *
 * Description:   VDU command to join the text and graphics cursors
 *
 * Other notes:   Calls SWI 0x105.
 */

extern os_error *xos_join_cursors (void);
__swi (0x105) void os_join_cursors (void);

/* ------------------------------------------------------------------------
 * Function:      os_vdu_on()
 *
 * Description:   VDU command to enable screen output
 *
 * Other notes:   Calls SWI 0x106.
 */

extern os_error *xos_vdu_on (void);
__swi (0x106) void os_vdu_on (void);

/* ------------------------------------------------------------------------
 * Function:      os_bell()
 *
 * Description:   VDU command to ring the bell
 *
 * Other notes:   Calls SWI 0x107.
 */

extern os_error *xos_bell (void);
__swi (0x107) void os_bell (void);

/* ------------------------------------------------------------------------
 * Function:      os_backspace()
 *
 * Description:   VDU command to perform a backspace
 *
 * Other notes:   Calls SWI 0x108.
 */

extern os_error *xos_backspace (void);
__swi (0x108) void os_backspace (void);

/* ------------------------------------------------------------------------
 * Function:      os_tab()
 *
 * Description:   VDU command to perform a horizontal tab
 *
 * Other notes:   Calls SWI 0x109.
 */

extern os_error *xos_tab (void);
__swi (0x109) void os_tab (void);

/* ------------------------------------------------------------------------
 * Function:      os_line_feed()
 *
 * Description:   VDU command to perform a linefeed
 *
 * Other notes:   Calls SWI 0x10A.
 */

extern os_error *xos_line_feed (void);
__swi (0x10A) void os_line_feed (void);

/* ------------------------------------------------------------------------
 * Function:      os_vertical_tab()
 *
 * Description:   VDU command to perform a vertical tab
 *
 * Other notes:   Calls SWI 0x10B.
 */

extern os_error *xos_vertical_tab (void);
__swi (0x10B) void os_vertical_tab (void);

/* ------------------------------------------------------------------------
 * Function:      os_cls()
 *
 * Description:   VDU command to clear the text window
 *
 * Other notes:   Calls SWI 0x10C.
 */

extern os_error *xos_cls (void);
__swi (0x10C) void os_cls (void);

/* ------------------------------------------------------------------------
 * Function:      os_return()
 *
 * Description:   VDU command to perform a return
 *
 * Other notes:   Calls SWI 0x10D.
 */

extern os_error *xos_return (void);
__swi (0x10D) void os_return (void);

/* ------------------------------------------------------------------------
 * Function:      os_page_mode_on()
 *
 * Description:   VDU command to turn paged mode on
 *
 * Other notes:   Calls SWI 0x10E.
 */

extern os_error *xos_page_mode_on (void);
__swi (0x10E) void os_page_mode_on (void);

/* ------------------------------------------------------------------------
 * Function:      os_page_mode_off()
 *
 * Description:   VDU command to turn paged mode off
 *
 * Other notes:   Calls SWI 0x10F.
 */

extern os_error *xos_page_mode_off (void);
__swi (0x10F) void os_page_mode_off (void);

/* ------------------------------------------------------------------------
 * Function:      os_clg()
 *
 * Description:   VDU command to clear the graphics window
 *
 * Other notes:   Calls SWI 0x110.
 */

extern os_error *xos_clg (void);
__swi (0x110) void os_clg (void);

/* ------------------------------------------------------------------------
 * Function:      os_set_text_colour()
 *
 * Description:   VDU command to set the text colour - must be followed by
 *                'colour' - prefer OS_SetColour
 *
 * Other notes:   Calls SWI 0x111.
 */

extern os_error *xos_set_text_colour (void);
__swi (0x111) void os_set_text_colour (void);

/* ------------------------------------------------------------------------
 * Function:      os_set_gcol()
 *
 * Description:   VDU command to set the graphics colour and action - must
 *                be followed by 'action,' 'colour' - prefer OS_SetColour
 *
 * Other notes:   Calls SWI 0x112.
 */

extern os_error *xos_set_gcol (void);
__swi (0x112) void os_set_gcol (void);

/* ------------------------------------------------------------------------
 * Function:      os_set_palette()
 *
 * Description:   VDU command to set the palette - must be followed by
 *                'colour,' 'mode,' 'red,' 'green,' 'blue'
 *
 * Other notes:   Calls SWI 0x113.
 */

extern os_error *xos_set_palette (void);
__swi (0x113) void os_set_palette (void);

/* ------------------------------------------------------------------------
 * Function:      os_reset_colours()
 *
 * Description:   VDU command to restore default colours
 *
 * Other notes:   Calls SWI 0x114.
 */

extern os_error *xos_reset_colours (void);
__swi (0x114) void os_reset_colours (void);

/* ------------------------------------------------------------------------
 * Function:      os_vdu_off()
 *
 * Description:   VDU command to disable screen output
 *
 * Other notes:   Calls SWI 0x115.
 */

extern os_error *xos_vdu_off (void);
__swi (0x115) void os_vdu_off (void);

/* ------------------------------------------------------------------------
 * Function:      os_set_mode()
 *
 * Description:   VDU command to change display mode - must be followed by
 *                'mode' - prefer OS_ScreenMode
 *
 * Other notes:   Calls SWI 0x116.
 */

extern os_error *xos_set_mode (void);
__swi (0x116) void os_set_mode (void);

/* ------------------------------------------------------------------------
 * Function:      os_misc()
 *
 * Description:   Miscellaneous VDU commands - must be followed by
 *                'command' and 8 more arguments
 *
 * Other notes:   Calls SWI 0x117.
 */

extern os_error *xos_misc (void);
__swi (0x117) void os_misc (void);

/* ------------------------------------------------------------------------
 * Function:      os_set_graphics_window()
 *
 * Description:   VDU command to define the graphics window - must be
 *                followed by 'x0,' 'y0,' 'x1,' 'y1' (2 bytes each)
 *
 * Other notes:   Calls SWI 0x118.
 */

extern os_error *xos_set_graphics_window (void);
__swi (0x118) void os_set_graphics_window (void);

/* ------------------------------------------------------------------------
 * Function:      os_plot_vdu()
 *
 * Description:   General VDU plot commands - must be followed by
 *                'plot_type' and 'x,' 'y' (2 bytes each) - prefer OS_Plot
 *
 * Other notes:   Calls SWI 0x119.
 */

extern os_error *xos_plot_vdu (void);
__swi (0x119) void os_plot_vdu (void);

/* ------------------------------------------------------------------------
 * Function:      os_reset_windows()
 *
 * Description:   VDU command to restore default windows
 *
 * Other notes:   Calls SWI 0x11A.
 */

extern os_error *xos_reset_windows (void);
__swi (0x11A) void os_reset_windows (void);

/* ------------------------------------------------------------------------
 * Function:      os_escape()
 *
 * Description:   VDU command for escape (no operation)
 *
 * Other notes:   Calls SWI 0x11B.
 */

extern os_error *xos_escape (void);
__swi (0x11B) void os_escape (void);

/* ------------------------------------------------------------------------
 * Function:      os_set_text_window()
 *
 * Description:   VDU command to define the text window - must be followed
 *                by 'x0,' 'y0,' 'x1,' 'y1'
 *
 * Other notes:   Calls SWI 0x11C.
 */

extern os_error *xos_set_text_window (void);
__swi (0x11C) void os_set_text_window (void);

/* ------------------------------------------------------------------------
 * Function:      os_set_graphics_origin()
 *
 * Description:   VDU command to set the graphics origin - must be followed
 *                by 'x,' 'y' (2 bytes each)
 *
 * Other notes:   Calls SWI 0x11D.
 */

extern os_error *xos_set_graphics_origin (void);
__swi (0x11D) void os_set_graphics_origin (void);

/* ------------------------------------------------------------------------
 * Function:      os_home_text_cursor()
 *
 * Description:   VDU command to home the text cursor
 *
 * Other notes:   Calls SWI 0x11E.
 */

extern os_error *xos_home_text_cursor (void);
__swi (0x11E) void os_home_text_cursor (void);

/* ------------------------------------------------------------------------
 * Function:      os_set_text_cursor()
 *
 * Description:   VDU command to position the text cursor - must be
 *                followed by 'x,' 'y'
 *
 * Other notes:   Calls SWI 0x11F.
 */

extern os_error *xos_set_text_cursor (void);
__swi (0x11F) void os_set_text_cursor (void);

/* ------------------------------------------------------------------------
 * Function:      os_space()
 *
 * Description:   VDU command to perform a space
 *
 * Other notes:   Calls SWI 0x120.
 */

extern os_error *xos_space (void);
__swi (0x120) void os_space (void);

/* ------------------------------------------------------------------------
 * Function:      os_delete()
 *
 * Description:   VDU command to perform a delete
 *
 * Other notes:   Calls SWI 0x17F.
 */

extern os_error *xos_delete (void);
__swi (0x17F) void os_delete (void);

/* ------------------------------------------------------------------------
 * Function:      ukvdu23v()
 *
 * Description:   Called when an unrecognised VDU 23 command is issued
 *
 * Input:         n - value of R0 on entry
 *                queue - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x34 with R9 = 0x17.
 */

extern os_error *xukvdu23v (int n,
      byte const *queue);
extern void ukvdu23v (int n,
      byte const *queue);

/* ------------------------------------------------------------------------
 * Function:      vduxv()
 *
 * Description:   Called when VDU output has been redirected
 *
 * Input:         b - value of R0 on entry
 *
 * Other notes:   Calls SWI 0x34 with R9 = 0x1B.
 */

extern os_error *xvduxv (byte b);
extern void vduxv (byte b);

/* ------------------------------------------------------------------------
 * Function:      service_release_fiq()
 *
 * Description:   FIQ released
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0xB.
 */

extern os_error *xservice_release_fiq (void);
extern void service_release_fiq (void);

/* ------------------------------------------------------------------------
 * Function:      service_claim_fiq()
 *
 * Description:   FIQ claimed
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0xC.
 */

extern os_error *xservice_claim_fiq (void);
extern void service_claim_fiq (void);

/* ------------------------------------------------------------------------
 * Function:      service_reset()
 *
 * Description:   Post-reset. Also, request to task modules and filing
 *                system-specific desktop filers to set task_handle
 *                variable to 0
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0x27.
 */

extern os_error *xservice_reset (void);
extern void service_reset (void);

/* ------------------------------------------------------------------------
 * Function:      service_key_handler()
 *
 * Description:   Keyboard handler changed
 *
 * Input:         keyboard_id - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0x44.
 */

extern os_error *xservice_key_handler (int keyboard_id);
extern void service_key_handler (int keyboard_id);

/* ------------------------------------------------------------------------
 * Function:      service_mode_change()
 *
 * Description:   Mode changed
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0x46.
 */

extern os_error *xservice_mode_change (void);
extern void service_mode_change (void);

/* ------------------------------------------------------------------------
 * Function:      service_claim_fiq_in_background()
 *
 * Description:   FIQ claimed in background
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0x47.
 */

extern os_error *xservice_claim_fiq_in_background (void);
extern void service_claim_fiq_in_background (void);

/* ------------------------------------------------------------------------
 * Function:      service_memory_moved()
 *
 * Description:   Memory moved
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0x4E.
 */

extern os_error *xservice_memory_moved (void);
extern void service_memory_moved (void);

/* ------------------------------------------------------------------------
 * Function:      service_pre_mode_change()
 *
 * Description:   Mode change
 *
 * Input:         mode - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0x4D.
 */

extern os_error *xservice_pre_mode_change (os_mode mode);
extern void service_pre_mode_change (os_mode mode);

/* ------------------------------------------------------------------------
 * Function:      service_mode_extension()
 *
 * Description:   Allow soft modes
 *
 * Input:         mode - value of R2 on entry
 *                monitor_type - value of R3 on entry
 *                bandwidth - RISC OS 3.5+
 *                vram_size - RISC OS 3.5+
 *
 * Output:        unclaimed - value of R1 on exit (X version only)
 *                vidc_list - value of R3 on exit
 *                workspace_list - value of R4 on exit
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0x50.
 */

extern os_error *xservice_mode_extension (os_mode mode,
      int monitor_type,
      int bandwidth,
      int vram_size,
      osbool *unclaimed,
      int **vidc_list,
      int **workspace_list);
extern osbool service_mode_extension (os_mode mode,
      int monitor_type,
      int bandwidth,
      int vram_size,
      int **vidc_list,
      int **workspace_list);

/* ------------------------------------------------------------------------
 * Function:      service_mode_translation()
 *
 * Description:   Translate modes for unknown monitor types
 *
 * Input:         mode - value of R2 on entry
 *                monitor_type - value of R3 on entry
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0x51.
 */

extern os_error *xservice_mode_translation (os_mode mode,
      int monitor_type);
extern void service_mode_translation (os_mode mode,
      int monitor_type);

/* ------------------------------------------------------------------------
 * Function:      service_validate_address()
 *
 * Description:   OS_ValidateAddress has been called with an unknown area
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0x6D.
 */

extern os_error *xservice_validate_address (void);
extern void service_validate_address (void);

/* ------------------------------------------------------------------------
 * Function:      service_monitor_lead_translation()
 *
 * Description:   Translate monitor lead ID
 *
 * Input:         lead_id - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0x76.
 */

extern os_error *xservice_monitor_lead_translation (int lead_id);
extern void service_monitor_lead_translation (int lead_id);

/* ------------------------------------------------------------------------
 * Function:      service_mode_changing()
 *
 * Description:   Mode change
 *
 * Input:         mode - value of R2 on entry
 *                monitor_type - value of R3 on entry
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0x89.
 */

extern os_error *xservice_mode_changing (os_mode mode,
      int monitor_type);
extern void service_mode_changing (os_mode mode,
      int monitor_type);

/* ------------------------------------------------------------------------
 * Function:      service_enumerate_screen_modes()
 *
 * Description:   Enumerates the available screen modes
 *
 * Input:         context - value of R2 on entry
 *                monitor_type - value of R3 on entry
 *                bandwidth - value of R4 on entry
 *                vram_size - value of R5 on entry
 *                block - value of R6 on entry
 *                block_size - value of R7 on entry
 *
 * Output:        unclaimed - value of R1 on exit (X version only)
 *                context_out - value of R2 on exit
 *                end - value of R6 on exit
 *                free - value of R7 on exit
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0x8D.
 */

extern os_error *xservice_enumerate_screen_modes (int context,
      int monitor_type,
      int bandwidth,
      int vram_size,
      os_mode_block *block,
      int block_size,
      osbool *unclaimed,
      int *context_out,
      byte **end,
      int *free);
extern osbool service_enumerate_screen_modes (int context,
      int monitor_type,
      int bandwidth,
      int vram_size,
      os_mode_block *block,
      int block_size,
      int *context_out,
      byte **end,
      int *free);

/* ------------------------------------------------------------------------
 * Function:      service_pages_unsafe()
 *
 * Description:   Pages specified are about to be swapped for different
 *                pages - RISC OS 3.5+
 *
 * Input:         page_block - value of R2 on entry
 *                request_count - value of R3 on entry
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0x8E.
 */

extern os_error *xservice_pages_unsafe (os_page_block const *page_block,
      int request_count);
extern void service_pages_unsafe (os_page_block const *page_block,
      int request_count);

/* ------------------------------------------------------------------------
 * Function:      service_pages_safe()
 *
 * Description:   Pages specified have been swapped for different pages -
 *                RISC OS 3.5+
 *
 * Input:         request_count - value of R2 on entry
 *                old_page_block - value of R3 on entry
 *                new_page_block - value of R4 on entry
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0x8F.
 */

extern os_error *xservice_pages_safe (int request_count,
      os_page_block const *old_page_block,
      os_page_block const *new_page_block);
extern void service_pages_safe (int request_count,
      os_page_block const *old_page_block,
      os_page_block const *new_page_block);

/* ------------------------------------------------------------------------
 * Function:      service_dynamic_area_create()
 *
 * Description:   Dynamic area has just been successfully created - RISC O
 *                S 3.5+
 *
 * Input:         area - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0x90.
 */

extern os_error *xservice_dynamic_area_create (int area);
extern void service_dynamic_area_create (int area);

/* ------------------------------------------------------------------------
 * Function:      service_dynamic_area_remove()
 *
 * Description:   Dynamic area is about to be removed - RISC OS 3.5+
 *
 * Input:         area - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0x91.
 */

extern os_error *xservice_dynamic_area_remove (int area);
extern void service_dynamic_area_remove (int area);

/* ------------------------------------------------------------------------
 * Function:      service_dynamic_area_renumber()
 *
 * Description:   Dynamic area is being renumbered - RISC OS 3.5+
 *
 * Input:         area - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x30 with R1 = 0x92.
 */

extern os_error *xservice_dynamic_area_renumber (int area);
extern void service_dynamic_area_renumber (int area);

/* ------------------------------------------------------------------------
 * Function:      event_char_input()
 *
 * Description:   Character input
 *
 * Input:         buffer - value of R1 on entry
 *                b - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x22 with R0 = 0x2.
 */

extern os_error *xevent_char_input (int buffer,
      byte b);
extern void event_char_input (int buffer,
      byte b);

/* ------------------------------------------------------------------------
 * Function:      event_vsync()
 *
 * Description:   Vertical sync
 *
 * Other notes:   Calls SWI 0x22 with R0 = 0x4.
 */

extern os_error *xevent_vsync (void);
extern void event_vsync (void);

/* ------------------------------------------------------------------------
 * Function:      event_interval_timer()
 *
 * Description:   Interval timer
 *
 * Other notes:   Calls SWI 0x22 with R0 = 0x5.
 */

extern os_error *xevent_interval_timer (void);
extern void event_interval_timer (void);

/* ------------------------------------------------------------------------
 * Function:      event_escape()
 *
 * Description:   Escape
 *
 * Other notes:   Calls SWI 0x22 with R0 = 0x6.
 */

extern os_error *xevent_escape (void);
extern void event_escape (void);

/* ------------------------------------------------------------------------
 * Function:      event_mouse_transition()
 *
 * Description:   Mouse button up/down
 *
 * Input:         x - value of R1 on entry
 *                y - value of R2 on entry
 *                state - value of R3 on entry
 *                t - value of R4 on entry
 *
 * Other notes:   Calls SWI 0x22 with R0 = 0xA.
 */

extern os_error *xevent_mouse_transition (int x,
      int y,
      bits state,
      os_t t);
extern void event_mouse_transition (int x,
      int y,
      bits state,
      os_t t);

/* ------------------------------------------------------------------------
 * Function:      event_key_transition()
 *
 * Description:   Key up/down
 *
 * Input:         down - value of R1 on entry
 *                key - value of R2 on entry
 *                keyboard_id - value of R3 on entry
 *
 * Other notes:   Calls SWI 0x22 with R0 = 0xB.
 */

extern os_error *xevent_key_transition (osbool down,
      int key,
      int keyboard_id);
extern void event_key_transition (osbool down,
      int key,
      int keyboard_id);

/* ------------------------------------------------------------------------
 * Function:      event_pointer_scroll()
 *
 * Description:   Pointer scroll - RISC OS 4.32+
 *
 * Input:         xmove - value of R2 on entry
 *                ymove - value of R3 on entry
 *
 * Other notes:   Calls SWI 0x22 with R0 = 0x15, R1 = 0x4.
 */

extern os_error *xevent_pointer_scroll (int xmove,
      int ymove);
extern void event_pointer_scroll (int xmove,
      int ymove);

/* ------------------------------------------------------------------------
 * Function:      upcall_move_memory_request()
 *
 * Description:   Informs your program that RISC OS would like to move
 *                memory
 *
 * Input:         size_change - value of R1 on entry
 *
 * Output:        not_moved - value of R0 on exit (X version only)
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x33 with R0 = 0x101.
 */

extern os_error *xupcall_move_memory_request (int size_change,
      osbool *not_moved);
extern osbool upcall_move_memory_request (int size_change);

#ifdef __cplusplus
   }
#endif


#else /* EXECUTE_ON_UNIX */

#define xos_writec(c) \
   (os_error *)_swix (OS_WriteC, _IN (0), (int) (c))

#define os_writec(c) \
   _swi (OS_WriteC, _IN (0), (int) (c))

#define xos_write0(s) \
   (os_error *)_swix (OS_Write0, _IN (0), (char *) (s))

#define os_write0(s) \
   _swi (OS_Write0, _IN (0), (char *) (s))

#define xos_new_line() \
   (os_error *)_swix (OS_NewLine, 0)

#define os_new_line() \
   _swi (OS_NewLine, 0)

#define xos_readc(c, flags) \
   (os_error *)_swix (OS_ReadC, _OUT (0) | _OUT (_FLAGS), \
         (int *) (c), (int *) (flags))

#define xos_cli(s) \
   (os_error *)_swix (OS_CLI, _IN (0), (char *) (s))

#define xos_byte(reason_code, r1, r2, r1_out, r2_out) \
   (os_error *)_swix (OS_Byte, _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (reason_code), (int) (r1), (int) (r2), \
         (int *) (r1_out), (int *) (r2_out))

#define xos_word(reason_code, block) \
   (os_error *)_swix (OS_Word, _IN (0) | _IN (1), (int) (reason_code), (int *) (block))

/*OS_File: see "os/file.h"*/

/*OS_Args: see "os/args.h"*/

/*OS_GBPB: see "os/gbpb.h"*/

/*OS_Find: see "os/find.h"*/

#define xos_read_line() \
   (os_error *)_swix (OS_ReadLine, )

#define xos_control() \
   (os_error *)_swix (OS_Control, )

#define xos_get_env(tail, ram_limit, start_time) \
   (os_error *)_swix (OS_GetEnv, _OUT (0) | _OUT (1) | _OUT (2), \
         (char **) (tail), (int *) (ram_limit), (os_clock **) (start_time))

#define xos_exit(error, rc) \
   (os_error *)_swix (OS_Exit, _IN (0) | _IN (1) | _IN (2), \
         (os_error *) (error), *(int *) "ABEX", (int) (rc))

#define xos_set_env() \
   (os_error *)_swix (OS_SetEnv, )

#define xos_int_on() \
   (os_error *)_swix (OS_IntOn, 0)

#define xos_int_off() \
   (os_error *)_swix (OS_IntOff, 0)

#define xos_call_back() \
   (os_error *)_swix (OS_CallBack, )

#define xos_enter_os() \
   (os_error *)_swix (OS_EnterOS, 0)

#define xos_break_pt() \
   (os_error *)_swix (OS_BreakPt, )

#define xos_break_ctrl() \
   (os_error *)_swix (OS_BreakCtrl, )

#define xos_unused_swi() \
   (os_error *)_swix (OS_UnusedSWI, )

#define xos_update_memc() \
   (os_error *)_swix (OS_UpdateMEMC, )

#define xos_set_call_back() \
   (os_error *)_swix (OS_setCallBack, )

#define xos_mouse() \
   (os_error *)_swix (OS_Mouse, )

#define xos_heap() \
   (os_error *)_swix (OS_Heap, )

/*OS_Module: see "os/module.h"*/

#define xos_claim() \
   (os_error *)_swix (OS_Claim, )

#define xos_release() \
   (os_error *)_swix (OS_Release, )

#define xos_read_unsigned() \
   (os_error *)_swix (OS_ReadUnsigned, )

#define xos_generate_event() \
   (os_error *)_swix (OS_GenerateEvent, )

#define xos_read_var_val(var, value, len, context, var_type, len_out, \
      context_out, var_type_out) \
   (os_error *)_swix (OS_ReadVarVal, _IN (0) | _IN (1) | _IN (2) | _IN (3) | _IN (4) | \
         _OUT (2) | _OUT (3) | _OUT (4), \
         (char *) (var), (char *) (value), (int) (len), \
         (int) (context), (int) (var_type), (int *) (len_out), \
         (int *) (context_out), (int *) (var_type_out))
   /*see also "os/vartype.h"*/

#define xos_set_var_val() \
   (os_error *)_swix (OS_SetVarVal, )

   /*see also "os/vartype.h"*/

#define xos_gs_init(gs, flags, context1, context2) \
   (os_error *)_swix (OS_GSInit, _IN (0) | _IN (2) | _OUT (0) | _OUT (2), \
         (char *) (gs), (int) (flags), (int *) (context1), (int *) (context2))

#define xos_gs_read(context1, context2, c, flags) \
   (os_error *)_swix (OS_GSRead, \
         _IN (0) | _IN (2) | _OUT (0) | _OUT (1) | _OUT (2) | _OUT (_FLAGS), \
         *(int *) (context1), *(int *) (context2), \
         (int *) (context1), (char *) (c), (int *) (context2), (int *) (flags))

#define xos_gs_trans(gs, buffer, size, end, count, flags) \
   (os_error *)_swix (OS_GSTrans, \
         _IN (0) | _IN (1) | _IN (2) | _OUT (0) | _OUT (2) | _OUT (_FLAGS), \
         (char *) (gs), (char *) (buffer), (int) (size), (char *) (end), \
         (int *) (count), (int *) (flags))

#define xos_binary_to_decimal(i, buffer, size, count) \
   (os_error *)_swix (OS_BinaryToDecimal, _IN (0) | _IN (1) | _IN (2) | _OUT (2), \
         (int) (i), (char *) (buffer), (int) (size), (int *) (count))

/*OS_FSControl: see "os/fscontrol.h" */

#define xos_change_dynamic_area() \
   (os_error *)_swix (OS_change_dynamic_area, )

#define os_generate_error(error) \
   _swi (OS_GenerateError, _IN (0), (os_error *) (error))

#define xos_read_escape_state() \
   (os_error *)_swix (OS_read_escape_state, )

#define xos_evaluate_expression() \
   (os_error *)_swix (OS_evaluate_expression, )

/*OS_SpriteOp: see "os/spriteop.h"*/

#define xos_read_palette() \
   (os_error *)_swix (OS_read_palette, )

/*OS_ServiceCall: see "os/service.h"*/

#define xos_read_vdu_variables() \
   (os_error *)_swix (OS_read_vdu_variables, )

#define xos_read_point() \
   (os_error *)_swix (OS_read_point, )

#define xos_up_call() \
   (os_error *)_swix (OS_UpCall, )

#define xos_call_a_vector() \
   (os_error *)_swix (OS_call_a_vector, )

#define xos_read_mode_variable(mode, var_no, var_val, flags) \
   (os_error *)_swix (OS_ReadModeVariable, _IN (0) | _IN (1) | _OUT (2) | _OUT (_FLAGS), \
         (int) (mode), (int) (var_no), (int *) (var_val), (int *) (flags))
   /*See modevar.h for variable numbers*/

#define xos_remove_cursors() \
   (os_error *)_swix (OS_remove_cursors, )

#define xos_restore_cursors() \
   (os_error *)_swix (OS_restore_cursors, )

#define xos_swi_number_to_string(no, buffer, len, len_out) \
   (os_error *)_swix (OS_SWINumberToString, _IN (0) | _IN (1) | _IN (2) | _OUT (2), \
         (int) (no), (char *) (buffer), (int) (len), (int *) (len_out))

#define xos_swi_number_from_string(name, no) \
   (os_error *)_swix (OS_SWINumberFromString, _IN (1) | _OUT (0), \
         (char *) (name), (int *) (no))

#define xos_validate_address() \
   (os_error *)_swix (OS_validate_address, )

#define xos_call_after() \
   (os_error *)_swix (OS_call_after, )

#define xos_call_every() \
   (os_error *)_swix (OS_call_every, )

#define xos_remove_ticker_event() \
   (os_error *)_swix (OS_remove_ticker_event, )

#define xos_install_key_handler() \
   (os_error *)_swix (OS_install_key_handler, )

#define xos_check_mode_valid() \
   (os_error *)_swix (OS_check_mode_valid, )

#define xos_change_environment() \
   (os_error *)_swix (OS_change_environment, )

#define xos_claim_screen_memory() \
   (os_error *)_swix (OS_claim_screen_memory, )

#define xos_read_monotonic_time(time) \
   (os_error *)_swix (OS_ReadMonotonicTime, _OUT (0), (os_t *) (time))

#define xos_substitute_args(args, buffer, size, template, template_size, \
      size_out) \
   (os_error *)_swix (OS_SubstituteArgs, \
         _IN (0) | _IN (1) | _IN (2) | _IN (3) | _IN (4) | _OUT (2), \
         (char *) (args), (char *) (buffer), (int) (size), (char *) (template), \
         (int) (template_len), (int *) (size_out))

#define xos_pretty_print(s, dict, special) \
   (os_error *)_swix (OS_PrettyPrint, _IN (0) | _IN (1) | _IN (2), \
         (char *) (s), (void *) (dict), (char *) (special))

#define xos_plot() \
   (os_error *)_swix (OS_plot, )

#define xos_writen(s, n) \
   (os_error *)_swix (OS_WriteN, _IN (0) | _IN (1), (char *) (s), (int) (n))

#define xos_add_to_vector() \
   (os_error *)_swix (OS_add_to_vector, )

#define xos_write_env() \
   (os_error *)_swix (OS_write_env, )

#define xos_read_args(keywords, input, output, size, size_left) \
   (os_error *)_swix (OS_ReadArgs, _IN (0) | _IN (1) | _IN (2) | _IN (3) | _OUT (3), \
         (char *) (keywords), (char *) (input), (char *) (output), \
         (int) (size), (int *) (size_left))

#define xos_read_ramfs_limits() \
   (os_error *)_swix (OS_read_ramfs_limits, )

#define xos_claim_device_vector() \
   (os_error *)_swix (OS_claim_device_vector, )

#define xos_release_device_vector() \
   (os_error *)_swix (OS_release_device_vector, )

#define xos_delink_application() \
   (os_error *)_swix (OS_delink_application, )

#define xos_relink_application() \
   (os_error *)_swix (OS_relink_application, )

#define xos_heap_sort() \
   (os_error *)_swix (OS_heap_sort, )

#define xos_exit_and_die(error, rc, module_name) \
   (os_error *)_swix (OS_ExitAndDie, _IN (0) | _IN (1) | _IN (2) | _IN (3), \
         (os_error *) (error), *(int *) "ABEX", (int) (rc), \
         (char *) (module_naem))

#define xos_read_mem_map_info(page_size, no_pages) \
   (os_error *)_swix (OS_ReadMemMapInfo, _OUT (0) | _OUT (1), \
         (int *) (page_size), (int *) (no_pages))

#define xos_read_mem_map_entries() \
   (os_error *)_swix (OS_read_mem_map_entries, )

#define xos_set_mem_map_entries() \
   (os_error *)_swix (OS_set_mem_map_entries, )

#define xos_add_call_back() \
   (os_error *)_swix (OS_add_call_back, )

#define xos_read_default_handler() \
   (os_error *)_swix (OS_read_default_handler, )

#define xos_set_ecf_origin() \
   (os_error *)_swix (OS_set_ecf_origin, )

#define xos_serial_op() \
   (os_error *)_swix (OS_serial_op, )

#define xos_read_sys_info() \
   (os_error *)_swix (OS_ReadSysInfo, )

#define xos_confirm() \
   (os_error *)_swix (OS_confirm, )

#define xos_changed_box() \
   (os_error *)_swix (OS_changed_box, )

#define xos_crc() \
   (os_error *)_swix (OS_crc, )

#define xos_read_dynamic_area() \
   (os_error *)_swix (OS_read_dynamic_area, )

#define xos_print_char() \
   (os_error *)_swix (OS_print_char, )

#define xos_change_redirection() \
   (os_error *)_swix (OS_change_redirection, )

#define xos_remove_call_back() \
   (os_error *)_swix (OS_remove_call_back, )

#define xos_find_mem_map_entries() \
   (os_error *)_swix (OS_find_mem_map_entries, )

#define xos_convert_standard_date_and_time() \
   (os_error *)_swix (OS_ConvertStandardDateAndTime, )

#define xos_convert_date_and_time() \
   (os_error *)_swix (OS_ConvertDateAndTime, )

#define xos_convert_hex1(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertHex1, _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_hex2(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertHex2, _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_hex4(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertHex4, _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_hex6(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertHex6, _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_hex8(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertHex8, _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_cardinal1(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertCardinal1, \
         _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_cardinal2(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertCardinal2, \
         _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_cardinal3(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertCardinal3, \
         _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_cardinal4(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertCardinal4, \
         _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_integer1(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertInteger1, \
         _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_integer2(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertInteger2, \
         _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_integer3(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertInteger3, \
         _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_integer4(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertInteger4, \
         _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_binary1(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertBinary1, _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_binary2(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertBinary2, _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_binary3(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertBinary3, _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_binary4(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertBinary4, _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_spaced_cardinal1(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertSpacedCardinal1, \
         _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_spaced_cardinal2(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertSpacedCardinal2, \
         _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_spaced_cardinal3(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertSpacedCardinal3, \
         _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_spaced_cardinal4(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertSpacedCardinal4, \
         _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_spaced_integer1(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertSpacedInteger1, \
         _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_spaced_integer2(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertSpacedInteger2, \
         _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_spaced_integer3(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertSpacedInteger3, \
         _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_spaced_integer4(value, buffer, size, end, free) \
   (os_error *)_swix (OS_ConvertSpacedInteger4, \
         _IN (0) | _IN (1) | _IN (2) | _OUT (1) | _OUT (2), \
         (int) (value), (char *) (buffer), (int) (size), (char **) (end), \
         (int *) (free))

#define xos_convert_fixed_net_station() \
   (os_error *)_swix (OS_ConvertFixedNetStation, )

#define xos_convert_net_station() \
   (os_error *)_swix (OS_ConvertNetStation, )

#define xos_convert_fixed_file_size() \
   (os_error *)_swix (OS_ConvertFixedFileSize, )

#define xos_convert_file_size() \
   (os_error *)_swix (OS_ConvertFileSize, )

#endif /* EXECUTE_ON_UNIX */

#ifndef oscore32_H
#include "oslib/oscore32.h"
#endif

#endif
