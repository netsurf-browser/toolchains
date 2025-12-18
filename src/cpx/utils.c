/* -----------------------------------------------------------------
 *	CPX LDG Version 2.21
 *	Ce programme est freeware et fait partie
 *	du package LDG
 *	Tous droits r‚serv‚s … l'auteur (c) 1999
 * -----------------------------------------------------------------
 *	Auteur : Dominique B‚r‚ziat, 1999
 *	Module: utils.c
 *	But : fonctions secondaires
 *	Tabulation : 4
 * -----------------------------------------------------------------
 * $Id: utils.c 27 2005-09-10 21:21:38Z bereziat $
 * -----------------------------------------------------------------
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <av.h>
#ifdef __PUREC__
#include <tos.h>
#else
#include <osbind.h>
#endif
#include "ldgcpx.h"
#include "global.h"

/* Variables externes
 */
extern GRECT 	  cpx_wind;	/* Taille et coord. du CPX */
extern LDG_INFOS *cook;
extern OBJECT	 *dialog;
extern int		  currform, lastform;
extern CPX_SAVE   conf;
extern int		  cpxid;
extern int		  snd_to;
extern struct ldg_infos	intern;

/*
 * Formatte le nom d'un fichier dans un champ
 * trop petit.
 */
 
void format_filename( char *dest, char *src, int size) {
	int len = (int)strlen( src);
	int middle = size/2;
	char buf [255];
	
	if( len > size) {
		strncpy( buf, src, size);
		buf[middle-3] = '\0';
		strcat( buf, "...");
		strcat( buf, src + len - middle - size%2);
		buf[size]='\0';
		strcpy( dest, buf);
	} else
		strcpy( dest, src);
}

/*
 * Ajoute un backslash si besoin
 */

void add_slash( char *p) {
	if( p[strlen(p)-1] != '\\')
		strcat( p, "\\");
}

/*
 * dessin d'un object avec le bon clippage
 */

void draw_object( OBJECT *tree, int object) {
	mt_objc_draw( tree, object, MAX_DEPTH, 
		      cpx_wind.g_x,cpx_wind.g_y,cpx_wind.g_w,cpx_wind.g_h, aes_global);	
}

/*
 *	dessin du formulaire et mise … jour de sa position
 */

void draw_tree(OBJECT *tree, int root) {
	tree[0].ob_x = cpx_wind.g_x;
	tree[0].ob_y = cpx_wind.g_y;
	draw_object( tree, root);
}

/*
 *	Interroge un gestionnaire et met … jour 
 *	le formulaire.
 */

void select_manager( int to) {
	if( to == TO_MGR)
		ldg_mgrlist( &intern, aes_global);
	else
		ldg_libslist( &intern, aes_global);
/*	itoa( ldg_loadlib( &intern), TEXT(EXP_LIBLOADED), 10); */
	sprintf( TEXT(EXP_LIBLOADED), "%d", ldg_loadlib( &intern));
}

/*
 *	Appliquer les r‚glages du formulaire 
 *	(valable en fait que pour le cookie)
 */

void get_form( void) {
	switch( currform) {
	case CONFIGURE:
		if( cook) {	
			strcpy( cook->path, conf.path);
			cook->garbage = atoi( TEXT(CNF_GARBAGE));
		}
		/* Pour la sauvegarde du CPX */
		conf.garbage  = atoi( TEXT(CNF_GARBAGE));
		break;
	case EXPERT:
		if( cook)
			cook->idle    = atoi( TEXT(EXP_IDLE));
		/* Pour la sauvegarde du CPX */
		conf.idle     = atoi( TEXT(EXP_IDLE));
		break;
	}
}

/*
 * Choix du formulaire et initialise le formulaire.
 */

void select_form( int form) {
	short dum;
	char *p;
	
	lastform = currform;
	currform = form;
	
	/* Cacher les formulaires, sauf l'actif */
	dialog[CONFIGURE].ob_flags 		|= HIDETREE;
	dialog[EXPERT].ob_flags 		|= HIDETREE;
	dialog[LIBRARY].ob_flags 		|= HIDETREE;
	dialog[INFORMATION].ob_flags 	|= HIDETREE;
	dialog[COMMENT].ob_flags 		|= HIDETREE;

	/* reactiver les boutons globaux si besoin */
	if( lastform == COMMENT) {
		dialog[CNF_SAVE].ob_state &= ~DISABLED;
		dialog[CNF_CANCEL].ob_state &= ~DISABLED;
		dialog[CNF_MENU].ob_state &= ~DISABLED;
		strcpy( dialog[CNF_OK].ob_spec.free_string,  "Ok");
	}

	/* recaler l'actif */	
	dialog[form].ob_flags 			&= ~HIDETREE;	
	dialog[form].ob_x = 0;
	dialog[form].ob_y = 0;
	
	/* D‚sactiver les objets editables */
	
	dialog[EXP_IDLE].ob_flags &= ~EDITABLE;
	dialog[CNF_GARBAGE].ob_flags &= ~EDITABLE;

	switch( form) {
	case CONFIGURE:
		/* Initialisation des champs */
		if( cook) {	
			/* itoa( cook->version, TEXT(CNF_VERSION), 16);*/
			sprintf( TEXT(CNF_VERSION), "%X", cook->version);
			strcpy ( conf.path, cook->path);
			format_filename( TEXT(CNF_PATH), cook->path, LEN_PATH-1);
			/* itoa( cook->garbage, TEXT(CNF_GARBAGE), 10); */
			sprintf( TEXT(CNF_GARBAGE), "%d", cook->garbage);
		} else {
		  if( mt_shel_envrn( &p, "LDGPATH=", aes_global)) {
				strcpy( conf.path, p);
				format_filename( TEXT(CNF_PATH), p, LEN_PATH-1);
			}
		  if( mt_shel_envrn( &p, "LDG_GARBAGE=", aes_global))
				strcpy( TEXT(CNF_GARBAGE), p);
		}

		/* Curseur sur champs ‚ditable */
		dum = 0;
		mt_objc_edit( dialog, CNF_GARBAGE, 0, &dum, ED_INIT, aes_global);
		dialog[CNF_GARBAGE].ob_flags |= EDITABLE;
		break;

	case LIBRARY:
		break;
	case EXPERT:
		/* Initialisation des champs */
		select_manager( snd_to);
		if( cook) 
			/* itoa( cook->idle, TEXT(EXP_IDLE), 10);*/
			sprintf( TEXT(EXP_IDLE), "%d", cook->idle);
		else {
		  if( mt_shel_envrn( &p, "LDG_IDLE=",aes_global))
				strcpy( TEXT(EXP_IDLE), p);
		}
		/* Curseur sur champ ‚ditable */
		dum = 0;
		mt_objc_edit( dialog, EXP_IDLE, 0, &dum, ED_INIT, aes_global);
		dialog[EXP_IDLE].ob_flags |= EDITABLE;
		break;
	case COMMENT:
		dialog[CNF_SAVE].ob_state |= DISABLED;
		dialog[CNF_CANCEL].ob_state |= DISABLED;
		dialog[CNF_MENU].ob_state |= DISABLED;
		strcpy( dialog[CNF_OK].ob_spec.free_string,  "<<");
		break;	
	case INFORMATION:
		strcpy( TEXT(CNF_CPXVERSION), CPXVERSION);		
		break;
	}
}

#define CLEN 30
#define NLINE 6

void fill_comment( char *fmt, ...) {
	int i,j,k;
	char buf[CLEN*NLINE];
	char *q, *p = NULL;
	va_list arg;
	
	/* Copie des arguments dans un buffer temporaire */
	va_start( arg, fmt);
	vsprintf( buf, fmt, arg);
	va_end( arg);
	
	/* Mise … z‚ro des champs */
	for( i=0; i<NLINE; memset( TEXT(LINE1+i++), 0, CLEN));
	/*	 *TEXT(LINE1+i++)='\0'); */

	/* Copie du buffer vers les champs */
	q = buf;
	p = TEXT(LINE1);
	for( i=j=k=0; i < CLEN*NLINE && *q; i++) {
		if( (k && k % CLEN == 0) || *q == '\n') {
			j ++;
			p = TEXT(LINE1+j);
			k = 0;
		} 

		if( *q == '\n')
			q ++;	/* avancer sans copier */
		else {
			*p++ = *q++;
			k ++;
		}
	}
	*p = '\0';
}

/*
 *	Binding AES 
 */

#define AES_PROCESS		4

int is_appl_search( void) {
	short dum, parm3;

	if( mt_appl_find( "?AGI", aes_global) != -1) {
	  mt_appl_getinfo( AES_PROCESS, &dum, &dum, &parm3, &dum,aes_global);
		if( parm3 == 1) return 1;	
	}
	return 0;
}

char *appl_name( int id) {
	short i,m=0x0F;
	static char name[9] = "???"; 
	char name2[9];
	
	if( is_appl_search() &&  mt_appl_search(0, name2, &m, &i, aes_global) && i != id) {
	  while( mt_appl_search(1, name2, &m, &i, aes_global)) {
			if( i == id )
				break;
		}
	}
	sscanf( name2, "%s", name);
	return name;
}

/*
 *	Execute une appli GEM
 */

void exec_app( char *ap, char *cmd) {
	char *p, *q, name[9];
	int id;
	char *app = strdup( ap);
	
	p = strrchr( app, '\\');
	if( p)  p++;
	else	p = app;
	q = strrchr( p, '.');
	if( q) *q = '\0';
	strupr( p);
	strcpy( name, "        ");
	for( id=0; id < strlen(p); id++)
		name[id]=p[id];
	id = mt_appl_find(name,aes_global);
	if( id == -1) {
	  id = mt_shel_write( 1, 1, 100, ap, cmd, aes_global);
	  mt_evnt_timer( 2000, aes_global);
	}
	ApplWrite( id, VA_START, cmd);
	free( ap);
}

struct objcolor {
		unsigned borderc:4;
		unsigned textc  :4;
		unsigned opaque :1;
		unsigned pattern:3;
		unsigned fillc  :4;
	};

void editable3d( OBJECT *tree) {
	int res;
#define COOKIE_MAGX 0x4D61658UL
	if( !ldg_cookie( COOKIE_MAGX, NULL)) {
		res = 0;
		while( !(tree[res].ob_flags & LASTOB)) {
			if( tree[res].ob_type == G_FTEXT &&
				!(tree[res].ob_state & (1<<15))) {
				struct objcolor *color;
			
				tree[res].ob_state |= SELECTED;
				tree[res].ob_flags |= (1<<9);
				tree[res].ob_flags |= (1<<10);
				color = (struct objcolor*)&tree[res].ob_spec.tedinfo->te_color;
				color -> fillc = 8;
			}
			res ++;
		}
	}
}

/*
 * La fonction appl_write en plus pratique (extrait de WinDom)
 */

int ApplWrite( int to, int msg, ...) {
	va_list args;	
	short buf[8];

	buf[0] = msg;
	buf[1] = cpxid;
	buf[2] = 0;
	va_start( args, msg);
	buf[3] = va_arg( args, short);
	buf[4] = va_arg( args, short);
	buf[5] = va_arg( args, short);
	buf[6] = va_arg( args, short);
	buf[7] = va_arg( args, short);
	va_end( args);
	return mt_appl_write( to, 16, buf, aes_global);
}

/*
 * Trouver l'id de BubbleGEM. Au besoin, le charger.
 */

int BubbleGet( void) {
  int bubble_id = mt_appl_find("BUBBLE  ", aes_global);

        if( bubble_id < 0 && mt_AESnumapps(aes_global) != 1) {
		char *value;
    	
		mt_shel_envrn( &value, "BUBBLE=", aes_global);
		if( !value) mt_shel_envrn( &value, "BUBBLEGEM=", aes_global);
    	if( value)
	  mt_shel_write( 1, 1, 100, value, "", aes_global);
    	else 
    		return -2;

    	mt_evnt_timer( 500,aes_global);
    	bubble_id = mt_appl_find("BUBBLE  ",aes_global);
	}
	return bubble_id;
}

#define BGS7_MOUSE 		0x0004
#define BUBBLEGEM_SHOW	0xBABB

int BubbleCall( char *help, int x, int y) {
	char *bubble_buf;
	int bubble_id;
	
	bubble_id = BubbleGet();

    if (bubble_id >= 0) {

    	/* Demander de la m‚moire pour la communication */
    	bubble_buf = ldg_Malloc( 256);
		if( bubble_buf == NULL) return -3;
	
		/* Affichage de la bulle */
		strcpy(bubble_buf, help);
		ApplWrite( bubble_id, BUBBLEGEM_SHOW, x, y, bubble_buf, BGS7_MOUSE);
 		
		/* Lib‚ration m‚moire */        
        ldg_Free( bubble_buf);

        return 0;
    }
    return bubble_id;
}

/* EOF */
