/*
 * XGEM.LDG: a userdef library for windom
 * Copyright (c) 2005 Arnaud Bercegeay
 *
 * some routines are based on Eric REBOUX's XGEM library
 * Copyright (c)1997-2003 Eric Reboux
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
 * $Source: /cvsroot/windom/windom/userdef/xgem/extend.c,v $
 * WinDom header file
 *  
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2005/11/03 22:08:58 $
 *   $Revision: 1.2 $
 */

#include <stdlib.h>
#include <string.h>

#include "global.h"


/*
 *  re-align border of 3D object so that the 3D effect is always
 *  within the AES clip area of the object.
 *
 *  mode  = 0 : restore original alignment
 *  mode != 0 : set re-alignment.
 *
 *  TODO: we should take care of OUTLINED and SHADOW attributes too...
 */
 
static void 
_xobjc_align_obj3d( OBJECT *tree, int idx, int mode) {
    int border, e;

    /* notation: border < 0 : outside border */
    /*           border > 0 : inside  border */

    switch (tree[idx].ob_type & 0xFF)
    {
        case G_BOXTEXT :
            border = tree[idx].ob_spec.tedinfo->te_thickness & 0x00FF;
            if (border >= 128) border = border - 256 ;
            break;
        case G_BOX :
        case G_IBOX:
        case G_BOXCHAR:
            border = tree[idx].ob_spec.obspec.framesize;
            break;
#if 0 /* test */
		case G_BUTTON:
			border = -1;
			break;
#endif
        default:  /* TODO: what kind of ob_type need the following ? */
            border = -1;
            if( tree[idx].ob_flags & EXIT) border --;
            if( tree[idx].ob_flags & DEFAULT) border --;
    }

	/*  Object with inside border don't need to be re-aligned 
	 *  (see hereafter)
	 */
    if (border>=0) return;
    
	/*  Here, the object has outside border.
	 *  The object dimensions and position will be changed so
	 *  that the border is within the object area.
	 *  => ob_x/y/w/h will be modified
	 *  => ob_x/y of the children will be modified to keep the
	 *     same absolute coordinates
	 *
	 *  WARNING: the field "border" remains negative. Userdef
	 *  subroutines have to take care of that, and consider 
	 *  a negative border as an "inside" border (just as for
	 *  positive border).
	 */
	 
    if( mode == 0) mode =  1;
    else           mode = -1;

    tree[idx].ob_x -= (border*mode);
    tree[idx].ob_y -= (border*mode);
    tree[idx].ob_width  += ((border<<1)*mode);
    tree[idx].ob_height += ((border<<1)*mode);

    for (e=tree[idx].ob_head; (e>=0) && (e!=idx); e=tree[e].ob_next) {
        tree[e].ob_x += (border*mode);
        tree[e].ob_y += (border*mode);
    }
}

/*
 *  @retval 0 if success, any other value in case of failure
 */

static int 
_xobjc_init_type( OBJECT *tree, int idx, void *func, short extype, short old_flag, APPENV *env)
{
    USERBLK *user;

    user = (USERBLK *) APP_MALLOC( sizeof(USERBLK));
	if (user==0L) return -1;
	
    user -> ub_parm = (long)APP_MALLOC( sizeof(W_PARM));
	if (user -> ub_parm == 0L) {
		APP_FREE(user);
		return -1;
	}
	
    user -> ub_code = func;
    ((W_PARM*)user -> ub_parm) -> wp_type   = tree[idx].ob_type;
    ((W_PARM*)user -> ub_parm) -> wp_oldf   = old_flag; /* save 3D flags */
    ((W_PARM*)user -> ub_parm) -> wp_spec   = tree[idx].ob_spec.index;
    ((W_PARM*)user -> ub_parm) -> wp_uldata = env->udlib_data;
    tree[idx].ob_spec.userblk = user;
    tree[idx].ob_type = G_USERDEF | (extype<<8);
	
	/* remove the 3D flag to prevent the AES to add dirty 3D effects */
 	tree[idx].ob_flags &= ~FL3DMASK;
	
	return 0;
}

static void 
_xobjc_free_type( OBJECT *tree, int idx, APPENV *env) 
{
    long addr;
    short type;
	short oldf;

    type = ((W_PARM*)tree[idx].ob_spec.userblk->ub_parm)->wp_type;
    addr = ((W_PARM*)tree[idx].ob_spec.userblk->ub_parm)->wp_spec;
	oldf = ((W_PARM*)tree[idx].ob_spec.userblk->ub_parm)->wp_oldf;
	
    APP_FREE( (void*)tree[idx].ob_spec.userblk->ub_parm);
    APP_FREE( tree[idx].ob_spec.userblk);
	
    tree[idx].ob_spec.index = addr;
    tree[idx].ob_type = type;
	
	/* remove 3D flags */
	tree[idx].ob_flags &= ~FL3DMASK;
	/* and restore old 3D flags */
	tree[idx].ob_flags |= (oldf & FL3DMASK);
}


#ifdef MODIF_XEDIT

/**  transform an editable object to XEDIT
 *
 *  @param tree
 *  @param idx
 *  @param env
 *  @return
 *
 */

static int 
_xobjc_init_xedit( OBJECT *tree, int idx, APPENV *env) 
{
    USERBLK *user;
	W_XEDIT* xedit;
	
    user = (USERBLK *) APP_MALLOC( sizeof(USERBLK));
	if (user==0L) return -1;
	
    xedit = (W_XEDIT *) APP_MALLOC( sizeof(W_XEDIT));
	if (xedit == 0L) {
		APP_FREE(user);
		return -1;
	}

	user->ub_parm = (long)xedit;

    user -> ub_code = ub_editable;
	xedit -> size = _xobjc_realloc_xedit( 0, strlen(tree[idx].ob_spec.tedinfo->te_ptext)+1);
	xedit -> line = (char *) APP_MALLOC( xedit->size * sizeof(char));
	if (xedit -> line == 0L) {
		APP_FREE(user);
		APP_FREE(xedit);
		return -1;
	}
	strcpy( xedit -> line, tree[idx].ob_spec.tedinfo->te_ptext);
	xedit -> tedinfo = tree[idx].ob_spec.tedinfo;
	xedit -> pos = 0;
	xedit -> curs = xedit -> blcbeg = 0;
	xedit -> flags = 0;
	xedit -> wp_type = tree[idx].ob_type;
	xedit -> wp_uldata = env->udlib_data;
	tree[idx].ob_spec.userblk = user;
	tree[idx].ob_type = G_USERDEF | (XEDIT<<8);
	
	return 0;
}


/**  transform an XEDIT object back to a standard editable object
 *
 *  @param tree
 *  @param idx
 *  @param env
 *
 */

static void 
_xobjc_free_xedit( OBJECT *tree, int idx, APPENV *env) 
{
	W_XEDIT *xedit;
	void *user;
	
	xedit = (W_XEDIT *)tree[idx].ob_spec.userblk->ub_parm;
	user = tree[idx].ob_spec.userblk;
	tree[idx].ob_spec.tedinfo = xedit->tedinfo;
	tree[idx].ob_type = xedit->wp_type;
	/* TODO : copy xedit->line to tree[idx].ob_spec.tedinfo 
	 * warning: take care of the max len in tedinfo !!!
	 */
	APP_FREE( xedit->line);
	APP_FREE( xedit);
	APP_FREE( user);
}

#endif

#define TRANSFORM_3DOBJECT(func,extype) 								\
	_xobjc_align_obj3d( tree, ob, 1); 									\
	if (_xobjc_init_type( tree, ob, func, extype, old_flags, env)) { 	\
		_xobjc_align_obj3d( tree, ob, 0);						 		\
		return 0;												 		\
	}															 		\
	return 1;

#define TRANSFORM_OBJECT(func,extype) 									\
	if (_xobjc_init_type( tree, ob, func, extype, old_flags, env))  	\
		return 0;												 		\
	return 1;

long 
_xobjc_extended( OBJECT *tree, long ob, long mode, APPENV *env) 
{
    short type, extype, old_flags;

	type = tree[ob].ob_type & 0x00FF;
	extype = tree[ob].ob_type >> 8;
	old_flags = tree[ob].ob_flags;
	
	if( type == G_USERDEF)
		return 0;
	
	if (mode & RSRC_XTYPE) {
		switch( extype) {
			case DCRBUTTON: 	TRANSFORM_3DOBJECT(ub_button,extype)
		    case SLIDEPART:  	TRANSFORM_3DOBJECT(ub_boxchar3d,extype)
		    case XBOXLONGTEXT:	TRANSFORM_3DOBJECT(ub_multiligne,extype)
		    case XTEDINFO:   	TRANSFORM_3DOBJECT(ub_boxtext3d,extype)
		    case TITLEBOX:   	TRANSFORM_OBJECT(ub_boxtitle,extype)
		    case UNDERLINE:  	TRANSFORM_OBJECT(ub_ulinetext,extype)
		    case CIRCLEBUT:  	TRANSFORM_OBJECT(ub_boxcircle,extype)
		    case ONGLET:	 	TRANSFORM_3DOBJECT(ub_onglet,extype)
		    case KPOPUPSTRG: 	TRANSFORM_OBJECT(ub_popuptext,extype)
		    case MENUTITLE:  	TRANSFORM_OBJECT(ub_mntitle,extype)
		    case DIALMOVER:  	TRANSFORM_OBJECT(ub_dialmover,extype)
#ifdef MODIF_XEDIT
			case XEDIT:
				if (_xobjc_init_xedit( tree, ob, env))
					return 0;
				return 1;
#endif
		}
	}
	
	if ( (mode & RSRC_X3D) /* && (extype == 0) */ &&
	     ((tree[ob].ob_state & DRAW3D) || (tree[ob].ob_flags & FL3DMASK)) )
	{
		tree[ob].ob_state |= DRAW3D;  /* force flag DRAW3D */
 		switch( type) {
 			case G_BUTTON:	TRANSFORM_3DOBJECT(ub_but3d,XFREESTR)
 			case G_BOX:		TRANSFORM_3DOBJECT(ub_box3d,XBFOBSPEC)
 			case G_IBOX:	TRANSFORM_3DOBJECT(ub_ibox3d,XBFOBSPEC)
 			case G_BOXCHAR:	TRANSFORM_3DOBJECT(ub_boxchar3d,XBFOBSPEC)
 			case G_BOXTEXT:	
				/* a G_BOXTEXT object should not be EDITABLE (the FBOXTEXT
				 * object is there for that purpose).
				 * Anyway, to be sure...
				 */
				if (tree[ob].ob_flags & EDITABLE)
					return 0;
				TRANSFORM_3DOBJECT(ub_boxtext3d,XTEDINFO)
	 	}
	}
	
	if (ob == 0 && tree->ob_type == G_BOX) {
		TRANSFORM_OBJECT(ub_dialmover,0)
	}
	
	return 0;
}

void 
_xobjc_extfree(  OBJECT *tree, long ob, APPENV *env) 
{
    switch( tree[ob].ob_type >> 8 )
    {
        case DCRBUTTON:
        case XFREESTR:
        case XTEDINFO:
        case XBFOBSPEC:
        case SLIDEPART:
        case ONGLET:
           _xobjc_free_type( tree, ob, env);
           _xobjc_align_obj3d( tree, ob, 0);
		   break;
        case TITLEBOX:
        case UNDERLINE:
        case CIRCLEBUT:
        case KPOPUPSTRG:
        case MENUTITLE:
        case DIALMOVER:
            _xobjc_free_type( tree, ob, env);
            break;
#ifdef MODIF_XEDIT
		case XEDIT:
			_xobjc_free_xedit( tree, ob, env);
			break;
#endif
		default:
            _xobjc_free_type( tree, ob, env);	
            _xobjc_align_obj3d( tree, ob, 0);
    }
}

static void 
_xobjc_unextended_type( OBJECT *tree, int idx, APPENV *env) 
{
    long addr;
    short type;
	short oldf;

    type = ((W_PARM*)tree[idx].ob_spec.userblk->ub_parm)->wp_type;
    addr = ((W_PARM*)tree[idx].ob_spec.userblk->ub_parm)->wp_spec;
	oldf = ((W_PARM*)tree[idx].ob_spec.userblk->ub_parm)->wp_oldf;

    tree[idx].ob_spec.index = addr;
    tree[idx].ob_type = type;
	
	/* remove 3D flags */
	tree[idx].ob_flags &= ~FL3DMASK;
	/* and restore old 3D flags */
	tree[idx].ob_flags |= (oldf & FL3DMASK);
}

#ifdef MODIF_XEDIT
static void 
_xobjc_unextended_xedit( OBJECT *tree, int idx, APPENV *env) 
{
	W_XEDIT *xedit;
	void *user;
	
	xedit = (W_XEDIT *)tree[idx].ob_spec.userblk->ub_parm;
	user = tree[idx].ob_spec.userblk;
	tree[idx].ob_spec.tedinfo = xedit->tedinfo;
	tree[idx].ob_type = xedit->wp_type;
	/* TODO : copy xedit->line to tree[idx].ob_spec.tedinfo 
	 * warning: take care of the max len in tedinfo !!!
	 */
}
#endif

long 
_xobjc_get_unextended( OBJECT *tree, int idx, APPENV *env)
{
    switch( tree[idx].ob_type >> 8 )
    {
        case DCRBUTTON:
        case XFREESTR:
        case XTEDINFO:
        case XBFOBSPEC:
        case SLIDEPART:
        case ONGLET:
            _xobjc_unextended_type( tree, idx, env);
            _xobjc_align_obj3d( tree, idx, 0);
			break;
        case TITLEBOX:
        case UNDERLINE:
        case CIRCLEBUT:
        case KPOPUPSTRG:
        case MENUTITLE:
        case DIALMOVER:
            _xobjc_unextended_type( tree, idx, env);
            break;
#ifdef MODIF_XEDIT
		case XEDIT:
			_xobjc_unextended_xedit( tree, idx, env);
			break;
#endif
		default:
            _xobjc_unextended_type( tree, idx, env);	
            _xobjc_align_obj3d( tree, idx, 0);
    }
	
	return 1L;
}

