/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * Main function of LDG TSR. This TSR just installs the LDGM cookie in memory.
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
 * $Id: tsrmain.c 112 2015-05-14 13:57:17Z landemarre $
 */

#include <stdio.h>
#include <string.h>
#include <options.h>
#include "../version.h"

#ifdef __PUREC__
#include <tos.h>
/*#ifndef BASEPAGE
#define	BASEPAGE BASPAG
#endif*/
#endif
#if defined(__GNUC__) || defined(__SOZOBONX__)
# if (__GNUC__ > 2) || ( __GNUC_MINOR__ > 8 )
#  include <mint/basepage.h>
# else
#  include <basepage.h>
# endif
# include <osbind.h>
# include <mintbind.h>
#define _BasPag _base
#endif

#include "global.h"
#include "cookie.h"

#ifndef __CDECL
#define __CDECL cdecl
#endif
/* protos fonctions externes
 */

LDG*  __CDECL cdecl_ldg_open    ( char *, short *);
LDG*  __CDECL cdecl_ldg_open_ev ( char *, short *, void (*)(short *));
short __CDECL cdecl_ldg_close   ( LDG *,  short *);
void* __CDECL cdecl_ldg_find	( char *, LDG *);
void  __CDECL cdecl_ldg_garbage ( short *);
void  __CDECL cdecl_ldg_release ( short *);
short __CDECL cdecl_ldg_libpath ( char *path, short *global);

extern void _ldg_begin_stdcall (void );
static struct { short _r1; void * _r2; short _r3; void * _r4;}
	stdcall_ldg_open    = { 0x41F9, cdecl_ldg_open   , 0x4EF9, _ldg_begin_stdcall},
	stdcall_ldg_open_ev = { 0x41F9, cdecl_ldg_open_ev, 0x4EF9, _ldg_begin_stdcall},
	stdcall_ldg_close   = { 0x41F9, cdecl_ldg_close  , 0x4EF9, _ldg_begin_stdcall},
	stdcall_ldg_find    = { 0x41F9, cdecl_ldg_find   , 0x4EF9, _ldg_begin_stdcall},
	stdcall_ldg_garbage = { 0x41F9, cdecl_ldg_garbage, 0x4EF9, _ldg_begin_stdcall},
	stdcall_ldg_release = { 0x41F9, cdecl_ldg_release, 0x4EF9, _ldg_begin_stdcall},
	stdcall_ldg_libpath = { 0x41F9, cdecl_ldg_libpath, 0x4EF9, _ldg_begin_stdcall};

/*
 * Sauvegarde des contextes (registres)
 * NB_REG_CONTEXT correspond au nb max de fonctions du cookie qui sont executees
 * simultanement, et donc au nb max d'applications pouvant appeler les fonctions
 * du cookie. Cela correspond au nb max de client du systeme LDG 
 */

#define NB_REG_CONTEXT MAX_CLIENT_TOTAL
unsigned long __ldg_nb_regctx  = NB_REG_CONTEXT;
struct { unsigned long _r1, _r2, _r3, _r4;
	} __ldg_adr_regctx [ NB_REG_CONTEXT ] ;

struct lib	_libs	[MAX_LIB];
int 		_usedby	[MAX_LIB*MAX_CLIENT];
/* STR9 		_apps	[MAX_CLIENT_TOTAL]; */
int 		_ndecl	[MAX_LIB*MAX_CLIENT];

struct internal intern[] = { 
		&_libs[0],
	/*	&_apps[0], */
		MAX_LIB,
		MAX_CLIENT,
		MAX_CLIENT_TOTAL };

char DataStart[] = "LdgData";	/* Pour identifier le d‚but 
				 * de la zone des donn‚es.
				 * (Utilis‚ par ldginfo.ttp pour
				 * modifier les parametres du
				 * du TSR) */

LDG_INFOS ldgcook = {	
  TSR_VERSION_NUMBER, 
  "C:\\gemsys\\ldg\\", 
  30, 1000,
  (void*)&stdcall_ldg_open, 
  (void*)&stdcall_ldg_close, 
  (void*)&stdcall_ldg_find,
  (void*)&stdcall_ldg_open_ev,
  0,
  (void*)&stdcall_ldg_garbage,
  (void*)&stdcall_ldg_release,
  &intern[0],
  (void*)&stdcall_ldg_libpath
};

#ifdef NOSTARTUP
int errno, _FilSysV;
#endif
int boot;

void message( char *msg);
void start_info( char *version);
void end_info( void);
int  vq_aes( void);

/* Note that this program does NOT use the standard startup
 * code. The function below needs to be the first one in the
 * text segment : bien dit */

void __CDECL main( /*BASPAG *bp*/) {
	long len ,old_stack;				/* Programml„nge */
	int success=0;
	int resident = 1, flag=0;
	void internal_raz( void);

	/* calculate size of TPA */
/*	len = sizeof(BASPAG) + bp->p_tlen + bp->p_dlen + bp->p_blen+4096L; */
	len = sizeof(BASEPAGE) + _BasPag->p_tlen + _BasPag->p_dlen + _BasPag->p_blen+4096L;
			/* faut pas oublier la taille de la pile ici 4096 */

	/*	Mshrink (0, bp, len); sert a rien le Ptermres s'en charge … la fin */
	boot = (vq_aes() == -1)?1:0;

	/* print banner */
	if( boot) {
		Cconws( "\r\n\r\033pLibrairies Dynamiques Gem, version " TSR_VERSION_TEXT "\r\n");
		Cconws( "by Bercegeay, B‚r‚ziat & Landemarre, ½ 1997 - 2012\033q\r\n");
	} else
		start_info( "Librairies Dynamiques Gem, version " TSR_VERSION_TEXT);

	/* cookie lib needs supervisor mode */
	old_stack = Super ((void *)1L);
	if(old_stack == 0L) {
    	old_stack = Super(0L);
    	flag=1;
	}
	
	/* find out whether we need to terminate and stay resident
	 * (cookie jar created or resized : et toc! 
	 */	
	
	if (CK_JarInstalled ()) {
		LDG_INFOS *oldldg;

		/* cookie d‚j… l… */
		if (CK_ReadJar ('LDGM', (long*)&oldldg)) {
			resident = 0;
			/*	Cconws( "\033lWarning: Librairies Dynamiques Gem already installed!\r\n");*/
			if( oldldg->version < ldgcook.version) {
				if( boot) {
					Cconws( "Warning: an older version of LDG TSR is already installed !\r\n");
					Cconws( "         cookie will be replaced !\r\n");
				}
				/* On s'insŠre de force */
				resident = 1;
			} else if( oldldg->version == ldgcook.version) {				
#if TSR_FORCE_INSTALL
				if( boot) {
					Cconws( "Warning: LDG TSR with a same version number is already installed !\r\n");
					Cconws( "         cookie will be replaced !\r\n");
				} 
				/* On s'insŠre de force */
				resident = 1;
#endif
			}
		}

		/* cookiejar plein … craquer 
		 * Ce cas n'est pas g‚r‚ pour une raison que
		 * j'ai oubli‚. Si on l'active, il faut pr‚voir
		 * l'erreur dans l'affichage */

		/* if(CK_JarSize () - CK_UsedEntries () > 0) {
			Cconws( "LDG:fatal: CookieJar is fulled.\r\n");
			resident = 0;
		} */
	}
	
	/* Allocation m‚moire & initialisation */
	if( resident) {
		/* IMPORTANT: the buffer HAVE TO be zero-ed before use */
		memset( __ldg_adr_regctx, 0, sizeof(__ldg_adr_regctx));

		/* libs */
		internal_raz();
		/* set the cookie */
		success = CK_WriteJar ( 'LDGM', (long)&ldgcook);
	}
		
	if( flag) 
		Super((void *)old_stack); /* on remet le mode de d‚part */
	
	/* report what happened */
	if( success) {
		if( boot) Cconws( "Successfully installation.\r\n\r\n");
		else	  message( "Successfully installation.");
	} else {
		if( boot) Cconws( "LDG: Installation error (LDG already installed).\r\n\r\n");
		else 	  message( "Installation error (LDG already installed)");
	}

	if( !boot)	end_info();
	/* and terminate */
	if( success)
		Ptermres( len, 0);
	else
		Pterm ( 1);
}

LDG* __CDECL cdecl_ldg_open_ev( char *name, short *gl, void (*ev)(short*)) {
	return cdecl_ldg_open( name, gl);
}

int vq_aes(void) {
  short gl[AES_GLOBMAX];
  int ret;
  
  mt_AESversion(gl) = 0;
  mt_appl_init( gl);
  ret = (mt_AESversion(gl) == 0) ? -1: mt_AESversion(gl);
  mt_appl_exit( gl);
  return ret;
}

/* EOF */
