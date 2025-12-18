/*
 * TEXTURED.LDG: a userdef library for windom
 * Copyright (c) 2005 Arnaud Bercegeay
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
 * $Source: /cvsroot/windom/windom/userdef/textured/global.h,v $
 * WinDom header file
 *  
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2005/11/03 22:18:05 $
 *   $Revision: 1.6 $
 */


#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#define MODIF_XEDIT 1
#define __GEMLIB_OLDNAMES

#ifndef _COMPILER_H
#include <compiler.h>
#endif

#include <windom/udlib.h>
#include <ldg.h>  /* for ldg_callback */

#define APP_MALLOC(size) ldg_callback(env->app_malloc,(long)(size))
#define APP_FREE(addr)   ldg_callback(env->app_free,addr)

#define FL3DMASK   0x0600  /* 3D objects AES 3.4    */

/* extended objects internal to this xobjc lib */
#define XFREESTR 	0x0C 	/* object G_BUTTON 3D (obspec = free_string) */
#define XTEDINFO 	0x0B 	/* object G_BOXTEXT & co 3D (obspec = tedinfo) */
#define XBFOBSPEC 	0x0A  	/* object G_BOXCHAR 3D (obspec = bfobspec) */

#define IS_FLAGS(pblk,flags) (((pblk)->pb_tree[(pblk)->pb_obj].ob_flags & (flags))?1:0)
#define IS_STATE(pblk,state) (((pblk)->pb_currstate & (state))?1:0)
#define MASK_STATE		(DISABLED|CROSSED|CHECKED|OUTLINED|SHADOWED)
#define STATE8  0x0100
#define STATE9  0x0200
#define STATE10 0x0400
#define STATE11 0x0800
#define STATE12 0x1000
#define STATE13 0x2000
#define STATE14 0x4000
#define STATE15 0x8000

#define BOLD		0x01
#define LIGHT		0x02
#define ITALIC		0x04
#define ULINE		0x08
#define OUTLINE		0x10
#define SHADOW		0x20

#define _FSM 0x5F46534DUL

#define CONF(uldata) ((uldata)->config)

#define BUT3D_PAT 	0
#define ONGLT_PAT 	0
#define DIALM_PAT 	0

#define SYSFONT 0

enum t_ind_mfdb { mfdb_bg=0, mfdb_button, mfdb_thumb, mfdb_thumb_back, mfdb_last};

typedef struct {
	short has_fsmgdos;
	short vdih;
	short nplanes;
	UDLIB_CONFIG *config;
	MFDB *mfdb[mfdb_last];
} XOBJC_DATA;

typedef struct {
	long    	wp_spec;
	short  		wp_type;
	short 		wp_oldf;
	XOBJC_DATA * wp_uldata;
} W_PARM;

typedef struct {
	TEDINFO *tedinfo;	/**< original Tedinfo structure */
	char *line;			/**< buffer */
	short pos; 			/**< first character showed */
	short curs;			/**< cursor position */
	short blcbeg;		/**< begin of the block */
	short blcend;		/**< end of the block */
	short flags;		/**< some flags */
	short size;			/**< size of the line */
	short wp_type;		/**< type of the object */
	XOBJC_DATA * wp_uldata; /**< global data of the library for this application */
} W_XEDIT;

/* values for W_XEDIT.flags */
#define XEDIT_CURS_ON 0x1
#define XEDIT_BLOC_ON 0x2

short loadimg_init( short *aes_global);
void  loadimg_exit( short *aes_global);
short loadimg_load( const char *filename, MFDB *mfdb, APPENV *);


long   _xobjc_extended( OBJECT *, long , long , APPENV *);
void   _xobjc_extfree ( OBJECT *, long , APPENV *);
char * _xobjc_string  ( OBJECT *, long , char *, APPENV *);
long   _xobjc_char    ( OBJECT *, long , long , APPENV *);
long   _xobjc_edit    ( OBJECT *, long , long , short* , long , long, GRECT*, APPENV *);
long   _xobjc_get_edpos( OBJECT *, long , long , long , APPENV *);
long   _xobjc_get_unextended( OBJECT *, int, APPENV *);

void	_xobjc_xedit_curs( XOBJC_DATA *, OBJECT *, int, GRECT *);
void	_xobjc_xedit_bloc( XOBJC_DATA *, OBJECT *, int, GRECT *);
int		_xobjc_width_curs( XOBJC_DATA *uldata, W_XEDIT *xedit, int curs);

size_t	_xobjc_realloc_xedit( size_t old, size_t len);

short __CDECL ub_button( PARMBLK *pblk);
short __CDECL ub_boxtitle( PARMBLK *pblk);
short __CDECL ub_ulinetext( PARMBLK *pblk);
short __CDECL ub_boxcircle( PARMBLK *pblk);
short __CDECL ub_onglet( PARMBLK *pblk);
short __CDECL ub_popuptext( PARMBLK *pblk);
short __CDECL ub_boxchar3d( PARMBLK *pblk);
short __CDECL ub_mntitle( PARMBLK *pblk);
short __CDECL ub_dialmover( PARMBLK *pblk);
short __CDECL ub_multiligne( PARMBLK *pblk);
short __CDECL ub_boxtext3d( PARMBLK *pblk);
short __CDECL ub_editable( PARMBLK *pblk); 
short __CDECL ub_but3d( PARMBLK *pblk);
short __CDECL ub_box3d( PARMBLK *pblk);
short __CDECL ub_ibox3d( PARMBLK *pblk); 

long   __CDECL xobjc_libinit( APPENV *);
void   __CDECL xobjc_udconfig( UDLIB_CONFIG *, APPENV *);
void   __CDECL xobjc_libexit( APPENV *);
long   __CDECL xobjc_extended( OBJECT *, long , long , APPENV *);
void   __CDECL xobjc_extfree( OBJECT *, long , APPENV *);
char * __CDECL xobjc_string( OBJECT *, long , char *, APPENV *);
long   __CDECL xobjc_char( OBJECT *, long , long , APPENV *);
long   __CDECL xobjc_edit( OBJECT *, long , long , short* , long , long , GRECT *, APPENV *);
long   __CDECL xobjc_get_edpos( OBJECT *, long , long , long , APPENV *);
long   __CDECL xobjc_get_unextended( OBJECT *, long , APPENV *);

#endif /* _GLOBAL_H_ */
