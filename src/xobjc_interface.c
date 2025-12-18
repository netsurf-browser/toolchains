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
 * $Source: /cvsroot/windom/windom/userdef/extended/interface.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/06/20 20:00:16 $
 *   $Revision: 1.9 $
 */

#include <stdlib.h>
#include <string.h>
#include "xobjc_global.h"

#include "wsystem.rh"
#include "wsystem.rsh"

/**  initialisation of the library
 *
 *  @param env the context of this application provided by windom
 *
 *  @retval <0 any negative value in case of error,
 *  @retval 1 if this library uses the "SIMPLE" interface (not yet specified -- don't use !)
 *  @retval 2 if this library uses the "FREEDOM" interface
 *
 *   This function is called by windom when a new AES application may use
 *   this userdef library.
 *
 *   The env data provides a user data field (udlib_data) that the library may use for its
 *   own needs. This data may be use to store the configuration of the library for
 *   example.
 *   This env data will be provided to the library in each function of the interface, so that
 *   the private data stored in udlib_data will be available in all interface function.
 *
 *   this function is mandatory.
 */

long __CDECL 
xobjc_libinit( APPENV *env)
{
	XOBJC_DATA *ulib_data;
	int ind;
	
	static const ATTRIB sysfont_13_black = {SYSFONT, 13, BLACK};
	static const ATTRIB sysfont_13_defval = {SYSFONT, 13, DEFVAL};
	static const ATTRIB sysfont_10_defval = {SYSFONT, 10, DEFVAL};
	
	ulib_data = (XOBJC_DATA*) APP_MALLOC (sizeof(XOBJC_DATA));
	if (ulib_data == NULL) return -1;
	
	ulib_data->config = (UDLIB_CONFIG*) APP_MALLOC (sizeof(UDLIB_CONFIG));
	if (ulib_data->config == NULL) { APP_FREE(ulib_data); return -1;}

	ulib_data->rs_trindex = rs_trindex[USERDEF];
	ulib_data->draw_cicon = env->fixcicon.draw_cicon;
	ulib_data->vdih = env->vdih;
	ulib_data->nplanes = env->nplanes;
	ulib_data->has_fsmgdos = (vq_vgdos() == _FSM);
	
	CONF(ulib_data)->key_color			= BLACK;
	CONF(ulib_data)->string 			= sysfont_13_black;
	CONF(ulib_data)->button 			= sysfont_13_black;
	CONF(ulib_data)->exit				= sysfont_13_black;
	CONF(ulib_data)->title				= sysfont_13_black;
	CONF(ulib_data)->xedit_text 		= sysfont_13_defval;
	CONF(ulib_data)->xedit_label		= sysfont_13_defval;
	CONF(ulib_data)->xedit_curs_color	= BLACK;
	CONF(ulib_data)->xlongedit			= sysfont_10_defval;
	CONF(ulib_data)->xlgedt_smlfnt		= 8;
	CONF(ulib_data)->xtedinfo			= sysfont_10_defval;
	CONF(ulib_data)->xtdinf_smlfnt		= 8;
	CONF(ulib_data)->actmono			= 0;
	CONF(ulib_data)->actcol 			= LWHITE;
	
	env->udlib_data = ulib_data;
	
	/* ask windom to convert the following OBJECT to userdefined CICON */
	for( ind=0; ind<NUM_OBS; mt_rsrc_obfix( rs_object, ind++, env->aes_global));
	env->fixcicon.rs_object   = rs_object;
	env->fixcicon.num_obs     = NUM_OBS;
	env->fixcicon.num_cib     = NUM_CIB;
	env->fixcicon.rgb_palette = rgb_palette;
	
	return 2L;
}


/**  this function update the configuration
 *
 *  @param config the configuration of windom that contains options relative
 *         to userdef objects.
 *  @param env the context of this application provided by windom
 *
 *  This function is called when the library has just been initialised, and
 *  when the windom configuration has changed for this application.
 *
 *  The config data contain all the option of windom that define the aspect of
 *  userdefined objects. 
 *
 *  The library may use these parameters (or some of them) to adjust the
 *  rendering the userdef objects.
 *
 *  This function is optional
 *
 */

void __CDECL
xobjc_udconfig( UDLIB_CONFIG *config, APPENV *env)
{
	size_t size;
	
	size = config->size_of_this;
	if (size > sizeof(UDLIB_CONFIG)) 
		size = sizeof(UDLIB_CONFIG);
	
	memcpy( ((XOBJC_DATA*)(env->udlib_data))->config, config, size);	
}


/**  exit the library
 *
 *  @param env the context of this application provided by windom
 *
 *   this function is called by windom when this AES application no more use
 *   and no more required this userdef library (application exiting, or change
 *   of userdef-lib on the fly).
 *
 *   This function shall free all the data allocated by this library.
 *
 *   This function is optional
 *
 */

void __CDECL 
xobjc_libexit( APPENV *env)
{
	APP_FREE(((XOBJC_DATA*)(env->udlib_data))->config);
	APP_FREE(env->udlib_data);
}


/**  request for transformation of userdef objects
 *
 *  @param tree the address of the formular
 *  @param ob the object of the formular
 *  @param type kind of transformation requested by the user (see mt_RsrcXtype())
 *  @param env the context of this application provided by windom
 *
 *  @retval 1L if the library has transformed this object to a userdef object
 *  @retval 0L if the library has not changed this object
 *  @retval -1L if an error has occured.
 *
 *  this function is called by windom when the application requested for extended type
 *  (see mt_RsrcXtype()).
 *
 *  If tree[ob] should be transformed to a userdefined object, then the library
 *  have to do the transformation: change the ob_type of this object to G_USERDEF,
 *  and maybe change some other stuff (ob_spec, ob_x, ob_y, ob_flags...).
 *  Nota: the extended ob_type (ob_type & 0xFF00) is free - the library may use
 *  this extended ob_type to store informations about the object.
 *  Nota: Remember that the transformation OBJECT->userdef must be reversible. So
 *  the library must be able to restore all the modified data (original ob_type
 *  and ob_spec for example).
 *
 *  The VDI handle given in env->vdih is the handle of the virtual workstation opened
 *  by the windom application that must be used by the userdef subroutine to draw
 *  on the screen.
 *
 *  The APPENV parameter may be stored in the userblk structure, if the userdef
 *  subroutine needs to access to malloc function, or to the udlib_data pointer.
 *  this structure (APPENV) will be allocated by windom before calling xobjc_libinit(),
 *  and free-ed after xobjc_libexit(), and the same ENVAPP pointer will be given to
 *  all the xobjc_xxx functions.
 *
 *  this function is mandatory
 */

long __CDECL 
xobjc_extended( OBJECT *tree, long ob, long type, APPENV *env)
{
	return _xobjc_extended(tree,ob,type,env);
}


/**  set a userdef object back to its original aspect.
 *
 *  @param tree the address of the formular
 *  @param object the object of the formular
 *  @param env the context of this application provided by windom
 *
 *  tree[ob] has been transformed to a userdef object
 *  by this library. The userlib doesn't have to check this.
 *
 *  this function is mandatory
 */

void __CDECL 
xobjc_extfree( OBJECT *tree, long object, APPENV *env)
{
	_xobjc_extfree(tree,object,env);
}


/**  read/set the string pointer of a userdefined object.
 *  
 *  @param tree the address of the formular
 *  @param object the object of the formular
 *  @param string a pointer to the new string or NULL
 *  @param env the context of this application provided by windom
 *  
 *  @return the pointer to the text, or NULL if this object doesn't have any
 *          text field.
 *
 *  If "string" is NULL, this function shall return a pointer to the text of
 *  the userdef object.
 *
 *  If "string" is not NULL, this value will be the new value of the text pointer
 *  of this userdef object.
 *  Nota: the pointer shall be changed. A simple strcpy() is not ok !
 *  
 *  For SELECTABLE objects, if the string contains the '[' character, then the following
 *  character will be used as shortcut to select this object. When the user presses
 *  a key, windom will use this function to find if the key pressed is a shortcut
 *  to select this object.
 *
 *  tree[ob] has been transformed to userdef object
 *  by this library. The userlib doesn't have to check this.
 *
 *  this function is optionnal, but highly recommanded if any of the userdefined
 *  object transformed by this library may contain some text.
 *
 */

char *__CDECL 
xobjc_string( OBJECT *tree, long object, char *string, APPENV *env)
{
	return _xobjc_string(tree,object,string,env);
}


/**  read/set the character of a userdefined object.
 *  
 *  @param tree the address of the formular
 *  @param object the object of the formular
 *  @param the_char a character (from 0 to 255) or -1L
 *  @param env the context of this application provided by windom
 *  
 *  @return the character of this object, or -1L if this object doesn't have any
 *          character field.
 *
 *  If "the_char" equals -1L, this function shall return the character stored in this
 *  object (value from 0 to 255).
 *
 *  If "the_char" is a valid character (value from 0 to 255), this value will be
 *  the new value of the character of this userdef object.
 *
 *  this function is optionnal, but highly recommanded if any of the userdefined
 *  object transformed by this library may contain character.
 *
 */

long __CDECL 
xobjc_char( OBJECT *tree, long object, long the_char, APPENV *env)
{
	return _xobjc_char( tree, object, the_char, env);
}


/**  similar to objc_edit(tree, ob, the_char, index, kind)
 *
 *  @param tree the address of the formular
 *  @param object the object of the formular
 *  @param the_char a character (from 0 to 255)
 *  @param ob
 *  @param kind
 *  @param nclip
 *  @param gclip
 *  @param env the context of this application provided by windom
 *
 *  @return as objc_edit().
 *
 *  similar to objc_edit for userdefined object with the EDITABLE flag set.
 *
 *  gclip is a array of GRECT. If this function has to draw something to the screen
 *  ( a cursor for example), then the list of rectangles given in gclip shall be
 *  used as clipping area. nclip is the number of element in the gclip array.
 *
 *  This function is mandatory if the library may create an editable userdefined object.
 *
 */

long __CDECL 
xobjc_edit( OBJECT *tree, long object, long the_char, short* ob, long kind,
            long nclip, GRECT *gclip, APPENV *env)
{
	return _xobjc_edit( tree, object, the_char, ob, kind, nclip, gclip, env);
}


/**  return the character of the editable object at (mouse) position x,y.
 *
 *  @param tree the address of the formular
 *  @param object the object of the formular
 *  @param x x-position of the mouse, relative to the left border of the object
 *  @param y y-position of the mouse, relative to the top border of the object
 *  @param env the context of this application provided by windom
 *
 *  @return the index of the character in the string, or a negative value
 *          in case of error ( x or y point outside of the object...)
 *
 *  this function is used when the user clic on such object. Then windom
 *  set the cursor just under the mouse position using xobjc_edit().
 *
 *  This function is optional.
 */

long __CDECL 
xobjc_get_edpos( OBJECT *tree, long object, long x, long y, APPENV *env)
{
	return _xobjc_get_edpos( tree, object, x, y, env);
}


/**   utility for ObjcDup()
 *
 *  @param tree the address of the formular
 *  @param object the object of the formular
 *  @param env the context of this application provided by windom
 *
 *   tree[ob] is a userdef object that has been transformed by this library.
 *   org is pointer to another OBJECT structure. the org object contain a
 *   copy of the userdefined object (*org == tree[ob]).
 *
 *   this function will copy in the org structure the original aspect of
 *   tree[ob]. In other words, org will be tree[ob] after xobjc_extfree(),
 *   but without freeing the userdef tree[ob].
 *
 *   This function is mandatory
 */

long __CDECL 
xobjc_get_unextended( OBJECT *tree, long object, APPENV *env)
{
	return _xobjc_get_unextended( tree, object, env);
}

