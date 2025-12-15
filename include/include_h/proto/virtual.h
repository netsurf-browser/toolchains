#ifndef PROTO_VIRTUAL_H
#define PROTO_VIRTUAL_H

/*
**    $Id: virtual.h,v 1.11 2010-01-31 01:29:38 ssolie Exp $
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
#ifndef   INTUITION_CLASSUSR_H
#include <intuition/classusr.h>
#endif
#ifndef   GADGETS_LAYOUT_H
#include <gadgets/layout.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
extern struct Library * VirtualBase;
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/virtual.h>
 #ifdef __USE_INLINE__
  #include <inline4/virtual.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_VIRTUAL_PROTOS_H
  #define CLIB_VIRTUAL_PROTOS_H 1
 #endif /* CLIB_VIRTUAL_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct VirtualIFace *IVirtual;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_VIRTUAL_PROTOS_H
  #include <clib/virtual_protos.h>
 #endif /* CLIB_VIRTUAL_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/virtual.h>
  #else
   #include <ppcinline/virtual.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/virtual_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/virtual_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_VIRTUAL_H */
