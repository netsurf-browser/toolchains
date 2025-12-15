#ifndef PROTO_ONCHIPMEM_H
#define PROTO_ONCHIPMEM_H

/*
**	$Id$
**	Includes Release 50.1
**
**	Prototype/inline/pragma header file combo
**
**	Copyright (c) 2013 ACube Systems Srl.
**	All Rights Reserved.
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
 #ifndef __USE_BASETYPE__
  extern struct Library * OCMBase;
 #else
  extern struct Library * OCMBase;
 #endif /* __USE_BASETYPE__ */
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/onchipmem.h>
 #ifdef __USE_INLINE__
  #include <inline4/onchipmem.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_ONCHIPMEM_PROTOS_H
  #define CLIB_ONCHIPMEM_PROTOS_H 1
 #endif /* CLIB_ONCHIPMEM_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct OCMIFace *IOCM;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_ONCHIPMEM_PROTOS_H
  #include <clib/onchipmem_protos.h>
 #endif /* CLIB_ONCHIPMEM_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/onchipmem.h>
  #else /* __PPC__ */
   #include <ppcinline/onchipmem.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/onchipmem_protos.h>
  #endif /* __PPC__ */
 #else /* __GNUC__ */
  #include <pragmas/onchipmem_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_ONCHIPMEM_H */
