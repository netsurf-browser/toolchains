/*
 * Backward compatibility with LDG 1.xx
 * These messages were used by LDG 1 and are
 * obsolete now. They are used by tools (CPX, LDGINFO, LDGMGR)
 * to support LDG 1.xx.
 */

#define LDG_REQUEST 	0x4C45  /* demande d'infos … la librairie (client->ldg)*/
#define LDG_TERM       	0x4C47  /* la librairie quitte (ldg->client) */
							    /* le client n'utilise plus la lib (client->ldg) */
#define LDG_GARBAGE    	0x4C48  /* m‚canisme de garbage collector */
#define LDG_TERM       	0x4C47  /* la librairie quitte (ldg->client) */
							    /* le client n'utilise plus la lib (client->ldg) */
#define LDG_ERROR      	0x4C49  /* erreur de d‚claration */
#define LDG_QUIT       	0x4C4A  /* message interne aux libs */
#define LDG_LIST       	0x4C4B  /* la lib retourne la liste de ses clients */
#define LDG_FORCE       0x4C4C  /* forcer la lib‚ration des librairies */

