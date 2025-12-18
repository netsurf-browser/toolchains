/*
 * XGEM.LDG: a userdef library for windom
 * Copyright (c) 2005 Arnaud Bercegeay
 *
 * some routines are based on Eric REBOUX's XGEM library
 * Copyright (c)1997-2003 Eric Reboux
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
 * $Source: /cvsroot/windom/windom/userdef/xgem/ldg.c,v $
 * WinDom header file
 *  
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2005/02/09 21:17:48 $
 *   $Revision: 1.1 $
 */

#include <stdlib.h>
#include "global.h"

#include <ldg.h>

PROC Proc[]  = 
{
	{
		"libinit",          
		"/* initialise the library */\n"
		"libinit( APPENV *);",
		(void *)xobjc_libinit
	},
	{
		"udconfig",          
		"/*  */\n"
		"udconfig( UDLIB_CONFIG *, APPENV *);",
		(void *)xobjc_udconfig
	},
	{
		"libexit",          
		"/*  */\n"
		"libexit( APPENV *);",
		(void *)xobjc_libexit
	},
	{
		"extended",          
		"/*  */\n"
		"extended( OBJECT *, long , long , APPENV *);",
		(void *)xobjc_extended
	},
	{
		"extfree",          
		"/*  */\n"
		"extfree( OBJECT *, long , APPENV *);",
		(void *)xobjc_extfree
	},
	{
		"string",          
		"/*  */\n"
		"string( OBJECT *, long , char *, APPENV *);",
		(void *)xobjc_string
	},
	{
		"char",          
		"/*  */\n"
		"char( OBJECT *, long , long , APPENV *);",
		(void *)xobjc_char
	},
	{
		"edit",          
		"/*  */\n"
		"edit( OBJECT *, long , long , short* , long , long , GRECT *, APPENV *);",
		(void *)xobjc_edit
	},
	{
		"get_edpos",          
		"/*  */\n"
		"get_edpos( OBJECT *, long , long , long , APPENV *);",
		(void *)xobjc_get_edpos
	},
	{
		"get_unextended",          
		"/*  */\n"
		"get_unextended( OBJECT *, long , APPENV *);",
		(void *)xobjc_get_unextended
	}
};

char Info[] = "XGEM.LDG: a userdef library for windom\n"
              "version 0.01 (" __DATE__ " " __TIME__ ")"
#ifdef UDEFTOS
			  " (shareable)"
#else
			  " (monoTOS)"
#endif
			  ;

LDGLIB Ldg[] = {{
				0x0001,	/* version of the library */
				10,	  	/* number of functions in the library */
				Proc, 	/* pointer to library functions */
				Info,  	/* "about..." free string */
#ifdef UDEFTOS
				LDG_NOT_SHARED,/* flag of the library */
#else
				0,    	       /* flag of the library */
#endif
				NULL,	/* no "close" function */
				0,      /* no extension */
				}};


int main( void) {
	ldg_init( Ldg);
	return 0;
}
