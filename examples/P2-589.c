#include <kernel.h>

#include "oslib/filecore.h"
#include "oslib/osfscontrol.h"

extern char *Filing_System_Name;

os_error *select (void)

{  return xosfscontrol_select_fs ((osfscontrol_id) Filing_System_Name);
}

#define Module_DiscOp 0x12345 /*My disc op SWI number*/

extern byte *Instance;

   /*This is a CMHG-style SWI entry point.*/

_kernel_oserror *main_swi (int swi, _kernel_swi_regs *regs, void *workspace)

{  os_error *error = NULL;

   switch (swi)
   {  case Module_DiscOp & 0x3F:
         regs->r [8] = (int) Instance;
         if ((error = (os_error *) _kernel_swi (FileCore_DiscOp,
               regs, regs)) != NULL)
            goto finish;
      break;

      /*other SWI code*/
   }

finish:
   return (_kernel_oserror *) error;
}
