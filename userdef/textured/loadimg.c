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
 * $Source: /cvsroot/windom/windom/userdef/textured/loadimg.c,v $
 * WinDom header file
 *  
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2005/11/03 22:18:05 $
 *   $Revision: 1.6 $
 */


#include <stdlib.h>
#include <ldg.h>
#include <string.h>  /* strrchr() */
#include "global.h"

extern char * ldg_getpath( void);

static LDG *ldg_me = NULL;
static char img_path[256];

typedef struct 
{
	char  filename[32];     /* file name of the RIM/WIM      */
	char  Header  [8] ;     /* 'READ_IMG' if it's a RIM      */
	                        /* 'WRIT_IMG' if it's a WIM      */
	char  Format  [4] ;     /* '_VDI' | '_SHI' | '_VSH'      */
	short Type ;            /* see PARX developer doc.       */
	char  Id      [8] ;     /* ID sur 8 caractäres           */
	short Version     ;     /* version in decimal format     */
	char  Description[31];  /* 30 caractäres + octet nul     */
	char  Configurable   ;  /* 1 s'il est configurable       */
} MEINFOS ;

typedef struct 
{
	/********************** BEGIN of the MFDB-like structure ********/
	long  rsvd1;          /* reserved                               */
	short width;          /* width of the image in pixels           */
	short height;         /* height of the image in pixels          */
	short wdwidth;        /* width of the img in nb of words        */
	short stand;          /* standard used : 0 | 1                  */
	short nplanes;        /* nb planes for xxx_palette & xxx_img    */
	long  rsvd2;          /* reserved                               */
	short rsvd3;          /* reserved                               */
	/*********************** END of the MFDB-like structure *********/
	long  rsvd4;          /* reserved                               */
	long  rsvd5;          /* reserved                               */
	long  rsvd6;          /* reserved                               */
	long  handle;         /* VDI Handle needed for some trnfm...    */
	/*********************** Image & palette in the native format   */
	long   size_palette;  /* size of the palette in bytes           */
	char * buff_palette;  /* buffer for the palette                 */
	long   size_img;      /* size of the image in bytes             */
	char * buff_img;      /* buffer for the image                   */
	/*********************** Image converted in the 24 bits format  */
	long   size_R8V8B8;   /* size of image in the 24bits format     */
	char * buff_R8V8B8;   /* buffer for the image in the 24b fmt    */
	/*********************** Image & palette in the screen format   */	
	long   size_DDpalette;/* size of the "DD" palette in bytes      */
	char * buff_DDpalette;/* buffer for the "DD" palette            */
	long   size_DDimg;    /* size of the "DD" image in bytes        */
	char * buff_DDimg;    /* buffer for the "DD" image              */
	/*********************** Informations about the RIM/WIM used    */
	MEINFOS * meinfos;    /* info about the WIM/RIM used            */
} MEIMG;

/* parametres "mode" de RIM_LoadImg() */
#define ME_FMT_NATIF          0x0001L
#define ME_FMT_R8V8B8         0x0002L
#define ME_FMT_DIRECTDISPLAY  0x0004L
#define ME_RGB_TO_DISK        0x0100L

static long __CDECL (*RIM_OpenImg)(MEIMG * meimg, const char * filename, long mode) = NULL;
static long __CDECL (*RIM_CloseImg)(MEIMG * meimg) = NULL;
static long __CDECL (*RIM_LoadImg)(MEIMG * meimg, long mode, long no_algo, long vdi_handle, short * global) = NULL;
static void __CDECL (*ME_Init)( short * global, long mode) = NULL;
static void __CDECL (*ME_ExplainError)( short * global) = NULL;


short loadimg_init( short *aes_global)
{
	char *separator;
	
	ldg_me = ldg_open( "me.ldg", aes_global);
	if (ldg_me == NULL)
		return -1;
	
	RIM_OpenImg        = ldg_find( "RIM_OpenImg"	 ,ldg_me);
	RIM_CloseImg       = ldg_find( "RIM_CloseImg"	 ,ldg_me);
	RIM_LoadImg        = ldg_find( "RIM_LoadImg"	 ,ldg_me);
	ME_Init            = ldg_find( "ME_Init"		 ,ldg_me);
	ME_ExplainError    = ldg_find( "ME_ExplainError" ,ldg_me);
	
	if (ME_Init==NULL || RIM_OpenImg==NULL || RIM_CloseImg==NULL || RIM_LoadImg==NULL || ME_ExplainError==NULL) {
		ldg_close(ldg_me, aes_global);
		ldg_me = NULL;
		return -2;
	} 
	
	ME_Init(aes_global,0L);
	
	strcpy( img_path, ldg_getpath() ); 
	if ( (separator = strrchr( img_path, '/')) != NULL)
		strcpy(separator,"/img/");
	else if ( (separator = strrchr( img_path, '\\')) != NULL)
		strcpy(separator,"\\img\\");
	else /* is this possible ? */
		strcpy(img_path,"img\\");
	
	return 0;
}

void loadimg_exit( short *aes_global)
{
	ldg_close(ldg_me, aes_global);
	ldg_me = NULL;
}

short loadimg_load( const char *_filename, MFDB *mfdb, APPENV * env)
{
	MEIMG meimg;
	long err;
	char filename[256];
	
	strcpy( filename, img_path);
	strcat( filename, _filename);
	
	err = RIM_OpenImg(&meimg,filename,0L);
	if (err < 0) {
		ME_ExplainError(env->aes_global);
		return -1;
	}
	
	/* allocate memory for the palette */
	if (meimg.size_DDpalette)
	{
		meimg.buff_DDpalette = (char*) APP_MALLOC ( meimg.size_DDpalette );
		if (meimg.buff_DDpalette == NULL) {
			mt_form_alert(1,"[1][Memory allocation failed|for buff_DDpalette][continue]",env->aes_global);
			RIM_CloseImg(&meimg);
			return -1;
		}
	}
	
	/* allocate memory for the img */
	meimg.buff_DDimg = (char*) APP_MALLOC ( meimg.size_DDimg );
	if (meimg.buff_DDimg == NULL)
	{
		mt_form_alert(1,"[1][Memory allocation failed|for buff_DDimg][continue]",env->aes_global);
		if (meimg.size_DDpalette && meimg.buff_DDpalette)
			APP_FREE(meimg.buff_DDpalette);
		RIM_CloseImg(&meimg);
		return -1;
	}
	
	/* load the img in DIRECT_DISPLAY format */
	err = RIM_LoadImg(&meimg,ME_FMT_DIRECTDISPLAY,6L,env->vdih,env->aes_global);
	if (err < 0) {
		ME_ExplainError(env->aes_global);
		if (meimg.size_DDpalette && meimg.buff_DDpalette)
			APP_FREE(meimg.buff_DDpalette);
		APP_FREE(meimg.buff_DDimg);
		RIM_CloseImg(&meimg);
		return -1;
	}
	
	/* to end properly */
	RIM_CloseImg(&meimg);
	
	/* init MFDB structure */
	mfdb->fd_r1 = 0;
	mfdb->fd_r2 = 0;
	mfdb->fd_r3 = 0;
	mfdb->fd_w  = meimg.width;
	mfdb->fd_h  = meimg.height;
	mfdb->fd_wdwidth = meimg.wdwidth;
	mfdb->fd_stand = meimg.stand;
	mfdb->fd_addr = meimg.buff_DDimg;
	mfdb->fd_nplanes = env->nplanes;
	
	/* we don't care about the palette, so we can free it now */
	if (meimg.size_DDpalette && meimg.buff_DDpalette)
		APP_FREE(meimg.buff_DDpalette);
	
	return 0;
}
