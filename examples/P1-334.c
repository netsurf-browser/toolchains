#include "oslib/os.h"
#include "oslib/osbyte.h"

extern void Timer_Veneer (void);
extern void *Alarm;

extern void Call_Back_Veneer (void);
extern void *Save_Block;
extern void Event_Veneer (void);

void add_call_back (void)

   /*The code to install the handler only.*/

{  os_call_after (200, (void *) &Timer_Veneer, Alarm);
}

void call_back_handler (void)

   /*Installation code only.*/

{  void *old_callbacks, *old_events;
   byte *old_buffer;
   osbool shift_pressed;

   old_callbacks = os_change_environment (os_HANDLER_CALL_BACK,
         (void *) &Call_Back_Veneer, 0, Save_Block, NULL, &old_buffer);
   old_events = os_change_environment (os_HANDLER_EVENT,
         (void *) &Event_Veneer, 0, 0, NULL, NULL);

   os_byte (osbyte_ENABLE_EVENT, Event_MouseTransition, SKIP, NULL, NULL);

   do os_byte (osbyte_IN_KEY, 0 ^ 0xFF, 0xFF, &shift_pressed, NULL);
   while (!shift_pressed); /*loop until shift*/

   os_byte (osbyte_DISABLE_EVENT, Event_MouseTransition, SKIP, NULL, NULL);

   os_change_environment (os_HANDLER_EVENT, old_events, 0, 0,
         NULL, NULL);
   os_change_environment (os_HANDLER_CALL_BACK, old_callbacks, 0, old_buffer,
         NULL, NULL);
}
