/*
 * WinDom: a high level GEM library
 * Copyright (c) 1997-2006 windom authors (see AUTHORS file)
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
 * $Source: /cvsroot/windom/windom/src/cookie.h,v $
 * Module : definitions of system cookies
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.8 $
 */

#ifndef __WINDOM_COOKIE__
#define __WINDOM_COOKIE__

/* WinX */

#define WINX_COOKIE	0x57494E58UL	/* 'WINX' */

/* MagiC */
#define MAGX_COOKIE     0x4D616758UL	/* 'MagX' */
typedef struct {
     long magic;                   /* muž $87654321 sein              */
     void *membot;                 /* Ende der AES- Variablen         */
     void *aes_start;              /* Startadresse                    */
     long magic2;                  /* ist 'MAGX'                      */
     long date;                    /* Erstelldatum ttmmjjjj           */
     void (*chgres)(int res, int txt);  /* Aufl”sung „ndern           */
     long (**shel_vector)(void);   /* residentes Desktop              */
     char *aes_bootdrv;            /* von hieraus wurde gebootet      */
     int  *vdi_device;             /* vom AES benutzter VDI-Treiber   */
     void *reservd1;
     void *reservd2;
     void *reservd3;
     int  version;                 /* z.B. $0201 ist V2.1             */
     int  release;                 /* 0=alpha..3=release              */
} AESVARS;

typedef struct {
     long    config_status;
     void *dosvars;
     AESVARS *aesvars;
     void *res1;
     void *hddrv_functions;
     long status_bits;             /* MagiC 3 ab 24.5.95         */
} MAGX_INFOS;

/* MiNT */
#define MiNT_COOKIE     0x4D694E54UL	/* 'MiNT' */

/* Let's Them Fly */
#define LTMF_COOKIE		0x4C544D46UL	/* 'LTMF' */

/* Nvdi */
#define NVDI_COOKIE		0x4E564449UL	/* 'NVDI' */

/* Naes */
#define NAES_COOKIE		0x6E414553UL	/* 'nAES' */

/* BubbleGEM */

#define BUBBLEGEM_REQUEST  0xBABA
#define BUBBLEGEM_SHOW     0xBABB
#define BUBBLEGEM_ACK      0xBABC
#define BUBBLEGEM_ASKFONT  0xBABD
#define BUBBLEGEM_FONT     0xBABE
#define BUBBLEGEM_HIDE     0xBABF

/* drapeaux pour 'BHLP' */
#define BGC_FONTCHANGED		0x01
#define BGC_NOWINSTYLE		0x02
#define BGC_SENDKEY			0x04
#define BGC_DEMONACTIVE		0x08
#define BGC_TOPONLY			0x10

/* drapeaux pour  mot 7 de BUBBLEGEM_SHOW */
#define BGS7_USRHIDE		0x0001
#define BGS7_MOUSE 			0x0004


#define BHLP_COOKIE	0x42484C50UL	/* 'BHLP' */
#define BGEM_COOKIE 0x4247454DUL	/* 'BGEM' */

typedef struct {
	long   magic;   /* nombre magique valant 'BGEM' */
	long   size;    /* taille de la structure R07: 18 */
	int    release; /* num‚ro de version R07: 7, pas de valeur ant‚rieur … 5 */
	int    active;  /* <>0, if straight line a help displayed becomes;
                       	 0  else */
	MFORM *mhelp;   /* forme de la souris de Bubble */
	int    dtimer;  /* depuis R06, D„mon-Timer; Default 200ms; */
} BGEM;


#endif /* __WINDOM_COOKIE__ */

/* EOF */
