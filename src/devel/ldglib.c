/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * ldg_init() : the LDG interface from LDG-library side
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
 * $Id: ldglib.c 130 2015-08-27 21:41:27Z landemarre $
 */

#include <stdlib.h>
#include <string.h>

#if defined(__PUREC__) || defined(__VBCC__)
#include <tos.h>
#else
#include <osbind.h>
#endif

#include "../version.h"
#include "../options.h"
#include "../../include/ldg.h"

#ifdef __SOZOBONX__
volatile static LDG lib;
#else
static volatile LDG lib;
#endif

/*
 *  D‚tournement des fonctions pour les rendre compatible "stdcall"
 *  c…d, seuls les registres a0-a1 et d0-d1 sont des scratch registers.
 *  les autres registres (comme a2 et d2) doivent ˆtre pr‚serv‚s,
 *  ce qui n'est pas le cas avec sozobon et pure c.
 *  (ajout Arnaud Bercegeay nov 2001)
 */

/* sauvegarde des contextes (registres), aussi utilisé par ldg_callback() */

#define NB_REG_CONTEXT 512
unsigned long _ldg_nb_regctx  = NB_REG_CONTEXT;
struct { unsigned long _r1, _r2, _r3, _r4;
	} _ldg_adr_regctx [ NB_REG_CONTEXT ] ;


#ifndef __GNUC__

/* nouvelle destination des fonctions d‚tourn‚es */
/* voir stdcall.s */
extern void _ldg_begin_stdcall (void );

/* point d'entr‚e de la redirection des fonctions */

#define NB_PROC_REDIRECT 500
static struct { short _r1; void * _r2; short _r3; void * _r4;
	} tab_redirect_proc[ NB_PROC_REDIRECT ];


static void trnfm_to_stdcall ( void ) {
	int i;

	if ( lib.num > NB_PROC_REDIRECT ) {
		Cconws("\r\nldg_trnfm_to_stdcall failed (tab_redirect_proc too small)\r\n");
		return;
	}
		
	/* init des proc‚dures de redirection */
	for (i = 0; i < lib.num; i++) {
		tab_redirect_proc[i]._r1 = 0x41F9 ;  		 /* LEA A0, <abs.l ea> */
		tab_redirect_proc[i]._r2 = lib.list[i].func; /* the ea */
		tab_redirect_proc[i]._r3 = 0x4EF9 ;  		 /* JMP <abs.l ea> */
		tab_redirect_proc[i]._r4 = _ldg_begin_stdcall;
		lib.list[i].func = &tab_redirect_proc[i];
	}

	lib.flags |= LDG_STDCALL;
}
#endif /* __GNUC__ */

/*
 *	Interface LDG des libraries
 */
 
int ldg_init( LDGLIB *liblib) {
	char *env;
	long *offset_pere;
	long offset = (long ) &lib;   /* directement */

	lib.magic    = LDG_COOKIE;
	lib.vers     = liblib->vers;
	lib.num      = liblib->num;
	lib.list     = liblib->list;
	lib.infos    = liblib->infos;
	lib.flags    = liblib->flags;
	lib.close    = liblib->close;
 	lib.vers_ldg = LDG_NUM;
 	lib.user_ext = liblib->user_ext;
 	lib.addr_ext = 0L;
	
	/* IMPORTANT: the buffer HAVE TO be zero-ed before use */
	/* memset( _ldg_adr_regctx, 0, sizeof(_ldg_adr_regctx));  */
	{
		int len =sizeof(_ldg_adr_regctx), i;
		unsigned char *pt=(unsigned char *)_ldg_adr_regctx;
		for (i=0;i<len;i++)
		{
			*pt++=0;
		}
	}
	
	/* d‚tournement des fonction pour pr‚server les registres */
	/* a2 et d2 si cela est n‚cessaire (sozobon et pure c) */
	
#ifndef __GNUC__
	lib.flags &= ~LDG_STDCALL;
	trnfm_to_stdcall();
#else
	lib.flags |= LDG_STDCALL;
#endif

	env = getenv("OFFSETLDG");
	if( env) {
    	/* r‚cup‚ration de l'adresse offset du client pour la remplir */
    	offset_pere = (long*)atol(env);
    	*offset_pere = offset;
    	return 0;  /* Pas d'erreurs */
	} else
    	return -1; /* ne peut pas etre lanc‚ du bureau */
}

/*
 *	Vrai chemin de la librarie. Doit ˆtre appel‚e depuis une fonction
 *	de la librairie mais jamais du main() de la lib.
 */

char *ldg_getpath( void) {
	return (char *)(lib.path);
}

