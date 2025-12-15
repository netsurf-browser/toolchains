#ifndef PROTO_LABEL_H
#define PROTO_LABEL_H

/*
**    $Id: label.h,v 1.11 2010-01-31 01:29:36 ssolie Exp $
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
extern struct Library * LabelBase;
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/label.h>
 #ifdef __USE_INLINE__
  #include <inline4/label.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_LABEL_PROTOS_H
  #define CLIB_LABEL_PROTOS_H 1
 #endif /* CLIB_LABEL_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct LabelIFace *ILabel;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_LABEL_PROTOS_H
  #include <clib/label_protos.h>
 #endif /* CLIB_LABEL_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/label.h>
  #else
   #include <ppcinline/label.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/label_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/label_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_LABEL_H */
