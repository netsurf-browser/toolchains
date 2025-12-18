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
 * $Source: /cvsroot/windom/windom/src/list.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include "globals.h"

/** splice a list (extract a part of a list)
 *
 *  @param list will contain the extracted list
 *  @param first is the first element of the extract
 *  @param pastLast is the element after the last element of the extract
 *  @return \a list
 *  
 *  Prerequise: a list of element exists. The elements \a first and \a
 *  pastLast are in this list of element. \a first must be ranged before
 *  \a pastLast in the list.
 *
 *  This function will extract the element from \a first (included)
 *  to \a pastLast (excluded), and create a new list with these element.
 *  This is the list stored in \a list. These elements are removed from
 *  the original list.
 */
LIST *listSplice( LIST *list, LINKABLE *first, LINKABLE *pastLast ) {
	listInit( list );

	if ( first == pastLast )
		return list;

	first->prev->next = pastLast;
	pastLast->prev->next = &list->tail;
	list->tail.prev = pastLast->prev;
	pastLast->prev = first->prev;
	first->prev = &list->head;
	list->head.next = first;
	return list;
}

/** create an empty list 
 *
 *  @return a pointer to a new LIST, or NULL in case of failure.
 *
 *  The list returned has been mallocated and initialised (listInit()).
 *  Don't forget to free() this LIST at the end.
 *
 */
LIST *createList( void ) {
	LIST *result = malloc( sizeof(LIST) );
	if ( !result )
		return NULL;

	listInit( result );
	return result;
}

