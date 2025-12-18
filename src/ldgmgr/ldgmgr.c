/* ---------------------------------------------------------
 *	Librairie Dynamique GEM
 *	Olivier Landemarre, Dominique B‚r‚ziat, Arnaud Bercegeay
 *	tous droits r‚serv‚s 1997-2003
 * ---------------------------------------------------------
 * Source of LDG manager.
 *
 * In LDG version 1.xx, the LDG manager was the core of LDG
 * system. In version 2.xx, it is obsolet because the job is
 * performed more efficiently by the LDG TSR.
 *
 * However, some old clients using LDG 1.xx needs it to
 * work.
 * This manager emulates completly the LDG manager using
 * directly functions of the LDG TSR and allows to old LDG
 * client to work and share libraries with LDG 2.xx.
 * ----------------------------------------------------------
 * Current version is 2.00 (previous was 1.23).
 *
 * $Id: ldgmgr.c 92 2012-05-12 18:31:26Z landemarre $
 */

#include <gem.h>
#include <stdio.h>
#include <stdarg.h>
/*#include <av.h>*/
#define VA_START 0x4711

#include "..\..\include\ldg.h"
#include "..\version.h"
#include "..\share\backward.h"   /* support of previous LDG version */
#include "..\share\internal.h"   /* structure of internal lists
						          * held by the LDG TSR */
#define NEXT_EVNT	1
#define QUIT_MGR	2
/*#define TIMER(a)	(INT16)a,(INT16)(a>>16) */
#define TIMER(a)	a

extern short _app;

/*
 * Variables globales 
 */

short  apid, gl[16];
struct ldg_infos intern;

void ApplWrite( int to, int msg, ...) {
	va_list args;
	short msgbuf[8], i;
	msgbuf[0] = msg;
	msgbuf[1] = apid;
	msgbuf[2] = 0;
	va_start( args, msg);
	for( i=3; i<8; msgbuf[i++] = va_arg( args, short));
	va_end( args);
	appl_write( to, 16, msgbuf);
}

void show_info( void) {
	char alrt[200];
	
	sprintf( alrt, "[1][LDG manager " MGR_VERSION_TEXT "|"
					   "%d libs used|"
					   "Max libs: %d|"
					   "Max apps: %d|"
					   "Max apps per lib: %d][Ok]",
				 	ldg_loadlib( &intern), 
				 	intern.maxlib, 
				 	intern.totalclient, 
				 	intern.maxclient);
	form_alert( 1, alrt);
}


int main( void) {
	INT16 res, dum, run;
	INT16 msg[8];
	LDG *ldg;
	LDG_INFOS *cook;
	long garbage;
	
	apid = appl_init();

	if( ldg_cookie( LDG_COOKIE, (long*)&cook) && cook->version >= 0x0220) {
		garbage = cook->garbage*1000L;
		ldg_libslist( &intern, ldg_global);
	} else {
		form_alert( 1, "[1]["
						"     Warning, this manager works only|"
						"with LDG 2 (ie a TSR version 2.20 at least|"
						"         is required).][OK]");
		appl_exit();
		return 0;
	}

	for( dum=0; dum<16; dum++)
		gl[dum] = ldg_global[dum];
  
	if( ldg_global[1] == -1 || !_app) 
		menu_register( apid, "  LDG Manager");

  	/* gestion des ‚v‚nements */
  	run = NEXT_EVNT;
  	do {
  		msg[0] = 0;
		res = evnt_multi( MU_MESAG | MU_TIMER*(garbage!=0),
  						  0,0,0,
				    	  0,0,0,0,0,
						  0,0,0,0,0,
						  msg,
						  TIMER(garbage),
						  &dum,&dum,&dum,
						  &dum,&dum,&dum);
		if( res & MU_MESAG) {
			switch( msg[0]) {
			case LDG_REQUEST:
				gl[2] = msg[1];
				ldg = (*cook->ldg_open) ( *(char**) &msg[5], gl);
				if( ldg) {
					long *ldg_pere, *val_pt;
					/* Avec la version LDG 2.00 ce champs n'‚tait plus
					 * utilis‚. Il est toutefois utilis‚ par les clients
					 * LDG 1.xx pour connaitre le destinataire du message
					 * LDG_TERM */
					ldg->id = apid;
					/* on transmet l'adresse du module */
					val_pt = (long *) &msg[3];
					ldg_pere = (long *) *val_pt;
					*ldg_pere= (long) ldg;
      			} else
      				ApplWrite( msg[1], LDG_ERROR, cook->error, msg[5], msg[6], 0, 0);
 
				break;
			case LDG_TERM:
				gl[2] = msg[1];
				(*cook->ldg_close)( *(LDG**)&msg[3], gl);
				break;
			case LDG_GARBAGE:
				(*cook->ldg_garbage)( ldg_global);
				if( ldg_loadlib( &intern) == 0)
					run = QUIT_MGR;
				break;
			case VA_START: 
				show_info();
				break;
			case AP_TERM:
				run = QUIT_MGR;
				break;
			/* case LDG_QUIT: impossible venant d'une lib */
			case LDG_LIST:
				ApplWrite( msg[1], LDG_LIST, 
							intern.listlib,
							intern.maxlib,
							intern.maxclient,
							MGR_VERSION_NUMBER);
				break;
			case LDG_FORCE:
				/* Dans LDG 2, on ne sait pas forcer la fin d'une
				 * librarie pr‚cise */
				(*cook->ldg_reset)( ldg_global);
				break;
			}
		}
		if( res & MU_TIMER) ApplWrite( apid, LDG_GARBAGE);
		
  	} while(  run != QUIT_MGR); 
	appl_exit();
  	return 0;
}

/* EOF */
