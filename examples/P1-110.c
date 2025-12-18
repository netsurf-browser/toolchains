#include "oslib/os.h"
#include "oslib/osword.h"

os_error *read_palette (os_gcol gcol, int entry_type, int on, int off)

{  os_error *error = NULL;
   osbool incomplete;
   osword_palette_block entry;

   if ((error = xpalettev_set_entry (gcol, entry_type,
         on, off, &incomplete)) != NULL)
      goto finish;

   if (incomplete)
   {  entry.gcol = gcol;
      entry.colour_number = entry_type;
      entry.r = (on & os_R) >> os_RSHIFT;
      entry.g = (on & os_G) >> os_GSHIFT;
      entry.b = (on & os_B) >> os_BSHIFT;

      if ((error = xosword_write_palette (&entry)) != NULL)
         goto finish;
   }

finish:
   return error;
}
