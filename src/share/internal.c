/*
 *  LDG 2.xx - module de d‚veloppement
 *	par D.B‚r‚ziat & O.Landemarre.
 *	(c) 2000 - tous droits r‚serv‚s.
 *
 *	La routine - complexe - qui permet d'avoir la
 *	liste internes des libraries. Elle intŠgre toutes les
 *	version de LDG. Elle est utilis‚ par les outils de
 *	maintenance (CPX et showlib).
 */


#include <gem.h>
#include <stdio.h>
#include <ldg.h>
#include <string.h>

#include "internal.h"
#include "backward.h"

#define LIB221	0x0100

/*
 *	M‚canisme :
 *	pour LDG 1.xx. La liste des libraries est fournie par le
 *	manager avec le message LDG_LIST
 *	pour LDG 2.xx elle est cod‚e en interne dans le cookie. On
 *	peut y acc‚der via le champ 'internal' du cookie. Il y a
 *	toutefois une diff‚rence entre le cookie 2.20 et 2.21 : la
 *	structure y change afin de g‚rer le cas des d‚clarations
 *	multiple d'une mˆme librarie par un mˆme client.
 *
 *	Les fonctions retourne les infos dans un format unique 
 *	(la structure ldg_infos).
 */

/*
 * Stucture utilis‚e par le gestionnaire LDG 
 * et le TSR version 2.20
 */

struct lib220 {
	char name[32];          	/* nom AES de la lib */
	LDG *ldg;               	/* adresse de la LDG */
	short *usedby;     			/* liste des clients */
};

/*
 * Stucture utilis‚e par le TSR version >= 2.20
 */

typedef char STR9[9];

struct internal {
	union _libs {		/* liste des libs */
		struct lib220  *old;		/* si 'LDGM'->version == 0x0220 */
		struct ldg_lib *new;		/* si 'LDGM'->version >= 0x0221 */
	} libs;
	STR9	   *apps;	/* nom des clients */
	short	  maxlib;
	short	maxclient;
	short totalclient;
};

/*
 *	Interroge le gestionnaire LDG
 */

int ldg_mgrlist( struct ldg_infos *infos, short *gl) {
	short i, mgrid, msg[]={LDG_LIST,0,0,0,0,0,0,0};

	mgrid = mt_appl_find( "LDGMGR  ", gl);
	if( mgrid == -1)	
		return LDG_NO_MANAGER;

	msg[1]=mgrid;	
	i = 0;
	do {
		evnt_mesag( msg);
		i++;
	} while( msg[0] != LDG_LIST && i < 20);

	
	if( msg[0] == LDG_LIST) {
		infos->maxlib = msg[5];
		infos->version = msg[7];
		infos->maxclient = msg[6];
		infos->current = 0;
		infos->totalclient = -1;	/* info non disponible */
		if( infos->version >= 0x0200) {
			LDG_INFOS *cook;

			infos->type = LDGMGR;
			/* Le manager en version 2.xx utilise directement
			 * le cookie. */
			ldg_cookie( 'LDGM', (long*)&cook);			
			if( cook->version >= 0x0221)
				infos->type |= LIB221;
			infos->listlib = *(struct ldg_lib**) &msg[3];
		} else {
			infos->type = LDGMGR;
			infos->listlib = *(struct lib220**) &msg[3];
		}
	} else
		return LDG_TIME_IDLE;
	return mgrid;
}

/*
 *	Cette fonction retourne la liste des librairies.
 *	Elle utilise le format lib220 pour simplifier
 *	l'utilisation cot‚ client.
 *
 *	retourne : -1 : erreur,
 *				1 info par manager,
 *				2 info par cookie.
 */

int ldg_libslist( struct ldg_infos *infos,  short *gl) {
	LDG_INFOS *cook;

	if( ldg_cookie( LDG_COOKIE, (long*)&cook) ) {
		struct internal *intern;
	
		if( cook->version < 0x220) {
			/* Cas LDG 1.xx avec cookie */
			return ldg_mgrlist( infos, gl);
		} else if( cook->version == 0x220) {
			/* Cas LDG 2.00 */
			intern = cook->internal;
			infos->maxlib = intern->maxlib;
			infos->maxclient = intern->maxclient;
			infos->totalclient = intern->totalclient;
			infos->version = cook->version;
			infos->type = LDGTSR;
			infos->listlib = intern->libs.old;
			infos->current = 0;
			return 0;
		} else {
			/* Cas LDG 2.10 */
			intern = cook->internal;
			infos->maxlib = intern->maxlib;
			infos->maxclient = intern->maxclient;
			infos->totalclient = intern->totalclient;
			infos->version = cook->version;
			infos->type = LDGTSR | LIB221;
			infos->listlib = intern->libs.new;
			infos->current = 0;
			return 0;
		}
	} else
		/* Cas LDG 1.xx sans cookie */
		return ldg_mgrlist( infos, gl);
}

void ldg_initlist( struct ldg_infos *infos) {
	infos->current = 0;
}

int ldg_getlib( struct ldg_infos *infos, struct ldg_lib *lib) {	
	if( infos->type & LIB221) {
		struct ldg_lib *listlib = infos->listlib;
	
		while( listlib[infos->current].ldg == NULL &&
			   infos->current < infos->maxlib)
			   infos->current ++;
		if( infos->current < infos->maxlib) {
			strcpy( lib->name, listlib[infos->current].name);
			lib->ldg = listlib[infos->current].ldg;
			lib->usedby = listlib[infos->current].usedby;
			lib->ndecl = listlib[infos->current].ndecl;
			infos->current ++;
			return 1;
		}
	} else {
		struct lib220 *listlib = infos->listlib;
	
		while( listlib[infos->current].ldg == NULL &&
			   infos->current < infos->maxlib)
			   infos->current ++;
		if( infos->current < infos->maxlib) {
			strcpy( lib->name, listlib[infos->current].name);
			lib->ldg = listlib[infos->current].ldg;
			lib->usedby = listlib[infos->current].usedby;
			lib->ndecl = NULL;
			infos->current ++;
			return 1;
		}
	}
	return 0;
}

int ldg_loadlib( struct ldg_infos *infos) {
	int i;
	int count = 0;

	if( infos->type & LIB221) {
		struct ldg_lib *lib = infos->listlib;
		for( i=0; i<infos->maxlib; i++)
			if( lib[i].ldg) count ++;
	} else {
		struct lib220 *lib = infos->listlib;
		for( i=0; i<infos->maxlib; i++)
			if( lib[i].ldg) count ++;	
	}
	return count;
}

/* EOF */
