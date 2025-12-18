/* GEM Resource C Source */

#include <portab.h>
#include <aes.h>
#include "LDGCPX.H"

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

TEDINFO rs_tedinfo[] =
{ "Menu",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1100, 0, -1, 5, 1,
  "Ins",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1200, 0, -1, 4, 1,
  "Undo",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1200, 0, -1, 5, 1,
  "Space",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1200, 0, -1, 6, 1,
  "   ",
  "TSR version : _.__",
  "999",
  IBM  , 0, TE_CNTR , 0x1180, 0, -2, 4, 19,
  "___________________",
  "Path libs: ___________________",
  "PPPPPPPPPPPPPPPPPPP",
  IBM  , 0, TE_CNTR , 0x1180, 0, -2, 20, 31,
  " LDG cpx : configuration        ",
  "\0",
  "\0",
  IBM  , 0, TE_LEFT , 0x11F1, 0, 0, 33, 1,
  "F6",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1200, 0, -1, 3, 1,
  "    ",
  "Garbage time: ____ sec",
  "9999",
  IBM  , 0, TE_CNTR , 0x1180, 0, -2, 5, 23,
  "BubbleGEM inside !",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1100, 0, 1, 19, 1,
  "by D.B\202r\202ziat",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1400, 0, 1, 14, 1,
  "and O.Landemarre",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1400, 0, 1, 17, 1,
  " LDG cpx : expert actions       ",
  "\0",
  "\0",
  IBM  , 0, TE_LEFT , 0x11F1, 0, 0, 33, 1,
  "   ",
  "Lib loaded: ___",
  "999",
  IBM  , 0, TE_CNTR , 0x1180, 0, -2, 4, 16,
  " action ",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 9, 1,
  "F6",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1200, 0, -1, 3, 1,
  "  tsr  ",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 8, 1,
  "F7",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1200, 0, -1, 3, 1,
  "    ",
  "Idle time: ____ msec",
  "9999",
  IBM  , 0, TE_CNTR , 0x1180, 0, -2, 5, 21,
  " Internal ",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1180, 0, 0, 11, 1,
  " LDG 1.xx specific ",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1180, 0, 0, 20, 1,
  "_____________",
  "Select a lib >> _____________",
  "PPPPPPPPPPPPP",
  IBM  , 0, TE_CNTR , 0x1180, 0, -2, 14, 30,
  "   ",
  "Version: _.__",
  "999",
  SMALL, 0, TE_CNTR , 0x1180, 0, -2, 4, 14,
  " no functions ",
  "\0",
  "\0",
  IBM  , 0, TE_CNTR , 0x1180, 0, -1, 15, 1,
  "   ",
  "Shareable: ___",
  "XXX",
  SMALL, 0, TE_CNTR , 0x1180, 0, -2, 4, 15,
  "   ",
  "Locked: ___",
  "XXX",
  SMALL, 0, TE_CNTR , 0x1180, 0, -2, 4, 12,
  "   ",
  "Resident: ___",
  "XXX",
  SMALL, 0, TE_CNTR , 0x1180, 0, -2, 4, 14,
  "   ",
  "Closure function: ___",
  "XXX",
  SMALL, 0, TE_CNTR , 0x1180, 0, -2, 4, 22,
  "   ",
  "Private extension: ___",
  "XXX",
  SMALL, 0, TE_CNTR , 0x1180, 0, -2, 4, 23,
  "   ",
  "Special extension: ___",
  "XXX",
  SMALL, 0, TE_CNTR , 0x1180, 0, -2, 4, 23,
  "   ",
  "LDG protocol: _.__",
  "999",
  SMALL, 0, TE_CNTR , 0x1180, 0, -2, 4, 19,
  " LDG cpx : libraries            ",
  "\0",
  "\0",
  IBM  , 0, TE_LEFT , 0x11F1, 0, 0, 33, 1,
  "F6",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1200, 0, -1, 3, 1,
  "F7",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1200, 0, -1, 3, 1,
  "All rigths reserved by the authors.",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1400, 0, -1, 36, 1,
  "It\047s a freeware product so diffuse-it!",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1400, 0, -1, 39, 1,
  "\275 1997-2000.",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1400, 0, -1, 13, 1,
  "Dominique.B\202r\202ziat@inria.fr ",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1100, 0, 0, 29, 1,
  "Olivier.Landemarre@utbm.fr",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1100, 0, 0, 27, 1,
  "http://ldg.atari.org/",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1100, 0, -1, 22, 1,
  "CPX VERSION:",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1C00, 0, -1, 13, 1,
  " LDG cpx : informations         ",
  "\0",
  "\0",
  IBM  , 0, TE_LEFT , 0x11F1, 0, 0, 33, 1,
  "LDG stands for Librairies Dynamiques GEM",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1400, 0, -1, 41, 1,
  "For details, see the LDG",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1400, 0, -1, 25, 1,
  "documentation.",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1400, 0, -1, 15, 1,
  "The CPX has been written by D. B\202r\202ziat",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1400, 0, -1, 40, 1,
  "2.20  ",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1C00, 0, -1, 7, 1,
  " LDG cpx : comments             ",
  "\0",
  "\0",
  IBM  , 0, TE_LEFT , 0x11F1, 0, 0, 33, 1,
  "___________________\000\000\000\000\000\000\000\000\000\000\000",
  "______________________________",
  "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
  IBM  , 0, TE_CNTR , 0x1180, 0, -2, 31, 31,
  "___________________\000\000\000\000\000\000\000\000\000\000\000",
  "______________________________",
  "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
  IBM  , 0, TE_CNTR , 0x1180, 0, -2, 31, 31,
  "___________________\000\000\000\000\000\000\000\000\000\000\000",
  "______________________________",
  "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
  IBM  , 0, TE_CNTR , 0x1180, 0, -2, 31, 31,
  "___________________\000\000\000\000\000\000\000\000\000\000\000",
  "______________________________",
  "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
  IBM  , 0, TE_CNTR , 0x1180, 0, -2, 31, 31,
  "___________________\000\000\000\000\000\000\000\000\000\000\000",
  "______________________________",
  "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
  IBM  , 0, TE_CNTR , 0x1180, 0, -2, 31, 31,
  "___________________\000\000\000\000\000\000\000\000\000\000\000",
  "______________________________",
  "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
  IBM  , 0, TE_CNTR , 0x1180, 0, -2, 31, 31,
  "copy to clipboard",
  "\0",
  "\0",
  SMALL, 0, TE_CNTR , 0x1180, 0, -1, 18, 1,
  "^W",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1200, 0, -1, 3, 1,
  "___________________\000\000\000\000\000\000\000\000\000\000\000",
  "______________________________",
  "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
  IBM  , 0, TE_CNTR , 0x1180, 0, -2, 31, 31,
  "100",
  "vers LDG _.__",
  "999",
  SMALL, 0, TE_CNTR , 0x1180, 0, -2, 4, 14,
  "100",
  "vers LIB _.__",
  "999",
  SMALL, 0, TE_CNTR , 0x1180, 0, -2, 4, 14,
  " LDG cpx : expert actions       ",
  "\0",
  "\0",
  IBM  , 0, TE_LEFT , 0x11F1, 0, 0, 33, 1,
  "LDG 2.xx specific",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1100, 0, -1, 18, 1,
  "______",
  "Manager: ______",
  "XXXXXX",
  SMALL, 0, TE_CNTR , 0x1180, 0, -2, 7, 16,
  "   ",
  "Libs loaded: ___",
  "999",
  IBM  , 0, TE_CNTR , 0x1180, 0, -2, 4, 17,
  " action ",
  "\0",
  "\0",
  IBM  , 0, TE_LEFT , 0x1180, 0, -1, 9, 1,
  "F6",
  "\0",
  "\0",
  SMALL, 0, TE_LEFT , 0x1200, 0, -1, 3, 1
};

WORD RSBB0DATA[] =
{ 0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x007F, 0x01FF, 
  0xC007, 0xE600, 0x3E00, 0x001E, 
  0x0078, 0xE01E, 0x3E00, 0x7F00, 
  0x001C, 0x0078, 0x3038, 0x1E00, 
  0xC780, 0x001C, 0x0070, 0x3870, 
  0x0C01, 0x8180, 0x003C, 0x00F0, 
  0x1CE0, 0x0C00, 0x0180, 0x0038, 
  0x00F0, 0x1DC0, 0x0C00, 0x0180, 
  0x0038, 0x00E0, 0x1DC0, 0x0000, 
  0x0100, 0x0038, 0x01E0, 0x1DC0, 
  0x0000, 0x0700, 0x0078, 0x01E0, 
  0x1F80, 0x0000, 0x0600, 0x0070, 
  0x01E0, 0x1F81, 0xFE00, 0x0C00, 
  0x0070, 0x01C0, 0x3B80, 0x7800, 
  0x1800, 0x0070, 0x03C0, 0x3B80, 
  0x7000, 0x3000, 0x00F0, 0x63C0, 
  0x7380, 0xF000, 0x6000, 0x00E0, 
  0x6380, 0xE1C0, 0xF000, 0xC100, 
  0x00E0, 0xC781, 0xC1C0, 0xE001, 
  0x8100, 0x01E1, 0xC787, 0x80E1, 
  0xE003, 0xFF00, 0x03FF, 0xDFFE, 
  0x003F, 0x8007, 0xFE00, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000
};

WORD RSBB1DATA[] =
{ 0x0000, 0x0000, 0x3FFC, 0x4002, 
  0x4002, 0x4002, 0x4002, 0x4002, 
  0x4002, 0x3C3C, 0x05C0, 0x0E00, 
  0x1800, 0x2000, 0x0000, 0x0000
};

BITBLK rs_bitblk[] =
{ RSBB0DATA,  10,  30,   0,   0, 0x0004,
  RSBB1DATA,   2,  16,   0,   0, 0x0001
};

BYTE *rs_frstr[] =
{ "[2][The LDG manager is not|in memory.][Ok|Try load it]",
  " LDG cpx : loading manager... ",
  " LDG cpx : inquire manager...  ",
  " LDG cpx : LDG 1.xx compatible  "
};

OBJECT rs_object[] =
{ 
  /******** Tree 0 CPXFORM ****************************************************/
        -1,        1, COMMENT         , G_BOX             ,   /* Object 0  */
  FLAGS9, NORMAL, (LONG)0x00FF1100L,
  0x0000, 0x0000, 0x0020, 0x000B,
  CONFIGURE       , CNF_SAVE        ,        8, G_BOX             ,   /* Object 1  */
  FLAGS9, NORMAL, (LONG)0x00FF1100L,
  0x0000, 0x0009, 0x0020, 0x0002,
  CNF_CANCEL      ,       -1,       -1, G_BUTTON          ,   /* Object 2 CNF_SAVE */
  SELECTABLE|EXIT|FLAGS9|FLAGS10, NORMAL, (LONG)"Save",
  0x0407, 0x0800, 0x0405, 0x0001,
  CNF_OK          ,       -1,       -1, G_BUTTON          ,   /* Object 3 CNF_CANCEL */
  SELECTABLE|EXIT|FLAGS9|FLAGS10, NORMAL, (LONG)"Cancel",
  0x040E, 0x0800, 0x0007, 0x0001,
  CNF_MENU        ,       -1,       -1, G_BUTTON          ,   /* Object 4 CNF_OK */
  SELECTABLE|DEFAULT|EXIT|FLAGS9|FLAGS10, NORMAL, (LONG)"Ok",
  0x0017, 0x0800, 0x0007, 0x0001,
         6,       -1,       -1, G_BOXTEXT         ,   /* Object 5 CNF_MENU */
  SELECTABLE|TOUCHEXIT|FLAGS9|FLAGS10, SHADOWED, (LONG)&rs_tedinfo[0],
  0x0001, 0x0800, 0x0604, 0x0001,
         7,       -1,       -1, G_TEXT            ,   /* Object 6  */
  NONE, NORMAL, (LONG)&rs_tedinfo[1],
  0x0009, 0x0000, 0x0202, 0x0800,
         8,       -1,       -1, G_TEXT            ,   /* Object 7  */
  NONE, NORMAL, (LONG)&rs_tedinfo[2],
  0x0410, 0x0000, 0x0003, 0x0800,
         1,       -1,       -1, G_TEXT            ,   /* Object 8  */
  NONE, NORMAL, (LONG)&rs_tedinfo[3],
  0x0401, 0x0000, 0x0603, 0x0800,
  EXPERT          , CNF_VERSION     ,       19, G_BOX             ,   /* Object 9 CONFIGURE */
  FLAGS9, NORMAL, (LONG)0x00FF1100L,
  0x0000, 0x0000, 0x0020, 0x0009,
  CNF_PATH        ,       -1,       -1, G_FTEXT           ,   /* Object 10 CNF_VERSION */
  FLAGS10, NORMAL, (LONG)&rs_tedinfo[4],
  0x0001, 0x0806, 0x0012, 0x0001,
        12,       -1,       -1, G_FTEXT           ,   /* Object 11 CNF_PATH */
  SELECTABLE|EXIT|FLAGS10, DRAW3D, (LONG)&rs_tedinfo[5],
  0x0001, 0x0803, 0x001E, 0x0001,
        13,       -1,       -1, G_TEXT            ,   /* Object 12  */
  NONE, SELECTED, (LONG)&rs_tedinfo[6],
  0x0000, 0x0000, 0x0020, 0x0001,
        14,       -1,       -1, G_TEXT            ,   /* Object 13  */
  NONE, NORMAL, (LONG)&rs_tedinfo[7],
  0x000A, 0x0003, 0x0401, 0x0001,
  CNF_GARBAGE     ,       -1,       -1, G_IMAGE           ,   /* Object 14  */
  NONE, NORMAL, (LONG)&rs_bitblk[0],
  0x0403, 0x0001, 0x000A, 0x0E01,
        16,       -1,       -1, G_FTEXT           ,   /* Object 15 CNF_GARBAGE */
  EDITABLE|FLAGS9, DRAW3D, (LONG)&rs_tedinfo[8],
  0x0001, 0x0005, 0x0016, 0x0001,
        17,       -1,       -1, G_IMAGE           ,   /* Object 16  */
  NONE, NORMAL, (LONG)&rs_bitblk[1],
  0x001E, 0x0807, 0x0002, 0x0001,
        18,       -1,       -1, G_TEXT            ,   /* Object 17  */
  NONE, NORMAL, (LONG)&rs_tedinfo[9],
  0x0012, 0x0808, 0x000E, 0x0800,
        19,       -1,       -1, G_TEXT            ,   /* Object 18  */
  NONE, NORMAL, (LONG)&rs_tedinfo[10],
  0x000F, 0x0801, 0x040A, 0x0800,
  CONFIGURE       ,       -1,       -1, G_TEXT            ,   /* Object 19  */
  NONE, NORMAL, (LONG)&rs_tedinfo[11],
  0x000E, 0x0002, 0x040C, 0x0800,
  LIBRARY         ,       21,       33, G_BOX             ,   /* Object 20 EXPERT */
  FLAGS9, NORMAL, (LONG)0x00FF1100L,
  0x0000, 0x0000, 0x0020, 0x0009,
        22,       -1,       -1, G_TEXT            ,   /* Object 21  */
  NONE, SELECTED, (LONG)&rs_tedinfo[12],
  0x0000, 0x0000, 0x0020, 0x0001,
        30, EXP_LIBLOADED   ,       29, G_IBOX            ,   /* Object 22  */
  NONE, NORMAL, (LONG)0x00FF1141L,
  0x0001, 0x0801, 0x001E, 0x0004,
  EXP_REQUEST     ,       -1,       -1, G_FTEXT           ,   /* Object 23 EXP_LIBLOADED */
  FLAGS10, NORMAL, (LONG)&rs_tedinfo[13],
  0x000D, 0x0800, 0x000F, 0x0001,
        25,       -1,       -1, G_BOXTEXT         ,   /* Object 24 EXP_REQUEST */
  SELECTABLE|TOUCHEXIT, SHADOWED, (LONG)&rs_tedinfo[14],
  0x0014, 0x0002, 0x0008, 0x0001,
        26,       -1,       -1, G_STRING          ,   /* Object 25  */
  NONE, NORMAL, (LONG)"Send :",
  0x000D, 0x0002, 0x0006, 0x0001,
  EXP_TO          ,       -1,       -1, G_TEXT            ,   /* Object 26  */
  NONE, NORMAL, (LONG)&rs_tedinfo[15],
  0x0014, 0x0801, 0x0401, 0x0600,
        28,       -1,       -1, G_BOXTEXT         ,   /* Object 27 EXP_TO */
  SELECTABLE|TOUCHEXIT, SHADOWED, (LONG)&rs_tedinfo[16],
  0x0400, 0x0801, 0x0008, 0x0001,
        29,       -1,       -1, G_TEXT            ,   /* Object 28  */
  NONE, NORMAL, (LONG)&rs_tedinfo[17],
  0x0407, 0x0001, 0x0401, 0x0600,
        22,       -1,       -1, G_STRING          ,   /* Object 29  */
  NONE, NORMAL, (LONG)"Target",
  0x0400, 0x0800, 0x0006, 0x0001,
        32, EXP_IDLE        , EXP_IDLE        , G_IBOX            ,   /* Object 30  */
  NONE, NORMAL, (LONG)0x00FF1141L,
  0x0001, 0x0806, 0x001E, 0x0002,
        30,       -1,       -1, G_FTEXT           ,   /* Object 31 EXP_IDLE */
  EDITABLE|FLAGS9, DRAW3D, (LONG)&rs_tedinfo[18],
  0x0001, 0x0800, 0x0014, 0x0001,
        33,       -1,       -1, G_BOXTEXT         ,   /* Object 32  */
  FLAGS10, NORMAL, (LONG)&rs_tedinfo[19],
  0x0002, 0x0001, 0x0407, 0x0001,
  EXPERT          ,       -1,       -1, G_BOXTEXT         ,   /* Object 33  */
  FLAGS10, NORMAL, (LONG)&rs_tedinfo[20],
  0x0002, 0x0006, 0x020E, 0x0001,
  INFORMATION     , LIB_SELECT      ,       48, G_BOX             ,   /* Object 34 LIBRARY */
  FLAGS9, NORMAL, (LONG)0x00FF1100L,
  0x0000, 0x0000, 0x0020, 0x0009,
  LIB_VERSION     ,       -1,       -1, G_FTEXT           ,   /* Object 35 LIB_SELECT */
  SELECTABLE|TOUCHEXIT|FLAGS10, DRAW3D, (LONG)&rs_tedinfo[21],
  0x0101, 0x0D01, 0x001D, 0x0001,
  LIB_LIST        ,       -1,       -1, G_FTEXT           ,   /* Object 36 LIB_VERSION */
  FLAGS9, DRAW3D, (LONG)&rs_tedinfo[22],
  0x0302, 0x0605, 0x0609, 0x0800,
  LIB_SHARE       ,       -1,       -1, G_BOXTEXT         ,   /* Object 37 LIB_LIST */
  SELECTABLE|TOUCHEXIT, SHADOWED, (LONG)&rs_tedinfo[23],
  0x0011, 0x0803, 0x000E, 0x0001,
  LIB_LOCKED      ,       -1,       -1, G_FTEXT           ,   /* Object 38 LIB_SHARE */
  FLAGS9, DRAW3D, (LONG)&rs_tedinfo[24],
  0x0700, 0x0406, 0x040A, 0x0800,
  LIB_RESIDENT    ,       -1,       -1, G_FTEXT           ,   /* Object 39 LIB_LOCKED */
  FLAGS9, DRAW3D, (LONG)&rs_tedinfo[25],
  0x0103, 0x0008, 0x0208, 0x0800,
  LIB_CLOSURE     ,       -1,       -1, G_FTEXT           ,   /* Object 40 LIB_RESIDENT */
  FLAGS9, DRAW3D, (LONG)&rs_tedinfo[26],
  0x0501, 0x0207, 0x0609, 0x0800,
  LIB_EXT         ,       -1,       -1, G_FTEXT           ,   /* Object 41 LIB_CLOSURE */
  FLAGS9, DRAW3D, (LONG)&rs_tedinfo[27],
  0x010E, 0x0606, 0x060F, 0x0800,
  LIB_SPEC        ,       -1,       -1, G_FTEXT           ,   /* Object 42 LIB_EXT */
  FLAGS9, DRAW3D, (LONG)&rs_tedinfo[28],
  0x030D, 0x0307, 0x0410, 0x0800,
  LIB_PROTOCOL    ,       -1,       -1, G_FTEXT           ,   /* Object 43 LIB_SPEC */
  FLAGS9, DRAW3D, (LONG)&rs_tedinfo[29],
  0x030D, 0x0008, 0x0410, 0x0800,
        45,       -1,       -1, G_FTEXT           ,   /* Object 44 LIB_PROTOCOL */
  EDITABLE|FLAGS9, DRAW3D, (LONG)&rs_tedinfo[30],
  0x0111, 0x0805, 0x040D, 0x0800,
        46,       -1,       -1, G_TEXT            ,   /* Object 45  */
  NONE, SELECTED, (LONG)&rs_tedinfo[31],
  0x0000, 0x0000, 0x0020, 0x0001,
        47,       -1,       -1, G_STRING          ,   /* Object 46  */
  NONE, NORMAL, (LONG)"List functions:",
  0x0001, 0x0803, 0x000F, 0x0001,
        48,       -1,       -1, G_TEXT            ,   /* Object 47  */
  NONE, NORMAL, (LONG)&rs_tedinfo[32],
  0x040E, 0x0801, 0x0401, 0x0800,
  LIBRARY         ,       -1,       -1, G_TEXT            ,   /* Object 48  */
  NONE, NORMAL, (LONG)&rs_tedinfo[33],
  0x000F, 0x0003, 0x0401, 0x0001,
  COMMENT         ,       50, CNF_CPXVERSION  , G_BOX             ,   /* Object 49 INFORMATION */
  FLAGS9, NORMAL, (LONG)0x00FF1100L,
  0x0000, 0x0000, 0x0020, 0x0009,
        51,       -1,       -1, G_TEXT            ,   /* Object 50  */
  NONE, NORMAL, (LONG)&rs_tedinfo[34],
  0x0001, 0x0003, 0x041A, 0x0800,
        52,       -1,       -1, G_TEXT            ,   /* Object 51  */
  NONE, NORMAL, (LONG)&rs_tedinfo[35],
  0x0001, 0x0802, 0x041C, 0x0800,
        53,       -1,       -1, G_TEXT            ,   /* Object 52  */
  NONE, NORMAL, (LONG)&rs_tedinfo[36],
  0x0001, 0x0803, 0x0009, 0x0800,
        57,       54, URL_LDG         , G_IBOX            ,   /* Object 53  */
  NONE, DRAW3D, (LONG)0x00FF1140L,
  0x0001, 0x0E04, 0x041D, 0x0003,
        55,       -1,       -1, G_TEXT            ,   /* Object 54  */
  NONE, NORMAL, (LONG)&rs_tedinfo[37],
  0x0403, 0x0800, 0x0015, 0x0800,
  URL_LDG         ,       -1,       -1, G_TEXT            ,   /* Object 55  */
  NONE, NORMAL, (LONG)&rs_tedinfo[38],
  0x0403, 0x0001, 0x0413, 0x0800,
        53,       -1,       -1, G_BOXTEXT         ,   /* Object 56 URL_LDG */
  SELECTABLE|EXIT|FLAGS9|FLAGS10, NORMAL, (LONG)&rs_tedinfo[39],
  0x0405, 0x0002, 0x0010, 0x0800,
        58,       -1,       -1, G_TEXT            ,   /* Object 57  */
  NONE, NORMAL, (LONG)&rs_tedinfo[40],
  0x0001, 0x0001, 0x0009, 0x0800,
        59,       -1,       -1, G_TEXT            ,   /* Object 58  */
  NONE, SELECTED, (LONG)&rs_tedinfo[41],
  0x0000, 0x0000, 0x0020, 0x0001,
        60,       -1,       -1, G_TEXT            ,   /* Object 59  */
  NONE, NORMAL, (LONG)&rs_tedinfo[42],
  0x0001, 0x0002, 0x001E, 0x0800,
  INF_DOC         ,       -1,       -1, G_TEXT            ,   /* Object 60  */
  NONE, NORMAL, (LONG)&rs_tedinfo[43],
  0x0001, 0x0004, 0x0012, 0x0800,
        62,       -1,       -1, G_BOXTEXT         ,   /* Object 61 INF_DOC */
  SELECTABLE|EXIT|FLAGS9, NORMAL, (LONG)&rs_tedinfo[44],
  0x0413, 0x0004, 0x040A, 0x0800,
  CNF_CPXVERSION  ,       -1,       -1, G_TEXT            ,   /* Object 62  */
  NONE, NORMAL, (LONG)&rs_tedinfo[45],
  0x0201, 0x0308, 0x021D, 0x0800,
  INFORMATION     ,       -1,       -1, G_TEXT            ,   /* Object 63 CNF_CPXVERSION */
  NONE, NORMAL, (LONG)&rs_tedinfo[46],
  0x040A, 0x0001, 0x0404, 0x0800,
         0,       65,       73, G_BOX             ,   /* Object 64 COMMENT */
  HIDETREE|FLAGS9, NORMAL, (LONG)0x00FF1100L,
  0x0000, 0x0000, 0x0020, 0x0009,
  LINE1           ,       -1,       -1, G_TEXT            ,   /* Object 65  */
  NONE, SELECTED, (LONG)&rs_tedinfo[47],
  0x0000, 0x0000, 0x0020, 0x0001,
        67,       -1,       -1, G_FTEXT           ,   /* Object 66 LINE1 */
  FLAGS10, DRAW3D, (LONG)&rs_tedinfo[48],
  0x0001, 0x0802, 0x001E, 0x0001,
        68,       -1,       -1, G_FTEXT           ,   /* Object 67  */
  FLAGS10, DRAW3D, (LONG)&rs_tedinfo[49],
  0x0001, 0x0803, 0x001E, 0x0001,
        69,       -1,       -1, G_FTEXT           ,   /* Object 68  */
  FLAGS10, DRAW3D, (LONG)&rs_tedinfo[50],
  0x0001, 0x0804, 0x001E, 0x0001,
        70,       -1,       -1, G_FTEXT           ,   /* Object 69  */
  FLAGS10, DRAW3D, (LONG)&rs_tedinfo[51],
  0x0001, 0x0805, 0x001E, 0x0001,
  LINE6           ,       -1,       -1, G_FTEXT           ,   /* Object 70  */
  FLAGS10, DRAW3D, (LONG)&rs_tedinfo[52],
  0x0001, 0x0806, 0x001E, 0x0001,
  COM_CLIP        ,       -1,       -1, G_FTEXT           ,   /* Object 71 LINE6 */
  FLAGS10, DRAW3D, (LONG)&rs_tedinfo[53],
  0x0001, 0x0807, 0x001E, 0x0001,
        73,       -1,       -1, G_BOXTEXT         ,   /* Object 72 COM_CLIP */
  SELECTABLE|EXIT|FLAGS9|FLAGS10, NORMAL, (LONG)&rs_tedinfo[54],
  0x000A, 0x0801, 0x060C, 0x0800,
  COMMENT         ,       -1,       -1, G_TEXT            ,   /* Object 73  */
  LASTOB, NORMAL, (LONG)&rs_tedinfo[55],
  0x0017, 0x0801, 0x0401, 0x0800,
  
  /******** Tree 1 TAMPON ****************************************************/
        -1,        1,       16, G_BOX             ,   /* Object 0  */
  NONE, OUTLINED, (LONG)0x00021100L,
  0x0000, 0x0000, 0x0431, 0x000F,
        16,        2, CLOSE           , G_BOX             ,   /* Object 1  */
  HIDETREE|FLAGS9, NORMAL, (LONG)0x00FF1100L,
  0x0005, 0x0802, 0x0020, 0x0E06,
  LIBINF          ,        3,        6, G_BOX             ,   /* Object 2  */
  NONE, NORMAL, (LONG)0x00FF1100L,
  0x0001, 0x0800, 0x000E, 0x0004,
         4,       -1,       -1, G_STRING          ,   /* Object 3  */
  NONE, NORMAL, (LONG)" MINITEXT.LDG ",
  0x0000, 0x0000, 0x000E, 0x0001,
         5,       -1,       -1, G_STRING          ,   /* Object 4  */
  NONE, NORMAL, (LONG)" MINITEXT.LDG ",
  0x0000, 0x0001, 0x000E, 0x0001,
         6,       -1,       -1, G_STRING          ,   /* Object 5  */
  NONE, NORMAL, (LONG)" MINITEXT.LDG ",
  0x0000, 0x0002, 0x000E, 0x0001,
         2,       -1,       -1, G_STRING          ,   /* Object 6  */
  NONE, NORMAL, (LONG)" MINITEXT.LDG ",
  0x0000, 0x0003, 0x000E, 0x0001,
  LOCKED          ,       -1,       -1, G_FTEXT           ,   /* Object 7 LIBINF */
  SELECTABLE|EXIT|FLAGS10, DRAW3D, (LONG)&rs_tedinfo[56],
  0x0001, 0x0805, 0x001E, 0x0001,
  SHARED          ,       -1,       -1, G_BUTTON          ,   /* Object 8 LOCKED */
  SELECTABLE|FLAGS9|FLAGS10, NORMAL, (LONG)"Locked",
  0x0012, 0x0800, 0x0406, 0x0001,
  VERSLIB         ,       -1,       -1, G_BUTTON          ,   /* Object 9 SHARED */
  SELECTABLE|FLAGS9|FLAGS10, NORMAL, (LONG)"Shared",
  0x0012, 0x0002, 0x0406, 0x0001,
  UP              ,       -1,       -1, G_FTEXT           ,   /* Object 10 VERSLIB */
  FLAGS9, DRAW3D|WHITEBAK, (LONG)&rs_tedinfo[57],
  0x0012, 0x0803, 0x0609, 0x0001,
  DN              ,       -1,       -1, G_BOXCHAR         ,   /* Object 11 UP */
  SELECTABLE|FLAGS9|FLAGS10, NORMAL, (LONG) ((LONG)'\001' << 24)|0x00FE1100L,
  0x040F, 0x0800, 0x0401, 0x0002,
  VERSLDG         ,       -1,       -1, G_BOXCHAR         ,   /* Object 12 DN */
  SELECTABLE|FLAGS9|FLAGS10, NORMAL, (LONG) ((LONG)'\002' << 24)|0x00FE1100L,
  0x040F, 0x0802, 0x0401, 0x0002,
  INIT            ,       -1,       -1, G_FTEXT           ,   /* Object 13 VERSLDG */
  FLAGS9, DRAW3D|WHITEBAK, (LONG)&rs_tedinfo[58],
  0x0012, 0x0404, 0x0609, 0x0001,
  CLOSE           ,       -1,       -1, G_BUTTON          ,   /* Object 14 INIT */
  SELECTABLE|FLAGS9|FLAGS10, NORMAL, (LONG)"Init",
  0x0019, 0x0800, 0x0406, 0x0001,
         1,       -1,       -1, G_BUTTON          ,   /* Object 15 CLOSE */
  SELECTABLE|FLAGS9|FLAGS10, NORMAL, (LONG)"Close",
  0x0019, 0x0002, 0x0406, 0x0001,
         0,       17,       23, G_BOX             ,   /* Object 16  */
  FLAGS9, NORMAL, (LONG)0x00FF1100L,
  0x0001, 0x0800, 0x0020, 0x0009,
        18,       -1,       -1, G_TEXT            ,   /* Object 17  */
  NONE, SELECTED, (LONG)&rs_tedinfo[59],
  0x0000, 0x0000, 0x0020, 0x0001,
  MGR_VERSION     ,       -1,       -1, G_TEXT            ,   /* Object 18  */
  NONE, NORMAL, (LONG)&rs_tedinfo[60],
  0x0409, 0x0001, 0x060C, 0x0001,
  MGR_NLIB        ,       -1,       -1, G_FTEXT           ,   /* Object 19 MGR_VERSION */
  FLAGS9, DRAW3D|WHITEBAK, (LONG)&rs_tedinfo[61],
  0x0401, 0x0803, 0x040C, 0x0001,
        21,       -1,       -1, G_FTEXT           ,   /* Object 20 MGR_NLIB */
  FLAGS9, DRAW3D, (LONG)&rs_tedinfo[62],
  0x0401, 0x0805, 0x0010, 0x0001,
  MGR_ACTION      ,       -1,       -1, G_STRING          ,   /* Object 21  */
  NONE, NORMAL, (LONG)"Send request:",
  0x0002, 0x0807, 0x000D, 0x0001,
        23,       -1,       -1, G_BOXTEXT         ,   /* Object 22 MGR_ACTION */
  SELECTABLE|TOUCHEXIT, SHADOWED, (LONG)&rs_tedinfo[63],
  0x040F, 0x0807, 0x0008, 0x0001,
        16,       -1,       -1, G_TEXT            ,   /* Object 23  */
  LASTOB, NORMAL, (LONG)&rs_tedinfo[64],
  0x000E, 0x0007, 0x0401, 0x0800
};

OBJECT *rs_trindex[] =
{ &rs_object[0],   /* Tree  0 CPXFORM          */
  &rs_object[74]    /* Tree  1 TAMPON           */
};
