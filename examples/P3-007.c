#include "oslib/os.h"
#include "oslib/wimp.h"

int main (void)

{  osboolfinished = FALSE;
   wimp_block blk; /*get the block for wimp_poll()*/
   int event;
   
   wimp_initialise (...);
      /*tell the wimp about the task*/
   
   while (!finished)
   {  wimp_poll (... &blk ... &event);
   
      switch (event)
      {  case wimp_NULL_REASON_CODE:
            /*... do Wimp_NullReasonCode ...*/
         break;
   
         case wimp_REDRAW_WINDOW_REQUEST:
            /*... do Wimp_RedrawWindowRequest ...*/
         break;
   
         /*etc*/
      }
   }
   
   wimp_close_down (...);

   return 0;
}
