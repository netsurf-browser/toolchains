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
 * $Source: /cvsroot/windom/windom/examples/tutorial/tutorial.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:39:25 $
 *   $Revision: 1.2 $
 */

/*! @file */

/*!

@defgroup Tutorial Tutorial
@brief tutorial to introduce windom programming to newbies.

\section intro Introduction

WinDom programming is very similar to GEM programming. Actually, 
it is possible to use WinDom exactly like GEM but it is not the 
most efficient way. The main difference between GEM and WinDom 
in this example is that the GEM events returned by EvntWindom() 
(the WinDom equivalent of evnt_multi()) are not handled ! 
In fact, events are implicitly handled by EvntWindom(). When
an event occurs, EvntWindom() looks for an handler function, and
if it exists, this function is called. So, EvntWinDom() is the
\b core of WinDom : almost special features of WinDom are
handled in this function.

\b Definition (Callback function) <br> 
A function defined by the programmer but executed by WinDom core 
is called a <i>callback function</i>. For example, all GEM event
can be handled by callback functions.

\b Defintion (Standard callback function) <br>
Standard callack functions are callback function predefined by
WinDom during its initialization i.e. during the call of ApplInit().
For example, all events related to window have a standard callback
function (see manual of WindCreate()).

\section compile Compiling tutorial

Makefile provided is designed to work with gcc.

\code
% make          ; to compile all tutorials
% make tut-1-1  ; to compile this tutorial
\endcode

There are special switches :

\code
% make CROSS=yes  ; switch to add for cross-compiling
% make clean      ; cleaning the directory
\endcode

\section Examples

the first group of examples shows how it is simple to write a
simple GEM application. These are basic examples with simple window :
- \ref tut1_1
- \ref tut1_2
- \ref tut1_3

The second group of examples deals with more complex windows
- \ref tut2_1
- \ref tut2_2
- \ref tut2_3

*/

/** @addtogroup Tutorial
 * @{ */
 
/* -------------------------------------------------------------------------------- */

/** @defgroup tut1_1 Example 1.1 of the tutorial : Create and handle a window

@brief Create, open and handle a blank window in a GEM environment

\dontinclude tut-1-1.c

This is a very simple program. A single window is created and 
opened on screen.

\until MARK1

Very important: you have to include windom.h header file.

\until MARK2

You may have noticed than gemlib header in not here. It's ok because
windom \a requieres gemlib as GEM library, and so windom.h internally
includes gem.h and gemx.h. You don't have to include "gem.h" in your
code.

\until MARK3

In this example, user can manipulate the window (resizing,
moving, topping, iconifying, ...). Windom has attached the
standard callback function by default to the new created window,
so that all event received by the application (resize of the window,
window placed on the top, window iconified...) are "internally"
treated by windom.

The standard callback function of \b WM_REDRAW
event is mt_stdWindClear(). So, the work area of the window is
always "cleared".

These standard callback functions are attached to the window
by the function WindCreate().

\until MARK4

The main loop of any windom application should call EvntWindom().
In this function, windom will internally call the AES evnt_multi() 
to get informed of AES event for this application, and then EvntWindom()
will dispatch the event to the right callback function.

This loop ended when the condition "wglb.first" is false. This is a 
pointer to the first window of the application managed by windom.
When all the windows of this application are closed, then wglb.first
is NULL and then, the while() loop is leaved.

In our example, when the user destroys the window. In fact,
when you clic on the CLOSER widget, AES emits an \b WM_CLOSED
message. The standard callback function std_cls() linked to this event acts
by emitting an \b WM_DESTROY message and the destroy standard callback 
function std_dstry() acts by closing and deleting the window. In WinDom, we 
make the distinction beetween close a window and destroy a window. 
\b WM_DESTROY means that the window and its data have to be destroyed
and \b WM_CLOSED means window is just closed but not deleted

From that point, wglb.first is set to NULL, and the while() loop is then leaved.

\until MARK5

Probably, your first attempt is to define a custom window 
redraw callback and a custom destroy function, so go to \ref tut1_2.

 */

/* -------------------------------------------------------------------------------- */

/** @defgroup tut1_2 Example 1.2 of the tutorial : Window drawing callback

@brief Create, open and handle a proportional window.

\include tut-1-2.c

*/

/* -------------------------------------------------------------------------------- */

/** @defgroup tut1_3 Example 1.3 of the tutorial : Controling events

@brief  More events, best terminaison. In some case, EvntAdd() is better than EvntAttach()

\include tut-1-3.c

*/

/* -------------------------------------------------------------------------------- */

/** @defgroup tut2_1 Example 2.1 of the tutorial : Controling window sliders

@brief  

\include tut-2-1.c

*/


/* -------------------------------------------------------------------------------- */

/** @defgroup tut2_2 Example 2.2 of the tutorial : Adding GEM desktop

@brief  

\include tut-2-2.c

*/


/* -------------------------------------------------------------------------------- */

/** @defgroup tut2_3 Example 2.3 of the tutorial : Menu and toolbar in windows

@brief  

\include tut-2-3.c

*/
/** @} */
