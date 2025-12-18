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
 * $Source: /cvsroot/windom/windom/src/fsel.c,v $
 * Module : the WinDom interface to system file selector
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.8 $
 */

/*
 * Warning: some functions come from module fsel_inp.c
 *          written by Olivier Scheel.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "fsel.h"
#include "cookie.h"

/* Types
 */
typedef  char str17[17];
typedef  char str128[128];


/* Pr‚paration d'une liste pour
 * les s‚lecteurs de fichiers.
 * Retourne le nombre de chaines.
 */

static int conv_var( char *str) {
	int num = 1;
	
	while( *str) {
		if( *str == ';') {
			*str = '\0';
			num ++;
		}
		str ++;
	}
	return num;
}

/*
 *	path: buffer de 200 octets
 * 			"c:"
 *			"c:\\"
 *			"c:\\auto"
 *			"c:\\auto\\"
 *			""
 *  name: buffer de 14 octets
 *
 *  avant l'appel path contient le r‚pertoire
 *  et name le nom par d‚faut
 *	si *path = 0, on cherche dans le r‚pertoire courant
 *  si *ext = 0, on utilise *.* comme mask
 *
 *  aprŠs l'appel path contient le repertoire final
 *  et name le nom final.
 *
 *  Peut servir pour choisir des fichiers ou des r‚pertoires
 *
 *	Nouveau : paramŠtres lpath et lext (ainsi que
 *				les autres pr‚selections)
 */
/**
 * @brief Universal file selector.
 *
 * @param app application descriptor,
 * @param path directory where the file selector is opened, then the directory
 *             of the selected item,
 * @param name of a default file, then the name of the selected file,
 * @param ext file mask,
 * @param title selector title if selector supports it, otherwise, it will be ignored
 * @param lpath list of predefined directories if selector supports it or \b NULL,
 * @param lext list of predefined mask file if selector supports it or \b NULL,
 * @return 1 if an object has been sucessfully selected, 0 otherwise.
 *
 * mt_FselInput() is a custom call of the GEM file selector. If an alternative
 * file selector is available ('FSEL' cookie, Selectric, BoxKite 2, FLSX
 * extensions), it will be used instead of the GEM standard file selector.
 *
 * When the function returns, parameters \e path and \e name
 * are filled in with the directory and the filename selected. Then 
 * mt_FselInput() can be used both as a file and as a directory selector. If the
 * \e path parameter is an empty string, the current directory is
 * used. If the \e ext parameter is an empty string, the default
 * mask used by mt_FselInput() will be "*.*". 
 *
 * Parameter \e title sets fileselector title and can be used
 * for any TOS version. If fileselelector does not support this
 * feature, this parameter has no effect.
 *
 * The \e lpath and \e lext parameters allow you to define a list
 * of preset directories and file mask. There list are displayed inside the file
 * selector if it is possible (that is the case with Selectrics,
 * BoxKite 2 and the FLSX selectors). A list is a string whose each
 * items are delimited by a ';' character, for example : @code
  "C:\\usr;C:\\usr\\bin"
  "*.c;*.h"
@endcode
 *
 * mt_FselInput() adds in the directories preset, the path of the
 * user directory (if the $HOME environ variable is defined). The
 * environ variables $FSELPATH and $FSELMASK are used to build
 * a default list of directories and file mask. The additional
 * lists given by \e lpath and \e lext are adding in
 * these lists. Environ variables provide a way to configure 
 * globally all WinDom clients. In addition, it is possible, for
 * the user, to configure a specific application : the 
 * \b windom.fsel.path and \b windom.fsel.mask variable in the configuration
 * file define the list of directories and file mask. These
 * values are adding to the lists displayed by the file selector.
 *
 * If the system has a FSLX extension (see mt_appl_getinfo()), the
 * file selector is displayed in a modal window. 
 * The \b windom.fsel.fslx variable from the WinDom configuration file
 * set to \b FALSE forces mt_FselInput() to not use the FSLX extension. 
 *
 * \par Example    
 * @code
int CallFsel ( name) {
  static char path[255]=""; // First usage : current directory
  char fullname[255]="";
     
  if( FselInput( path, name, "Load a file", "", NULL, NULL)) {
    strpcy( completname, path);
    strcat( completname, name);
    strpcy( name, fullname); // return the full path
    return 1;
  } else 
    return 0;
}
@endcode
 */

int mt_FselInput( APPvar *app, char *path, char *name, char *ext, char *title,
		  char *lpath, char *lext) {

	INT16 res, dum;
	int npath, nmask;
	char *p;
	char *paths=NULL, *mask=NULL;
	char buf[FILE_SIZE];

	/* protection against the case ext==NULL */
	if (!ext) ext="";

	/* Find current directory */
	if( *path == '\0') {
		path[0] = 'A' + Dgetdrv();
		path[1] = ':';
		Dgetpath( path + 2, 0);
	}
	add_slash( path);

	if( app->aes4 & AES4_FSLX && !(CONF(app)->flag & FLG_NOMGXFSEL)) {
		int len;
		/* Build the directories lists */
	
		/* compute buffer size to store all paths */
		len = (int)strlen( path)+1;
		len += (lpath?(int)strlen(lpath):0)+1;
		mt_shel_envrn( &p, "HOME=",app->aes_global);
		if( p && *p) len += (int)strlen( p) + 1;
		if( mt_ConfInquire( app, "windom.fsel.path", "%S", buf)>0 && *buf)
			len += (int)strlen(buf)+1;
		mt_shel_envrn( &p, "FSELPATH=", app->aes_global);
		if( p && *p) len += (int)strlen( p)+1;
		paths = malloc( len*sizeof(char));
		if( paths == NULL) return -1;
		*paths = '\0';

		/* fillin list */
		strcat( paths, path);
		strcat( paths, ";");
		if( lpath) {
			strcat( paths, lpath);
			strcat( paths, ";");
		}
		mt_shel_envrn( &p, "HOME=", app->aes_global);
		if( p && *p) {
			strcat( paths, p);
			add_slash( paths);
			strcat( paths, ";");		
		}	
		if( mt_ConfInquire( app, "windom.fsel.path", "%S", buf)>0 && *buf) {
			strcat( paths, buf);
			strcat( paths, ";");
		}
		mt_shel_envrn( &p, "FSELPATH=", app->aes_global);
		if( p && *p) {
			strcat( paths, p);
			strcat( paths, ";");		
		}
		/* convert to admissible format for selector */
		npath = conv_var(paths);
	
		/* Build list mask files */	
	
		/* compute size of buffer */
				
		len = (int)strlen( (*ext)?ext:"*") + 1;
		len += (lext?(int)strlen(lext):0)+1;
		if( mt_ConfInquire( app, "windom.fsel.mask", "%S", buf)>0 && *buf)
			len += (int)strlen(buf)+1;
		mt_shel_envrn( &p, "FSELMASK=",app->aes_global);
		if( p && *p) len += (int)strlen( p)+1;
		mask = malloc( len*sizeof(char));
		if( mask == NULL) {
			free( paths);
			return -1;
		}
		*mask = '\0';

		/* fillin buffer */
		strcat( mask, (*ext)?ext:"*");
		strcat( mask, ";");
		if( lext) {
			strcat( mask, lext);
			strcat( mask, ";");
		}
		if( mt_ConfInquire( app, "windom.fsel.mask", "%S", buf)>0 && *buf) {
			strcat( mask, buf);
			strcat( mask, ";");
		}
		if( p && *p) {
			strcat( mask, p);
			strcat( mask, ";");		
		}
		/* conversion for selector */
		nmask = conv_var(mask);
	}

	/* Is FLSX available ? Yes : handle in a window the selector */

	if( app->aes4 & AES4_FSLX && !(CONF(app)->flag & FLG_NOMGXFSEL)) {
		INT16 handle;
		INT16 sortmode = 0;
		void *fslx;
		EVNT ev;
		int evnt_res;
		EVNTvar old;
		WINDOW *win;
		short buff[8];
						
		mt_MenuDisable(app);		
		fslx = mt_fslx_open(
				title,
				-1,-1,
				&handle, /* handle fenetre */
				path, 256,
				name, 65,
				mask,
				0L,			/* kein Filter */
				paths,
				0 			/*SORTBYNAME*/,
				0 			/*GETMULTI */,
				app->aes_global );
		
		/* 'handle' is an external window, we insert it in the WinDom environ ... */
		win = mt_WindAttach( app, handle);
		/* ... mainly to handle it as a modal window :-) */
		mt_WindSet( app, win, WF_BEVENT, BEVENT_MODAL, 0, 0, 0);
		
		old = app->evnt;
		app->evnt.bclick = 2;
		app->evnt.bmask = app->evnt.bstate = 1;
		do {
		 	ev.mwhich = mt_EvntWindomBuff( app, MU_KEYBD|MU_BUTTON|MU_MESAG, buff);
		 	ev.mx = app->evnt.mx;
		 	ev.my = app->evnt.my;
		 	ev.mbutton = app->evnt.mbut;
		 	ev.kstate = app->evnt.mkstate;
		 	ev.key = app->evnt.keybd;
		 	ev.mclicks = app->evnt.nb_click;
		 	
		 	for( dum = 0; dum < 8; dum++)
		 		ev.msg[dum] = buff[dum];
		 	
		 	
			evnt_res = mt_fslx_evnt( fslx, &ev, path, name,
								  &res, &dum, 
								  &sortmode, &p,
								  app->aes_global);
		} while( evnt_res);
		app->evnt = old;
		mt_fslx_close(fslx,app->aes_global);

		/* Destroy the window */
		mt_WindClose( app, win);
		mt_WindDelete( app, win);
		mt_MenuEnable(app);
	} else {
		long cookval;
		if( !get_cookie( MiNT_COOKIE, &cookval)
			&& !get_cookie( MAGX_COOKIE, &cookval))	strupr( ext);
		strcat( path, (*ext)?ext:"*.*");

 		mt_fsel_exinput( path, name, &res, title, app->aes_global);
	}
	p = strrchr( path , '\\');
	*(p+1) = '\0';

	if( paths) {
		free( paths);
		free( mask);
	}
	return res;
}

/* EOF */
