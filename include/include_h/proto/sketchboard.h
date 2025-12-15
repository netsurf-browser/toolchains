#ifndef PROTO_SKETCHBOARD_H
#define PROTO_SKETCHBOARD_H

/*
**    $Id: sketchboard.h,v 1.6 2010-01-31 01:29:36 ssolie Exp $
**
**    Prototype/inline/pragma header file combo
**
**    (C) Copyright 2001-2005 Hyperion Entertainment VOF and Amiga, Inc.
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
extern struct Library * SketchBoardBase;
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #ifdef __USE_INLINE__
  #include <inline4/sketchboard.h>
 #endif /* __USE_INLINE__ */

 #include <interfaces/sketchboard.h>

 #ifndef __NOGLOBALIFACE__
  extern struct SketchBoardIFace *ISketchBoard;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #if defined(__GNUC__)
  #include <inline/sketchboard.h>
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/sketchboard_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/sketchboard_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_SKETCHBOARD_H */
