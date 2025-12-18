#include <stdio.h>

#include "oslib/econet.h"
#include "oslib/os.h"

void tx1 (void)

{  byte buf [20];
   int status;
   os_error error;

   status = econet_do_transmit (NONE, 99, 7, 0, buf, sizeof buf, 6, 100,
         NULL);
   if (status != econet_STATUS_TRANSMITTED)
      econet_convert_status_to_error (status, &error, sizeof error,
            SKIP, SKIP);
}

os_error *tx2 (void)

{  byte buf [20];
   int status;
   os_error *error = NULL, error1;

   if ((error = xeconet_do_transmit (NONE, 99, 7, 0, buf, sizeof buf,
         6, 100, &status, NULL)) != NULL)
      goto finish;

   if (status != econet_STATUS_TRANSMITTED)
   {  error = xeconet_convert_status_to_error (status, &error1,
            sizeof error1, SKIP, SKIP);
      goto finish;
   }

finish:
   return error;
}
