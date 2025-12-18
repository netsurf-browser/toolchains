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
 * $Source: /cvsroot/windom/windom/src/bubble_modal.c,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.9 $
 */

#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "cookie.h"

/**
 * @brief Display a bubble help in a classic formular
 *
 * @param app application descriptor,
 * @param hlp pointeur to the string to display in a bubble
 * @param mx,my coordinates of the bubble (use the mouse position) 
 * @return 0 (success) or a negative error code :
 *
 * mt_BubbleModal() is the equivalent function of mt_BubbleCall() function
 * in the case of classic formulars (i.e. a formular stopping AES
 * events). This function works only from BubbleGEM R05. With former
 * version of BubbleGEM, it is not possible to call BubbleGEM from
 * a classic formular.
 *
 * If BUBBLE.APP is not present in memory, mt_BubbleModal() is not able
 * to load it (because AES is stopped). So the BubbleGEM daemon can
 * be launched prevously with the BubbletGet() function.
 *
 * Because the form_do() function can not handle the right mouse
 * button event, WinDom offers an alternative function to display
 * systematically bubbles with mt_BubbleAttach() and mt_BubbleEvnt() functions :
 * it is the mt_BubbleDo() function. This function is not universal, for
 * custom usage, write your own mt_BubbleDo() function (looking at the
 * mt_BubbleDo() source code. This function is simple, it uses the
 * functions of the AES form library.
 *
 * A complet example is given is the folder EXAMPLES/BUBBLE of the
 * WDK package.
 * 
 * @sa mt_BubbleGet(), mt_BubbleDo(), mt_BubbleAttach().
 */

int mt_BubbleModal( APPvar *app, char *hlp, int mx, int my) {
	BGEM *bgem;
	BHLP bhlp;
	int bmask, bstate, bclicks, ev;
	int bubble_id = mt_appl_find( "BUBBLE  ", app->aes_global);
	short buff[8];
	
	static MFORM help_form = { 0, 0, 1, 0, 1,
		{-16130,-24125,-28287,-30311,-31247,-31773,-32313,-32625,-32743,-31871,-27709,-22017,-13849,-31513,1278,896},
		{0,16444,24702,28774,30734,31772,32312,32624,32742,31870,27708,17920,1560,792,768,0}
	};

	if( bubble_id == -1)
		return -1;

	if( get_cookie( BGEM_COOKIE, (long*)&bgem)) {
		/* TODO: use a better Galloc function, with lock, auto garbage after timeout... */
		if( app->pipe == NULL) return -3;
		strcpy( app->pipe, hlp);
		
 		/* RequŠte fonte */
 		mt_ApplWrite( app, bubble_id, BUBBLEGEM_FONT, CONF(app)->bubble.font, CONF(app)->bubble.size, 0, 0, 0);

		/* Envoi message a Bubble */
		mt_ApplWrite( app, bubble_id, BUBBLEGEM_SHOW, mx, my, ADR(app->pipe), BGS7_USRHIDE);
		mt_evnt_timer( 10L, app->aes_global);

		/* en mode modal, … nous de g‚rer certaines choses ... */
		mt_graf_mkstate( NULL, NULL, &app->evnt.mbut, NULL, app->aes_global);
        if( (app->evnt.mbut & 0x3) == 0) {
			bclicks = 258;
			bmask = 3;
			bstate = 0;
		} else {
			bclicks = 0;
            bmask = 3;
            bstate = 0;
		}
		/* set the mouse form */
		mt_graf_mouse(USER_DEF,&help_form,app->aes_global);
     	ev = mt_evnt_multi( MU_KEYBD|MU_BUTTON|MU_M1, 
     					 bclicks, bmask, bstate, 
     					 1, mx-6, my-6, 13, 13, 
     					 0, 0, 0, 0, 0, buff,
						 0L,
          				 &app->evnt.mx, &app->evnt.my,  &app->evnt.mbut, 
          				 &app->evnt.mkstate, &app->evnt.keybd, 
          				 &app->evnt.nb_click,
						 app->aes_global);
        		/* attente */
        if( !get_cookie( BHLP_COOKIE, (long*)&bhlp))
       		bhlp.delay = 200;
		mt_evnt_timer( bhlp.delay, app->aes_global);
		/* fin protocole */
        mt_ApplWrite( app, bubble_id, BUBBLEGEM_HIDE, 0, 0, 0, 0, 0);
		if( !(app->aes4 & AES4_XGMOUSE) || mt_graf_mouse( M_PREVIOUS, 0L, app->aes_global) == 0 )
			mt_graf_mouse( ARROW, NULL, app->aes_global);
        /* Restore config */
        if( ev & MU_KEYBD)
        	mt_ApplWrite( app, mt_AppId(app), AP_KEYBD, app->evnt.mkstate, app->evnt.keybd, 0, 0, 0);
        if( ev & MU_BUTTON)
       		mt_snd_mu_button( app, app->evnt.mbut, app->evnt.nb_click);
        return 0;
	} else {
		/* error: this requires bubble gem deamon to work. 
		 * the deamon exists since bubble 06, and the cookie 'BGEM' has appeared
		 * with bubble 06, so the existence of the deamon is checked using the
		 * check of the BGEM cookie
		 */
		return -4;
	}
}
