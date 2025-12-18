#include <kernel.h>

#include "oslib/os.h"
#include "oslib/wimp.h"

extern wimp_t Task_Handle;

extern char My_Command [];

/*This is a CMHG-style termination entry point.*/

_kernel_oserror *main_terminate (osbool fatal, int instance, void *workspace)

{  os_error *error = NULL, *error1;

   if (Task_Handle != 0)
   {  error1 = xwimp_close_down (Task_Handle);
      if (error == NULL) error = error1;
   }

   return (_kernel_oserror *) error;
}
