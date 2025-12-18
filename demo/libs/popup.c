/*
 * popup.c : a basic GEM popup as an example of GEM LDG programmation.
 * Author : Dominique Béréziat
 * Require :  modern version of GEMLIB (pl43 for example)
 * Compile on Pure C, GCC and Sozobon.
 *
 * last modif : adaptation to LDG 2.31, adding stuff to compile with SpareMint
 */

#include <gem.h> /* GEM library is no longer defined by ldg.h */
#include <ldg.h>
#include <stdlib.h>

/* Macros */
#define max(a,b)	(((a)>(b))?(a):(b))

/* Prototypages */
char* __CDECL __infos	( void);
void  __CDECL __get_bkgr( long handle, long of_x, long of_y, long of_w, long of_h, MFDB *img);
void  __CDECL __put_bkgr( long handle, long of_x, long of_y, long of_w, long of_h, MFDB *img);
short __CDECL __popup	( OBJECT *menu, long x, long y, long item, short *gl);

/* LDG variable */
PROC Proc[]  = {
  "__infos",    "/* Returns information on the LDG library */\n"
  "char *information( void);\n", (void *)__infos,
  
  "__get_bkgr", "/* Copy a screen area in a buffer */\n"
  "void (*_get_bkgr)( long handle, long of_x, long of_y, long of_w, long of_h, MFDB *img);\n", 
  (void *)__get_bkgr,
  "__put_bkgr", "/* Restore a screen area from a buffer */\n"
  "void (*_put_bkgr)( long handle, long of_x, long of_y, long of_w, long of_h, MFDB *img);\n", 
  (void *)__put_bkgr,
  "__popup",    "/* Call of the GEM popup */\n"
  "short (*_popup)( OBJECT *menu, long x, long y, long item, AESGL *global);", (void *)__popup};

char Info[] = "A GEM Popup. Dominique B‚r‚ziat 1997/1998";

LDGLIB Ldg[] = {
  0x0100,	/* version de la lib, recommand‚ */
  4,		/* nombre de fonction dans la lib */
  Proc,	        /* Pointeurs vers nos fonctions */
  Info,	        /* Information sur la lib */
  0x0    	/* Flags de la lib, obligatoire */
};

/* 
 * Fonctions de la librairie
 */

char* __CDECL __infos( void) {
  return("[1][Librairie PopUp:|1 - save screen area|2 - restore screen area|3 - GEM popup call][OK]");
}

/*
 * copy screen area in a buffer.
 */

void __CDECL __get_bkgr( long handle, 
			 long of_x, long of_y, long of_w, long of_h, 
			 MFDB *img) {
  short pxy[8];
  size_t taille;
  MFDB ecr = {0}; /* Logic screen */
  short work[57];
  
  vq_extnd( handle, 1, work);
	
  /* Taille tampon de copie fond */
  /* work[4] = number of screen planes */
  
  taille = ((((size_t)(of_w / 16) + ((of_w % 16) != 0)) * 2 * (size_t)work[ 4]) * (size_t)of_h) + 256; 
  
  img->fd_addr = malloc (taille);
  /* fillin MFDB structure */
  img->fd_w = (int)of_w;		 
  img->fd_h = (int)of_h;
  img->fd_wdwidth = ((int)of_w / 16) + (((int)of_w % 16) != 0);
  img->fd_stand = 1;
  img->fd_nplanes = work[ 4];
  
  /* fill in pxy */
  pxy[0] = of_x; pxy[1] = of_y;
  pxy[2] = pxy[0] + of_w - 1;
  pxy[3] = pxy[1] + of_h - 1;
  pxy[4] = 0; pxy[5] = 0;
  pxy[6] = of_w - 1;
  pxy[7] = of_h - 1;

  /* screen copy to buffer */
  v_hide_c ( handle);
  vro_cpyfm( handle, S_ONLY, pxy, &ecr, img);
  v_show_c ( handle, 1);
}

/* Restaure la portion d'‚cran contenue dans img aux coodonn‚es
   (x,y,w,h) 
 */

void CDECL __put_bkgr( long handle, 
		       long of_x, long of_y, long of_w, long of_h,
		       MFDB *img) {
  short pxy[8];
  MFDB ecr = {0};		/* Ecran logique */
  
  /* Fill in pxy */
  pxy[0] = 0; pxy[1] = 0;
  pxy[2] = of_w - 1; pxy[3] = of_h - 1;
  pxy[4] = of_x; pxy[5] = of_y;
  pxy[6] = of_x + of_w - 1;
  pxy[7] = of_y + of_h - 1;
 
  v_hide_c ( handle);
  vro_cpyfm( handle, S_ONLY, pxy, img, &ecr);
  v_show_c ( handle, 1);
  free ( img->fd_addr);
}

int vdihandle(void) {
  short	work_in[11], work_out[57], i;
  
  for( i=0; i<10; work_in[i++]=1);
  work_in[10]=2;
  v_opnvwk(work_in, &i, work_out);
  return i;
}

/*
 * MenuPopUp()
 * Version plus simple que celui de ma lib
 * menu: l'arbre du menu popup
 * x,y: position d'affichage
 * retour: l'index de l'item selectionn‚ ou -1
 *
 * Remarque que la fonction n'utilise pas de TIMER
 * mais que des ‚v‚nements MU_M1, ainsi ca prend moins
 * de temps machine.
 */

short __CDECL __popup( OBJECT *menu, long x, long y, long item, short *gl) {
  MFDB fond;
  int res,key;
  short dum,but;
  int last_choice = -1;
  int new_choice;
  short xclip, yclip, wclip, hclip;
  /* Parametres pour evnt_multi */
  int m1_flag;
  int m1_x, m1_y, m1_w, m1_h;
  short mx,my;
  short buff[8];
  int handle;	
  
  /* Cr‚er une station virtuelle */
  handle = vdihandle();
  
  /* position du popup */
  if( (int)item != -1) {
    mt_objc_offset( menu, item, &xclip, &yclip, gl);
    x += (menu->ob_x - xclip);
    y += (menu->ob_y - yclip);
  }
  
  /* On recale par rapport … la taille d'‚cran */
  mt_wind_get( 0, WF_WORKXYWH, &xclip, &yclip, &wclip, &hclip, gl);
  
  if( x+menu->ob_width>xclip+wclip)
    menu -> ob_x = xclip+wclip-menu->ob_width-1;
  else
    menu -> ob_x = (int)x;
  if( y+menu->ob_height>yclip+hclip)
    menu -> ob_y = yclip+hclip-menu->ob_height-1;
  else
    menu -> ob_y = (int)y;
  
  xclip=menu->ob_x-3;
  yclip=menu->ob_y-3;
  wclip=menu->ob_width+6;
  hclip=menu->ob_height+6;
  
  /* Sauvegarde fond d'‚cran */
  __get_bkgr( handle, xclip, yclip, wclip, hclip, &fond);
  
  /* Affichage Popup */
  mt_objc_draw( menu, 0, MAX_DEPTH, xclip, yclip, wclip, hclip, gl);
  
  /* Reserver les ‚venements */
  mt_wind_update ( BEG_MCTRL, gl);
  
  /* Gestion des ‚venements par EvntMulti() */
  
  mt_graf_mkstate( &mx, &my, &but, &dum, gl);
  
  /* initialiser les paramŠtres pour MU_M1 */
  last_choice = mt_objc_find( menu, 0, 2, mx, my, gl);
  if( last_choice == -1) {
    last_choice = max((int)item,0);
    m1_flag = 0;
  } else
    m1_flag = 1;
  
  if( last_choice != -1 &&
      !(menu[last_choice].ob_flags & OF_SELECTABLE)) {
    m1_x = mx-1;
    m1_y = my-1;
    m1_w = 2;
    m1_h = 2;
  } else {
    short ix,iy;
    
    mt_objc_offset( menu, last_choice, &ix, &iy, gl);
    x = ix;
    y = iy;
    m1_x = (int)x;
    m1_y = (int)y;
    m1_w = menu[last_choice].ob_width;
    m1_h = menu[last_choice].ob_height;
  }
  if( menu[last_choice].ob_flags & OF_SELECTABLE)
    mt_objc_change( menu, last_choice, 0, 
		    xclip, yclip, wclip, hclip, 
		    OS_SELECTED, 1, gl);
  
  key = 0;
  do {
    res = mt_evnt_multi(
			MU_BUTTON|MU_M1,
			1, 1, but?0:1,
			m1_flag, m1_x, m1_y, m1_w, m1_h,
			0, 0, 0, 0, 0,
			buff,
			0L,
			&mx, &my,
			&dum, &dum, &dum, &dum, 
			gl);
    
    if( res & MU_M1) {
      key = 0;
      new_choice = mt_objc_find( menu, 0, 2, mx, my, gl);
      if( last_choice != new_choice) {
	mt_graf_mouse( M_OFF, 0L, gl);
	if( last_choice != -1 && menu[ last_choice].ob_flags & OF_SELECTABLE
	    && !(menu[ last_choice].ob_state & OS_DISABLED))
	  {
	    mt_objc_change( menu, last_choice, 0, 
			    xclip, yclip, wclip, hclip, OS_NORMAL, 1, 
			    gl);
	  }
	last_choice = new_choice;
	if( new_choice != -1 && menu[ new_choice].ob_flags & OF_SELECTABLE 
	    && !(menu[ new_choice].ob_state & OS_DISABLED))
	  {
	    mt_objc_change( menu, new_choice, 0, 
			    xclip, yclip, wclip, hclip, OS_SELECTED, 1,
			    gl);
	  }
	
	/* choisir le nouveau rectangle M1 */
	if( key == 0) {
	  if( new_choice == -1) { /* hors formulaire */
	    dum = 0;
	    m1_flag = 0;
	  } else {				/* dans formulaire */
	    dum = new_choice;
	    m1_flag = 1;
	  }
	  if( new_choice != -1 &&		/* dans formulaire & objet non selectionnable */
	      !(menu[new_choice].ob_flags & OF_SELECTABLE) ) {
	    m1_x = mx-1;
	    m1_y = my-1;
	    m1_w = 2;
	    m1_h = 2;
	  } else {
	    short ix, iy;
	    
	    mt_objc_offset( menu, dum, &ix, &iy, gl);
	    x = ix;
	    y = iy;
	    m1_x = (int)x;
	    m1_y = (int)y;
	    m1_w = menu[dum].ob_width;
	    m1_h = menu[dum].ob_height;
	  }
	}
	graf_mouse( M_ON, 0L);
      }
    }
    
  } while( !(res & MU_BUTTON));
  
  /* fin */
  
  /* enlever le popup */
  
  mt_wind_update ( END_MCTRL, gl);
  __put_bkgr( handle, xclip, yclip, wclip, hclip, &fond);
  
  if( new_choice != -1 && menu[ new_choice].ob_flags & OF_SELECTABLE) {
    menu[ new_choice].ob_state &= ~OS_SELECTED;
    if( menu[ new_choice].ob_state & OS_DISABLED)
      new_choice = -1;
  }
  
  v_clsvwk( handle);
  return new_choice;
}

#ifndef DEBUG

int main( void) {
  if( ldg_init( Ldg) == -1) {
    appl_init();
    form_alert( 1, "[1][This program is a LDG library][End]");
    appl_exit();
  }
  return 0;
}

#endif
