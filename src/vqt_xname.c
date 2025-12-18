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
 * $Source: /cvsroot/windom/windom/src/vqt_xname.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/04/18 18:50:26 $
 *   $Revision: 1.10 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"

/*
 *	Appel custom des fonctions vqt_extname, vqt_xfntinfo,
 *	vqt_fontheader.
 *	name: nom de la fonte (pas forc‚ment sur 31 octets)
 *	flags: FNT_OUTLINE, fonte vectorielle (ou bitmap sinon)
 *		   FNT_MONO,	fonte non proportionnelle (ou proportionnelle sinon)
 *		   FNT_SPEEDO,  fonte vectorielle de type speedo
 *		   FNT_TRUETYPE,fonte vectorielle de type true type
 *		   FNT_TYPE1,	fonte vectorielle de type postscript type 1
 *	_index: index fonte
 *	retour: id de la fonte
 */

static int vqt_xname( APPvar *app, int id, int _index, char *name, int *flags) {
	int speedo;
	INT16 nvdi_fmt, nvdi_flg;
	short handle = app->graf.handle;
	XFNT_INFO xfnt;
	INT16 cellw1, cellw2, dum;

	if( _index == 0) {
		for( _index = 0; _index < app->gdos; _index++) {
			if( id == vqt_name( handle, _index, name))
				break;
		}
		if( _index == app->gdos) return -1;	/* erreur, id non trouv‚e */
	} 
	
	id = vqt_extname( handle, _index, name, &speedo, &nvdi_fmt, &nvdi_flg);

	*flags = 0;
	/* fonte vectorielle ? */
	if( speedo == 1 /*SP_OUTLINE*/) 
		*flags |= FNT_OUTLINE;
	/* fonte monospaced ? */
	if( nvdi_flg == 1 /*NVDI_MONO*/)
		*flags |= FNT_MONO;
	if( nvdi_fmt & 0x2 /*NVDI_SPEEDO*/)
		*flags |= FNT_SPEEDO;
	if( nvdi_fmt & 0x4 /*NVDI_TRUETYPE*/)
		*flags |= FNT_TRUETYPE;
	if( nvdi_fmt & 0x8 /*NVDI_TYPE1*/)
		*flags |= FNT_TYPE1;

	if (speedo == 0)
		return id;
	
	if (vqt_xfntinfo( handle, 0x1 /*XFNT_NAME*/, id, 0, &xfnt) != 0 ) {
		/* success => the VDI supports this function */
		strcpy( name, xfnt.font_name);
		return id;
	}
	
	if (speedo == 1) {
		char buf [421];
		char path[128];
	
		vst_font( handle, id);
		*(buf+24) = '\0';
		vqt_fontheader( handle, buf, path);
		
		if (*(buf+24))  {
			/* call succeed */
			strcpy( name, buf+24);  /* long name under speedoGDOS */
			if (*(buf+263) & 0x2)   /* fonte monospaced ? */
				*flags |= FNT_MONO;
			*flags |= FNT_SPEEDO;
			
			return id;
		}
	}
	
	#define l_ASCII	0x6C
	#define W_ASCII	0x57

	vqt_width( handle, l_ASCII, &cellw1, &dum, &dum);
	vqt_width( handle, W_ASCII, &cellw2, &dum, &dum);
	if( cellw1 == cellw2) 
		*flags |= FNT_MONO;
	return id;
}

/** customized function that sumary VDI functions vqt_extname, vqt_xfntinfo and vqt_fontheader
 *
 *  @param app application descriptor
 *  @param id font-ID
 *  @param _index same as 2nd parameter of VDI functions vqt_name() or vqt_extname()
 *  @param name TODO
 *  @param fl is a bitmap variable wich is a OR of the following flags:
 *         - \b FNT_OUTLINE : vectorial font (bitmap font if unset)
 *         - \b FNT_MONO : monospaced font (proportional font if unset)
 *         - \b FNT_SPEEDO : vectorial font type is SPEEDO
 *         - \b FNT_TRUETYPE : vectorial font type is True Type
 *         - \b FNT_TYPE1 : vectorial font type is PostScript type 1
 *
 *  @return font ID or -1 in case of error
 *
 */

int mt_VqtXname( APPvar *app, int id, int _index, char *name, int *fl) {
	if( vq_gdos())
		return vqt_xname( app, id, _index, name, fl);
	else {
		/* recherche par id */
		if( _index == 0) {
			while( app->priv->listfontid[_index].id != id && _index < app->priv->maxfontid)
				_index ++;
		}
		if( _index < app->priv->maxfontid) {
			strcpy( name, app->priv->listfontid[_index].name);
			*fl = app->priv->listfontid[_index].flags;
			return _index?app->priv->listfontid[_index].id:_index;
		} else
			return -1;
	}
}


