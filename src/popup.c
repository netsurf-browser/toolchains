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
 * $Source: /cvsroot/windom/windom/src/popup.c,v $
 * Module : MenuPopUp function
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 13:52:34 $
 *   $Revision: 1.9 $
 */


#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"

/* TODO: this module needs a BIG cleanup, and maybe some re-write... */

static void __CDECL pop_drw( WINDOW *win, short buff[8], void *menu, APPvar *app) {
	mt_objc_draw_grect( (OBJECT*)menu, 0, 7, &win->graf->clip, app->aes_global);
}

/*
 * pour le moment, seen ne sert … rien 
 *	Premiere grosse modif par Arnaud (mais ca cela va
 *	encore ‚voluer) : parametre marge.
 */

/* objets du "formulaire" pop up/list
    0    -  parent
    1    -  premier choix du menu
    2    -  deuxiŠme choix du menu
    (etc...)
    seen -  dernier choix du menu
    
    (et si size > seen, cad s'il y a un ascenseur)
    seen+ASC_BG : parent de l'ascenseur
    seen+ASC_UP : fleche "haut" de l'ascenseur
    seen+ASC_DW : flŠche "bas" de l'ascenseur
    seen+ASC_PG : fond de l'ascenseur pour les PAGE UP/DW
    seen+ASC_SL : le slider
*/

#define ASC_BG  1
#define ASC_UP  2
#define ASC_DW  3
#define ASC_PG  4
#define ASC_SL  5

static void str2menu( OBJECT * tree, int ypos, int seen, char *ptxt[], 
					  int len_max, int item) {
	int i;
	
	for (i=0; i<seen; i++) {
		if (strncmp(ptxt[i+ypos],"---",3)==0)  /* c'est un s‚parateur */
		{
			tree[i+1].ob_state &= ~CHECKED;
			tree[i+1].ob_flags &= ~SELECTABLE;
			memset( tree[i+1].ob_spec.free_string, '-', len_max);
			tree[i+1].ob_spec.free_string[len_max] = 0;
		} else {
			tree[i+1].ob_flags |= SELECTABLE;
			if (item==i+1+ypos)
				tree[i+1].ob_state |= CHECKED;
			else
				tree[i+1].ob_state &= ~CHECKED;
			strcpy( tree[i+1].ob_spec.free_string, "  ");
			strcat( tree[i+1].ob_spec.free_string, ptxt[i+ypos]);
			strcat( tree[i+1].ob_spec.free_string, " ");
		}
	}
}

static void pop_udslider( OBJECT * tree, int ypos, int size, int seen) {
	/* calcul de la hauteur du slider */
	tree[seen+ASC_SL].ob_height = (seen * tree[seen+ASC_PG].ob_height) / size ;
	/* calcul la position du slider */
	tree[seen+ASC_SL].ob_y = (ypos * tree[seen+ASC_PG].ob_height) / size ;
	/* et on borne */
	if (tree[seen+ASC_SL].ob_height < 16)
		tree[seen+ASC_SL].ob_height = 16;
	if (ypos + seen == size || tree[seen+ASC_SL].ob_y + tree[seen+ASC_SL].ob_height > tree[seen+ASC_PG].ob_height)
		tree[seen+ASC_SL].ob_y = tree[seen+ASC_PG].ob_height - tree[seen+ASC_SL].ob_height;
}

static OBJECT *txt2objc( APPvar *app, char *ptxt[], int size, int seen, int poplist,
                         INT16 attrib[], int * ptr_len_max) {
	int i;
#define wcell attrib[8]
#define hcell attrib[9]
	int dum;
	OBJECT *tree;
	int width = 0;
	int dy;
#define len_max (*ptr_len_max)
	int nb_obj;
	short state3d;

	/* effet 3d ? */
	state3d = (CONF(app)->poppatt & 0x100)?OS_DRAW3D:0;
	
	nb_obj = seen + 1 ;    /* 1 (fond) + seen (choix) */ 
	if (poplist)
		nb_obj += 5 ;       /* + 5 (ascenseur) */

	tree = (OBJECT*)malloc(sizeof(OBJECT)*(nb_obj));
	if (tree == NULL) return NULL;
	
	tree[0].ob_x = 0;
	tree[0].ob_y = 0;
	tree[0].ob_flags = 0;
	tree[0].ob_state = SHADOWED | state3d;
	tree[0].ob_height = hcell*seen;
	tree[0].ob_type = G_BOX;
	/* tree[0].ob_tail d‚fini selon "poplist" un peu plus bas */
	tree[0].ob_next = -1;
	tree[0].ob_head = 1;
	tree[0].ob_spec.obspec.textmode = 0;
	tree[0].ob_spec.obspec.interiorcol = (CONF(app)->popcolor<app->color)?CONF(app)->popcolor:WHITE;
	tree[0].ob_spec.obspec.fillpattern = (CONF(app)->poppatt & 0xFF);
	tree[0].ob_spec.obspec.framesize = -CONF(app)->popborder;
	tree[0].ob_spec.obspec.framecol = (CONF(app)->popfcolor<app->color)?CONF(app)->popfcolor:BLACK;

	dy = 0;

	/* largeur max du texte */
	for( dum = 0; dum<size; dum++)
		len_max = MAX( len_max, (int)strlen( ptxt[dum]));
	len_max += 3;  /* 2 " " avant, et 1 aprŠs */
	width = len_max * wcell;
	
	for( i = 1; i<=seen; i++) {
		tree[i].ob_type = G_STRING;
		tree[i].ob_x = 0;
		tree[i].ob_y = dy;
		tree[i].ob_width = width;
		tree[i].ob_height = hcell;
		tree[i].ob_tail = -1;
		tree[i].ob_next = i+1;
		tree[i].ob_head = -1;
		tree[i].ob_flags = SELECTABLE;
		tree[i].ob_state = 0;
		tree[i].ob_spec.free_string = malloc(len_max+1);
		dy += hcell;
	}
	if( poplist) {  /* on ajoute l'ascenseur vertical */
		/************ BG = BackGround : objet parent de l'ascenseur */
		tree[seen+ASC_BG].ob_next   = 0;
		tree[seen+ASC_BG].ob_head   = seen + ASC_UP;
		tree[seen+ASC_BG].ob_tail   = seen + ASC_PG;
		tree[seen+ASC_BG].ob_type   = G_BOX;
		tree[seen+ASC_BG].ob_flags  = TOUCHEXIT;
		tree[seen+ASC_BG].ob_state  = NORMAL;
		tree[seen+ASC_BG].ob_spec.index   = app->nplanes<4 ? 0x019131L : 0x019179L;
		tree[seen+ASC_BG].ob_x      = width ;
		tree[seen+ASC_BG].ob_y      = -2 ;
		tree[seen+ASC_BG].ob_width  = 20 ;
		tree[seen+ASC_BG].ob_height = tree[0].ob_height +3;
		/************ UP = bouton "flŠche vers le haut" */
		tree[seen+ASC_UP].ob_next   = seen + ASC_DW;
		tree[seen+ASC_UP].ob_head   = -1;
		tree[seen+ASC_UP].ob_tail   = -1;
		tree[seen+ASC_UP].ob_type   = G_BOXCHAR;
		tree[seen+ASC_UP].ob_flags  = SELECTABLE;
		tree[seen+ASC_UP].ob_state  = DRAW3D;
		tree[seen+ASC_UP].ob_spec.index   = 0x1021100L;
		tree[seen+ASC_UP].ob_x      = 0  ;
		tree[seen+ASC_UP].ob_y      = 0  ;
		tree[seen+ASC_UP].ob_width  = 20 ;
		tree[seen+ASC_UP].ob_height = 20 ;
		/************ DW = bouton "flŠche vers le bas" */
		tree[seen+ASC_DW].ob_next   = seen + ASC_PG;
		tree[seen+ASC_DW].ob_head   = -1;
		tree[seen+ASC_DW].ob_tail   = -1;
		tree[seen+ASC_DW].ob_type   = G_BOXCHAR;
		tree[seen+ASC_DW].ob_flags  = SELECTABLE;
		tree[seen+ASC_DW].ob_state  = DRAW3D;
		tree[seen+ASC_DW].ob_spec.index   = 0x2021100L ;
		tree[seen+ASC_DW].ob_x      = 0 ;
		tree[seen+ASC_DW].ob_y      = tree[seen+ASC_BG].ob_height - 20 ;
		tree[seen+ASC_DW].ob_width  = 20;
		tree[seen+ASC_DW].ob_height = 20;
		/************ PG = fond du slider pour PAGE_UP/PAGE_DW */
		tree[seen+ASC_PG].ob_next   = seen + ASC_BG;
		tree[seen+ASC_PG].ob_head   = seen + ASC_SL;
		tree[seen+ASC_PG].ob_tail   = seen + ASC_SL;
		tree[seen+ASC_PG].ob_type   = G_IBOX;
		tree[seen+ASC_PG].ob_flags  = NONE;
		tree[seen+ASC_PG].ob_state  = NORMAL;
		tree[seen+ASC_PG].ob_spec.index   = 0x000000L;
		tree[seen+ASC_PG].ob_x      = 0  ;
		tree[seen+ASC_PG].ob_y      = tree[seen+ASC_UP].ob_height + tree[seen+ASC_UP].ob_y - 1 ;
		tree[seen+ASC_PG].ob_width  = 20 ;
		tree[seen+ASC_PG].ob_height = tree[seen+ASC_DW].ob_y - tree[seen+ASC_PG].ob_y + 1 ;
		/************ SL = le slider */
		tree[seen+ASC_SL].ob_next   = seen + ASC_PG;
		tree[seen+ASC_SL].ob_head   = -1;
		tree[seen+ASC_SL].ob_tail   = -1;
		tree[seen+ASC_SL].ob_type   = G_BOXCHAR;
		tree[seen+ASC_SL].ob_flags  = SELECTABLE;
		tree[seen+ASC_SL].ob_state  = DRAW3D;
		tree[seen+ASC_SL].ob_spec.index   = 0x021100L;
		tree[seen+ASC_SL].ob_x      = 0 ;
		tree[seen+ASC_SL].ob_y      = 0 ;
		tree[seen+ASC_SL].ob_width  = tree[seen+ASC_PG].ob_width;
		tree[seen+ASC_SL].ob_height = 20;

		tree[0].ob_tail = seen+ASC_BG;  /* 0=fond 1=premier ‚l‚ment etc ... */
		tree[0].ob_width = width + tree[seen+ASC_BG].ob_width - 1;
	} else {
		tree[seen].ob_next = 0; /* le dernier fils pointe vers le pŠre */
		tree[0].ob_tail = seen;  /* 0=fond 1=premier ‚l‚ment etc ... */
		tree[0].ob_width = width ;
	}
	tree[nb_obj-1].ob_flags |= LASTOB;

	
	objc_extended(app,RSRC_XALL,tree);
	
	return tree;
#undef wcell
#undef hcell
#undef len_max
}

/**
 * @brief Display and handle a menu popup.
 *
 * @param app application descriptor,
 * @param list address of a valid object tree or a list of entries (see \e mode),
 * @param xpop,ypop menu popup position in the desktop,
 * @param size if \b P_LIST is used, indicates the number of 
 *    element in \e list, otherwise this value is ignored,
 * @param seen if \b P_LIST is used, indicates the maximum of
 *    entries seen in the popup, otherwise this value is ignored,
 * @param item an index in \e list array : the entry matching to this item is
 *        centered (and popup parent too) on the mouse position,
 * @param mode a bit field, possible values are :
 *   - \b P_RDRW  this bit means that a simple redraw message
 *     will be sent to screen to redraw the area hiding
 *     by the popup instead of save the area in a buffer.
 *     This mode can be used if the popup is called from
 *     a window. With the (!B)P_WNDW(!b) mode, this mode
 *     is always used,
 *   - \b P_WNDW the popup will be drawn in a window instead of a
 *     classic formular. This mode does not stop the
 *     AES events. It should not be used when the popup is
 *     call from a classic formular,
 *   - \b P_LIST this mode means that \a list parameter points to
 *     list of entries, ie an array of strings.
 *     Each string is an entry in the popup menu. It allows to
 *     creating popup without use an object tree.
 *   - \b P_CHCK The \e item index entry will be checked.
 * @return index of item selected in the popup or -1 if no selection.
 *
 * If \b P_WNDW mode is set, it can be disabled by the user if
 * the \e windom.popup.window variable in the WinDom configuration file
 * is set to \b FALSE. This mode should be used always be used when it
 * is possible (a call of mt_MenuPopup() over a window) because the user
 * has the choice to enable or disable this feature.
 * 
 * A list of entries is a pointer such as \c "char *ptxt[]". If
 * the \a seen parameter is used, the popup is displayed with a
 * slider and contains \a seen items.
 *
 * Keyboards can be used to naviguate in the popup (up and down arrow) and
 * validate an entry (RETURN or ENTER keys). 
 *
 * With a \b P_LIST mode, the menu popup aspect can be defined by
 * the user from the WinDom configuration file (see \e windom.popup
 * variables).
 */

int mt_MenuPopUp( APPvar *app, void *list, int xpop, int ypop, 
			      int size, int seen, int item, int mode) {
	OBJECT *menu;
	MFDB fond;
	WINDOW *win;
	int res,key;
	INT16 dum, button;
	INT16 xclip, yclip, wclip, hclip;
	INT16 xtclip, ytclip, wtclip, htclip;  /* zone "T"exte, sans l'ascenseur */
	INT16 x = xpop ;
	INT16 y = ypop ;
	INT16 x16,y16;
	EVNTvar save;
	int last_choice = -1;
	int new_choice;
	/***** ajout Arnaud *******/
	int popwind = 0;  /* par d‚faut, le pop n'est pas en fenˆtre */
	int poplist=0;  /* TRUE pour les P_LIST avec un ascenseur vertical … g‚rer */
	int max;
	int ypos=0, ypos_max;  /* ascenseur vertical si poplist */
	INT16 attrib[10];
#define HCELL attrib[9]
	int len_max = 0;
	short handle = app->graf.handle;
	short buff[8];
	
	/* calcul des paramŠtres du mode P_LIST */

	if( mode & P_LIST) {
		/* le vst_height est n‚cessaire */
		vst_height( handle, 13, &dum, &dum, &dum, &dum);
		vqt_attributes( handle, attrib);
		max = app->h/HCELL - 1;

		/* bornage de "seen" */
		if( seen < 1)        /* -1 ou 0 pour prendre la valeur par d‚faut */
			seen = 16;        /* valeur par d‚faut */
		else if( seen < 6)   /* hauteur mini d'un poplist */
			seen = 6;         /* pour afficher correctement les ascenseurs */
		if( seen > max)      /* hauteur max pour rentrer … l'‚cran */
			seen = max;      /* et donc, … ne pas d‚passer */
		/* affectation de poplist */
		poplist = seen < size;
		/* seen contiendra le nb de choix … r‚ellement afficher */
		if( seen > size) seen = size;
		/* init de la position de l'ascenseur vertical */
		ypos_max = size - seen + 1;
		ypos = 0;
	}
	
	/* choix OBJECT ou liste */
	if( mode & P_LIST)
		menu = txt2objc( app, list, size, seen, poplist, attrib, &len_max);
	else
		menu = (OBJECT*)list;
	if( menu == NULL)
		return -1;		/* Fin si plus de m‚moire */

	/* mode pr‚emptif ? */
	if( CONF(app)->popwind == TRUE) {
		/* si l'‚cran n'est pas bloqu‚ par un formulaire */
		/* "pas en fenetre", il ne faut pas positionner   */
		/* popwind … TRUE : c'est incompatible !         */
		/* bon ben, yapuka ajouter le test.............  */
		popwind = TRUE;
	}
#if 1
	if( CONF(app)->popwind != DEFVAL) {
		if( CONF(app)->popwind == TRUE)
			mode |= P_WNDW;
		else
			mode &= ~P_WNDW;
	}
#endif

	/*
	 * position du popup 
	 */

	/* cas particulier de x=y=-1 : il faut positionner 
	 * le popup sous la souris, centr‚ sur la 1ere entr‚e
	 * du menu 
	 */
	if( (x==-1) && (y==-1) ) {
		mt_graf_mkstate( &x, &y, &dum, &dum,app->aes_global);
		x -= (menu[1].ob_width  >> 1);
		y -= (menu[1].ob_height >> 1);
	}

	if( item != -1) {
		if (poplist) {
			/* d‚placer ‚ventuellement le pop et l'ascenseur */
			/* ‡a peut devenir compliqu‚... */
			/* par d‚faut, on centre le pop et on ajuste */
			/* l'ascenseur pour que ca colle */
			ypos = item - (seen >> 1) -1;
			y -= (menu[1].ob_height) * (seen >> 1);
			/* si ypos sort des limites [0..ypos_max[ */
			/* et bien... on corrige */
			if (ypos < 0) {
				y += (menu[1].ob_height) * (-ypos);
				ypos = 0;
			}
			if (ypos >= ypos_max) {
				y -= (menu[1].ob_height) * (ypos-ypos_max+1);
				ypos = ypos_max-1;
			}
		} else {
			mt_objc_offset( menu, item, &xclip, &yclip, app->aes_global);
			if(mode & P_CHCK) menu[item].ob_state |= CHECKED;
			x += (menu->ob_x - xclip);
			y += (menu->ob_y - yclip);
		}
	}
	
	/* a partir d'ici, item est le numero de l'entr‚e */
	/* qui doit ˆtre check‚e */
	if ((item <= 0) || !(mode & P_CHCK))
		item = -1;

	if( x+menu->ob_width>app->x+app->w)
		menu -> ob_x = app->x+app->w-menu->ob_width-1;
	else
		menu -> ob_x = MAX(x,app->x+3);
	if( y+menu->ob_height>app->y+app->h)
		menu -> ob_y = app->y+app->h-menu->ob_height-1;
	else
		menu -> ob_y = MAX(y,app->y+3);

	xclip=menu->ob_x-3;
	yclip=menu->ob_y-3;
	wclip=menu->ob_width+6;
	hclip=menu->ob_height+6;

	if (poplist) {
		xtclip=menu->ob_x + menu[1].ob_x;
		ytclip=menu->ob_y;
		wtclip=menu[1].ob_width;
		htclip=menu->ob_height;
	}

	
	/* maintenant que ypos est parfaitement d‚fini, */
	/* on peut initialiser les free_strings de menu */
	if (mode & P_LIST)
		str2menu(menu, ypos, seen, list, len_max, item);
	if (poplist)
		pop_udslider(menu,ypos,size,seen);

	/* afficher le popup */
	if( popwind) {
		win = mt_WindCreate( app, 0, app->x, app->y, app->w, app->h);
		win -> status &= ~WS_GROW;
		if( win == NULL) {
			if( mode & P_LIST)
			free( menu);
			return -1;
		}
		mt_MenuDisable(app);
		mt_WindSet( app, win, WF_BEVENT, BEVENT_MODAL, 0, 0, 0);
		win->w_min = win->h_min = 1;
		mt_WindOpen( app, win, menu->ob_x , menu->ob_y, menu->ob_width, menu->ob_height);
		mt_EvntDataAttach( app, win, WM_REDRAW, pop_drw, menu);
	} else {
		if( mode & P_RDRW)	/* j'ai invers‚ par rapport … la doc car c'est pas logique */
			mt_form_dial( FMD_START, 0, 0, 0, 0, xclip, yclip, wclip, hclip, app->aes_global);
		else
			w_get_bkgr( app, xclip, yclip, wclip, hclip, &fond);
		mt_objc_draw( menu, 0, MAX_DEPTH, xclip, yclip, wclip, hclip, app->aes_global);
		while( !mt_wind_update (BEG_MCTRL, app->aes_global));
	}

	
	/* Gestion des ‚venements par EvntMulti() */
	
	/* initialiser les paramŠtres MU_BUTTON */
	mt_graf_mkstate( &app->evnt.mx, &app->evnt.my, &button, &dum, app->aes_global);
	save = app->evnt;

	/* Modif Arnaud, qui rend obsolete l'option P_RGHT : a tester */
	if( button) {
		/* l'‚v‚nement attendu est le relach‚ du "button" */
		app->evnt.bmask = button ;
		app->evnt.bstate = 0;
		app->evnt.bclick = 1;
	} else {
		/* l'‚vŠnement attendu est l'appuie sur 1 button quelconque */
		app->evnt.bclick = 258;
		app->evnt.bmask  = 3;
		app->evnt.bstate = 0;
	}
	
	/* initialiser les paramŠtres pour MU_M1 */
	last_choice = mt_objc_find( menu, 0, 4 /*2*/, app->evnt.mx, app->evnt.my, app->aes_global);
	if( last_choice == -1) {
		last_choice = 0;
		app->evnt.m1_flag = 0;
	} else
		app->evnt.m1_flag = 1;
	
	if( last_choice != -1 &&
		!(menu[last_choice].ob_flags & SELECTABLE)) {
		app->evnt.m1_x = app->evnt.mx-1;
		app->evnt.m1_y = app->evnt.my-1;
		app->evnt.m1_w = 2;
		app->evnt.m1_h = 2;
	} else {
		mt_objc_offset( menu, last_choice, &x, &y, app->aes_global);
		app->evnt.m1_x = x;
		app->evnt.m1_y = y;
		app->evnt.m1_w = menu[last_choice].ob_width;
		app->evnt.m1_h = menu[last_choice].ob_height;
	}
	if( menu[last_choice].ob_flags & SELECTABLE) {
		if ( (poplist) && (last_choice > seen) )  /* sur l'ascenseur */
			; /* ne rien faire */
		else if( popwind)
			mt_ObjcWindChange( app, win, menu, last_choice, xclip, yclip, wclip, hclip, SELECTED);
		else
			mt_objc_change( menu, last_choice, 0, xclip, yclip, wclip, hclip, SELECTED, 1, app->aes_global);
	}
	key = 0;
	do {
		if( popwind)
			res = mt_EvntWindom( app, MU_BUTTON|MU_KEYBD|MU_M1|MU_MESAG);
		else
			res = mt_evnt_multi( 
					MU_BUTTON|MU_KEYBD|MU_M1,
					app->evnt.bclick, app->evnt.bmask, app->evnt.bstate,
					app->evnt.m1_flag, app->evnt.m1_x, app->evnt.m1_y, app->evnt.m1_w, app->evnt.m1_h,
					0, 0, 0, 0, 0,
					buff,
					0L,
					&app->evnt.mx, &app->evnt.my,
					&dum, &dum, &app->evnt.keybd, &dum, app->aes_global);

		if( res & MU_KEYBD) {
			switch( app->evnt.keybd>>8) {
			case 72: /* Haut */
				/* TODO */
				break;
			case 80: /* Bas */
				if ((poplist) && (last_choice==seen))
				{
					if (ypos < ypos_max-1)
					{
						ypos ++ ;
						/* recaluler les objets de menu */
						str2menu(menu, ypos, seen, list, len_max, item);
						/* recalculer la position du slider */
						pop_udslider(menu,ypos,size,seen);
						/* redraw  */
						if (popwind) {
							mt_ObjcWindDraw(app,win,menu, 0, MAX_DEPTH, xtclip, ytclip, wtclip, htclip);
							mt_ObjcWindDrawParent(app,win,menu, seen+ASC_PG, 1, MAX_DEPTH, xclip, yclip, wclip, hclip);
						} else {
							mt_objc_draw( menu, 0, MAX_DEPTH, xtclip, ytclip, wtclip, htclip, app->aes_global);
							mt_ObjcDrawParent(app,OC_OBJC, menu, seen+ASC_PG, 1, MAX_DEPTH);
						}
					}
					new_choice = seen ;
				} else 
					new_choice = obj_fd_flag( menu, MAX(last_choice,0)+1, SELECTABLE);
				key = 1;
				app->evnt.m1_flag = 1;
				mt_graf_mkstate( &x16, &y16, &dum, &dum, app->aes_global);
				app->evnt.m1_x = x16 - 2;
				app->evnt.m1_y = y16 - 2;
				app->evnt.m1_w = app->evnt.m1_h = 4;
				goto GO_M1;
			case 28:
			case 114:
				res = MU_BUTTON;
				break;
			}
		}
		if( res & MU_M1) {
			key = 0;
			new_choice = mt_objc_find( menu, 0, 4 /*2*/, app->evnt.mx, app->evnt.my, app->aes_global);
GO_M1:
			if( last_choice != new_choice) {
				mt_graf_mouse( M_OFF, 0L, app->aes_global);
				if( last_choice != -1 && menu[ last_choice].ob_flags & SELECTABLE
					&& !(menu[ last_choice].ob_state & DISABLED))
				{
					if ( (poplist) && (last_choice > seen) )  /* sur l'ascenseur */
						; /* ne rien faire */
					else if( popwind)
						mt_ObjcWindChange( app, win, menu, last_choice, xclip, yclip, wclip, hclip, NORMAL);
					else
						mt_objc_change( menu, last_choice, 0, xclip, yclip, wclip, hclip, NORMAL, 1, app->aes_global);
				}
				last_choice = new_choice;
				if( new_choice != -1 && menu[ new_choice].ob_flags & SELECTABLE 
					&& !(menu[ new_choice].ob_state & DISABLED))
				{
					if ( (poplist) && (new_choice > seen) )  /* sur l'ascenseur */
						; /* ne rien faire */
					else if( popwind)
						mt_ObjcWindChange( app, win, menu, new_choice, xclip, yclip, wclip, hclip, SELECTABLE);
		    		else
		    			mt_objc_change( menu, new_choice, 0, xclip, yclip, wclip, hclip, SELECTED, 1, app->aes_global);
				}
				
				/* choisir le nouveau rectangle M1 */
				if( key == 0) {
				 	if( new_choice == -1) { /* hors formulaire */
				 		dum = 0;
				 		app->evnt.m1_flag = 0;
				 	} else {				/* dans formulaire */
				 		dum = new_choice;
				 		app->evnt.m1_flag = 1;
				 	}
				 	if( new_choice != -1 &&		/* dans formulaire & objet non selectionnable */
				 		!(menu[new_choice].ob_flags & SELECTABLE) ) {
				 		app->evnt.m1_x = app->evnt.mx-1;
						app->evnt.m1_y = app->evnt.my-1;
						app->evnt.m1_w = 2;
						app->evnt.m1_h = 2;
				 	} else {
				 		mt_objc_offset( menu, dum, &x, &y, app->aes_global);
						app->evnt.m1_x = x;
						app->evnt.m1_y = y;
						app->evnt.m1_w = menu[dum].ob_width;
						app->evnt.m1_h = menu[dum].ob_height;
					}
				}
				mt_graf_mouse( M_ON, 0L, app->aes_global);
			}
		}
		if( res & MU_BUTTON) {
			/* si on a MU_KEYBD, new_choice a d‚j… ‚t‚ calcul‚ */
			/* et ne correspond pas … la position de la souris */
			/* il ne faut donc pas faire de objc_find() !!!    */
			if (! (res & MU_KEYBD) )
				new_choice = mt_objc_find( menu, 0, 4 /*2*/, app->evnt.mx, app->evnt.my, app->aes_global);
			if ((poplist) && (!button) && (new_choice>seen))  /* clic sur l'ascenseur */
			{
				int decal=0;  /* dcalage … appliquer sur ypos ou 0 */
				INT16 x, y;
				int new_ypos;
				EVNTvar save;
				int e;
				int obj_asc_sele; /* objet du formulaire s‚lectionn‚ */
				
				/* calcul de decal */
				switch( new_choice - seen) {
					case ASC_UP : decal = -1; break;
					case ASC_DW : decal = 1;  break;
					case ASC_PG : 
						mt_objc_offset(menu,seen+ASC_SL,&x,&y, app->aes_global);
						decal = (app->evnt.my < y) ? -seen : seen;
						break;
					case ASC_SL : decal = 0;
				}
				
				switch (new_choice - seen) {
					case ASC_UP : 
					case ASC_DW :
					case ASC_SL :
						obj_asc_sele = new_choice; break;
					default :
						obj_asc_sele = 0;
				}
				if( obj_asc_sele) {
					menu[obj_asc_sele].ob_state |= SELECTED;
					if( popwind) {
						mt_ObjcWindDraw( app, win,menu, obj_asc_sele, MAX_DEPTH, xclip, yclip, wclip, hclip);
					} else {
						mt_objc_draw( menu, obj_asc_sele, MAX_DEPTH, xclip, yclip, wclip, hclip, app->aes_global);
					}
				}
					
				if( decal) {  /* gestion d'un evnt LINE/PAGE_UP/DW */
					save = app->evnt;
					app->evnt.bmask  = 1 ;
					app->evnt.bstate = 0;
					app->evnt.bclick = 1;
#define TEMPO1        460
#define TEMPO2        20
					app->evnt.timer = TEMPO1;
					do {
						new_ypos = ypos + decal;
						if (new_ypos < 0) new_ypos = 0;
						if (new_ypos > ypos_max-1) new_ypos = ypos_max-1;
						if (new_ypos != ypos) {
							ypos = new_ypos;
							/* recaluler les objets de menu */
							str2menu(menu, ypos, seen, list, len_max, item);
							/* recalculer la position du slider */
							pop_udslider(menu,ypos,size,seen);
							/* redraw  */
							if (popwind) {
								mt_ObjcWindDraw(app,win,menu, 0, MAX_DEPTH, xtclip, ytclip, wtclip, htclip);
								mt_ObjcWindDrawParent(app,win,menu, seen+ASC_PG, 1, MAX_DEPTH, xclip, yclip, wclip, hclip);
							} else {
								mt_objc_draw( menu, 0, MAX_DEPTH, xtclip, ytclip, wtclip, htclip, app->aes_global);
								mt_ObjcDrawParent(app,OC_OBJC, menu, seen+ASC_PG, 1, MAX_DEPTH);
							}
						}
					
						/* attente avant de reboucler */
						if( popwind)
							e = mt_EvntWindom( app, MU_BUTTON|MU_TIMER);
						else
							e = mt_evnt_multi( 
							      MU_BUTTON|MU_TIMER,
									app->evnt.bclick, app->evnt.bmask, app->evnt.bstate,
									0, 0, 0, 0, 0,
									0, 0, 0, 0, 0,
									buff,
									app->evnt.timer,
									&app->evnt.mx, &app->evnt.my,
									&dum, &dum, &app->evnt.keybd, &dum, app->aes_global);
						app->evnt.timer = TEMPO2;
					} while ( ! (e & MU_BUTTON) ) ;
					app->evnt = save;
				} else { /* gestion d'un SLIDER temps r‚els */
					/* decal est maintenant le decalage entre la */
					/* position du slider et la position de la souris */
					mt_objc_offset(menu,seen+ASC_SL,&x,&y,app->aes_global);
					decal = app->evnt.my - y;
					mt_objc_offset(menu,seen+ASC_PG,&x,&y,app->aes_global);
										
					do {
						/* ‚valuation de new_ypos */
						new_ypos = app->evnt.my - decal - y;  /* futur ob_y du slider */
						new_ypos = ((long)((long)((long)new_ypos * (long)size)<<1) + (long)menu[seen+ASC_PG].ob_height) / (long)(menu[seen+ASC_PG].ob_height << 1);
						if (new_ypos < 0) new_ypos = 0;
						if (new_ypos > ypos_max-1) new_ypos = ypos_max-1;
						if (new_ypos != ypos)
						{
							ypos = new_ypos;
							/* recaluler les objets de menu */
							str2menu(menu, ypos, seen, list, len_max, item);
							/* recalculer la position du slider */
							pop_udslider(menu,ypos,size,seen);
							/* redraw  */
							if (popwind) {
								mt_ObjcWindDraw( app, win,menu, 0, MAX_DEPTH, xtclip, ytclip, wtclip, htclip);
								mt_ObjcWindDrawParent( app, win,menu, seen+ASC_PG, 1, MAX_DEPTH, xclip, yclip, wclip, hclip);
							} else {
								mt_objc_draw( menu, 0, MAX_DEPTH, xtclip, ytclip, wtclip, htclip,app->aes_global);
								mt_ObjcDrawParent(app,OC_OBJC, menu, seen+ASC_PG, 1, MAX_DEPTH);
							}
						}
						/* et on reboucle tant que le bouton est appuy‚ */
						mt_graf_mkstate(&app->evnt.mx,&app->evnt.my,&app->evnt.mbut,&dum,app->aes_global);
					} while (app->evnt.mbut) ;
				}
				if (obj_asc_sele) {
					menu[obj_asc_sele].ob_state &= ~SELECTED;
					if( popwind) {
						mt_ObjcWindDraw(app, win,menu, obj_asc_sele, MAX_DEPTH, xclip, yclip, wclip, hclip);
					} else {
						mt_objc_draw( menu, obj_asc_sele, MAX_DEPTH, xclip, yclip, wclip, hclip,app->aes_global);
					}
				}
				res &= ~MU_BUTTON ;
			}
			/* si on vient de cliquer sur une entr‚e du menu */
			/* on continu … gerer le pop jusqu'au relache du bouton */
			if( (new_choice > 0) && (new_choice <= seen)  && 
			    (!(res & MU_KEYBD)) && !button) {
				/* l'‚v‚nement attendu est le relach‚ du "button" */
				button = 1;
				app->evnt.bmask = button ;
				app->evnt.bstate = 0;
				app->evnt.bclick = 1;
				res &= ~MU_BUTTON ;
			}
		}
		
	} while( !(res & MU_BUTTON));

	/* fin */

	/* enlever le popup */
	if( popwind) {
		mt_WindClose( app, win);
		mt_WindDelete( app, win);
		mt_MenuEnable(app );
/*		EvntWindom(MU_MESAG); */
	} else {
		mt_wind_update (END_MCTRL, app->aes_global);
		if(mode & P_RDRW)
			mt_form_dial( FMD_FINISH, 0, 0, 0, 0, xclip, yclip, wclip, hclip, app->aes_global);
		else
			w_put_bkgr( app, xclip, yclip, wclip, hclip, &fond);
	}

	app->evnt = save;

	if( new_choice != -1 && menu[ new_choice].ob_flags & SELECTABLE) {
		menu[ new_choice].ob_state &= ~SELECTED;
		if( menu[ new_choice].ob_state & DISABLED)
			new_choice = -1;
	}	

	if( mode & P_LIST) {
		objc_extfree(app,menu);
		for( dum = 1; dum <= seen; dum++)
			free(menu[dum].ob_spec.free_string);
		free( menu);
	}

	if (poplist) {
		if (new_choice < 0)
			return new_choice;
		else
			return new_choice + ypos;
	}
	
	return new_choice;
}

