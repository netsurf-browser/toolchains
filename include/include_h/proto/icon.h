#ifndef PROTO_ICON_H
#define PROTO_ICON_H

/*
**    $Id: icon.h,v 1.14 2010-01-31 01:29:36 ssolie Exp $
**
**    Prototype/inline/pragma header file combo
**
**    (C) Copyright 2003-2005 Hyperion Entertainment VOF and Amiga, Inc.
**        All Rights Reserved
**
**    Copyright (c) 2010 Hyperion Entertainment CVBA.
**        All Rights Reserved.
*/

#ifndef   WORKBENCH_WORKBENCH_H
#include <workbench/workbench.h>
#endif
#ifndef   DATATYPES_PICTURECLASS_H
#include <datatypes/pictureclass.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
extern struct Library * IconBase;
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/icon.h>
 #ifdef __USE_INLINE__
  #include <inline4/icon.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_ICON_PROTOS_H
  #define CLIB_ICON_PROTOS_H 1
 #endif /* CLIB_ICON_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct IconIFace *IIcon;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_ICON_PROTOS_H
  #include <clib/icon_protos.h>
 #endif /* CLIB_ICON_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/icon.h>
  #else
   #include <ppcinline/icon.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/icon_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/icon_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_ICON_H */
