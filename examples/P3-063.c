#include <kernel.h>

#include "oslib/os.h"
#include "oslib/osmodule.h"
#include "oslib/wimp.h"

#define ENTRY_COMMAND 42

static char Title [] = "MyModule";

static wimp_t Task_Handle;

/*This is a CMHG-style command entry point.*/

_kernel_oserror *main_command (char *tail, int argc, int cmd_no,
      void *workspace)

{  os_error *error = NULL;

   switch (cmd_no)
   {  case ENTRY_COMMAND:
         if ((error = xosmodule_enter (Title, tail)) != NULL)
            goto finish;
      break;

      /*other commands*/
   }

finish:
   return (_kernel_oserror *) error;
}

/*This is the module's language entry point.*/

int main (void)

{  os_error *error = NULL;

   if (Task_Handle != 0)
   {  wimp_t t = Task_Handle;

      Task_Handle = 0;
      if ((error = xwimp_close_down (t)) != NULL)
         goto finish;
   }

   /*Now claim any workspace etc required before initialising the WIMP*/

   if ((error = xwimp_initialise (wimp_VERSION_RO3, Title, NULL, NULL,
         &Task_Handle)) != NULL)
      goto finish;

   /*Rest of the application.*/

finish:
   return 0;
}
