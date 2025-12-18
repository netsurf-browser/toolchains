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
 * $Source: /cvsroot/windom/windom/src/rsrc_ghdr.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */


#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/**
 * @brief return the header of the RSC 
 *
 * @param app application descriptor,
 * @param rsc pointer to a resource descriptor,
 * @return address of resource header, or \b NULL if an error occurs.
 *
 * mt_RsrcGhdr() returns the header of a resource loaded by mt_RsrcXload(). 
 * The header is a rscHDR structure (defined in windom.h)
 * containing some information about the resource.
 * @code
typedef struct {
  long		nobs;	// number of OBJECT items
  long		ntree;	// number of tree OBJECT
  long		nted;	// number of TEDINFO items
  long		ncib;	// number of CICON items
  long		nib;
  long		nbb;
  long		nfstr;  // number of string items
  long		nfimg;  // number of IMAGE items
  OBJECT	*object; // address of OBJECTS
  TEDINFO	*tedinfo; // address of TEDINFO
  ICONBLK	*iconblk; // address of ICONBLK
  BITBLK	*bitblk;  // address of BITBLK
  CICON		*cicon;   // address of CICON
  CICONBLK	*ciconblk; // table of CICONBLK 
  char		**frstr;
  BITBLK	**frimg;
  OBJECT	**trindex;  // address of tree OBJECTS
  } rscHDR;
 *@endcode
 * 
 * @sa mt_RsrcXload()
 */

rscHDR *mt_RsrcGhdr( APPvar *app, void *rsc) {
	return &((RSC *)rsc)->head;
}
