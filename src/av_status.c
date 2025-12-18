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
 * $Source: /cvsroot/windom/windom/src/av_status.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.5 $
 */

#include <stdlib.h>
#include <string.h>
#include "globals.h"

/**
 * @brief returns features supported by AV server.
 *
 * @param app application descriptor
 * @return an array of 3 integers describing features supported by
 *         the AV server. Each element is a bitfield, each bit set to
 *         indicates a feature supported by the server. 
 *         - The first element may have the following bits :
 *           - \b V_SENDKEY (0x1) :  AV_SENDKEY message is supported
 *           - \b V_ASKFILEFONT (0x2) : AV_ASKFILEFONT is message supported
 *           - \b V_ASKCONFONT (0x4) : AV_ASKCONFONT is message supported
 *           - \b V_ASKOBJECT (0x8) : AV_ASKOBJECT message supported
 *           - \b V_OPENWIND (0x10) : 
 *           - \b V_STARTPROG (0x20) :
 *           - \b V_ACCWINDOPEN (0x40) :
 *           - \b V_STATUS (0x80) : 
 *           - \b V_COPY_DRAGGED (0x100) :
 *           - \b V_PATH_UPDATE (0x200) :
 *           - \b V_EXIT (0x400) :
 *           - \b V_XWIND (0x800) :
 *           - \b V_FONTCHANGED (0x1000) :
 *           - \b V_STARTED (0x2000) :
 *           - \b V_QUOTE (0x4000) : quoted filename supported
 *           - \b V_FILEINFO (0x8000) :
 *         - The second element may have the following bits :
 *           - \b V_COPYFILE (0x1) :
 *           - \b V_DELFILE (0x2) :
 *           - \b V_VIEW (0x4) :
 *           - \b V_SETWINDPOS (0x8) :
 *         - The third element is currently not used.
 *
 * This function returns the actions supported by the AV server,
 * if the AV session was succefully opened with the my_AvInit()
 * function. mt_AvStatus() returns a pointer to a 3-integer array.
 * Each value of this array is a bit field whose the values
 * are listed below.
 *
 */

INT16 *mt_AvStatus( APPvar *app) {
	return app->priv->__av_status;
}

