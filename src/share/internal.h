/*
 *  LDG 2.xx - module de d‚veloppement
 *	par D.B‚r‚ziat & O.Landemarre.
 *	(c) 2000 - tous droits r‚serv‚s.
 *
 *	La routine - complexe - qui permet d'avoir la
 *	liste internes des libraries. Elle intŠgre toutes les
 *	version de LDG. Elle est utilis‚e par les outils de
 *	maintenance (CPX et showlib) et le manager (la derniŠre
 *	version de compatibilit‚).
 */

struct ldg_lib {
	char name[32];		/* nom AES de la lib */
	LDG *ldg;			/* adresse de la LDG */
	short *usedby;     	/* liste des clients */
	short *ndecl;		/* cumul pour d‚claration multiple */
};


struct ldg_infos {
	void *listlib;		/* r‚serv‚ */
	short current;		/* r‚serv‚ */
	short maxlib;		/* nombre maximale de librairies */
	short maxclient;	/* nombre max de client par lib  */
	short totalclient;	/* nombre total max de client */
	short version;		/* version du gestionnaire */
	short type;			/* type du gestionnaire */
};

#define LDGMGR	1
#define LDGTSR	2

int ldg_mgrlist( struct ldg_infos *infos, short *gl);
int ldg_libslist( struct ldg_infos *infos,  short *gl);
void ldg_initlist( struct ldg_infos *infos);
int ldg_getlib( struct ldg_infos *infos, struct ldg_lib *lib);
int ldg_loadlib( struct ldg_infos *infos);

/* EOF */

