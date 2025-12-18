#include "oslib/econet.h"
#include "oslib/messagetrans.h"
#include "oslib/os.h"

static os_error Error_No_Econet = {error_ECONET_NO_ECONET, "NoEco"};

os_error *has_econet (void)

   /*Return an error if no econet*/

{  os_error *error = NULL, *error1;

   if ((error1 = xeconet_set_protection (NONE, ALL, NULL)) != NULL &&
         error1->errnum == error_NO_SUCH_SWI)
   {  error = xmessagetrans_error_lookup (&Error_No_Econet, NULL, NULL, 0,
            0, 0, 0, 0);
      goto finish;
   }

finish:
   return error;
}
