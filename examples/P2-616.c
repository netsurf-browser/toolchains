#include <stdio.h>

#include "oslib/econet.h"

extern econet_tx_cb Tx_Cb;

void ex1 (void)

{  printf ("The final status was %d\n", econet_abandon_transmit (Tx_Cb));
}

void ex2 (void)

{  byte buf [20];

   printf ("The final status was %d\n",
         econet_do_transmit (NONE, 99, 7, 0, buf, sizeof buf, 6, 100, NULL));
}
