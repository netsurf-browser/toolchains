/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * Definition of mem.ldg calls, this file should be included by 
 * client applications
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
 * $Id: mem.h 6 2005-07-25 19:13:10Z bercegeay $
 */

#include <ldg.h>

#define ALLOCMODE_GLOBAL   0x1
#define ALLOCMODE_STRAM    0x2


extern void* __CDECL (*_ldg_malloc)			( long size, long context);
extern void* __CDECL (*_ldg_calloc)			( long num, long size, long context);
extern void* __CDECL (*_ldg_realloc)		( void *adr, long size, long context);
extern void  __CDECL (*_ldg_free)			( void* addr, long context);
extern void  __CDECL (*_ldg_mallocczero)	( long yes, long context);
extern void  __CDECL (*_ldg_mallocChunkSize)( long size, long context);
extern char* __CDECL (*_ldg_strdup)			( char *src, long context);
extern char* __CDECL (*_ldg_allocmode)		( long mode, long context);

#define ldg_malloc(s) _ldg_malloc(s,ldg_global[2])
#define ldg_calloc(s) _ldg_calloc(s,ldg_global[2])
#define ldg_realloc(s) _ldg_realloc(s,ldg_global[2])
#define ldg_free(s) _ldg_free(s,ldg_global[2])
#define ldg_mallocczero(s) _ldg_mallocczero(s,ldg_global[2])
#define ldg_mallocChunkSize(s) _ldg_mallocChunkSize(s,ldg_global[2])
#define ldg_strdup(s) _ldg_strdup(s,ldg_global[2])
#define ldg_allocmode(s) _ldg_allocmode(s,ldg_global[2])

int ldg_mem_init( char *lib, int vers, LDG **mem);

