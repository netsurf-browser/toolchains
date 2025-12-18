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
 * $Source: /cvsroot/windom/windom/src/bubble_find.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include "globals.h"

/**
 * @brief Find a bubble linked to an object
 *
 * @param app application descriptor,
 * @param tree address of an objects tree,
 * @param index object index,
 * @param help will be filled-in with the address of the string to display in the bubble
 * @return 1 if a bubble is found, 0 otherwise.
 *
 * mt_BubbleFind() is called by mt_BubbleEvnt() to find the text to display
 * in a bubble help. It can be used to find a bubble and display it
 * with mt_BubbleCall() if you are not interesting to use mt_BubbleEvnt().
 *
 * @sa mt_BubbleCall(), mt_BubbleEvnt(), mt_BubbleAttach()
 */
	
int mt_BubbleFind( APPvar *app, OBJECT *tree, int index, char **help)
{
	struct bubble *b=app->priv->bubble_index;
	
	while( b) {
		if( b->tree == tree && b->index == index) {
			*help = b->help;
			return 1;
		}
		b=b->next;
	}
	return 0;
}

