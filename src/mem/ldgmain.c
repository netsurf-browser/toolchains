/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * Main part of the MEM.LDG library
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
 * $Id: ldgmain.c 9 2005-07-25 19:37:25Z bercegeay $
 */

#define VERSION		"0.50"
#define NUM_VER		0x0050

/* Fichiers headers */

/*#include <portab.h>*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gem.h>
#include <ldg.h>
#include "globals.h"

/* Macros */
#define max(a,b)	(((a)>(b))?(a):(b))

/* Prototypages */
void* __CDECL ldg_malloc( unsigned long size, long context);
void* __CDECL ldg_calloc( unsigned long, unsigned long, long context);
void* __CDECL ldg_realloc( void *adr, unsigned long size, long context);
void  __CDECL ldg_free( void *adr, long context);
void  __CDECL ldg_malloczero( long yes, long context);
void  __CDECL ldg_mallocChunkSize( unsigned long size, long context);
char* __CDECL ldg_strdup( char *, long context);

/* long CDECL  getusedmem( long context); */

void __CDECL ldg_allocmode( long mode, long context) {
	extern short mxalloc[];
	mxalloc[context] = mode ? 1:0;
}

/* Variables globales */
static PROC Proc[]  = {
  "ldg_calloc", 
  "/* Memory reservation with initialization */\n"
  "void* ldg_calloc( unsigned long num, unsigned long size, long context);\n", 
  (void *)ldg_calloc,

  "ldg_free", 
  "/* Memory release */\n"
  "void ldg_free( void *addr, long context);\n", 
  (void *)ldg_free,

  "ldg_malloczero", 
  "/* ldg_malloc() set memory to zero */\n"
  "void ldg_malloczero( long yes, long context);\n", 
  (void *)ldg_malloczero,

  "ldg_mallocChunkSize", 
  "/* Ajust memory parameters */\n"
  "void ldg_mallocChunkSize( unsigned long size, long context);\n", 
  (void *)ldg_mallocChunkSize,
  
  "ldg_malloc", 
  "/* Memory reservation */\n"
  "void* ldg_malloc( unsigned long size, long context);\n", 
  (void *)ldg_malloc,

  "ldg_realloc", 
  "/* Memory reallocation */\n"
  "void* ldg_realloc( void* adr, unsigned long size, long context);\n", 
  (void *)ldg_realloc,
  
  "ldg_strdup", "/* String duplication */\n"
  "char* ldg_strdup( char *srd, long context);\n", 
  (void *)ldg_strdup, 
  
  "ldg_allocmode", "/* Choice memory allocation mode */\n"
  "void ldg_allocmode( long mode, long context);", 
  (void*)ldg_allocmode 
  
};

static char Info[] = "Memory manager for LDG by Dominique B‚r‚ziat version " 
                     VERSION " ( " __DATE__ " at " __TIME__ ").";

LDGLIB Ldg[] = { NUM_VER, 8, Proc, Info, 0};

extern int ldg_context; 

void* __CDECL ldg_malloc( unsigned long mem, long context) {
	void *my_malloc( unsigned long);
	void *adr;
	ldg_context = (int)context;
	adr = my_malloc( mem);
	return adr;
}

void* __CDECL ldg_calloc( unsigned long num, unsigned long size, long context) {
	void *my_calloc( unsigned long, unsigned long);
	ldg_context = (int)context;
	return my_calloc( num, size);
}

void* __CDECL ldg_realloc( void* adr, unsigned long mem, long context) {
	void *my_realloc( void *, unsigned long);
	ldg_context = (int)context;
	return my_realloc( adr, mem);
}

void __CDECL ldg_free( void* addr, long context) {
	void my_free( void *);
	ldg_context = (int)context;
	my_free( addr);
}

void __CDECL ldg_malloczero( long yes, long context) {
	void _malloczero( int);
	ldg_context = (int)context;
	_malloczero( (int)yes);
}

void __CDECL ldg_mallocChunkSize( unsigned long size, long context) {
	void _mallocChunkSize ( size_t siz);
	ldg_context = (int)context;
	_mallocChunkSize( (size_t)size);
}

char* __CDECL ldg_strdup( char *str, long context) {
	void *my_malloc( unsigned long);
	ldg_context = (int)context;
	if( str && *str) {
		char *p = my_malloc( (strlen( str)+1)*sizeof(char));
		strcpy( p, str);
		return p;
	} else
		return NULL;
}

void init_global_vars(void);

int main( void) {
	extern int __mint, __magic;

	ldg_cookie( 'MiNT', (long*)&__mint);
	__magic = ldg_cookie( 'MagX', NULL);
	init_global_vars();
	if( ldg_init( Ldg)) 
		puts( "Warning : this program is a LDG-library and cannot be launched from desktop");
	return 0;
}

