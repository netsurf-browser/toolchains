#include <stdio.h>

#include "oslib/econet.h"

extern econet_tx_cb Tx_Cb;

void ex1 (void)

{  byte buf [20];

   Tx_Cb = econet_start_transmit (NONE, 99, 7, 0, buf, sizeof buf, 3, 100,
         NULL);
}

void ex2 (void)

{  int status;

   do
   {  status = econet_poll_transmit (Tx_Cb);
      printf ("%d\n", status);
   }
   while (status == econet_STATUS_TX_READY ||
         status == econet_STATUS_TRANSMITTING);
}
