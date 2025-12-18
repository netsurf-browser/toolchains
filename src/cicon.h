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
 * $Source: /cvsroot/windom/windom/src/cicon.h,v $
 * Module : definitions of color icon
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

/*****************************************************************************/
/*                                                                           */
/* Modul: XRSRCFIX.H                                                         */
/* Datum: 19.10.92                                                           */
/*                                                                           */
/*****************************************************************************/

#ifndef __XRSRCFIX__
#define __XRSRCFIX__

/****** DEFINES **************************************************************/

/****** TYPES ****************************************************************/

typedef struct {
	USERBLK	ublk;
	W_PARM  w_parm;
} OBBLK;

typedef struct _xrsrcfix {
	OBBLK   *obblk;
	CICON   *cicon_table;
	WORD    farbtbl[256][32];
	ULONG   farbtbl2[256];
	WORD    is_palette;
	INT16   rgb_palette[256][4];
	WORD 	num_cib;
} XRSRCFIX;

/****** VARIABLES ************************************************************/

/****** FUNCTIONS ************************************************************/

#endif /* __XRSRCFIX__ */

