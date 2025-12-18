#include "oslib/osfscontrol.h"

os_error *ex (char *command_tail_ptr, byte *execution_address,
      char *command_name_ptr)

{  os_error *error = NULL;
   os_error *(*fn) (void) =
         (os_error *(*) (void)) ((bits) execution_address & ~0xFC000003);
         /*address with no flags, user mode*/

   if ((error = xosfscontrol_start_application (command_tail_ptr,
         (byte *) fn, command_name_ptr)) != NULL)
      goto finish;

   /*assume not in SVC mode (mode-switching harder in C)*/

   /*Enter application: assume CAO is function address*/
   error = (*fn) ();

   os_exit (error, 0); /*in case it returns*/

finish:
   return error;
}
