/* -----------------------------------------------------------------
 *	CPX LDG Version 2.01
 *	Ce programme est freeware et fait partie
 *	du package LDG
 *	Tous droits r‚serv‚s … l'auteur (c) 1999
 * -----------------------------------------------------------------
 *	Auteur : Dominique B‚r‚ziat, 1999
 *	Module: bubble.c
 *	But : gestion des bulles d'aide (BubbleGEM)
 *	Tabulation : 4
 * -----------------------------------------------------------------
 * $Id: bubble.c 27 2005-09-10 21:21:38Z bereziat $
 * -----------------------------------------------------------------
 */

#include "ldgcpx.h"
#include <gem.h>
extern OBJECT *dialog;

int BubbleGet( void);						/* WinDom */
int BubbleCall( char *help, int x, int y);	/* WinDom */

static char *bubble_help[] = {
	/* 0 */
	"Apply the settings and save the CPX configuration",
	"Close CPX without apply the settings",
	"Close CPX and apply the settings",
	"Select a topic:|"
		 "configure: system configuration|"
		 "library: infos about *.LDG files|"
		 "expert: some experts actions|"
		 "Back.Comp.: compatibility with LDG 1.xx|"
		 "informations: infos about this program",
	"Display the 'LDGM' cookie version",
	/* 5 */
	"Define the path where are stored the libraries.",
	"Set the garbage collector time (in min) (if LDGGARB.APP is resident or if LDG 1.xx is used).",
	"Set the idle time (in msec) (for LDG 1.xx only).",
	"Display the version of the LDG-manager.",
	"Display how many libs are currenlty loaded by the LDG-manager.",
	"Choice a request to send to the LDG_manager.",
	"Select a library and display infos about it.",
	"Display the version of the library.",
	"A popup containing the list of the functions available in the library."
	"Indicates if the library may be shared with severals clients",
	"Indicates if the library is busy (by a client) and can be used",
	"Indicates if the library is memory-resident",
	"Indicates if the library have a closure-function",
	"Indicates a personal extension of the library",
	"Indicates if the library have an extension (a new LDG version)",
	"Display the version of the LDG protocol used by the library"
};

void bubble_do( int button, int mx, int my) {
	int idx = -1;
		
	switch( button) {
	case CNF_SAVE:
		idx = 0;
		break;
	case CNF_CANCEL:
		idx = 1;
		break;
	case CNF_OK:
		idx = 2;		
		break;
	case CNF_MENU:
		idx = 3;
		break;
	case CNF_VERSION:
		idx = 4;
		break;
	case CNF_PATH:
		idx = 5;
		break;
	case CNF_GARBAGE:
		idx = 6;
		break;

	/* Formulaire Expert actions */

	/* Formulaire BackWard Compatibility */

	case EXP_IDLE:
		idx = 7;
		break;
	case MGR_VERSION:
		idx = 8;
		break;
	case MGR_NLIB:
		idx = 9;
		break;
	case MGR_ACTION:
		idx = 10;
		break;

	/* Formulaire library */

	case LIB_SELECT:
		idx = 11;
		break;
	case LIB_VERSION:
		idx = 12;
		break;
	case LIB_LIST:
		idx = 13;
		break;
	case LIB_SHARE:
		idx = 14;
		break;
	case LIB_LOCKED:
		idx = 15;
		break;
	case LIB_RESIDENT:
		idx = 16;
		break;
	case LIB_CLOSURE:
		idx = 17;
		break;
	case LIB_EXT:
		idx = 18;
		break;
	case LIB_SPEC:
		idx = 19;
		break;
	case LIB_PROTOCOL:
		idx = 20;
		break;
	}

	if( idx != -1) BubbleCall( bubble_help[idx], mx, my);
}

