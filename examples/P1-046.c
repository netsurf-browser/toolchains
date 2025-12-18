#include "oslib/messagetrans.h"
#include "oslib/os.h"

extern void Acknowledge_Escape (void);
static os_error Error = {error_ESCAPE, "Escape"}; /*error block for escape*/

void ex (void)

{  if ((os_read_escape_state () & _C) != NONE)
   {  Acknowledge_Escape (); /*acknowledge it*/

      os_generate_error (xmessagetrans_error_lookup (&Error, NULL, NULL, 0,
            SKIP, SKIP, SKIP, SKIP));

      /* Exactly the same effect would be given by

         messagetrans_error_lookup (&Error, NULL, NULL, 0, SKIP, SKIP, SKIP,
               SKIP);
      */
}  }
