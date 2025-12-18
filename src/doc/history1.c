/* $Id: history1.c,v 1.1 2005/11/16 20:40:10 bercegeay Exp $ */

/**@file*/

/** @page pagehist1 History of windom1

Version 1.21.3 (November 2004)
-     Fix: 3D buttons of the font selector were smaller each time the font selector
      is invoked (bug fixed in the way OBJECTs are transformed to USERDEF).
-     Fix: restoring of the mouse form (if AES4_XGMOUSE flag is set)
-     Fix: area returned by WF_FIRSTXYWH is now intersected with the screen area
     (needed for some AES that returns area partially out of the screen when the
     window is partially out of the screen)
-     fix: double call of functions binded to menu
-     fix: redraw of menu in window under xaaes
-     fix: drawing of children in ObjcDraw() with OC_OBJC mode

Version 1.21.2 (September 2004)
-     Fix a critical bug which was crashed
	extended objects userdefs routines under PlainTOS. Userdefs
	routines use now less static memory, a gemdos call performed
	under supervisor mode has been removed. The WinDom demo
	works nows under plainTOS :-).

Version 1.21.1 (July 2004)
-     Only bug fixes (See file Changelog for details).

Version 1.21.0 (i.e. 1.21 Release 00, December 2003)
-     WinDom is now developped on the Sourceforge plateform (see http://sourceforge.net).
	Last release can be obtained from http://windom.sourceforge.net which
	become the official home page (old windom.free.fr will be not
	supported).
-     Source files have been totally reorganized : each C module has been
	splitted into small module containing only one WinDom function.
	This method allows to decrease size of executable when we compile
	with Gcc (useless modules will be not linked).
-     WinDom source compilation requires a file system supporting 
	long name.
-     WinDom works on Aranym. The demo program run happily but Xaaes
	does not like MENUTITLE object as G_TITLE in the desktop
	menu. So we have removed these type from the demo resource file.
-     WinDom requires now only GEMLIB pl43 library. 
	PCGEMLIB is no long support on Pure C compiler.
-     The specific WinDom WM_MNSELECTED message has been removed. Instead
	of this message, WinDom use now the extended MN_SELECTED message
	protocol (AES 3.3) to handle menu in windows.
-     WinDom tries to support window menu capability of Xaaes.
	The new widget MENUBAR can be used in WindCreate().
	Adding the variable windom.menu.system variable
-     Some constants from windom.h have been removed or renamed :
	-     WF_MENU removed (defined in gem.h)
	-     APC_xxx constants removed (defined in gem.h)	
	-     WF_WINX and WF_WINXCFG removed (defined in gem.h) 
	-     B_UNTOPPABLE removed (defined as BEVENT_WORK in gem.h)
	-     B_MODAL rename as BEVENT_MODAL
	-     ED_xxx constants removed (defined in gem.h)

Version 1.20 Release 02 (November 2002)
-     The only reason of this new WinDom release is the change of
	legal status of WinDom. WinDom is now distributed under the
	conditions specified by the GNU LESSER GENERAL PUBLIC LICENCE.
	In the authors spirit, usage of WinDom was really closed to the
	GNU Licence, however we clarify officially this point in this special
	issue. If you doesnot know the GNU licence, you HAVE TO read the 
	section LGPL (or file COPYRIGHT).
-     Adding in tools folder some hack to handle Drag'n Drop
	protocol.

Version 1.20 Release 01 (October 2002)
-     The FRAMEINFO structure has been hidden from the WINDOW structure
	(see details below). 
-     Bag bug fixed in Galloc().
-     conf_path() converts correctly absolute unix standard path
	(e.g. /etc is converted to U:/etc).
-     Bag bug fixed in ObjcEdit() : parameter obj is now checked.
-     app.work_in had a bad size !! (10 elements instead of 11).
-     New prototype for ApplSet() with APS_FLAGS mode (see manual of ApplSet()).
-     Color palet handling is disabled in true color mode (e.g. when app.nplanes > 8).
-     Bug fixed in color palet updating when a WF_TOP message occures.
-     New message WM_PREREDRAW.
-     WinDom functions (try to) use standard TOS error specified in
	the header file toserror.h provided in the package.
-     New functions  EvntDataAttach(), EvntDataAdd() : user data can
	be binded to event function. ObjcAttach() and
	RsrcUserDraw() can bind data too. This new feature is really
	powerfull and makes the Data library deprecated.
-     Due to the previous extension, FormThumb() can now handle severals
	thumb indexes by formular.
-     WinDom can be compiled with the Pure C driver of Julian F. Reschke. It 
	is easier to the author to compile WinDom (a simple gmake command is
	used to compile WinDom for all supported compiler).
-     Extended object XEDIT works better but crashes under Naes.
-     Bug fixed in obj_root() and ShelWrite().
-     When it is possible (app.nplanes>=4) disabled objects in extended
	types are displaying with LBLACK color (and not the LIGHT attribut).
-     New extended objects XLONGBOXTEXT (a box containing several line 
	of text) and XTEDINFO (as TEDINFO objects but with a same visual aspect 
	however the AES version) (see 
	(!link [Extended types and ressource editor section] [xtype_editor])).
-     Objects SLIDEPART had been improved.
-     Several bugs fixed in frame library and modification :
	-     Better aspect of frame widgets (using Arnaud's widgets).
	-     New mode APS_FRAMEWIDGETSIZE in ApplSet().
	-     Frame structures are now private and hidden (futur enhancement
		will be more portable from user point of view).
	-     New function FrameGet() to access frame informations (now hidden).
	-     All examples of frame programming have been recompiled 
			(and adapted to new window specifications).
-     app.id contains -1 when AES session is not initialised (i.e. before
	ApplInit() call).
-     New function FormResize().
-     New Sliders library to handle slider objects in formulars
	(see examples/sliders/ demonstration in the distribution).

Version 1.10 (September 2001)
-     In this version, many fundamental changes appear. Please,
	  read the section Convert your old WinDom applications if you
      already have used WinDom,
-     Arnaud Bercegeay is now associated at the WinDom Project.
	  Most of new features are supported by him. Its last contribution
	  is the Gcc 32 bits support (thanx to him, it is really a great
	  hacker !).
-     WinDom is now available for Gcc ! Read the section 
      Gcc 32 bits portability). Notice it is
      the main change in this release.
-     New functions FormThbGet() and FormThbSet().
-     New function EvntRedraw().
-     Function ObjcEdit() have new prototype. 
-     rect_set(), set_clip(), clip_off() respectively renammed in
  rc_set(), rc_clip_on(), rc_clip_off().
-     ExecGemApp() renammed in ShelWrite().
-     New mode in RsrcXtype().
-     New prototype for ObjcDup() (more object types are supported).
-     It is now possible to attach a variable or a function to
	the desktop menu (see ObjcAttach()).
-     New Functions WindTop() and WindFind().
-     The undocumented AvServer() function is now officiel and
	had changed its prototype.
-     New WinDom configuration feature: if a variable is not found
	in the application configuration, it is searched in the default
	configuration. It is meanning that the default configuration
	affects all WinDom applications except if addressed variables are
	defined in the application configuration.
-     if the user click on a menu window or a toolbar window without
	  actived an object, the window is topped.
-     function rc_intersect() and grect_to_array() had moved in PCGMXLIB 
	because they are binded in GemLIB.
-     DEMO program release 3.
-     WindClear() : if a background color is not supported, white color
	is taken.
-     new public variable app.pipe : it is a 256-buffer in
	global memory used by WinDom for extern GEM communication. It can
	used by programer for his own communications.
-     Fixed bug in FselInput (case for ext == NULL).
-     Fixed bug in toolbar resizing.
-     Fixed bug in window menu selection with Naes.
-     Fixed bug in FormThumb().

Version 1.00 (November 2000)
-     In this version, many fundamental changes appear. Please,
	  read the section Convert your old WinDom applications if you
      already have used WinDom,
-     WinDom is now available to Sozobon X (thanks to A.Bercegeay for
      his precious help),
-     Source files have new structure : all new AES and new VDI
      functions are binded in a separated library. WinDom should
      be linked with GemLib pl38 if you use Sozobon version and
      should be linked with PCGEMLIB.LIB and PCGMXLIB.LIB (this
      last library is a part of WinDom package) if you use
      Pure C.
-     Many bugs have been fixed in Event Library. It is now
	  possible to attach severals function to a same function
	  (see EvntAdd(), EvntRemove(), EvntEnable() and EvntDisable()).
	  For this raison, functions EvntAttach(), EventDelete() and
	  EvntExec() have a different behavior than in the previous
	  version. MU_XTIMER, MU_XM1 and MU_XM2 
	  events can be binded to a global function,
-     An illimited number of data can be attached to a window 
	  (see Data Library) : the fields data and data2 in WINDOW 
	  structure are now obsolet !
-     Bag bug fixed in modal formulars (this bug appeared in the
	  last version),
-     TEDINFO objects are now duplicated by ObjcDup(),
-     New mode WF_MENU_HILIGHT in WindSet() : a function
      can be called when the user navigates a window menu : very
      usefull for help message !
-     New mode WF_ICONDRAW in WindSet(),
-     New functions ApplSet() / ApplGet() to parametrize the
	  application,
-     Bug fixed in the detection of the ApplControl() function,
-     WinDom configuration file is now searched in the $ETC
	  directory,
-     The variable evnt.timer replaces the two variable evnt.hi_timer
	  and evnt.lo_timer,
-     New standard function, std_btm, to handle WM_BOTTOMED message,
-     Bugs fixed in std_rtlnd() and std_dnlnd() (fixed by A.Bercegeay),
-     New mode in ObjcChange() : a state can be unset (contrib of A.Bercegeay),
-     Thumbs objects are now automatically handled (FormThumb()),
-     New variable app.nplanes giving the number of planes of the
      current screen resolution (requested by A.Bercegeay),
-     New messages WM_(UP|DN)(LINE|PAGE), WM_(LF|RT)(LINE|PAGE) allows
	  to bind directly the arrowed event (WA_) without use an
	  arrowed event function (WM_ARROWED),
-     New functions RsrcXload(), RsrcGaddr() and RsrcGhdr(),
-     MenuPopUp() has been completly rewritten by A.Bercegeay to support slider
      widget for large popup,
-     std_dstry() sends an AP_TERM message if a desktop menu is
	  not defined,
-     some problems in DCRBUTTON object redraw have been fixed,
-     when disabled, a MENUTITLE can contain a label (see DEMO.APP
	  for an example).

Version of March 2000
-     In this version, many fundamental changes appear. Please,
	  read the section Convert your old WinDom applications if you
      already have used WinDom,
-     Documentation has been translated in english. Only the
	  english version is now supported. The documentation has
	  been revised, many errors corrected ...
-     New functions EvntAttach(), EvntDelete(), EvntFind(),
	  EvntExec() to handle easily all GEM event. WinDom uses
	  a new way to handle window events and all function pointers
	  of the WINDOW structure disappear,
-     Bug fixed in ConfWrite(),
-     New messages AP_BUTTON and AP_KEYBD,
-     FRAME_NOBG bit (see FrameSet()) now documented,
-     with low resolution screen, the extended object
	  text are displayed with the system font with a fixed
	  size,
-     new FormCreate() feature: if a form is bigger than the
	  desktop, widget scrollers are automatically added to the
	  window,
-     new extended objet DIALMOVER (17) to draw background forms,
-     new variables windom.relief.{color,mono} in configuration
	  file,
-     bug fixed in give_iconifyxywh() : it returns now a correct value 
	  with MagiC,
-     the MENUTITLE seperator objects are better displayed,
-     the message WM_LOADCONF is renamed to AP_LOADCONF,
-     new FormAlert() function,
-     if a menu or a toolbar is added or removed in a opened window, the
	  size of the window is update in order to keep the same workspace
      size (see WindSet()),
-     MouseObjc() works now correctly,
-     bog fixed in MenuPopUp() : a false value was returned when the popup
	  was closed without mouse movement,
-     new function ObjcAttach() which allow to link object to variable
      or function,
-     bog fixed in frm_fld() function (fuller event of window formular),
-     new WF_ICONTITLE mode in WindSet() and WindGet() to define
	  the iconified window title, new comportement of iconify and uniconify
	  standard event function: the window gets the icon or normal title without
	  a specific call of WindSet(),
-     new WF_ICONDRAW mode in WindSet() and WindGet() to define
	  the icon redraw function,
-     the fullsize field in the WINDOW structure is replaced by the WS_FULLSIZE
	  bit in the status field of the WINDOW structure,
-     work area of WinDom windows (toolbar and menu take into account) are
      now correctly clipped during a WM_REDRAW event (Zerkman report).

Version of June 1999
-      bad bug in get_cookie() fixed (supervisor mode),
-      diverses bugs fixed in WindGet(),
-      the app.ntree variable contains the numbre of tree
	   in the ressource file loaded by RsrcLoad(),
-      a menu window can scroll (see (windom.menu.scroll),
-      New function MenuScroll(),
-      bug fixed in DEMO.APP,
-      MenuPopUp():
	-     popup placement bug fixed,
	-     Zerkman suggestion: a list popup style can be parametrized
		  in configuration file (see windom.popup.* variables),
	-     new P_CHECK option in MenuPopup()
	!end_itemize
-      WindClear() : new variables  windom.window.bg.*,
-      GrectCenter() : new prototype and use the configuration
	   variable windom.window.center,
-      new coonfiguration variables: windom.popup.window, windom.mform.attrib
	   and windom.window.effect,
-      FselInput() :
	!begin_itemize
	-      new prototype de FselInput() ( two additional parameters),
	-      nez configuration variables windom.fsel.fslx,
		   windom.fsel.path and windom.fsel.mask,
	-      BoxKite 2.00 compatibilty,
	-      specific use of fslx_() functions (window file selector) if 
		   possible (MagiC, Wdialog),
    -      Selectric compatibility,
	!end_itemize
-     If you use new file system with lmong filename (MinixFS,Vfat,Vfat32,...)
	  the WINDOM.CNF can be rename in .windomrc (unix style),
-     Bug fixed in RsrcLoad(): ressources placed in a subdirectory are
	  correctly loaded with Naes. The ressource pathname can have a
	  TOS format (C:/subdir\) or a MINT format (/c/subdir/)
-     Bug fixed in ApplInit(): system extensions are correctly interpreted
	  when appl_getinfo() function is partially implemented (WinX,Wdialog),
-     New functions: conv_path(), vq_extfs(), vq_winx(), Galloc().
-     If the AV environnent is installed (AvInit()), the window opened
	  are declared to the AV-server (message AV_ACCWINDOPEN, AV_ACCWINDCLOSED).
	  In this case, the AV server can possibly use the AV_DRAGDROP message
	  instead of the standard AP_DRAGDROP,
-     New mode in ObjcDraw()/ObjcChange(): OC_OBJC.
-     BubbleGEM functions are implmented for classical GEM formulars 
	  (see functions BubbleModal(), BubbleDo() and BubbleGet()).
-     The %S type in ConfInquire() function is now obsolet: ConfInquire()
	  detect if the string have double quote deliminters. This type is keeped
	  for compatibility,
-     New fonction WriteConf().
-     New fonctions vq_naes(), ApplControl(), appl_control().
-     New window feature: with MagiC or Naes, a shift-closer on a window
	  masks the application,

*/
