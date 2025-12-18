/* GEM Resource C Source */

#include <portab.h>
#include <gem.h>
#include "TSRINFO.H"

#if !defined(WHITEBAK)
#define WHITEBAK    0x0040
#endif
#if !defined(DRAW3D)
#define DRAW3D      0x0080
#endif

#define FLAGS9  0x0200
#define FLAGS10 0x0400
#define FLAGS11 0x0800
#define FLAGS12 0x1000
#define FLAGS13 0x2000
#define FLAGS14 0x4000
#define FLAGS15 0x8000
#define STATE8  0x0100
#define STATE9  0x0200
#define STATE10 0x0400
#define STATE11 0x0800
#define STATE12 0x1000
#define STATE13 0x2000
#define STATE14 0x4000
#define STATE15 0x8000

OBJECT rs_object[] =
{ 
  /******** Tree 0 TSRINFO ****************************************************/
        -1, TSRVERSION      , TSRMESAG        , G_BOX             ,   /* Object 0  */
  FLAGS9|FLAGS10, OUTLINED, (LONG)0x00021100L,
  0x0000, 0x0000, 0x002F, 0x0004,
         2,       -1,       -1, G_STRING          ,   /* Object 1 TSRVERSION */
  NONE, NORMAL, (LONG)"Librairies Dynamiques GEM version ________",
  0x0401, 0x0800, 0x002A, 0x0001,
  TSRMESAG        ,       -1,       -1, G_STRING          ,   /* Object 2  */
  NONE, NORMAL, (LONG)"(c) 2003 by Landemarre, B\202r\202ziat & Bercegeay",
  0x0001, 0x0801, 0x002C, 0x0001,
         0,       -1,       -1, G_STRING          ,   /* Object 3 TSRMESAG */
  LASTOB, NORMAL, (LONG)"Bee dee___________________________________",
  0x0401, 0x0802, 0x002A, 0x0001
};

OBJECT *rs_trindex[] =
{ &rs_object[0]    /* Tree  0 TSRINFO          */
};
