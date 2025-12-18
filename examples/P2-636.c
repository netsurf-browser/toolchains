#include "oslib/econet.h"
#include "oslib/os.h"

extern byte Buffer [40];
static os_error *print_version (byte *);

os_error *report_station_version (byte station, byte net)

{  os_error *error = NULL;
   int status;

   static os_error Error;

   if ((error = xeconet_do_immediate (econet_IMMEDIATE_MACHINE_PEEK,
         SKIP, station, net, Buffer, sizeof Buffer, 40, 5, &status,
         NULL)) != NULL)
      goto finish;

   if (status == econet_STATUS_TRANSMITTED)
      print_version (Buffer);
   else if (status == econet_STATUS_NOT_LISTENING)
      status = econet_STATUS_NOT_PRESENT;

   if ((error = xeconet_convert_status_to_error (status, &Error,
         sizeof Error, station, net)) != NULL)
      goto finish;
   error = &Error;

finish:
   return error;
}

os_error *print_version (byte *buf)

{  os_error *error = NULL;

   if ((error = xos_convert_hex2 (buf [3], (char *) Buffer, sizeof Buffer,
         NULL)) != NULL)
      goto finish;

   if ((error = xos_write0 ((char *) Buffer)) != NULL)
      goto finish;

   if ((error = xos_writec ('.')) != NULL)
      goto finish;

   if ((error = xos_convert_hex2 (buf [2], (char *) Buffer, sizeof Buffer,
         NULL)) != NULL)
      goto finish;

   if ((error = xos_write0 ((char *) Buffer)) != NULL)
      goto finish;

finish:
   return error;
}
