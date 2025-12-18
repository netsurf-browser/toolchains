/*
 * FILEUTIL.LDG
 * (c) 2001-2003 By Dominique B‚r‚ziat
 * Usefull functions to copy, move, delete
 * files and directories.
 *
 * Example of ldg header containing both external
 * declaration of functions and binding of functions.
 */

#ifndef _FILEUTIL_H_
#define _FILEUTIL_H_

#include <ldg.h>

#ifdef STATICLIB
# define __LDGPROTO(a)	(a)
#else
# define __LDGPROTO(a)	__CDECL (*a)
#endif

typedef struct {
    char            d_reserved[21];
    unsigned char   d_attrib;
    unsigned int    d_time;
    unsigned int    d_date;
    unsigned long   d_length;
    char            d_fname[64];
} XDTA;  /* Extended DTA structure */

#ifndef BIND_FILEUTIL		/* define this constant to bind functions */

extern short __LDGPROTO(regexp) ( const char *, const char *);
extern void  __LDGPROTO(fsetdta)( XDTA *);
extern XDTA* __LDGPROTO(fgetdta)( void);
extern short __LDGPROTO(fsfirst)( char *, long attr);
extern short __LDGPROTO(fsnext) ( void);

extern short __LDGPROTO(cp)		( char *src, char *dest);
extern short __LDGPROTO(mkdir)	( char *path);
extern short __LDGPROTO(mv)		( char *src, char *dest);
extern short __LDGPROTO(rm)		( char *path);
extern short __LDGPROTO(rmdir)	( char *path);

#else

short __LDGPROTO(regexp) ( const char *, const char *);
void  __LDGPROTO(fsetdta)( XDTA *);
XDTA* __LDGPROTO(fgetdta)( void);
short __LDGPROTO(fsfirst)( char *, long attr);
short __LDGPROTO(fsnext) ( void);

short __LDGPROTO(cp)	( char *src, char *dest);
short __LDGPROTO(mkdir)	( char *path);
short __LDGPROTO(mv)	( char *src, char *dest);
short __LDGPROTO(rm)	( char *path);
short __LDGPROTO(rmdir)	( char *path);

void init_ldgfile( LDG *ldg) {	 
	cp    = ldg_find( "cp",    ldg);
	mkdir = ldg_find( "mkdir", ldg);
	mv    = ldg_find( "mv",    ldg);
	rm    = ldg_find( "rm",    ldg);
	rmdir = ldg_find( "rmdir", ldg);

	regexp  = ldg_find( "regexp",  ldg);
	fsetdta = ldg_find( "fsetdta", ldg);
	fgetdta = ldg_find( "fgetdta", ldg);
	fsfirst = ldg_find( "fsfirst", ldg);
	fsnext  = ldg_find( "fsnext",  ldg);
}
#endif

#endif /* _FILEUTIL_H */

