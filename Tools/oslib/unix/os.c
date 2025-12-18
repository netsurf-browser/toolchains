/*
 * OS Lib very limited port
 */

#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oslib/os.h"

#include "support/kernel.h"
#include "support/riscos.h"
#include "support/trace.h"
#include "support/x.h"

// size of error ring buffer
#define MAX_OSERR 8

/* --- types ------------------------------------------------------------- */

typedef struct
{
    char    *name;
    int        value;
} s_table;

/* --- gloabl data ------------------------------------------------------- */

/* --- static data ------------------------------------------------------- */
static s_table swi_names[] =
{
    {"OS_WriteC",                               0x0},
    {"OS_WriteS",                               0x1},
    {"OS_Write0",                               0x2},
    {"OS_NewLine",                              0x3},
    {"OS_ReadC",                                0x4},
    {"OS_CLI",                                  0x5},
    {"OS_Byte",                                 0x6},
    {"OS_Word",                                 0x7},
    {"OS_File",                                 0x8},
    {"OS_Args",                                 0x9},
    {"OS_BGet",                                 0xA},
    {"OS_BPut",                                 0xB},
    {"OS_GBPB",                                 0xC},
    {"OS_Find",                                 0xD},
    {"OS_ReadLine",                             0xE},
    {"OS_Control",                              0xF},
    {"OS_GetEnv",                               0x10},
    {"OS_Exit",                                 0x11},
    {"OS_SetEnv",                               0x12},
    {"OS_IntOn",                                0x13},
    {"OS_IntOff",                               0x14},
    {"OS_CallBack",                             0x15},
    {"OS_EnterOS",                              0x16},
    {"OS_BreakPt",                              0x17},
    {"OS_BreakCtrl",                            0x18},
    {"OS_UnusedSWI",                            0x19},
    {"OS_UpdateMEMC",                           0x1A},
    {"OS_SetCallBack",                          0x1B},
    {"OS_Mouse",                                0x1C},
    {"OS_Heap",                                 0x1D},
    {"OS_Module",                               0x1E},
    {"OS_Claim",                                0x1F},
    {"OS_Release",                              0x20},
    {"OS_ReadUnsigned",                         0x21},
    {"OS_GenerateEvent",                        0x22},
    {"OS_ReadVarVal",                           0x23},
    {"OS_SetVarVal",                            0x24},
    {"OS_GSInit",                               0x25},
    {"OS_GSRead",                               0x26},
    {"OS_GSTrans",                              0x27},
    {"OS_BinaryToDecimal",                      0x28},
    {"OS_FSControl",                            0x29},
    {"OS_ChangeDynamicArea",                    0x2A},
    {"OS_GenerateError",                        0x2B},
    {"OS_ReadEscapeState",                      0x2C},
    {"OS_EvaluateExpression",                   0x2D},
    {"OS_SpriteOp",                             0x2E},
    {"OS_ReadPalette",                          0x2F},
    {"OS_ServiceCall",                          0x30},
    {"OS_ReadVduVariables",                     0x31},
    {"OS_ReadPoint",                            0x32},
    {"OS_UpCall",                               0x33},
    {"OS_CallAVector",                          0x34},
    {"OS_ReadModeVariable",                     0x35},
    {"OS_RemoveCursors",                        0x36},
    {"OS_RestoreCursors",                       0x37},
    {"OS_SWINumberToString",                    0x38},
    {"OS_SWINumberFromString",                  0x39},
    {"OS_ValidateAddress",                      0x3A},
    {"OS_CallAfter",                            0x3B},
    {"OS_CallEvery",                            0x3C},
    {"OS_RemoveTickerEvent",                    0x3D},
    {"OS_InstallKeyHandler",                    0x3E},
    {"OS_CheckModeValid",                       0x3F},
    {"OS_ChangeEnvironment",                    0x40},
    {"OS_ClaimScreenMemory",                    0x41},
    {"OS_ReadMonotonicTime",                    0x42},
    {"OS_SubstituteArgs",                       0x43},
    {"OS_PrettyPrint",                          0x44},
    {"OS_Plot",                                 0x45},
    {"OS_WriteN",                               0x46},
    {"OS_AddToVector",                          0x47},
    {"OS_WriteEnv",                             0x48},
    {"OS_ReadArgs",                             0x49},
    {"OS_ReadRAMFsLimits",                      0x4A},
    {"OS_ClaimDeviceVector",                    0x4B},
    {"OS_ReleaseDeviceVector",                  0x4C},
    {"OS_DelinkApplication",                    0x4D},
    {"OS_RelinkApplication",                    0x4E},
    {"OS_HeapSort",                             0x4F},
    {"OS_ExitAndDie",                           0x50},
    {"OS_ReadMemMapInfo",                       0x51},
    {"OS_ReadMemMapEntries",                    0x52},
    {"OS_SetMemMapEntries",                     0x53},
    {"OS_AddCallBack",                          0x54},
    {"OS_ReadDefaultHandler",                   0x55},
    {"OS_SetECFOrigin",                         0x56},
    {"OS_SerialOp",                             0x57},
    {"OS_ReadSysInfo",                          0x58},
    {"OS_Confirm",                              0x59},
    {"OS_ChangedBox",                           0x5A},
    {"OS_CRC",                                  0x5B},
    {"OS_ReadDynamicArea",                      0x5C},
    {"OS_PrintChar",                            0x5D},
    {"OS_ChangeRedirection",                    0x5E},
    {"OS_RemoveCallBack",                       0x5F},
    {"OS_FindMemMapEntries",                    0x60},
    {"OS_SetColour",                            0x61},
    {"OS_Pointer",                              0x64},
    {"OS_ScreenMode",                           0x65},
    {"OS_DynamicArea",                          0x66},
    {"OS_Memory",                               0x68},
    {"OS_ClaimProcessorVector",                 0x69},
    {"OS_Reset",                                0x6A},
    {"OS_MMUControl",                           0x6B},
    {"OS_PlatformFeatures",                     0x6D},
    {"OS_SynchroniseCodeAreas",                 0x6E},
    {"OS_CallASWI",                             0x6F},
    {"OS_AMBControl",                           0x70},
    {"OS_CallASWIR12",                          0x71},
    {"OS_EnterUSR32",                           0x73},
    {"OS_EnterUSR26",                           0x74},
    {"OS_Hardware",                             0x7A},
    {"OS_LeaveOS",                              0x7C},
    {"OS_ReadLine32",                           0x7D},
    {"OS_SubstituteArgs32",                     0x7E},
    {"OS_HeapSort32",                           0x7F},
    {"OS_ConvertStandardDateAndTime",           0xC0},
    {"OS_ConvertDateAndTime",                   0xC1},
    {"OS_ConvertHex1",                          0xD0},
    {"OS_ConvertHex2",                          0xD1},
    {"OS_ConvertHex4",                          0xD2},
    {"OS_ConvertHex6",                          0xD3},
    {"OS_ConvertHex8",                          0xD4},
    {"OS_ConvertCardinal1",                     0xD5},
    {"OS_ConvertCardinal2",                     0xD6},
    {"OS_ConvertCardinal3",                     0xD7},
    {"OS_ConvertCardinal4",                     0xD8},
    {"OS_ConvertInteger1",                      0xD9},
    {"OS_ConvertInteger2",                      0xDA},
    {"OS_ConvertInteger3",                      0xDB},
    {"OS_ConvertInteger4",                      0xDC},
    {"OS_ConvertBinary1",                       0xDD},
    {"OS_ConvertBinary2",                       0xDE},
    {"OS_ConvertBinary3",                       0xDF},
    {"OS_ConvertBinary4",                       0xE0},
    {"OS_ConvertSpacedCardinal1",               0xE1},
    {"OS_ConvertSpacedCardinal2",               0xE2},
    {"OS_ConvertSpacedCardinal3",               0xE3},
    {"OS_ConvertSpacedCardinal4",               0xE4},
    {"OS_ConvertSpacedInteger1",                0xE5},
    {"OS_ConvertSpacedInteger2",                0xE6},
    {"OS_ConvertSpacedInteger3",                0xE7},
    {"OS_ConvertSpacedInteger4",                0xE8},
    {"OS_ConvertFixedNetStation",               0xE9},
    {"OS_ConvertNetStation",                    0xEA},
    {"OS_ConvertFixedFileSize",                 0xEB},
    {"OS_ConvertFileSize",                      0xEC},
    {"OS_WriteI",                               0x100},
    {"Font_CacheAddr",                          0x40080},
    {"Font_FindFont",                           0x40081},
    {"Font_LoseFont",                           0x40082},
    {"Font_ReadDefn",                           0x40083},
    {"Font_ReadIdentifier",                     0x40083},
    {"Font_ReadInfo",                           0x40084},
    {"Font_StringWidth",                        0x40085},
    {"Font_Paint",                              0x40086},
    {"Font_Caret",                              0x40087},
    {"Font_ConverttoOS",                        0x40088},
    {"Font_Converttopoints",                    0x40089},
    {"Font_SetFont",                            0x4008A},
    {"Font_CurrentFont",                        0x4008B},
    {"Font_FutureFont",                         0x4008C},
    {"Font_FindCaret",                          0x4008D},
    {"Font_CharBBox",                           0x4008E},
    {"Font_ReadScaleFactor",                    0x4008F},
    {"Font_SetScaleFactor",                     0x40090},
    {"Font_ListFonts",                          0x40091},
    {"Font_SetFontColours",                     0x40092},
    {"Font_SetPalette",                         0x40093},
    {"Font_SetTruePalette",                     0x40093},
    {"Font_ReadThresholds",                     0x40094},
    {"Font_SetThresholds",                      0x40095},
    {"Font_FindCaretJ",                         0x40096},
    {"Font_StringBBox",                         0x40097},
    {"Font_ReadColourTable",                    0x40098},
    {"Font_MakeBitmap",                         0x40099},
    {"Font_UnCacheFile",                        0x4009A},
    {"Font_SetFontMax",                         0x4009B},
    {"Font_ReadFontMax",                        0x4009C},
    {"Font_ReadFontPrefix",                     0x4009D},
    {"Font_SwitchOutputToBuffer",               0x4009E},
    {"Font_SwitchOutputToBufferFlags",          0x4009E},
    {"Font_ReadFontMetrics",                    0x4009F},
    {"Font_DecodeMenu",                         0x400A0},
    {"Font_ScanString",                         0x400A1},
    {"Font_SetColourTable",                     0x400A2},
    {"Font_CurrentRGB",                         0x400A3},
    {"Font_FutureRGB",                          0x400A4},
    {"Font_ReadEncodingFilename",               0x400A5},
    {"Font_FindField",                          0x400A6},
    {"Font_ApplyFields",                        0x400A7},
    {"Font_LookupFont",                         0x400A8},
    {"Wimp_Initialise",                         0x400C0},
    {"Wimp_CreateWindow",                       0x400C1},
    {"Wimp_CreateIcon",                         0x400C2},
    {"Wimp_CreateIconRelative",                 0x400C2},
    {"Wimp_CreateIconPrioritised",              0x400C2},
    {"Wimp_DeleteWindow",                       0x400C3},
    {"Wimp_DeleteIcon",                         0x400C4},
    {"Wimp_OpenWindow",                         0x400C5},
    {"Wimp_OpenWindowNested",                   0x400C5},
    {"Wimp_OpenWindowNestedWithFlags",          0x400C5},
    {"Wimp_CloseWindow",                        0x400C6},
    {"Wimp_Poll",                               0x400C7},
    {"Wimp_RedrawWindow",                       0x400C8},
    {"Wimp_UpdateWindow",                       0x400C9},
    {"Wimp_GetRectangle",                       0x400CA},
    {"Wimp_GetWindowState",                     0x400CB},
    {"Wimp_GetWindowStateAndNesting",           0x400CB},
    {"Wimp_GetWindowInfo",                      0x400CC},
    {"Wimp_GetWindowInfoHeaderOnly",            0x400CC},
    {"Wimp_SetIconState",                       0x400CD},
    {"Wimp_GetIconState",                       0x400CE},
    {"Wimp_GetPointerInfo",                     0x400CF},
    {"Wimp_DragBox",                            0x400D0},
    {"Wimp_DragBoxWithFlags",                   0x400D0},
    {"Wimp_ForceRedraw",                        0x400D1},
    {"Wimp_ForceRedrawFurniture",               0x400D1},
    {"Wimp_ForceRedrawTitle",                   0x400D1},
    {"Wimp_SetCaretPosition",                   0x400D2},
    {"Wimp_GetCaretPosition",                   0x400D3},
    {"Wimp_CreateMenu",                         0x400D4},
    {"Wimp_DecodeMenu",                         0x400D5},
    {"Wimp_WhichIcon",                          0x400D6},
    {"Wimp_SetExtent",                          0x400D7},
    {"Wimp_SetPointerShape",                    0x400D8},
    {"Wimp_OpenTemplate",                       0x400D9},
    {"Wimp_CloseTemplate",                      0x400DA},
    {"Wimp_LoadTemplate",                       0x400DB},
    {"Wimp_ProcessKey",                         0x400DC},
    {"Wimp_CloseDown",                          0x400DD},
    {"Wimp_StartTask",                          0x400DE},
    {"Wimp_ReportError",                        0x400DF},
    {"Wimp_ReportErrorByCategory",              0x400DF},
    {"Wimp_GetWindowOutline",                   0x400E0},
    {"Wimp_PollIdle",                           0x400E1},
    {"Wimp_PlotIcon",                           0x400E2},
    {"Wimp_SetMode",                            0x400E3},
    {"Wimp_SetPalette",                         0x400E4},
    {"Wimp_ReadPalette",                        0x400E5},
    {"Wimp_ReadTruePalette",                    0x400E5},
    {"Wimp_SetColour",                          0x400E6},
    {"Wimp_SendMessage",                        0x400E7},
    {"Wimp_SendMessageToWindow",                0x400E7},
    {"Wimp_CreateSubMenu",                      0x400E8},
    {"Wimp_BaseOfSprites",                      0x400EA},
    {"Wimp_BlockCopy",                          0x400EB},
    {"Wimp_SlotSize",                           0x400EC},
    {"Wimp_ReadPixTrans",                       0x400ED},
    {"Wimp_ClaimFreeMemory",                    0x400EE},
    {"Wimp_CommandWindow",                      0x400EF},
    {"Wimp_TextColour",                         0x400F0},
    {"Wimp_TransferBlock",                      0x400F1},
    {"Wimp_SetFontColours",                     0x400F3},
    {"Wimp_GetMenuState",                       0x400F4},
    {"Wimp_RegisterFilter",                     0x400F5},
    {"Wimp_AddMessages",                        0x400F6},
    {"Wimp_RemoveMessages",                     0x400F7},
    {"Wimp_SetColourMapping",                   0x400F8},
    {"Wimp_TextOp",                             0x400F9},
    {"Wimp_SetWatchdogState",                   0x400FA},
    {"Wimp_ResizeIcon",                         0x400FC},
    {"Wimp_AutoScroll",                         0x400FD},
    {"Sound_Configure",                         0x40140},
    {"Sound_Enable",                            0x40141},
    {"Sound_Stereo",                            0x40142},
    {"Sound_Speaker",                           0x40143},
    {"Sound_Mode",                              0x40144},
    {"Sound_LinearHandler",                     0x40145},
    {"Sound_SampleRate",                        0x40146},
    {"Sound_Volume",                            0x40180},
    {"Sound_SoundLog",                          0x40181},
    {"Sound_LogScale",                          0x40182},
    {"Sound_InstallVoice",                      0x40183},
    {"Sound_RemoveVoice",                       0x40184},
    {"Sound_AttachVoice",                       0x40185},
    {"Sound_ControlPacked",                     0x40186},
    {"Sound_Tuning",                            0x40187},
    {"Sound_Pitch",                             0x40188},
    {"Sound_Control",                           0x40189},
    {"Sound_AttachNamedVoice",                  0x4018A},
    {"Sound_ReadControlBlock",                  0x4018B},
    {"Sound_WriteControlBlock",                 0x4018C},
    {"Sound_QInit",                             0x401C0},
    {"Sound_QSchedule",                         0x401C1},
    {"Sound_QRemove",                           0x401C2},
    {"Sound_QFree",                             0x401C3},
    {"Sound_QSDispatch",                        0x401C4},
    {"Sound_QTempo",                            0x401C5},
    {"Sound_QBeat",                             0x401C6},
    {"Sound_QInterface",                        0x401C7},
    {"Socket_Creat",                            0x41200},
    {"Socket_Bind",                             0x41201},
    {"Socket_Listen",                           0x41202},
    {"Socket_Accept",                           0x41203},
    {"Socket_Connect",                          0x41204},
    {"Socket_Recv",                             0x41205},
    {"Socket_Recvfrom",                         0x41206},
    {"Socket_Recvmsg",                          0x41207},
    {"Socket_Send",                             0x41208},
    {"Socket_Sendto",                           0x41209},
    {"Socket_Sendmsg",                          0x4120A},
    {"Socket_Shutdown",                         0x4120B},
    {"Socket_Setsockopt",                       0x4120C},
    {"Socket_Getsockopt",                       0x4120D},
    {"Socket_Getpeername",                      0x4120E},
    {"Socket_Getsockname",                      0x4120F},
    {"Socket_Close",                            0x41210},
    {"Socket_Select",                           0x41211},
    {"Socket_Ioctl",                            0x41212},
    {"Socket_Read",                             0x41213},
    {"Socket_Write",                            0x41214},
    {"Socket_Stat",                             0x41215},
    {"Socket_Readv",                            0x41216},
    {"Socket_Writev",                           0x41217},
    {"Socket_Gettsize",                         0x41218},
    {"Socket_Sendtosm",                         0x41219},
    {"Socket_Sysctl",                           0x4121A},
    {"Socket_Accept_1",                         0x4121B},
    {"Socket_Recvfrom_1",                       0x4121C},
    {"Socket_Recvmsg_1",                        0x4121D},
    {"Socket_Sendmsg_1",                        0x4121E},
    {"Socket_Getpeername_1",                    0x4121F},
    {"Socket_Getsockname_1",                    0x41220},
    {"Socket_InternalLookup",                   0x41221},
    {"Socket_Version",                          0x41222},
    {"MessageTrans_FileInfo",                   0x41500},
    {"MessageTrans_OpenFile",                   0x41501},
    {"MessageTrans_Lookup",                     0x41502},
    {"MessageTrans_MakeMenus",                  0x41503},
    {"MessageTrans_CloseFile",                  0x41504},
    {"MessageTrans_EnumerateTokens",            0x41505},
    {"MessageTrans_ErrorLookup",                0x41506},
    {"MessageTrans_GSLookup",                   0x41507},
    {"MessageTrans_CopyError",                  0x41508},
    {"MessageTrans_Dictionary",                 0x41509},
    {"ResourceFS_RegisterFiles",                0x41B40},
    {"ResourceFS_DeregisterFiles",              0x41B41},
    {"Filter_RegisterPreFilter",                0x42640},
    {"Filter_RegisterPostFilter",               0x42641},
    {"Filter_DeRegisterPreFilter",              0x42642},
    {"Filter_DeRegisterPostFilter",             0x42643},
    {"Filter_RegisterRectFilter",               0x42644},
    {"Filter_DeRegisterRectFilter",             0x42645},
    {"Filter_RegisterCopyFilter",               0x42646},
    {"Filter_DeRegisterCopyFilter",             0x42647},
    {"Filter_RegisterPostRectFilter",           0x42648},
    {"Filter_DeRegisterPostRectFilter",         0x42649},
    {"Filter_RegisterPostIconFilter",           0x4264A},
    {"Filter_DeRegisterPostIconFilter",         0x4264B},
    {"SharedCLibrary_LibInitAPCS_A",            0x80680},
    {"SharedCLibrary_LibInitAPCS_R",            0x80681},
    {"SharedCLibrary_LibInitModule",            0x80682},
    {"SharedCLibrary_LibInitAPCS_32",           0x80683},
    {"SharedCLibrary_LibInitModule32",          0x80684},
    {NULL,                                      0}
};

/* ------------------------------------------------------------------------ */
/* we maintain a ring buffer of error blocks, so that error pointers
** can remain valid despite further errors occurring
*/
static os_error *_os_lasterr;
static os_error _os_err[MAX_OSERR];
extern _kernel_oserror* _kernel_last_error;

/* copy the current error into the next entry in the error ring buffer
**	and return a pointer to the last entry or NULL if none
** do nothing if a null error poinetr is passed
**
** this allows us to locally generate an error block, and then copy it
**	into static memory for use in further processing.
*/
extern os_error *os_set_lasterr( const os_error *err )
{
	// skip the process if no error
	if (err)
	{
		// set the pointer
		if ( !_os_lasterr || ++_os_lasterr > _os_err + sizeof( _os_err ) )
			_os_lasterr = _os_err;

		// copy the error block
		memcpy( _os_lasterr, err, sizeof( os_error ));

		// keep the kernel's idea of things up to date
		_kernel_last_error = (_kernel_oserror*)_os_lasterr;

	}
	return err ? _os_lasterr : 0;
}

/* ------------------------------------------------------------------------ */
/*  Return the last OS error, or NULL if none
*/
extern const os_error *os_lasterr()
{
	return _os_lasterr;
}

/* ------------------------------------------------------------------------
 * Function:      os_generate_error()
 *
 * Description:   Generates an error and invokes the error handler
 *
 * Input:         error - value of R0 on entry
 *
 * Other notes:   Calls SWI 0x2B.
 */
void os_generate_error(os_error const *e)
{
	if(e)
	{
		tracef( "os_generate_error: 0x%x; \"%s\"\n" \
			_ e -> errnum \
			_ e -> errmess \
			);
		OS_ERROR(e);

		if ( raise(SIGOSERROR) )
		{
			tracef( "os_generate_error: failed to raise signal\n" );
		}
	}
}


/* ------------------------------------------------------------------------
 * Function:      os_cli()
 *
 * Description:   Processes a supervisor command
 *
 * Input:         command - value of R0 on entry
 *
 * Other notes:   Calls SWI 0x5.
 */

extern os_error *xos_cli (char const *command)
{
    NOT_USED(command);
    return NULL;
}

void os_cli (char const *command)
{
    NOT_USED(command);
}

/* ------------------------------------------------------------------------
 * Function:      os_swi_number_to_string()
 *
 * Description:   Converts a SWI number to a string containing its name
 *
 * Input:         swi - value of R0 on entry
 *                buffer - value of R1 on entry
 *                size - value of R2 on entry
 *
 * Output:        used - value of R2 on exit (X version only)
 *
 * Returns:       R2 (non-X version only)
 *
 * Other notes:   Calls SWI 0x38.
 */

os_error *xos_swi_number_to_string (int swi,
      char *buffer,
      int size,
      int *used_)
{
    int index   = -1;
    int nox_swi = swi & ~(1<<17);

		int used = 0;

		if( buffer )
		{
			do
			{
					index++;
			} while(swi_names[index].value<nox_swi && swi_names[index].name!=NULL);

			if(swi_names[index].value==nox_swi)
					used = size - sprintf(buffer, swi & (1<<17) ?
						"X%s" : "%s", swi_names[index].name);
			else
					used = size - sprintf(buffer, swi & (1<<17) ?
						"XUser" : "User");
		}

	  if (used_) *used_=used;
    return NULL;
}

int os_swi_number_to_string (int swi,
      char *buffer,
      int size)
{
    int used;
    xos_swi_number_to_string (swi, buffer, size, &used);
    return used;
}

/* ------------------------------------------------------------------------
 * Function:      os_swi_number_from_string()
 *
 * Description:   Converts a string to a SWI number if valid
 *
 * Input:         swi_name - value of R1 on entry
 *
 * Output:        swi - value of R0 on exit (X version only)
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x39.
 */

extern os_error *xos_swi_number_from_string (char const *swi_name, int *swi)
{
    int         index     = -1;
    osbool      x         = swi_name[0]=='X';
    const char *nonx_name = swi_name + (x ? 1:0);
    os_error *err = 0;
    os_error oserr;

    do
    {
        index++;
    } while(swi_names[index].name!=NULL && strcmp(nonx_name, swi_names[index].name)!=0);

    if(swi_names[index].name==NULL)
    {
        if (swi) *swi = 0xFFFFFFFF;
        oserr.errnum = 0x1E6;
        strcpy(oserr.errmess, "Unknown SWI");
        err = &oserr;
    }
    else
    {
        if (swi) *swi = swi_names[index].value + (x ? (1<<17) : 0);
    }
    return OS_ERROR(err);
}

extern int os_swi_number_from_string (char const *swi_name)
{
    int swi;
    os_generate_error( xos_swi_number_from_string (swi_name, &swi) );
    return swi;
}


/* ------------------------------------------------------------------------
 * Function:      os_writec()
 *
 * Description:   Writes a character to all of the active output streams
 *
 * Input:         c - value of R0 on entry
 *
 * Other notes:   Calls SWI 0x0.
 */

extern os_error *xos_writec (char c)
{
	if( putchar( c ) == c )
		return 0;
	else
		return OS_ERROR(riscos_errno());
}

void os_writec (char c)
{
	os_generate_error( xos_writec( c ) );
}

/* ------------------------------------------------------------------------
 * Function:      os_writen()
 *
 * Description:   Writes a counted string to all of the active output
 *                streams
 *
 * Input:         s - value of R0 on entry
 *                size - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x46.
 */

extern os_error *xos_writen (char const *s, int size)
{
	char temp[ size + 1 ];
	os_error *err = 0;

	strncpy( temp, s, size );
	*(temp + size) = '\0';

	if( fprintf( stderr, "%s", temp )  < 0 )
		err = riscos_errno();

	return OS_ERROR(err);
}

extern void os_writen (char const *s, int size)
{
	os_generate_error( xos_writen( s, size ) );
}

/* ------------------------------------------------------------------------
 * Function:      os_pretty_print()
 *
 * Description:   Writes a string with some formatting to all of the active
 *                output streams
 *
 * Input:         string - value of R0 on entry
 *                dictionary - value of R1 on entry
 *                special - value of R2 on entry
 *
 * Other notes:   Calls SWI 0x44.
 */

os_error *xos_pretty_print(char const *string, byte const *arg1, char const *arg2)
{
  for (;;)
  {
    char *s = strchr(string, '\r');
    if (!s)
    {
      puts(string);
      return NULL;
    }
    fwrite(string, s - string, 1, stdout);
    putc('\n', stdout);
    string = s + 1;
  }
}

void os_pretty_print(char const *string, byte const *arg1, char const *arg2)
{
	os_generate_error( xos_pretty_print( string, arg1, arg2 ) );
}

/* ------------------------------------------------------------------------
 * Function:      os_write0()
 *
 * Description:   Writes a string to all of the active output streams
 *
 * Input:         s - value of R0 on entry
 *
 * Other notes:   Calls SWI 0x2.
 */

extern os_error *xos_write0 (char const *s)
{
	os_error *err = 0;

	if( s )
	{
		int len = riscos_strlen( s );
		char* temp = malloc( len + 1 );
		riscos_strcpy( temp, s );
		if( fputs( temp, stdout ) < 0 )
			err = riscos_errno();
		free ( temp );
	}
	return OS_ERROR(err);
}

void os_write0 (char const *s)
{
	os_generate_error( xos_write0( s ) );
}

/* ------------------------------------------------------------------------
 * Function:      os_new_line()
 *
 * Description:   Writes a line feed followed by a carriage return to all
 *                of the active output streams
 *
 * Other notes:   Calls SWI 0x3.
 */

extern os_error *xos_new_line (void)
{
	if( fputs( "\n\r", stdout ) > 0 )
		return 0;
	else
		return OS_ERROR(riscos_errno());
}

void os_new_line (void)
{
	os_generate_error(xos_new_line());
}

/* ------------------------------------------------------------------------
 * Function:      os_readc()
 *
 * Description:   Reads a character from the input stream
 *
 * Output:        c - value of R0 on exit
 *                psr - processor status register on exit (X version only)
 *
 * Returns:       psr (non-X version only)
 *
 * Other notes:   Calls SWI 0x4.
 */

extern os_error *xos_readc (char *c, bits *psr)
{
	int chr;
	chr = getchar( );

	if( chr > 0 )
	{
		*psr = 0;
		*c = (char)chr;
		return 0;
	}
	else
	{
		*psr = _V;
		return OS_ERROR(riscos_errno());
	}
}

extern bits os_readc (char *c)
{
	bits psr;
	xos_readc( c, &psr );
	return psr;
}

