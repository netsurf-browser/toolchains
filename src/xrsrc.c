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
 * $Source: /cvsroot/windom/windom/src/xrsrc.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:35 $
 *   $Revision: 1.8 $
 */


#include <stddef.h>
#include <stdlib.h>

#include "globals.h"

void objc_extended( APPvar *app, int mode, OBJECT *tree) {
	int idx;
	
	for( idx=0; ; idx++) {
		
		if ( (tree[idx].ob_type & 0x00FF) != G_USERDEF)
			udlib_extended( app, tree, idx, mode);
		
		if( tree[idx].ob_flags & LASTOB) break;
    } 
}

void objc_extfree( APPvar *app, OBJECT *tree) {
    int idx = 0;

    for( idx=0; ; idx++) {
	
		if ((tree[idx].ob_type & 0x00FF) == G_USERDEF)
			udlib_extfree( app, tree, idx);
		
        if( tree[idx].ob_flags & LASTOB) break;
    }
}

/**
 * @brief Install/remove WinDom extended objects.
 *
 * @param app application descriptor,
 * @param mode possible values are :
 *   - \b RSRC_XTYPE : install extended objects (for objects having an extended type),
 *   - \b RSRC_X3D : install extended object for all objects,
 *   - \b RSRC_XALL : \b RSRC_XTYPE + \b RSRC_X3D,
 *   - 0 :  uninstall extended objects.
 * @param trindex address of object tree array or \b NULL,
 * @param num_tree number of object tree in \e trindex.
 *
 * mt_RsrcXtype() installs or removes WinDom extended objects. It can work
 * on internal resource, loaded by mt_RsrcLoad() or mt_RsrcXload(), 
 * or on external resource, included in the C-source during the compilation.
 * 
 * An WinDom extended object is an object having a special value in
 * \e ob_type field of the OBJECT structure. The low byte of \e ob_type is
 * used by AES to identify the type of object (button, box, text, ...). The
 * high byte is not read by AES. WinDom uses it to specify new objects.
 * mt_RsrcXtype() transform theses objects into a G_USERDEF
 * objects or insersly can restore objects previously transformed.
 *
 * The \e mode parameter describes the action to perform. Mode 
 * \b RSRC_XTYPE installs extended type for objects having an extended type. 
 * There is a second mode, \b RSRC_X3D, which installs new object
 * types with a 3D look for all objects. The goal of this mode is to allow 
 * your dialog boxes to have the same aspect with MagiC, Naes, TOS or 
 * any other GEM system. Addressed objects are buttons and boxes (!U)without
 * extended types(!u). And off 
 * course, these two modes can be cumulated (mode \b RSRC_XALL).
 *
 * To fix the objects in the internal resource, the
 * \e trindex parameter must set to \b NULL and the \e trindex
 * parameter is not used. To fix an external resource, the \e trindex
 * parameter should be filled with the address of object trees in the
 * resource. This address is supplied with the RSH file created by
 * your resource editor. The RSH file must be include in your source
 * code with an \c \#include directive. The \e num_tree value is also supplied 
 * with the RSH file.
 *
 * When the application finishes, extended objects should be freed by
 * a call to RsrcXtype() with mode 0. 
 */
void mt_RsrcXtype( APPvar *app, int mode, OBJECT **trindex, int num_tree) {
    OBJECT *tree;
    int dum, num;

	/* fix extended objects (including 3D objects) */
	if (mode) {
    	num = (trindex == NULL)?app->ntree:num_tree;
	    for( dum = 0; dum < num; dum ++) {
    	    if( trindex == NULL)  mt_rsrc_gaddr( 0, dum, &tree, app->aes_global);
	        else                  tree = trindex[dum];
            objc_extended( app, mode, tree);
		}
	}
	
	/* update xrsrc_init */
	app->priv->xrsrc_init |= mode;
	
	/* now, treat "free" request */
	if (mode == 0) {
		
		/* free extended objects */
   		num = (trindex == NULL)?app->ntree:num_tree;
    	for( dum = 0; dum < num; dum ++) {
        	if( trindex == NULL) mt_rsrc_gaddr( 0, dum, &tree, app->aes_global);
        	else                 tree = trindex[dum];
            objc_extfree( app, tree);
		}
		
		/* reset xrsrc_init flags */
		app->priv->xrsrc_init = 0;
    }
}

/* EOF */



