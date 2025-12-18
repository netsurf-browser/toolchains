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
 * $Source: /cvsroot/windom/windom/src/appl_set.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.6 $
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "globals.h"

static void set_value( short *var, int val) {
	if( val != -1)
		*var = val;
}

/**
 * @brief Set application parameters
 *
 * @param app application descriptor
 * @param mode, v1,v2,v3,v4 see below.
 * @return 0 if no error.
 *
 * mt_ApplSet() sets global application parameters. When mt_ApplInit()
 * is called, the WinDom configuration is read. However, the 
 * developper can set its own settings using mt_ApplSet(). 
 *
 * Refer to the documentation of the ::_config private stucture for 
 * details about the configuration and possible values of \a mode,
 * \a v1, \a v2, \a v3 and \a v4.
 * 
 * The special \b DEFVAL value does not change the value. For example : @code
// Set only the color of string objects
mt_ApplSet( app, APS_STRSTYLE, DEFVAL, DEFVAL, RED); @endcode
 *
 *
 * @sa mt_ApplGet()
 * @sa Configuration
 */

int mt_ApplSet( APPvar *app, int mode, short v1, short v2, short v3, short v4) {
	switch( mode) {
	case APS_ICONSIZE:
		set_value( &CONF(app)->hicon, v1);
		set_value( &CONF(app)->wicon, v2);
		break;
	case APS_FLAG:
		SET_BIT( CONF(app)->flag , v1, v2);
		break;
	case APS_WINBG:
		set_value( &CONF(app)->bgcolor, v1);
		set_value( &CONF(app)->bgpatt,  v2);
		set_value( &CONF(app)->bgstyle, v3);
		break;
	case APS_KEYCOLOR:
		set_value( &CONF(app)->key_color, v1);
		break;
	case APS_STRSTYLE:
		set_value( &CONF(app)->string.font, v1);
		set_value( &CONF(app)->string.size, v2);
		set_value( &CONF(app)->string.color, v3);
		break;
	case APS_BUTSTYLE:
		set_value( &CONF(app)->button.font, v1);
		set_value( &CONF(app)->button.size, v2);
		set_value( &CONF(app)->button.color, v3);
		break;
	case APS_EXITSTYLE:
		set_value( &CONF(app)->exit.font, v1);
		set_value( &CONF(app)->exit.size, v2);
		set_value( &CONF(app)->exit.color, v3);
		break;
	case APS_TITLESTYLE:
		set_value( &CONF(app)->title.font, v1);
		set_value( &CONF(app)->title.size, v2);
		set_value( &CONF(app)->title.color, v3);
		break;
	case APS_3DEFFECT:
		set_value( &CONF(app)->actmono, v1);
		set_value( &CONF(app)->actcol, v2);
		break;
	case APS_MENUEFFECT:
		set_value( &CONF(app)->menu_effect, v1);
		break;
	case APS_BUBBLESTYLE:
		set_value( &CONF(app)->bubble.font, v1);
		set_value( &CONF(app)->bubble.size, v2);
		break;
	case APS_POPUPSTYLE:
		set_value( &CONF(app)->popcolor, v1);
		set_value( &CONF(app)->popborder, v2);
		set_value( &CONF(app)->popfcolor, v3);
		set_value( &CONF(app)->poppatt, v4);
		break;
	case APS_POPUPWIND:
		set_value( &CONF(app)->popwind, v1);
		break;
	case APS_WINDOWS:
		set_value( &CONF(app)->wcenter, v1);
		set_value( &CONF(app)->weffect, v2);
		set_value( &CONF(app)->mwidget, v3);
		break;
	case APS_XEDITTEXTSTYLE:
		set_value( &CONF(app)->xedit_text.font, v1);
		set_value( &CONF(app)->xedit_text.size, v2);
		set_value( &CONF(app)->xedit_text.color, v3);
		break;
	case APS_XEDITLABELSTYLE:
		set_value( &CONF(app)->xedit_label.font, v1);
		set_value( &CONF(app)->xedit_label.size, v2);
		set_value( &CONF(app)->xedit_label.color, v3);
		break;
	case APS_XTEDINFOSTYLE:
		set_value( &CONF(app)->xtedinfo.font, v1);
		set_value( &CONF(app)->xtedinfo.size, v2);
		set_value( &CONF(app)->xtedinfo.color, v3);
		set_value( &CONF(app)->xtdinf_smlfnt, v4);
		break;
	case APS_XLONGEDITSTYLE:
		set_value( &CONF(app)->xlongedit.font, v1);
		set_value( &CONF(app)->xlongedit.size, v2);
		set_value( &CONF(app)->xlongedit.color, v3);
		set_value( &CONF(app)->xlgedt_smlfnt, v4);
		break;
	case APS_FRAMEWIDGETSIZE:
		set_value( &CONF(app)->fwidsize, v1);
		if( v1 != -1) __frame_set_widget( app, v1);
	default:
		return 1;
	}
	return 0;
}

