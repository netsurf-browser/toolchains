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
 * $Source: /cvsroot/windom/windom/src/conv_path.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include "globals.h"

/** convert a file name between TOS and MiNT formats
 *
 *  @param p buffer containing the path to convert
 *  @return \a p
 *
 *	conv_path() converts a TOS filename o pathname in MiNT (Unix) format
 *	and inversly. Absolute or relative path are converted. Concerning
 *	the TOS convertion (MiNT to TOS), the root path (/ in Unix) is
 *	converted to U:\\ except disk paths (/x/) which are converted
 *	into x:\\ format.
 *  
 *  Some examples:
 *	- \c folder\\dum.cnf is converted in \c folder/dum.cnf
 *	- \c /c/multitos/mint.cnf" is converted in \c c:\\multitos\\mint.cnf
 *	- \c /etc/passwd" is converted to \c U:\\etc\\passwd 
 *	- \c /u/etc/passwd" is converted in \c u:\\etc\\passwd
 *	- \c /etc/passwd" is converted in \c U:\\etc\\passwd
 *
 */

 char *conv_path( char *p) {
	char *q;
	
	/* Transform root of absolute path */
	if( p[1] == ':') {
		p[1] = p[0];
		p[0] = '\\';
	} else if( p[0] == '/') {
		if( p[2] == '/') {
			p[0] = p[1];
			p[1] = ':';
		} else {
			/* Add U: behind root dir (/) */
			q = p;
			while( *q++);
			while( q >= p) {
				q[2] = q[0];
				q --;
			}
			p[0] = 'U';
			p[1] = ':';
		}
	}

	/* Transform  / into \ or inversly */
	q = p;
	while( *q) {
		if( *q == '\\') *q= '/';
		else if( *q == '/') *q = '\\';
		q ++;
	}
	return p;
}
