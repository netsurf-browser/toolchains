#ifndef PROTO_GLYPH_H
#define PROTO_GLYPH_H

/*
**    $Id: glyph.h,v 1.11 2010-01-31 01:29:36 ssolie Exp $
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

/****************************************************************************/

#ifndef __NOLIBBASE__
extern struct Library * GlyphBase;
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/glyph.h>
 #ifdef __USE_INLINE__
  #include <inline4/glyph.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_GLYPH_PROTOS_H
  #define CLIB_GLYPH_PROTOS_H 1
 #endif /* CLIB_GLYPH_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct GlyphIFace *IGlyph;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_GLYPH_PROTOS_H
  #include <clib/glyph_protos.h>
 #endif /* CLIB_GLYPH_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/glyph.h>
  #else
   #include <ppcinline/glyph.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/glyph_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/glyph_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_GLYPH_H */
