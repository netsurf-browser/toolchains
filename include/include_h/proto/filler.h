#ifndef PROTO_FILLER_H
#define PROTO_FILLER_H

/*
**    $Id: filler.h,v 1.6 2010-01-31 01:29:36 ssolie Exp $
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
extern struct Library * FillerBase;
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/filler.h>
 #ifdef __USE_INLINE__
  #include <inline4/filler.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_FILLER_PROTOS_H
  #define CLIB_FILLER_PROTOS_H 1
 #endif /* CLIB_FILLER_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct FillerIFace *IFiller;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_FILLER_PROTOS_H
  #include <clib/filler_protos.h>
 #endif /* CLIB_FILLER_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/filler.h>
  #else
   #include <ppcinline/filler.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/filler_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/filler_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_FILLER_H */
