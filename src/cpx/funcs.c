/* -----------------------------------------------------------------
 *	CPX LDG Version 2.20
 *	Ce programme est freeware et fait partie
 *	du package LDG
 *	Tous droits r‚serv‚s … l'auteur (c) 1999
 * -----------------------------------------------------------------
 *	Auteur : Dominique B‚r‚ziat, 1999
 *	Module: funcs.c
 *	But : les actions du CPX, les popups
 *	Tabulation : 4
 * -----------------------------------------------------------------
 * $Id: funcs.c 27 2005-09-10 21:21:38Z bereziat $
 * -----------------------------------------------------------------
 */

#include <av.h>
#undef NULL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __PUREC__
#include <tos.h>
#else
#include <osbind.h>
#define FO_WRITE 1
#endif
#include "ldgcpx.h"
#include "global.h"

extern LDG*			ldg;
extern OBJECT*		dialog;
extern CPX_PARAMS*	params;
extern CPX_SAVE		conf;
extern char**		free_string;
extern int			cpxid;
extern short*		global;
extern LDG_INFOS*	cook;
extern int		   	request_listlib;
extern GRECT	   	cpx_wind;
extern struct ldg_infos intern;

char *appl_name( int id);

/* Appel du menu popup MENU */

char *mainMENU[] = { "  Configure    F1 ",
					 "  Libraries    F2 ",
					 "  Expert       F3 ",
					 "  Information  F4 " };

int call_menu( int button, int *lastform, GRECT *work) {
	int res, item;
	GRECT pop;
	
	switch( *lastform) {
	case CONFIGURE:
		item = 0;
		break;
	case LIBRARY:
		item = 1;
		break;
	case EXPERT:
		item = 2;
		break;
	case INFORMATION:
		item = 3;
		break;
	}
	mt_objc_offset( dialog, button, &pop.g_x, &pop.g_y,aes_global);
	pop.g_w = dialog[button].ob_width;
	pop.g_h = dialog[button].ob_height;
	res = (*params->do_pulldown)( mainMENU, 4, item, LARGE, &pop, work);
	switch( res ) {
	case 0:
		*lastform = CONFIGURE;
		break;
	case 1:
		*lastform = LIBRARY;
		break;
	case 2:
		*lastform = EXPERT;
		break;
	case 3:
		*lastform = INFORMATION;
		break;
	
	}
	return res;
}

/* Choix du r‚pertoire LDG */

void select_path( void) {
	char path[128], *p;
	short res;

	strcpy( path, conf.path);
	add_slash(path);
	strcat( path, "*.*");
	if (mt_fsel_input( path, "", &res, aes_global) && res) {
		p = strrchr( path, '\\');
		if( p) *(p+1) = '\0';
		strcpy( conf.path, path);
		format_filename( TEXT(CNF_PATH), path, LEN_PATH-1);
	}
}


char *list_error[] = {
	"Manager error (libs fulled)",
	"Manager error (apps fulled)",
	"Pexec error",
	"Bad format",
	"Strange error",
	"Lib locked",
	"No manager",
	"Lib not found",
	"Memory exhausted",
	"Time idle",
	"No TSR",
	"Functionality not supported"
	};

void request_lib( char *lib) {
	char *p;

#ifdef USE_LDGLOAD
	void ldg_unload( int, LDG*);
	LDG* ldg_load( int, char*);

	if( ldg) ldg_unload( mt_AESapid(aes_global), ldg);
	ldg = ldg_load( mt_AESapid(aes_global), lib);
#else
	if( ldg) ldg_close( ldg, aes_global);
	ldg = ldg_open( lib, aes_global);
#endif
	
	
	if( ldg) {
		p = strrchr( lib, '\\');
		strcpy( TEXT(LIB_SELECT), p+1);
		/* itoa( ldg->vers, TEXT(LIB_VERSION), 16); */
		sprintf( TEXT(LIB_VERSION), "%1x%02x", ldg->vers>>8, ldg->vers & 0x00FF);
		strcpy( TEXT(LIB_SHARE), (ldg->flags & LDG_NOT_SHARED)?"no ":"yes");
		strcpy( TEXT(LIB_LOCKED), (ldg->flags & LDG_LOCKED)?"yes":"no ");
		strcpy( TEXT(LIB_RESIDENT), (ldg->flags & LDG_RESIDENT)?"yes":"no ");
		strcpy( TEXT(LIB_CLOSURE), (ldg->close)?"yes":"no ");
		if( ldg->vers_ldg >= 0x100) {
		/*	itoa( ldg->vers_ldg, TEXT(LIB_PROTOCOL), 16); */
			sprintf( TEXT(LIB_PROTOCOL), "%1x%02x", ldg->vers_ldg>>8, ldg->vers_ldg & 0x00FF);
			strcpy( TEXT(LIB_EXT), ldg->user_ext?"yes":"no ");
			strcpy( TEXT(LIB_SPEC), ldg->addr_ext?"yes":"no ");	
		}
		/* itoa( ldg->num, TEXT(LIB_LIST), 10); */
		sprintf( TEXT(LIB_LIST), "%d", ldg->num);
		strcat( TEXT(LIB_LIST), " functions");
		draw_tree( dialog, ROOT);
	} else {
		char alrt[80];
		int err = ldg_error();

		if( err < 0 && err >= LDG_BAD_TSR) {
			sprintf( alrt, "[1][LDG error %d:|%s][OK]", err, list_error[-err-1]);
			mt_form_alert( 1, alrt, aes_global);
		} else {
			sprintf( alrt, "[1][LDG error %d:|unknown][OK]", err);
			mt_form_alert( 1, alrt, aes_global);
		}
	}
}

void list_functions( int button, GRECT *work) {
	if( ldg) {
		char **list;
		GRECT pop;
		int i;
		
		list = (char **)malloc( sizeof(char*)*(ldg->num+1));
		list[0] = " *General infos* ";
		for( i = 0; i<ldg->num; i++)
			list[i+1] = ldg->list[i].name;
		mt_objc_offset( dialog, button, &pop.g_x, &pop.g_y, aes_global);
		pop.g_w = dialog[button].ob_width;
		pop.g_h = dialog[button].ob_height;
		i = (*params->do_pulldown)( list, ldg->num+1, -1, LARGE, &pop, work);
		free( list);
		if( i != -1) {
			if( i == 0)
				fill_comment( "%s", ldg->infos);
			else
				fill_comment( "%s", ldg->list[i-1].info);
			select_form( COMMENT);
			draw_tree( dialog, ROOT);
		}
	}
}

char *actionMENU[]={ "  Garbage collecting  F11 ",
					 "  Reset               F12 ",
					 "  List libs           F13 ",
					 "  Internal infos      F14 ",
					 "  Shutdown            F15 "};

int call_action( int button, GRECT *work, int items) {
	GRECT pop;
	
	mt_objc_offset( dialog, button, &pop.g_x, &pop.g_y, aes_global);
	pop.g_w = dialog[button].ob_width;
	pop.g_h = dialog[button].ob_height;
	return (*params->do_pulldown)( actionMENU, items, -1, LARGE, &pop, work);
}

char *sndtoMENU[]= { "  Tsr     F16 ",
					 "  Manager F17 " };

int call_sndto( int button, int def, GRECT *work) {
	GRECT pop;
	
	mt_objc_offset( dialog, button, &pop.g_x, &pop.g_y, aes_global);
	pop.g_w = dialog[button].ob_width;
	pop.g_h = dialog[button].ob_height;
	return (*params->do_pulldown)( sndtoMENU, 2, def, LARGE, &pop, work);
}

void lib_select( void) {
	char path[128];
	char name[20], *p;
	short res;

	strcpy( path, conf.path);
	add_slash( path);
	strcat( path, "*.ldg");
	*name = '\0';
	if( mt_fsel_input( path, name, &res, aes_global) && res) {
		p = strrchr( path, '\\');
		if( p) *(p+1) = '\0';
		strcat( path, name);
		request_lib( path);
	}
}

/* Envoie une requete au manager ou au TSR */

void snd_request( int act, int to) {
	if( cook == NULL || cook->version < LDG2_VERS ) 
		to &= ~TO_TSR;
	
	switch( act) {
	case 0:
		/* Garbage */
		if( to == TO_MGR)
		  ApplWrite( mt_appl_find( "LDGMGR  ", aes_global), LDG_GARBAGE);
		else
			(*cook->ldg_garbage)( aes_global);			
		break;
	case 1:
		/* Reset manager */
		if( to == TO_MGR)
		  ApplWrite( mt_appl_find( "LDGMGR  ", aes_global), LDG_FORCE);
		else
			(*cook->ldg_reset)( aes_global);
		break;
	case 2:
		{
			char **poplib;
			struct ldg_lib *libs;
			GRECT pop;
			int i, nlib = ldg_loadlib( &intern);
							  
			poplib = (char **)malloc( sizeof(char*)*(nlib+1));
			libs = (struct ldg_lib *)malloc( sizeof(struct ldg_lib)*(nlib+1));
			poplib[0] = nlib?" Libs loaded: ":" Any lib loaded";
			ldg_initlist( &intern);
			for( i=1; i<=nlib; i++) {
				ldg_getlib( &intern, libs+i);
				poplib[i] = libs[i].name;
			}
			mt_objc_offset( dialog, EXP_REQUEST, &pop.g_x, &pop.g_y, aes_global);
			pop.g_w = dialog[EXP_REQUEST].ob_width;
			pop.g_h = dialog[EXP_REQUEST].ob_height;
			i = (*params->do_pulldown)( poplib, nlib+1, -1, LARGE, &pop, &cpx_wind);
			if( i>0) {
				int j, k;
				sprintf( TEXT(LINE1), "Lib '%s' used by: ", libs[i].name);
				for( j=1; j<6; j++) TEXT(LINE1+j) [0] = '\0';
				for( k=1, j=0; j<intern.maxclient && k<6; j++) {
					if( libs[i].usedby[j] != -1)
						sprintf( TEXT(LINE1+k++), "  + %d '%s'", 
								libs[i].usedby[j],
								appl_name( libs[i].usedby[j]));
				}
				select_form( COMMENT);
				draw_tree( dialog, ROOT);
			}
			free( poplib);
			free( libs);

		}
		request_listlib = 1;
		break;
	case 3:
		/* Internal infos */
		if( to == TO_MGR)
		  ApplWrite( mt_appl_find( "LDGMGR  ", aes_global), VA_START);
		else {
			fill_comment( "Max libs: %d\nMax Apps: %d\nMax lib per apps: %d", 
						  intern.maxlib, intern.totalclient, intern.maxclient);
			select_form( COMMENT);
			draw_tree( dialog, ROOT);
		}
		break;
	case 4:
		/* Shutdown */
		if( to == TO_MGR)
		  ApplWrite( mt_appl_find( "LDGMGR  ", aes_global), AP_TERM);
		break;
	}
}


/* requete sur le TSR, LDG 2.xx */


/*
 *	Copie le formulaire COMMENT vers le presse papier GEM
 */

void copy_clipbd( void) {
	char path[128];
	int  fp;
	int  line;
	
	if( !mt_scrp_read(path,aes_global)) {
		return;	/* Pas de dossier clipboard install‚ */
	}
	strcat( path, "scrap.txt");
	fp = (int)Fopen( path, FO_WRITE);
	for( line = LINE1; line <= LINE6; line++)
		Fwrite( fp, strlen(TEXT(line)), TEXT(line));
	Fwrite( fp, 2, "\r\n");
	Fclose( fp);
}

/* EOF */
