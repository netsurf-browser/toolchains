#ifndef PROTO_IFFPARSE_H
#define PROTO_IFFPARSE_H

/*
**    $Id: iffparse.h,v 1.13 2010-01-31 01:29:36 ssolie Exp $
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
#ifndef   LIBRARIES_IFFPARSE_H
#include <libraries/iffparse.h>
#endif
#ifndef   UTILITY_HOOKS_H
#include <utility/hooks.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
extern struct Library * IFFParseBase;
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/iffparse.h>
 #ifdef __USE_INLINE__
  #include <inline4/iffparse.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_IFFPARSE_PROTOS_H
  #define CLIB_IFFPARSE_PROTOS_H 1
 #endif /* CLIB_IFFPARSE_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct IFFParseIFace *IIFFParse;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_IFFPARSE_PROTOS_H
  #include <clib/iffparse_protos.h>
 #endif /* CLIB_IFFPARSE_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/iffparse.h>
  #else
   #include <ppcinline/iffparse.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/iffparse_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/iffparse_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_IFFPARSE_H */
