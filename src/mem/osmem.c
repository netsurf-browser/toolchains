/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * Request memory to OS
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
 * $Id: osmem.c 122 2015-05-30 09:07:41Z landemarre $
 */

#include <stdio.h>
#ifdef __PUREC__
#include <tos.h>
#include <options.h>
#else
#include <osbind.h>
#endif
#include <gem.h>
#include <ldg.h>
#include "globals.h"

#define MagX_COOKIE     0x4D616758L	/* 'MagX' */
#define MiNT_COOKIE     0x4D694E54L	/* 'MiNT' */
#define SHMSETBLK       0x4d01

#ifdef DOSHM
static int __memcount = 0;
extern short mxalloc[];
extern int   ldg_context;
extern int 	 __mint, __magic;

static void create_shm( int memid, void *adr) {
	long fd;
	char path[50];
	int ldgd;
	
	sprintf( path, MEM_SHM_FILENAME, memid);
	fd = (int) Fcreate( path, 0);
	if( fd >= 0) {
		Fcntl( (int)fd, (long)adr, SHMSETBLK);
		Fclose( (int)fd);
		ldgd = appl_find( "LDGD    ");
    	if( ldgd != -1) {
    		short buf[8] = {LDGD_SHM_REQUEST, 0, 0, LDGD_SHARE_MEM, 0, 0, 0};
    		buf[4] = memid;
    		appl_write( ldgd, 16, buf);
		}
	}
}
#endif

void *OS_Malloc( long size) { 
	void *bloc = 0L; 
	static int flag = 0;	/* vaut 1,-1 ou 0 */

#ifdef DEBUG
extern int ldg_context;
printf("requete %ld de %d\n", size, ldg_context);
#endif

	if( flag == 0) {
	union {
#ifdef __SOZOBONX__
		struct {
			unsigned minor:8;
			unsigned major:8;
		} braker;
#else
		struct {
			unsigned int minor:8;
			unsigned int major:8;
		} braker;
#endif
		short all;
	} versn;
		
		versn.all = (short)Sversion();
		if((versn.braker.major>0) || (versn.braker.minor>=0x19))		
			flag = -1;
		else 
			flag=1;
 	}
	if( flag == -1) {
#ifdef DOSHM
		if( mxalloc[ldg_context] && ( __mint || __magic))
#else
		if( ldg_cookie( MagX_COOKIE, NULL) || ldg_cookie( MiNT_COOKIE, NULL))
#endif
			/* allocation globale pr‚f‚rentiellement en TT Ram */
    		bloc = (void *)Mxalloc( size, MX_PREFTTRAM+MX_MINTMP+MX_GLOBAL);
		else
			/* sinon la m‚moire reste priv‚e */
			bloc = (void *)Mxalloc( size, MX_PREFTTRAM);
	} else
		bloc = (void *)Malloc( size);
#ifdef DOSHM
	if( mxalloc[ldg_context]) create_shm ( __memcount ++, bloc);
#endif
	return( bloc);
}

void OS_Free( void *ptr) {
#ifdef DOSHM
	char path[50];
	int ldgd;

	if( mxalloc[ldg_context]) {
		ldgd = appl_find( "LDGD    ");
		__memcount --;
	    if( ldgd != -1) {
	    	short buf[8] = {LDGD_SHM_REQUEST, 0, 0, LDGD_UNSHARE_MEM, 0, 0, 0};
	    	buf[4] =  __memcount;
	    	appl_write( ldgd, 16, buf);
		}
		sprintf( path, MEM_SHM_FILENAME, __memcount);
		Fdelete( path);
	}
#endif
	ldg_Free( ptr);
}


/* eof */
