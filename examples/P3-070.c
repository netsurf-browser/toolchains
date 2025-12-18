#include <kernel.h>

#include "oslib/os.h"
#include "oslib/wimp.h"

extern wimp_t Task_Handle;

extern char My_Command [];

/*This is a CMHG-style service entry point.*/

void main_service (int service, _kernel_swi_regs *regs, void *workspace)

{  switch (service)
   {  case Service_StartWimp:
         if (Task_Handle == 0)
            Task_Handle = (wimp_t) -1;

         regs->r [0] = (int) My_Command;
         regs->r [1] = FALSE;
      break;
}  }
