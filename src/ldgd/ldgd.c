/*
 *	LDG Deamon	1.10
 *	For auto garbage collecting
 *	(c) 2002 D. B‚r‚ziat, O. Landemarre, A. Bercegeay
 * 
 * version 1.0 : first version handling auto garbage collecting.
 * version 1.1 : support of LDG_SHM_REQUEST message (see HISTORIC for
 *               explanation).
 * version 1.2 : support for GCC
 */

#ifdef __PUREC__
#include <tos.h>
#endif

#if defined(__GNUC__) || defined(__SOZOBONX__)
#include <osbind.h>
#include <mintbind.h>
#endif

#include <gem.h>
#include "..\..\include\ldg.h"
#include <stdio.h>

#include "..\version.h"
#include "..\options.h"

#ifndef VA_START
# define VA_START 18193
#endif

#ifndef SHMSETBLK
# define SHMSETBLK 0x4D01
# define SHMGETBLK 0x4D00
#endif

void *block_ldg[MAX_LIB];
void *mem_ldg  [MAX_MEM];

int main( void) {
	LDG_INFOS *cook;
	char path[50];
	int fd;

	if( ldg_cookie( 'LDGM', (long*)&cook) && cook->version >= 0x0220) {
		short ev, dum;
		long timer=1000;
		short msg[8];
		
		appl_init();
		for( ;; ){

			timer = cook->garbage?(1000*cook->garbage):10000;
		 
			ev = evnt_multi( MU_MESAG /*+ MU_TIMER*/,
  						  	 0,0,0,
				    	  	 0,0,0,0,0,
						     0,0,0,0,0,
						  	 msg, timer,
						   	 &dum,&dum,&dum,
						   	 &dum,&dum,&dum);
			if( ev & MU_TIMER && cook->garbage != 0 && appl_find( "LDGMGR  ") == -1)
				/* form_alert( 1, "[1][Garbage][OK]"),*/
				(*cook->ldg_garbage)( ldg_global);
			if( ev & MU_MESAG) {
				if( msg[0] == AP_TERM)
					break;  /* sortie de la boucle for() */
				else if( msg[0] == VA_START) 
					form_alert( 1, "[1][Deamon for LDG 2.xx|version " 
									LDGD_VERSION_TEXT 
									"][OK]");
				else if( msg[0] == LDGD_SHM_REQUEST) {
					/* R‚servation d'un LDG ou un bloc m‚moire */
					switch( msg[3]) {
						case LDGD_SHARE_LDG: 
						/* LDG r‚sident */	
      					sprintf( path, LDG_SHM_FILENAME, msg[4]);
      					fd = (int) Fopen( path, FO_RW);
      					if( fd >= 0L) {	
      						char *blk = (char *) Fcntl( (int)fd, 0L, SHMGETBLK);
      						block_ldg[msg[4]] = blk;
      						(void) Fclose( (int)fd);
      					}
      					break;
      				case LDGD_SHARE_MEM: 
      					/* m‚moire multiutilisateur */
      					sprintf( path, MEM_SHM_FILENAME, msg[4]);
      					fd = (int) Fopen( path, FO_RW);
      					if( fd >= 0L) {
      						char *blk = (char *) Fcntl( (int)fd, 0L, SHMGETBLK);
      						mem_ldg[ msg[4] ] = blk;
      						(void) Fclose( (int)fd);
      					}
						break;
						case LDGD_UNSHARE_LDG: 
						/* LDG r‚sident a quitter */
							Mfree( block_ldg[ msg[4] ]); 
						/* Attention sous Magic<6.20 ce bloc sera detruit pas sous Mint 
						 * donc a envoyer uniquement en dernier lieu quand on veut effacer 
						 */
						break;
						case LDGD_UNSHARE_MEM: 
						/* m‚moire multiutilisateur a quitter */
							Mfree( mem_ldg[ msg[4] ]); 
						/* attention sous Magic <6.20 ce bloc sera d‚truit pas sous Mint 
						 * donc a envoyer uniquement en dernier lieu quand on veut effacer 
						 */
						break;
						
					}
				}	
			}
		} 
		appl_exit();
	}
	return 0;
}

