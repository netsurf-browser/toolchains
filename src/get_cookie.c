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
 * $Source: /cvsroot/windom/windom/src/get_cookie.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */

#ifdef __PUREC__
	/* Pure C has non-standard libraries */
	#include <tos.h>
#else
	/* Gnu C, Sozobon, Lattice */
#ifdef __SOZOBONX__
#include <atari.h>  /* MUST be included before osbind.h */
#endif
	#include <osbind.h>
//	#include <fcntl.h>
#endif

#ifdef __MINTLIB__
#include <mint/cookie.h>
#endif

#include "globals.h"


#ifndef __MINTLIB__

/*  This is Getcookie() highly based on the mintlib Getcookie() function
 *  This function is already provided by mintlib, so this is not
 *  compiled if mintlib is detected
 */

#ifndef Ssystem
#define Ssystem(mode,arg1,arg2) gemdos((short)0x154,(short)mode,(long)arg1,(long)arg2) 
#endif

#ifndef S_GETCOOKIE
#define S_GETCOOKIE   8
#endif

int
Getcookie (long cookie, long *p_value)
{
	static int __has_no_ssystem = 0;
	static int initialised = 0;
	
	if (!initialised) {
		__has_no_ssystem = Ssystem(-1,NULL,NULL);
		initialised = 1;
	}
	
	if (__has_no_ssystem)
	{
		/* The Ssystem() is not available and therefore
		 * we assume that the system has no memory protection.
		 * We go directly into the cookie jar. */

		/* old method */
		long *cookieptr = (long *)Supexec (__cookieptr);

		if (cookieptr)
		{
			while (*cookieptr)
			{
				if (*cookieptr == cookie)
				{
					if (p_value)
						*p_value = cookieptr [1];
					
					return 0;
				}
				
				cookieptr += 2;
			}
		}
		if (p_value)
			*p_value = 0;

		return ERROR;
	}
	else
	{
		/* Ssystem supported, use it */
		int	r;
		long	v = -42;

		/* Make sure that P_VALUE is zeroed if the cookie can't
		   be found.  Reported by Tommy Andersen
		   (tommya@post3.tele.dk).  */
		if (p_value)
			*p_value = 0;
			
		r = Ssystem(S_GETCOOKIE, cookie, &v);
		/*
		 * Backward compatibility for MiNT 1.14.7:
		 * Ssystem() returns cookie value and ignores arg2!!
		 */
		if (r != -1 && v == -42)				
			v = r;
 
		if (r == -1)							/* not found */
		{
			v = 0;
			r = ERROR;
		}
		else
			r = 0;

		if (p_value)
			*p_value = v;

		return r;
	}
}
#endif


int get_cookie (long cookie, long *p_value) {
	return Getcookie(cookie,p_value) == 0 ? 1 : 0;
}
