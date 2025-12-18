/*
 * LDG : Gem Dynamical Libraries
 * Copyright (c) 1997-2004 Olivier Landemarre, Dominique Bereziat & Arnaud Bercegeay
 *
 * Implementation of LDGM cookie functions
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
 * $Id: ldgcook.c 107 2015-02-08 21:08:42Z landemarre $
 */

/*
 * Utilisateur de Pure C : corrigez la d‚claration de Fcntl() dans
 * TOS.H : la fonction retourne un long (et non un int).
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <options.h>
#if defined(__GNUC__) || defined(__SOZOBONX__)
#include <mintbind.h>
#endif
#include <mt_gem.h>
#include "global.h"

int flag_semaphore=0;

/*	Fonctions externes
 */
LDG* 	ldg_load	( int id, char *lib);
int		ldg_unload	( LDG *ldg);
void 	ldg_snd_msg	( int from_id, int to_id, int msg, 
		  		  	  int mot3, int mot4, int mot5, int mot6, int mot7,
		  		  	  short *global);
int 	appl_name	( char *name, int id, short *gl);
int 	get_app		( int id, int *list);
int 	get_lib		( char *name, LDG *ldg);
void 	set_error	( int);

/*	Variables externes
 */
extern struct internal intern[];

static void close_shm( int id) { 
	char path[50];
	sprintf( path, LDG_SHM_FILENAME, id);
	/* si la shm n'est pas support‚e, cet appel
	 * est sans cons‚quence ...
	 */
	Fdelete( path);
/*	sprintf( path, LDGBK_SHM_FILENAME, id);
	Fdelete( path); */
}

static void addslash( char *p) {
  if( p[strlen(p)-1] != '\\') strcat( p, "\\");
}

/*
 *	Cette fonction retourne le vrai chemin d'une librairie
 *	retourne 1 si le chemin est Okay 0 sinon.
 *	path doit pointer sur une chaine suffisament longue
 *
 * TODO: if path is empty : return a valid LDG directory
 *       if path is a directory (finishing by / or \) try to locate
 *       this directory among LDG paths
 */

short __CDECL cdecl_ldg_libpath( char *path, short *global) {
  char ppath[256];
  char *p;
  int fd;
  LDG_INFOS *cook;

  /* Convert in TOS format if needed */

  /* case of mint unified fs /.. -> u:/..  */
  if( *path == '/') {
#if 0
    path[0] = tolower(path[1]);
    path[1] = ':';
    p = path + 2; 
#else
    strcpy( ppath, path);
    strcpy( path, "u:");
    strcat( path, ppath);
  }
#endif
  /* convert slash to anti-slash */
  for( p=path; *p != '\0'; p++)
    if( *p == '/') *p = '\\';
  
  strcpy( ppath, path);

  /* ... dans le r‚pertoire courant (relatif ou complet)... */

  fd = (int)Fopen( path, FO_READ);
  
  /* ... dans le cookie LDGM ... */
  if( fd < 0 && ldg_cookie( LDG_COOKIE, (long*)&cook)) {
    strcpy( path, cook->path);
    addslash( path);
    strcat( path, ppath);
    fd = (int)Fopen( path, FO_READ);
  }

  /* ... ou dans la variable d'environnement LDGPATH ... */
  if( fd < 0) {
    mt_shel_envrn( &p, "LDGPATH=", global);
    if( p) {
      strcpy( path, p);
      addslash( path);
      strcat( path, ppath);
      fd = (int)Fopen( path, FO_READ);
    }	
  }
  
  /* dans les chemins PATH */
  if( fd < 0) {
    mt_shel_find( path, global);
    fd = (int)Fopen( path, FO_READ);
  }

  /* Le r‚sultat */ 
  if( fd >= 0 ) {
    (void)Fclose(fd);
    return 1;
  }
  return 0;		/* Librairie absente */
}


void ldg_garbage( short *gl);

/*
 *	d‚claration d'une lib et de son client
 *	LDG: adresse la lib
 *	name: nom de la lib
 *	id: id client
 *  fload : si 1, force le rechargement de la librairie.
 */

static LDG* ldg_open_flag( char *name, short *gl, int fload) {
  int i,j, id = mt_AESapid(gl);
  char *p;
  int loaded = 0;
  int fd;

  Syield();
  while(flag_semaphore) Syield();
  flag_semaphore=1;
  
  ldg_garbage( gl); /* Si le manager n'est plus l… pour faire le travail vaut mieux faire cela avnt de charger une nouvelle lib, c'est un peu long mais plus prudent */	
  /* librairie d‚j… ouverte partageable ? */
  
  /* remove path, keep filename, in TOS or MiNT format */
#if 1  /* Modif for LDG 2.33 */
  p = strpbrk( name, "/\\");
  p = p?(p+1):name;
#else
  p = name;
#endif
  i = get_lib( p, NULL);

  if( i != -1 && intern->libs[i].copy_of_flags & LDG_NOT_SHARED) i = -1;
  
  /* librairie non ouverte */
  if( i == -1 || fload) {
    /* ins‚rer dans la liste et charger la lib */
    for( i=0; i<intern->maxlib; i++)
      if( intern->libs[i].ldg == NULL) {
	char path[255];

	strcpy( path, name);
	/* Chemin de la lib puis chargement */
	if( cdecl_ldg_libpath( path, gl)) {
	  intern->libs[i].ldg = ldg_load( id, path);
	  intern->libs[i].copy_of_flags = intern->libs[i].ldg->flags;
	  sprintf( path, LDG_SHM_FILENAME, i);
	  fd = (int) Fcreate(path,0);
	  /* erreur ou SHM non support‚ */
	  if( fd >= 0) {
	    Fcntl( fd, (long)intern->libs[i].ldg->baspag, SHMSETBLK);
	    Fclose( fd);
#if 0
ne sert … rien SHM rend directement resident
	    /* Cas des librairies r‚sidentes */
	    if( intern->libs[i].ldg->flags & LDG_RESIDENT) {
	      int id = mt_appl_find( "LDGD    ", gl);
	      if( id != -1) {
		ldg_snd_msg( gl[2], id, 
			     LDGD_SHM_REQUEST, /* requete SHM pour LDGD */ 
			     LDGD_SHARE_LDG,   /* partage de la lib */
			     i,                /* librarie concern‚e */
			     0, 0, 0, gl);
	      }
	      
	    }
#endif
	  }
#if 0
	  /* partage du bloc LDG */
	  sprintf( path, LDGBK_SHM_FILENAME, i);
	  fd = (int) Fcreate(path,0);
	  /* erreur ou SHM non support‚ */
	  if( fd >= 0) {
	    Fcntl( fd, (long)intern->libs[i].ldg, SHMSETBLK);
	    Fclose( fd);
	  }
#endif	  
	  
	  loaded = 1;
	} else {
	  set_error( LDG_NOT_FOUND);
	  flag_semaphore =0;
	  return NULL;
	}
	strcpy( intern->libs[i].name, p);
	for( j=0; j<intern->maxclient; intern->libs[i].usedby[j++] = -1);
	break;
      }
    if( i == intern->maxlib) {
      set_error( LDG_LIB_FULLED);
      flag_semaphore =0;
      return NULL;		/* erreur, plus de place */
    }
  }

  /* d‚claration du client */
/*  appl_name( intern->apps[id], id, gl); */
  /* client d‚j… d‚clar‚ ? */
  j = get_app( id, intern->libs[i].usedby);
  if( j == -1) {
    if( loaded==0 && intern->libs[i].copy_of_flags & LDG_LOCKED) {
      set_error( LDG_LIB_LOCKED);
      flag_semaphore =0;
      return NULL;	/* erreur, librairie verrouill­e */
    }
    /* ins‚rer le client dans la liste */
    for( j=0; j<intern->maxclient; j++)
      if( intern->libs[i].usedby[j] == -1) {
	if( j != 0) {
	  char path[50];
      				
	  sprintf( path, LDG_SHM_FILENAME, i);
	  fd = (int)Fopen( path, FO_RW);
	  /* erreur ou SHM non support‚ */
	  if( fd >= 0) {	
	    char *blk;
      					
	    blk = (char *)Fcntl( fd, 0L, SHMGETBLK);
	    Fclose( fd);
	    if( blk != intern->libs[i].ldg->baspag) {
	      /* aie Mint a d‚plac‚ la lib ! 
	       * on pourrait vouloir la recharger
	       * mais ce sera un autre jour ! 
	       */ 
	      /*
		set_error( LDG_LOST_LIB);
		flag_semaphore =0;
		return NULL;
	      */
	      if( blk) Mfree( blk);
	      /* La librarie se recharge */
	      /* Dom -> Olivier : est-ce bien utile de passer par
	       * ldg_open_flag et pas directement cdecl_ldg_open() ? */
	       flag_semaphore =0;
	      return ldg_open_flag( name, gl, 1);
	    }
	  }
	}
	intern->libs[i].usedby[j] = id;
	intern->libs[i].ndecl[j] = 1;
	flag_semaphore =0;
	return intern->libs[i].ldg;
      }
    set_error( LDG_APP_FULLED);
    flag_semaphore =0;
    return NULL;		/* erreur, plus de place */
  } else
    intern->libs[i].ndecl[j] ++;
  
  flag_semaphore =0;
  return intern->libs[i].ldg;
}

LDG* __CDECL cdecl_ldg_open( char *name, short *gl) {
	return ldg_open_flag( name, gl, 0);
}

/*
 *	Une librarie n'est plus utilis‚e
 */

short __CDECL cdecl_ldg_close( LDG *ldg, short *gl) {
	int ilib, 		/* index lib */
		iclient;	/* index client */
	int apid = mt_AESapid(gl);
	
	Syield();
  	while(flag_semaphore) Syield();
  	flag_semaphore=1;
/*	printf("cdecl_ldg_close in\n"); Cconin(); */
    /* Chercher l'index de la lib */
	ilib = get_lib( NULL, ldg);
	if( ilib != -1) {
		/* chercher l'index du client */
		iclient = get_app( apid, intern->libs[ilib].usedby);
		/* Si client trouv‚, on l'enlŠve de la liste */
		if( iclient != -1) {
			if(intern->libs[ilib].ndecl[iclient] > 1)
				intern->libs[ilib].ndecl[iclient] --;
			else
				intern->libs[ilib].usedby[iclient] = -1;
		}

		/* Si plus de client, on retire la lib */
		if( get_app( -1, intern->libs[ilib].usedby) == 0 ) {
		/*	ldg_unload( intern->libs[ilib].ldg); */
			close_shm( ilib);
			intern->libs[ilib].ldg = NULL;
			*(intern->libs[ilib].name) = '\0';
		} else {

	/* DOM : ICI JE COMPREND PAS TROP */

		/* Arg ce qui suit ne fonctionne pas comme il faut sous 
		 * Magic 5 le bougre efface purrement et simplement le bloc !
		 * donc le LDG, on laisse faire le systeme alors de toute
		 * facon ce qui devra etre lib‚r‚ le sera
		 * En theorie c'est ce qu'il faudrait faire sous Mint
		
			if( is_Fcntl())
				Mfree(intern->libs[ilib].ldg->baspag); / * desaloc du block obtenu par Fcntl() * /
		 */
		}
	} else
	{
		flag_semaphore =0;
		return LDG_NOT_FOUND;	/* lib pas trouv‚e */
	}
	flag_semaphore =0;
	
	return 0;
}

void ldg_garbage( short *gl) {
	int i, j;

	
  	for( i=0; i<intern->maxlib; i++) {
  		/* Librarie charg‚e et non r‚sidente */
  		if( intern->libs[i].ldg && !(intern->libs[i].copy_of_flags & LDG_RESIDENT)) {
  			for( j=0; j<intern->maxclient; j++) {
  				if( (intern->libs[i].usedby[j] != -1) && (mt_AESapid(gl)!=intern->libs[i].usedby[j]) /* cas util et rapide sur systeme monotache */) {
  					/* Pseudo ping du client */
  				/*	if( mt_appl_find( intern->apps[intern->libs[i].usedby[j]], gl) == -1) */
  					if( mt_appl_find( (char *)((unsigned long)intern->libs[i].usedby[j]|0xFFFE0000L), gl) == -1) /* recherche par l'indice AES */
  						intern->libs[i].usedby[j] = -1;
  				}
  			}
  			if( get_app( -1, intern->libs[i].usedby) == 0) {
#if 0
/* le close_shm() est suffisant pour effacer les blocs quand il n'y a plus de client */
				char path[50]; int fd;
      				
	 			sprintf( path, LDG_SHM_FILENAME, i);
	 	 		fd = (int)Fopen( path, FO_RW);
	  			/* erreur ou SHM non support‚ */
	  			if( fd >= 0) {	
	   	 			char *blk;
      					
	    				blk = (char *)Fcntl( fd, 0L, SHMGETBLK);
	    				Fclose( fd);
	    				Mfree(blk);
	    			}
	    			sprintf( path, LDGBK_SHM_FILENAME, i);
	 	 		fd = (int)Fopen( path, FO_RW);
	  			/* erreur ou SHM non support‚ */
	  			if( fd >= 0) {	
	   	 			char *blk;
      					
	    				blk = (char *)Fcntl( fd, 0L, SHMGETBLK);
	    				Fclose( fd);
	    				Mfree(blk);
	    			}
#endif  				
  				
  			/*	ldg_unload( intern->libs[i].ldg); on n'a pas le droit d'utiliser ldg_unload() cela ne se peut que par le client appeleur vu que la zone passage environnement est allou‚ par lui et pas par le TSR */
  				close_shm( i); /* le close_shm() est suffisant pour effacer les blocs quand il n'y a plus de client */
  				*(intern->libs[i].name) = '\0';
  				intern->libs[i].ldg = NULL;
  			}
  		}
  	}
  	
}

void __CDECL cdecl_ldg_garbage( short *gl) {
	int count =0;
	Syield();
	while(flag_semaphore) 
	{
		count++;
		if(count>5) return;
		Syield();
	}
	flag_semaphore=1;
	ldg_garbage( gl);
	flag_semaphore=0;
}

/* on supprime toutes les libs */

void __CDECL cdecl_ldg_release( short *gl) {
	int i, j;

	Syield();
  	while(flag_semaphore) Syield();
  	flag_semaphore=1;
  	
	for( i=0; i<intern->maxlib; i++) {
		if( intern->libs[i].ldg) {
			*(intern->libs[i].name) = '\0';
			for( j=0; j<intern->maxclient; j++) {
				if( intern->libs[i].usedby[j] != -1) {
					/* On informe les clients de la terminaison
					 * des libraries */
					ldg_snd_msg( mt_AESapid(gl), intern->libs[i].usedby[j],
								 LDG_QUIT, ADR2WORD( intern->libs[i].ldg),
								 2, /* LDG 1.xx sp‚cifie cette valeur qui indique
								 	 * la raison de la terminaison */
								 0, 0, gl);		
				}
			}
			Syield();
			Syield();
      		/*	ldg_unload( intern->libs[i].ldg); ne surtout pas utiliser cela ! */
      			{
      				char path[50]; int fd;
      				
	 			sprintf( path, LDG_SHM_FILENAME, i);
	 	 		fd = (int)Fopen( path, FO_RW);
	  			/* erreur ou SHM non support‚ */
	  			if( fd >= 0) {	
	   	 			char *blk;
      					
	    				blk = (char *)Fcntl( fd, 0L, SHMGETBLK);
	    				Fclose( fd);
	    				Mfree(blk);
	    			}
#if 0
	    			sprintf( path, LDGBK_SHM_FILENAME, i);
	 	 		fd = (int)Fopen( path, FO_RW);
	  			/* erreur ou SHM non support‚ */
	  			if( fd >= 0) {	
	   	 			char *blk;
      					
	    				blk = (char *)Fcntl( fd, 0L, SHMGETBLK);
	    				Fclose( fd);
	    				Mfree(blk);
	    			}
#endif
      			}
      			close_shm( i);			
			intern->libs[i].ldg = NULL;
    	}
    }
    flag_semaphore =0;
}

void* __CDECL cdecl_ldg_find( char *name_fct, LDG *ldg) {
	static int lastpos = 0;
	int i, j;

	if( ldg == NULL) return NULL;
	for( i=0; i < ldg->num; i++) {
		j = (i + lastpos) % ldg->num;
		if(!strcmp( ldg->list[j].name, name_fct)) {
			lastpos = j+1;
			return((void *)ldg->list[j].func);
		}
	}
	return(NULL);
}

/* EOF */

