#ifndef PROTO_TIMER_H
#define PROTO_TIMER_H

/*
**    $Id: timer.h,v 1.14 2010-01-31 01:29:36 ssolie Exp $
**
**    Prototype/inline/pragma header file combo
**
**    (C) Copyright 2003-2005 Hyperion Entertainment VOF and Amiga, Inc.
**        All Rights Reserved
**
**    Copyright (c) 2010 Hyperion Entertainment CVBA.
**        All Rights Reserved.
*/

#ifndef   DEVICES_TIMER_H
#include <devices/timer.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
extern struct Device * TimerBase;
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/timer.h>
 #ifdef __USE_INLINE__
  #include <inline4/timer.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_TIMER_PROTOS_H
  #define CLIB_TIMER_PROTOS_H 1
 #endif /* CLIB_TIMER_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct TimerIFace *ITimer;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_TIMER_PROTOS_H
  #include <clib/timer_protos.h>
 #endif /* CLIB_TIMER_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/timer.h>
  #else
   #include <ppcinline/timer.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/timer_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/timer_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_TIMER_H */
