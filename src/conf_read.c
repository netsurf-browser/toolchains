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
 * $Source: /cvsroot/windom/windom/src/conf_read.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.8 $
 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"

/* r‚pertoire courant, r‚pertoires HOME[/Defaults], r‚pertoires PATH,
   r‚pertoire C:\
   valeur de  retour: -33 pas de fichier
                      -1  aucune balise pour l'application
                       0  pas d'erreur
*/

/* try to open windom.cnf and then .windomrc in the given path/subpath
 */
static
FILE *fconf( APPvar *app, char *path, char *subpath) {
	FILE *fp;
	
	strcpy( app->priv->__confpath, path);
	add_slash( app->priv->__confpath);
	strcat( app->priv->__confpath, subpath);
	add_slash( app->priv->__confpath);
	strcat( app->priv->__confpath, "windom.cnf");
	fp = fopen( app->priv->__confpath, "r");
	if( fp == NULL && vq_extfs(path)) {
		strcpy( app->priv->__confpath, path);
		add_slash( app->priv->__confpath);
		strcat(app->priv-> __confpath, subpath);
		add_slash( app->priv->__confpath);
		strcat(app->priv-> __confpath, ".windomrc");
		fp = fopen( app->priv->__confpath, "r");
	}
	return fp;
}

/*	read data of ATTRIB type in the config data. The field
 *	to read are:
 *	- data.color
 *	- data.size
 *	- data.font
 */
static void read_attrib( APPvar *app, char *name, ATTRIB *attrib) {
	char buf[80];
	
	strcpy( buf, name);
	strcat( buf, ".color");
	mt_ConfInquire( app, buf, "%d", &attrib->color);
	strcpy( buf, name);
	strcat( buf, ".size");
	mt_ConfInquire( app, buf, "%d", &attrib->size);
	strcpy( buf, name);
	strcat( buf, ".font");
	if( mt_ConfInquire( app, buf, "%S", buf) >= 1) {
		attrib->font = mt_FontName2Id( app, buf);
		if(attrib->font == -1) attrib->font = 0;
	}
}


/* look for a [label] and load its content */
static
char *find_and_read( FILE *fp, char *tag, int *top) {
	int find = FALSE;
	int tagtop=0, real; /* tagtop init unused, just here to shut up gcc warnings */
	int bottom = -1;
	char *buffer, *p;
	char buf[LINE_SIZE+1];
	
	rewind( fp);

	*top = 0;
	while( !feof( fp)) {
		/* position indicator for ConfInquire() */
		tagtop = (int) ftell( fp);			
		p = fgets( buf, LINE_SIZE, fp);
		(*top) ++;
		if( p == NULL) break;
		if( *buf == '#' || *buf=='\n')
			continue;
		while( *p == ' ' || *p == '\t')
			p ++;		
		if( p == strstr( p, tag) ) {
			find = TRUE;
			break;
		}
	}	

	/* [label] not found */
	if( find == FALSE) 
		return NULL;
	
	/* look for the [end] tag */
	fseek( fp, tagtop, 0);
	while( !feof( fp)) {
		p = fgets( buf, LINE_SIZE, fp);
		if( p == NULL) break;
		if(*buf == '#' || *buf=='\n')
			continue;
		while( *p == ' ' || *p == '\t')
			p ++;
		if( p == strstr( p, "[end]")) {
			bottom = (int)ftell( fp);
			break;
		}
	}

	if (bottom == -1) {       /* the [end] tag doesn't exist */
		bottom = (int) ftell( fp);  /* feof(fp)==TRUE => bottom = end of the file */
	}

	buffer = (char*)malloc( sizeof(char)*(bottom-tagtop+1));
	fseek( fp, tagtop, 0);
	real = (int) fread( buffer, sizeof(char), bottom-tagtop, fp);
	buffer[MIN(real, bottom-tagtop)] = '\0';

	return buffer;
}

/** read the configuration file and set the WinDom parameters
 *
 *  @param app application descriptor
 *  @retval 0 success
 *  @retval -1 application not find in the configuration file
 *  @retval -33 configuration file not found
 *
 *	mt_ConfRead() reads in the WinDom configuration file the settings of
 *	the application. Only WinDom parameters are set. This file can
 *	contain other parameters readable with the functions mt_ConfGetLine()
 *	and mt_ConfInquire(). These parameters are specific to the application.
 *
 *	This function is used by mt_ApplInit() to initialize the WinDom
 *	settings and by mt_EvntWindom() when it receives the \c AP_CONF
 *	message.
 *
 *	The WinDom configuration file is an unique text file grouping the
 *	configuration of all WinDom application in a similar way then the
 *	'.Xdefaults' X11 file for example. This file is typically located
 *	in the HOME folder. Actually, WinDom searchs this file in the
 *	following directories :
 *	- the application directory,
 *	- the $HOME/Defaults directory,
 *	- the $HOME directory,
 *	- the $ETCDIR directory,
 *	- the $PATH directories (can be multiple),
 *	- the 'C:\\' directory
 *
 *	This file is nammed 'windom.cnf' or '.windomrc' if the file system
 *	supports the long name file.
 *
 *  @bug
 *  Each line has the following syntax :
 *  @code keyword = value list ... @endcode
 *  A space character is required between the keyword and the '=' character.
 *
 *  @sa mt_ApplInit(), mt_EvntWindom(), mt_ConfInquire(), mt_ConfGetLine()
 */
 
int mt_ConfRead( APPvar *app) {
	FILE *fp = NULL;
	char path[FILE_SIZE], *p;
	char buf[LINE_SIZE];
	int  dum;
	char nameprg[20];
	CONFIG *conf = CONF(app);
	long drv;
	
	/* search the config file 'windom.cnf' (or '.windomrc')... */
	
	/* ...in current directory */
	fp = fconf( app, ".", "");
	
	/* ...in $HOME[\Defaults] */
	mt_shel_envrn( &p, "HOME=", app->aes_global);
	if( fp == NULL && p ) {
		fp = fconf( app, p, "Defaults");
		if( fp == NULL ) 
			fp = fconf( app, p, "");
	}

	/* ...in $ETCDIR */
	mt_shel_envrn( &p, "ETCDIR=", app->aes_global);
	if( fp == NULL && p )
		fp = fconf( app, p, "");
	
	/* ...in directories from $PATH */
	if( fp == NULL) {
		strcpy( app->priv->__confpath, "windom.cnf");
		mt_shel_find( app->priv->__confpath, app->aes_global);
		fp = fopen( app->priv->__confpath, "r");
	}
	
	/* ... in C:\ */
	if (fp == NULL) {
		drv = Dgetdrv();
		if (Dsetdrv(drv) & 0x4)
			fp = fconf( app, "C:\\", "");
	}

	if( app->priv->__confapp_buffer != NULL) {
		free( app->priv->__confapp_buffer);
		app->priv->__confapp_buffer = NULL;
	}
	if( app->priv->__confdef_buffer != NULL) {
		free( app->priv->__confdef_buffer);
		app->priv->__confdef_buffer = NULL;
	}
				
	if( fp == NULL)
		return -33;  /* TODO: replace -33 by a constant from errno.h */

	get_appname( app, nameprg);
	
	/* We search the tag corresponding to our application settings and to
	 * the defaults settings, and load both settings
	 */

	app->priv->__confapp_buffer = find_and_read( fp, nameprg, &app->priv->__confapp_line_top);
	app->priv->__confapp_line_pos = app->priv->__confapp_line_top;	
	app->priv->__confdef_buffer = find_and_read( fp, "[Default Settings]", &app->priv->__confdef_line_top);
	app->priv->__confdef_line_pos = app->priv->__confdef_line_top;
	fclose( fp);
	
	/* none of theses tag exists */
	if( !app->priv->__confapp_buffer && !app->priv->__confdef_buffer)
		return -1;

	/*
	 *  Here, the default settings and the setting specific to this application
	 *  are loaded. We can now inquire values for theses settings./
	 */
	
	/* Events */
	if( mt_ConfInquire( app, "windom.evnt.keybd", "%s", buf)>=1)
		SET_BIT( conf->flag, FLG_KEYMOUSE, !stricmp(buf,"mouse"));
	if( mt_ConfInquire( app, "windom.evnt.button", "%s", buf)>=1)
		SET_BIT( conf->flag, FLG_BUTMOUSE, !stricmp(buf,"mouse"));

	/* extended object type */
	read_attrib( app, "windom.string", 	&conf->string);
	read_attrib( app, "windom.button", 	&conf->button);
	read_attrib( app, "windom.exit", 	&conf->exit);
	read_attrib( app, "windom.xedit.text", 	&conf->xedit_text);
	read_attrib( app, "windom.xedit.label", 	&conf->xedit_label);
	mt_ConfInquire( app, "windom.xedit.cursor.color", "%d", &conf->xedit_curs_color);
	read_attrib( app, "windom.xlongedit", 	&conf->xlongedit);
	mt_ConfInquire( app, "windom.xlongedit.smallsize", "%d", &conf->xlgedt_smlfnt);
	read_attrib( app, "windom.xtedinfo",		&conf->xtedinfo);
	mt_ConfInquire( app, "windom.xtedinfo.smallsize", "%d", &conf->xtdinf_smlfnt);
	read_attrib( app, "windom.menu", 	&conf->title);
	read_attrib( app, "windom.bubble",	&conf->bubble);
	mt_ConfInquire( app, "windom.relief.mono" , "%d", &conf->actmono);
	mt_ConfInquire( app, "windom.relief.color", "%d", &conf->actcol);
	
	/* popup  */
	mt_ConfInquire( app, "windom.popup.color",  "%d", &conf->popcolor);
	mt_ConfInquire( app, "windom.popup.border", "%d", &conf->popborder);
	mt_ConfInquire( app, "windom.popup.framec", "%d", &conf->popfcolor);
	mt_ConfInquire( app, "windom.popup.pattern","%d", &conf->poppatt);
	if( mt_ConfInquire( app, "windom.popup.relief", "%b", &dum) >= 1)
		if( dum) conf->poppatt |= 0x100;
	if( mt_ConfInquire( app, "windom.popup.window", "%s", buf) >= 1 ) {
		if( !stricmp( buf, "default"))	conf->popwind = DEFVAL;
		if( !stricmp( buf, "false"))	conf->popwind = FALSE;
		if( !stricmp( buf, "true"))		conf->popwind = TRUE;
	}

	/* GrectCenter */
	if( mt_ConfInquire( app, "windom.window.center", "%s", buf) >= 1) {
		if( !stricmp( buf, "screen"))	conf->wcenter = CENTER;
		if( !stricmp( buf, "mouse"))	conf->wcenter = WMOUSE;
		if( !stricmp( buf, "upleft"))	conf->wcenter = UP_LEFT;
		if( !stricmp( buf, "upright"))	conf->wcenter = UP_RIGHT;
		if( !stricmp( buf, "dnleft"))	conf->wcenter = DN_LEFT;
		if( !stricmp( buf, "dnright"))	conf->wcenter = DN_RIGHT;
		if( !stricmp( buf, "form"))		conf->wcenter = FCENTER;
	}

	/* windows */
	if( mt_ConfInquire( app, "windom.window.effect", "%s", buf) >= 1) {
		if( !stricmp( buf, "default"))	conf->weffect = DEFVAL;
		if( !stricmp( buf, "false"))	conf->weffect = FALSE;
		if( !stricmp( buf, "true"))		conf->weffect = TRUE;
	}
	mt_ConfInquire( app, "windom.mform.widget", "%x", &conf->mwidget);
	mt_ConfInquire( app, "windom.window.bg.pattern", "%d", &conf->bgpatt);
	mt_ConfInquire( app, "windom.window.bg.color", "%d", &conf->bgcolor);
	mt_ConfInquire( app, "windom.window.bg.style", "%d", &conf->bgstyle);
	
	/* userdef library */
	if( mt_AppAESnumapps(app) == 1 ) {   /* singletasking OS */
		if (mt_ConfInquire( app, "windom.userdef.list.tos", "%s", buf) >= 1)
			udlib_ConfSetLib(app,buf);
	} else {  /* mutlitasking OS */
		if (mt_ConfInquire( app, "windom.userdef.list", "%s", buf) >= 1)
			udlib_ConfSetLib(app,buf);
	}
	
	/* others */
	dum = TRUE;
	if( mt_ConfInquire( app, "windom.fsel.fslx", "%b", &dum) > 0)
		SET_BIT( conf->flag, FLG_NOMGXFSEL, !dum);
	mt_ConfInquire( app, "windom.shortcut.color", "%d", &conf->key_color);
	mt_ConfInquire( app, "windom.menu.effect", "%d", &conf->menu_effect);
	mt_ConfInquire( app, "windom.frame.widget.size", "%d", &conf->fwidsize);

/*	dum = FALSE;
	if( ConfInquire( "windom.menu.autokey", "%b", &dum))
		SET_BIT( conf->flag, FLG_NOKEYMENU, !dum); */

	dum = FALSE;
	if( mt_ConfInquire( app, "windom.menu.scroll", "%b", &dum) > 0)
		SET_BIT( conf->flag, FLG_MNSCRL, dum);
	dum = FALSE;
	if( mt_ConfInquire( app, "windom.menu.system", "%b", &dum) > 0)
		SET_BIT( conf->flag, FLG_MNSYSTEM, dum);
	mt_ConfInquire( app, "windom.iconify.geometry", "%d,%d", &conf->wicon, &conf->hicon);
	if( mt_ConfInquire( app, "windom.version", "%b", &dum)>=1 && dum) {
		sprintf( path, "[1][******* WinDom version *******|"
						   "version number: %x.%02x R%02d|"
						   "date : %s at %s|"
						   "compiler : %s ][Ok]",
				 WinDom.patchlevel>>8, WinDom.patchlevel&0x00FF,
				 WinDom.release, WinDom.date, WinDom.time, WinDom.cc_name);
		mt_form_alert( 1, path, app->aes_global);
	}
	return 0;
}

