/*
 * WinDom: a high level GEM library
 * Copyright (c) 1997-2006 windom authors (see AUTHORS file)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * $Source: /cvsroot/windom/windom/include/windom/udlib.h,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:46:25 $
 *   $Revision: 1.6 $
 */

#ifndef _UDLIB_H_
#define _UDLIB_H_

#ifndef _MT_GEMLIB_H_
#include <mt_gem.h>  /* for OBJECT */
#endif

#ifndef __MT_WINDOM__
#include <mt_wndm.h>    /* for ATTRIB */
#endif

__BEGIN_DECLS

/** @addtogroup UserdefAPI
 *  @{ 
 */

/** standard windom configuration data 
 *  see windom _config:: documentation for details 
 */
typedef struct {
	long	size_of_this;
	short	key_color;
	ATTRIB	string;
	ATTRIB	button;
	ATTRIB	exit;
	ATTRIB	title;
	ATTRIB	xedit_text;
	ATTRIB	xedit_label;
	short	xedit_curs_color;
	ATTRIB	xlongedit;
	short	xlgedt_smlfnt;
	ATTRIB  xtedinfo;
	short	xtdinf_smlfnt;
	short	actmono;
	short	actcol;
} UDLIB_CONFIG;

/** structure to access windom CICON subroutine from
 *  a userdef library
 */ 
typedef struct {
	OBJECT *rs_object;
	short num_obs;
	short num_cib;
	void *rgb_palette;
	void *fix;
	short __CDECL (*draw_cicon)( long, OBJECT*, long, long, long, long);
} UDLIB_FIXCICON;

/** environment data set as paramter to all functions of the userdef library
 */
typedef struct {
	short * aes_global;
	short	vdih;
	short	nplanes;
	void * __CDECL (* app_malloc)(long size);
	void   __CDECL (* app_free)( void *addr);
	void *  udlib_data;
	UDLIB_FIXCICON fixcicon;
} APPENV;

/** @} */

__END_DECLS

#endif /* _UDLIB_H_ */
