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
 * $Source: /cvsroot/windom/windom/userdef/extended/interface.h,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:52:21 $
 *   $Revision: 1.3 $
 */

/** @addtogroup UserdefAPI 
 *  @{
 */

long   __CDECL xobjc_libinit( APPENV *);
void   __CDECL xobjc_udconfig( UDLIB_CONFIG *, APPENV *);
void   __CDECL xobjc_libexit( APPENV *);
long   __CDECL xobjc_extended( OBJECT *, long , long , APPENV *);
void   __CDECL xobjc_extfree( OBJECT *, long , APPENV *);
char * __CDECL xobjc_string( OBJECT *, long , char *, APPENV *);
long   __CDECL xobjc_char( OBJECT *, long , long , APPENV *);
long   __CDECL xobjc_edit( OBJECT *, long , long , short* , long , long , GRECT *, APPENV *);
long   __CDECL xobjc_get_edpos( OBJECT *, long , long , long , APPENV *);
long   __CDECL xobjc_get_unextended( OBJECT *, long , APPENV *);

/** @} */
