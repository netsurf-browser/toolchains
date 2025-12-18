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
 * $Source: /cvsroot/windom/windom/src/wportab.h,v $
 * Module : WinDom portab file
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.4 $
 */


#ifndef __WPORTAB_H__
#define __WPORTAB_H__

#ifndef __PORTAB_H__
#define __PORTAB_H__

/* Compiler */

#ifdef __PUREC__
# define __MSHORT__		/* int have a 16-bit size */
# define __CDECL cdecl
# define __EXTERN extern
# define __PROTO(a) a
#else
# ifndef _COMPILER_H_
#  include <compiler.h>
# endif
#endif


/* Type */

#ifdef __PUREC__
# define INT16 int
#else
# define INT16 short
#endif

#define BYTE 	char
#define UBYTE	unsigned char
#define WORD 	INT16
#define UWORD	unsigned INT16
#define LONG	long
#define	ULONG	unsigned long
#define VOID	void
#define VOIDP	void*

/* Special keyword */
#define EXTERN	__EXTERN

/* Constants */
#define TRUE	1
#define FALSE	0

#endif /* __PORTAB_H__ */
#endif /* __WPORTAB_H__ */
