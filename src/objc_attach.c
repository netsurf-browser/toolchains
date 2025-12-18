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
 * $Source: /cvsroot/windom/windom/src/objc_attach.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/04/12 20:53:56 $
 *   $Revision: 1.8 $
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"

/**
 * @brief Attach a variable or a callback function at an object.
 *
 * @param app application descriptor,
 * @param mode destination type
 *   - \b OC_FORM  if the object is in a window formular,
 *   - \b OC_TOOLBAR object is in a toolbar,
 *   - \b OC_MENU object is in a window menu,
 * @param win targeted window or \b NULL,
 * @param ob object index to attach,
 * @param type type of binding
 *   - \b BIND_VAR attach a variable,
 *   - \b BIND_BIT attach a specific bit of a variable,
 *   - \b BIND_FUNC attach a function,
 * @param ... variable argument list depending on value of 
 * \e type parameter. With \b BIND_VAR and \b BIND_BIT, argument
 * is a pointer to the (integer) variable to bind. \b BIND_BIT requires a
 * second parameter which is the bit to bind. \b BIND_VAR requires
 * a second parameter which is the value of the variable when the
 * object is selected. With \b BIND_FUNC, argument
 * is a pointer to the function and a second optional argument is
 * a pointer to a user data which will given to the function (see 
 * examples).
 * @return 0 or a negative code error.
 *
 * mt_ObjcAttach() attaches a function or a variable at an object from
 * a window formular, a toolbar or a menu. The rules are
 * differents if you use a formular and a toolbar or a menu.
 *
 * With formulars or toolbars,
 * only \b EXIT or \b TOUCHEXIT objects can be attached at a callback
 * function. When the user selects these objects, the callback function is
 * invoked. Only \b SELECTABLE objects can be binded to a variable
 * When the user selects these objects, the variable is
 * set to its "value_when_selected" value. Only \b SELECTABLE objects can
 * be binded to a bitset. When the user selects these objects, the bit
 * of the variable is set. If such object is unselected, then the bit
 * is cleared.
 * Please note that all these rules also appli for RADIO button (the
 * variable is no more filled with the index of the selected object).
 *
 * With menu, an item of the menu can be attached at a function or 
 * at a bitset. When an item is linked to a bitset, it is
 * checked or unchecked when the user selects it. The variable linked
 * is filled with 1 or 0 (or a specific bit with the \b BIND_BIT
 * mode) when the item is checked or unchecked. Notice desktop menu
 * is addressed if win \e parameter is set to \b NULL.
 *
 * \par Callback function prototype
 *
 * A function linked with to an object in a formular or a toolbar 
 * has the following interface: @code
  void __CDECL func ( APPvar *ap, WINDOW *win, int ob, int mode, void *data);
@endcode
 * where \e win is the host window, \e ob is the index
 * of the attached object and \e mode can be \b OC_FORM,
 * \b OC_TOOLBAR or \b OC_MENU. If an user data pointer
 * is specified with ObjcAttach(), this pointer can be read as
 * a fourth parameter of the binded function (\e data in our 
 * example).
 *
 * A function linked to a menu object has an additionnal parameter - 
 * \e title - which indicated the menu title index selected. This
 * parameter is required by mt_MenuTNormal():@code
  void __CDECL func ( APPvar *ap, WINDOW *win, int ob, int mode, int title, void *data);
@endcode
 * The fifth parameter \e data is an optional user pointer data 
 * specified by mt_ObjcAttach().
 *
 *
 * It's highly recommended to use specialized ObjcAttachXxx() functions instead of
 * this general function. They are:
 * - mt_ObjcAttachVar() similar to mt_ObjcAttach(...,BIND_VAR,...)
 * - mt_ObjcAttachBit() similar to mt_ObjcAttac(...,BIND_BIT,...)
 * - mt_ObjcAttachFormFunc() similar to mt_ObjcAttach(OC_FORM,...,BIND_FUNC,...)
 * - mt_ObjcAttachTBFunc() similar to mt_ObjcAttach(OC_TOOLBAR,...,BIND_FUNC,...)
 * - mt_ObjcAttachMenuFunc() similar to mt_ObjcAttach(OC_MENU,...,BIND_FUNC,...)
 *
 *
 * \par Examples
 * @code

static int radio = 1;
#define OPTION1	0x1	// bit 0
#define OPTION2 0x2	// bit 1
#define OPTION3 0x4     // bit 2
static int options = 0;

{
 // Before : create the form with FormCreate()
 // Then attach the objects
		
 // 3 radio buttons in a formular
 // the variable "radio" will contain either 1, 2 or 3
 // depending on the selected radio button
  ObjcAttachVar( OC_FORM, win, RAD1, &radio, 1);
  ObjcAttachVar( OC_FORM, win, RAD2, &radio, 2);
  ObjcAttachVar( OC_FORM, win, RAD3, &radio, 3);

  // some checkboxes ... 
  ObjcAttachBit( OC_FORM, win, BUT1, &options, OPTION1);
  ObjcAttachBit( OC_FORM, win, BUT2, &options, OPTION2);
  ObjcAttachBit( OC_FORM, win, BUT3, &options, OPTION3);

  // An example of function linked to an object
  // see after for the definition of the function
  ObjcAttachFormFunc( win, OK, ButOk, "a dummy data");
}

// Function linked to OK object
void ButOk( WINDOW *win, int ob, int mode, char *mydata) {
  // Unselect the object ... 
  ObjcChange( mode, win, ob, NORMAL, 0);
  puts( mydata); // Should print 'a dummy data' ...
  // ... and destroy the window
  ApplWrite( app.id, WM_DESTROY, win->handle);
}
@endcode
  */

int mt_ObjcAttach( APPvar *app, int mode, WINDOW *win, int ob, int type, ...) {
	BIND   *bind = NULL;
	W_FORM *wfrm = NULL;  /* to shut up gcc warning */
	OBJECT *tree = NULL;  /* idem */
	va_list arg;

	/* win == NULL -> desktop window */
	if (!win) win = app->priv->desk;
	
	/* first, take care of impossible cases */
	if (mode == OC_MENU && type == BIND_VAR)
		return EBADRQ;
	if (type != BIND_VAR && type != BIND_FUNC && type != BIND_BIT)
		return EBADRQ;
	if (mode != OC_MENU && mode != OC_TOOLBAR && mode != OC_FORM)
		return EBADRQ;
	
	va_start( arg, type);
	switch( mode) {
	case OC_MENU:
		if( win->menu.bind == NULL) {
			win->menu.bind = (BIND*)calloc( obj_nb(win->menu.root), sizeof(BIND));
			if (win->menu.bind == NULL) return ENSMEM;
			memset( win->menu.bind, 0, obj_nb(win->menu.root) * sizeof(BIND));
		}
		bind = win->menu.bind;
		tree = win->menu.root;
		break;
	case OC_TOOLBAR:
		if( win->tool.bind == NULL) {
			win->tool.bind = (BIND*)calloc( win->tool.nb_ob, sizeof(BIND));
			if (win->tool.bind == NULL) return ENSMEM;
			memset( win->tool.bind, 0, win->tool.nb_ob * sizeof(BIND));
		}
		bind = win->tool.bind;
		tree = win->tool.root;
		break;
	case OC_FORM:
		if( (wfrm = mt_DataSearch( app, win, WD_WFRM)) != NULL) {
			if( wfrm->bind == NULL) {
				wfrm->bind = (BIND*)calloc( wfrm->nb_ob, sizeof(BIND));
				if (wfrm->bind == NULL) return ENSMEM;
				memset( wfrm->bind, 0, wfrm->nb_ob * sizeof(BIND));
			}
			bind = wfrm->bind;
			tree = wfrm->root;
		}
		break;
	default:
		return EBADRQ;
	}
	
	if( bind) {
	
		bind[ob].type = type;
		
		switch( type) {
		
		case BIND_VAR:
			bind[ob].desc.var.var   = (int*) va_arg( arg, int*);
			bind[ob].desc.var.value = (int ) va_arg( arg, int );
			/* update the SELECTED flag of ob_state */
			SET_BIT(tree[ob].ob_state, SELECTED, ( *(bind[ob].desc.var.var) == bind[ob].desc.var.value) ? 1 : 0);
			break;
			
		case BIND_BIT:
			bind[ob].desc.bit.var = (int*) va_arg( arg, int*);
			bind[ob].desc.bit.bit = (int ) va_arg( arg, int );
			/* now, it's time to update the object state */
			if (mode == OC_MENU)
				mt_MenuIcheck( app, win, ob, ( *(bind[ob].desc.bit.var) & bind[ob].desc.bit.bit ) ? 1 : 0 );
			else  /* OC_TOOLBAR or OC_FORM */
				SET_BIT( tree[ob].ob_state, SELECTED, ( *(bind[ob].desc.bit.var) & bind[ob].desc.bit.bit ) ? 1 : 0);
			break;
			
		case BIND_FUNC:
			bind[ob].desc.func.f.form = (func_bind)va_arg( arg, void*);
			bind[ob].desc.func.data = (void*)va_arg( arg, void*);			
			break;
			
		default:
			return ERANGE;
		}
	}
	va_end( arg);
	return E_OK;
}
