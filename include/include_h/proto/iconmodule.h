#ifndef PROTO_ICONMODULE_H
#define PROTO_ICONMODULE_H

/*
**	$Id: iconmodule.h,v 1.2 2010-01-31 01:29:36 ssolie Exp $
**
**	Prototype/inline/pragma header file combo
**
**	(C) Copyright 2003-2006 Amiga, Inc.
**	    All Rights Reserved
**
** Copyright (c) 2010 Hyperion Entertainment CVBA.
**     All Rights Reserved.
*/

#ifndef WORKBENCH_WORKBENCH_H
#include <workbench/workbench.h>
#endif
#ifndef WORKBENCH_ICON_H
#include <workbench/icon.h>
#endif
#ifndef WORKBENCH_ICONMODULE_H
#include <workbench/iconmodule.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
 #ifndef __USE_BASETYPE__
  extern struct Library * IconModuleBase;
 #else
  extern struct Library * IconModuleBase;
 #endif /* __USE_BASETYPE__ */
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/iconmodule.h>
 #ifdef __USE_INLINE__
  #include <inline4/iconmodule.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_ICONMODULE_PROTOS_H
  #define CLIB_ICONMODULE_PROTOS_H 1
 #endif /* CLIB_ICONMODULE_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct IconModuleIFace *IIconModule;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_ICONMODULE_PROTOS_H
  #include <clib/iconmodule_protos.h>
 #endif /* CLIB_ICONMODULE_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/iconmodule.h>
  #else
   #include <ppcinline/iconmodule.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/iconmodule_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/iconmodule_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_ICONMODULE_H */
