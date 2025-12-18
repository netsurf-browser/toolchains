#include "oslib/hourglass.h"
#include "oslib/os.h"
#include "oslib/osfile.h"

extern byte Buffer [];

extern os_error *Process_Byte (byte);

os_error *do_load_and_process (char *file_name)

{  os_error *error = NULL;
   int i, size;
   osbool done_on = FALSE;

   if ((error = xosfile_load_stamped (file_name, Buffer, NULL, NULL, NULL,
         &size, NULL)) != NULL)
      goto finish; /*note that osfile_load loads a file at its load address*/

   if (size != 0)
   {  if ((error = xhourglass_on ()) != NULL)
         goto finish;
      done_on = TRUE;

      for (i = 0; i < size; i++)
         if ((error = Process_Byte (Buffer [i])) != NULL)
            goto finish;
   }

finish:
   if (done_on)
   {  os_error *error1;

      error1 = xhourglass_off ();
      if (error == NULL) error = error1;
   }

   return error;
}
