/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * Some usefull functions (for LDG kernel & client application)
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
 * $Id: ldgutil.c 131 2015-08-27 21:42:48Z landemarre $
 * Patch Coldfire Vincent Riviere
 */

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "../version.h"
#include "../options.h"
#include "../../include/ldg.h"
#include "global.h"

/*#include <gem.h>*/
#if defined(__PUREC__) || defined(__VBCC__)
#include <tos.h>
#endif
#if defined(__GNUC__) || defined(__SOZOBONX__)
#include <osbind.h>
#define BASPAG	BASEPAGE
#endif

struct _ldg_version ldg_version = { LDG_NAME, LDG_NUM};

#define MagX_COOKIE     0x4D616758L	/* 'MagX' */
#define MiNT_COOKIE     0x4D694E54L	/* 'MiNT' */


/*
 * @TODO : get a better version of get_cookie (from WinDom or Ssytem()'s Mint)
 */

static long cookieptr(void) { 
  return *((long *)0x5a0); 
}

int	ldg_cookie (long cookie, long *value) {
  long *cookiejar = (long *)Supexec (cookieptr);
  
  if (cookiejar) {
    while (*cookiejar) {
      if (*cookiejar==cookie) {
	if (value) *value = *++cookiejar;
				return( 1);
      }
      cookiejar += 2;
    }
  }
	return( 0);
}

/*
 * Gestion des erreurs : le code erreur est
 * ‚crit dans le champ du cookie LDGM d‚di‚ …
 * cet usage.
 */

void set_error( int code) {
	LDG_INFOS *cook;
	
	if( ldg_cookie( LDG_COOKIE, (long*)&cook) && cook->version >= 0x0210)
		cook->error = (short)(code&0xFFFF);
}

#if 0
/*
 *	Fonction de d‚bugage 
 *  nouvelle version : si la chaine <format> est
 *	au format d'une alerte GEM, on utilisera form_alert
 */

void ldg_debug( char *format, ...) {
	char *path, debug[255];
	va_list arglist;
	FILE *log;
	
	/* Provisoire, on pourrait aussi utiliser le cookie
	 * car ‡a serait plus souple */
	shel_envrn( &path, "LDG_DEBUG=");

	/* format */
	if( format) {
		va_start( arglist, format);
		vsprintf( debug, format, arglist);
		va_end( arglist);
		if( *format == '[')
			form_alert( 1, debug);
		else {
			log = fopen( path, "a");
			if( log) fputs( debug, log);
		} 
	} else {
		log = fopen( path, "w");
		if(log)	fclose( log);
	}
}

#endif

void *ldg_Malloc( long size) { 
	void *bloc = 0L; 
	static short flag = 0;	/* vaut 1,-1 ou 0 */
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
		/* Mxalloc() … partir de GEMDOS 0 r‚vision 19 */
		/* flag = ((versn.braker.major>0) || (versn.braker.minor>=0x19))?1:-1; */
		/* Franchement il y a aucune difference */

		if((versn.braker.major>0) || (versn.braker.minor>=0x19))		
			flag = -1;
		else 
			flag=1;
 	}
	if( flag == -1) {
		if( ldg_cookie( MagX_COOKIE, NULL) || ldg_cookie( MiNT_COOKIE, NULL))
		/* allocation globale pr‚f‚rentiellement en TT Ram */
    		bloc = (void *)Mxalloc( size, 3+8+32);
		else
			bloc = (void *)Mxalloc( size, 3);
	} else
		bloc = (void *)Malloc( size);
	return( bloc);
}

void *ldg_Calloc(long count,long size) { 
	void *bloc = 0L; 
	char *pt = 0L; 
	long i, taille;
  
	taille = count*size;
	bloc = ldg_Malloc( taille);
	if(bloc != 0L) {
		pt = (char *)bloc;
		for( i= 0L; i < taille; i++)
			*pt++ = 0;
	}
	return( bloc);
}

void *ldg_Realloc( void *oldblk, long oldsize, long newsize) {
	void *newblk;
	
	newblk = ldg_Malloc( newsize);
	if( newblk) {
		memcpy( newblk, oldblk, oldsize);
		ldg_Free( oldblk);
		return newblk;
	}
	return NULL;
}

int ldg_Free(void *memory) {
	return( Mfree(memory));
}

#ifndef __mcoldfire__

/*
 *  ldg_cpush
 *
 * vidage des caches data et instruction du processeur
 * par l'appel … la fonction asm "cpush".
 * ce code peut etre appel‚ … partir du 68040 et doit ˆtre
 * ex‚cut‚ en mode superviseur (d'o— l'utilisation de Supexec).
 *
 * 1er mouture le 17 avril 2002 par Arnaud BERCEGEAY <bercegeay@atari.org>
 */

static unsigned short do_cpush[] = {
	0x4E71, /* NOP         ; 1er nop utile pour certains 68040 */
	0xF4F8, /* CPUSHA BC   ; vidage des caches data & instruction */
	0x4E75  /* RTS         ; fin de la procedure */
};

#define CPU_COOKIE 0x5F435055UL  /* _CPU */

void ldg_cpush(void) {
	static int initialized = 0;
	static long _cpu = 0L;

	if (!initialized) {
		ldg_cookie( CPU_COOKIE, &_cpu);
		_cpu &= 0xFFFF;
		initialized = 1;
	}

	if (_cpu > 30)
		Supexec( (long (*)())do_cpush);
}

#endif

/* EOF */
