#include "oslib/wimp.h"

wimp_t task_from_icon (wimp_w w, wimp_i i)

{  wimp_message message;

   message.size = 20;
   message.your_ref = 0;
   /*rest not used*/

   return wimp_send_message_to_window (wimp_USER_MESSAGE_ACKNOWLEDGE,
         &message, w, i);
}
