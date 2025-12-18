#include "oslib/os.h"
#include "oslib/wimp.h"

#define EXIT 6 /*position in Task Manager menu of Exit entry*/

extern wimp_block *Block;
extern wimp_t Quit_Sender;
extern wimp_message Pre_Quit_Block, Quit_Block;
extern int Event, Pre_Quit_Ref, Quit_Ref;
extern void Quit (void);

void ex1 (void)

{  wimp_get_caret_position (&Block->caret);
   Block->key.c = wimp_KEY_SHIFT | wimp_KEY_CONTROL | wimp_KEY_F12;
   wimp_send_message (wimp_KEY_PRESSED, &Block->message, Quit_Sender);
}

void ex2 (void)

{  switch (Event)
   {  case wimp_MENU_SELECTION:
         if (Block->selection.items [0] == EXIT)
         {  wimp_send_message (wimp_USER_MESSAGE_RECORDED, &Pre_Quit_Block,
                  wimp_BROADCAST);
            Pre_Quit_Ref = Pre_Quit_Block.my_ref;
         }
      break;

      case wimp_USER_MESSAGE_ACKNOWLEDGE:
         if (Block->message.my_ref == Pre_Quit_Ref)
         {  wimp_send_message (wimp_USER_MESSAGE_RECORDED, &Quit_Block,
                  wimp_BROADCAST);
            Quit_Ref = Quit_Block.my_ref;
         }
         else if (Block->message.action == message_QUIT &&
               Block->message.my_ref == Quit_Ref)
            Quit ();
      break;

      case wimp_USER_MESSAGE:
      case wimp_USER_MESSAGE_RECORDED:
         if (Block->message.action == message_QUIT &&
               Block->message.my_ref != Quit_Ref)
            Quit ();
      break;
   }
}
