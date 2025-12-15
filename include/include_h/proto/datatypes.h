#ifndef PROTO_DATATYPES_H
#define PROTO_DATATYPES_H

/*
**    $Id: datatypes.h,v 1.13 2010-01-31 01:29:36 ssolie Exp $
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
#ifndef   EXEC_LISTS_H
#include <exec/lists.h>
#endif
#ifndef   INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif
#ifndef   INTUITION_CLASSES_H
#include <intuition/classes.h>
#endif
#ifndef   INTUITION_CLASSUSR_H
#include <intuition/classusr.h>
#endif
#ifndef   INTUITION_GADGETCLASS_H
#include <intuition/gadgetclass.h>
#endif
#ifndef   UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif
#ifndef   DATATYPES_DATATYPESCLASS_H
#include <datatypes/datatypesclass.h>
#endif
#ifndef   DATATYPES_DATATYPES_H
#include <datatypes/datatypes.h>
#endif
#ifndef   REXX_STORAGE_H
#include <rexx/storage.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
extern struct Library * DataTypesBase;
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/datatypes.h>
 #ifdef __USE_INLINE__
  #include <inline4/datatypes.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_DATATYPES_PROTOS_H
  #define CLIB_DATATYPES_PROTOS_H 1
 #endif /* CLIB_DATATYPES_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct DataTypesIFace *IDataTypes;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_DATATYPES_PROTOS_H
  #include <clib/datatypes_protos.h>
 #endif /* CLIB_DATATYPES_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/datatypes.h>
  #else
   #include <ppcinline/datatypes.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/datatypes_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/datatypes_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_DATATYPES_H */
