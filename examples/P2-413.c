#include <kernel.h>

#include "oslib/resourcefs.h"

extern resourcefs_file_list Files;

   /*This is a CMHG-style service entry point.*/

void main_service (int service, _kernel_swi_regs *regs, void *workspace)

{  switch (service)
   {  case Service_ResourceFSStarting:
         (*(void (*) (resourcefs_file_list *, int, int, int)) regs->r [2])
               (&Files, SKIP, SKIP, regs->r [3]);
      break;
   }
}
