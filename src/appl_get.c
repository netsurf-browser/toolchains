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
 * $Source: /cvsroot/windom/windom/src/appl_get.c,v $
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

/** 
 *  @brief Get application parameters.
 *
 *  @param app application descriptor,
 *  @param mode, v1, v2, v3, v4 similar to mt_ApplSet().
 *  @return 0 if no error.
 *
 *  mt_ApplGet() returns global application parameters. mt_ApplGet()
 *  performs the inverse action of mt_ApplSet().
 *
 *  Usage of \a v1,  \a v2, \a v3 and \a v4
 *  depends on \a mode value. For details about this mode, read
 *  manual of ::_config. 
 *
 *  The \b NULL value can be used if a parameter has not to be read
 *
 *  @sa mt_ApplSet()
 *  
 *  @todo names between windom configuration variable, constant
 *  for \a mode parameter and internal variable should be harmonized.
 */

int mt_ApplGet( APPvar *app, int mode, short *v1, short *v2, short *v3, short *v4) {
	switch( mode) {
	case APS_ICONSIZE:
		if( v1) *v1 = CONF(app)->hicon;
		if( v2) *v2 = CONF(app)->wicon;
		break;
	case APS_FLAG:
		if( v1) *v1 = CONF(app)->flag;
		break;
	case APS_WINBG:
		if( v1) *v1 = CONF(app)->bgcolor;
		if( v2) *v2 = CONF(app)->bgpatt;
		if( v3) *v3 = CONF(app)->bgstyle;
		break;
	case APS_KEYCOLOR:
		*v1 = CONF(app)->key_color;
		break;
	case APS_STRSTYLE:
		if( v1) *v1 = CONF(app)->string.font;
		if( v2) *v2 = CONF(app)->string.size;
		if( v3) *v3 = CONF(app)->string.color;
		break;
	case APS_BUTSTYLE:
		if( v1) *v1 = CONF(app)->button.font;
		if( v2) *v2 = CONF(app)->button.size;
		if( v3) *v3 = CONF(app)->button.color;
		break;
	case APS_EXITSTYLE:
		if( v1) *v1 = CONF(app)->exit.font;
		if( v2) *v2 = CONF(app)->exit.size;
		if( v3) *v3 = CONF(app)->exit.color;
		break;
	case APS_TITLESTYLE:
		if( v1) *v1 = CONF(app)->title.font;
		if( v2) *v2 = CONF(app)->title.size;
		if( v3) *v3 = CONF(app)->title.color;
		break;
	case APS_3DEFFECT:
		if( v1) *v1 = CONF(app)->actmono;
		if( v2) *v2 = CONF(app)->actcol;
		break;
	case APS_MENUEFFECT:
		if( v1) *v1 = CONF(app)->menu_effect;
		break;
	case APS_BUBBLESTYLE:
		if( v1) *v1 = CONF(app)->bubble.font;
		if( v2) *v2 = CONF(app)->bubble.size;
		break;
	case APS_POPUPSTYLE:
		if( v1) *v1 = CONF(app)->popcolor;
		if( v2) *v2 = CONF(app)->popborder;
		if( v3) *v3 = CONF(app)->popfcolor;
		if( v4) *v4 = CONF(app)->poppatt;
		break;
	case APS_POPUPWIND:
		if( v1) *v1 = CONF(app)->popwind;
		break;
	case APS_WINDOWS:
		if( v1) *v1 = CONF(app)->wcenter;
		if( v2) *v2 = CONF(app)->weffect;
		if( v3) *v3 = CONF(app)->mwidget;
		break;
	case APS_XEDITTEXTSTYLE:
		if( v1) *v1 = CONF(app)->xedit_text.font;
		if( v2) *v2 = CONF(app)->xedit_text.size;
		if( v3) *v3 = CONF(app)->xedit_text.color;
		break;
	case APS_XEDITLABELSTYLE:
		if( v1) *v1 = CONF(app)->xedit_label.font;
		if( v2) *v2 = CONF(app)->xedit_label.size;
		if( v3) *v3 = CONF(app)->xedit_label.color;
		break;
	case APS_XTEDINFOSTYLE:
		if( v1) *v1 = CONF(app)->xtedinfo.font;
		if( v2) *v2 = CONF(app)->xtedinfo.size;
		if( v3) *v3 = CONF(app)->xtedinfo.color;
		if( v4) *v4 = CONF(app)->xtdinf_smlfnt;
		break;
	case APS_XLONGEDITSTYLE:
		if( v1) *v1 = CONF(app)->xlongedit.font;
		if( v2) *v2 = CONF(app)->xlongedit.size;
		if( v3) *v3 = CONF(app)->xlongedit.color;
		if( v4) *v4 = CONF(app)->xlgedt_smlfnt;
		break;
	case APS_FRAMEWIDGETSIZE:
		if( v1) *v1 = CONF(app)->fwidsize;
		break;
	default:
		return 1;	/* Error */
	}	
	return 0;	/* No Error */
}
