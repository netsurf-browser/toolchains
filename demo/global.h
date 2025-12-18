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
 * $Source: /cvsroot/windom/windom/demo/global.h,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/03/22 20:58:30 $
 *   $Revision: 1.7 $
 */


#define SETUP_INSTALL_DESKTOP	0x1
#define SETUP_SAVE_WINDOWS		0x2
#define SETUP_AUTOSAVE			0x4
#define SETUP_XTYPE				0x8

struct _param {
	OBJECT *desk;
	WINDOW *winuser;
	int setup, FrameWidth, FrameColor;
};

extern struct _param param;

extern void *rsc;
extern OBJECT **rsc_trindex;
extern short rsc_ntree;

/* win-divr.c */
extern void create_simple( void);
extern void call_fontselector( void);
extern void call_fileselector( void);

/* win-cust.c */
extern void create_custom( void);

/* win-fram.c */
extern void create_two_frame(void);
extern void create_three_frame(void);

/* setup.c */
extern void setup_save( void);
extern void setup_read( void);
extern void create_form_conf( void);

/* load-bub.c */
extern int LoadBubble( char *filename);
extern void UnLoadBubble( void);

/* evnt-fnc.c */
extern void __CDECL ap_term( WINDOW *w, short buff[8]);
extern void __CDECL av_sendclick( WINDOW *w, short buff[8]);
extern void __CDECL av_sendkey( WINDOW *w, short buff[8]);
extern void __CDECL mu_button( WINDOW *w, short buff[8]);
extern void menu_binding( void);
extern void __CDECL menu_desktop( WINDOW *w, short buff[8]);

/* win-tool.c */
extern void create_windowtoolbar( void);
extern void create_windowtoolmenu( void);

/* win-form.c */
extern void create_info_form( void);
extern void create_modal_form( void);
extern void create_quit_form( void);
extern void create_widget_form( int dup);

/* utils.c */
extern char *get_string( int __index);
extern OBJECT *get_tree( int __index);
extern void display_info( WINDOW *win, int handle);
extern void draw_color_icon( WINDOW *win);
extern void draw_mono_icon( WINDOW *win);
extern void __CDECL stdDESTROY( WINDOW *win, short buff[8]);

/* evnt-av.c */
extern void av_status ( void );
extern void do_av_opendir( void);
extern void do_av_sendfile( void);
extern void do_av_view( void);
extern void do_av_sendkey( void);
