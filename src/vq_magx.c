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
 * $Source: /cvsroot/windom/windom/src/vq_magx.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include "globals.h"
#include "cookie.h"

/** test if MagiC is present and returns the version number
 *
 *  @return the MagiC version number (from MagiC 3) or 0
 *          if MagiC is not present.
 */

int vq_magx( void) {
	MAGX_INFOS *cv;
	/* Here we don't need to care about MP as MagiC never had one.
	 * So the cv->aesvars IS not OK (no MP)
	 *
	 * And having MAGX cookie in MiNT under MP does not have any
	 * sense and would probably be intended to make some app work
	 * but definitely with no MP.
	 **/
	if( get_cookie( MAGX_COOKIE,(LONG *)&cv) && cv->aesvars)
		return cv->aesvars->version;
	else
		return 0;
}

