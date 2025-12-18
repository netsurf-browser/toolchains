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
 * $Source: /cvsroot/windom/windom/src/appl_control.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/04 19:14:11 $
 *   $Revision: 1.7 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "globals.h"

#define MA	0x4D41	/* 'MA' */
#define GX	0x4758	/* 'GX' */
 
/** 
 * @brief Control of GEM process.
 *
 * @param app application descriptor,
 * @param ap_cid identifier of application targetted,
 * @param ap_cwhat action to perform.
 * @param ap_cout pointer for the return value if any.
 *
 * @return same as AES function mt_appl_control()
 *
 * Refer to the documentation of the AES function mt_appl_control()
 * for details about these parameters.
 *
 * If the AES supports mt_appl_control(), then mt_appl_control
 * with the given parameters is invoked.
 *
 * If Magic is detected, as Magic doesn't support mt_appl_control(),
 * we try to emulate this function thanks to the Magic specific
 * message SM_M_SPECIAL. At the moment, only the following actions
 * (\a parameter ap_cwhat) are emulated :
 *       - \b APC_HIDE: mask the application,
 *       - \b APC_SHOW: show the application,
 */

int mt_ApplControl( APPvar *app, int ap_cid, int ap_cwhat, void *ap_cout) {
	/* no need to protect this global variable by mutex because all running
	 * applications have the same value (the AES is common for all applications)
	 * and if it's running twice at the same time, it's not a pb at all.
	 */
	static int has_appl_control = -1;
	
	if (has_appl_control == -1) {
		short parm1, dum;
		if (mt_appl_getinfo( AES_NAES, &parm1, &dum, &dum, &dum, app->aes_global) == 1 && parm1)
			has_appl_control = 1;
		else
			has_appl_control = 0;
	}
	
	if( has_appl_control) {
		return mt_appl_control( ap_cid, ap_cwhat, ap_cout, app->aes_global);
	} else if( vq_magx() >= 0x0310) {
		switch( ap_cwhat) {
		case APC_HIDE:
			mt_ApplWrite( app, SCREENMGR, SM_M_SPECIAL, 0, MA, GX, SMC_HIDEACT, 0);
			break;
		case APC_SHOW:
			mt_ApplWrite( app, SCREENMGR, SM_M_SPECIAL, 0, MA, GX, SMC_UNHIDEALL, 0);
			break;
		case APC_TOP:
			/* TODO: WindSet(0) for applications and ApplWrite( AC_OPEN, ...) for accessories ? */
			break;
		case APC_HIDENOT:
			mt_ApplWrite( app, SCREENMGR, SM_M_SPECIAL, 0, MA, GX, SMC_HIDEOTHERS, 0);
			break;
		}
	}
	return 1;	/* pas d'erreur ? */
}
