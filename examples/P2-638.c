#include "oslib/econet.h"
#include "oslib/macros.h"
#include "oslib/os.h"

#define ERROR_NUMBER_SYNTAX 1
#define ERROR_NUMBER_UNABLE_TO_DEFAULT 2

static os_error Error_Get_Regs_Syntax =
      {ERROR_NUMBER_SYNTAX, "Syntax: *Command <station number>"};

static os_error Error_Unable_To_Default =
   {  ERROR_NUMBER_UNABLE_TO_DEFAULT,
      "Either a station number or a full network address is required"
   };

os_error *command_start (char *buf, byte *station_out, byte *net_out)

{  os_error *error = NULL;
   int station, net; /*must be int to detect -1 returns*/

   if (EMPTY (buf))
   {  error = &Error_Get_Regs_Syntax;
      goto finish;
   }

   if ((error = xeconet_read_station_number (buf, NULL, &station, &net))
         != NULL)
      goto finish;

   if (station == -1)
   {  error = &Error_Unable_To_Default;
      goto finish;
   }

   if (net == -1) net = 0;

   *station_out = (byte) station; /*narrowing cast*/
   *net_out     = (byte) net;

finish:
   return error;
}
