#include <stdio.h>
#include <string.h>

#include "oslib/econet.h"
#include "oslib/os.h"

extern byte Rx_Buffer [10];
extern byte Tx_Buffer [10];
extern byte Net_To_Test_For;

void ex (void)

{  byte port;
   econet_rx_cb rx_cb;
   int status;

   port = econet_allocate_port ();

   rx_cb = econet_create_receive (port, 0, 0, Rx_Buffer,
         sizeof Rx_Buffer, NULL);

   strcpy ((char *) Tx_Buffer, "Bridge");
   Tx_Buffer [6] = port;
   Tx_Buffer [7] = Net_To_Test_For;

   (void) econet_do_transmit (0x83u, 0x9Cu, 0xFFu, 0xFFu,
         Tx_Buffer, sizeof Tx_Buffer, 5, 5, NULL);

   status = econet_wait_for_reception (rx_cb, 10, FALSE, NULL, NULL, NULL,
         NULL, NULL, NULL);

   if (status == econet_STATUS_RECEIVED)
      printf ("Net number %d exists.\n", Net_To_Test_For);

   econet_de_allocate_port (port);
}
