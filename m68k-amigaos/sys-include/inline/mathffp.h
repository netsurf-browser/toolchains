#ifndef _INLINE_MATHFFP_H
#define _INLINE_MATHFFP_H

#ifndef CLIB_MATHFFP_PROTOS_H
#define CLIB_MATHFFP_PROTOS_H
#endif

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif

#ifndef  EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef MATHFFP_BASE_NAME
#define MATHFFP_BASE_NAME MathBase
#endif

#define SPFix(parm) \
	LP1(0x1e, LONG, SPFix, FLOAT, parm, d0, \
	, MATHFFP_BASE_NAME)

#define SPFlt(integer) \
	LP1(0x24, FLOAT, SPFlt, LONG, integer, d0, \
	, MATHFFP_BASE_NAME)

#define SPCmp(leftParm, rightParm) \
	LP2(0x2a, LONG, SPCmp, FLOAT, leftParm, d1, FLOAT, rightParm, d0, \
	, MATHFFP_BASE_NAME)

#define SPTst(parm) \
	LP1(0x30, LONG, SPTst, FLOAT, parm, d1, \
	, MATHFFP_BASE_NAME)

#define SPAbs(parm) \
	LP1(0x36, FLOAT, SPAbs, FLOAT, parm, d0, \
	, MATHFFP_BASE_NAME)

#define SPNeg(parm) \
	LP1(0x3c, FLOAT, SPNeg, FLOAT, parm, d0, \
	, MATHFFP_BASE_NAME)

#define SPAdd(leftParm, rightParm) \
	LP2(0x42, FLOAT, SPAdd, FLOAT, leftParm, d1, FLOAT, rightParm, d0, \
	, MATHFFP_BASE_NAME)

#define SPSub(leftParm, rightParm) \
	LP2(0x48, FLOAT, SPSub, FLOAT, leftParm, d1, FLOAT, rightParm, d0, \
	, MATHFFP_BASE_NAME)

#define SPMul(leftParm, rightParm) \
	LP2(0x4e, FLOAT, SPMul, FLOAT, leftParm, d1, FLOAT, rightParm, d0, \
	, MATHFFP_BASE_NAME)

#define SPDiv(leftParm, rightParm) \
	LP2(0x54, FLOAT, SPDiv, FLOAT, leftParm, d1, FLOAT, rightParm, d0, \
	, MATHFFP_BASE_NAME)

#define SPFloor(parm) \
	LP1(0x5a, FLOAT, SPFloor, FLOAT, parm, d0, \
	, MATHFFP_BASE_NAME)

#define SPCeil(parm) \
	LP1(0x60, FLOAT, SPCeil, FLOAT, parm, d0, \
	, MATHFFP_BASE_NAME)

#endif /*  _INLINE_MATHFFP_H  */
