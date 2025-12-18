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
 * $Source: /cvsroot/windom/windom/include/windom.h,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/17 20:00:23 $
 *   $Revision: 1.58 $
 */

#ifndef __WINDOM__
#define __WINDOM__

#ifndef __MT_WINDOM__
#include <mt_wndm.h>
#endif

#include <gem.h>

__BEGIN_DECLS

/* Global variables
 ***********************/

extern APPvar           *gl_appvar;

/* some macros for backward compatibility */
#define app (*gl_appvar)
#define wglb (gl_appvar->wglb)
#define evnt (gl_appvar->evnt)

#ifdef FORM
#undef FORM
#define FORM(win)   (((W_FORM*)mt_DataSearch( gl_appvar, win, WD_WFRM))->root)
#endif


/* Function Prototypes
 ***********************/

/* Application Library */
/** @addtogroup Application 
 * @{ */

/** see mt_ApplInit() */
extern int ApplInit( void);

/** see mt_ApplExit() */
extern int ApplExit( void);

/** see mt_ApplSet() */
static inline int ApplSet( int mode, short v1, short v2, short v3, short v4) {
	return mt_ApplSet( gl_appvar, mode, v1, v2, v3, v4);
}

/** see mt_ApplGet() */
static inline int ApplGet( int mode, short *v1, short *v2, short *v3, short *v4) {
	return mt_ApplGet( gl_appvar, mode, v1, v2, v3, v4);
}

/** see mt_ApplName() */
static inline int ApplName( char *name, int id) {
	return mt_ApplName( gl_appvar, name, id);
}

/** see mt_ApplControl() */
static inline int ApplControl( int ap_cid, int ap_cwhat, void *ap_cout) {
	return mt_ApplControl( gl_appvar, ap_cid, ap_cwhat, ap_cout);
}

/** see mt_ApplWrite() */
static inline int ApplWrite(int to, int msg, int b3, int b4, int b5, int b6, int b7) {
 	return mt_ApplWrite(gl_appvar,to,msg,b3,b4,b5,b6,b7);
}

/** @} */

/* AV library */
/** @addtogroup Avlib
 * @{ */

static inline int AvInit( char *name, int status, long idle) {
	return mt_AvInit( gl_appvar, name, status, idle);
}

static inline void  AvExit ( void) {
	mt_AvExit( gl_appvar);
}

static inline INT16* AvStatus ( void) {
	return mt_AvStatus( gl_appvar);
}

static inline char* AvServer ( void) {
	return mt_AvServer( gl_appvar);
}

static inline int AvWaitfor( int msg, INT16 *buf, long idle) {
	return mt_AvWaitfor( gl_appvar, msg, buf, idle);
}

/** @} */

/* BubbleGEM library */ 
/** @addtogroup BubbleGEM
 * @{ */
 
static inline int BubbleCall  ( char *help, int x, int y) {
	return mt_BubbleCall ( gl_appvar, help, x, y);
}

static inline int BubbleModal ( char *help, int x, int y) {
	return mt_BubbleModal ( gl_appvar, help,  x,  y);
}

static inline int  BubbleDo    ( OBJECT *tree, int __index) {
	return mt_BubbleDo ( gl_appvar, tree, __index);
}

static inline int BubbleAttach( OBJECT *tree, int __index, char *help) {
	return mt_BubbleAttach ( gl_appvar, tree, __index, help) ;
}

static inline void BubbleFree  ( void) {
	mt_BubbleFree  (gl_appvar );
}

static inline int BubbleFind  ( OBJECT *tree, int __index, char **help) {
	return mt_BubbleFind  ( gl_appvar, tree, __index, help);
}

static inline int BubbleEvnt  ( void) {
	return mt_BubbleEvnt  ( gl_appvar);
}

static inline int BubbleGet   ( void) {
	return mt_BubbleGet   ( gl_appvar);
}

/* this function no more exist */
#define BubbleConf(a)		  /* nothing */

/** @} */ 

/* Configuration library */
/** @addtogroup Configuration
 * @{ */

static inline int ConfRead( void) {
	return mt_ConfRead(gl_appvar);
}

static inline int ConfGetLine( char *string) {
	return mt_ConfGetLine(gl_appvar,string);
}

static inline int ConfWindom( void) {
	return mt_ConfWindom(gl_appvar);
}

/** see mt_ConfInquire() */
int     ConfInquire( char *keyword, char *fmt, ...);

/** see mt_ConfWrite() */
int     ConfWrite( char *keyword, char *fmt, ...);

/** @} */ 

/* Data library */
/** @addtogroup Data
 * @{ */
static inline int DataAttach ( WINDOW *win, long magic, void *data) {
	return mt_DataAttach(gl_appvar,win,magic,data);
}

static inline int DataDelete ( WINDOW *win, long magic) {
	return mt_DataDelete(gl_appvar,win,magic);
}

static inline void* DataSearch ( WINDOW *win, long magic) {
	return mt_DataSearch(gl_appvar,win,magic);
}
/** @} */ 

/* Event library */
/** @addtogroup Event
 * @{*/

/**@brief See mt_EvntWindom() */
static inline int EvntWindom( int __evnt) {
	return mt_EvntWindom(gl_appvar, __evnt);
}

static inline int EvntWindomBuff( int __evnt, short buff[8]) {
	return mt_EvntWindomBuff(gl_appvar,__evnt,buff);
}

/**@brief See mt_EvntAdd() */
static inline int EvntAdd( WINDOW *win, int msg, func_evnt proc, int mode) {
	return mt_EvntAdd(gl_appvar,win,msg,proc,mode);
}

/**@brief See mt_EvntAttach() */
static inline int EvntAttach ( WINDOW *win, int msg, func_evnt proc) {
	return mt_EvntAttach(gl_appvar,win,msg,proc);
}

/**@brief See mt_EvntDataAdd() */  
static inline int EvntDataAdd ( WINDOW *win, int msg, func_evntdata proc, void *data, int mode) {
	return mt_EvntDataAdd(gl_appvar,win,msg,proc,data,mode);
}

/**@brief See mt_EvntDataAttach() */
static inline int EvntDataAttach( WINDOW *win, int msg, func_evntdata proc, void *data) {
	return mt_EvntDataAttach(gl_appvar,win,msg,proc,data);
}

/**@brief See mt_EvntDelete() */
static inline int EvntDelete ( WINDOW *win, int msg) {
	return mt_EvntDelete(gl_appvar,win,msg);
}

/**@brief See mt_EvntFind() */ 
static inline void* EvntFind ( WINDOW *win, int msg) {
	return mt_EvntFind(gl_appvar,win,msg);
}

/**@brief See mt_EvntExec() */
static inline int EvntExec ( WINDOW *win, short buff[8]) {
	return mt_EvntExec(gl_appvar,win,buff);
}

/**@brief See mt_EvntClear() */ 
static inline void EvntClear ( WINDOW *win) {
	mt_EvntClear(gl_appvar,win);
}

/**@brief See mt_EvntRemove() */
static inline int EvntRemove ( WINDOW *win, int msg, void *proc) {
	return mt_EvntRemove(gl_appvar,win,msg,proc);
}

/**@brief See mt_EvntDisable() */ 
static inline void EvntDisable ( WINDOW *win, int msg) {
	mt_EvntDisable(gl_appvar,win,msg);
}

/**@brief See mt_EvntEnable() */ 
static inline void EvntEnable ( WINDOW *win, int msg) {
	mt_EvntEnable(gl_appvar,win,msg);
}

/**@brief See mt_EvntRedraw() */
static inline void EvntRedraw ( WINDOW *win) {
	mt_EvntRedraw(gl_appvar,win);
}

/**@brief See mt_EvntRedrawGrect() */
static inline void	EvntRedrawGrect( WINDOW *win, GRECT *work) {
	mt_EvntRedrawGrect(gl_appvar,win,work);
}

/** @} */

/* Formular library */
/** @addtogroup Formular
 * @{ */

/**@brief See mt_FormWindBegin() */
static inline WINDOW* FormWindBegin( OBJECT *dial, char *nom) {
	return mt_FormWindBegin(gl_appvar,dial,nom);
}

/**@brief See mt_FormWindEnd() */
static inline void FormWindEnd  ( void) {
	mt_FormWindEnd(gl_appvar);
}

/**@brief See mt_FormWindDo() */
static inline int FormWindDo ( int __evnt) {
	return mt_FormWindDo(gl_appvar,__evnt);
}

static inline int FormWindDoBuff( int __evnt, short buff[8]) {
	return mt_FormWindDoBuff(gl_appvar,__evnt,buff);
}

/**@brief See mt_FormAttach() */
static inline int FormAttach ( WINDOW *win, OBJECT *tree, func_evnt func) {
	return mt_FormAttach(gl_appvar,win,tree,func);
}

/**@brief See mt_FormResize() */
static inline void 	FormResize ( WINDOW *win, INT16 *x, INT16 *y, INT16 *w, INT16 *h) {
	mt_FormResize(gl_appvar,win,x,y,w,h);
}

/**@brief See mt_FormCreate() */
static inline WINDOW* FormCreate ( OBJECT *tree, int attrib, func_evnt func, char *name, GRECT *coord, int grow, int dup) {
	return mt_FormCreate(gl_appvar,tree,attrib,func,name,coord,grow,dup);
}

/**@brief See mt_FormSave() */
static inline int FormSave ( WINDOW *win, int mode) {
	return mt_FormSave(gl_appvar,win,mode);
}

/**@brief See mt_FormRestore() */
static inline int FormRestore ( WINDOW *win, int mode) {
	return mt_FormRestore ( gl_appvar, win, mode);
}

/**@brief See mt_FormBegin() */
static inline void FormBegin ( OBJECT *tree, MFDB *bckgrnd) {
	mt_FormBegin ( gl_appvar, tree, bckgrnd);
}

/**@brief See mt_FormEnd() */
static inline void FormEnd ( OBJECT *tree, MFDB *bckgrnd) {
	mt_FormEnd ( gl_appvar, tree, bckgrnd);
}

/**@brief See mt_FormDo() */
#define FormDo(a,b)					mt_form_do(a,b,gl_appvar->aes_global)

/**@brief See mt_FormThumb() */
static inline void* FormThumb ( WINDOW *win, int *idxthb, int *idxbut, int nb) {
	return mt_FormThumb ( gl_appvar, win, idxthb, idxbut, nb);
}

/**@brief See mt_FormThbSet() */
static inline void FormThbSet ( WINDOW *win, int but) {
	mt_FormThbSet ( gl_appvar, win, but);
}

/**@brief See mt_FormThbGet() */
static inline int FormThbGet ( void   *thb, int mode) {
	return mt_FormThbGet ( gl_appvar, thb, mode);
}

/** see mt_FormAlert() */
int     FormAlert( int but, char fmt[], ...);

/** see mt_stdFormClose() */
void __CDECL stdFormClose(WINDOW *win, int ind, int mode, void *data);

/** @} */


/* Mouse library */
/** @addtogroup Mouse
 * @{ */

static inline void MouseObjc ( OBJECT *tree, int __index) {
	mt_MouseObjc ( gl_appvar, tree, __index);
}

static inline void MouseWork ( void) {
	mt_MouseWork( gl_appvar);
}

static inline void MouseSprite ( OBJECT *tree, int __index) {
	mt_MouseSprite ( gl_appvar, tree, __index);
}

/** @} */ 


/* Frame library */
/** @addtogroup Frame
 * @{ */
 
static inline void FrameInit ( void) {
	mt_FrameInit(gl_appvar);
}

static inline void FrameExit ( void) {
	mt_FrameExit(gl_appvar);
}

static inline WINDOW* FrameCreate ( int attrib) {
	return mt_FrameCreate ( gl_appvar, attrib);
}

static inline void FrameAttach ( WINDOW *win, WINDOW *src, int line, int col, int w, int h, int mode) {
	mt_FrameAttach ( gl_appvar, win, src, line, col, w, h, mode);
}

static inline WINDOW* FrameRemove ( WINDOW *win, WINDOW *child, int line, int col) {
	return mt_FrameRemove ( gl_appvar, win, child, line, col);
}

void    FrameSet( WINDOW *win, int mode, ...);

void 	FrameGet( WINDOW *win, int mode, ...);

static inline WINDOW* FrameFind ( WINDOW *win, int x, int y) {
	return mt_FrameFind   ( gl_appvar, win, x, y);
}

static inline WINDOW* FrameSearch ( WINDOW *win, int line, int col) {
	return mt_FrameSearch ( gl_appvar, win, line, col);
}

static inline int FrameCalc ( WINDOW *win, int mode, INT16 *x, INT16 *y, INT16 *w, INT16 *h) {
	return mt_FrameCalc ( gl_appvar, win, mode, x, y, w, h);
}

/** @} */ 

/* Font  library */
/** @addtogroup Font
 * @{ */
static inline int     FontName2Id	  ( char *name) {
	return mt_FontName2Id ( gl_appvar,name );
}

static inline int     FontId2Name	  ( int id, char *name) {
	return mt_FontId2Name ( gl_appvar, id, name);
}

static inline int     VstLoadFonts    ( int res) {
	return mt_VstLoadFonts ( gl_appvar, res );
}

static inline void    VstUnloadFonts( int res) {
	mt_VstUnloadFonts ( gl_appvar, res);
}

static inline int     VstFont		  ( int id) {
	return mt_VstFont ( gl_appvar, id);
}

static inline int     VqtName		  ( int elem, char *name) {
	return mt_VqtName ( gl_appvar, elem, name);
}

static inline int     VqtXname  	  ( int id, int __index, char *name, int *fl) {
	return mt_VqtXname ( gl_appvar, id, __index, name, fl);
}

/** @} */ 

/* Selector library */
/** @addtogroup Selector
 * @{ */
static inline int FselInput ( char *path, char *fname, char *ext, char *title, char *lpath, char *lext) {
	return mt_FselInput ( gl_appvar, path, fname, ext, title, lpath, lext);
}

static inline int FontSel ( char *title, char *example, int flags, int *id, int *size, char *name) {
	return mt_FontSel ( gl_appvar, title, example, flags, id, size, name);
}
/** @} */ 

/* Menu library */
/** @addtogroup Menu
 * @{ */
static inline int     MenuBar 	( OBJECT *menu, int mode) {
	return mt_MenuBar( gl_appvar, menu, mode);
}

static inline int     MenuTnormal ( WINDOW *win, int title, int mode) {
	return mt_MenuTnormal( gl_appvar, win, title,mode );
}

static inline int     MenuIcheck  ( WINDOW *win, int title, int mode) {
	return mt_MenuIcheck( gl_appvar, win, title, mode);
}

static inline int     MenuText	( WINDOW *win, int title, char *txt) {
	return mt_MenuText( gl_appvar, win, title, txt);
}

static inline int     MenuDisable ( void) {
	return mt_MenuDisable( gl_appvar);
}

static inline int     MenuEnable  ( void) {
	return mt_MenuEnable( gl_appvar);
}

static inline int     MenuPopUp	( void *data, int x, int y, int max, int size, int item, int mode) {
	return mt_MenuPopUp( gl_appvar, data, x, y, max, size, item, mode);
}

static inline void    MenuScroll  ( WINDOW *win, int dir) {
	mt_MenuScroll( gl_appvar, win, dir);
}

/* TODO function under developped. Dont use it */
static inline int     MenuAttach  ( int flag, OBJECT *tree, short item, MENU *mdata) {
	return mt_MenuAttach( gl_appvar, flag, tree, item, mdata);
}

/** @} */ 

/* Object library */
/** @addtogroup Object
 * @{ */

/**@brief See mt_ObjcChar() */
static inline int ObjcChar ( OBJECT *tree, int idx, int put) {
	return mt_ObjcChar (gl_appvar, tree, idx, put);
}

/**@brief See mt_ObjcChange() */
static inline int ObjcChange (int mode, void *data, int __index, int state, int redraw) {
	return mt_ObjcChange( gl_appvar, mode, data, __index, state, redraw);
}

/**@brief See mt_ObjcDraw() */
static inline int ObjcDraw ( int mode, void *win, int __index, int child_depth) {
	return mt_ObjcDrawParent ( gl_appvar, mode, win, __index, 0, child_depth);
}

/**@brief See mt_ObjcDrawParent() */
static inline int ObjcDrawParent ( int mode, void *win, int __index, int parent_depth, int child_depth) {
	return mt_ObjcDrawParent ( gl_appvar, mode, win, __index, parent_depth, child_depth);
}

/**@brief See mt_ObjcIsHidden() */
static inline int ObjcIsHidden ( int mode, void *win, int __index) {
	return mt_ObjcIsHidden    ( gl_appvar, mode, win, __index);
}

/**@brief See mt_ObjcDup() */
static inline OBJECT* ObjcDup ( OBJECT *tree, WINDOW *win) {
	return mt_ObjcDup         ( gl_appvar, tree, win);
}

/**@brief See mt_ObjcNDup() */
static inline OBJECT *ObjcNDup ( OBJECT *src,  WINDOW *win, int nb) {
	return mt_ObjcNDup        ( gl_appvar, src,  win, nb);
}

/**@brief See mt_ObjcEdit() */
static inline int ObjcEdit ( int mode, WINDOW *win, int obj, int val, INT16 *idx, int kind) {
	return mt_ObjcEdit        ( gl_appvar, mode, win, obj, val, idx, kind);
}

/**@brief See mt_ObjcFree() */
static inline void ObjcFree ( OBJECT *tree) {
	mt_ObjcFree        ( gl_appvar, tree);
}

/**@brief See mt_ObjcString() */
static inline char* ObjcString ( OBJECT *tree, int __index, char *put) {
	return mt_ObjcString      ( gl_appvar, tree, __index, put);
}

/**@brief See mt_ObjcStrCpy() */
#define ObjcStrCpy(tree,index,str)		strcpy(mt_ObjcString(gl_appvar,tree,index,NULL),str)

/**@brief See mt_ObjcStr3d() */
static inline void ObjcStr3d ( OBJECT *tree) {
	mt_ObjcStr3d       ( gl_appvar, tree);
}

/**@brief See mt_ObjcWindDraw() */
static inline int ObjcWindDraw ( WINDOW *win, OBJECT *tree, int __index, int depth, int xclip, int yclip, int wclip, int hclip) {
	return mt_ObjcWindDraw    ( gl_appvar, win, tree, __index, depth, xclip, yclip, wclip, hclip);
}

/**@brief See mt_ObjcWindDrawParent() */
static inline int ObjcWindDrawParent( WINDOW *win, OBJECT *tree, int __index, int parent_depth, int child_depth, int xclip, int yclip, int wclip, int hclip) {
	return mt_ObjcWindDrawParent( gl_appvar, win, tree, __index, parent_depth, child_depth, xclip, yclip, wclip, hclip);
}

/**@brief See mt_ObjcWindChange() */
static inline int ObjcWindChange ( WINDOW *win, OBJECT *tree, int __index, int xclip, int yclip, int wclip, int hclip, int state) {
	return mt_ObjcWindChange  ( gl_appvar, win, tree, __index, xclip, yclip, wclip, hclip, state);
}

/**@brief See mt_ObjcTree() */
static inline OBJECT *ObjcTree ( int mode, WINDOW *win) {
	return mt_ObjcTree		   ( gl_appvar, mode, win);
}

static inline int ObjcAttachVar( int mode, WINDOW *win, int __index, int *var, int value_when_selected ) {
	int mt_ObjcAttach ( APPvar *, int, WINDOW *, int, int, ...);
	return mt_ObjcAttach( gl_appvar, mode, win, __index, BIND_VAR, var, value_when_selected);
}

static inline int ObjcAttachBit( int mode, WINDOW *win, int __index, int *var, int bit ) {
	int mt_ObjcAttach ( APPvar *, int, WINDOW *, int, int, ...);
	return mt_ObjcAttach( gl_appvar, mode, win, __index, BIND_BIT, var, bit);
}

static inline int ObjcAttachFormFunc( WINDOW *win, int __index, func_bind fn, void *data ) {
	int mt_ObjcAttach ( APPvar *, int, WINDOW *, int, int, ...);
	return mt_ObjcAttach( gl_appvar, OC_FORM, win, __index, BIND_FUNC, fn, data);
}

static inline int ObjcAttachTBFunc( WINDOW *win, int __index, func_bind fn, void *data ) {
	int mt_ObjcAttach ( APPvar *, int, WINDOW *, int, int, ...);
	return mt_ObjcAttach( gl_appvar, OC_TOOLBAR, win, __index, BIND_FUNC, fn, data);
}

static inline int ObjcAttachMenuFunc( WINDOW *win, int __index, func_bindmenu fn, void *data ) {
	int mt_ObjcAttach ( APPvar *, int, WINDOW *, int, int, ...);
	return mt_ObjcAttach( gl_appvar, OC_MENU, win, __index, BIND_FUNC, fn, data);
}

/** @} */

/* Window library */
/** @addtogroup Window
 * @{ */
 
/**@brief See mt_WindCreate() */ 
static inline WINDOW* WindCreate ( int GemAttrib, int x, int y, int w, int h) {
	return mt_WindCreate ( gl_appvar, GemAttrib, x, y, w, h);
}

/**@brief See mt_WindOpen() */ 
static inline int WindOpen ( WINDOW *win, int x, int y, int w, int h) {
	return mt_WindOpen ( gl_appvar, win, x, y, w, h);
}

/**@brief See mt_WindClose() */ 
static inline int WindClose ( WINDOW *win) {
	return mt_WindClose ( gl_appvar, win);
}

/**@brief See mt_WindDelete() */ 
static inline int WindDelete  ( WINDOW *win) {
	return mt_WindDelete  ( gl_appvar, win);
}

/**@brief See mt_WindHandle() */ 
static inline WINDOW* WindHandle ( int handle) {
	return mt_WindHandle ( gl_appvar, handle);
}

/**@brief See mt_WindSet() */ 
static inline int WindSet ( WINDOW *win, int mode, int par1, int par2, int par3, int par4) {
	return mt_WindSet ( gl_appvar, win, mode, par1, par2, par3, par4);
}
#define WindSetGrect( win, mode, grect ) WindSet( win, mode, (grect)->g_x, (grect)->g_y, (grect)->g_w, (grect)->g_h )

/**@brief See mt_WindGet() */ 
static inline int WindGet ( WINDOW *win, int mode, INT16* par1, INT16* par2, INT16* par3, INT16* par4) {
	return mt_WindGet ( gl_appvar, win, mode, par1, par2, par3, par4);
}
static inline void* WindGetPtr ( WINDOW *win, int mode) {
	return mt_WindGetPtr ( gl_appvar, win, mode);
}
#define WindGetGrect( win, mode, grect ) WindGet( win, mode, &((grect)->g_x), &((grect)->g_y), &((grect)->g_w), &((grect)->g_h) )
static inline int WindXGet ( WINDOW *win, int mode, GRECT *clip, INT16* par1, INT16* par2, INT16* par3, INT16* par4) {
	return mt_WindXGet ( gl_appvar, win, mode, clip, par1, par2, par3, par4);
}
#define WindXGetGrect( app, win, mode, clip, grect ) WindXGet( win, mode, clip, &((grect)->g_x), &((grect)->g_y), &((grect)->g_w), &((grect)->g_h) )

/**@brief See mt_WindSetStr() */ 
static inline int WindSetStr ( WINDOW *win, int mode, char *str) {
	return mt_WindSetStr ( gl_appvar, win, mode, str);
}

/**@brief See mt_WindSetPtr() */ 
static inline int WindSetPtr ( WINDOW *win, int mode, void *par1, void *par2) {
	return mt_WindSetPtr ( gl_appvar, win, mode, par1, par2);
}

/**@brief See mt_WindCalc() */ 
static inline int WindCalc ( int type, WINDOW *wind, int xin, int yin, int win, int hin, INT16 *xout, INT16 *yout, INT16 *wout, INT16 *hout) {
	return mt_WindCalc ( gl_appvar, type, wind, xin, yin, win, hin, xout, yout, wout, hout);
}
#define	WindCalcGrect( mode, win, inrect, outrect ) \
                          WindCalc( mode, win, \
                                       (inrect)->g_x, (inrect)->g_y, (inrect)->g_w, (inrect)->g_h, \
                                       &(outrect)->g_x, &(outrect)->g_y, &(outrect)->g_w, &(outrect)->g_h )

/**@brief See mt_WindSlider() */ 
static inline void WindSlider ( WINDOW *win, int mode) {
	mt_WindSlider ( gl_appvar, win, mode);
}

/**@brief See mt_WindClear() */ 
static inline void WindClear ( WINDOW *win) {
	mt_WindClear ( gl_appvar, win);
}

/**@brief See mt_stdWindClear() */ 
void __CDECL stdWindClear( WINDOW *win, short buff[8]);

/**@brief See mt_WindAttach() */ 
static inline WINDOW* WindAttach ( int handle) {
	return mt_WindAttach  ( gl_appvar, handle);
}

/**@brief See mt_WindTop() */ 
static inline void WindTop ( WINDOW *win) {
	mt_WindTop ( gl_appvar, win);
}

/**@brief See mt_WindNew() */ 
static inline void WindNew ( void) {
	mt_WindNew ( gl_appvar);
}

/* see glue.c */
WINDOW* WindFind    ( int mode, ...);

/** @} */ 

/* Slider library (for window formular) */
/** @addtogroup Slider
 * @{ */

/**@brief See mt_SlidCreate() */
static inline void*	SlidCreate  ( short min, short max, short sinc, short linc, short value, int ori, int upd) {
	return mt_SlidCreate ( gl_appvar, min, max, sinc, linc, value, ori, upd);
}

/**@brief See mt_SlidDelete() */
static inline void	SlidDelete  ( void *slid) {
	mt_SlidDelete ( gl_appvar, slid);
}

/**@brief See mt_SlidAttach() */
static inline void  	SlidAttach  ( void *slid, int mode, WINDOW *win, int up, int bsld, int sld, int dn) {
	mt_SlidAttach ( gl_appvar, slid, mode, win, up, bsld, sld, dn);
}

/**@brief See mt_SlidGetValue() */
static inline short 	SlidGetValue( void *slid) {
	return mt_SlidGetValue( gl_appvar, slid);
}

/**@brief See mt_SlidSetFunc() */
static inline void  	SlidSetFunc ( void *slid, func_doslid func, void *data) {
	mt_SlidSetFunc ( gl_appvar, slid, func, data);
}

/**@brief See mt_SlidSetSize() */
static inline void  	SlidSetSize ( void *slid, int size) {
	mt_SlidSetSize ( gl_appvar, slid, size);
}

/**@brief See mt_SlidSetValue() */
static inline void  	SlidSetValue( void *slid, short value) {
	mt_SlidSetValue( gl_appvar, slid, value);
}

/**@brief See mt_SlidSetUpdat() */
static inline void  	SlidSetUpdat( void *slid, int upd) {
	mt_SlidSetUpdat( gl_appvar, slid, upd);
}
/** @} */ 

/* Ressource library */
/** @addtogroup Resource
 * @{ */

/**@brief See mt_RsrcLoad() */ 
static inline int RsrcLoad ( const char *name) {
	return mt_RsrcLoad ( gl_appvar, name);
}

/**@brief See mt_RsrcXtype() */ 
static inline void RsrcXtype ( int mode, OBJECT **rs_trindex, int num_tree) {
	mt_RsrcXtype ( gl_appvar, mode, rs_trindex, num_tree);
}

/**@brief See mt_RsrcFixCicon() */ 
static inline void RsrcFixCicon ( OBJECT *object, int num_obs, int num_cib, INT16 palette[][4], void *fix) {
	mt_RsrcFixCicon ( gl_appvar, object, num_obs, num_cib, palette, fix);
}

/**@brief See mt_RsrcFreeCicon() */ 
static inline void RsrcFreeCicon ( void *fix) {
	mt_RsrcFreeCicon ( gl_appvar, fix);
}

/**@brief See mt_RsrcUserDraw() */ 
static inline int RsrcUserDraw ( int mode, WINDOW *win, int __index, func_userdraw draw, void *data) {
	return mt_RsrcUserDraw ( gl_appvar, mode, win, __index, draw, data);
}

/**@brief See mt_RsrcFree() */ 
#define RsrcFree() mt_rsrc_free(gl_appvar->aes_global)

/**@brief See mt_RsrcXload() */ 
static inline void* RsrcXload ( const char *name) {
	return mt_RsrcXload ( gl_appvar, name);
}

/**@brief See mt_RsrcXfree() */ 
static inline void RsrcXfree ( void *rsc) {
	mt_RsrcXfree ( gl_appvar, rsc);
}

/**@brief See mt_RsrcGaddr() */ 
static inline int RsrcGaddr ( void *rsc, int type, int __index, void *addr) {
	return mt_RsrcGaddr ( gl_appvar, rsc, type, __index, addr);
}

/**@brief See mt_RsrcGhdr() */ 
static inline rscHDR* RsrcGhdr ( void *rsc) {
	return mt_RsrcGhdr ( gl_appvar, rsc);
}

/** @} */ 
/** @brief Release from memory resource file loaded by mt_RsrcLoad(). */

/* Utility library */

static inline void snd_rdw ( WINDOW *win) {
	mt_snd_rdw ( gl_appvar, win);
}
static inline void snd_arrw ( WINDOW *win, int msg) {
	mt_snd_arrw ( gl_appvar, win, msg);
}

static inline void give_iconifyxywh( INT16 *x, INT16 *y, INT16 *w, INT16 *h) {
	mt_give_iconifyxywh( gl_appvar, x, y, w, h);
}

static inline void ClipOn  ( W_GRAFPORT *graf, GRECT *r) {
	mt_ClipOn  ( gl_appvar, graf, r);
}
static inline void ClipOff ( W_GRAFPORT *graf) {
	mt_ClipOff ( gl_appvar, graf);
}

/* draw_page was a private function that some developpers use.
 * NOW, this is a DEPRECATED function (please use EvntRedrawGrect instead)
 * the following is just a hack for compatibility between windom 1.21 and windom 2.0
 */
static inline void draw_page( WINDOW *win, int x, int y, int w, int h) {
    GRECT rect;
    rc_set( &rect, x, y, w, h );
    mt_EvntRedrawGrect( gl_appvar, win, &rect );
}

static inline int ShelHelp ( char *file, char *chapter) {
	return mt_ShelHelp ( gl_appvar, file, chapter);
}

static inline int ShelWrite ( char *prg, char *cmd, void *env, int av, int single) {
	return mt_ShelWrite ( gl_appvar, prg, cmd, env, av, single);
}

static inline void GrectCenter ( int w, int h, INT16 *x, INT16 *y ) {
	mt_GrectCenter ( gl_appvar, w, h, x, y );
}

#if 0 /* TODO */
void    DebugWindom ( const char *format, ...);
#endif /* 0*/


__END_DECLS
#endif
