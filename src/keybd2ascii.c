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
 * $Source: /cvsroot/windom/windom/src/keybd2ascii.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include "globals.h"

typedef struct {
        char *unshift;
        char *shift;
        char *capslock;
} MY_KEYTAB;

/** get the ascii code of a keyboard event
 *  
 *  @param keybd keyboard scancode provides by evnt_keybd() or \b MU_KEYBD event,
 *  @param shift should be set to 1 if the shift key is depressed, 0 else
 *  @return the ascii code associated to the event
 *  
 *  keybd2ascii() identify the real ascii code of a keyboard event even
 *  the shift, control and alternate keys are depressed. When these keys
 *  are used, the ascii code in the scancode (value returned by a
 *  keyboard event) are different. Moreover, the scancode depends on
 *  the country of the keyboard. This function gets the real ascii
 *  code of the key pressed and can be used for keyboard shortcut.
 *
 *  \par Example:
 *   @code  
#include <windom.h>
#include <scancode.h> // definition of keyboard scancodes

void ex_keybd( WINDOW *win) {
	char key =  keybd2ascii( evnt.keybd, evnt.mkstate & (K_LSHIFT|K_RSHIFT));
	switch( key) {
	case 'w':
	case 'W':
		// key w
		if( evnt.mkstate & K_CTRL)
			; // key Control-w
		break;
	// ...
	default:
		// Some keys have no ascii code (function key, numeric pad, ...).
		// These keys can be identified by their scancode.
		switch( evnt.keybd>>8) {
		case SC_HELP:
			// HELP key
			break;
		// ...
		}
	}
}
@endcode
 *
 */

int keybd2ascii( int keybd, int shift) {
	MY_KEYTAB *key;
	key = (MY_KEYTAB *)Keytbl( (char*)-1, (char*)-1, (char*)-1);
	return (shift)?key->shift[keybd>>8]:key->unshift[keybd>>8];
}

