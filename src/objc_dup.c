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
 * $Source: /cvsroot/windom/windom/src/objc_dup.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.12 $
 */


#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"

#define MAX_ICONTEXT_LEN  128

/* duplicate the ob_spec field
 * WARNING: tree[ob] shall not be a G_USERDEF object trasnformed by udlib
 */

static void duplicate_obspec( APPvar *app, OBJECT *tree, int ob, WINDOW *win) {
	TEDINFO *tedinfo;
	char *freestring;
	long mem_size, img_size;
	char * mem_adr;
	USERBLK *user;
	USER_DRAW *udraw;
	ICONBLK *iconblk;
	BITBLK *bitblk;
	const CICON * cicon_read;
	CICON * cicon_write;
	CICONBLK * ciconblk;
	
	switch( tree[ob].ob_type & 0x00FF) {
	case G_TEXT:
	case G_FTEXT:
	case G_BOXTEXT:
	case G_FBOXTEXT:
		tedinfo = (TEDINFO *)malloc( sizeof( TEDINFO));
		memcpy( tedinfo, tree[ob].ob_spec.tedinfo, sizeof(TEDINFO));
		
		mem_size = (tedinfo->te_txtlen + 1) * 2;
		mem_size += tedinfo->te_tmplen + 1;
		mem_adr = __malloc_obspec(app,&tree[ob],mem_size);
		
		if (tedinfo->te_ptext) {
			strncpy(mem_adr,tedinfo->te_ptext,tedinfo->te_txtlen);
			tedinfo->te_ptext = mem_adr;
		}
		
		mem_adr += tedinfo->te_txtlen + 1;
		if (tedinfo->te_pvalid) {
			strncpy(mem_adr,tedinfo->te_pvalid,tedinfo->te_txtlen);
			tedinfo->te_pvalid = mem_adr;
		}
		
		mem_adr += tedinfo->te_txtlen + 1;
		if (tedinfo->te_ptmplt) {
			strncpy(mem_adr,tedinfo->te_ptmplt,tedinfo->te_tmplen);
			tedinfo->te_ptmplt = mem_adr;
		}
		
		tree[ob].ob_spec.tedinfo = tedinfo;
		break;
		
	case G_STRING:
	case G_BUTTON:
	case G_TITLE:
		freestring = __malloc_obspec(app,&tree[ob],strlen( tree[ob].ob_spec.free_string)+1);
		strcpy(freestring,tree[ob].ob_spec.free_string);
		tree[ob].ob_spec.free_string = freestring;
		break;
	
	case G_IMAGE:
#if 0 /* FIXME: temporary disabled because there's a bug somewhere */
		img_size =   tree[ob].ob_spec.bitblk->bi_wb 
		           * tree[ob].ob_spec.bitblk->bi_hl;
		mem_size = sizeof(BITBLK) + img_size;
		mem_adr = __malloc_obspec(app,&tree[ob],mem_size);

		bitblk = (BITBLK*) mem_adr;
		memcpy( bitblk, tree[ob].ob_spec.bitblk, sizeof(BITBLK));
		
		mem_adr += sizeof(BITBLK);
		bitblk->bi_pdata = (short *) mem_adr;
		memcpy( bitblk->bi_pdata, tree[ob].ob_spec.bitblk->bi_pdata, img_size);
		
		tree[ob].ob_spec.bitblk = bitblk;
#endif
		break;
	
	case G_ICON:
#if 0 /* FIXME: temporary disabled because there's a bug somewhere */
		img_size =   ((tree[ob].ob_spec.iconblk->ib_wicon + 15) / 16 )  /* width in WORDs of 16bits */
		            * ( tree[ob].ob_spec.iconblk->ib_hicon )            /* height */
					* 2;                                                /* width is now in BYTEs */
		
		mem_size  = sizeof(ICONBLK);
		mem_size += img_size * 2;  /* mask + image */
		mem_size += MAX_ICONTEXT_LEN;  /* max size for text of icon */
		
		mem_adr = __malloc_obspec(app,&tree[ob],mem_size);
		iconblk = (ICONBLK *) mem_adr;
		memcpy( iconblk, tree[ob].ob_spec.iconblk, sizeof(ICONBLK));
		
		mem_adr += sizeof(ICONBLK);
		iconblk->ib_pmask = (short *) mem_adr;
		memcpy( iconblk->ib_pmask, tree[ob].ob_spec.iconblk->ib_pmask, img_size);
		
		mem_adr += img_size;
		iconblk->ib_pdata = (short *) mem_adr;
		memcpy( iconblk->ib_pdata, tree[ob].ob_spec.iconblk->ib_pdata, img_size);
		
		mem_adr += img_size;
		iconblk->ib_ptext = (char *) mem_adr;
		strncpy(iconblk->ib_ptext, tree[ob].ob_spec.iconblk->ib_ptext, MAX_ICONTEXT_LEN);
		
		tree[ob].ob_spec.iconblk = iconblk;
#endif
		break;
		
	case G_CICON:
#if 0 /* FIXME: temporary disabled because there's a bug somewhere */
		img_size =   ((tree[ob].ob_spec.ciconblk->monoblk.ib_wicon + 15) / 16 )  /* width in WORDs of 16bits */
		            * (tree[ob].ob_spec.ciconblk->monoblk.ib_hicon )             /* height */
					* 2;                                                        /* width is now in BYTEs */
		
		mem_size  = sizeof(CICONBLK);
		mem_size += img_size * 2;  /* mask + image */
		mem_size += MAX_ICONTEXT_LEN;  /* max size for text of icon */
		
		cicon_read = tree[ob].ob_spec.ciconblk->mainlist;
		while (cicon_read) {
			mem_size += sizeof(CICON);
			mem_size += img_size * cicon_read->num_planes * 2;  /* 1 cicon = 2 * color image */
			mem_size += img_size * 2;                           /*         + 2 * mono mask */
			cicon_read = cicon_read->next_res;
		}
		
		mem_adr = __malloc_obspec(app,&tree[ob],mem_size);
		ciconblk = (CICONBLK*) mem_adr;
		memcpy( ciconblk, tree[ob].ob_spec.ciconblk, sizeof(CICONBLK));

		iconblk = &(ciconblk->monoblk);
		
		mem_adr += sizeof(CICONBLK);
		iconblk->ib_pmask = (short *) mem_adr;
		memcpy( iconblk->ib_pmask, tree[ob].ob_spec.ciconblk->monoblk.ib_pmask, img_size);
		
		mem_adr += img_size;
		iconblk->ib_pdata = (short *) mem_adr;
		memcpy( iconblk->ib_pdata, tree[ob].ob_spec.ciconblk->monoblk.ib_pdata, img_size);
		
		mem_adr += img_size;
		iconblk->ib_ptext = (char *) mem_adr;
		strncpy(iconblk->ib_ptext, tree[ob].ob_spec.ciconblk->monoblk.ib_ptext, MAX_ICONTEXT_LEN);
		
		mem_adr += MAX_ICONTEXT_LEN;
		cicon_read = tree[ob].ob_spec.ciconblk->mainlist;
		cicon_write = NULL;
		while( cicon_read) {
			if (cicon_write)  cicon_write->next_res = (CICON*) mem_adr;
			else              ciconblk->mainlist    = (CICON*) mem_adr;
			cicon_write = (CICON*) mem_adr;
			cicon_write->num_planes = cicon_read->num_planes;
			mem_adr += sizeof(CICON);
			cicon_write->col_data = (short*) mem_adr;
			memcpy( cicon_write->col_data, cicon_read->col_data, img_size * cicon_read->num_planes);
			mem_adr += img_size * cicon_read->num_planes;
			cicon_write->col_mask = (short*) mem_adr;
			memcpy( cicon_write->col_mask, cicon_read->col_mask, img_size);
			mem_adr += img_size;
			cicon_write->sel_data = (short*) mem_adr;
			memcpy( cicon_write->sel_data, cicon_read->sel_data, img_size * cicon_read->num_planes);
			mem_adr += img_size * cicon_read->num_planes;
			cicon_write->sel_mask = (short*) mem_adr;
			memcpy( cicon_write->sel_mask, cicon_read->sel_mask, img_size);
			mem_adr += img_size;
			cicon_write->next_res = NULL;
			cicon_read = cicon_read->next_res;
		}
		
		tree[ob].ob_spec.ciconblk = ciconblk;
#endif
		break;
		
	case G_USERDEF:
		switch (tree[ob].ob_type >> 8) {
		case XCICON :
			/* TODO FIXME
			 * cicon are not really duplicated at the moment.
			 * the ob_spec pointer of the duplicated object point to
			 * the ob_spec data of the original OBJECT.
			 *
			 * If the CICON of the duplicated object is changed (or if
			 * its text field is changed), then
			 * the CICON of the original object (or its text field) 
			 * will be changed too.
			 */
			break;
		case USERDRAW:
			user = (USERBLK *)malloc( sizeof(USERBLK));
			memcpy( user, tree[ob].ob_spec.userblk, sizeof(USERBLK));
			udraw = (USER_DRAW *)malloc( sizeof(USER_DRAW));
			memcpy( udraw, (void*)user->ub_parm, sizeof(USER_DRAW));
			udraw -> win  = win;
			user->ub_parm = (long)udraw;
			tree[ob].ob_spec.userblk = user;
			break;
		}
	}

}

/**
 * @brief Objects tree duplication.
 *
 * @param app application descriptor,
 * @param src objects tree to duplicate,
 * @param win window hosting the \e src object tree if \e src contains userdefs objects,
 * @return a copy of \e src object tree.
 * 
 * mt_ObjcDup() performs a dynamic copy of an object tree. The new
 * object tree created try to be totally memory independant of the
 * source. However, some objects are not totally duplicated yet. It 
 * is the case with icons and images bitmap. We plan to fix that in
 * futur. An object tree created
 * with mt_ObjcDup() should be freed by mt_ObjcFree().
 *
 * If object tree contains \b USERDRAW objects, the parameter
 * \e win is absolutely required. In other case, \b NULL is a
 * correct value.
 *
 * This function is used by mt_FormCreate() to open several formulars
 * with the same objects tree. If the \b WS_FORMDUP bit of
 * the \e win->status window descriptor field is set to 1, the
 * standard destroy function releases the memory with mt_ObjcFree().
 * 
 * Toolbars and menus attached to a window with mt_WindSet() are duplicated 
 * in memory using mt_ObjcDup() and the memory automatically released when
 * the window is destroyed.
 *
 * @sa mt_ObjcFree()
 * @todo mt_ObjcDup() should duplicate icon and image bitmap.
 */
OBJECT *mt_ObjcDup( APPvar *app, OBJECT *src, WINDOW *win) {
	return mt_ObjcNDup( app, src, win, obj_nb( src));
}


/**
 * @brief Objects duplication.
 *
 * @param app application descriptor,
 * @param src objects tree to duplicate,
 * @param win window hosting the \e src object tree if \e src contains userdefs objects,
 * @param nb number of objects to copy,
 * @return a copy of the \e nb objects starting at \e src.
 * 
 * @sa mt_ObjcDup(), mt_ObjcNDupAtAddr()
 **/
OBJECT *mt_ObjcNDup( APPvar *app, OBJECT *src, WINDOW *win, int nb) {
	OBJECT *cpy;

	cpy = (OBJECT *)malloc(sizeof(OBJECT)*nb);
	if( !cpy )
		return NULL;
	
	return mt_ObjcNDupAtAddr( app, src, win, nb, cpy);
}


/** Objects duplication at a given address.
 *
 * @param app application descriptor,
 * @param src objects tree to duplicate,
 * @param win window hosting the \a src object tree if \a src contains userdefs objects,
 * @param nb number of objects to copy,
 * @param cpy address where objects tree will be duplicated
 * @return a copy of the \a nb objects starting at \a src.
 * 
 * @sa mt_ObjcDup(), mt_ObjcNDup()
 */

OBJECT *mt_ObjcNDupAtAddr( APPvar *app, OBJECT *src, WINDOW *win, int nb, OBJECT *cpy) {
	int i;
	
	/* step 1: duplicate the OBJECT structure */
	
	memcpy( cpy, src, sizeof(OBJECT)*nb);
	
	
	/* step 2 : duplicate the ob_spec stuff */
	
	for( i = 0; i < nb; i++) {
	
		if ( (cpy[i].ob_type & 0X00FF) == G_USERDEF 
		   &&( udlib_get_type(app,src,i) != 0 ) )
		{
			/* this is a userdefined object that has been transformed
			 * by udlib
			 */
			udlib_unextended(app,src,i,cpy);
			duplicate_obspec(app,cpy,i,NULL);
			udlib_extended(app,cpy,i,RSRC_XALL); /* TODO: is RSRC_XALL ok in all cases ? */
		} else {
			duplicate_obspec(app,cpy,i,win);
		}
	}
	
	return cpy;
}
