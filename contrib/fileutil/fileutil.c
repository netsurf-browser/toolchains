/*
 *	Fonctions fichiers
 *  Version 0.21 (mai 2003)
 */

#ifdef __PUREC__
#include <tos.h>
#endif
#ifdef __GNUC__
#include <mint/mintbind.h>
#ifndef DTA
#define DTA _DTA
#define FA_SUBDIR FA_DIR
#endif
#endif
#include <gem.h>
#include <ldg.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h> 

#define VERSION		"0.21"
#define VERSNUM		0x0021
#define min(a,b)	(((a)<(b))?(a):(b))
#define DEBUG(a)	ldg_debug( "[1][%d|%s][OK]", __LINE__, a)

#define ERANGE      -64 /* Out of range ? */
#define ENMFIL		-49 /* No more file ? */

typedef struct {
    char            d_reserved[21];
    unsigned char   d_attrib;
    unsigned int    d_time;
    unsigned int    d_date;
    unsigned long   d_length;
    char            d_fname[64];
} XDTA;  /* Extended DTA structure */

#ifndef XATTR
	typedef struct {
		unsigned short	mode;			/*	file access mode	*/	
		long	index;					/*	file number	*/
		unsigned short	dev;			/*	device number	*/
		unsigned short	rdev;			/*	real bios device number	*/
		unsigned short	nlink;			/*	number of links to this file	*/
		unsigned short	uid;			/*	user id	*/
		unsigned short	gid;			/*	group id	*/
		long	size;					/*	file size in bytes	*/
		long	blksize;				/*	block size	*/
		long	nblocks;				/*	number of blocks occupied	*/
		unsigned short	mtime, mdate;	/*	modification date & time	*/
		unsigned short	atime, adate;	/*	last access date & time	*/
		unsigned short	ctime, cdate;	/*	creation date & time	*/
		unsigned short	attr;			/*	TOS attributes	*/
		short	reserved2;
		long	reserved3[2];
	} XATTR;
#endif


typedef struct {
	long  d_dirhandle;
	long  d_attr;
	char* d_regexp;
	char* d_dir;
} EXT;

static 
int opendir = 0;	/* True if Dopendir() function is present */
static
XDTA dta;
static
XDTA *_dta = &dta;

#ifndef STATICLIB

/* LDG interface
 ********************************************************************/

char regexp_info[] = "/* TOS regular expressions\n"
					 " * returns true if name matches to rexp */\n"
					 "short regexp( const char *rexp, const char *name);";
char fgetdta_info[] = "/* Get current XDTA */\n"
					  "XDTA* fgetdta(void);\n"
					  "";
char fsetdta_info[] = "/* Set a new XDTA */\n"
					  "void fgetdta( XDTA*);\n";
char fsfirst_info[] = "/* new implementation of Fsfirst() supporting \n"
					  " * long filename */\n"
					  "short fsfirst( char *pathname, long attr)";
char fsnext_info[] = "/* new implementation of Fsnext() supporting \n"
					 " * long filename */\n"
					 "short fsnext( void);";

char cp_info[] = "/* Copy file(s)\n"
				 " * returns number of file copied.*/\n"
				 "short cp( char *src, char *dest);";
char mv_info[] = "/* Move file(s) \n"
				 " * returns number of file moved.*/\n"
				 "short mv( char *src, char *dest);";
char mkdir_info[] = "/* Create a complet path \n"
					" * returns number of subfolder created.*/"
				    "short mkdir( char *path);";
char rm_info[] = "/* Delete file(s)\n"
				 " * returns number of file deleted.*/\n"
				 "short rm( char *path);";
char rmdir_info[] = "/* Delete directories\n"
					" * returns number of directories deleted.*/\n"
				 	"short rmdir( char *path);";
char fileset_info[] = "/* Private function for beta test */\n"
				 	"short fileset( long mode, long val);";

short __CDECL regexp ( const char *, const char *);
void  __CDECL fsetdta( XDTA *);
XDTA* __CDECL fgetdta( void);
short __CDECL fsfirst( char *, long attr);
short __CDECL fsnext ( void);

short __CDECL cp   ( char *src, char *dest);
short __CDECL mv   ( char *src, char *dest);
short __CDECL mkdir( char *path);
short __CDECL rm   ( char *path);
short __CDECL rmdir( char *path);

short __CDECL fileset( long mode, long value);

PROC Func[] = { 
				/* Shell functions */
				"cp", 	 cp_info,		cp,
				"mkdir", mkdir_info, 	mkdir,
				"mv",	 mv_info,		mv,
				"rm",	 rm_info, 		rm,
				"rmdir", rmdir_info,	rmdir,
				
				/* Low level file functions */
				"regexp",  regexp_info,	 regexp,
				"fgetdta", fgetdta_info, fgetdta,
				"fsetdta", fsetdta_info, fsetdta,
				"fsfirst", fsfirst_info, fsfirst,
				"fsnext",  fsnext_info,  fsnext,
				
				/* pour debugage */
				"fileset", fileset_info, fileset};

LDGLIB  Ldg[] = { VERSNUM, 11,
			 	  Func, "File utilities version " VERSION " by D.B‚r‚ziat (c) 2002",
				  0, 0L, 0L};

#endif

/******************************************************************/

static
char *strdup_plus( char *path, int size) {
	char *p = malloc( (strlen( path) + 1 + size)*sizeof( char));
	strcpy( p, path);
	return p;
}

/*
 * Internal function : low level file copy.
 */

static
int copy( char *src, char *dest, size_t blocsize) {
	int fi, fo;
	char *buff;
	size_t size;
	size_t count,i,rest;
	
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

	buff = (char *)malloc( size);
	
	if( buff == NULL) {
		Fclose( fi);
		Fclose( fo);
		return -69;
	}
	
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
	free( buff);
	return 0;
}

/*
 * Internal function : regular expression for
 * file matching (see UTILS/REGEXP).
 */


short __CDECL regexp( const char *rexp, const char *name) {
	char *pr;
	char c;

	while( *rexp || *name) {
		switch( *rexp) {
		case '*':
			c = *(rexp + 1);
			/* cas ou c'est la derniere expression reguliere */
			if( c == 0)
				return 1;
			else {
				/* Le cas ou 'c' est un caractere special
				 * est a traiter */
				 
				/* On avance dans name jusqu'a trouver 'c' ou
				 * la fin */
				while( *name != c && *name != 0) name ++;
				if( *name != c)
					return 0; /* caractere non trouv‚ */
				rexp ++;
			}
			break;
		case '?':
			if( *name == 0)
				return 0;
			rexp ++;
			name ++;
			break;
		case '[':	/* Liste de choix */
			/* On verifie que le crochet fermant est l… */
			if( (pr=strchr( rexp, ']')) != 0) {
				int found = 0;

				rexp ++;
				while( *rexp != ']' && !found) {
					/* on decrit la liste offert par rexp entre les []
					 * jusqu'a trouver un caractŠre ad‚quat */
					if( *rexp == '-') {
						/* cas special */
						if( *(rexp - 1) != '[' &&
							*(rexp + 1) != ']') {
							if( *name >= *(rexp - 1) &&
								*name <= *(rexp + 1) )
								found = 1;
							rexp += 2; /* on continue la recherche */
						} else
							return 0; /* erreur de syntaxe '-' doit etre entoure 
									   * de deux caracteres r‚guliers */
					} else {
						if( *name == *rexp) 
							found = 1; /* OK, caractere trouv‚ */
						rexp ++;   /* on recommence avec le suivant */
					}
				}
				if(found) {
					rexp = pr + 1;
					name ++;
				} else
					return 0;
			} else
				return 0; /* mauvaise syntaxe (pas de ]) */
			break;
		default:
			if( *rexp != *name) 
				return 0;
			rexp ++;
			name ++;
			break;
		}
	}
	return (*name == 0 && *rexp == 0);
}

/* Ask for Dopendir() function */

typedef struct {
     long magic;
     void *membot, *aes_start;
     long magic2, date;
     void (*chgres)(int res, int txt);
     long (**shel_vector)(void);
     char *aes_bootdrv;
     int  *vdi_device;
     void *reservd1, *reservd2, *reservd3;
     int  version;
} AESVARS;

typedef struct {
     long config_status;
     void *dosvars;
     AESVARS *aesvars;
} MAGX_INFOS;

static
int test_opendir( void) {
	long vers;
	MAGX_INFOS *cv;

	if( ldg_cookie( 'MiNT',(long *)&vers))
		return (vers>=0x090);
	else if( ldg_cookie( 'MagX',(long *)&cv) && cv->aesvars)
		return cv->aesvars->version >= 0x500;
	else
		return 0;
}

/* Fsfirst() dans Fsnext() functions working with
 * long filename (Fsfirst() and Fsnext() can work with
 * MiNT but not with MagiC).
 ******************************************************/

void __CDECL fsetdta( XDTA *dta) {
	if( opendir)
		_dta = dta;
	else
		Fsetdta( (DTA*) dta);
}

XDTA* __CDECL fgetdta( void) {
	if( opendir) 
		return _dta;
	else
		return (XDTA*)Fgetdta();
}

char buf[256];

short __CDECL fsnext( void) {
	if( opendir) {
		EXT*  ext = (EXT*) _dta -> d_reserved;
		char *p = _dta -> d_fname;
		long ret;
		int ok;
		
		do {
			ret = Dreaddir( 64, ext->d_dirhandle, p);
			if( ret == ENMFIL) {
				Dclosedir( ext -> d_dirhandle);
				free( ext -> d_regexp);
				if( ext -> d_dir) 
					free( ext -> d_dir);
				return ENMFIL;
			}
			if( ret == 0 || ret == ERANGE) {
				strcpy( p, p+4);
				strlwr( p);
				if( regexp( ext -> d_regexp, p)) {
					XATTR xattr;
					
					/* il faut remplir les autres informations 
					 * avec Fxattr */
					if( ext -> d_dir) {
						strcpy( buf, ext -> d_dir);
						strcat( buf, _dta -> d_fname);
					} else
						strcpy( buf, _dta -> d_fname);
				
					if( Fxattr( 1, buf, &xattr) == 0) {	
						_dta -> d_date = xattr.cdate;
						_dta -> d_time = xattr.ctime;
						_dta -> d_length = xattr.size;
						_dta -> d_attrib = xattr.attr;
					} else {
						/* On essaye avec Fattrib() */
						_dta -> d_attrib = Fattrib( buf, 0, 0);
						/* date et taille a p‚cho ailleurs */
					}					
					if( _dta -> d_attrib & FA_SUBDIR)
						ok = (ext->d_attr & FA_SUBDIR)?1:0;
					else if( _dta -> d_attrib & FA_HIDDEN)
						ok = (ext->d_attr & FA_HIDDEN)?1:0;
					else
						ok = 1;
				} else
					ok = 0;
			}
		} while( !ok);

		return ok?0:ENMFIL;
	} else {
		return Fsnext();
	}
}

short __CDECL fsfirst( char *dir, long attr) {
	if( opendir) {
		EXT *ext = (EXT*)_dta -> d_reserved;
		char *p = strrchr( dir, '\\');

		if( p) {
			char c;

			p ++;
			c = *p;
			ext -> d_regexp = strdup(p);
			strlwr( ext -> d_regexp);
			*p = '\0';
			ext -> d_dir = strdup(dir);
			ext -> d_dirhandle = Dopendir( dir, 0);
			*p = c;
		} else {
		  	ext -> d_regexp = dir;
		  	ext -> d_dirhandle = Dopendir( ".\\", 0);
		  	ext -> d_dir = NULL;
		}
		ext -> d_attr = attr;
		if( ext -> d_dirhandle < 0) 
			return (int) ext -> d_dirhandle;
		return fsnext();
	} else {
		return Fsfirst( dir, (int) attr);
	}
}

static
int cp_mv( char *s, char *d, int mv) {
	char *p, *q;
	char *src, *dest;
	int count = 0;
	int attr;
	unsigned long mfree;
	XDTA dta, *old;
	
	old = fgetdta();
	fsetdta( &dta);

	dest = strdup_plus( d, 32);
	src  = strdup_plus( s, 32);
	
	mfree = (unsigned long)Malloc( -1);
	mfree = (mfree/512)*512;
	
	if( dest[strlen(dest)-1] == '\\')
		dest[strlen(dest)-1] = '\0';
	attr = Fattrib( dest, 0, 0);
	
	if( fsfirst( src, 0) >= 0 && attr > 0 && attr & FA_SUBDIR) {
		p = strrchr( src, '\\');
		if( p)	p ++;
		else	p = src;
	
		strcat( dest, "\\");
		q = dest + strlen(dest);
	
		do {
			/* fixer le nom de d‚part */
			strcpy( p, dta.d_fname);
			/* fixer le nom d'arrive */
			strcpy( q, dta.d_fname);
			if( !copy( src, dest, mfree)) {
				count ++;
				if( mv) Fdelete( src);
			}
		} while( fsnext() >= 0);
	} else 
		if( !copy( src, dest, mfree)) {
			count ++;
			if( mv) Fdelete( src);
		}
	
	free( src);
	free( dest);

	fsetdta( old);
	return count;
}

/*
 * Binding of LDG functions
 *********************************************************/
 
short __CDECL cp( char *src, char *dest) {
	return  cp_mv( src, dest, 0);
}

short __CDECL mv( char *src, char *dest) {
	return  cp_mv( src, dest, 1);
}

/* mkdir r‚cursif */

short __CDECL mkdir( char *pa) {
	char *p, c;
	int count = 0;
	char *path = strdup_plus( pa, 1);

	if( path[strlen(path)-1] != '\\')
		strcat( path, "\\");

	for( p=strchr( path, '\\'); p; p = strchr( p, '\\')) {
		c = *p;
		*p = '\0';
		if( Fattrib( path, 0, 0) < 0) {
			/* r‚pertoire … cr‚er */
			Dcreate( path);
			count ++;
		}
		*p = c;
		p++;
	}
	free( path);
	return count;
}

short __CDECL rm( char *pa) {
	char *p;
	int count = 0;
	char *path = strdup( pa);
	XDTA dta, *old;
	
	old = fgetdta();
	fsetdta( &dta);
	
	if( path[strlen(path)-1] == '\\')
		path[strlen(path)-1] = '\0';
	if( Fattrib( path, 0, 0) & FA_SUBDIR) {
		free( path);
		return -1;
	}

	p = strrchr( path, '\\');
	if( p) p ++;
	else   p = path;
	
	if( fsfirst( path, 0) >= 0) {
		do {
			strcat( p, dta.d_fname);
			Fdelete( path);
			count ++;
		} while( fsnext() >= 0);
	}
	free( path);
	fsetdta( old);
	return count;
}

short __CDECL rmdir( char *pa) {
	char *p;
	int count = 0;
	char *path = strdup( pa);
	XDTA dta, *old;
	
	old = fgetdta();
	fsetdta( &dta);
	
	if( path[strlen(path)-1] == '\\')
		path[strlen(path)-1] = '\0';

	p = strrchr( path, '\\');
	if( p) p++;
	else p = path;

	if( fsfirst( path, FA_SUBDIR) >= 0) {
		do {
			strcat( p, dta.d_fname);
			Ddelete( path);
			count ++;
		} while( fsnext() >= 0);
	}
	free( path);
	fsetdta( old);
	return count;
}

/* Pour beta test */

short __CDECL fileset( long mode, long val) {
	switch( (int)mode) {
	case 0:	/* Set Opendir */
		opendir = (int)val;
		return 0; /* OK */
	case 1: /* Get Opendir */
		return opendir;
	case 2: /* Set AutoOpendir */
		opendir = test_opendir();
		return opendir;
	}
	return -1; /* ERR */
}

#ifndef STATICLIB
int main( void) {
	opendir = test_opendir();
	ldg_init( Ldg);
	return 0;
}

#endif
