/* -----------------------------------------------------------------
 *	CPX LDG Version 2.22
 *	Ce programme est freeware et fait partie
 *	du package LDG
 *	Tous droits r‚serv‚s … l'auteur (c) 1999
 * -----------------------------------------------------------------
 *	Auteur : Dominique B‚r‚ziat, 1999
 *	Module: utils.h
 *	But : d‚finitions des types, macros et 
 *		  proto des fonctions utiles
 *	Tabulation : 4
 * -----------------------------------------------------------------
 * $Id: global.h 115 2015-05-20 17:50:41Z landemarre $
 * ----------------------------------------------------------------- 
 */

#define __GEMLIB_OLDNAMES	/* Activer les vieux noms GEMLIB */
#include <mt_gem.h>
#include <ldg.h>
#include "xcontrol.h"
#include <backward.h>
#include <internal.h>

#define CPXVERSION "2.34"

/* Types
 */

struct foobar {
	int dummy;
}; /* type inutile requit par le ressource int‚gr‚ */

/* Stuctures utilis‚es par le gestionnaire LDG */

struct lib {
	char name[32];          	/* nom AES de la lib */
	LDG *ldg;               	/* adresse de la LDG */
	short *usedby;     			/* liste des clients */
};

typedef char STR9[9];

struct internal {
	struct lib *libs;  	/* liste des libs */
	STR9	   *apps;	/* nom des clients */
	short	  maxlib;
	short	maxclient;
	short totalclient;
};

/* Sauvegarde du CPX */

typedef struct {
	short  	saved;
	char 	path[128];
	short  	garbage, 
		 	idle;
} CPX_SAVE;

/* Macros
 */
#define TEXT(i)		dialog[(i)].ob_spec.tedinfo->te_ptext
#define min(a,b) ((a<b)?a:b)
#define UNUSED(a) (void)(a)

/* Constantes
 */
#define LDG2_VERS 	0x0220	/* version cookie correspondant … LDG 2.xx */
#define DRAW3D 		0x0080
#define LEN_PATH	20	/* longueur du champs CNF_PATH */
#define TO_TSR	0
#define TO_MGR	1
#define FALSE 	0
#define TRUE 	1


/* Variables globales
 */
extern CPX_PARAMS* params;
extern OBJECT*	   dialog;
extern LDG*		   ldg;
extern GRECT	   cpx_wind;
extern CPX_SAVE	   conf;
extern int		   currform, lastform;
extern int		   cpxid;
extern short	   edit, pos;
extern LDG_INFOS*  cook;
extern char**	   free_string;
extern struct lib* listlib;
extern int		   snd_to;

extern short	   aes_global[];  /* not defined in m_gem.h */


/* Protos fonctions
 */

void snd_request	( int act, int to);
void select_path	( void);
void lib_select		( void);
int  call_menu		( int button, int *lastform, GRECT *work);
int  call_sndto		( int button, int def, GRECT *work);
int  call_action	( int button, GRECT *work, int items);
void list_functions	( int button, GRECT *work);
void bubble_do		( int, int, int);
char*appl_name		( int id);
void copy_clipbd	( void);
void select_manager	( int);

/* Dans utils.c */
void format_filename( char *dest, char *src, int size);
void add_slash		( char *);
void draw_object	( OBJECT *tree, int object);
void draw_tree		( OBJECT *tree, int root);
void get_form		( void);
void select_form	( int form);
void fill_comment	( char *, ...);
void exec_app		( char *app, char *cmd);
int ApplWrite		( int, int, ...);
void editable3d		( OBJECT *);


/* dans evnt.c */
short CDECL cpx_call  ( GRECT *work);
void  CDECL cpx_draw  ( GRECT *clip);
void  CDECL cpx_wmove ( GRECT *work);
void  CDECL cpx_key   ( short kbstate, short key, short *evnt);
void  CDECL cpx_button( CPX_MOUSE_RET *mrets, short nclicks, short *event);
short CDECL cpx_evhook( short ev, short *msgbuff, CPX_MOUSE_RET *mrets, short *key, short *nclicks);
void  CDECL cpx_close ( short);
