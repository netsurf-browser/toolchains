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
 * $Source: /cvsroot/windom/windom/examples/frame/viewtext.h,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:39:25 $
 *   $Revision: 1.4 $
 */

#define WTXT 0x57545854L  /*'WTXT'*/

WINDOW *mt_ViewTextCreate	( APPvar *, char *file, int attrib);
void    mt_ViewTextOpen		( APPvar *, WINDOW *win, int x, int y, int w, int h);
void    mt_ViewTextReCreate	( APPvar *, WINDOW *win, char *file);
int     mt_ViewTextSearch	( APPvar *, WINDOW *win, char *target, int lastpos, int capsens);
int 	mt_ViewTextGet		( APPvar *, WINDOW *win, int mode, int *val);

#ifdef __WINDOM__
static inline WINDOW *ViewTextCreate ( char *file, int attrib) {
	return mt_ViewTextCreate( gl_appvar, file, attrib);
}
static inline void ViewTextOpen ( WINDOW *win, int x, int y, int w, int h) {
	mt_ViewTextOpen( gl_appvar, win, x, y, w, h);
}
static inline void ViewTextReCreate ( WINDOW *win, char *file) {
	mt_ViewTextReCreate ( gl_appvar, win, file);
}
static inline int ViewTextSearch ( WINDOW *win, char *target, int lastpos, int capsens) {
	return mt_ViewTextSearch ( gl_appvar, win, target, lastpos, capsens);
}
static inline int ViewTextGet ( WINDOW *win, int mode, int *val) {
	return mt_ViewTextGet ( gl_appvar, win, mode, val);
}
#endif
