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
 * $Source: /cvsroot/windom/windom/src/bubble_call.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/04/18 18:46:30 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "cookie.h"

/**
 * @brief Display a bubble help
 *
 * @param app application descriptor,
 * @param help pointeur to the string to display in a bubble
 * @param x,y coordinates of the bubble (use the mouse position) 
 * @return 0 (success) or one of the following error code :
 *        - (-1) bubbleGEM not loaded
 *        - (-2) the environment variable "BUBBLE=" is wrong
 *        - (-3) memory allocation error
 *
 * mt_BubbleCall() sends a message to BUBBLE.APP to display a bubble help.
 * If BUBBLE.APP is not loaded, mt_BubbleCall() tries to load it using the
 * PATH or BUBBLE or BUBBLEGEM environ variables. This function just
 * display a bubble help. It is possible to attach a bubble help to an
 * object from a formular or a toolbar and display them systematically. For
 * that purpose, see the mt_BubbleAttach() and mt_BubbleEvnt() functions.
 * 
 * The AES should not be stopped, that is the case with classic formulars
 * which make use of wind_update() function, when the mt_BubbleCall() function
 * is invoked. However, it is possible to call BubbleGEM from a classic
 * formular with the mt_BubbleModal() function.
 * 
 * @note 
 *  A '|' character forces a carriage return inside the bubble help
 * 
 * @sa mt_BubbleAttach(), mt_BubbleEvnt(), mt_BubbleFree(), mt_BubbleFind().
 */

int mt_BubbleCall( APPvar *app, char *help, int x, int y) {
	INT16 msg[8];
	int bubble_id;
	
	bubble_id = mt_BubbleGet(app);

    if (bubble_id >= 0) {
		if( app->pipe == NULL) return -3;

 		/* request font */
 		mt_ApplWrite( app, bubble_id, BUBBLEGEM_FONT, CONF(app)->bubble.font, CONF(app)->bubble.size, 0, 0, 0);
	
		/* display bubble */
		strcpy( app->pipe, help);
		mt_ApplWrite( app, bubble_id, BUBBLEGEM_SHOW, x, y, ADR(app->pipe),  BGS7_MOUSE);
 		
 		/* wait for ACK from bubbleGEM, timeout = 1 second */
 		mt_AvWaitfor( app, BUBBLEGEM_ACK, msg, 1000);

        return 0;
    }
    return bubble_id;
}
