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
 * $Source: /cvsroot/windom/windom/src/evnt_list.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/06/20 19:59:30 $
 *   $Revision: 1.19 $
 */


#include <stdlib.h>
#include <stdio.h>   /* for DataTrace only: should be included befor global.h */
#include "globals.h"

/*
 *    WARNING: the EVNT list is a not a sorted list, but all
 *    the EV_MSG with the same "msg" value shall be linked together (no fragmentation)
 */

static EV_MSG *evnt_find( LIST *binding, short msg) {
	EV_MSG *scan;

	/* find the first msg handler one */
	listForEach( EV_MSG*, scan, binding ) {
		if( scan->msg == msg) return scan;
	}
	return NULL;
}


static int evnt_insert( LIST *binding, EV_MSG **result, short msg, void *proc, void *data, int mode) {
	EV_MSG *scan;
	EV_MSG *new = (EV_MSG *)malloc( sizeof(EV_MSG));
	if( !new) return ENSMEM;

	new -> msg  = msg;
	new -> flags = 0;
	new -> proc = proc;
	new -> data = data;
	
	scan = evnt_find( binding, msg);

	/* scan is standing on the first msg handler (EV_TOP) */
	if( mode == EV_BOT) {
		while( scan && scan->msg == msg) {
			scan = (EV_MSG*)listNext( scan);
		}
		/* now scan may also be of NULL value (end of the list) */
	}

	/* no such binding found: add to the list tail */
	if ( !scan ) {
		scan = (EV_MSG*)&binding->tail;
	}

	listInsert( (LINKABLE*)scan, (LINKABLE*)new );

	/* the output value is the just added entry */
	if (result) *result = new;
	return E_OK;
}


static void evnt_dispose( EV_MSG *trash) {
	if (trash->flags & EVM_IN_USE)
		trash->flags |= EVM_DELETED;
	else {
		listRemove( (LINKABLE*)trash );
		free( trash);
	}
}

static int evnt_remove( LIST *binding, int msg, void *proc) {
	EV_MSG *scan = evnt_find( binding, msg );
	while( scan && scan->msg == msg ) {
		if (scan -> proc == proc) {
			evnt_dispose( scan);
			return 1;
		}
		/* move on */
		scan = (EV_MSG*)listNext( scan);
	}
	return 0;
}

static int evnt_delete( LIST *binding, int msg) {
	int found = 0;
	EV_MSG *scan = evnt_find( binding, msg);
	while ( scan && scan->msg == msg ) {
		EV_MSG *trash = scan;
		scan = (EV_MSG*)listNext( scan);

		found = 1;
		evnt_dispose( trash );
	}

	return found;
}


/*
 * Remove all the event with the flag EVM_DELETED from a list
 */
static void evnt_clean_up( LIST *binding) {
	/* get the first item */
	EV_MSG *scan = (EV_MSG*)listFirst(binding);
	/* dispose deleted handlers */
	while ( scan ) {
		EV_MSG *trash = scan;
		scan = (EV_MSG*)listNext(scan); /* get the next list item */

		/* remove if deleted */
		if ( trash->flags & EVM_DELETED ) {
			listRemove( (LINKABLE*)trash);
			free( trash);
		}
	}
}

static void evnt_clear( LIST *list) {
	/* get the first item */
	EV_MSG *scan = (EV_MSG*)listFirst(list);
	/* dispose all items */
	while ( scan ) {
		EV_MSG *trash = scan;
		scan = (EV_MSG*)listNext(scan); /* get the next list item */
		evnt_dispose( trash);
	}
}

static void evnt_flag( LIST *binding, int msg, short flag, short onoff) {
	EV_MSG *scan;

	scan = evnt_find( binding, msg);
	while( scan && scan->msg == msg) {
		SET_BIT( scan->flags, flag, onoff);
		scan = (EV_MSG*)listNext(scan); /* get the next list item */
	}
}



/**
 * @brief find the first callback function bound to an event.
 *
 * @param app application descriptor,
 * @param win window descriptor or NULL (if application is addressed)
 * @param msg event to find.
 * @return callback function address or \b NULL.
 *
 * mt_EvntFind() returns the address of the first callback function
 * bound to an event. If the event is unbound,
 * mt_EvntFind() returns \b NULL. Currently, it is not possible to have
 * the list of function bound to a message. This function is just
 * used to know if a message is defined.
 *
 */

void *mt_EvntFind( APPvar *app, WINDOW *win, int msg) {
	EV_MSG *scan = evnt_find( win ? &win->binding : &app->priv->desk->binding, msg );
	if ( !scan ) return NULL;

	return scan->proc;
}



/**
 * @brief Remove one callback function bound to an event.
 *
 * @param app application descriptor,
 * @param win window descriptor (or NULL),
 * @param msg event to find,
 * @param proc callback function to remove,
 * @return \b TRUE if the callback function is found and removed.
 *
 * mt_EvntRemove() removes one and only one callback function bound to an
 * event. It is different to mt_EvntDelete() which removes all functions
 * bound to an same event.
 * Because a same function can be bound to different event, you should
 * give an event (\e msg parameter) and, of course, the address of function
 * to delete (\e proc parameter.
 *
 * @sa mt_EvntDelete(), mt_EvntClear().
 */

int mt_EvntRemove( APPvar *app, WINDOW *win, int msg, void *proc) {
	return evnt_remove( win ? &win->binding : &app->priv->desk->binding, msg, proc );
}

/** TODO: DOCS
 * 
 *  @param app application descriptor
 *  @param c
 *  @param msg
 *  @param proc
 *
 */
int mt_CompEvntRemove( APPvar *app, COMPONENT *c, int msg, void *proc) {
	return evnt_remove( &c->binding, msg, proc );
}


/**
 * @brief Delete an event
 *
 * @param app application descriptor,
 * @param win window descriptor or \b NULL (application targeted),
 * @param msg event to delete,
 * @return 0 or negative error code.
 *
 * EvntDelete() removes all callback functions bound to an event.
 *
 * @sa EvntDataAttach()
 */

int mt_EvntDelete( APPvar *app, WINDOW *win, int msg) {
	return evnt_delete( win ? &win->binding : &app->priv->desk->binding, msg );
}

/** TODO: DOCS
 * 
 *  @param app application descriptor
 *  @param c
 *  @param msg
 *
 */
int mt_CompEvntDelete( APPvar *app, COMPONENT *c, int msg) {
	return evnt_delete( &c->binding, msg );
}


/**
 * @brief removes all defined events of a window or of the application.
 *
 * @param app application descriptor,
 * @param win window descriptor or \b NULL.
 *
 * mt_EvntClear() removes all events defined to a window
 * or to the application (if \e win is \b NULL). Actually, this function
 * is called by mt_WindDelete() or mt_ApplExit() to release
 * allocated memory.
 *
 * @sa mt_EvntDelete(), mt_EvntRemove().
 */

void mt_EvntClear( APPvar *app, WINDOW *win) {
	evnt_clear( win ? &win->binding : &app->priv->desk->binding );
}

/** TODO: DOCS
 * 
 *  @param app application descriptor
 *  @param c
 *
 */
void mt_CompEvntClear( APPvar *app, COMPONENT *c) {
	evnt_clear( &c->binding );
}



/**
 * @brief Add a callback function to event and its data to a window or to the application.
 *
 * @param app application descriptor
 * @param win,msg,proc,data see manual of mt_EvntDataAttach()
 * @param mode way to add the callback function :
 *   - \b EV_TOP : callback function is added in top position (will be called in first)
 *   - \b EV_BOT : callback function is added in bottom position (will be called in last)
 * @return 0 or negative error code.
 *
 * This function is very similar to mt_EvntDataAttach(). The only difference is the
 * callback function is added in a list of callback instead of remplacing previous
 * callback function (or list of callback function) as mt_EvntDataAttach() acts.
 *
 * @sa mt_EvntAdd(), mt_EvntDataAttach().
 */

int mt_EvntDataAdd( APPvar *app, WINDOW *win, int msg, func_evntdata proc, void *data, int mode) {
	return evnt_insert( win ? &win->binding : &app->priv->desk->binding, NULL, msg, proc, data, mode);
}


/** add a function to a GEM event without data
 *
 * This function is just a call to mt_EvntDataAdd() with
 * parameter \a data set to NULL.
 */

int mt_EvntAdd( APPvar *app, WINDOW *win, int msg, func_evnt proc, int mode) {
	return mt_EvntDataAdd( app, win, msg, (func_evntdata)proc, app, mode);
}



/**
 * @brief bind a function to a GEM event.
 *
 * @param app application descriptor,
 * @param win window descriptor or \b NULL if event address the application not a window,
 * @param msg event to bind, possible are :
 *   - \b WM_XTIMER : timer event (\b MU_TIMER),
 *   - \b WM_XBUTTON : button event (\b MU_BUTTON),
 *   - \b WM_XKEYBD : keyboard event (\b MU_KEYBD),
 *   - \b WM_XM1 : \b MU_M1 event,
 *   - \b WM_XM2 : \b MU_M2 event,
 *   - other value : any GEM event mesag (\b MU_MESAG), the
 *         value specifie the number mesag (e.g. \b WM_CLOSED).
 * @param proc pointer to the callback function.
 * @param data data to pass as argument of callback function.
 * @return 0 or a negative code error.
 *
 * mt_EvntDataAttach() links a GEM event to a function (such a function is
 * called a \e callback function). A GEM event is
 * a button event, keyboard event, a mesage event too.
 * An event can be attached to a window or more, to the application or
 * both window and application. If an event is previously defined, a new
 * call of mt_EvntAttach() on this event removes the old event link. To cumulate
 * several callbacks to a same event, use mt_EvntAdd(). Of course,
 * that works only if mt_EvntWindom() is invoked (instead of evnt_multi())
 * to handle GEM event.
 *
 * A callback function has the following prototype : 
 * @code void function( WINDOW *win, short buf[8], void *data, APPvar *app); @endcode
 * where \e app is the application descriptor and \e win is the descriptor
 * of the targeted window or \b NULL if the event addresses the application.
 * \e data is the pointer provided by EvntDataAttach(), and \e buf[] contain the AES
 * message catched that triggered the invoking of this callback function.
 *
 * Here some examples of binding : @code
    // Define a global button event:
    EvntDataAttach( NULL, WM_XBUTTON, AppButton);

    // Define the button event of a window:
    EvntDataAttach( win, WM_XBUTTON, WinButton);

    // Define the window destroy event:
    EvntDataAttach( win, WM_DESTROY, WinDestroy);

    // Define a global event message (the application shutdown):
    EvntDataAttach( NULL, AP_TERM, ApTerm); @endcode
 *
 * @sa mt_EvntDataAdd(), mt_EvntAttach(), mt_EvntDelete(), EvntWindom().
 *
 * @todo A suggestion : mt_EvntAttach() appears to be useless. One could remove
 *       mt_EvntAttach() then rename mt_EvntDataAttach() in mt_EvntAttach().
 *       Same remark addressing mt_EvntAdd() and mt_EvntDataAdd().
 */


int mt_EvntDataAttach( APPvar *app, WINDOW *win, int msg, func_evntdata proc, void *data) {
	/* 1) remove old events */
	mt_EvntDelete( app, win, msg );

	/* 2) add an event */
    return mt_EvntDataAdd( app, win, msg, proc, data, EV_TOP );
}

/**
 * @brief bind a function to a GEM event without data.
 *
 * This function is just a call to mt_EvntDataAttach() with
 * parameter \e data set to NULL.
 */

int mt_EvntAttach( APPvar *app, WINDOW *win, int msg, func_evnt proc) {
	return mt_EvntDataAttach( app, win, msg, (func_evntdata)proc, app);
}


/** TODO: DOCS
 * 
 *  @param app application descriptor
 *  @param c
 *  @param msg
 *  @param proc
 *  @param data
 *  @param mode
 *
 */
int mt_CompEvntDataAdd( APPvar *app, COMPONENT *c, short msg, func_comp_evntdata proc, void *data, int mode) {
	return evnt_insert( &c->binding, NULL, msg, proc, data, mode );
}

/** TODO: DOCS
 * 
 *  @param app application descriptor
 *  @param c
 *  @param msg
 *  @param proc
 *  @param mode
 *
 */
int mt_CompEvntAdd( APPvar *app, COMPONENT *c, short msg, func_comp_evnt proc, int mode) {
	return mt_CompEvntDataAdd( app, c, msg, (func_comp_evntdata)proc, app, mode );
}


/** TODO: DOCS
 * 
 *  @param app application descriptor
 *  @param c
 *  @param msg
 *  @param proc
 *  @param data
 *
 */
int mt_CompEvntDataAttach( APPvar *app, COMPONENT *c, short msg, func_comp_evntdata proc, void* data) {
	/* 1) remove old events */
	mt_CompEvntDelete( app, c, msg );

	/* 2) add an event */
    return mt_CompEvntDataAdd( app, c, msg, proc, data, EV_TOP );
}

/** TODO: DOCS
 * 
 *  @param app application descriptor
 *  @param c
 *  @param msg
 *  @param proc
 *
 */
int mt_CompEvntAttach( APPvar *app, COMPONENT *c, short msg, func_comp_evnt proc) {
	return mt_CompEvntDataAttach( app, c, msg, (func_comp_evntdata)proc, app );
}



/**
 * @brief Enable all callback functions bound to an event.
 *
 * @param app application descriptor,
 * @param win targeted window or \b NULL (if application is targeted)
 * @param msg event to enable.
 *
 * mt_EvntEnable() enables an event previously disabled by
 * mt_EvntDisable().
 *
 * @sa mt_EvntExec(), mt_EventDisable().
 */

void mt_EvntEnable( APPvar *app, WINDOW *win, int msg) {
	evnt_flag( win ? &win->binding : &app->priv->desk->binding, msg, EVM_DISABLE, FALSE );
}

/**
 * @brief Disable all callback functions bound to an event.
 *
 * @param app application descriptor,
 * @param win window descriptor or \b NULL (if application is targeted)
 * @param msg event to disable.
 *
 * mt_EvntDisable() disables an event : callback functions bound to this event
 * will not be executed (by mt_EvntExec() then by mt_EvntWindom()). This
 * function is used to disabled temporally an event.
 *
 * @sa mt_EvntExec(), mt_EventEnable().
 */

void mt_EvntDisable( APPvar *app, WINDOW *win, int msg) {
	evnt_flag( win ? &win->binding : &app->priv->desk->binding, msg, EVM_DISABLE, TRUE );
}


/**
 * @brief Enable all callback functions bound to an event.
 *
 * @param app application descriptor,
 * @param c target component
 * @param msg event to enable.
 *
 * mt_CompEvntEnable() enables an event previously disabled by
 * mt_CompEvntDisable().
 *
 * @sa mt_CompEvntExec(), mt_CompEventDisable().
 */

void mt_CompEvntEnable( APPvar *app, COMPONENT *c, int msg) {
	evnt_flag( &c->binding, msg, EVM_DISABLE, FALSE );
}

/**
 * @brief Disable all callback functions bound to an event.
 *
 * @param app application descriptor,
 * @param c component descriptor.
 * @param msg event to disable.
 *
 * mt_CompEvntDisable() disables an event : callback functions bound to this event
 * will not be executed (by mt_CompEvntExec() then by mt_EvntWindom()). This
 * function is used to disable an event temporarily.
 *
 * @sa mt_CompEvntExec(), mt_CompEventEnable().
 */

void mt_CompEvntDisable( APPvar *app, COMPONENT *c, int msg) {
	evnt_flag( &c->binding, msg, EVM_DISABLE, TRUE );
}




/**
 * @brief Execute all functions bound to an event.
 *
 * @param app application descriptor,
 * @param w window descriptor or NULL (if application is targeted),
 * @param buff event to execute,
 * @return TRUE if an event function is found and correctly executed.
 *
 * EvntExec() finds all event functions bound to the event
 * \e msg and executes them if found. Important: \b WM_REDRAW cannot
 * be executed by EvntExec() because it should be call several time on
 * each rectangle of the AES redraw rectangle list. We have to used EvntRedraw
 * for that.
 *
 * User is advised to execute an event mesag it is better to
 * use mt_ApplWrite() instead of mt_EvntExec(). With mt_ApplWrite()
 * we give the control to AES before to execute. With EvntExec(),
 * events are executed directly. Some actions, as closing a window
 * for example, can confuses AES if they are executed directly.
 * And give the control to AES increases the multasking performance.
 * Actually, mt_EvntExec() is used by mt_EvntWindom() to execute
 * callback functions when an event occurs.
 *
 * @sa mt_EvntDisable(), mt_EvntEnable()
 *
 * @bug With the use of the flag EVM_IN_USE, problems may appear if
 * this function is re-entrant (f.e. if one of these find->proc callback
 * function calls EvntExec(). Normally, this doesn't happen.
 */

int mt_EvntExec( APPvar *app, WINDOW *w, short buff[8]) {
	WINDOW *win = w ? w : app->priv->desk;
	EV_MSG *handler = evnt_find( &win->binding, buff[0]);
	int found = 0;
	
	/* set flag WS_WIN_IN_USE to prevent mt_WindDelete() to free win */
	win->status |= WS_WIN_IN_USE;
	
	while( handler && handler->msg == buff[0]) {
		if( handler->proc && !(handler->flags & (EVM_DISABLE|EVM_DELETED)) ) {
			/* set the flag "in use" so that "handler" couldn't
			 * be deleted by someone else (a "destroy" evnt
			 * can delete itself !) */
			found = 1;
			handler->flags |=  EVM_IN_USE;
#ifdef NFD 
			NFD(("EvntExec: w=%04lx: msg=%04x  grect=[%3d,%3d,%3d,%3d]\r\n", (long)win,
				buff[0], buff[4], buff[5], buff[6], buff[7] ));
#endif
			(*handler->proc)( w, buff, handler->data, app);
			handler->flags &= ~EVM_IN_USE;
		}
		handler = (EV_MSG*)listNext((LINKABLE*)handler);
	}
	
	if ( found ) {
		/* now, it's time to free all deleted "handler" which just
		 * have their flag set to EVM_DELETED because they were
		 * "in use" */
		evnt_clean_up(&win->binding);
	}
	
	/* release the WS_WIN_IN_USE flag */
	win->status &= ~WS_WIN_IN_USE;
	
	/* if mt_WindDelete() has tried to free the window (but failed because
	 * of the WS_WIN_IN_USE flag), then we have to do it ourself now
	 */
	if (win->status & WS_DESTROYED) {
		mt_DataClear( app, win);
		free( win);
	}
	
	return found;
}

/** TODO: DOCS
 * 
 *  @param app application descriptor
 *  @param p
 *  @param buff
 *
 */
int mt_CompEvntExec( APPvar *app, COMPONENT *p, long buff[8]) {
	EV_MSG *handler = evnt_find( &p->binding, buff[0]);
	int found = 0;

	/* set flag to prevent mt_CompDelete(p) destruction */
	p->status |= CS_IN_USE;
	
	while( handler && handler->msg == buff[0]) {
		if( handler->proc && !(handler->flags & EVM_DISABLE)) {
			/* set the flag "in use" so that "handler" couldn't
			 * be deleted by someone else (a "destroy" evnt
			 * can delete itself !) */
			found = 1;
			handler->flags |=  EVM_IN_USE;
#ifdef NFD 
			NFD(("EvntExec: c=%04lx: msg=%04lx  grect=[%3ld,%3ld,%3ld,%3ld]\r\n", (long)p,
				buff[0], buff[4], buff[5], buff[6], buff[7] ));
#endif
			(*(func_comp_evntdata)handler->proc)( p, buff, handler->data, app);
			handler->flags &= ~EVM_IN_USE;
		}
		handler = (EV_MSG*)listNext((LINKABLE*)handler);
	}
	if ( found ) {
		/* now, it's time to free all deleted "handler" which just
		 * have their flag set to EVM_DELETED because they were
		 * "in use" */
		evnt_clean_up(&p->binding);
	}

	p->status &= ~CS_IN_USE;
	
	/* if mt_CompDelete() has been called to delete p (but failed
	 * because of the CS_IN_USE flag), then we have to do it now again */
	if (p->status & CS_DESTROYED) {
	       	/* Prevent loop - mt_CompDelete() calls handlers too */
		p->status &= ~CS_DESTROYED;
		mt_CompDelete( app, p);
	}

	return found;
}

/* EOF */
