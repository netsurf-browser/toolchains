#ifndef PROTO_INPUT_H
#define PROTO_INPUT_H

/*
**    $Id: input.h,v 1.11 2010-01-31 01:29:36 ssolie Exp $
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
#ifndef   EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

#ifndef __NOLIBBASE__
extern struct Library *InputBase;
#endif

#ifdef __amigaos4__
#include <interfaces/input.h>
#ifdef __USE_INLINE__
#include <inline4/input.h>
#endif /* __USE_INLINE__ */
#ifndef CLIB_INPUT_PROTOS_H
 #define CLIB_INPUT_PROTOS_H 1
#endif /* CLIB_INPUT_PROTOS_H */
#ifndef __NOGLOBALIFACE__
extern struct InputIFace *IInput;
#endif /* __NOGLOBALIFACE__*/
#else /* __amigaos4__ */
#ifndef CLIB_INPUT_PROTOS_H
 #include <clib/input_protos.h>
#endif /* CLIB_INPUT_PROTOS_H */
#ifdef __GNUC__
#ifndef __PPC__
 #include <inline/input.h>
#else
 #include <ppcinline/input.h>
#endif /* __PPC__ */
#else
#include <pragmas/input_pragmas.h>
#endif /* __GNUC__ */
#endif /* __amigaos4__ */
#endif /* PROTO_INPUT_H */
