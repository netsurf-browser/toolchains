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
 * $Source: /cvsroot/windom/windom/src/data.c,v $
 *
 * CVS info:
 *   $Author: opichals $
 *   $Date: 2006/02/22 16:15:25 $
 *   $Revision: 1.6 $
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>   /* for DataTrace only: should be included before global.h */
#include "globals.h"


/*  This is a private stucture pointed by win->data, app->priv->desk->data or c->data
 *  All the data attached to a window/component are stored in a list
 */
typedef struct _dentry {
		LINKABLE lnk;

		long magic;				/* the magic number idenitfies data */
		void *data; 			/* actual data value */
	} DENTRY;


static void *dentry_search( LIST *list, long magic) {
	DENTRY *scan;

	/* find the first *magic* DENTRY structure */
	listForEach( DENTRY*, scan, list ) {
		if( scan->magic == magic) return scan;
	}
	return NULL;
}

static void *data_search( LIST *list, long magic) {
	DENTRY *scan = dentry_search( list, magic );
	if ( !scan ) return NULL;
	return scan->data;
}

static int data_attach( LIST *list, long magic, void *data) {
	int ret = E_OK;
	DENTRY *new;

	/* search for the *magic* data entry */
	DENTRY *scan = dentry_search( list, magic );
	if ( scan) {
		new = scan;
		ret = 1;
	} else {
		new = malloc(sizeof(DENTRY));
		if( !new)	return ENSMEM;

		new->magic = magic;

		/* append to the list */
		listInsert( &list->tail, (LINKABLE*)new);
	}

	new->data = data;
	return ret;
}

/*
 * Remove and free the \a magic data entry.
 */
static int data_delete( LIST *list, long magic) {
	/* search for the first data */
	DENTRY *scan = dentry_search( list, magic );
	if ( !scan) {
		return -1; /* FIXME!!! ERR_NOT_FOUND */
	}

	/* unhook from the list */
	listRemove((LINKABLE*)scan);
	/* delete it */
	free(scan);

	return E_OK;
}

/*
 * Remove and free all the list data items.
 */
static void data_clear( LIST *list) {
	/* get the item ahead the first */
	DENTRY *scan = (DENTRY*)listRewind(list);
	/* move to the first list item */
	scan = (DENTRY*)listNext(scan);

	/* dispose all items */
	while ( scan ) {
		DENTRY *trash = scan;
		scan = (DENTRY*)listNext(scan); /* get the next list item */
		free( trash);
	}
}


/**
 * @brief Search data linked into a window or application.
 *
 * @param app application descriptor,
 * @param win window descriptor or \b NULL,
 * @param magic a magic number identifying data,
 * @return the data attached for the \a magic id, or \b NULL if not found.
 * 
 * Window \a magic value data search. If \a win is set to \b NULL, data
 * will find into the application data list.
 *
 * @sa mt_DataAttach(), mt_DataDelete().
 */
void *mt_DataSearch( APPvar *app, WINDOW *win, long magic) {
	return data_search( win ? &win->data : &app->priv->desk->data, magic );
}


/**
 * @brief Component \a magic value data search.
 *
 * @param app application descriptor,
 * @param c component descriptor to inspect,
 * @param magic data magic number to search,
 * @return the data attached for the \a magic id, or NULL if not found.
 *
 * The same function than mt_DataSearch() related to COMPONENT instead
 * of WINDOW.
 *
 * \sa mt_CompDataAttach(), mt_CompDataDelete().
 */
void *mt_CompDataSearch( APPvar *app, COMPONENT *c, long magic) {
	return data_search( &c->data, magic );
}


/**
 * @brief Insert a data into a window or into the application.
 *
 * @param app application descriptor,
 * @param win window descriptor or \b NULL,
 * @param magic a unique magic number identifying data,
 * @param data address of data to attach.
 * @return A value of 0 on success, or 1 in the replacement
 *         case. Negative error code.
 *
 * Insert data into the window data list or into the application if \e
 * win is set to \b NULL. Data linked to application are \e globals.
 * In case the \a magic data is already present its value is replaced and
 * the function returns 1.
 *
 * Some magic number are reserved by WinDom (because some predefined 
 * windows,  such as form, use data). See WinDom header file (they have
 * a WD_ prefix).
 *
 * @sa mt_DataSearch(), mt_DataDelete().
 */
int mt_DataAttach( APPvar *app, WINDOW *win, long magic, void *data) {
	return data_attach( win ? &win->data : &app->priv->desk->data, magic, data);
}

/**
 * @brief Insert data into the component data list.
 *
 * @param app application descriptor,
 * @param c component descriptor,
 * @param magic magic number identifying data,
 * @param data data to link.
 * @return A value of 0 on success, or 1 in the replacement
 *         case. Negative error code.
 * In case the \a magic data is already present its value is replaced.
 *
 * @sa mt_CompDataSearch(), mt_CompDataDelete().
 */
int mt_CompDataAttach( APPvar *app, COMPONENT *c, long magic, void *data) {
	return data_attach( &c->data, magic, data);
}


/**
 * @brief Remove a data linked into a window or into the application.
 *
 * @param app application descriptor,
 * @param win window descriptor or \b NULL,
 * @param magic magic number identifying data,
 * @return A value of 0 on success or \b E_NOTFOUND when the \a magic
 *         data item is not found.
 *
 * Remove the \a magic data item from the window data list or the application
 * data list if \a win if set to \b NULL.
 *
 * @sa mt_DataAttach(), mt_DataSearch().
 */

int mt_DataDelete( APPvar *app, WINDOW *win, long magic) {
	return data_delete( win ? &win->data : &app->priv->desk->data, magic );
}

/**
 * @brief Remove a data linked into a component.
 * @param app application descriptor,
 * @param magic magic number identifying data,
 * @param c component descriptor.
 * @return a value of 0 on success or \b E_NOTFOUND when the \a magic
 *         data item is not found.
 * 
 * Remove the \a magic data item from the component data list.
 */
int mt_CompDataDelete( APPvar *app, COMPONENT *c, long magic) {
	return data_delete( &c->data, magic );
}


/**
 * @brief Remove all the window data list items.
 * @param app application descriptor,
 * @param win window descriptor or \b NULL for application data list.
 * 
 * Used by destroy callback function to release memory.
 * @sa mt_DataDelete().
 */
void mt_DataClear( APPvar *app, WINDOW *win) {
	data_clear( win ? &win->data : &app->priv->desk->data );
}

/**
 * @brief Remove all the component data list items.
 * @param app application descriptor,
 * @param c component descriptor.
 *
 * @sa mt_DataDelete().
 */
void mt_CompDataClear( APPvar *app, COMPONENT *c) {
	data_clear( &c->data );
}



/*  DEBUG
 */
static void data_trace( LIST *list) {
	DENTRY *scan;
	listForEach( DENTRY*, scan, list ) {
		printf( "magic=%c%c%c%c data=0x%lX\n",
				(char)(scan->magic >> 24),
				(char)(scan->magic >> 16),
				(char)(scan->magic >>  8),
				(char)(scan->magic >>  0),
				(long)(scan->data));
	}
}

/*  DEBUG
 */
void mt_DataTrace( APPvar *app, WINDOW *win) {
	if (win) printf( "window %d:\n", win->handle);
	else     printf( "application:\n");
	data_trace( win ? &win->data : &app->priv->desk->data );
}

/*  DEBUG
 */
void mt_CompDataTrace( APPvar *app, COMPONENT *c) {
	printf( "component %p:\n", (void*)c);
	data_trace( &c->data );
}

/* EOF */
