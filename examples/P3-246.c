/*This is a version using OSLib rather than RISC_OSLib. It emits the '/'
   ("run") (referred to in the text), checks that the environment variable
   exists before it dereferences it, and checks for the string being empty.*/

#include <stdlib.h>

#include "oslib/fileswitch.h"
#include "oslib/macros.h"
#include "oslib/os.h"
#include "oslib/wimp.h"

os_error *save_desktop (os_f handle)

{  char *ptr;
   os_error *error = NULL;

   if ((ptr = getenv ("Edit$Dir")) != NULL && !EMPTY (ptr))
   {  if ((error = xos_bput ('/', handle)) != NULL)
         goto finish;

      while (*ptr != '\0')
         if ((error = xos_bput (*ptr++, handle)) != NULL)
            goto finish;

      if ((error = xos_bput ('\n', handle)) != NULL)
         goto finish;
   }

finish:
   return error;
}
