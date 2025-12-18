/*assembler.c - output an ObjAsm/AsAsm or GAS assembler file given a module defn*/
#ifndef assembler_H
#define assembler_H

#include <stdio.h>

#ifndef os_H
   #include "oslib/os.h"
#endif

/* from support */
#ifndef lookup_H
   #include "lookup.h"
#endif

typedef enum
{
  AsmType_eUnknown,
  AsmType_eObjAsm,	/* ObjAsm.  */
  AsmType_eAsAsmAOF,	/* AsAsm and AOF output.  */
  AsmType_eAsAsmELF,	/* AsAsm and ELF output.  */
  AsmType_eGCCAOF,	/* Assembler used in GCCSDK 3.4 (AOF) release, called
    'as' (predecessor of AsAsm).  */
  AsmType_eARMASM,	/* ARM's assembler.  */
  AsmType_eGCCELF	/* Assembler used in GCCSDK 4 (ELF) release, called
    'gas' (GNU binutils assembler).  */
} AsmType_e;

typedef enum
{
  FloatABI_eSoft,	/* Floating point values are passed in integer
    registers and/or the stack, no FP instructions are used.  */
  FloatABI_eHard	/* Floating point values are passed in FP registers
    and/or the stack and FP instructions are used.  */
} FloatABI_e;

extern AsmType_e AsmType;
extern FloatABI_e FloatABI;
  
#define GASSyntax (AsmType == AsmType_eGCCELF)

/*if |separate|, output is to the directory |output|; otherwise, to the file |file|.*/
extern os_error *assembler_output (FILE *file, const char *title, const char *author, lookup_t types, lookup_t swis, osbool separate, osbool apcs32, char *output);

#endif
