#include <kernel.h>

#include "oslib/messagetrans.h"
#include "oslib/os.h"

static os_error Bad_Swi = {error_NO_SUCH_SWI, "BadSwi"};
extern char Module_Title [];

_kernel_oserror *main_swi (int swi, _kernel_swi_regs *regs, void *workspace)

{  os_error *error = NULL;

   switch (swi)
   {  case 0:
      break;

      case 1:
      break;

      /*... other cases as required ...*/

      default:
         error = xmessagetrans_error_lookup (&Bad_Swi, NULL, NULL, 0,
               Module_Title, SKIP, SKIP, SKIP);
         goto finish;
      break;
   }

finish:
   return (_kernel_oserror *) error;
}
