#include "oslib/econet.h"
#include "oslib/os.h"

os_error *tx (void)

{  byte buf [20];
   int status;
   os_error *error = NULL;
   char text [50], *end;

   if ((error = xeconet_do_transmit (NONE, 99, 7, 0, buf, sizeof buf,
         6, 100, &status, NULL)) != NULL)
      goto finish;

   if (status != econet_STATUS_TRANSMITTED)
   {  if ((error = xos_convert_cardinal1 (status, text, sizeof text, &end))
            != NULL)
         goto finish;

      if ((error = xeconet_convert_status_to_string (status, end,
               sizeof text - (end - text), 99, 7, NULL)) != NULL)
         goto finish;

      if ((error = xos_write0 (text)) != NULL)
         goto finish;
   }

finish:
   return error;
}
