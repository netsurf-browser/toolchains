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
 * $Source: /cvsroot/windom/windom/src/bubble_get.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.7 $
 */

#include <stdlib.h>
#include "globals.h"

/**
 * @brief load the BubbleGEM daemon in memory
 *
 * @param app application descriptor,
 * @return the AES application ID of bubbleGEM, or a negative
 * value if bubbleGEM is not loaded
 *
 * mt_BubbleGet() loads if needed BUBBLE.APP, the BubbleGEM daemon,
 * in memory and returns its AES app-id. Its uses the AES environ
 * variable 'BUBBLE=' and 'BUBBLEGEM=' to locate the program. A
 * search in 'PATH=' folders is performed too. mt_BubbleGet() is
 * called because a mt_BubbleModal() call.
 *
 * @sa mt_BubbleModal(), mt_BubbleCall().
 */

int mt_BubbleGet( APPvar *app) {
	int bubble_id = mt_appl_find("BUBBLE  ", app->aes_global);

	if( bubble_id < 0 && app->aes_global[1] != 1) {
		char *value;
    	
    	mt_shel_envrn( &value, "BUBBLE=", app->aes_global);
    	if( !value) mt_shel_envrn( &value, "BUBBLEGEM=",app->aes_global);
    	if( value)
    		mt_shel_write( 1, 1, 100, value, "", app->aes_global);
    	else 
    		return -2;

    	mt_evnt_timer( 500, app->aes_global);
    	app->priv->__bubble_quit = bubble_id = mt_appl_find("BUBBLE  ",app->aes_global);
	}
	return bubble_id;
}
