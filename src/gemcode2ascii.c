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
 * $Source: /cvsroot/windom/windom/src/gemcode2ascii.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.4 $
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "globals.h"
#include "scancode.h"

/*
 * a good idea would be to use NKCC... (TODO)
 */

void GemCode2Ascii( int key, int kstate, char *txt) {
	char code = key >> 8;

	switch( code){
	case SC_ESC:
		strcpy( txt, "ESC");
		break;
	case SC_HELP:
		strcpy( txt, "HELP");
		break;
	case SC_UNDO:
		strcpy( txt, "UNDO");
		break;
	case SC_INSERT:
		strcpy( txt, "INSERT");
		break;
	case SC_HOME:
		strcpy( txt, "HOME");
		break;
	case SC_TAB:
		strcpy( txt, "TAB");
		break;
	case SC_DEL:
		strcpy( txt, "DEL");
		break;
	case SC_BACK:
		strcpy( txt, "BACK");
		break;
	case SC_F1:
		strcpy( txt, "F1");
		break;
	case SC_F2:
		strcpy( txt, "F2");
		break;
	case SC_F3:
		strcpy( txt, "F3");
		break;
	case SC_F4:
		strcpy( txt, "F4");
		break;
	case SC_F5:
		strcpy( txt, "F5");
		break;
	case SC_F6:
		strcpy( txt, "F6");
		break;
	case SC_F7:
		strcpy( txt, "F7");
		break;
	case SC_F8:
		strcpy( txt, "F8");
		break;
	case SC_F9:
		strcpy( txt, "F9");
		break;
	case SC_F10:
		strcpy( txt, "F10");
		break;
	default:
		txt[1] = '\0';
		txt[0] = toupper( keybd2ascii( key, kstate & (K_LSHIFT|K_RSHIFT)));
		break;
	}
}
