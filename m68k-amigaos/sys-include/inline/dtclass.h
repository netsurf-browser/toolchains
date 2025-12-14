#ifndef _INLINE_DTCLASS_H
#define _INLINE_DTCLASS_H

#ifndef CLIB_DTCLASS_PROTOS_H
#define CLIB_DTCLASS_PROTOS_H
#endif

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif

#ifndef  EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef  INTUITION_CLASSES_H
#include <intuition/classes.h>
#endif
#ifndef  INTUITION_CLASSUSR_H
#include <intuition/classusr.h>
#endif

#ifndef DTCLASS_BASE_NAME
#define DTCLASS_BASE_NAME DTClassBase
#endif

#define ObtainEngine() \
	LP0(0x1e, Class *, ObtainEngine, \
	, DTCLASS_BASE_NAME)

#endif /*  _INLINE_DTCLASS_H  */
