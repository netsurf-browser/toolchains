/*
 *	Librairies Dynamiques GEM
 *	Global definitions
 * 	Portable version for Gnu C/Pure C/Sozobon C
 *
 *	Copyright 1997-2000 by Olivier Landemarre and Dominique B‚r‚ziat
 */

#ifdef __PUREC__
#include <tos.h>
#else
#include <osbind.h>
#endif
#include <gem.h>
#include <ldg.h>

#define ADR2WORD(c)     (int)((long)(c)>>16), (int)(long)(c)

struct lib {
	char name[32];  		/* nom AES de la lib */
	LDG *ldg;   			/* adresse de la LDG */
	int *usedby;			/* liste des clients */
	int *ndecl;				/* nbre de chargement d'une meme lib */
	unsigned short copy_of_flags;	/* copie des flags de la lib au moment du chargement */ 	
};

typedef char STR9[9];

struct internal {
	struct lib *libs;  		/* liste des libs */
/*	STR9	   *apps; */		/* nom des clients */
	int		  maxlib;
	int		maxclient;
	int	  totalclient;
};

/* Patch systŠme */

#ifndef FO_READ
# define FO_READ 	0
#endif

#ifndef SHMSETBLK
# define SHMSETBLK 0x4D01
# define SHMGETBLK 0x4D00
#endif
