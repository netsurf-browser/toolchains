#ifndef PROTO_MOUNTER_H
#define PROTO_MOUNTER_H

/*
**	$Id$
**
**	Prototype/inline/pragma header file combo
**
**	Copyright (c) 2010 Hyperion Entertainment CVBA.
**	All Rights Reserved.
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef UTILITY_UTILITY_H
#include <utility/utility.h>
#endif
#ifndef LIBRARIES_MOUNTER_H
#include <libraries/mounter.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
 #ifndef __USE_BASETYPE__
  extern struct Library * MounterBase;
 #else
  extern struct MounterBase * MounterBase;
 #endif /* __USE_BASETYPE__ */
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/mounter.h>
 #ifdef __USE_INLINE__
  #include <inline4/mounter.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_MOUNTER_PROTOS_H
  #define CLIB_MOUNTER_PROTOS_H 1
 #endif /* CLIB_MOUNTER_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct MounterIFace *IMounter;
  extern struct MounterPrivateIFace *IMounterPrivate;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_MOUNTER_PROTOS_H
  #include <clib/mounter_protos.h>
 #endif /* CLIB_MOUNTER_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/mounter.h>
  #else /* __PPC__ */
   #include <ppcinline/mounter.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/mounter_protos.h>
  #endif /* __PPC__ */
 #else /* __GNUC__ */
  #include <pragmas/mounter_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_MOUNTER_H */
