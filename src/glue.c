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
 * $Source: /cvsroot/windom/windom/src/glue.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.10 $
 */

#include <stdlib.h>
#include <stdarg.h>
#include "globals.h"

#ifdef __PUREC__
#include "..\include\windom.h"
#else
#include "../include/windom.h"
#endif

#include <gem.h>

APPvar *gl_appvar = NULL;

/**
 * @brief AES and WinDom initialization.
 * @return AES application descriptor.
 *
 * This function is the version single threaded of mt_ApplInit().
 *
 * @sa ApplExit(), mt_ApplInit()
 */

int ApplInit(void) {
	gl_appvar= __mt_ApplInit(aes_global);
	if (gl_appvar == NULL) return -1;
	gl_apid = gl_appvar->aes_global[2];
	return gl_appvar->aes_global[2];
}

/**
 * @brief Terminate a WinDom and AES session.
 * @return 0 or a negative code error.
 *
 * This function is the version single threaded of mt_ApplExit().
 *
 * @sa ApplInit(), mt_ApplExit()
 */

int ApplExit(void) {
	return __mt_ApplExit(gl_appvar);
}

int  ConfInquire ( char *keyword, char *fmt, ...) {
 	va_list args;
	int res;
	
 	va_start( args, fmt);
	res = mt_vConfInquire( gl_appvar, keyword, fmt, args);
 	va_end( args);
	
	return res;
}

int  ConfWrite   ( char *keyword, char *fmt, ...) {
 	va_list args;
	int res;
	
 	va_start( args, fmt);
	res = mt_vConfWrite( gl_appvar, keyword, fmt, args);
 	va_end( args);
	
	return res;
}

int FormAlert( int but, char fmt[], ...) {
	va_list list;
	int res;
	
	va_start( list, fmt);
	res = mt_vFormAlert( gl_appvar, but, fmt, list);
	va_end( list);
	
	return res;
}

/** see mt_FrameSet() */
void    FrameSet    ( WINDOW *win, int mode, ...) {
	long a,b,c,d;
 	va_list args;
 	va_start( args, mode);
	a = va_arg( args, long);
 	b = va_arg( args, long);
	c = va_arg( args, long);
	d = va_arg( args, long);
 	va_end( args);
	mt_FrameSet(gl_appvar,win,mode,a,b,c,d);
}

/** see mt_FrameGet() */
void 	FrameGet    ( WINDOW *win, int mode, ...) {
	long a,b,c,d;
 	va_list args;
 	va_start( args, mode);
	a = va_arg( args, long);
 	b = va_arg( args, long);
	c = va_arg( args, long);
	d = va_arg( args, long);
 	va_end( args);
	mt_FrameGet(gl_appvar,win,mode,a,b,c,d);
}

/** see mt_WindFind() */
WINDOW* WindFind    ( int mode, ...) {
	long a,b,c,d;
 	va_list args;
 	va_start( args, mode);
	a = va_arg( args, long);
 	b = va_arg( args, long);
	c = va_arg( args, long);
	d = va_arg( args, long);
 	va_end( args);
	return mt_WindFind(gl_appvar,mode,a,b,c,d);
}

void __CDECL stdFormClose( WINDOW *win, int index, int mode, void *data) {
	mt_stdFormClose(win,index,mode,data,gl_appvar);
}

void __CDECL stdWindClear( WINDOW *win, short buff[8]) {
	mt_stdWindClear(win,buff,gl_appvar);
}
