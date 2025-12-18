#include <string.h>

#include "oslib/econet.h"
#include "oslib/os.h"
#include "oslib/osbyte.h"
#include "oslib/osmodule.h"
#include "oslib/osword.h"

typedef struct clock_list clock_list;

extern int Event_Veneer (void);

struct clock_list {clock_list *next; econet_tx_cb tx; os_date_and_time clock;};

extern byte Command_Port;
extern econet_rx_cb Rx_Handle;
extern clock_list *Tx_List;
extern int Reply_Count, Reply_Delay;
extern byte Buffer [80];

os_error *start (void)

{  os_error *error = NULL;

   if ((error = xos_claim (EventV, (void *) &Event_Veneer, NULL)) != NULL)
      goto finish;

   if ((error = xos_byte (osbyte_ENABLE_EVENT,
         Event_EconetRx, SKIP, NULL, NULL)) != NULL)
      goto finish;

   if ((error = xos_byte (osbyte_ENABLE_EVENT,
         Event_EconetTx, SKIP, NULL, NULL)) != NULL)
      goto finish;

   if ((error = xeconet_create_receive (Command_Port, 0, 0,
         Buffer, sizeof Buffer, &Rx_Handle, NULL)) != NULL)
      goto finish;

finish:
   /*If error != NULL, should abandon receive (if created), disable
      enabled events and release EventV (if claimed) here.*/
   return error;
}

os_error *event (int *event, econet_rx_cb rx, int status)
           /*or (int *event, econet_tx_cb tx, int status)*/

   /*The mechanism by which event_handler() calls event() is not
      detailed here. Assume that if event is set to -1 on exit, the call is
      claimed.*/

{  os_error *error = NULL;
   clock_list *clock_ptr;
   byte port, station, net, *buffer;
   oswordreadclock_utc_block utc;

   switch (*event)
   {  case Event_EconetRx:
         if (rx == Rx_Handle)
         {  if ((error = xeconet_read_receive (rx, NULL, NULL, &port,
                  &station, &net, &buffer, NULL)) != NULL)
               goto finish;

            if ((error = xosmodule_alloc (sizeof *clock_ptr,
                  (void **) &clock_ptr)) != NULL)
               goto finish;

            utc.op = oswordreadclock_OP_UTC;
            if ((error = xoswordreadclock_utc (&utc)) != NULL)
               goto finish;
            memcpy (clock_ptr->clock, utc.utc, sizeof (os_date_and_time));

            if ((error = xeconet_start_transmit (0, Buffer [0], station,
                  net, (byte *) &clock_ptr->clock, sizeof clock_ptr->clock,
                  Reply_Count, Reply_Delay, &clock_ptr->tx, NULL)) !=
                  NULL)
               goto finish;

            /*Add the list to new record.*/
            clock_ptr->next = Tx_List;
            Tx_List = clock_ptr;

            /*Now reopen the reception.*/
            if ((error = xeconet_create_receive (Command_Port, 0, 0,
                  Buffer, sizeof Buffer, &Rx_Handle, NULL)) != NULL)
               goto finish;

            /*claim vector*/
            *event = -1;
         }
      break;

      case Event_EconetTx:
      {  econet_tx_cb tx = (econet_tx_cb) rx; /*change of name only*/
         clock_list **t;

         for (t = &Tx_List; *t != NULL; t = &(*t)->next)
            if ((*t)->tx == tx)
            {  /*Remove from the list*/
               *t = (*t)->next;
               (void) xeconet_abandon_transmit (tx, NULL);
               (void) xosmodule_free ((byte *) *t);
            }

         /*claim vector*/
         *event = -1;
      }
      break;
   }

finish:
   return error;
}
