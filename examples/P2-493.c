#include <kernel.h>

#include "oslib/filer.h"
#include "oslib/wimp.h"

extern wimp_t Task_Handle;

   /*This is a CMHG-style service entry point.*/

void main_service (int service, _kernel_swi_regs *regs, void *workspace)

{  switch (service)
   {  case Service_FilerDying:
         if (!(Task_Handle == (wimp_t) 0 || Task_Handle == (wimp_t) -1))
            (void) xwimp_close_down (Task_Handle);

         Task_Handle = 0;
      break;
}  }
