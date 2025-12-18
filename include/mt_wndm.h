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
 * $Source: /cvsroot/windom/windom/include/mt_wndm.h,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/06/22 09:40:21 $
 *   $Revision: 1.60 $
 */

#ifndef __MT_WINDOM__
#define __MT_WINDOM__

#define __GEMLIB_OLDNAMES
#include <mt_gem.h>
#include <mt_gemx.h>

#ifdef __PUREC__
# define __MSHORT__
# include <stddef.h>
#endif

#ifndef __WINDOM_LIST__
#include <windom/list.h>
#endif

__BEGIN_DECLS

/*************************************************************
 *  WINDOM DEFINITIONS
 *************************************************************/

/* Macros functions 
 *************************/
/** @addtogroup macros
 * @{ */
/** AES number version */
#define mt_AppAESversion(ap)   ((ap)->aes_global[0]) 
/** Number of concurrent applications */ 
#define mt_AppAESnumapps(ap)   ((ap)->aes_global[1]) 
/** Application identifier */ 
#define mt_AppAESapid(ap)      ((ap)->aes_global[2])  
#define mt_AppId(ap)      	   ((ap)->aes_global[2])  /**< shorter name for mt_AppAESapid() */
/** Address of the Global Array application */
#define mt_AppAESappglobal(ap) (*((long *)&((ap)->aes_global[3])))  
/** Pointer to resource loaded by rsrc_load() */
#define mt_AppAESrscfile(ap)   ((OBJECT **)(*((long *)&((ap)->aes_global[5]))))  
/** Maximal character size use to print on screen */
#define mt_AppAESmaxchar(ap)   ((ap)->aes_global[13]) 
/** Minimal character size use to print on screen */
#define mt_AppAESminchar(ap)   ((ap)->aes_global[14]) 

#ifndef MIN
/** Returns maximal value of \e a and \e b */
#define MAX(a,b) ((a)>(b)?(a):(b))
/** Returns minimal value of \e a and \e b */
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

/** Use this macro on unused parameter avoid warning message from your compiler */
#define UNUSED( a)  ((void )(a))
/** Return \b TRUE if (xp,yp) coordinate belongs to area (x,y,w,h) */
#define IS_IN( xp, yp, x, y, w, h) (xp>=x && yp>=y && xp<x+w && yp<y+h)
/** Returns FORM structure linked to a window formular */
#define FORM(app,win)   (((W_FORM*)mt_DataSearch( app, win, WD_WFRM))->root)
/** Returns TOOL structure linked to a window containing a toolbar */
#define TOOL( win) ((win)->tool.root)

#ifndef ADR   /* Portab can defined this macro */
/** Convert a 32 bit adress value into a couple of 16 bit values */
# define ADR(c)     (int)((long)(c)>>16), (int)(long)(c)
#endif
/** Set to value \c value the bit \c bit of a bit field \c field */
#define SET_BIT(field,bit,val) field = (val)?((field)|(bit)):((field) & ~(bit))
/** @} */

#ifndef TRUE
# define TRUE 1
# define FALSE 0
#endif

/* Structures
 ************************/
 
/* WinDom version */

#define __WINDOM_MAJOR__     2
#define __WINDOM_MINOR__     0
#define __WINDOM_REVISION__  1
#define __WINDOM_BETATAG__   ""

struct w_version {
    short patchlevel;
    short release;
    char *date;
    char *time;
    char *cc_name;
    short cc_version;
};

/* Application related definitions */

typedef INT16 W_COLOR[3];

#ifdef __GNUC__
struct _window;
struct _appvar;
#endif


/* long GRECT version */

#ifndef __LGRECT
# define __LGRECT
typedef struct long_graphic_rectangle
{
	long g_x;			/**< TODO */
	long g_y;			/**< TODO */
	long g_w;			/**< TODO */
	long g_h;			/**< TODO */
} LGRECT;
#endif

#ifdef __WINDOM__  /* for the non-mt version, the last APPvar parameter doesn't exist */
typedef void __CDECL (* func_evntdata)(struct _window *, short buff[8], void *);
typedef void __CDECL (* func_evnt    )(struct _window *, short buff[8]);
typedef void __CDECL (* func_bind    )(struct _window *, int, int, void *);
typedef void __CDECL (* func_bindmenu)(struct _window *, int, int, void *);
typedef void __CDECL (* func_doslid  )(struct _window *, int, short, void *);
typedef void __CDECL (* func_userdraw)(struct _window *, PARMBLK *, void *);
#else
typedef void __CDECL (* func_evntdata)(struct _window *, short buff[8], void *, struct _appvar *);
typedef void __CDECL (* func_evnt    )(struct _window *, short buff[8], struct _appvar *);
typedef void __CDECL (* func_bind    )(struct _window *, int, int, void *, struct _appvar *);
/** Callback function for menu entry object registered by mt_ObjcAttachMenuFunc().
 * @param win window descriptor (can be NULL if menu is the desktop menu,
 * @param item object index of the entry,
 * @param title object index of the menu, 
 * @param data pointer to user data,
 * @param ap application descriptor.
 */
typedef void __CDECL (* func_bindmenu)(struct _window *win, int item, int title, void *data, struct _appvar *ap);
typedef void __CDECL (* func_doslid  )(struct _window *, int, short, void *, struct _appvar *);
/** callback function for USERDEF objects registered by mt_RsrcUserDraw()
 *
 *  @param win window descriptor
 *  @param pblk contains informations related to the object and the USERDEF
 *	       structure used (object state, clip area size, object previous and current
 *	       state), refer to gemlib documentation or any other AES documentation for details.
 *  @param data is a user data (set by calling mt_RsrcUserDraw())
 *  @param ap application descriptor
 *
 *  The drawing function has the following limitations :
 *   - AES call should never be used (because AES is not reentrant), and
 *     consequently, WinDom functions using AES calls should never be called,
 *   - do not use too many local variables : this function is
 *     executed by AES and then in supervisor mode,
 *   - the drawing function should never clip screen because it is 
 *     already performed by WinDom.
 *  
 */
typedef void __CDECL (* func_userdraw)(struct _window *win, PARMBLK *pblk, void *data, struct _appvar *ap);
#endif

/** @addtogroup Window
 * @{ */
/**
 * @struct W_GRAFPORT
 * This structure groups informations related to graphic window VDI workstation.
 */
typedef struct {
        INT16   handle;     /**< VDI virtual workstation associated to the window */
        W_COLOR *palette;   /**< If non null, window color palette */
        GRECT   clip;       /**< Current coordinate of aera clipped by WinDom kernel */
    } W_GRAFPORT;

/**
 * @struct W_MENU
 * This structure groups variables related to handle a menu 
 * into a window.
 */
typedef struct {
        OBJECT *root; /**< Address of root objects tree. It is an object tree memory duplicated */
        OBJECT *real; /**< Address of objects tree used to duplicated \e root */
        int scroll;   /**< Private, use to handle scroll menu widget */
        void *bind;   /**< Private, list of binded events */
        void (*hilight)( struct _appvar *app, struct _window *win, int title, int item);
#ifdef BETAMENU
        /* Activ menu */
        int ac_title, ac_item;
#endif
    } W_MENU;

/**
 * @struct W_FORM
 * This structure groups variables related to handle a formular 
 * into a window.
 */
typedef struct {
        OBJECT *root; /**< Address of root objects tree. It is an object tree memory duplicated */
        OBJECT *real; /**< Address of objects tree used to duplicated \e root */
        int  *save;
        void *bind;   /**< Private, list of binded events */
        int edit, nb_ob;
        INT16 cursor;
    } W_FORM;

/**
 * @struct W_ICON
 * Structure grouping information related to iconified window
 */
typedef struct {
        char *name;
        INT16 x, y, w, h;  /* Original coordinate/size of the uniconified window */
    } W_ICON;

/**
 * @struct _window
 * @brief Window descriptor structure.
 */
struct _window {
  int         handle; /**< AES handle of the window */
  int         attrib; /**< window widget attributes */
  long        status; /**< WinDom status of the window (see constants \link win_status WS_\endlink) */
  W_GRAFPORT  *graf;   /**< VDI virtual workstation opened for the window */
  W_MENU      menu;   /**< Information related to the window menu (if exist) */
  W_FORM      tool;   /**< Information related to the window toolbar (if exist) */
  W_ICON      icon;   /**< Information needed when window is iconified */
  GRECT       createsize;    /**< Original window size */
  int         __rsvd[11];    /**< reserved */
  char        *name, *info;  /**< window name and information bar */
  INT16       w_max, h_max;  /**< maximal window size */
  INT16       w_min, h_min;  /**< minimal window size */
  long        xpos, ypos;         /**< relative data position in the window */
  long        xpos_max, ypos_max; /**< Maximal values of previous variables */
  INT16       w_u, h_u; /**< vertical and horizontal scroll offset */
  struct _window *next; /**< next window */
  int         type;     /**< user window type */
  LIST        data;     /**< window data - reserved */
  LIST        binding;  /**< window events - reserved */
};

/**
 * @struct WINDOW
 * @brief WINDOW is an alias of struct _window.
 */
typedef struct _window WINDOW;

/**
 * @struct WINvar
 * @brief window global variable 
 */
typedef struct {
        WINDOW *first;    /**< Pointer to the first window in the window list. */
        WINDOW *front;    /**< Pointer to the global top window, can be \b NULL
			   * if the application is in the background. */
        WINDOW *appfront; /**< Pointer to the local top window, can never be
			   * \b NULL except if application has no opened windows. */
    } WINvar;
/**@} */

/** @addtogroup Event 
 * @{ */
/**
 * @struct EVNTvar
 * @brief Parametrization of mt_EvntWindom() calling.
 *
 * Instead of giving a complexe parameters to EvntWindom(), as it 
 * is done with the AES function evnt_multi(), we have grouped these
 * parameters in a structure variable, then we redefine only needed
 * parameters or use the default/current values. 
 */

typedef struct {
  long timer; /**< time idle (in msec) between two \b MU_TIMER events. */
  int bclick; /**< Number of click wished (1, 2 or 3) in \b MU_BUTTON event.*/
  int bmask;  /**< Mouse buttons to wait for in \b MU_BUTTON event.
	       * - 0x1 : left button
	       * - 0x2 : right button
	       * - 0x4 : middle button
	       */
  int bstate; /**< Button state to wait for in \b MU_BUTTON event.
	       * - 0x0 : all buttons released
	       * - 0x1 : left button depressed
	       * - 0x2 : right button depressed
	       * - 0x4 : middle button depressed
	       */
  int m1_flag; /**< type of mouse motion in \b MU_M1 event.
		* - \b MO_ENTER (0) : wait for mouse to enter rectangle.
		* - \b MO_LEAVE (1) : wait for mouse to leave rectangle.
		*/
  int m1_x, m1_y, m1_w, m1_h; /**< coordinate of rectangle for \b MU_M1 event */
  int m2_flag; /**< type of mouse motion in \b MU_M2 event.
		* - \b MO_ENTER (0) : wait for mouse to enter rectangle.
		* - \b MO_LEAVE (1) : wait for mouse to leave rectangle.
		*/
  int m2_x, m2_y, m2_w, m2_h; /**< coordinate of rectangle for \b MU_M2 event */
  INT16 mx, my;  /**< After an event, contain mouse position at screen. */
  INT16 mbut;    /**< After an event, contains button state. Same value than \e bstate. */
  INT16 mkstate; /**< After a \b MU_BUTTON event, contains keyboard shift state.
		  * - K_RSHIFT	(0x01) : right shift depressed,
		  * - K_LSHIFT	(0x02) : left shift depressed,
		  * - K_CTRL	(0x04) : control  depressed,
		  * - K_ALT	(0x08) : alternate depressed.
		  */
  INT16 keybd;    /**< After a \b MU_KEYBD event, contains code of key hited.
		   * - the lower eight bit contains the ASCII code,
		   * - the higher eight bit contains the scancode.
		   */
  INT16 nb_click; /**< After a \b MU_BUTTON event, contains the number of clicks occured. */
} EVNTvar;
/** @} */

#ifndef APPRSVD
#define APPRSVD void
#endif

/** @addtogroup Application 
 * @{ */
/**
 * @struct APPvar
 *
 * @brief the application descriptor structure.
 *
 * When function mt_ApplInit() is invoqued to initiate a GEM/WinDom
 * session, an instance of this structure is created. This data describes
 * the GEM application and contains informations about the process itself,
 * the GEM environnement and some variables which are used to parametrize
 * some functions and WinDom behavior.
 * 
 * Almost WinDom functions require, as first parameter, an application
 * descriptor, i.e. a pointer to an APPvar variable. Most of time, application
 * descriptor is the current application process. However WinDom can run in
 * a multi-thread environment and this parameter can address an another
 * application (a thread for example).
 *
 * Only \e pipe and \e evnt fields can be written. Other fields are set
 * by mt_ApplInit() and should not be modified.
 */

typedef struct _appvar {
  /* Public part */  

  short  *aes_global; /**< The AES global array (use for mt_AES calls) */
  INT16   aeshdl;     /**< The screen VDI handle used by AES */		
  INT16   x,y,w,h;    /**< Coordinates and size of desktop workarea (not screen size) */
	
  short   color;      /**< Number of colors of screen display */
  short   nplanes;    /**< Number of planes of screen display (e.g. color = 2^nplanes) */
  OBJECT  *menu;          /**< address of the desktop menu tree */
  INT16   work_in[11];    /**< array input of v_opnvwrk() function */
  INT16   work_out[57];   /**< array output of v_opnvwrk() function */
  long   aes4;       /**< bitfield indicating some AES 4 features supported by the OS 
                       (see constants \link aes4_status AES4_\endlink) */
  short   avid;       /**< Application AES id of the AV server (or -1) */
  short   ntree;      /**< Number of object trees in the loaded resource */
  char    *pipe;      /**< A user buffer used for AES message exchange (more).
		       * ApplInit() reserves a 256 byte buffer allocated in global memory
		       * mode pointed by this variable. This buffer can be used to
		       * send information safely in MU_MESAG event. 
		       *
		       * For example, 
		       * \b AV_STARTPROG (AV-protocol) uses \e app->evnt.buff[3-4] to emit a
		       * address pointing to a string character. This address should be
		       * \e app.pipe.
		       */
  int	  error;      /**< Last error encountered */
  
  W_GRAFPORT graf;    /**< Information about the application virtual workstation */
  EVNTvar evnt;       /**< Parameters for evnt_multi() calling */
  WINvar  wglb;       /**< Information about application windows */
  
  APPRSVD *priv;    /**< Private variables.
		     * Private variables are grouped in a hidden structure in order to
		     * prevent their utilisation.
		     */	
  short   gdos; /**< Contains the number of fonts loaded (FIXME: should contain the value returns by vq_gdos() ?) */
} APPvar;
/** @} */

/**
 * @struct rscHDR
 * Header structure of a GEM resource file loaded by
 * mt_RsrcXload().
 */
typedef struct {
  long        nobs;     /**< Number of elements in object field */
  long        ntree;    /**< Number of elements in trindex field */
  long        nted;     /**< Number of elements in tedinfo field */
  long        ncib;     /**< Number of elements in ciconblk field */
  long        nib;      /**< Number of elements in iconblk field*/
  long        nbb;      /**< Number of elements in bitblk field */
  long        nfstr;    /**< Number of elements in frstr field */
  long        nfimg;    /**< Number of elements in frimg field */
  OBJECT      *object ; /**< Table of OBJECTs */
  TEDINFO     *tedinfo; /**< Table of TEDINFOs */
  ICONBLK     *iconblk; /**< Table of ICONBLKs */
  BITBLK      *bitblk;  /**< Table of BITBLKs */
  CICON       *cicon;   /**< Table of CICONs */
  CICONBLK    *ciconblk;/**< Table of CICONBLKs */
  char        **frstr;  /**< Table of free string */
  BITBLK      **frimg;  /**< Table of free image data */
  OBJECT      **trindex;/**< Table of objects trees */
} rscHDR;


/* BubbleGEM */

typedef struct {
    unsigned short delay;       /* time to wait before bubble released */
    unsigned short flag ;       /* see constants BGC_ */
} BHLP;


/* text attributes used in userdef library */
typedef struct {
	short font;
	short size;
	short color;
} ATTRIB;


/* Constants
 ***********************/

/** @addtogroup aes4_status
 * @{ */
/**@brief \b WM_BOTTOM message supported */
#define AES4_BOTTOM         0x000001L
/**@brief \b WM_ICONIFY message supported */
#define AES4_ICONIFY        0x000002L
/**@brief \b WM_ICONIFYXYWH message supported */
#define AES4_ICONIFYXYWH    0x000004L
/**@brief \b SMALLER widget (iconifier) supported by wind_create() */
#define AES4_SMALLER        0x000008L
/**@brief \b BOTTOMER widget (backdropper) supported by wind_create() */
#define AES4_BOTTOMER       0x000010L
/**@brief appl_search() is available */
#define AES4_APPSEARCH      0x000020L
/**@brief MagiC extended file selector is available */
#define AES4_FSLX           0x000040L
/**@brief \b WF_BEVENT supported by wind_set() */
#define AES4_BEVENT         0x000080L
/**@brief \b WM_UNTOPPED message supported */
#define AES4_UNTOPPED       0x000100L
/**@brief \b WF_TOOLBAR mode supported by wind_set() */
#define AES4_TOOLBAR        0x000200L
/**@brief appl_control() is available */
#define AES4_APPLCONTROL    0x000400L
/**@brief graf_mouse() supports extended modes */
#define AES4_XGMOUSE        0x000800L
/**@brief \b MN_SELECTED message is extended */
#define AES4_MNSELECTED	    0x001000L
/**@brief \b WF_MENU mode supported by wind_set() */
#define AES4_MENUBAR	    0x002000L
/**@brief \b WF_FIRSTAREAXYWH mode supported by wind_set() */
#define AES4_FIRSTAREAXYWH  0x004000L
/**@brief \b WF_WORKXYWH is supported by the AES */
#define AES4_SETWORKXYWH    0x010000L
/** @} */
/* Definitions for ApplGet()/ApplSet() 
 */

/* modes */
#define APS_ICONSIZE        0
#define APS_FLAG            1
#define APS_WINBG           2
#define APS_KEYCOLOR        3
#define APS_STRSTYLE        4
#define APS_BUTSTYLE        5
#define APS_EXITSTYLE       6
#define APS_TITLESTYLE      7
#define APS_3DEFFECT        8
#define APS_MENUEFFECT      9
#define APS_BUBBLESTYLE     10
#define APS_POPUPSTYLE      11
#define APS_POPUPWIND       12
#define APS_WINDOWS         13
#define APS_XTEDINFOSTYLE   14
#define APS_XLONGEDITSTYLE  15
#define APS_XEDITTEXTSTYLE  16
#define APS_XEDITLABELSTYLE 17
#define APS_FRAMEWIDGETSIZE 18

/* values */
#define DEFVAL     -1
#define CENTER      0
#define WMOUSE      1
#define UP_RIGHT    2
#define UP_LEFT     3
#define DN_RIGHT    4
#define DN_LEFT     5
#define WFORM       6

/* Bitfield for APS_FLAG */
#define FLG_KEYMOUSE  0x0001
#define FLG_BUTMOUSE  0x0002
#define FLG_NOPAL     0x0004
#define FLG_MNSCRL    0x0010
#define FLG_NOKEYMENU 0x0020
#define FLG_NOMGXFSEL 0x0040
#define FLG_MNSYSTEM  0x0080

/* app.key_color */
#define LTMFLY_COLOR  0x0010

/* extended ob_type values */
#ifndef __MYDIAL__
#define DIALMOVER  0x11
#define DCRBUTTON  0x12
#define UNDERLINE  0x13
#define TITLEBOX   0x14
#define HELPBUT    0x15
#define CIRCLEBUT  0x16
#define POPUPSTRG  0x17
#define KPOPUPSTRG 0x18
#define SLIDEPART  0x19
#define LONGINPUT  0x1A
#define UNDOBUT    0x1F
#endif
#define XBOXLONGTEXT	0x09	/* BOXTEXT with text on several lines */
#define ONGLET			0x10	/* thumb */
#define	MENUTITLE		0x0F	/* menu 	*/
#define XEDIT			0x0E	/* editable text  */

/* ObjcEdit(): additional modes */
#define ED_CURS      10 	/**< return the position of the text cursor */
#define ED_BLC_OFF   11 	/**< unselect the bloc (if any) */
#define ED_BLC_START 12
#define ED_BLC_END   13

/* RsrcXtype() mode */
#define RSRC_XTYPE 0x01   /* extended object type + color icons */
#define RSRC_X3D   0x02   /* standard 3D objects */
#define RSRC_XALL  0xFF   /* all (extended type + 3D objects)  */

/* WindFind() mode */
#define WDF_NAME  0
#define WDF_INFO  1
#define WDF_ID    2
#define WDF_MENU  3
#define WDF_FORM  4
#define WDF_TOOL  5
#define WDF_DATA  6

/* Cumulates window widgets */
#define WAT_ALL     0x4FFF  /* All widgets */
#define WAT_NOINFO  0x4FEF  /* All widget execpt the info bar */
#define WAT_FORM    0x400F  /* Formular: name, mover, closer, fuller, smaller */
#define WAT_MFORM   0x400D  /* Modal formular: name, mover, smaller, fuller */

/*  Windows status */
/** @addtogroup win_status
 * @{ */
/**@brief Window is opened */
#define WS_OPEN         0x0001  
/**@brief Window is iconified */
#define WS_ICONIFY      0x0002  
/**@brief Window has menu */
#define WS_MENU         0x0004  
/**@brief Window has toolbar */
#define WS_TOOLBAR      0x0008L  
/**@brief Window use graphic effects */
#define WS_GROW         0x0010  
/**@brief Window is untoppable */
#define WS_UNTOPPABLE   0x0020  
/**@brief Window is a formular */
#define WS_FORM         0x0040  
/**@brief Window is a duplicated formular */
#define WS_FORMDUP      0x0080  
/**@brief Window is modal */
#define WS_MODAL        0x0100  
/**@brief Window is a frame root
 * @deprecated To be replaced by COMPONENTs */
#define WS_FRAME_ROOT   0x0200  
/**@brief Window is a framed window
 * @deprecated To be replaced by COMPONENTs */
#define WS_FRAME        0x0400  
/**@brief Window is a custom handled window (the handle does not correspond to the AES one) */
#define WS_CUSTOM       0x0400
/**@brief All windows are iconified */
#define WS_ALLICNF      0x0800  
/**@brief Window has fulled size */
#define WS_FULLSIZE     0x1000  
/**@brief Window is closed because of Pexec-call (reserved by ShelWrite) */
#define WS_PEXEC        0x2000  
/**@brief Window is not created by application but inserted in WinDom list */
#define WS_FOREIGN      0x4000  
/**@brief Window structure is in use: should not be free-ed (see mt_WindDelete()) */
#define WS_WIN_IN_USE   0x00008000  
/**@brief Window structure should be free-ed as soon as possible */
#define WS_DESTROYED    0x00010000  

/** @} */
/*  WindSet() mode */
#define BEVENT_MODAL    0x1000
#define WF_ICONTITLE    1001
#define WF_ICONDRAW     1002
#define WF_HILIGHT      1003
#define WF_VWORK        1004
#define WF_SLIDXYWH     1005
#define WF_WORKCLIPXYWH 1006
#define WF_COMPONENT    1007
#define WF_MENUTNORMAL  1008

/* FormWindDo() mode */ 
#define FORM_EVNT   0x8000
 
/* WindSlider() mode */
#define HSLIDER         0x0001
#define VSLIDER         0x0002

/* WinDom specific messages */
#define WM_DESTROY      0x4400    /* old value 1000 */
/*#define WM_MNSELECTED   0x4401 (deprecated) */

/** message to control forms
 *
 * - msg[0] = #WM_FORM
 * - msg[1] = AES application ID of the sender
 * - msg[2] = 0
 * - msg[3] = handle of the window that contains the form
 * - msg[4] = index of the object selected
 * - msg[5] = mkstate (shift, control, alternate...)
 * - msg[6] = 0
 * - msg[7] = 0
 */
#define WM_FORM         0x4402
#define AP_LOADCONF     0x4403
/** message to send a mouse button event to a windom application
 *
 * - msg[0] = #AP_BUTTON
 * - msg[1] = AES application ID of the sender
 * - msg[2] = 0
 * - msg[3] = mx
 * - msg[4] = my
 * - msg[5] = mouse button
 * - msg[6] = mkstate (shift, control, alternate...)
 * - msg[7] = number of clicks
 */
#define AP_BUTTON       0x4404
/** message to send a keyboard event to a windom application
 *
 * - msg[0] = #AP_KEYBD
 * - msg[1] = AES application ID of the sender
 * - msg[2] = 0
 * - msg[3] = mkstate (shift, control, alternate...)
 * - msg[4] = keyboad code
 * - msg[5] = 0
 * - msg[6] = 0
 * - msg[7] = 0
 */
#define AP_KEYBD        0x4405
#define AP_DEBUG        0x4410
#define WM_UPLINE       0x4411
#define WM_DNLINE       0x4412
#define WM_LFLINE       0x4413
#define WM_RTLINE       0x4414
#define WM_UPPAGE       0x4415
#define WM_DNPAGE       0x4416
#define WM_LFPAGE       0x4417
#define WM_RTPAGE       0x4418
#define WM_PREREDRAW	0x4419
#define WM_ICONDRAW     0x441a
#define WM_SLIDEXY	0x441b
/* EV_MSG extensions. These message will never go thru the AES pipe */
#define WM_XTIMER       1
/** message internally used by windom to catch keyboard events
 *
 * - msg[0] = #WM_XKEYBD
 * - msg[1] = AES application ID of the sender
 * - msg[2] = 0
 * - msg[3] = mkstate (shift, control, alternate...)
 * - msg[4] = keyboad code
 * - msg[5] = 0
 * - msg[6] = 0
 * - msg[7] = 0
 */
#define WM_XKEYBD       2
/** message internally used by windom to catch mouse button events
 *
 * - msg[0] = #WM_XBUTTON
 * - msg[1] = AES application ID of the sender
 * - msg[2] = 0
 * - msg[3] = mx
 * - msg[4] = my
 * - msg[5] = mouse button
 * - msg[6] = mkstate (shift, control, alternate...)
 * - msg[7] = number of clicks
 */
#define WM_XBUTTON      3
#define WM_XM1          4
#define WM_XM2          5

/* EvntAdd() mode */
#define EV_BOT  0
#define EV_TOP  1

/* Reserved data magic number */
#define WD_WFRM 0x5746524DL /* 'WFRM' */
#define WD_WFRA 0x57465241L /* 'WFRA' */
#define WD_WFRI 0x57465249L /* 'WFRI' */
#define WD_WHLR 0x57484c52L /* 'WHLR': wind_get/wind_set/wind_calc handlers */


/* MenuPopUp() mode */
#define P_RDRW  0x0100
#define P_WNDW  0x0200
#define P_LIST  0x0400
#define P_CHCK  0x0800

/* ObjcChange() mode */
#define OC_TOOLBAR  0
#define OC_FORM     1
#define OC_OBJC     2
#define OC_MENU     3
#define OC_MSG      0x1000

/* ObjcAttach() type */
#define BIND_NONE   0
#define BIND_VAR    1
#define BIND_FUNC   2
#define BIND_BIT    3

/* SlidCreat() parameters */
#define SLD_HORI 0
#define SLD_VERT 1
#define SLD_DIFF 0
#define SLD_IMME 1

/* FontSel() flags */
#define VSTHEIGHT   0x01
#define BITMAP      0x02
#define MONOSPACED  0x04

/* Values for FrameSet()
 * @deprecated To be replaced by COMPONENTs */
#define FRAME_BORDER 1
#define FRAME_COLOR  2
#define FRAME_KEYBD  3
#define FRAME_SIZE   4
#define FRAME_FLAGS  5
#define FRAME_NOBG   6
#define FRAME_ACTIV  7
#define FRAME_TOPPED_ACTIV  8

/* Values for FrameGet()
 * @deprecated To be replaced by COMPONENTs */
#define FRAME_CELL   10

/* Flags for FrameAttach()
 * @deprecated To be replaced by COMPONENTs */
#define FRAME_WSCALE    0x0001
#define FRAME_HSCALE    0x0002
#define FRAME_WFIX      0x0004
#define FRAME_HFIX      0x0008
#define FRAME_NOBORDER  0x0010
#define FRAME_SELECT    0x0020

/* Values for BHLP.flag */
#define BGC_FONTCHANGED     0x01
#define BGC_NOWINSTYLE      0x02
#define BGC_SENDKEY         0x04
#define BGC_DEMONACTIVE     0x08
#define BGC_TOPONLY         0x10

/* AvStatus() return value */
#define V_SENDKEY       0x0001
#define V_ASKFILEFONT   0x0002
#define V_ASKCONFONT    0x0004  /* OPENCONSOLE */
#define V_ASKOBJECT     0x0008
#define V_OPENWIND      0x0010
#define V_STARTPROG     0x0020
#define V_ACCWINDOPEN   0x0040  /* ACCWINDCLOSED */
#define V_STATUS        0x0080  /* GETSTATUS */
#define V_COPY_DRAGGED  0x0100
#define V_PATH_UPDATE   0x0200  /* AV_PATH_UPDATE, WHAT_IZIT, DRAG_ON_WINDOW */
#define V_EXIT          0x0400
#define V_XWIND         0x0800
#define V_FONTCHANGED   0x1000
#define V_STARTED       0x2000
#define V_QUOTE         0x4000
#define V_FILEINFO      0x8000  /* FILECHANGED */

/* Valeur du mot 4 de VA_PROTOSTATUS
 * ou du mot 1 retourn par av_status()
 */ 

#define V_COPYFILE      0x0001  /* FILECOPIED */
#define V_DELFILE       0x0002  /* FILEDELETED */
#define V_VIEW          0x0004  /* FILEVIEWED */
#define V_SETWINDPOS    0x0008

/* AvInit() mode */
#define A_SETSTATUS     0x0001
#define A_START         0x0002
#define A_STARTED       0x0004
#define A_FONTCHANGED   0x0008
#define A_QUOTE         0x0010
#define A_PATH_UPDATE   0x0020

/* vqt_xname() flags */
#define FNT_OUTLINE     0x1 /* 0=bitmap 1=vectorielle */
#define FNT_MONO        0x2 /* 0=non proportionnelle, 1=proportionnelle */
#define FNT_SPEEDO      0x4
#define FNT_TRUETYPE    0x8
#define FNT_TYPE1       0x10

/* vqt_xfntinfo() flags */
#define XFNT_NAME       0x1
#define XFNT_FAMILLY    0x2
#define XFNT_STYLE      0x4
#define XFNT_FILE       0x8
#define XFNT_NBPTS      0x100
#define XFNT_LISTPTS    0x200

/* Global variables
 ***********************/

extern struct w_version WinDom;


/*********************************************************

  TODO: add an APPvar variable as 1st parameter
 
**********************************************************/

/* Function Prototypes
 ***********************/

/* Application Library */
/** @addtogroup Application 
 * @{ */
APPvar*	mt_ApplInit    ( void);
int     mt_ApplExit    ( APPvar *app);
int     mt_ApplGet     ( APPvar *app, int mode, short *v1, short *v2, short *v3, short *v4);
int     mt_ApplSet     ( APPvar *app, int mode, short v1, short v2, short v3, short v4);
int     mt_ApplName    ( APPvar *app, char *name, int handle);
int     mt_ApplWrite   ( APPvar *app, int to, int msg, int b3, int b4, int b5, int b6, int b7);
int     mt_ApplControl ( APPvar *app, int ap_cid, int ap_cwhat, void *ap_cout);
/** @} */

/* AV library */
/** @addtogroup Avlib
 * @{ */
int     mt_AvInit      ( APPvar *app, char *name, int status, long idle);
void    mt_AvExit      ( APPvar *app);
INT16*  mt_AvStatus    ( APPvar *app);
char*   mt_AvServer    ( APPvar *app);
int     mt_AvWaitfor   ( APPvar *app, int, INT16 *, long);

char*   AvStrfmt (int mode, char *src);
#define mt_AvStrfmt(a,b,c) AvStrfmt(b,c) /**< see AvStrfmt() */
/** @} */


/** @addtogroup Utility */
int     get_cookie  (long cookie, long *value);

/* BubbleGEM library */ 
/** @addtogroup BubbleGEM
 * @{ */
int     mt_BubbleCall  ( APPvar *app, char *help, int x, int y);
int     mt_BubbleModal ( APPvar *app, char *help, int x, int y);
int     mt_BubbleDo    ( APPvar *app, OBJECT *tree, int __index);
int     mt_BubbleAttach( APPvar *app, OBJECT *tree, int __index, char *help);
void    mt_BubbleFree  ( APPvar *app);
int     mt_BubbleFind  ( APPvar *app, OBJECT *tree, int __index, char **help);
int     mt_BubbleEvnt  ( APPvar *app);
int     mt_BubbleGet   ( APPvar *app);
/** @} */ 

/* Configuration library */
/** @addtogroup Configuration
 * @{ */
int     mt_ConfRead    ( APPvar *app);
int     mt_ConfInquire ( APPvar *app, char *keyword, char *fmt, ...);
int     mt_ConfGetLine ( APPvar *app, char *line);
int     mt_ConfWrite   ( APPvar *app, char *keyword, char *fmt, ...);
int		mt_ConfWindom  ( APPvar *app);
/** @} */ 

/* Data library */
/** @addtogroup Data
 * @{ */
int     mt_DataAttach  ( APPvar *app, WINDOW *win, long magic, void *data);
int     mt_DataDelete  ( APPvar *app, WINDOW *win, long magic);
void*   mt_DataSearch  ( APPvar *app, WINDOW *win, long magic);
/** @} */ 

/* Event library */
/** @addtogroup Event
 * @{ */
int     mt_EvntWindom    ( APPvar *app, int __evnt);
int     mt_EvntWindomBuff( APPvar *app, int __evnt, short buff[8]);
int     mt_EvntAdd       ( APPvar *app, WINDOW *win, int msg, func_evnt     proc, int mode);
int     mt_EvntAttach    ( APPvar *app, WINDOW *win, int msg, func_evnt     proc);
int     mt_EvntDataAdd   ( APPvar *app, WINDOW *win, int msg, func_evntdata proc, void *data, int mode);
int     mt_EvntDataAttach( APPvar *app, WINDOW *win, int msg, func_evntdata proc, void *data);
int     mt_EvntDelete    ( APPvar *app, WINDOW *win, int msg);
void*   mt_EvntFind      ( APPvar *app, WINDOW *win, int msg);
int     mt_EvntExec      ( APPvar *app, WINDOW *win, short buff[8]);
void    mt_EvntClear     ( APPvar *app, WINDOW *win);
int     mt_EvntRemove    ( APPvar *app, WINDOW *win, int msg, void         *proc);
void    mt_EvntDisable   ( APPvar *app, WINDOW *win, int msg);
void    mt_EvntEnable    ( APPvar *app, WINDOW *win, int msg);
void    mt_EvntRedraw    ( APPvar *app, WINDOW *win);
void	mt_EvntRedrawGrect( APPvar *app, WINDOW *win, GRECT *work);
/** @} */ 

/* Formular library */
/** @addtogroup Formular
 * @{ */
WINDOW* mt_FormWindBegin( APPvar *app, OBJECT *dial, char *nom);
void    mt_FormWindEnd  ( APPvar *app);
int     mt_FormWindDo   ( APPvar *app, int __evnt);
int     mt_FormWindDoBuff( APPvar *app, int __evnt, short buff[8]);

int     mt_FormAttach  ( APPvar *app, WINDOW *win, OBJECT *tree, func_evnt func);
void 	mt_FormResize  ( APPvar *app, WINDOW *win, INT16 *x, INT16 *y, INT16 *w, INT16 *h);
WINDOW* mt_FormCreate  ( APPvar *app, OBJECT *tree, int attrib, func_evnt func, char *name, GRECT *coord, int grow, int dup);
int     mt_FormSave    ( APPvar *app, WINDOW *win, int mode);
int     mt_FormRestore ( APPvar *app, WINDOW *win, int mode);
void    mt_FormBegin   ( APPvar *app, OBJECT *tree, MFDB *bckgrnd);
void    mt_FormEnd     ( APPvar *app, OBJECT *tree, MFDB *bckgrnd);
/** @brief Handle a formular created by mt_FormBegin().
 * This function is just an alias to mt_form_do(). 
 * @sa mt_FormBegin(), mt_FormEnd() */
#define mt_FormDo(a,b,c) mt_form_do(b,c,a->aes_global)
int     mt_FormAlert   ( APPvar *app, int but, char fmt[], ...);
void*   mt_FormThumb   ( APPvar *app, WINDOW *win, int *idxthb, int *idxbut, int nb);
void    mt_FormThbSet  ( APPvar *app, WINDOW *win, int but);
int     mt_FormThbGet  ( APPvar *app, void   *thb, int mode);

void __CDECL mt_stdFormClose(WINDOW *win, int ind, int mode, void *data, APPvar *app);
/** @} */ 

/* Mouse library */
/** @addtogroup Mouse
 * @{ */
void    mt_MouseObjc   ( APPvar *app, OBJECT *tree, int __index);
void    mt_MouseWork   ( APPvar *app);
void    mt_MouseSprite ( APPvar *app, OBJECT *tree, int __index);
/** @} */ 

/* Frame library */
/** @addtogroup Frame
 * @{ */
void    mt_FrameInit   ( APPvar *app);
void    mt_FrameExit   ( APPvar *app);
WINDOW* mt_FrameCreate ( APPvar *app, int attrib);
void    mt_FrameAttach ( APPvar *app, WINDOW *win, WINDOW *src, int line, int col, int w, int h, int mode);
WINDOW* mt_FrameRemove ( APPvar *app, WINDOW *win, WINDOW *child, int line, int col);
void    mt_FrameSet    ( APPvar *app, WINDOW *win, int mode, ...);
void 	mt_FrameGet    ( APPvar *app, WINDOW *win, int mode, ...);
WINDOW* mt_FrameFind   ( APPvar *app, WINDOW *win, int x, int y);
WINDOW* mt_FrameSearch ( APPvar *app, WINDOW *win, int line, int col);
int     mt_FrameCalc   ( APPvar *app, WINDOW *win, int mode, INT16 *x, INT16 *y, INT16 *w, INT16 *h);
/** @} */ 

/* Font  library */
/** @addtogroup Font
 * @{ */
int     mt_FontName2Id 	 ( APPvar *app, char *name);
int     mt_FontId2Name 	 ( APPvar *app, int id, char *name);
int     mt_VstLoadFonts	 ( APPvar *app, int res);
void    mt_VstUnloadFonts( APPvar *app, int res); 
int     mt_VstFont     	 ( APPvar *app, int id) ;
int     mt_VqtName     	 ( APPvar *app, int elem, char *name) ;
int     mt_VqtXname    	 ( APPvar *app, int id, int __index, char *name, int *fl);
int     vqt_extname 	 ( int handle, int __index, char *name, int *speedo, INT16 *format, INT16 *flags);
/** @} */ 

/* Selector library */
/** @addtogroup Selector
 * @{ */
int     mt_FselInput   ( APPvar *app, char *path, char *fname, char *ext, char *title, char *lpath, char *lext);
int     mt_FontSel     ( APPvar *app, char *title, char *example, int flags, int *id, int *size, char *name);
/** @} */ 

/* Menu library */
/** @addtogroup Menu
 * @{ */
int     mt_MenuBar     ( APPvar *app, OBJECT *menu, int mode);
int     mt_MenuTnormal ( APPvar *app, WINDOW *win, int title, int mode);
int     mt_MenuIcheck  ( APPvar *app, WINDOW *win, int title, int mode);
int     mt_MenuText    ( APPvar *app, WINDOW *win, int title, char *txt);
int     mt_MenuDisable ( APPvar *app);
int     mt_MenuEnable  ( APPvar *app);
int     mt_MenuPopUp   ( APPvar *app, void *data, int x, int y, int max, int size, int item, int mode);
void    mt_MenuScroll  ( APPvar *app, WINDOW *win, int dir);
/** @} */ 
/* function under developped. Dont use it */
int     mt_MenuAttach  ( APPvar *app, int flag, OBJECT *tree, short item, MENU *mdata);

/* Object library */
/** @addtogroup Object
 * @{ */
/* int     mt_ObjcAttach      ( APPvar *app, int mode, WINDOW *win, int __index, int type, ...);*/
int     mt_ObjcChange      ( APPvar *app, int mode, void *data, int __index, int state, int redraw);
int     mt_ObjcChar        ( APPvar *app, OBJECT *tree, int idx, int put);
int     mt_ObjcDrawParent  ( APPvar *app, int mode, void *win, int __index, int parent_depth, int child_depth);
#define mt_ObjcDraw(a,b,c,d,e)	mt_ObjcDrawParent(a,b,c,d,0,e)
int     mt_ObjcIsHidden    ( APPvar *app, int mode, void *win, int __index);
OBJECT* mt_ObjcDup         ( APPvar *app, OBJECT *tree, WINDOW *win);
OBJECT *mt_ObjcNDup        ( APPvar *app, OBJECT *src,  WINDOW *win, int nb);
OBJECT *mt_ObjcNDupAtAddr  ( APPvar *app, OBJECT *src,  WINDOW *win, int nb, OBJECT *dest);
int     mt_ObjcEdit        ( APPvar *app, int mode, WINDOW *win, int obj, int val, INT16 *idx, int kind);
void    mt_ObjcFree        ( APPvar *app, OBJECT *tree);
void    mt_ObjcNFreeAtAddr ( APPvar *app, OBJECT *tree, int nb);
char*   mt_ObjcString      ( APPvar *app, OBJECT *tree, int __index, char *put);
/** @brief Modify an object label. Just a fast call of mt_ObjcString(). */
#define mt_ObjcStrCpy(a,tree,index,str)  strcpy(mt_ObjcString(a,tree,index,NULL),str)
void    mt_ObjcStr3d       ( APPvar *app, OBJECT *tree);
int     mt_ObjcWindDraw    ( APPvar *app, WINDOW *win, OBJECT *tree, int __index, int depth, int xclip, int yclip, int wclip, int hclip);
int     mt_ObjcWindDrawParent( APPvar *app, WINDOW *win, OBJECT *tree, int __index, int parent_depth, int child_depth, int xclip, int yclip, int wclip, int hclip);
int     mt_ObjcWindChange  ( APPvar *app, WINDOW *win, OBJECT *tree, int __index, int xclip, int yclip, int wclip, int hclip, int state);
OBJECT *mt_ObjcTree		   ( APPvar *app, int mode, WINDOW *win);

void    ObjcStrFmt         ( char *dest, char *src, int size);
/**@brief See ObjcStrFmt() */
#define mt_ObjcStrFmt(a,b,c,d) ObjcStrFmt(b,c,d)


/** attach a "int" variable to an object
 *
 *  @param app application descriptor
 *  @param mode type of target :
 *         - OC_FORM if the formular is a window,
 *         - OC_TOOLBAR if the formular is a toolbar,
 *         - OC_OBJC if the formular is a classic formular.
 *  @param win window
 *  @param __index number of the objet
 *  @param var pointer to the "int" variable to attach
 *  @param value_when_selected is the value of \a var when the object \a __index is selected
 *
 *  This function attach the variable to the object, and initialise the variable to
 *  \a value_when_selected if the object is selected.
 */
static inline int mt_ObjcAttachVar( APPvar *app, int mode, WINDOW *win, int __index, int *var, int value_when_selected ) {
	int mt_ObjcAttach ( APPvar *, int, WINDOW *, int, int, ...);
	return mt_ObjcAttach( app, mode, win, __index, BIND_VAR, var, value_when_selected);
}

static inline int mt_ObjcAttachBit( APPvar *app, int mode, WINDOW *win, int __index, int *var, int bit ) {
	int mt_ObjcAttach ( APPvar *, int, WINDOW *, int, int, ...);
	return mt_ObjcAttach( app, mode, win, __index, BIND_BIT, var, bit);
}

static inline int mt_ObjcAttachFormFunc( APPvar *app, WINDOW *win, int __index, func_bind fn, void *data ) {
	int mt_ObjcAttach ( APPvar *, int, WINDOW *, int, int, ...);
	return mt_ObjcAttach( app, OC_FORM, win, __index, BIND_FUNC, fn, data);
}

static inline int mt_ObjcAttachTBFunc( APPvar *app, WINDOW *win, int __index, func_bind fn, void *data ) {
	int mt_ObjcAttach ( APPvar *, int, WINDOW *, int, int, ...);
	return mt_ObjcAttach( app, OC_TOOLBAR, win, __index, BIND_FUNC, fn, data);
}

static inline int mt_ObjcAttachMenuFunc( APPvar *app, WINDOW *win, int __index, func_bindmenu fn, void *data ) {
	int mt_ObjcAttach ( APPvar *, int, WINDOW *, int, int, ...);
	return mt_ObjcAttach( app, OC_MENU, win, __index, BIND_FUNC, fn, data);
}

/** @} */ 

/* Window library */
/** @addtogroup Window
 * @{ */
WINDOW* mt_WindCreate  ( APPvar *app, int GemAttrib, int x, int y, int w, int h);
int     mt_WindOpen    ( APPvar *app, WINDOW *win, int x, int y, int w, int h);
int     mt_WindClose   ( APPvar *app, WINDOW *win);
int     mt_WindDelete  ( APPvar *app, WINDOW *win);
WINDOW* mt_WindHandle  ( APPvar *app, int handle);
int     mt_WindGet     ( APPvar *app, WINDOW *win, int mode, INT16* par1, INT16* par2, INT16* par3, INT16* par4);
#define mt_WindGetGrect( app, win, mode, grect ) mt_WindGet( app, win, mode, &((grect)->g_x), &((grect)->g_y), &((grect)->g_w), &((grect)->g_h) )
int     mt_WindXGet    ( APPvar *app, WINDOW *win, int mode, GRECT *clip, INT16* par1, INT16* par2, INT16* par3, INT16* par4);
#define mt_WindXGetGrect( app, win, mode, clip, grect ) mt_WindXGet( app, win, mode, clip, &((grect)->g_x), &((grect)->g_y), &((grect)->g_w), &((grect)->g_h) )
void*   mt_WindGetPtr  ( APPvar *app, WINDOW *win, int mode);
int     mt_WindSet     ( APPvar *app, WINDOW *win, int mode, int par1, int par2, int par3, int par4);
#define mt_WindSetGrect( app, win, mode, grect ) mt_WindSet( app, win, mode, (grect)->g_x, (grect)->g_y, (grect)->g_w, (grect)->g_h )
int     mt_WindSetStr  ( APPvar *app, WINDOW *win, int mode, char *str);
int     mt_WindSetPtr  ( APPvar *app, WINDOW *win, int mode, void *par1, void *par2);
int     mt_WindCalc    ( APPvar *app, int type, WINDOW *wind, int xin, int yin, int win, int hin,
                         INT16 *xout, INT16 *yout, INT16 *wout, INT16 *hout);
#define	mt_WindCalcGrect( app, mode, win, inrect, outrect ) \
                          mt_WindCalc( app, mode, win, \
                                       (inrect)->g_x, (inrect)->g_y, (inrect)->g_w, (inrect)->g_h, \
                                       &(outrect)->g_x, &(outrect)->g_y, &(outrect)->g_w, &(outrect)->g_h )
void    mt_WindSlider  ( APPvar *app, WINDOW *win, int mode);
WINDOW* mt_WindAttach  ( APPvar *app, int handle);
WINDOW* mt_WindFind    ( APPvar *app, int mode, ...);
void    mt_WindTop     ( APPvar *app, WINDOW *win);
void	mt_WindNew	   ( APPvar *app);
void    mt_WindClear   ( APPvar *app, WINDOW *win);

void  __CDECL mt_stdWindClear( WINDOW *win, short buff[8], APPvar *app);
/** @} */ 

/* Slider library (for window formular) */
/** @addtogroup Slider
 * @{ */
void*	mt_SlidCreate  ( APPvar *app, short min, short max, short sinc, short linc, short value, int ori, int upd);
void	mt_SlidDelete  ( APPvar *app, void *slid);
void  	mt_SlidAttach  ( APPvar *app, void *slid, int mode, WINDOW *win, int up, int bsld, int sld, int dn);
short 	mt_SlidGetValue( APPvar *app, void *slid);
void  	mt_SlidSetFunc ( APPvar *app, void *slid, func_doslid func, void *data);
void  	mt_SlidSetSize ( APPvar *app, void *slid, int size);
void  	mt_SlidSetValue( APPvar *app, void *slid, short value);
void  	mt_SlidSetUpdat( APPvar *app, void *slid, int upd);
/** @} */ 

/* Resource library */
/** @addtogroup Resource
 * @{ */
int     mt_RsrcLoad        ( APPvar *app, const char *name);
void    mt_RsrcXtype       ( APPvar *app, int mode, OBJECT **rs_trindex, int num_tree);
void    mt_RsrcFixCicon    ( APPvar *app, OBJECT *object, int num_obs, int num_cib, INT16 palette[][4], void *fix);
void    mt_RsrcFreeCicon   ( APPvar *app, void *fix);
int     mt_RsrcUserDraw    ( APPvar *app, int mode, WINDOW *win, int __index, func_userdraw draw, void *data);
/** @brief Release from memory resource file loaded by mt_RsrcLoad(). */
#define mt_RsrcFree(app)    mt_rsrc_free(app->aes_global)
void*   mt_RsrcXload       ( APPvar *app, const char *name);
void    mt_RsrcXfree       ( APPvar *app, void *rsc);
int     mt_RsrcGaddr       ( APPvar *app, void *rsc, int type, int __index, void *addr);
rscHDR* mt_RsrcGhdr        ( APPvar *app, void *rsc);
/** @} */ 

/* Utility library */
/** @addtogroup Utility
 * @{ */
void    mt_give_iconifyxywh
                    ( APPvar *app, INT16 *x, INT16 *y, INT16 *w, INT16 *h);
void    mt_snd_rdw  ( APPvar *app, WINDOW *win);
void    mt_snd_arrw ( APPvar *app, WINDOW *win, int msg);
void    mt_snd_msg	( APPvar *app, WINDOW *win, int msg, int par1, int par2, int par3, int par4);
void    rc_set      ( GRECT *, int, int, int, int);
void    rc_lset     ( LGRECT *, long, long, long, long);
void    w_getpal    ( APPvar *app, WINDOW *win);
void    w_setpal    ( APPvar *app, WINDOW *win);
int     w_get_hndl  ( APPvar *app, WINDOW *win);
void    w_get_bkgr  ( APPvar *app, int of_x, int of_y, int of_w, int of_h, MFDB *img);
void    w_put_bkgr  ( APPvar *app, int of_x, int of_y, int of_w, int of_h, MFDB *img);
int     keybd2ascii ( int keybd, int shift);
char*   conv_path   ( char *);

int     mt_ShelHelp ( APPvar *app, char *file, char *chapter);
int     mt_ShelWrite   ( APPvar *app, char *prg, char *cmd, void *env, int av, int single);
void    mt_DebugWindom ( APPvar *app, const char *format, ...);
void*   Galloc      ( long size);
void    mt_GrectCenter ( APPvar *app, int w, int h, INT16 *x, INT16 *y );

void mt_ClipOn  ( APPvar *app, W_GRAFPORT *graf, GRECT *r);
void mt_ClipOff ( APPvar *app, W_GRAFPORT *graf);

void    rc_lgrect2grect ( GRECT *, const LGRECT *);
void    rc_grect2lgrect ( LGRECT *, const GRECT *);
short   rc_lintersect (const LGRECT * r1, LGRECT * r2);

/** @} */ 



/* Component library (FIXME: experimental) */
/** @addtogroup Component
 * @{ */

/*
 * Maximal value of a component width of height.
 */
#define CPNT_MAXSIZE  (LONG_MAX)

/**
 * COMPONENT layout type values.
 **/
#define CLT_VERTICAL	0
#define CLT_HORIZONTAL	1
#define CLT_STACK	2

/**
 * COMPONENT status bitmask values.
 **/
#define CS_INTERACTIVE		0x0010	/**< can get \b CM_GETFOCUS and \b CM_LOSEFOCUS */
#define CS_HSLIDE		0x0020
#define CS_VSLIDE		0x0040
#define CS_HIDDEN		0x0080
#define CS_CLIP 		0x0100
#define CS_PROPORTIONAL		0x0200	/**< visible contents proportions changes (needs redraw) on resize */

#define CS_RESERVED		0x00010000UL  /**< internal: reserved for internal use */
#define CS_FOCUSED		0x00020000UL  /**< internal: COMPONENT is currently under focus */
#define CS_IN_USE		0x00080000UL  /**< internal: COMPONENT is in use: should not be free-ed (see mt_CompDelete()) */
#define CS_DESTROYED		0x00040000UL  /**< internal: COMPONENT should be free-ed as soon as possible */

/**
 * COMPONENT special messages.
 **/
#define CM_GETFOCUS		0x44c0	/**< received upon getting focus */
#define CM_LOSEFOCUS		0x44c1	/**< received upon losing focus */
#define CM_REFLOW		0x44c2	/**< reflow the component according to the layout settings */
#define CM_REPOSED		0x44c3	/**< move&resize the component */


/**
 * COMPONENT special messages.
 **/
#define CF_WINDOW      0x44e0	/**< the WINDOW the component is hooked to or NULL if none */
#define CF_MAXSIZE     0x44e1	/**< fixed maximum size rectagle FIXME: rename? */
#define CF_HSLIDE      0x44d0	/**< FIXME: TODO */
#define CF_VSLIDE      0x44d1	/**< FIXME: TODO */
#define CF_FOCUS       0x44d2   /**< Focus component pointer */

/**
 * COMPONENT/WINDOW relation magic values.
 **/
#define WD_CCRT 0x43435254L	/**< 'CCRT': component container's root win/component component/window */
#define WD_CCWD 0x43435744L	/**< 'CCWD': component window/window component */
#define WD_CWGT 0x43573743L	/**< 'CWGT': window widget component */


/**
 * @struct _component
 * The WinDom COMPONENT representation structure.
 **/
typedef struct _component {
	LINKABLE	item;				/**< Managed by WinDom */

	short		type;				/**< Component layout type (CLT_* values) */
	unsigned long	status;				/**< Component status mask (CS_* values) */
	short		flex;				/**< Flexibility ratio (0 for fixed size) */
	short		size;				/**< size (for CLT_VERTICAL/CLT_HORIZONTAL) modes) */
	LGRECT		rect;				/**< Actual component size rectangle */
	W_GRAFPORT	*graf;				/**< Component graphics port */

	/** Data internally managed by WinDom */
	struct {
		unsigned long	min_width;	/**< minimal component width */
		unsigned long	max_width;	/**< maximal component width */
		unsigned long	min_height;	/**< minimal component height */
		unsigned long	max_height;	/**< maximal component height */
	} bounds;

	struct _component	*parent;	/**< Container reference */
	LIST			*children;		/**< Children (FIXME: ?by value?) */
	LIST			binding;		/**< component handler list - reserved */
	LIST			data;			/**< component data - reserved */
} COMPONENT;


#define FOR_EACH_CHILD( i, list )	\
	listForEach( COMPONENT *, i, list ) \
		if ( !(((COMPONENT*)(i))->status & CS_HIDDEN) )


/* WINDOM slider component data interface values */
#define CDT_SLIDER_POS		0x455f4dUL  /* WF_[VH]SLIDE-like data values (from the range 0-1000) */
#define CDT_SLIDER_SIZE		0x455f4eUL  /* WF_[VH]SLSIZE-like data value (from the range 0-1000) */
#define CDT_SLIDER_CONTROL	0x455f4fUL  /* The pointer to a slider controlled component */


/* COMPONENT essentials */
COMPONENT *mt_CompCreate( APPvar *app, short type, long size, short flex );
COMPONENT *mt_CompFind( APPvar *app, COMPONENT *p, short mx, short my);
int   mt_CompDelete( APPvar *app, COMPONENT *p );
void  mt_CompAttach( APPvar *app, COMPONENT *p, COMPONENT *c);

/* COMPONENT data handling functions */
int   mt_CompDataAttach( APPvar *app, COMPONENT *c, long magic, void *data);
int   mt_CompDataDelete( APPvar *app, COMPONENT *c, long magic);
void *mt_CompDataSearch( APPvar *app, COMPONENT *c, long magic);

#if 0
// FIXME: not used/usefull yet
void	mt_CompGet( APPvar *app, COMPONENT *c, short mode, long *par1, long *par2, long *par3, long *par4 );
#define mt_CompGetLGrect( app, win, mode, grect ) mt_CompGet( app, win, mode, (grect)->g_x, (grect)->g_y, (grect)->g_w, (grect)->g_h )
#endif

int	mt_CompSet( APPvar *app, COMPONENT *c, short type, long par1, long par2, long par3, long par4 );
int	mt_CompSetPtr( APPvar *app, COMPONENT *c, short mode, void *par1, void *par2);
int	mt_CompGetLGrect( APPvar *app, COMPONENT *c, short type, LGRECT *rect);
void*	mt_CompGetPtr( APPvar *app, COMPONENT *c, short type);

/* COMPONENT handler functions */
#ifdef __WINDOM__  /* for the non-mt version, the last APPvar parameter doesn't exist */
typedef void __CDECL (* func_comp_evntdata)(COMPONENT *, long buff[8], void *);
typedef void __CDECL (* func_comp_evnt    )(COMPONENT *, long buff[8]);
#else
typedef void __CDECL (* func_comp_evntdata)(COMPONENT *, long buff[8], void *, APPvar *app);
typedef void __CDECL (* func_comp_evnt    )(COMPONENT *, long buff[8], APPvar *app);
#endif

int   mt_CompEvntAdd( APPvar *app, COMPONENT *p, short msg, func_comp_evnt proc, int mode);
int   mt_CompEvntDataAdd( APPvar *app, COMPONENT *c, short msg, func_comp_evntdata proc, void* data, int mode);
int   mt_CompEvntAttach( APPvar *app, COMPONENT *p, short msg, func_comp_evnt proc);
int   mt_CompEvntDataAttach( APPvar *app, COMPONENT *c, short msg, func_comp_evntdata proc, void* data);
int   mt_CompEvntExec  ( APPvar *app, COMPONENT *p, long buff[8]);
void  mt_CompEvntClear( APPvar *app, COMPONENT *c);
int   mt_CompEvntRemove( APPvar *app, COMPONENT *c, int msg, void *proc);
int   mt_CompEvntDelete( APPvar *app, COMPONENT *c, int msg);
void  mt_CompEvntEnable( APPvar *app, COMPONENT *c, int msg);
void  mt_CompEvntDisable( APPvar *app, COMPONENT *c, int msg);
void  mt_CompEvntRedraw( APPvar *app, COMPONENT *c); /* evnt_redraw.c */
void  mt_CompEvntRedrawLGrect( APPvar *app, COMPONENT *c, LGRECT *rdraw); /* evnt_redraw.c */

/**
 * COMPONENT slider data magic data defines.
 **/
#define WD_CPNT_HSLIDE_INFO  0x43485349L /* 'CHSI' horizontal slider info */
#define WD_CPNT_VSLIDE_INFO  0x43565349L /* 'CVSI' vertical slider info */

#ifndef VSLIDERINFO
#define VSLIDERINFO(app,c)  ((C_SLIDERINFO*)mt_CompDataSearch( app, c, WD_CPNT_VSLIDE_INFO))
#endif
#ifndef HSLIDERINFO
#define HSLIDERINFO(app,c)  ((C_SLIDERINFO*)mt_CompDataSearch( app, c, WD_CPNT_HSLIDE_INFO))
#endif

typedef struct _sliderinfo {
	long  pos;
	long  max;
	short unit;
} C_SLIDERINFO;

/* FIXME: 1. attach the WINDOW slider to a particular COMPONENT */
/* FIXME: 2. attach COMPONENT private slider (rewrite the widget display) */
int   mt_CompSliderAttach(APPvar *app, COMPONENT *p, short type);

/** @} */ 

__END_DECLS

#endif /* __MT_WINDOM__ */
