#include <stdio.h>

#include "oslib/os.h"
#include "oslib/territory.h"

extern void Vector_Veneer (void);

void ex (void)

{  char *lowercase_table, *uppercase_table, flag = 1, command [80];
   os_error *error = NULL;
   osbool done_claim = FALSE;

   if ((error = xterritory_lower_case_table (territory_CURRENT,
         &lowercase_table)) != NULL)
      goto finish;
   if ((error = xterritory_upper_case_table (territory_CURRENT,
         &uppercase_table)) != NULL)
      goto finish;

   if ((error = xos_claim (WrchV, (void *) &Vector_Veneer,
         (byte *) &flag)) != NULL)
      goto finish;
   done_claim = TRUE;

   do
   {  fgets (command, sizeof command - 1, stdin);
      if ((error = xos_cli (command)) != NULL)
         goto finish;
   }
   while (command [0] != '\n');

finish:
   if (done_claim)
   {  os_error *error1;

      error1 = xos_release (WrchV, (void *) &Vector_Veneer,
            (byte *) &flag);
      if (error == NULL) error = error1;
   }

   if (error != NULL)
      os_generate_error (error);
}
