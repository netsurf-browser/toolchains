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
 * $Source: /cvsroot/windom/windom/src/menu_execcmd.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "scancode.h"

/*   parse the menu and look for the menu entry associated to
 *   the key pressed (shortcut).
 *   If found, an AES message MN_SELECTED is self-sent to
 *   execute the functions attached to this event.
 *
 *   The menu may be the main menu (win=NULL) or the menu within a
 *   window (win!=NULL).
 *
 *   return 1 if the key event is a shortcut for this menu.
 *
 *   Shortcut rules:
 *   a shortcut may have a space before and after its description.
 *   the shorcut description may be composed of:
 *      ^=control
 *   	=alternate
 * 		=shift
 *   	A-Z,F1-F10
 *   	ESC,UNDO,HELP,TAB,BACK,DEL,HOME,INSERT
 *   
 *   examples:
 *   			" I "
 *   			" ^C "
 *   			" UNDO "
 *   			" F3 "
 *   			" DEL "  etc ...
 *   
 *  NOTA: at the moment, combinaisons of SHIFT, CTRL and ALT doesn't work.
 */
int menu_exec_cmd( APPvar *app, WINDOW *win) {
	int titre, entre;
	char cmd[10], rac[10], *str, *p;
	OBJECT *menu;

	menu = win?win -> menu.root:app->menu;
	if( !menu) /* there's no menu */
		return 0;
	
	/* transfrom the key event to ascii string */
	GemCode2Ascii( app->evnt.keybd, app->evnt.mkstate, rac);

	if( *rac == '\0' || *rac == ' ')
		return 0;

	/* check shortcuts */
	strcpy( cmd, " ");
	if( app->evnt.mkstate & K_ALT)
		strcat( cmd, "");
	else if( app->evnt.mkstate & K_CTRL)
		strcat( cmd, "^");
	else if( app->evnt.mkstate & K_RSHIFT || app->evnt.mkstate & K_LSHIFT)
		strcat( cmd, "");
	else if ( strlen(rac)==1 ) /* a single char with no modifier -> not a shortcut key */
		return 0;
	strcat( cmd, rac);
	strcat( cmd, " ");
	titre = entre = 3;
	
	while( ! ob_istype(app,menu,entre,G_BOX))
		entre ++;
	
	while( 1) {
		do {
			entre ++;
			if( ob_istype(app,menu,entre,G_BOX))
				break;
			if( menu[titre].ob_state & DISABLED ||
				menu[entre].ob_state & DISABLED )
				continue;
			
			str = mt_ObjcString( app, menu, entre, NULL);
			p = strstr( str, cmd);
			if( p != NULL && str+1 != p) {
				mt_ApplWrite( app, mt_AppId(app), MN_SELECTED, titre, entre, ADR(menu), obj_root(menu,entre));
				return 1;
			}
		} while( !(menu[ entre].ob_flags & LASTOB));
		if( menu[ entre].ob_flags & LASTOB)
			return 0; /* no menu entry correspond to the key event */
		titre ++;
	}
}
