/*
 *	Installateur pour LDG 2.xx
 *	Le code est en dur mais facile … mettre
 *	… jour grace … FILEUTIL.LDG
 *  Version 2.10 : pour version LDG 2.30
 *
 *  Ecriture du logfile qui sert de desintallateur
 *  on pourra ‚crire un simple desinstallateur utilisant
 *  fileutil.ldg
 *
 * (c) 2003 by Dominique B‚r‚ziat
 */


#include <av.h>
#include <stdio.h>
#ifdef __PUREC__
 #include <tos.h>
 #include "..\..\src\version.h"	/* Pour connaitre la version de la distrib LDG */
#else
 #include <osbind.h>
 #include "../../src/version.h"	/* Pour connaitre la version de la distrib LDG */
#endif
#include <windom.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ldg.h>
#include <time.h>
#include "viewtext.h"


#define BIND_FILEUTIL
#include <ldg/fileutil.h>

#include "install.h"

/* #define DEBUG_INSTALL */

#define INSTALL_VERSION "2.10"
#define DATE	"2002"
#define LOGNAME "uninstal.log"

#define INST_SYS 0x1
#define INST_SDK 0x2
#define INST_UNX 0x4

#define LIBGCC GCCBUT
#define LIBGCC295 GCC295BUT
#define LIBSOZ SOZBUT
#define LIBPC  PCBUT

#define AUTOSTR		ObjcString( FORM(win), AUTOPATH, NULL)
#define CPXSTR		ObjcString( FORM(win), CPXPATH, NULL)
#define STARTSTR	ObjcString( FORM(win), STARTPATH, NULL)
#define LDGSTR		ObjcString( FORM(win), LDGPATH, NULL)
#define XTRASTR		ObjcString( FORM(win), XTRAPATH, NULL)
#define HYPSTR		ObjcString( FORM(win), HYPPATH, NULL)
#define INCSTR		ObjcString( FORM(win), INCPATH, NULL)
#define LIBSTR		ObjcString( FORM(win), LIBPATH, NULL)
#define BINSTR		ObjcString( FORM(win), BINPATH, NULL)
#define MANSTR		ObjcString( FORM(win), MANPATH, NULL)

void EditFsel( WINDOW *win, int index, char *title) {
	char path[128], name[64];

	strcpy( path, ObjcString( FORM(win), index, NULL));
	strcpy( name, "");
	if( FselInput( path, name, "*", title, NULL, NULL)) {
		if( path[strlen(path)-1] == '\\')
			path[strlen(path)-1] = '\0';
		strcpy( ObjcString( FORM(win), index, NULL), path);
		ObjcDraw( OC_FORM, win, index, 1);
	}
}

/*

   System : AUTO, CPX, GEMSYS/LDG, GEMSYS/MAGIC/START
            GUIDES, XTRAS (ldg_info)
   DSK :    INCLUDE, LIB, XTRAS (demo source)
   		 	MAN (CAT3L)
   UNIX :   BIN, LIB, MAN (CAT1)

 */


WINDOW *ViewText( char *file, char *title, int attrib, int x, int y, int w, int h) {
	WINDOW *win; 
	
	win = WindFind( WDF_NAME, title);
	if( win)
		WindTop( win);
	else {
		win = ViewTextCreate( file, attrib);
		if( !win) return NULL;
		WindSetStr( win, WF_NAME, title);
		ViewTextOpen( win, x, y, w, h);
	}
	return win;
}

/* Variables globales
 ***************************************************************************/

FILE *log = NULL; 				/* log file */
int inst_flags = INST_SYS;		/* options de l'installation */
int type_lib =   LIBGCC;		/* type de librairie de developpement */
int warning = FALSE;
LDG *ldg;
WINDOW *wdial;
INT16 lang = LAN_EN;				/* Language par d‚faut */

/* Fontions events
 ***************************************************************************/
 
void ApTerm( void) {
	while( wglb.first) {
		ApplWrite( app.id, WM_DESTROY, wglb.first->handle, 0, 0, 0, 0);
		EvntWindom ( MU_MESAG);
	}
	ldg_close( ldg, ldg_global);
	RsrcFree();	
	ApplExit();
	exit( 0);
}

void SndTerm( void) {
	ApplWrite( app.id, AP_TERM, 0, 0, 0, 0, 0);
}

void MnSelected( void) {
	int title = evnt.buff[3];
	if( evnt.buff[4] == MN_ABOUT) {
		FormAlert( 1, "[0][LDG installer version " INSTALL_VERSION "|by D.B‚r‚ziat][OK]");
	}
	MenuTnormal( NULL, title, 1);
}

OBJECT *get_tree( int ind) {
	OBJECT *tree;
	rsrc_gaddr( 0, ind, &tree);
	return tree;
}

char *get_string( int ind) {
	char *p;
	rsrc_gaddr( 5, ind, &p);
	return p;
}

void message( char *fmt, ...) {
	OBJECT *tree = get_tree( MAINFORM);
	INT16 x, y;
	static char buf[512];
	va_list arg;
	
	va_start( arg, fmt);
	vsprintf( buf, fmt, arg);
	va_end( arg);

	objc_offset ( tree, INFO_LINE, &x, &y);
	strncpy( ObjcString ( tree, INFO_LINE, NULL), buf, 43);
	ObjcString ( tree, INFO_LINE, NULL)[43] = '\0';
	ObjcWindDraw( wdial, tree, INFO_BACK, 1,
				  x, y, tree[INFO_LINE].ob_width, tree[INFO_LINE].ob_height);
}

int cp_verb( char *src, char *dest) {
	int ret;
	MouseWork();
	message( "cp %s %s", src, dest);
	if( log) {
		fprintf( log, "# copy file(s) %s into %s\n", src, dest);
		fprintf( log, "rm %s\\%s\n", dest, strrchr(src, '\\')? strrchr(src, '\\') + 1 : src);
	}
	ret = cp( src, dest);
	if( ret < 0) warning = TRUE;
#ifdef DEBUG_INSTALL
	if( log) fprintf( log, "# install %d objects\n", ret);
#endif
	return ret;
}

int mkdir_verb( char *src) {
	int ret;
	MouseWork();
	message( "mkdir %s", src);
	if( log) {
		long fatt = Fattrib( src, 0, 0);
		if( fatt >= 0 && fatt & 0x10 /* FA_SUBDIR*/ ) 
			fprintf( log, "# create directory %s (already created, will be removed if empty)\n", src);
		 else 
			fprintf( log, "# create directory %s (will be removed if empty) \n", src);
		fprintf( log, "rmdir %s\n", src);
	}
	ret = mkdir( src);
	if( ret < 0) warning = TRUE;
#ifdef DEBUG_INSTALL
	if( log) fprintf( log, "# install %d objects\n", ret);
#endif
	return ret;
}


/* Fonctions du formulaire
 ***************************************************************************/

/* La selection d'un label d‚clenche la selection du bouton 
 * qui s'y rattache */

void install_label( WINDOW *win, int obj) {
	switch( obj) {
	case SYSTEMLABEL:
		inst_flags ^= INST_SYS;
		FORM(win)[SYSTEMBUT].ob_state ^= SELECTED;
		break;
	case SDKLABEL:
		inst_flags ^= INST_SDK;
		FORM(win)[SDKBUT].ob_state ^= SELECTED;
		break;
	case UNIXLABEL:
		inst_flags ^= INST_UNX;
		FORM(win)[UNIXBUT].ob_state ^= SELECTED;
		break;
	}
	ObjcDraw( OC_FORM, win, obj, 1);
}

void lib_label( WINDOW *win, int obj) {
	switch( obj) {
	case GCCLABEL:
		ObjcChange( OC_FORM, win, GCCBUT, SELECTED, TRUE);
		break;
	case SOZLABEL:
		ObjcChange( OC_FORM, win, SOZBUT, SELECTED, TRUE);
		break;
	case PCLABEL:
		ObjcChange( OC_FORM, win, PCBUT, SELECTED, TRUE);
		break;
	}
}

void quit( WINDOW *win, int index) {
	ObjcChange( OC_FORM, win, index, ~SELECTED, TRUE);
	ApplWrite( app.id, AP_TERM, 0, 0, 0, 0, 0);
}

void help( WINDOW *win, int index) {
	ObjcChange( OC_FORM, win, index, ~SELECTED, TRUE);
	ViewText( (lang==LAN_EN)?"install\\HELP":"install\\AIDE"/* "install\\src\\test"*/,
			  (lang==LAN_EN)?"Help on LDG Installer":"Aide de l'installateur LDG", 
			  WAT_NOINFO, -1, -1, -1, -1);
}

void info( WINDOW *win, int index) {
	static OBJECT *tree = NULL;

	if( tree == NULL) {
		rsrc_gaddr( 0, INFO_ROOT, &tree);
		/* Fill in formular */
		strcpy( ObjcString( tree, INFO_INSTALL, NULL), INSTALL_VERSION);
		sprintf( ObjcString( tree, INFO_LDG, NULL), "%d.%02x",
			ldg_version.num >> 8, ldg_version.num);
		sprintf( ObjcString( tree, INFO_GEMLIB, NULL), "%d.%d.%d",
				 __GEMLIB_MAJOR__, __GEMLIB_MINOR__, __GEMLIB_REVISION__);
		sprintf( ObjcString( tree, INFO_WINDOM, NULL), "%d.%02x.%d",
				WinDom.patchlevel>>8, WinDom.patchlevel&0xFF,
				WinDom.release);
		sprintf( ObjcString( tree, INFO_FILEUTIL, NULL), "%d.%02x",
				ldg->vers>>8, ldg->vers&0xFF);
	}
	FormWindBegin( tree, "Install");
	FormWindDo(MU_MESAG);
	FormWindEnd();
	tree[INFO_CLOSE].ob_state &= ~SELECTED;	
	ObjcChange( OC_FORM, win, index, ~SELECTED, TRUE);
}


void move( WINDOW *win) {
	INT16 x, y, w, h;
	
	graf_mkstate( &x, &y, &w, &h);
	if( w) {
		WindGet( win, WF_CURRXYWH, &x, &y, &w, &h);
		wind_update( BEG_MCTRL);
		graf_dragbox( w, h, x, y, app.x, app.y, app.w, app.h, &x, &y);
		wind_update( END_MCTRL);
		ApplWrite( app.id, WM_MOVED, win->handle, x, y, w, h);
	} 
}

void icon( WINDOW *win) {
	INT16 x, y, w, h;
	OBJECT *icon = get_tree(APPICON);

	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);
	icon[1].ob_x = (icon[0].ob_width - icon[1].ob_width ) / 2; 
	icon[1].ob_y = (icon[0].ob_height - icon[1].ob_height ) / 2; 	
	icon->ob_x = x;
	icon->ob_y = y;
	WindClear( win);	
	objc_draw( icon, 0, 1, x, y, w, h);
}

char *getword( char **p) {
	char *q;
	/* Skip space */
	while( **p == ' ' ||
		   **p == '\t' )
		(*p) ++;
	q = (*p);
	/* Move after the word */
	while( **p != ' ' &&
		   **p != '\t' &&
		   **p != '\0')
		(*p) ++;
	/* Mark end of word */
	if( **p) {
		**p = '\0';
		(*p) ++;
	}
	return q;
}

#include <wout.h>

void exec_file( char *file) {
	FILE *fp = fopen( file, "r");
	char buf[256], *pb;
	char *compilo, *cmd, *arg1, *arg2;
	int section = 0;

	if( fp == NULL) return;
	while( fgets( buf, 255, fp)) {
		if( *buf == '#' || *buf == '\n' || *buf == '\t')
			continue;

		if( buf[strlen( buf) - 1 ] == '\n')
			buf[strlen( buf) - 1 ] = '\0';

		pb = buf;
		cmd = getword( &pb);

		if( *cmd == '[') {
			if( strstr(cmd, "[system]") == cmd)
				section = INST_SYS;
			else if( strstr(cmd, "[devel]") == cmd)
				section = INST_SDK;
			else if( strstr(cmd, "[unix]") == cmd)
				section = INST_UNX;
			continue;
		}

		if( system != 0  &&
			inst_flags & section) {

			compilo = "";
			if( *cmd == '?') {
				if( strstr(cmd, "?gcc") == cmd)
					compilo = "[gcc] ";
				else if( strstr(cmd, "?pc") == cmd)
					compilo = "[purec] ";
				else if( strstr(cmd, "?soz") == cmd)
					compilo = "[sozobon] ";
				cmd = getword( &pb);
			}
			arg1 = getword( &pb);
			if( strstr( cmd, "cp") == cmd) {
				arg2 = getword( &pb);
				wout_printf( "%scopy %s to %s\n", compilo, arg1, arg2);
			} else if( strstr( cmd, "mkdir") == cmd)
				wout_printf( "%screate dir %s\n", compilo, arg1);

		} else
			wout_printf( "warning : no section\n");
	}
	fclose( fp);
}

void install( WINDOW *win, int index) {
	char buf[255];
	int res;
	
	if( inst_flags == 0 
		&& FormAlert( 1, get_string( NOTHING_WARNING))
			== 1) {
		/* ApplWrite( app.id, AP_TERM); */
		ObjcChange( OC_FORM, win, index, NORMAL, TRUE);
		return;	
	}

	/* Le fichier LOG va dans le dossier des Extras */
	mkdir_verb( XTRASTR);
	sprintf( buf, "%s\\%s", XTRASTR, LOGNAME);
	log = fopen( buf, "w");
	if( log) {
		time_t timer;
		fprintf( log, "# LDG installer %s by D. B‚r‚ziat (c) %s\n", INSTALL_VERSION, DATE);
		time( &timer);
		fprintf( log, "# Installation on %s\n\n", ctime(&timer));
	}

	/* Installation du systeme */
#if 1
	if( inst_flags & INST_SYS) {

		mkdir_verb( AUTOSTR);
		cp_verb( "auto\\ldg.prg", AUTOSTR);
	
		mkdir_verb( CPXSTR);
		cp_verb( "cpx\\ldg.cpx", CPXSTR);

		mkdir_verb( LDGSTR);
		cp_verb( "gemsys\\ldg\\*.ldg", LDGSTR);
		cp_verb( "gemsys\\ldg\\ldgmgr.app", LDGSTR);

		mkdir_verb( STARTSTR);
		cp_verb( "gemsys\\magic\\start\\ldgd.app", STARTSTR);
	
		mkdir_verb( HYPSTR);
		cp_verb( "guides\\*.*", HYPSTR);

		/* Les extras */
		sprintf( buf, "%s\\ldg_info", XTRASTR);
		mkdir_verb( buf);
		cp_verb( "contrib\\ldg_info\\*.*", buf);

		cp_verb( "icon.rsc", XTRASTR);
	}

	if( inst_flags & INST_UNX) {
		mkdir_verb( BINSTR);
		cp_verb( "bin\\*.ttp", BINSTR);

		sprintf( buf, "%s\\cat1", MANSTR);
		mkdir_verb( buf);
		cp_verb( "man\\cat1\\*.1", buf);
	}

	/* Install SDK */
	if( inst_flags & INST_SDK) {
		/* LIB */

		mkdir_verb( LIBSTR);

		switch( type_lib) {
		case LIBGCC:
			cp_verb( "lib\\gcc281\\*.olb", LIBSTR);
			break;
		case LIBGCC295:
			cp_verb( "lib\\gcc295\\*.olb", LIBSTR);
			break;
		case LIBSOZ:
			cp_verb( "lib\\sozobon\\*.a", LIBSTR);
			break;
		case LIBPC:
			cp_verb( "lib\\purec\\*.lib", LIBSTR);
			break;
		}

		/* INCLUDE */
		mkdir_verb( INCSTR);
		cp_verb( "include\\*.h", INCSTR);

		sprintf( buf, "%s\\ldg", INCSTR);
		mkdir_verb( buf);
		cp_verb( "include\\ldg\\*.h", buf);

		/* MAN, si le drapeau INST_UNX est positionn‚ */
		
		if( inst_flags & INST_UNX) {								
			sprintf( buf, "%s\\cat3", MANSTR);
			mkdir_verb( buf);
			cp_verb( "man\\cat3\\*.3", buf);
			
			sprintf( buf, "%s\\cat3l", MANSTR);
			mkdir_verb( buf);
			cp_verb( "man\\cat3l\\*.3l", buf);
		}
		
		/* HYP */
		cp_verb( "hyp\\mem.*", HYPSTR);

		/* DEMO */
		sprintf( buf, "%s\\demo\\client",  XTRASTR);
		mkdir_verb( buf);
		cp_verb( "demo\\client\\*.*", buf);
		
		sprintf( buf, "%s\\demo\\libs",  XTRASTR);
		mkdir_verb( buf);
		cp_verb( "demo\\libs\\*.*", buf);

		sprintf( buf, "%s\\demo\\libs\\ldg",  XTRASTR);
		mkdir_verb( buf);
		cp_verb( "demo\\libs\\ldg\\*.*", buf);
	}
#else
	exec_file( "install\\INSTSYS");
#endif

	cp_verb( (lang==LAN_EN)?"install\\README":"install\\LISEZMOI",
			 XTRASTR);
	
	graf_mouse( ARROW, NULL);

	if( warning && FormAlert( 1, get_string( INSTALL_WARNING)) == 2) {
		goto END;
	}

	message( get_string( MSG_SUCCES));

	FormBegin( get_tree( SUCCES), NULL);
	res = FormDo( get_tree( SUCCES), -1);
	FormEnd( get_tree( SUCCES), NULL);

	if( res == SUCCES_ACTIVE) {

		/* Lancement du TSR */
	
		MouseWork();
		sprintf( buf, "%s\\ldg.prg", AUTOSTR);
		message( "exec %s", buf);
		ShelWrite( buf, "", NULL, FALSE, TRUE);
		
		/* Lancement du CPX */
		message( "exec LDG.CPX ...");
		if( appl_find( "COPS    ") != -1) {
			ApplWrite( appl_find( "COPS    "), VA_START, ADR("cpx\\ldg.cpx"), 0, 0, 0);
		} else
		if( appl_find( "AVSERVER") != -1) {
			ApplWrite( appl_find( "AVSERVER"), AV_STARTPROG, ADR("cpx\\ldg.cpx"), ADR(""), 0);
		} else 
		if( appl_find( "THING   ") != -1) {
			ApplWrite( appl_find( "THING   "), AV_STARTPROG, ADR("cpx\\ldg.cpx"), ADR(""), 0);
		} else
		if( appl_find( "MAGXDESK") != -1) {
			ApplWrite( appl_find( "MAGXDESK"), AV_STARTPROG, ADR("cpx\\ldg.cpx"), ADR(""), 0);
		} else
			FormAlert(1, get_string( CPX_ERROR));
	}

END:
	graf_mouse( ARROW, NULL);
	if( log) fclose( log);

	get_tree( SUCCES)[res].ob_state &= ~SELECTED;	
	ObjcChange( OC_FORM, win, index, DISABLED, TRUE);

	/* Lecture du fichier README */
	ViewText( (lang==LAN_EN)?"install\\README":"install\\LISEZMOI",
			  (lang==LAN_EN)?"Welcome to LDG " VERSION:"Bienvenue avec LDG " VERSION,
			   WAT_NOINFO, -1, -1, -1, -1);
	
	message( get_string( MSG_LEGAL));
}


void clean_field( OBJECT *tree, int index, char boot) {
	char *p = ObjcString( tree, index, NULL);
	
	if( strchr( p, '_'))
		*strchr( p, '_') = '\0';
	*p = boot;
}

void main( void) {
	int ret;
	char boot = 'C';
	
	ApplInit();
#ifndef __GNUC__
	Pdomain( 1);
#endif

	/* choix de la langue */

	if( has_appl_getinfo()) {
		INT16 dum;
		appl_getinfo( 3, &lang, &dum, &dum, &dum);
		if( lang == 2) lang = LAN_FR;
		else		   lang = LAN_EN;
	} else {
		long l;
		if( ldg_cookie( '_AKP', &l)) 
			if( (int)l == 2) lang = LAN_FR;
			else			 lang = LAN_EN;
	}

	if( !RsrcLoad( (lang == LAN_EN)?"install\\en.rsc":"install\\fr.rsc")) {
		FormAlert( 1, (lang == LAN_EN)?FA_ERROR "[Resource file|not found][Abort]":
									   FA_ERROR "[Fichier ressource|non trouv‚ !][Fin]");
		ApplExit();
		exit( 0);
	}
	
	FormBegin( get_tree( LANGUAGE), NULL);
	ret = FormDo( get_tree( LANGUAGE), BOOT_DRV);
	FormEnd( get_tree( LANGUAGE), NULL);
	boot = ObjcString( get_tree(LANGUAGE), BOOT_DRV, NULL)[0];

	if( ret != lang) {
		RsrcFree();
		if( ret == LAN_EN)
			RsrcLoad( "install\\en.rsc");
		else
			RsrcLoad( "install\\fr.rsc");
	}
	lang = ret;
	
	/* Menu cas MonoAES */
	
	if( _AESnumapps == 1) MenuBar( get_tree(MENU1), TRUE);	
	
	/* Cas MultiTOS */

	if( _AESnumapps == -1) menu_register( app.id, get_string( MN_REGISTER));
		
	/* Fenˆtre principale */

	/* Init du boot drive (et des champs) */

	clean_field( get_tree(MAINFORM), AUTOPATH, boot);
	clean_field( get_tree(MAINFORM), CPXPATH,  boot);
	clean_field( get_tree(MAINFORM), AUTOPATH, boot);
	clean_field( get_tree(MAINFORM), LDGPATH,  boot);
	clean_field( get_tree(MAINFORM), STARTPATH,boot);
	clean_field( get_tree(MAINFORM), HYPPATH,  boot);
	clean_field( get_tree(MAINFORM), XTRAPATH, boot);

	clean_field( get_tree(MAINFORM), INCPATH, boot);
	clean_field( get_tree(MAINFORM), LIBPATH, boot);
	clean_field( get_tree(MAINFORM), BINPATH, boot);
	clean_field( get_tree(MAINFORM), MANPATH, boot);

	/* Pour virer le label des icones */
	get_tree(MAINFORM)[GCCBUT].ob_spec.iconblk->ib_wtext = 0;
	get_tree(MAINFORM)[GCC295BUT].ob_spec.iconblk->ib_wtext = 0;
	get_tree(MAINFORM)[SOZBUT].ob_spec.iconblk->ib_wtext = 0;
	get_tree(MAINFORM)[PCBUT].ob_spec.iconblk->ib_wtext = 0;
	get_tree(MAINFORM)[SYSTEMBUT].ob_spec.iconblk->ib_wtext = 0;
	get_tree(MAINFORM)[SDKBUT].ob_spec.iconblk->ib_wtext = 0;
	get_tree(MAINFORM)[UNIXBUT].ob_spec.iconblk->ib_wtext = 0;
	if( app.color < 16)
		get_tree(MAINFORM)[INFO_BACK].ob_state &= ~SELECTED;
	
	/* Creation du formulaire principal */	

	wdial = FormCreate( get_tree(MAINFORM), NAME|MOVER|CLOSER|SMALLER|FULLER, 
						NULL, "Installation LDG " VERSION,
						NULL, TRUE, FALSE);
	WindSetStr( wdial, WF_ICONTITLE, "LDG " INSTALL_VERSION);
	WindSetPtr( wdial, WF_ICONDRAW, icon, NULL);
	EvntAdd( wdial, WM_DESTROY, SndTerm, EV_TOP);

	/* Initialisation des boutons */

	ObjcAttach( OC_FORM, wdial, SYSTEMBUT, BIND_BIT, &inst_flags, INST_SYS);
	ObjcAttach( OC_FORM, wdial, SDKBUT,    BIND_BIT, &inst_flags, INST_SDK);
	ObjcAttach( OC_FORM, wdial, UNIXBUT,   BIND_BIT, &inst_flags, INST_UNX);

	ObjcAttach( OC_FORM, wdial, SYSTEMLABEL, BIND_FUNC, install_label);
	ObjcAttach( OC_FORM, wdial, SDKLABEL,    BIND_FUNC, install_label);
	ObjcAttach( OC_FORM, wdial, UNIXLABEL,   BIND_FUNC, install_label);
	
	ObjcAttach( OC_FORM, wdial, GCCBUT,    BIND_VAR, &type_lib);
	ObjcAttach( OC_FORM, wdial, GCC295BUT, BIND_VAR, &type_lib);
	ObjcAttach( OC_FORM, wdial, SOZBUT,    BIND_VAR, &type_lib);
	ObjcAttach( OC_FORM, wdial, PCBUT ,    BIND_VAR, &type_lib);

	ObjcAttach( OC_FORM, wdial, GCCLABEL    , BIND_FUNC, lib_label);
	ObjcAttach( OC_FORM, wdial, GCC295LABEL , BIND_FUNC, lib_label);
	ObjcAttach( OC_FORM, wdial, SOZLABEL    , BIND_FUNC, lib_label);
	ObjcAttach( OC_FORM, wdial, PCLABEL     , BIND_FUNC, lib_label);

	ObjcAttach( OC_FORM, wdial, CANCEL  ,   BIND_FUNC, quit);
	ObjcAttach( OC_FORM, wdial, BUT_HELP,   BIND_FUNC, help);
	ObjcAttach( OC_FORM, wdial, BUT_INFO,   BIND_FUNC, info);
	ObjcAttach( OC_FORM, wdial, INSTALL ,   BIND_FUNC, install);
	ObjcAttach( OC_FORM, wdial, MAIN_MOVER, BIND_FUNC, move);

	ObjcAttach( OC_FORM, wdial, AUTOPATH, BIND_FUNC, EditFsel, "Select AUTO folder");
	ObjcAttach( OC_FORM, wdial, CPXPATH, BIND_FUNC, EditFsel, "Select CPX folder" );
	ObjcAttach( OC_FORM, wdial, LDGPATH, BIND_FUNC, EditFsel, "Select LDG folder");
	ObjcAttach( OC_FORM, wdial, STARTPATH, BIND_FUNC, EditFsel, "Select START folder");
	ObjcAttach( OC_FORM, wdial, HYPPATH, BIND_FUNC, EditFsel, "Select ST-Guide folder");
	ObjcAttach( OC_FORM, wdial, XTRAPATH, BIND_FUNC, EditFsel, "Select Extras folder");
	ObjcAttach( OC_FORM, wdial, INCPATH, BIND_FUNC, EditFsel, "Select C include folder");
	ObjcAttach( OC_FORM, wdial, LIBPATH, BIND_FUNC, EditFsel, "Select C libraries folder");
	ObjcAttach( OC_FORM, wdial, BINPATH, BIND_FUNC, EditFsel, "Select bin folder");
	ObjcAttach( OC_FORM, wdial, MANPATH, BIND_FUNC, EditFsel, "Select man folder");

	EvntWindom( MU_MESAG);
	message( get_string( MSG_LOADLDG));
	ldg = ldg_open( "gemsys\\ldg\\fileutil.ldg", ldg_global);
	if( !ldg) {
		FormBegin( get_tree( ERRORLDG), NULL);
		FormDo( get_tree( ERRORLDG), -1);
		FormEnd( get_tree( ERRORLDG), NULL);
		ApTerm();
	}
#ifndef STATICLIB
	 else
		init_ldgfile( ldg);
#endif

	if( cp == NULL || mkdir == NULL || rm == NULL) {
		FormAlert( 1, get_string( LDGFIND_ERROR));
		ApTerm();
	}
	
	message( get_string( MSG_LEGAL));
	EvntAttach( NULL, AP_TERM, ApTerm);
	EvntAttach( NULL, MN_SELECTED, MnSelected);
	for( ;;) EvntWindom( MU_MESAG|MU_KEYBD);
}

/* EOF */
