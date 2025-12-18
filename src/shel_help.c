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
 * $Source: /cvsroot/windom/windom/src/shel_help.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.2 $
 */


#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "av.h"

/** Interface to the AES helper service
 *	
 *  @param app application descriptor
 *  @param file help file to display (or any regular file)
 *  @param chapter string to find in file
 *  @return 1 if no error 0 otherwise.
 *
 *	mt_ShelHelp() uses shel_help() function to call the AES
 *  helper program if this service is available (NAES). Otherwise,
 *  mt_ShelHelp() checks the environ variable $STGUIDE then
 *  which should point to the st-Guide program (full path) or
 *  an equivalent program.
 *
 *  \par Examples (with ST-Guide as helper service):
@code
	// Display in ST-Guide a simple text file
	ShelWrite( "C:\\NEWDESK.INF", "");
	// Find the WINDOM.HYP file in ST-Guide paths and display it
	ShellWrite( "*:\\WINDOM.HYP", "");
	// Find in the WINDOM.HYP file the ShelWrite() reference and display it
	ShelWrite( "*:\\WINDOM.HYP", "ShelWrite()");
	// Find in the ST-Guide indexes a simple reference
	ShelWrite( "", "A simple reference");
@endcode
 * \todo an emulation of shel_help() when service is not available rather
 *       than use ST-GUIDE.
 */

int mt_ShelHelp( APPvar *app, char *file, char *chapter) {
  short out1, out2, out3, out4;

  /* check if shel_help() is present */
  if( mt_appl_getinfo( AES_NAES, &out1, &out2, &out3, &out4, app->aes_global) == 1
      && out3 == 1) {
    if( mt_shel_help( SHP_HELP, file, chapter, app->aes_global) == 1)
      return 1;
  } else {
    /* shel_help not present or fails */
    char *p;
    int res;
   
   /* comes from windom config */
   /* char helpserver[] = "hyp;C:\\gemsys\\acc\\st-guide.acc;"
      "c;C:\\gemsys\\acc\\st-guide.acc;"
      "htm;h:\\gemapps\\internet\\highwire\\highwire.app";
   */

   strcpy( app->pipe, file);
   if( *file && *chapter) strcat( app->pipe, " ");
   if( *chapter) strcat( app->pipe, chapter);

   /* This piece of code is still buggy ...

   // first : emulate shel_help()
   if( helpserver) {
     p = strrchr( file, '.'); // gets file extension
     if( p) {
       char *q, *r;
       
       p ++;
       q = strstr( helpserver, p); 
       if( q && *(r=q+strlen(p)) == ';'
	   && (q==helpserver || *(q-1) == ';') ) {
	 // extension found and OK 
	 q = strchr(r+1, ';');
	 if( q) *q = '\0';
	 res = mt_ShelWrite( app, r+1, app->pipe, NULL, TRUE, FALSE);
	 printf( "debug: emulates shel_help, extension supported,launch %s, returns %d\n",
		 res,r+1);
	 if( q) *q = ';';
	 if( res >= 0) return 1;
       }
     }
   } 
   ... end of buggy code */
   
   /* if no helpserver - try ST-Guide */
   
   mt_shel_envrn( &p, "STGUIDE=", app->aes_global);
   if( p) {
     res = mt_ShelWrite( app, p, app->pipe, NULL, TRUE, FALSE);
     if (res>0 || res == -2) return 1;
   } 
 }
 return 0;
}

