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
 * $Source: /cvsroot/windom/windom/src/rsrc_xfree.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/02/16 21:29:59 $
 *   $Revision: 1.6 $
 */


#include <stddef.h>
#include <stdlib.h>
#include "globals.h"

/**
 * @brief Release memory allocated by mt_RsrcXload().
 *
 * @param app application descriptor,
 * @param rsc pointer to resource structure (provided by mt_RsrcXload()),
 * @sa mt_RsrcXload().
 */

void mt_RsrcXfree( APPvar *app, void *rsc) {
	mt_RsrcFreeCicon( app, &((RSC *)rsc)->fix);
	free( (void *)((RSC *)rsc)->adr);
}
