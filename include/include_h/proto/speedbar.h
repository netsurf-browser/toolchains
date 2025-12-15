#ifndef PROTO_SPEEDBAR_H
#define PROTO_SPEEDBAR_H

/*
**    $Id: speedbar.h,v 1.11 2010-01-31 01:29:36 ssolie Exp $
**
**    Prototype/inline/pragma header file combo
**
**    (C) Copyright 2003-2005 Hyperion Entertainment VOF and Amiga, Inc.
**        All Rights Reserved
**
**    Copyright (c) 2010 Hyperion Entertainment CVBA.
**        All Rights Reserved.
*/

#ifndef   INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif
#ifndef   INTUITION_CLASSES_H
#include <intuition/classes.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
extern struct Library * SpeedBarBase;
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/speedbar.h>
 #ifdef __USE_INLINE__
  #include <inline4/speedbar.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_SPEEDBAR_PROTOS_H
  #define CLIB_SPEEDBAR_PROTOS_H 1
 #endif /* CLIB_SPEEDBAR_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct SpeedBarIFace *ISpeedBar;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_SPEEDBAR_PROTOS_H
  #include <clib/speedbar_protos.h>
 #endif /* CLIB_SPEEDBAR_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/speedbar.h>
  #else
   #include <ppcinline/speedbar.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/speedbar_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/speedbar_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_SPEEDBAR_H */
