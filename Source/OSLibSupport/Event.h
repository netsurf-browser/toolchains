/*
      OSLibSupport is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version - and this with the following clarification and
   special exception:

      Linking this library statically or dynamically with other modules
   is making a combined work based on this library. Thus, the terms
   and conditions of the GNU General Public License cover the whole
   combination.

      As a special exception, the copyright holders of this library
   give you permission to link this library with independent modules
   to produce an executable, regardless of the license terms of these
   independent modules, and to copy and distribute the resulting
   executable under terms of your choice, provided that you also meet,
   for each linked independent module, the terms and conditions of the
   license of that module. An independent module is a module which is
   not derived from or based on this library. If you modify this
   library, you may extend this exception to your version of the library,
   but you are not obliged to do so. If you do not wish to do so, delete
   this exception statement from your version.

      OSLibSupport is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
   along with this programme; if not, write to the Free Software
   Foundation, Inc, 675 Mass Ave, Cambridge, MA 02139, U S A.
*/
#ifndef event_H
#define event_H

#ifndef wimp_H
#include "oslib/wimp.h"
#endif

#ifndef toolbox_H
#include "oslib/toolbox.h"
#endif

#define event_ANY ((toolbox_o) -1)
#define action_ANY ((bits)0xFFFFFFFFu)

typedef osbool event_wimp_handler (wimp_event_no event_code,
      wimp_block *block, toolbox_block *id_block,
      void *handle);

typedef osbool event_toolbox_handler (bits event_code,
      toolbox_action *action, toolbox_block *id_block,
      void *handle);

typedef osbool event_message_handler (wimp_message *message,
      void *handle);

typedef struct event_wimp_handler_item event_wimp_handler_item;

struct event_wimp_handler_item
   {  event_wimp_handler_item *next;
      toolbox_o               object_id;
      wimp_event_no                     event_code;
      event_wimp_handler      *handler;
      void                    *handle;
   };


typedef struct event_toolbox_handler_item event_toolbox_handler_item;

struct event_toolbox_handler_item
   {  event_toolbox_handler_item *next;
      toolbox_o                  object_id;
      bits                        event_code;
      event_toolbox_handler      *handler;
      void                       *handle;
   };

typedef struct event_message_handler_item event_message_handler_item;

struct event_message_handler_item
   {  event_message_handler_item *next;
      int                        msg_no;
      event_message_handler      *handler;
      void                       *handle;
   };

#ifdef __cplusplus
extern "C"
{
#endif

extern void event_poll (wimp_event_no *event_code, wimp_block *poll_block,
      void *poll_word);

extern void event_poll_idle (wimp_event_no *event_code, wimp_block *poll_block,
      os_t earliest, void *poll_word);

/* Register wimp event handler
** returns FALSE if out of memory
*/
extern osbool event_register_wimp_handler (toolbox_o object_id,
      wimp_event_no event_code, event_wimp_handler *handler, void *handle);

extern void event_deregister_wimp_handler (toolbox_o object_id,
      wimp_event_no event_code, event_wimp_handler *handler, void *handle);

/* Register toolbox event handler
** returns FALSE if out of memory
*/
extern osbool event_register_toolbox_handler(toolbox_o object_id,
                                             bits event_code,
                                             event_toolbox_handler *handler,
                                             void *handle
                                            );

extern void event_deregister_toolbox_handler (toolbox_o object_id,
      bits event_code, event_toolbox_handler *handler, void *handle);

/* Register wimp message handler
** returns FALSE if out of memory
*/
extern osbool event_register_message_handler (bits msg_no,
      event_message_handler *handler, void *handle);

extern void event_deregister_message_handler (bits msg_no,
      event_message_handler *handler, void *handle);

extern void event_set_mask (wimp_poll_flags mask);

extern void event_get_mask (wimp_poll_flags *mask);

extern void event_initialise (toolbox_block *id_block);

#ifdef __cplusplus
}
#endif

#endif
