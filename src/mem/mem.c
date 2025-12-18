/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * Binding of mem.ldg calls
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
 * $Id: mem.c 9 2005-07-25 19:37:25Z bercegeay $
 */

#include <gem.h>
#ifdef __PUREC__
# include <ldg\mem.h>
#else
# include <ldg/mem.h>
#endif
#include <stdio.h>

void* __CDECL (*_ldg_malloc)          ( long size, long context);
void* __CDECL (*_ldg_calloc)          ( long num, long size, long context);
void* __CDECL (*_ldg_realloc)         ( void *adr, long size, long context);
void  __CDECL (*_ldg_free)            ( void* addr, long context);
void  __CDECL (*_ldg_mallocczero)     ( long yes, long context);
void  __CDECL (*_ldg_mallocChunkSize) ( long size, long context);
char* __CDECL (*_ldg_strdup)          ( char *src, long context);
char* __CDECL (*_ldg_allocmode)       ( long mode, long context);

int ldg_mem_init( char *lib, int vers, LDG **mem) {
  *mem = ldg_open( lib, ldg_global);

  if( *mem == NULL) return ldg_error();
  if( (*mem)->vers < vers) return -100;
  _ldg_calloc          = ldg_find( "ldg_calloc", *mem);
  _ldg_free            = ldg_find( "ldg_free", *mem);
  _ldg_mallocczero     = ldg_find( "ldg_mallocczero", *mem);
  _ldg_mallocChunkSize = ldg_find( "ldg_mallocChunkSize", *mem);
  _ldg_malloc          = ldg_find( "ldg_malloc", *mem);
  _ldg_realloc         = ldg_find( "ldg_realloc", *mem);
  _ldg_strdup          = ldg_find( "ldg_strdup", *mem);
  _ldg_allocmode       = ldg_find( "ldg_allocmode", *mem);
  return ldg_error();
}

