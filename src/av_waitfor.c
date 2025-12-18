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
 * $Source: /cvsroot/windom/windom/src/av_waitfor.c,v $
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
 * @brief Wait for a given message.
 *
 * @param app application descriptor
 * @param msg message to wait for,
 * @param buf 8-word buffer,
 * @param idle time idle,
 * @return 1 if the message is received, 0 otherwise.
 *
 * AvWaitfor() waits for a specific message \b MU_MESAG
 * event. Typically, it is the answer of an AV request sent
 * to the AV server. If other messages occur, these messages
 * are not lost but are resent to the application. When the
 * message is received, \e buf parameter contains the AES buffer
 * message.
 *
 * Importante remark : AvWaitfor() does not make use of EvntWindom(), just
 * evnt_multi().
 *
 * Example : @verbatim
      // ask to server to launch an application and wait the answer 
      int exec_prog( char *prg, char *cmd) {
        ApplWrite( app.avid, AV_STARTPROG, prg, cmd, 0);
	return AvWaitfor( VA_PROGSTARTED, evnt.buf, 1000);
      } @endverbatim
 *
 */
 
int mt_AvWaitfor( APPvar *app, int msg, INT16 *buf, long idle) {
	int res;
	long count = 0;
	
	while(1) {
		res = mt_evnt_multi( MU_TIMER|MU_MESAG,
					0,0,0,
					0,0,0,0,0,
					0,0,0,0,0,
					buf,
					100L,
					NULL,NULL,NULL,
					NULL,NULL,NULL,
					app->aes_global);
		if( res & MU_MESAG) {
			if( buf[0] != msg) {
				mt_appl_write( mt_AppId(app), 16, buf, app->aes_global);
				count += 100;
				mt_evnt_timer(100,app->aes_global);
				if( count > idle)
					return 0;
			} else return 1;
		}
		if( res & MU_TIMER) {
			count += 100;
			if( count > idle)
				return 0;
		}
	} 
}

