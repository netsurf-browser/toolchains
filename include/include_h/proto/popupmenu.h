#ifndef PROTO_POPUPMENU_H
#define PROTO_POPUPMENU_H

/*
**	$Id$
**
**	Prototype/inline/pragma header file combo
**
**	Copyright (c) 2010 Hyperion Entertainment CVBA.
**	All Rights Reserved.
*/

#ifndef CLASSES_POPUPMENU_H
#include <classes/popupmenu.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
 extern struct Library * PopupMenuBase;
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <intuition/classes.h>
 #ifndef __NOGLOBALCLASS__
  extern Class * PopupMenuClass;
 #endif /* __NOGLOBALCLASS__ */

 #include <interfaces/popupmenu.h>
 #ifdef __USE_INLINE__
  #include <inline4/popupmenu.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_POPUPMENU_PROTOS_H
  #define CLIB_POPUPMENU_PROTOS_H 1
 #endif /* CLIB_POPUPMENU_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct PopupMenuIFace *IPopupMenu;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_POPUPMENU_PROTOS_H
  #include <clib/popupmenu_protos.h>
 #endif /* CLIB_POPUPMENU_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/popupmenu.h>
  #else /* __PPC__ */
   #include <ppcinline/popupmenu.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/popupmenu_protos.h>
  #endif /* __PPC__ */
 #else /* __GNUC__ */
  #include <pragmas/popupmenu_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_POPUPMENU_H */
