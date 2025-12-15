#ifndef PROTO_LOWLEVEL_H
#define PROTO_LOWLEVEL_H

/*
**    $Id: lowlevel.h,v 1.14 2010-01-31 01:29:36 ssolie Exp $
**
**    Prototype/inline/pragma header file combo
**
**    (C) Copyright 2003-2005 Hyperion Entertainment VOF and Amiga, Inc.
**        All Rights Reserved
**
**    Copyright (c) 2010 Hyperion Entertainment CVBA.
**        All Rights Reserved.
*/

#ifndef   EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef   EXEC_INTERRUPTS_H
#include <exec/interrupts.h>
#endif
#ifndef   UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif
#ifndef   DEVICES_TIMER_H
#include <devices/timer.h>
#endif
#ifndef   LIBRARIES_LOWLEVEL_H
#include <libraries/lowlevel.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
extern struct Library * LowLevelBase;
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/lowlevel.h>
 #ifdef __USE_INLINE__
  #include <inline4/lowlevel.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_LOWLEVEL_PROTOS_H
  #define CLIB_LOWLEVEL_PROTOS_H 1
 #endif /* CLIB_LOWLEVEL_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct LowLevelIFace *ILowLevel;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_LOWLEVEL_PROTOS_H
  #include <clib/lowlevel_protos.h>
 #endif /* CLIB_LOWLEVEL_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/lowlevel.h>
  #else
   #include <ppcinline/lowlevel.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/lowlevel_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/lowlevel_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_LOWLEVEL_H */
