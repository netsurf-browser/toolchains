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
 * $Source: /cvsroot/windom/windom/src/rsrc_xload.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/02/16 21:29:59 $
 *   $Revision: 1.12 $
 */


#include <stddef.h>
#include <stdlib.h>
#ifdef __PUREC__
#include <tos.h>
#endif
#include <string.h>
#include "globals.h"
#include "cicon.h"

#ifndef FO_READ
#define FO_READ 0
#endif

/*  Loading of resource file.
 *  The following code is based on EG_LIB (by Christophe Boyanique)
 *  The validity of the RSC file is not checked (magic number)
 */

/*
 *	relocation of the RSC in memory
 */

static
void _relocRSC( APPvar *app, void *_rsc) {
	RSC 		*rsc = _rsc;
	RSHDR		*head;
	RSXHDR		*xhead=NULL;
	NRSHDR		*nhead;
	CICONBLK	*table;
	CICON		*cicon,*cold;
	long		*lp,iclen,num_cicon;
	long		len;
	int		i,j,type;
	char		*p;

	head = (RSHDR *)rsc->adr;
	if( head->rsh_vrsn==NRSC_LONG) {
		xhead=(RSXHDR *)rsc->adr;
		/*************************************************************************
			count of elements in arrays
		*************************************************************************/
		rsc->head.nobs		=	xhead->rsh_nobs;
		rsc->head.ntree		=	xhead->rsh_ntree;
		rsc->head.nted		=	xhead->rsh_nted;
		rsc->head.nib		=	xhead->rsh_nib;
		rsc->head.ncib		=	0;
		rsc->head.nbb		=	xhead->rsh_nbb;
		rsc->head.nfstr		=	xhead->rsh_nstring;
		rsc->head.nfimg		=	xhead->rsh_nimages;
		/*************************************************************************
			pointer to arrays
		*************************************************************************/
		rsc->head.trindex	=	(OBJECT **)(rsc->adr+xhead->rsh_trindex);
		rsc->head.object	=	(OBJECT *)(rsc->adr+xhead->rsh_object);
		rsc->head.tedinfo	=	(TEDINFO *)(rsc->adr+xhead->rsh_tedinfo);
		rsc->head.iconblk	=	(ICONBLK *)(rsc->adr+xhead->rsh_iconblk);
		rsc->head.bitblk	=	(BITBLK *)(rsc->adr+xhead->rsh_bitblk);
		rsc->head.frstr		=	(char **)(rsc->adr+xhead->rsh_frstr);
		rsc->head.frimg		=	(BITBLK **)(rsc->adr+xhead->rsh_frimg);
		len=xhead->rsh_rssize;
	} else {
		/*************************************************************************
			count of elements in arrays
		*************************************************************************/
		rsc->head.nobs		=	head->rsh_nobs;
		rsc->head.ntree		=	head->rsh_ntree;
		rsc->head.nted		=	head->rsh_nted;
		rsc->head.nib		=	head->rsh_nib;
		rsc->head.ncib		=	0;
		rsc->head.nbb		=	head->rsh_nbb;
		rsc->head.nfstr		=	head->rsh_nstring;
		rsc->head.nfimg		=	head->rsh_nimages;
		/*************************************************************************
			pointer to arrays
		*************************************************************************/
		rsc->head.trindex	=	(OBJECT **)(rsc->adr+head->rsh_trindex);
		rsc->head.object	=	(OBJECT *)(rsc->adr+head->rsh_object);
		rsc->head.tedinfo	=	(TEDINFO *)(rsc->adr+head->rsh_tedinfo);
		rsc->head.iconblk	=	(ICONBLK *)(rsc->adr+head->rsh_iconblk);
		rsc->head.bitblk	=	(BITBLK *)(rsc->adr+head->rsh_bitblk);
		rsc->head.frstr		=	(char **)(rsc->adr+head->rsh_frstr);
		rsc->head.frimg		=	(BITBLK **)(rsc->adr+head->rsh_frimg);
		len=head->rsh_rssize;
	}

	/****************************************************************************
		rs_trindex
	****************************************************************************/
	lp = (long *)rsc->head.trindex;
	for( i=0;i<rsc->head.ntree;i++) {
		*lp=*lp+rsc->adr;
		lp++;
	}
	/****************************************************************************
		TEDINFO
	****************************************************************************/
	for( i=0; i<rsc->head.nted; i++) {
		rsc->head.tedinfo[i].te_ptext+=rsc->adr;
		rsc->head.tedinfo[i].te_ptmplt+=rsc->adr;
		rsc->head.tedinfo[i].te_pvalid+=rsc->adr;
	}
	/****************************************************************************
		ICONBLK
	****************************************************************************/
	for( i=0; i<rsc->head.nib; i++) {
		lp=(long *)&rsc->head.iconblk[i].ib_pmask;	*lp=*lp+rsc->adr;
		lp=(long *)&rsc->head.iconblk[i].ib_pdata;	*lp=*lp+rsc->adr;
		rsc->head.iconblk[i].ib_ptext+=rsc->adr;
	}
	/****************************************************************************
		BITBLK
	****************************************************************************/
	for( i=0; i<rsc->head.nbb; i++) {
		lp=(long *)&rsc->head.bitblk[i].bi_pdata;
		*lp=*lp+rsc->adr;
	}
	/****************************************************************************
		free STRING
	****************************************************************************/
	for( i=0; i<rsc->head.nfstr; i++)
		rsc->head.frstr[i]+=rsc->adr;
	/****************************************************************************
		free IMAGE
	****************************************************************************/
	for( i=0; i<rsc->head.nfimg; i++) {
		lp=(long *)&rsc->head.frimg[i]; 
		*lp=*lp+rsc->adr;
	}
	/****************************************************************************
		CICONBLK
	****************************************************************************/
	if( head->rsh_vrsn>1 && rsc->len>len) {
		if (head->rsh_vrsn==NRSC_CIBK)
			nhead=(NRSHDR *)(rsc->adr+(long)head->rsh_rssize);
		else
			nhead=(NRSHDR *)(rsc->adr+(long)xhead->rsh_rssize);
		rsc->head.ciconblk=(CICONBLK *)(rsc->adr+nhead->cicon_offset);
		lp=(long *)rsc->head.ciconblk;
		while (!lp[rsc->head.ncib])
			rsc->head.ncib+=1;

		if (lp[rsc->head.ncib]!=-1L)
			rsc->head.ncib=0;
		else {
			table=(CICONBLK *)&lp[rsc->head.ncib+1];
			for( i=0; i<rsc->head.ncib; i++) {
				lp[i]=(long)table;
				p=(char *)&table[1];
				table->monoblk.ib_pdata = (INT16 *)p;
				iclen=table->monoblk.ib_wicon/8*table->monoblk.ib_hicon;
				p+=iclen;
				table->monoblk.ib_pmask = (INT16 *)p;
				p+=iclen;
				table->monoblk.ib_ptext=(char *)p;
				cicon=(CICON *)&p[12];
				p+=12L;
				cold=cicon;
				if( (num_cicon=(long)table->mainlist) > 0 ) {
					table->mainlist=cicon;
					for( j=0; j<num_cicon; j++) {
						p=(char *)&cicon[1];
						cicon->col_data=(INT16 *)p;
						p+=iclen*cicon->num_planes;
						cicon->col_mask=(INT16 *)p;
						p+=iclen;
						if( cicon->sel_data!=NULL) {
							cicon->sel_data=(INT16 *)p;
							p+=iclen*cicon->num_planes;
							cicon->sel_mask=(INT16 *)p;
							p+=iclen;
						}
						cicon->next_res=(CICON *)p;
						cold=cicon;
						cicon=(CICON *)p;
					}
					cold->next_res=NULL;
				}
				table=(CICONBLK *)p;
			}
		}
	} else {
		rsc->head.cicon=NULL;
		rsc->head.ciconblk=NULL;
	}

	/****************************************************************************
		OBJECT:	fix OB_SPEC data
	****************************************************************************/
	for( i=0; i<rsc->head.nobs; i++) {
		type=rsc->head.object[i].ob_type&0xFF;
		if (type==G_CICON)
			rsc->head.object[i].ob_spec.index=lp[rsc->head.object[i].ob_spec.index];
		else if ( (type!=G_BOX) && (type!=G_IBOX) && (type!=G_BOXCHAR) )
			rsc->head.object[i].ob_spec.index+=rsc->adr;
		mt_rsrc_obfix(&rsc->head.object[i],0,app->aes_global);
	}
	
	/* initialise XRSRCFIX data */
	memset (&rsc->fix, 0, sizeof (XRSRCFIX));
	/* fix color icons */
	mt_RsrcFixCicon( app, rsc->head.object, (int)rsc->head.nobs, (int)rsc->head.ncib, NULL, &rsc -> fix);
}


/**
 * @brief load multiple extended GEM resource file.
 *
 * @param app application descriptor,
 * @param name GEM resource file name to load.
 * @return a pointer to a ressource descriptor in memory or \b NULL if an error occurs.
 *
 * As mt_RsrcLoad(), mt_RsrcXload() load in memory a resource file. The difference
 * is RsrcXload() can load severals resource file. In addition ,RsrcXload()
 * can load extended resource files (as specified by AES 3.4) on all TOS
 * system.
 *
 * mt_RsrcXload()
 * use the \b PATH environment variable to locate the resource file (in this case, the
 * filename should not be a pathname). The function returns a pointer
 * which identify the resource or a \b NULL value if an error occurs. The
 * pointer must be kept in memory because it is used by mt_RsrcGaddr() to
 * get an object address in resource and it is used by mt_RsrcXfree() to
 * release resource memory. Icons colors are automatically fixed by
 * mt_RsrcXload() using mt_RsrcFixCicon() and transformed into a XCICON
 * extended type. Due to this, color icons are displaying on any TOS
 * version. To access to ICONBLK structure of a XCICON object, use
 * the macro GET_XCICON().
 *
 * \b Example
 * @code {
  void *rsc1, *rsc2;
  OBJECT *tree;
  // Loads resources 
  rsc1 = RsrcXload( "myrsc1.rsc");
  rsc2 = RsrcXload( "myrsc2.rsc");
  // Get an object
  RsrcGaddr( rsc1, R_TREE, FORM1, &tree);
  ...
  RsrcXfree( rsc2);
  RsrcXfree( rsc1);
}
@endcode
*
* @sa mt_RsrcXfree(), mt_RsrcGaddr(), mt_RsrcLoad().
 */

void *mt_RsrcXload( APPvar *app, const char *name) {
	int	ha;
	RSC *rsc;
	char realname[255];
	
	rsc = (RSC *)malloc( sizeof(RSC));
	if( !rsc) return NULL;
	
	rsc->adr	=	0;
	rsc->ublk	=	0;
	rsc->cicon	=	0;

	strcpy( realname, name);
	mt_shel_find( realname, app->aes_global);
	ha=(int)Fopen( realname, FO_READ);
	
	/* try other format */
	if( ha<0) {
		conv_path( strcpy( realname, name));
		mt_shel_find( realname, app->aes_global);
		ha=(int)Fopen( realname, FO_READ);
	}

	if( ha<0) {
		app->error = -33;
		return NULL;
	}
	
	rsc->len=Fseek(0L,ha,2);
	Fseek(0L,ha,0);

#ifdef __SOZOBONX__
	rsc->adr = (long)lalloc( rsc->len * (long)sizeof(char));
#else
	rsc->adr = (long)malloc( rsc->len*sizeof(char));
#endif
	if( rsc->adr == 0L) {
		Fclose(ha);
		app->error = -39;
		return NULL;
	}
	
	if( Fread(ha,rsc->len,(void *)rsc->adr) != rsc->len ) {
		Fclose(ha);
		free((void*)rsc->adr);
		app->error = -1;
		return NULL;
	}
	Fclose(ha);
	_relocRSC(app,rsc);
	return rsc;
}

