   /*Note there are bugs in the assembler: the receive block is not abandoned
if econet_do_transmit() fails, and R6, R7 are the wrong way round in the call
to Econet_DoTransmit.*/

#include "oslib/econet.h"
#include "oslib/os.h"

#define ENTRY_PORT 0x1Fu
#define ERROR_NUMBER_BUFFER_OVERFLOW 1

extern byte Server_Station, Server_Net;
extern int Server_Delay, Server_Tx_Delay, Server_Tx_Count;

static os_error Buffer_Overflow_Error =
      {ERROR_NUMBER_BUFFER_OVERFLOW, "Command too long for buffer"};

os_error *example (char *text)

{  byte reply_port = 0, buffer [80];
   os_error *error = NULL, error1;
   econet_rx_cb rx_cb = 0;
   int i,status;
   char c;

   if ((error = xeconet_allocate_port (&reply_port)) != NULL)
      goto finish;

   if ((error = xeconet_create_receive (reply_port, Server_Station,
         Server_Net, buffer, sizeof buffer, &rx_cb, NULL)) != NULL)
      goto finish;

   buffer [0] = reply_port;
   for (i = 1, c = *text++;; i++)
   {  if (!(i < sizeof buffer))
      {  error = &Buffer_Overflow_Error;
         goto finish;
      }
      if (c < ' ')
      {  buffer [i] = '\r';
         break;
      }
      else
         buffer [i] = c;
   }

   if ((error = xeconet_do_transmit (NONE, ENTRY_PORT, Server_Station,
         Server_Net, buffer, sizeof buffer, Server_Tx_Count,
         Server_Tx_Delay, &status, NULL)) != NULL)
      goto finish;

   if (status != econet_STATUS_TRANSMITTED)
   {  error = xeconet_convert_status_to_error (status, &error1,
            sizeof error1, Server_Station, Server_Net);
      goto finish;
   }

   if ((error = xeconet_wait_for_reception (rx_cb, Server_Delay,
         /*abort on excape?*/ FALSE, &status, NULL, NULL, NULL,
         NULL, NULL, NULL)) != NULL)
      goto finish;
   rx_cb = 0;

   if (status != econet_STATUS_RECEIVED)
   {  error = xeconet_convert_status_to_error (status, &error1,
            sizeof error1, Server_Station, Server_Net);
      goto finish;
   }

   Server_Command_Port = *(int *) buffer;

finish:
   if (error != NULL)
   {  if (rx_cb != 0)
      {  os_error *error1;

         error1 = xeconet_abandon_receive (rx_cb);
         if (error == NULL) error = error1;
      }

      if (reply_port != 0)
      {  os_error *error1;

         error1 = xeconet_de_allocate_port (reply_port);
         if (error == NULL) error = error1;
      }
   }

   return error;
}
