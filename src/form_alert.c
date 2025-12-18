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
 * $Source: /cvsroot/windom/windom/src/form_alert.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "globals.h"

int mt_vFormAlert( APPvar *app, int but, char fmt[], va_list list) {
	char alrt[255];
	vsprintf( alrt, fmt, list);
	return mt_form_alert( but, alrt, app->aes_global);
}

/**
 * @brief Display a GEM alert.
 *
 * @param app application descriptor,
 * @param but number of the exit button which is to be made default (1-3),
 * @param fmt formatted string as follows: "[#][Alert Text][Buttons]" and
 *        accepting printf's variable descriptor,
 * @param ... variable argument list according to format specified in \e fmt.
 *
 * This function is a direct call to the AES function mt_form_alert() allowing you printing
 * variable value as with printf().
 * 
 * \b Example
 * @code FormAlert( 1, "[1][Event message %d occurs.][OK]", buff[0]); @endcode
 */

int mt_FormAlert( APPvar *app, int but, char fmt[], ...) {
	va_list list;
	int res;
	
	va_start( list, fmt);
	res = mt_vFormAlert( app, but, fmt, list);
	va_end( list);
	
	return res;
}
