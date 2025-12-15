#ifndef PROTO_AMIGAINPUT_H
#define PROTO_AMIGAINPUT_H

/*
**	$Id$
**
**	Prototype/inline/pragma header file combo
**
**	Copyright (c) 2010 Hyperion Entertainment CVBA.
**	All Rights Reserved.
*/

#ifndef AMIGAINPUT_AMIGAINPUT_H
#include <amigainput/amigainput.h>
#endif
#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
 extern struct Library * AIN_Base;
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/AmigaInput.h>
 #ifdef __USE_INLINE__
  #include <inline4/AmigaInput.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_AMIGAINPUT_PROTOS_H
  #define CLIB_AMIGAINPUT_PROTOS_H 1
 #endif /* CLIB_AMIGAINPUT_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct AIN_IFace *IAIN_;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_AMIGAINPUT_PROTOS_H
  #include <clib/AmigaInput_protos.h>
 #endif /* CLIB_AMIGAINPUT_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/AmigaInput.h>
  #else /* __PPC__ */
   #include <ppcinline/AmigaInput.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/AmigaInput_protos.h>
  #endif /* __PPC__ */
 #else /* __GNUC__ */
  #include <pragmas/AmigaInput_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_AMIGAINPUT_H */
