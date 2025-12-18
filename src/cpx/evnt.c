/* -----------------------------------------------------------------
 *	CPX LDG Version 2.10
 *	Ce programme est freeware et fait partie
 *	du package LDG
 *	Tous droits r‚serv‚s … l'auteur (c) 1999
 * -----------------------------------------------------------------
 *	Auteur : Dominique B‚r‚ziat, 1999
 *	Module: evnt.c
 *	But : gestion des ‚v‚nements du CPX 
 *	Tabulation : 4
 * -----------------------------------------------------------------
 * $Id: evnt.c 27 2005-09-10 21:21:38Z bereziat $
 * -----------------------------------------------------------------
 */

#include <scancode.h>
#include <stdlib.h>
#include <av.h>
#undef NULL
#include <stdio.h>
#include <string.h>
#include "global.h"
#include "ldgcpx.h"

/*
 *	‚v‚nement WM_REDRAW du CPX
 */

void CDECL cpx_draw( GRECT *clip) {
	GRECT *xrect;
	
	xrect = (*params->rci_first)( clip);
	while( xrect) {
		mt_objc_draw( dialog, ROOT, MAX_DEPTH, 
			      xrect->g_x, xrect->g_y, xrect->g_w, xrect->g_h, aes_global);
		xrect = (*params->rci_next)();
	}
}

/*
 *	D‚placement de la fenˆtre
 */

void CDECL cpx_wmove( GRECT *work) {
	dialog->ob_x = work->g_x;
	dialog->ob_y = work->g_y;
	cpx_wind = *work;
}

/*
 *	Gestion des clicks souris sur les objets,
 *	sous fonction de cpx_button()
 *	retour : 1 : quitter le CPX, 0 sinon
 */

int do_button( int button) {
	int res, old;
	char *p;
	
	switch( button) {
	/* Les boutons GLOBAUX */
	case CNF_OK:
		if( currform == COMMENT) {
			/* Retour au formulaire pr‚c‚dent */
			select_form( lastform);
			draw_tree( dialog, 0);
			break;
		} else {
			/* Valider les r‚glages et quitter */
			get_form();
			return 1;
		}
	case CNF_SAVE:
		/* Valider les r‚glages et sauver */
		if( currform != COMMENT && (*params->alert)(0) ) {
			get_form();
			conf.saved = TRUE;
			(*params->write_config)( &conf, sizeof( CPX_SAVE));
		}
		break;
	case CNF_CANCEL:
		if( currform != COMMENT )
			/* Juste quitter sans valider */
			return 1;
	case CNF_MENU:
		/* Appel du popup MENU & affichage du formulaire */
		if( currform == COMMENT )break;
		old = currform;
		res = call_menu( button, &currform, &cpx_wind);
		if( res != -1 && old != currform) {
			select_form( currform);
			draw_object( dialog, currform);
		}
		break;

	/* Formulaire CONFIGURE */
		
	/* Appel du s‚lecteur de fichier */
	case CNF_PATH:
		select_path();
		draw_object( dialog, CNF_PATH);
		break;

	/* Formulaire Expert */
	case EXP_TO:
		res = call_sndto( button, snd_to, &cpx_wind);
		if( res >= 0) {
			snd_to = res;
			strcpy( TEXT(EXP_TO), res?"manager":"tsr");
			draw_object( dialog, EXP_TO);
			select_manager( snd_to);
			draw_object( dialog, EXP_LIBLOADED);
		}
		break;

	case EXP_REQUEST:
		snd_request( call_action( button, &cpx_wind, 
								  (snd_to == TO_MGR)?5:4), snd_to);
		break;
	
	/* Formulaire LIB */
	case LIB_SELECT:
		lib_select();
		break;
	case LIB_LIST:
		list_functions( button, &cpx_wind);
		break;

	/* Formulaire INFOS */
	case INF_DOC:
	  ApplWrite( mt_appl_find( "ST-GUIDE", aes_global), VA_START, "*:\\LDG.HYP");
		break;
	case URL_LDG:
	  if( mt_shel_envrn( &p, "BROWSER=", aes_global)) {
			exec_app( p, TEXT(URL_LDG));
		/*	ApplWrite( appl_find( p), VA_START, "*:\\LDG.HYP"); */
		}
		break;

	/* Formulaire COMMENT */
	case COM_CLIP:
		copy_clipbd();
		draw_object( dialog, COM_CLIP);
		break;
	}	

	if( currform != COMMENT)
		draw_tree( dialog, button);	
	return 0;
}

/*
 *	‚v‚nement MU_BUTTON
 */

void CDECL cpx_button( CPX_MOUSE_RET *mrets, short nclicks, short *event) {
	short button, dum;

	button = mt_objc_find( dialog, ROOT, MAX_DEPTH, mrets->mx, mrets->my, aes_global);
	if( button != -1 && mrets->mbutton & 0x1) {
	  if( !mt_form_button( dialog, button, nclicks, &dum, aes_global)) {
			/* d‚s‚lection du bouton */
			dialog[button].ob_state &= ~SELECTED;
			/* Gestion des objets */
			*event = do_button( button);
		}
		if( dialog[button].ob_flags & EDITABLE && button != edit) {
		  mt_objc_edit( dialog, edit, 0, &pos, ED_END, aes_global);
			edit = button;
			pos = 0;
			mt_objc_edit( dialog, edit, 0, &pos, ED_INIT, aes_global);			
		}
	} else if( button != -1 && mrets->mbutton & 0x2) {
		bubble_do( button, mrets->mx, mrets->my);
	}
}

/*
 *	‚v‚nement MU_KEYBD
 */

void CDECL cpx_key( short kstate, short scancode, short *evnt) {
	short key=0, button, res, old;
	int asc = scancode >> 8;

	UNUSED(kstate);
	switch(  asc ) {
	/* Commandes g‚n‚rales */
	case SC_RETURN:
	case SC_ENTER:
		if( currform == COMMENT) {
			select_form( lastform);
			draw_tree( dialog, 0);
			break;
		} else {
			get_form();
			*evnt = 1;
		}
		break;
	case SC_UNDO:
		if( currform == COMMENT) break;
		*evnt = 1;
		break;
	case SC_INSERT:
		if( currform == COMMENT) break;
		if( (*params->alert)(0) ) {
			get_form();
			conf.saved = TRUE;
			(*params->write_config)( &conf, sizeof( CPX_SAVE));
		}
		break;
	case SC_HELP:
	  ApplWrite( mt_appl_find( "ST-GUIDE",aes_global), VA_START, "*:\\LDG.HYP");
		break;
	case SC_SPACE:
		if( currform == COMMENT) break;
		old = currform;
		res = call_menu( CNF_MENU, &currform, &cpx_wind);
		if( res != -1 && old != currform) {
			select_form( currform);
			draw_object( dialog, currform);
		}		
		break;

	/* Raccourci clavier du popup g‚n‚ral */
	case SC_F1:
		if( currform != CONFIGURE) {
			select_form( CONFIGURE);
			draw_tree( dialog, CONFIGURE);
		}
		break;
	case SC_F2:
		if( currform != LIBRARY) {
			select_form( LIBRARY);
			draw_tree( dialog, LIBRARY);
		}
		break;
	case SC_F3:
		if( currform != EXPERT) {
 	 		select_form( EXPERT);
			draw_tree( dialog, EXPERT);
		}
		break;
	case SC_F4:
		if( currform != INFORMATION) {
			select_form( INFORMATION);
			draw_tree( dialog, INFORMATION);
		}
		break;

	/* Touches de fonction d‚pendant du dialogue courant */
	case SC_F6:
		switch( currform) {
		case CONFIGURE:
			select_path();
			draw_object( dialog, CNF_PATH);
			break;
		case LIBRARY:
			lib_select();
			break;
		case EXPERT:
			snd_request( call_action( EXP_REQUEST, &cpx_wind, 4), snd_to);
			break;
		}

	case SC_F7:
		switch( currform) {
		case LIBRARY:
			list_functions( LIB_LIST, &cpx_wind);
			break;
		case EXPERT:
			res = call_sndto( EXP_TO, snd_to, &cpx_wind);
			if( res >= 0) {
				snd_to = res;
				strcpy( TEXT(EXP_TO), res?"manager":"tsr");
				draw_object( dialog, EXP_TO);
				select_manager( snd_to);
				draw_object( dialog, EXP_LIBLOADED);
			}
			break;
		}
		break;

	/* Dialogue expert */

	case SC_F11:
	case SC_F12:
	case SC_F13:
	case SC_F14:
	case SC_F15:
		if( currform == EXPERT)
			snd_request( asc-SC_F11, snd_to);
		break;
	case SC_F16:
	case SC_F17:
		if( currform == EXPERT) {
			snd_to = (asc == SC_F16)?TO_TSR:TO_MGR;
			strcpy( TEXT(EXP_TO), snd_to?"manager":"tsr");
			draw_object( dialog, EXP_TO);
			select_manager( snd_to);
			draw_object( dialog, EXP_LIBLOADED);
		}
		break;
	
	/* Dialog Comment */
	case 0x2C:
		copy_clipbd();
		draw_object( dialog, COM_CLIP);
		break;
	default:
		/* gestion formulaire */	
	  if( !mt_form_keybd( dialog, (edit==-1)?0:edit, 1, scancode, &button, &key,aes_global))
			do_button( button);
		if( key) 
		  mt_objc_edit( dialog, edit, key, &pos, ED_CHAR, aes_global);
		else {
		  	mt_objc_edit( dialog, edit, 0, &pos, ED_END, aes_global);
			edit = button;
			mt_objc_edit( dialog, edit, (int)strlen(TEXT(edit)), &pos, ED_INIT, aes_global);
		}
	}
}

/*
 * Autres ‚v‚nements
 */
 
short CDECL cpx_evhook( short ev, short *msgbuff, CPX_MOUSE_RET *mrets, 
					    short *key, short *nclicks) {

	UNUSED(nclicks);
	UNUSED(mrets);
	UNUSED(key);

	if( ev & MU_MESAG) {
		if( cpxid == -1) cpxid = aes_global[2] = msgbuff[1];
		switch( msgbuff[0]) {
		
		/* Message qui indique la fin volontaire d'une lib */
		case LDG_QUIT:
			{
				char alrt[80], *p;
				LDG *ldg = *(LDG **)&msgbuff[3];
				
				p = strrchr( ldg->path, '\\');
				sprintf( alrt, "[][Warning: Library|%s|has finished|raison=%d][OK]",
								p?p+1:ldg->path, msgbuff[5]);
				mt_form_alert( 1, alrt, aes_global);			
			}
			break;
		}
	}
	return 0;
}

void CDECL cpx_close( short flag) {
	if( ldg) {
		/* ldg_close( ldg); */
		ldg_term( cpxid, ldg);
	}
	if( flag) get_form();
}
