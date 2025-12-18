/*
 * ldginfo : maintenance des librairies dynamiques GEM
 * (c) 1998/1999/2001/2002 Dominique B‚r‚ziat
 * Tous droits r‚serv‚s.
 *  version 2.23 : recompilation pour GEMLIB pl43 : appel plus conforme
 *                 a appl_getinfo()
 *                 Gestion des subdir dans les noms des libs (pour nouvelle
 *                 version des LDG 2.32)
 *  version 2.22 : backup des libraries lors de la mise a jour (-u)
 *  version 2.21 : nouveau nom ldginfo plutot que showlib.
 *                 c'est la version en ligne du LDG_INFO
 *				   d'Arnaud
 *	version 2.20 : on peut changer les valeurs par d‚faut
 *				   du TSR et ainsi se passer du CPX.
 *  version 2.10 : conformit‚ avec TSR 2.21
 *	version 2.00 : conformit‚ avec TSR 2.20
 *				   installation de libraries
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef __PUREC__
#include <tos.h>
#else
#include <osbind.h>
#include <mint/ostruct.h>
#define FA_SUBDIR FA_DIR
#define FO_READ 0
#endif
#include <gem.h>
#include <ldg.h>
#include <string.h>

#include <internal.h>
#include <backward.h>

#define LDGINFO_VERSION "2.23"
#define VERSION(v)	(v)>>8,(v)&0x00FF

#define LISTFONC	0x0001
#define HEADER		0x0002
#define LDGINFO		0x0004
#define INTERNAL	0x0008
#define HOLDKEY		0x0010
#define	APPLSEARCH	0x0020
#define COOKIEFNC	0x0040
#define INSTALL		0x0080

int debug = 0;
short apid;
int flags = 0;
LDG_INFOS *cook = NULL;
/* Utilis‚ par la fontion set_tsr_value() */
char *tsrpath = NULL;

/*
 *	Usage du programme
 */

void show_help(void) {
	fprintf( stderr, "ldginfo TTP version %s (%s at %s) by D.B‚r‚ziat 1998-2005\n", LDGINFO_VERSION, __DATE__, __TIME__);
	fprintf( stderr, "compiled with LDG version %s and GEMLIB pl43\n", ldg_version.name);
	fprintf( stderr, "syntax is: ldginfo [options] [lib1 [lib2 ...]]\n");
	fprintf( stderr, "Command line attents for a list of options eventually "
		 "followed by a list of LDG-libraries. Options are:\n"
		 "\t-h,--help: print this help\n"
		 "\t-v,--version: print the program version\n"
		 "\t-l,--ldg: print info about LDG cookie and environs variables\n"
		 "\t-i,--inspect [path]: inspect and list content of LDG-path or [path]\n"
		 "\t-f,--function: print functions of lib\n"
		 "\t-p,--header: print header lib information\n"
		 "\t-g,--garbage: perform garbage collecting\n"
		 "\t-r,--reset: reset LDG system (libs will be unloaded)\n"
		 "\t-t,--internal: print internal info and list libs currently loaded\n"
		 "\t-m,--manager: print manager informations\n"
		 "\t-a,--all: cumulate options -l, -t, -p, -f\n"
		 "\t-u,--update: update/install libraries following option\n"
		 "\t-k,--key: press a key to finish program\n"
		 "\t--cookie-idle [time]: display or set the cookie idle time (in msec.)\n"
		 "\t--cookie-garbage [time]: display or set the cookie garbage time (in sec.)\n"
		 "\t--cookie-path [path]: display or set the cookie libraries path (128 char max)\n"
		 "\t--cookie-version: display cookie version (i.e. active LDG kernel version)\n"
		 "\t--tsr-idle [time]: display or set TSR idle time (in msec.)\n"
		 "\t--tsr-garbage [time]: display or set TSR garbage time (in sec.)\n"
		 "\t--tsr-path [path]: display or set TSR libraries path (128 char max)\n"
		 "\t--tsr-version: display TSR version\n"
		 "\t--tsr path: specify location of LDG.PRG. This option "
		 "should be placed before options --tsr-*.\n"
		 );
}

/*
 * La fonction appl_write en plus pratique (extrait de WinDom)
 */

int ApplWrite( int from, int to, int msg, ...) {
	va_list args;	
	short buf[8];

	buf[0] = msg;
	buf[1] = from;
	buf[2] = 0;
	va_start( args, msg);
	buf[3] = va_arg( args, short);
	buf[4] = va_arg( args, short);
	buf[5] = va_arg( args, short);
	buf[6] = va_arg( args, short);
	buf[7] = va_arg( args, short);
	va_end( args);
	return appl_write( to, 16, buf);
}

/* Pour identifier un process AES */

#define AES_PROCESS		4

int is_appl_search( void) {
	short dum, parm3;
/* Avec GEMLIB PL43, appl_getinfo() teste sa presence */
/*	if( appl_find("?AGI") != -1) { */
		appl_getinfo( AES_PROCESS, &dum, &dum, &parm3, &dum);
/*		if( parm3 == 1) return 1;	
	}
	return 0; */
	return parm3 == 1;
}

char *appl_name( int id) {
	short i,m=0x0F;
	static char name[9]; 
	char name2[9];
	
	if( flags & APPLSEARCH && appl_search(0, name2, &m, &i) && i != id) {
		while( appl_search(1, name2, &m, &i)) {
			if( i == id )
				break;
		}
	}
	sscanf( name2, "%s", name);
	return name;
}

/*
 * gestion  des erreurs 
 */

void get_error( char *lib, int type) {
	int error = ldg_error();
	short buf[8];
		
	if( error == 0) {
		do {
			evnt_mesag( buf);
		} while( buf[0] != LDG_ERROR);
		error = buf[3];
	}
	if( type)
		fprintf( stdout, "Error with %s: ", lib);
	else
		fprintf( stdout, "error : ");
	switch( buf[3]) {
		case LDG_LIB_FULLED:
				fprintf( stdout, "not enough memory to declare a library\n");
				break;
		case LDG_APP_FULLED:
				fprintf( stdout, "not enough memory to declare an application\n");
				break;
		case LDG_ERR_EXEC:
				fprintf( stdout, "library is not GEMDOS executable\n");
				break;
		case LDG_BAD_FORMAT:
				fprintf( stdout, "bad library format\n");
			break;
		case LDG_LIB_LOCKED:
				fprintf( stdout, "library is locked\n");
				break;
		case LDG_NO_MANAGER:
				fprintf( stdout, "LDG-manager not found\n");
				break;
		case LDG_NOT_FOUND:
				fprintf( stdout, "library not found\n");
				break;
		default:
			fprintf( stdout, "unknown error (%d) (use a newer version of ldginfo)\n", buf[3]);
			break;
		}
	}

/*
 *	Infos sur une librairie
 */

void show_lib( char *lib) {
	LDG *ldg;
	int i;
	

	ldg = ldg_open( lib, ldg_global);

	if( !ldg && !strrchr( lib, '.')) {
		strcat( lib, ".ldg");
		ldg = ldg_open( lib, ldg_global);
	}

	if( !ldg && strrchr( lib, '\\') && lib[1] != ':') {
		char *p = NULL;
	
		if( cook) p = cook->path;
		if( !p) shel_envrn( &p, "LDGPATH=");
		if( !p) p = getenv( "LDGPATH");

		if( p) {
			char buf[128];
			
			strcpy( buf, p);
			strcat( buf, "\\");
			strcat( buf, lib);
			ldg = ldg_open( lib, ldg_global);
		}
	}
	if( ldg) {
		if( flags & HEADER) {
			fprintf( stdout, "library header %s :\n", lib);
			fprintf( stdout, "          Launched by : %d ", ldg->id);
			if( appl_find( "LDGMGR  ") == ldg->id)
				fprintf( stdout, "(LDG Manager)\n");
			else if( ldg->id == apid)
				fprintf( stdout, "(LdgInfo)\n");
			else
				fprintf( stdout, "(??)\n");
			fprintf( stdout, "        Infos library : %s\n", ldg->infos);
			fprintf( stdout, "          Lib version : %X.%02X\n", ldg->vers >> 8, ldg->vers & 0x00FF);
			fprintf( stdout, "  Number of functions : %d\n", ldg->num);
			fprintf( stdout, "            Shareable : %s\n", (ldg->flags & LDG_NOT_SHARED)?"no":"yes");
			fprintf( stdout, "               Locked : %s\n", (ldg->flags & LDG_LOCKED)?"yes":"no");
			fprintf( stdout, "             Resident : %s\n", (ldg->flags & LDG_RESIDENT)?"yes":"no");
			fprintf( stdout, "     Closure function : %s\n", ldg->close?"yes":"no");
			if( ldg->vers_ldg == 0)
				fprintf( stdout, "WARNING: lib was probably compiled with a version of LDG older than 1.00.\n");
			else {
				fprintf( stdout, "         Localization : %s\n", ldg->path);
				fprintf( stdout, " LDG protocol version : %X.%02X\n", ldg->vers_ldg >> 8, ldg->vers_ldg & 0x00FF);
				fprintf( stdout, "Private lib extension : %s\n", ldg->user_ext?"yes":"no");
				fprintf( stdout, "    Special extension : %s\n", ldg->addr_ext?"yes":"no");
			}
		}
		
		if( flags & LISTFONC) {
			if( ldg->num == 0)
				fprintf( stdout, "Warning, this library contents any function!\n");
			else {
				fprintf( stdout, "List of %s functions:\n", lib);
				for( i=0; i<ldg->num; i++) 
					fprintf( stdout, "    [%d] %s : %s\n", i, ldg->list[i].name, ldg->list[i].info);
			}
		}
		
		if( (flags & LISTFONC) == 0 && (flags & HEADER) == 0) {
			if( ldg)
				fprintf( stdout, "Librairie %s: %s\n", lib, ldg->infos);
		}
		
		fprintf( stdout, "\n");
		ldg_close( ldg, ldg_global);
	} else 
		get_error( lib, 1);
}

/*
 *	Infos cookie/environnement
 */

void show_info( void) {
	char *p;

	fprintf( stdout, "Information about LDG configuration :\n");

	fprintf( stdout, "  With environ variables :\n");
	shel_envrn( &p, "LDGPATH=");
	if( p) fprintf( stdout, "    LDGPATH=%s\n", p);
	shel_envrn( &p, "LDG_GARBAGE=");
	if( p) fprintf( stdout, "    LDG_GARBAGE=%s\n", p);
	shel_envrn( &p, "LDG_IDLE=");
	if( p) fprintf( stdout, "    LDG_IDLE=%s\n", p);
	shel_envrn( &p, "LDG_DEBUG=");
	if( p) fprintf( stdout, "    LDG_DEBUG=%s\n", p);
	fprintf( stdout, "\n");

	fprintf( stdout, "  With 'LDGM' cookie :\n");
	if( cook) {
	
		/* Info generals */
		fprintf( stdout, "    version : %x.%02x\n", VERSION(cook->version));
		fprintf( stdout, "       path : %s\n", cook->path);
		fprintf( stdout, "    garbage : %d sec %s\n", cook->garbage, 
				(cook->version >= 0x0220 && appl_find( "LDGD    ") == -1)?
				"(inactive with LDG 2.xx because LDG deamon not in memory)":"");
		fprintf( stdout, "       idle : %d msec %s\n", cook->idle,
							(cook->version >= 0x0220)?"(obsolet with LDG 2.xx)":"");

		/* Fonctions builtin */
		if( cook->version >= 0x0220) {
			/* LDG 2.00 */
			fprintf( stdout, "  functions : ldg_open(), ldg_close(), ldg_find(), ldg_garbage(), ldg_reset().\n");
		} else if( cook->version >= 0x0200) {
			/* LDG 1.00 */
			fprintf( stdout, "  functions : libexec(), libterm(), find()");
			/* LDG 1.10 */
			if( cook->version >= 0x0210) 
				fprintf( stdout, ", libexec_evnt().");
			fprintf( stdout, "\n");
		}
	}
}

void show_internal( int manager) {
	struct ldg_infos infos;
	struct ldg_lib	 lib;
	int i, type;
	
	if( manager == LDGTSR) {
		if( ldg_libslist( &infos, ldg_global) < 0) {
			fprintf( stdout, "informations not available.\n");
			return ;
		}
		type = infos.type & 0x00FF;
		fprintf( stdout, "Internal libraries list :\n");
		fprintf( stdout, "        manager: %s\n", (type == LDGMGR)?"LDG-manager":"LDG-tsr");
	} else {
		if( ldg_mgrlist( &infos, ldg_global) < 0) {
			fprintf( stdout, "informations not available.\n");
			return ;
		}
		fprintf( stdout, "Information about manager :\n");
	}
	fprintf( stdout, "        version: %x.%02x\n", VERSION( infos.version));
	fprintf( stdout, "max clients/lib: %d\n", infos.maxclient);
	fprintf( stdout, "       max libs: %d\n", infos.maxlib);
	if( type == LDGTSR)
		fprintf( stdout, "    max clients: %d\n", infos.maxclient);
	else
		fprintf( stdout, "    max clients: info not available\n");
		
	if( cook && infos.version < 0x0200 && cook->version >= 0x0220) {
		fprintf( stdout, "-> warning : you use a version of LDG-manager incompatible with "
						 "the current TSR version. A LDG-manager version 2 is required.\n");
	}

	fprintf( stdout, "List of libs and clients :\n");
	fprintf( stdout, "  name          subdir                used by        \n");
	fprintf( stdout, "-----------------------------------------------------\n");
	
	while( ldg_getlib( &infos, &lib)) {
		char p[256], *q;

		strcpy( p, lib.name);
		/* convert slash */
		for(q=p;*q;q++) if(*q=='/') *q='\\';

        q = strrchr(p, '\\');
		if( q) *q++ = '\0';
		else {
			q=lib.name;
			strcpy(p,".");
		}

		/* fprintf( stdout, ". %-13s 0x%lX  ", lib.name,  lib.ldg); */
		fprintf( stdout, ". %-13s %-21s ", q, p);
		for( i=0; i<infos.maxclient; i++) {
			if( lib.usedby[i] != -1) {
				fprintf( stdout, "%d (%s) ", lib.usedby[i],
						  (flags&APPLSEARCH)?appl_name(lib.usedby[i]):"??");
			}
		}
		if( ldg_loadlib( &infos) == 0) {
			if( lib.ldg -> flags & LDG_RESIDENT)
				fprintf( stdout, "(resident in memory)");
			else
				fprintf( stdout, "(client probably crashed)");
		}
		fprintf( stdout, "\n");
	}	
}

#define SET_PATH	1
#define SET_VERSION	0	/* pour set_cookie_value(), r‚serv‚ */
#define SET_IDLE	2
#define SET_GARBAGE	3

void set_cookie_value( int mode, char *val ) {	
	if( cook) {
		switch( mode) {
		case SET_VERSION:
			if( val) {
				sscanf( val, "%x", &cook->version);
				fprintf( stderr, "Value set.\n");
			} else
				fprintf( stdout, "%x.%02x\n", VERSION(cook->version));

			break;
		case SET_PATH:
			if( val) {
				if( strlen(val) < 127) {
					strcpy( cook->path, val);
					fprintf( stderr, "Value set.\n");
				} else
					fprintf( stderr, "Error : path too long (127 chars max).\n");
			} else
				fprintf( stdout, "%s\n", cook->path);
			
			break;
		case SET_IDLE:
			if( val) {
		 		cook->idle = atoi(val);
		 		fprintf( stderr, "Value set.\n");
		 	} else
				fprintf( stdout, "%d\n", cook->idle);
		 	
			break;
		case SET_GARBAGE:
			if( val) {
				cook->garbage = atoi(val);
				fprintf( stderr, "Value set.\n");
			} else
				fprintf( stdout, "%d\n", cook->garbage);

			break;
		}
	} else
		fprintf( stderr, "Cookie LDGM not in memory.\n");
}

/*
 * Recherche d'une occurence dans un buffer de donn‚es.
 */
 
char *memstr( char *buf, char *str, size_t size) {
	char *p, *q;

	while( size) {
		q = str;
		buf = memchr( buf, *q, size);
		
		if( buf) {
			p = buf;
			/* V‚rifier les suivants */
			while( *p == *q && size && *q) {
				size --;
				p ++;
				q ++;
			}
			if( !*q) return buf;
			else {
				buf += p-buf;
				size -= p-buf;
			}
		} else
			return NULL;
	}
	return NULL;
}

/*
 * J'aimerai connaitre la partoche de boot
 */
 
void set_tsr_value( int mode, char *val ) {	
	FILE *fp = fopen( tsrpath?tsrpath:"C:\\AUTO\\LDG.PRG", "rb");
	char *buf, *p;
	long size, s;

	if( debug) fprintf( stderr, "File to process: %s\n",tsrpath?tsrpath:"C:\\AUTO\\LDG.PRG");
	 	
	if( !fp) {
		fprintf( stderr, "Error, impossible to locate LDG.PRG.\n");
		fprintf( stderr, "You can specified the location with the option --tsr "
						 "placed before the options --tsr-*.\n" );
		return;
	}

	fseek( fp, 0, SEEK_END);
	size = ftell( fp);
	buf = malloc( sizeof(char)*size);
	if( !buf) {
		fprintf( stderr, "Error, not enough memory.\n");
		fclose( fp);
		return;
	}
	rewind( fp);
	s = fread( buf, sizeof(char), size, fp);

	if( size != s*sizeof(char)) {
		fprintf( stderr, "Warning, file maybe corrupted (%ld bytes read).\n", s*sizeof(char));
	}
	fclose(fp);

	p = memstr( buf, "LdgData", size);
	
	if( debug) fprintf( stderr, "Dump p: %s\n", p?p:"(null)");
	if( p == NULL) {
		fprintf( stderr, "Error, this version of TSR can not be set. "
						 "You have to use a TSR version 2.22 or higher.\n");
		free( buf);	
		return;
	} 

	p += strlen( "LdgData") + 1 + sizeof( short); /* "LdgData\0" + VERSION */

	switch( mode) {
	case SET_VERSION:
		fprintf( stdout, "%x.%02x\n", VERSION(*(short*)(p - sizeof( short))));
		break;

	case SET_PATH:
		if( val) {
			strncpy( p, val, PATHLEN);
			fprintf( stderr, "Value set.\n");
		} else 
			fprintf( stdout, "%s\n", p);
		break;

	case SET_IDLE:
		if( val) {
		 	*(short*)(p + PATHLEN) = atoi( val);
		 	fprintf( stderr, "Value set.\n");
		} else
			fprintf( stdout, "%d\n", *(short*)(p + PATHLEN));
		break;

	case SET_GARBAGE:
		if( val) {
		 	*(short*)(p + sizeof( short) + PATHLEN) = atoi( val);
			fprintf( stderr, "Value set.\n");
		} else
			fprintf( stdout, "%d\n", *(short*)(p + PATHLEN + sizeof( short)));
		break;
	}
	
	fp = fopen( tsrpath?tsrpath:"C:\\AUTO\\LDG.PRG", "wb");
	fwrite( buf, sizeof(char), size, fp);
	fclose( fp);

	free( buf);
}

/*
 *	Analyse r‚cursive d'un r‚pertoire de libs
 */

void list_dir( char *p) {
	char buf[255];
	LDG *ldg;
#ifdef __PUREC__
	DTA *save, dta;
#else
	_DTA *save, dta;
#define d_fname dta_name 
#define d_attrib dta_attribute
#endif
	
	save = Fgetdta();
	Fsetdta( &dta);
	
	strcpy( buf, p);
	strcat( buf, "\\*");
	
	if( Fsfirst( buf, FA_SUBDIR) >= 0 ) {
		if( strcmp( dta.d_fname, ".") &&
			strcmp( dta.d_fname, "..") &&
			dta.d_attrib & FA_SUBDIR ) {
			strcpy( buf, p);
			strcat( buf, "\\");
			strcat( buf, dta.d_fname);
			list_dir( buf);
		}
		while( Fsnext() >= 0) {
			if( strcmp( dta.d_fname, ".") &&
				strcmp( dta.d_fname, "..") &&
				dta.d_attrib & FA_SUBDIR) {
				strcpy( buf, p);
				strcat( buf, "\\");
				strcat( buf, dta.d_fname);
				list_dir( buf);
			}
		}
	} 
	
	fprintf( stdout, "Content of %s:\n", p);
	
	strcpy( buf, p);
	strcat( buf, "\\*.LDG");
	
	if( Fsfirst( buf, 0) >= 0 ) {
		strcpy( buf, p);
		strcat( buf, "\\");
		strcat( buf, dta.d_fname);
		ldg = ldg_exec( apid, buf);
		fprintf( stdout, "  %13s | %s", dta.d_fname, ldg?ldg->infos:"");
		if( !ldg) get_error(dta.d_fname,0);
		printf( "\n");
		ldg_term( apid, ldg);
		while( Fsnext() >= 0) {
			strcpy( buf, p);
			strcat( buf, "\\");
			strcat( buf, dta.d_fname);
			ldg = ldg_exec( apid, buf);
			fprintf( stdout, "  %13s | %s", dta.d_fname, ldg?ldg->infos:"");
			if( !ldg) get_error(dta.d_fname,0);
			fprintf( stdout, "\n");
			ldg_term( apid, ldg);
		}
	}
	fprintf( stdout, "\n");
	Fsetdta( save);
}

void inspect( char *path) {
	LDG_INFOS *cook;
	char *p;
	if( path) {
		list_dir( path);	
	} else if( ldg_cookie( LDG_COOKIE, (long*)&cook)) {
		/* R‚pertoire du cookie */
		list_dir( cook->path);
	} else {
		shel_envrn( &p, "LDGPATH=");
		if( p) {
			list_dir( p);
		} else {
			fprintf( stdout, "Error : set LDG cookie or LDG variables (see \"Installation of LDG\" in LDG.HYP\n");
		}
	}
}

/*
 * Installe une librairie dans les r‚pertoires
 * LDG si le num‚ro de version est plus r‚cent
 */

#define min(a,b) (((a)<(b))?(a):(b))

int copy( char *src, char *dest) {
	int fi, fo;
	char *buff;
	size_t size;
	size_t count,i,rest;
	size_t blocsize;


	blocsize = (unsigned long)Malloc( -1);
	blocsize = (blocsize/512)*512;
		
	fi = (int)Fopen( src, FO_READ);
	if( fi < 0)	return -33;
	fo = (int)Fcreate( dest, 0);
	size = Fseek( 0, fi, 2);
	Fseek(  0, fi, 0);

	if( blocsize == -1) {
		count = 0;
		rest  = size;
	} else {
		count = size / blocsize;
		rest  = size % blocsize;
		size  = min(blocsize,size);
	}

	buff = (char *)ldg_Malloc( size);
	
	for( i = 0; i < count; i ++) {
		Fread( fi, size, buff);
		Fwrite( fo, size, buff);
	}
	if( rest) {
		Fread( fi, rest, buff);
		Fwrite( fo, rest, buff);
	}

	Fclose( fi);
	Fclose( fo);
	ldg_Free( buff);
	return 0;
}

int install_lib( char *lib) {
	LDG *ldg_load( int, char *);
	void ldg_unload( int, LDG *);
	char *p = strrchr( lib, '\\');
	char path[255], *q;
	LDG *l1, *l2;
	int v1=-1, v2;
	
	/* Librairie install‚e */

	p = p?(p+1):lib;
	l1 = NULL;

	/* Cookie ? */
	if( cook) {
		sprintf( path, "%s\\%s", cook->path, p);
		l1 = ldg_load( ldg_global[2], path);
	}
	
	/* LDGPATH */
	if( !l1) {
		if( shel_envrn( &q, "LDGPATH=")) {
			sprintf( path, "%s\\%s", q, p);
			l1 = ldg_load( ldg_global[2], path);
		}
	}

	if( l1) {
		v1 = l1->vers;
		ldg_unload( ldg_global[2], l1);
	}	
		
	/* librarie … installer */
	l2 = ldg_load( ldg_global[2], lib);
	if( l2) {
		v2 = l2->vers;
		ldg_unload( ldg_global[2], l2);
	} else {
		fprintf( stdout, "Installation of %s failed.\n", p);
		return -33;
	}	

	if( l1 == NULL || ( v1 < v2)) {
		if( l1 == NULL) {
			/* Il faut trouver un r‚pertoire */
			if( cook && *cook->path) {
				strcpy( path, cook->path);
			} else if( shel_envrn( &q, "LDGPATH=")) 
					strcpy( path, q);
			else {
				fprintf( stdout, "I can't find a place to copy the file.\n");
				return -2;
			}
			strcat( path, "\\");
			strcat( path, p);
		}
		/* Nouveau : on backup la librairie pr‚c‚dente */
		if( v1 >= 0) {
			char new[255], *q;
			strcpy( new, path);
			q = strchr( new, '.');
			if( q) {
				sprintf( q+1, "%1x%02x", 0x00FF & (v1>>8), v1 & 0x00FF);
				rename( path, new);
				fprintf( stdout, "Previous library %s.ldg (version %x.%02x) "
								 "renamed to %s.%1x%02x\n", 
								 p, 0x00FF & (v1>>8), v1 & 0x00FF,
								 p, 0x00FF & (v1>>8), v1 & 0x00FF);
			}
		}
		copy( lib, path);
		fprintf( stdout, "Library %s version  %x.%02x installed.\n", p, VERSION(v2));

	} else if( v1 == v2) {
		fprintf( stdout, "Library %s version  %x.%02x already installed.\n", p, VERSION(v2));
		return -1;	
	} else if( v1 > v2) {
		fprintf( stdout, "A more recent version of this library (%x.%02x) is already installed.\n", VERSION(v1));
		return -1;		
	}
	return 0;
}

/*
 * Partie principale
 */

int main( int argc, char *argv[]) {
	int arg = 1;
	
	if( argc == 1) {
		fprintf( stdout, "Type ldginfo -h to get more informations.\n");
		return 0;
	}

	apid = appl_init();
	if( is_appl_search	() ) flags |= APPLSEARCH;
	ldg_cookie( LDG_COOKIE, (long*) &cook);

	while( arg < argc) {
		if( !strcmp( argv[arg], "--inspect") ||
			!strcmp( argv[arg], "-i")) {
			if( arg+1  < argc && *argv[arg+1] != '-' ) {
				inspect( argv[arg+1]);
				arg += 2;
			} else {
				arg ++;
				inspect(NULL);
			}
		} else
		if( !strcmp( argv[arg], "--ldg") ||
			!strcmp( argv[arg], "-l")) {
			arg ++;
			flags |= LDGINFO;
			show_info();
		} else
		if( !strcmp( argv[arg], "--help") ||
			!strcmp( argv[arg], "-h")) {
			arg ++;
			show_help();
		} else
		if( !strcmp( argv[arg], "--version") ||
			!strcmp( argv[arg], "-v")) {
			arg ++;
			fprintf( stdout, "LDGINFO TTP version %s compiled the %s at %s\n",
					 LDGINFO_VERSION, __DATE__, __TIME__);
		} else
		if( !strcmp( argv[arg], "--manager") ||
			!strcmp( argv[arg], "-m")) {
			arg ++;
			/* fprintf( stdout, "This option is obsolet - try ldginfo -t.\n"); */
			show_internal( LDGMGR);
		} else
		if( !strcmp( argv[arg], "--internal") ||
			!strcmp( argv[arg], "-t")) {
			arg ++;
			flags |= INTERNAL;
			show_internal( LDGTSR);
		} else
		if( !strcmp( argv[arg], "--function") ||
			!strcmp( argv[arg], "-f")) {
			arg ++;
			flags |= LISTFONC;
		} else
		if( !strcmp( argv[arg], "--header") ||
			!strcmp( argv[arg], "-p")) {
			arg ++;
			flags |= HEADER;
		} else
		if( !strcmp( argv[arg], "--all") ||
			!strcmp( argv[arg], "-a")) {
			arg ++;
			flags |= (HEADER|LISTFONC|INTERNAL|LDGINFO);
			show_info();
			show_internal( LDGTSR);
		} else 
		if( !strcmp( argv[arg], "--key") ||
			!strcmp( argv[arg], "-k")) {
			arg ++;
			flags |= HOLDKEY;
		} else
		if( !strcmp( argv[arg], "--garbage") ||
			!strcmp( argv[arg], "-g")) {
			arg ++;
			ApplWrite( apid, appl_find( "LDGMGR  "), LDG_GARBAGE, 0,0,0,0,0);
			if( cook && cook->version >= 0x0220)
				(*cook->ldg_garbage)( ldg_global);
		} else
		if( !strcmp( argv[arg], "--reset") ||
			!strcmp( argv[arg], "-r")) {
			arg ++;
			ApplWrite( apid, appl_find( "LDGMGR  "), LDG_FORCE, 0,0,0,0,0);
			if( cook && cook->version >= 0x0220)
				(*cook->ldg_reset)( ldg_global);
		} else
		if( !strcmp( argv[arg], "--update") ||
			!strcmp( argv[arg], "-u")) {
			arg ++;
			flags |= INSTALL;
		} else
			/* COOKIE settings */
		if( !strcmp( argv[arg], "--cookie-version")) {
			set_cookie_value( SET_VERSION, NULL);
			arg ++ ;
		} else
		if( !strcmp( argv[arg], "--cookie-idle")) {
			if( arg+1<argc && argv[arg+1][0] != '-') {
				set_cookie_value( SET_IDLE, argv[arg+1]);
				arg +=2 ;
			} else {
				set_cookie_value( SET_IDLE, NULL);
				arg ++ ;
			}
		} else
		if( !strcmp( argv[arg], "--cookie-garbage")) {
			if( arg+1<argc && argv[arg+1][0] != '-') {
				set_cookie_value( SET_GARBAGE, argv[arg+1]);
				arg +=2 ;
		 	} else {
				set_cookie_value( SET_GARBAGE, NULL);
				arg ++ ;
		 	}
		} else 
		if( !strcmp( argv[arg], "--cookie-path")) {
			if( arg+1<argc && argv[arg+1][0] != '-') {
				set_cookie_value( SET_PATH, argv[arg+1]);
				arg +=2 ;
			} else {
				set_cookie_value( SET_PATH, NULL);
				arg ++ ;
			}
		} else
			/* TSR settings */
		if( !strcmp( argv[arg], "--tsr-version")) {
			set_tsr_value( SET_VERSION, NULL);
			arg ++ ;
		} else
		if( !strcmp( argv[arg], "--tsr-idle")) {
			if( arg+1<argc && argv[arg+1][0] != '-') {
				set_tsr_value( SET_IDLE, argv[arg+1]);
				arg +=2 ;
			} else {
				set_tsr_value( SET_IDLE, NULL);
				arg ++ ;
			}
		} else
		if( !strcmp( argv[arg], "--tsr-garbage")) {
			if( arg+1<argc && argv[arg+1][0] != '-') {
				set_tsr_value( SET_GARBAGE, argv[arg+1]);
				arg +=2 ;
		 	} else {
				set_tsr_value( SET_GARBAGE, NULL);
				arg ++ ;
		 	}
		} else 
		if( !strcmp( argv[arg], "--tsr-path")) {
			if( arg+1<argc && argv[arg+1][0] != '-') {
				set_tsr_value( SET_PATH, argv[arg+1]);
				arg +=2 ;
			} else {
				set_tsr_value( SET_PATH, NULL);
				arg ++ ;
			}
		} else
		if( !strcmp( argv[arg], "--tsr")) {
			tsrpath = argv[arg+1];
			arg +=2 ;
		} else
		
		if( !strcmp( argv[arg], "--debug")) {
			arg +=1;
			debug = 1;
		} else
		if( argv[arg][0] != '-') {
			if( flags & INSTALL)
				install_lib(  argv[arg]);
			else
				show_lib( argv[arg]);		
			arg ++;
		} else
			fprintf( stdout, "unknown option %s\n", argv[arg++]);
	
	}

	if( flags & HOLDKEY) {
		fprintf( stdout, "Press any key to finish\n");
		Bconin(2);
	}

	appl_exit();
	return 0;
}

