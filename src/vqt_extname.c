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
 * $Source: /cvsroot/windom/windom/src/vqt_extname.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include "globals.h"
#include "cookie.h"

/**  utility function based on NVDI4 vqt_ext_name binding
 *
 *  @param handle,index,name,format,flags see VDI function vqt_ext_name()
 *  @param speedo will contain the flag \a name[32] returned by the VDI function vqt_ext_name()
 *  @return see VDI function vqt_ext_name()
 *
 *  the main change is the extra parameter speedo will contain
 *  \a name[32], and \a name[32] will be zero-ed (so that \a name[] is now a
 *  standard C string with a null byte at the end)
 */

int vqt_extname( int handle, int index, char *name, 
				 int *speedo, INT16 *format, INT16 *flags ) {
	int res;

	res = vqt_ext_name ( handle, index, name, format, flags);	
	*speedo = name[32];
	name[32] = '\0';
	
  	return( res);
}

