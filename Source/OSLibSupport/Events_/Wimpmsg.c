/* Title:   wimpmsg.c
 * Purpose: functions for wimp message handling
 * Author:  IDJ
 * History: 19-Jun-94:     IDJ:   created
 *          30th Aug 1995  J R C  Hacked for Support library
            30-Dec-00      TV     fix crash when last item in list is deleted
 *          15-jun-00:     TV     Fixed bug causing crash if handler deregisters itself
 */
/*
      OSLibSupport is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

      OSLibSupport is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
   along with this programme; if not, write to the Free Software
   Foundation, Inc, 675 Mass Ave, Cambridge, MA 02139, U S A.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oslib/os.h"

#include "Wimpmsg.h"
//#include "trace.h"
#include "X.h"

static event_message_handler_item *Handlers = NULL; /*the list of all
      wimp message handlers*/

/* ------------------ dispatching a wimp message ------------------ */
#if 0    // TV 20010615
         // If the current handler de-registers itself,
         // then the link to the next handler disappears -- *crash*
void wimpmsg_dispatch (wimp_block *poll_block, toolbox_block *id_block)

{  event_message_handler_item *h;

   id_block = id_block;

//   tracef ("wimpmsg_dispatch\n");
   for (h = Handlers; h != NULL; h = h->next)
      if (h->handler != NULL && (h->msg_no == -1 ||
            h->msg_no == poll_block->message.action))
         if (h->handler (&poll_block->message, h->handle))
            return;
}
#else
void wimpmsg_dispatch (wimp_block *poll_block, toolbox_block *id_block)
{
   event_message_handler_item *h;
   osbool claimed = FALSE;

   id_block = id_block;

//   tracef ("wimpmsg_dispatch\n");
   for (h = Handlers, claimed = FALSE; !claimed && h != NULL; )
   {
      event_message_handler_item *h1 = h;
      h = h -> next;

      if( h1->handler != NULL
          && ( h1->msg_no == -1 ||
               h1->msg_no == poll_block->message.action
             )
        )
      {
         claimed = h1->handler( &poll_block->message, h1->handle);
      }
   }
}
#endif

osbool wimpmsg_register_message_handler (int msg_no,
      event_message_handler *handler, void *handle)

{  event_message_handler_item *h;
   osbool done = FALSE;

//   h = x_ALLOC (sizeof *h);
   h = malloc(sizeof *h);
   if( h )
   {
      h->msg_no     = msg_no;
      h->handler    = handler;
      h->handle     = handle;
      h->next       = Handlers;
      Handlers = h;
      done = TRUE;
   }
   return done;
}

void wimpmsg_deregister_message_handler (int msg_no,
     event_message_handler *handler, void *handle)

{  event_message_handler_item **h;

   /*search for exact match of handler, list and handle*/
   for (h = &Handlers; *h != NULL; h = &(*h)->next)
      if ((*h)->handler == handler && (*h)->handle == handle &&
            (*h)->msg_no == msg_no)
      {  event_message_handler_item *hh = *h;

         *h = hh->next;
//#if TRACE
//         x_FREE (hh, sizeof *hh);
//#else
         free(hh);
//#endif
         /* TV 001230 fix crash when last item is deleted */
         break;
      }
}
