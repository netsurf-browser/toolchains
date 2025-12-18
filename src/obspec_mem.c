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
 * $Source: /cvsroot/windom/windom/src/obspec_mem.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */


#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/*  When objects are duplicated by ObjcDup(), some extra malloc could
 *  be done to duplicate the ob_spec stuff of this object.
 *
 *  we must keep a way to memorise the adress returned by malloc for the
 *  ob_spec stuff, so that we could free this memory later (by ObjcFree()).
 *
 *  Warning: the field ob_spec can be changed by the application. For example
 *  ObjcString may modify the pointer ob_spec. When the function ObjcFree() is
 *  called for such object, a free(ob->ob_spec) may crash !
 *
 *  So, we build a list of malloc() performed by ObjcDup for the ob_spec field.
 *  This way, when a duplicated object is freed, we can find easily the adress
 *  that has been returned by malloc() for the ob_spec field of this object.
 */
 
void * __malloc_obspec( APPvar *app, OBJECT * p_ob, long size)
{
	struct ty_obspec_dup  * obspec_dup;
	void * p_mem;
	
	if (! size) return NULL;
	
	p_mem = malloc (size);
	if (! p_mem) return NULL;
	
	obspec_dup = (struct ty_obspec_dup *) malloc (sizeof (struct ty_obspec_dup));
	if (! obspec_dup) {
		free (p_mem);
		return NULL;
	}
	
	obspec_dup->p_ob  = p_ob;
	obspec_dup->p_mem = p_mem;
	obspec_dup->next  = app->priv->__list_obspec_dup;
	app->priv->__list_obspec_dup = obspec_dup;
	
	return p_mem;
}

void __free_obspec( APPvar *app, OBJECT * p_ob)
{
	struct ty_obspec_dup  *here, *prev;
	
	here = app->priv->__list_obspec_dup;
	prev = NULL;
	
	while ((here) && (here->p_ob != p_ob)) {
		prev = here;
		here = here->next;
	}
	
	if (here) {
		free (here->p_mem);
		if (prev) {
			prev->next = here->next;
		} else {
			app->priv->__list_obspec_dup = here->next;
		}
		free (here);
	}
}


void __freeall_obspec( APPvar *app)
{
	struct ty_obspec_dup  *next;
	struct ty_obspec_dup  *trash = app->priv->__list_obspec_dup;
	
	while (trash) {
		next = trash->next;
		free(trash);
		trash = next;
	}
	
	app->priv->__list_obspec_dup = NULL;
}
