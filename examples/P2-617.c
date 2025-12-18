#include <stdio.h>
#include <stdlib.h>

#include "oslib/econet.h"

void ex1 (int status)

{  econet_convert_status_to_error (status, NULL, 0, 0, 0);
}

void ex2 (int status)

{  os_error error;

   econet_convert_status_to_error (status, &error, sizeof error, 0, 0);
}

void ex3 (int status, byte station, byte net)

{  os_error error;

   econet_convert_status_to_error (status, &error, sizeof error, station,
         net);
}

void ex4 (int status)

{  if (status == econet_STATUS_TRANSMITTED)
   {  printf ("OK\n");
      exit (0);
   }
}
