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
 * $Source: /cvsroot/windom/windom/src/rsrc_gaddr.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */


#include <stddef.h>
#include <stdlib.h>
#include "globals.h"


/**
 * @brief retrieve an address object from a loaded resource file.
 * 
 * @param app application descriptor,
 * @param _rsc resource descriptor or NULL
 * @param type type of object to retrieval. Possible values are:
 *  - \b R_TREE object tree (pointer to array of OBJECT)
 *  - \b R_OBJECT individual object (pointer to OBJECT)
 *  - \b R_TEDINFO TEDINFO structure
 *  - \b R_ICONBLK ICONBLK structure
 *  - \b R_BITBLK BITBLK structure
 *  - \b R_STRING, \b R_FRSTR free string
 *  - \b R_IMAGEDATA, \b FRIMG  free image data
 *  - \b R_OBSPEC ob_spec field within OBJECTs
 *  - \b R_TEPTEXT te_ptext field within TEDINFOs
 *  - \b R_TEPTMPLT te_ptmplt field within TEDINFOs
 *  - \b R_TEPVALID te_pvalid field within TEDINFOs
 *  - \b R_IBPMASK ib_pmask field within ICONBLKs
 *  - \b R_IBPDATA ib_pdata field within ICONBLKs
 *  - \b R_BIPDATA bi_pdata field within ICONBLKs
 * @param index object index to retrieval,
 * @param addr address of object,
 * @return 0 or negative code error.
 *
 * mt_RsrcGaddr() has the same action than mt_rsrc_gaddr() on resource loaded
 * by mt_RsrcXload(). Parameter \e _rsc is a value returned by mt_RsrcXload().
 * If parameter \e _rsc is \b NULL, mt_RsrcGaddr() retreivals
 * object from resource loaded by mt_RsrcLoad().
 *
 * @sa mt_RsrcXload(), mt_RsrcLoad().
 */

int mt_RsrcGaddr( APPvar *app, void *_rsc, int type, int index, void *addr) {
	RSC *rsc = _rsc;
	if( rsc) {
		switch( type) {
		case R_TREE:
			*(long*)addr = (long) (rsc->head.trindex[index]);
			break;
		case R_OBJECT:
			*(long*)addr = (long) (&rsc->head.object [index]);
			break;
		case R_TEDINFO:
			*(long*)addr = (long) (&rsc->head.tedinfo[index]);
			break;
		case R_ICONBLK:
			*(long*)addr = (long) (&rsc->head.iconblk[index]);
			break;
		case R_BITBLK:
			*(long*)addr = (long) (&rsc->head.bitblk[index]);
			break;
		case R_STRING:
		case R_FRSTR:
			*(long*)addr = (long) (rsc->head.frstr[index]);			
			break;
		case R_IMAGEDATA:
		case R_FRIMG:
			*(long*)addr = (long) (rsc->head.frimg[index]);
			break;
		case R_OBSPEC:
			*(long*)addr = (long) (&rsc->head.object[index].ob_spec);
			break;
		case R_TEPTEXT:
			*(long*)addr = (long) (rsc->head.tedinfo[index].te_ptext);
			break;
		case R_TEPTMPLT:
			*(long*)addr = (long) (rsc->head.tedinfo[index].te_ptmplt);
			break;
		case R_TEPVALID:
			*(long*)addr = (long) (rsc->head.tedinfo[index].te_pvalid);
			break;
		case R_IBPMASK:
			*(long*)addr = (long) (rsc->head.iconblk[index].ib_pmask);
			break;
		case R_IBPDATA:
			*(long*)addr = (long) (rsc->head.iconblk[index].ib_pdata);
			break;
		case R_IBPTEXT:
			*(long*)addr = (long) (rsc->head.iconblk[index].ib_ptext);
			break;
		case R_BIPDATA:
			*(long*)addr = (long) (rsc->head.bitblk[index].bi_pdata);
			break;
		default:
			return -1;
		}
		return 0;
	} else
		return mt_rsrc_gaddr( type, index, addr, app->aes_global);
}

