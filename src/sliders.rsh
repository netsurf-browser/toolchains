/* GEM Resource C Source */

#include "sliders.h"

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

static TEDINFO rs_tedinfo[] =
{ {"Informations",
  "\0",
  "\0",
  IBM  , 0, TE_LEFT , 0x1180, 0, -1, 13, 1}
};

static OBJECT rs_object[] =
{ 
  /******** Tree 0 FRAME_SLIDER ****************************************************/
  {      -1, M_V_BG          , M_INFO          , G_BOX             ,   /* Object 0  */
  TOUCHEXIT, OUTLINED, {(LONG)0x00011100L},
  0x0000, 0x0000, 0x041A, 0x0008},
  {M_H_BG          , M_V_UP          , M_V_PG          , G_IBOX            ,   /* Object 1 M_V_BG */
  TOUCHEXIT, NORMAL, {(LONG)0x00001100L},
  0x0616, 0x0801, 0x0402, 0x0C05},
  {M_V_DW          ,       -1,       -1, G_BOXCHAR         ,   /* Object 2 M_V_UP */
  SELECTABLE, DRAW3D, {(LONG) ((LONG)'\001' << 24)|0x00021100L},
  0x0000, 0x0000, 0x0402, 0x0401},
  {M_V_PG          ,       -1,       -1, G_BOXCHAR         ,   /* Object 3 M_V_DW */
  SELECTABLE, DRAW3D, {(LONG) ((LONG)'\002' << 24)|0x00021100L},
  0x0000, 0x0804, 0x0402, 0x0401},
  {M_V_BG          , M_V_SL          , M_V_SL          , G_BOX             ,   /* Object 4 M_V_PG */
  NONE, DRAW3D, {(LONG)0x00021179L},
  0x0000, 0x0301, 0x0402, 0x0603},
  {M_V_PG          ,       -1,       -1, G_BOX             ,   /* Object 5 M_V_SL */
  SELECTABLE, DRAW3D, {(LONG)0x00021100L},
  0x0000, 0x0800, 0x0402, 0x0801},
  {M_SZ            , M_H_LF          , M_H_PG          , G_IBOX            ,   /* Object 6 M_H_BG */
  TOUCHEXIT, NORMAL, {(LONG)0x00001100L},
  0x0401, 0x0002, 0x0511, 0x0401},
  {M_H_RT          ,       -1,       -1, G_BOXCHAR         ,   /* Object 7 M_H_LF */
  SELECTABLE, DRAW3D, {(LONG) ((LONG)'\004' << 24)|0x00021100L},
  0x0000, 0x0000, 0x0402, 0x0401},
  {M_H_PG          ,       -1,       -1, G_BOXCHAR         ,   /* Object 8 M_H_RT */
  SELECTABLE, DRAW3D, {(LONG) ((LONG)'\003' << 24)|0x00021100L},
  0x050E, 0x0000, 0x0402, 0x0401},
  {M_H_BG          , M_H_SL          , M_H_SL          , G_BOX             ,   /* Object 9 M_H_PG */
  NONE, DRAW3D, {(LONG)0x00021179L},
  0x0302, 0x0000, 0x030C, 0x0401},
  {M_H_PG          ,       -1,       -1, G_BOX             ,   /* Object 10 M_H_SL */
  SELECTABLE, DRAW3D, {(LONG)0x00021100L},
  0x0503, 0x0000, 0x0405, 0x0401},
  {M_INFO          ,       -1,       -1, G_BOXCHAR         ,   /* Object 11 M_SZ */
  SELECTABLE, DRAW3D, {(LONG) ((LONG)'\006' << 24)|0x00021100L},
  0x0401, 0x0005, 0x0402, 0x0401},
  {       0,       -1,       -1, G_BOXTEXT         ,   /* Object 12 M_INFO */
  LASTOB, NORMAL, {(LONG)&rs_tedinfo[0]},
  0x0201, 0x0800, 0x0512, 0x0101}
};

static OBJECT *rs_trindex[] =
{ &rs_object[0]    /* Tree  0 FRAME_SLIDER     */
};
