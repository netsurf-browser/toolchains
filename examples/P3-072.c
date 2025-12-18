#include <kernel.h>

#include "oslib/os.h"
#include "oslib/wimp.h"

extern wimp_t Task_Handle;

extern char My_Command [];

/*This is a CMHG-style service entry point.*/

void main_service (int service, _kernel_swi_regs *regs, void *workspace)

{  switch (service)
   {  case Service_Reset:
         Task_Handle = 0;
      break;

      case Service_StartedWimp:
         if (Task_Handle == (wimp_t) -1)
            Task_Handle = 0;
      break;
   }
}
