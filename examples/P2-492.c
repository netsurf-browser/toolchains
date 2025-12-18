#include <kernel.h>

#include "oslib/filer.h"
#include "oslib/wimp.h"

extern wimp_t Task_Handle;

   /*This is a CMHG-style service entry point.*/

void main_service (int service, _kernel_swi_regs *regs, void *workspace)

{  switch (service)
   {  case Service_StartedFiler:
         if (Task_Handle == (wimp_t) -1)
            Task_Handle = (wimp_t) 0;
      break;

      case Service_Reset:
         Task_Handle = (wimp_t) 0;
      break;
}  }
