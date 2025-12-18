/* -----------------------------------------------------------------
 *	CPX LDG Version 2.22
 *	Ce programme est freeware et fait partie
 *	du package LDG
 *	Tous droits r‚serv‚s … l'auteur (c) 1999/2000
 * -----------------------------------------------------------------
 *	Auteur : Dominique B‚r‚ziat
 *	Module : main.c
 *	But : section .TEXT, variables globales et d‚marrage du CPX 
 *  Tabulation : 4
 * -----------------------------------------------------------------
 * $Id: main.c 27 2005-09-10 21:21:38Z bereziat $
 * -----------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include "global.h"

/* Section .TEXT : sert de zone inscriptible
 * par Xcontrol pour les sauvegardes du CPX.
 * Le champ version sert d'indicateur pour savoir si
 * le CPX a d‚j… ‚t‚ sauv‚ */

/* Valeurs par defaut du CPX et donc du TSR */

CPX_SAVE conf = { FALSE, "C:\\gemsys\\ldg\\", 30, 1000};

/* external resource must be included after CPX data */
#include "resource.h"

/* Variables globales
 */
#ifdef NOSTARTUP
/* makes the compiler happy */
int errno;
int _FilSysV;
#endif

CPX_PARAMS*	params;
GRECT		cpx_wind;
OBJECT*		dialog;
LDG_INFOS*	cook = NULL;
LDG*		ldg = NULL;
int			currform, lastform;
int			cpxid = -1;
int			request_listlib = 0;
int			snd_to = TO_TSR;
struct lib* listlib = NULL;
char**		free_string;
short		edit, pos=0;
struct ldg_infos	intern;

/*
 *	main : Init Cpx : first fonction called by xcontrol
 */

CPX_INFO *CDECL main( CPX_PARAMS *par) {
  static CPX_INFO info = { cpx_call, cpx_draw, 
			   cpx_wmove, 0L,
			   cpx_key, cpx_button, 0L, 0L, 
			   cpx_evhook, cpx_close};

  /* debug : curseur en haut de l'‚cran */
  /* Cconws( "\33H\r\n\r\n"); */

  if( !(*par->find_cookie)( LDG_COOKIE, (long*)&cook))
    cook = NULL;
  
  /* Au boot de Xcontrol */
  if ( par->booting ) {

    /* Le CPX installe ses r‚glages */
    if( cook && conf.saved ) {
      strcpy( cook->path, conf.path);
      cook->garbage = conf.garbage;
      cook->idle    = conf.idle;
    }

    return (CPX_INFO*)1L;
  } else {
    /* Ouverture du CPX */
    params = par;
    /* On fixe le ressource ? */
    if( !params->rsc_init ) {
      int i;
      for( i=0; i<NUM_OBS; (*params->objc_adjust)( rs_object, i++));
    }
    
    /* C'est un Event-CPX ! */
    (*par->multi)( MU_MESAG|MU_KEYBD|MU_BUTTON, NULL, NULL, -1);
    /* On retourne les fonctions ‚v‚nements : le CPX est prˆt … s'ouvrir */
    return &info;
  }
}

/*  Lorsque le CPX est ouvert dans Xcontrol,
 *  ce dernier appelle cette fonction.
 */

short CDECL cpx_call( GRECT *work) {
  dialog = (OBJECT*)rs_trindex[CPXFORM];
  editable3d( dialog);

  select_form( CONFIGURE);
  dialog->ob_x = work->g_x;
  dialog->ob_y = work->g_y;
  mt_objc_draw( dialog, ROOT, MAX_DEPTH, 
		work->g_x, work->g_y, work->g_w, work->g_h, aes_global);
  cpx_wind = *work;
  free_string = rs_frstr;
  return 1;
}
