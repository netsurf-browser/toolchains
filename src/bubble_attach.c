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
 * $Source: /cvsroot/windom/windom/src/bubble_attach.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include "globals.h"

/**
 * @brief Link a bubble help to an object
 *
 * @param app application descriptor,
 * @param tree address of an objects tree,
 * @param index object index,
 * @param help address of string to display in the bubble
 *            (the string is not duplicated)
 * @return 0 (success) or -1 (memory error).
 *
 * This function links a bubble help to an objet in a window formular or
 * a toolbar. The bubble will be displayed by the mt_BubbleEvnt() function,
 * typically after a timer event or a right mouse button event. When the
 * program terminates, the bubbles should be free up with the mt_BubbleFree()
 * function.
 *
 * @sa mt_BubbleEvnt(), mt_BubbleFree(), mt_BubbleFind().
 */

int mt_BubbleAttach( APPvar *app, OBJECT *tree, int index, char *help) {
	struct bubble *b=app->priv->bubble_index;
	
	if( b == NULL) {
		b = (struct bubble *)malloc(sizeof(struct bubble));
		if( b == NULL) return -1; /* erreur */
		b->tree=tree;
		b->index=index;
		b->help=help;
		b->next=NULL;
		app->priv->bubble_index = b;
	} else {
		while( b->next) b = b->next;
		b->next = (struct bubble *)malloc(sizeof(struct bubble));
		if( b->next == NULL) return -1; /* erreur */
		b->next->tree=tree;
		b->next->index=index;
		b->next->help=help;
		b->next->next=NULL;
	}
	return 0;
}
