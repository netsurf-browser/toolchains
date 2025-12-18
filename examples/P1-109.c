#include "oslib/os.h"

os_error *read_palette (os_gcol entry_no, int entry_type,
      os_colour *on, os_colour *off)

{  os_error *error = NULL;
   osbool incomplete;

   if ((error = xpalettev_read_entry (entry_no, entry_type,
         on, off, &incomplete)) != NULL)
      goto finish;

   if (incomplete)
   {  if ((error = xos_read_palette (entry_no, entry_type, on, off)) != NULL)
         goto finish;

      *on  |= (*on  & 0xF0F0F000) >> 4;
      *off |= (*off & 0xF0F0F000) >> 4;
   }

finish:
   return error;
}
