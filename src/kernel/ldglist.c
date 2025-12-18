/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * Handling of internal list of clients and libraries
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
 * $Id: ldglist.c 72 2008-01-12 15:46:08Z landemarre $
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <options.h>
#include <mt_gem.h>
#include "global.h"


extern struct internal intern[];


/*
 *	Envoie un message … une application/librairie
 *	permet aussi d'ˆtre ind‚pendant du mode 16/32 bits
 *	de Gcc.
 */

void ldg_snd_msg( int from_id, int to_id, int msg, 
		  		  int mot3, int mot4, int mot5, int mot6, int mot7,
		  		  short *global) {
	short buf[8];
  
	buf[0] = (short)(msg&0xFFFF);
	buf[1] = (short)(from_id&0xFFFF);
	buf[2] = 0;
	buf[3] = (short)(mot3&0xFFFF);
	buf[4] = (short)(mot4&0xFFFF);
	buf[5] = (short)(mot5&0xFFFF);
	buf[6] = (short)(mot6&0xFFFF);
	buf[7] = (short)(mot7&0xFFFF);
  
	mt_appl_write( to_id, 16, buf, global);
}

#ifdef DEBUG
/*
 * Initialisation des tableaux : pour debogage
 */

long internal_init( void) {
	long mem;
	intern = ( struct internal *) ldg_Malloc( MAX_LIB*sizeof( struct internal));
	intern->libs = ( struct lib *) ldg_Malloc( MAX_LIB*sizeof( struct lib));
	intern->maxlib = MAX_LIB;
	intern->maxclient = MAX_CLIENT;
	mem = MAX_LIB*sizeof( struct lib);
	if( intern->libs) {
	/*	intern->apps = ( STR9 *) ldg_Malloc( MAX_CLIENT_TOTAL*sizeof( STR9));
		mem += MAX_CLIENT_TOTAL*sizeof( STR9);
		if( intern->apps) { */
			int res;
			for( res=0; res<MAX_LIB; res++) {
				intern->libs[res].ldg = NULL;
  				*(intern->libs[res].name) = '\0';
  			}
  	/*	} */
	}
	return mem;
}
#endif

/* 
 * Initialisation des tableaux globaux
 * et connections du tableau _usedby
 */
extern int _usedby[];
extern int _ndecl[];

void internal_raz( void) {
	int i, j;
	for( i = 0; i < intern->maxlib; i++) {
		*(intern->libs[i].name) = '\0';
		intern->libs[i].ldg = NULL;
		intern->libs[i].usedby = _usedby + i*MAX_CLIENT;
		intern->libs[i].ndecl = _ndecl + i*MAX_CLIENT;
		for( j = 0; j < intern->maxclient; j++)
			intern->libs[i].usedby[j] = -1;
	}
}

/*
 *	trouve le nom d'une application
 */

int appl_name( char *name, int id, short *gl) {
	short i,m=0x0F;

	if( mt_AESversion(gl) >= 0x0399) {
		if( mt_appl_search( 0, name, &m, &i, gl) == 0) {
			*name = '\0';
			return 0;
		}
		if( i == id) { 
			return 1;
		}
		while( mt_appl_search( 1, name, &m, &i, gl)) {
			if( i == id ){ 
				return 1;
			}
		}
	}
	*name = '\0';
	return 0;
}

/* FONCTIONS QUI GERENT LA LISTE DES LIBS */

/* 
 * id = -1 : nombre de client
 * id >= 0 : position d'un client <id> dans une liste -1
 */

int get_app( int id, int *list) {
	int i, count=0;
	
	for( i=0; i<intern->maxclient; i++)
		if( id == -1) {
			if( list[i] != -1) count ++;
		} else if( list[i]==id) return i;
	return (id == -1)?count:-1;
}

/*
 * position d'une lib
 * recherche par nom ou par
 * adresse LDG si <name> est nul
 */

int get_lib( char *name, LDG *ldg) {
	int i;	
	
	for( i=0; i<intern->maxlib; i++) {
		if( name && !stricmp( name, intern->libs[i].name))
			return i;
		if ( ldg && ldg == intern->libs[i].ldg)
			return i;
	}
	return -1;
}

/* EOF */
