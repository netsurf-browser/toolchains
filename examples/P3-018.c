#include "oslib/os.h"
#include "oslib/wimp.h"

extern wimp_block Block;
      /*|Block| is the wimp_poll block*/

void redraw (void)

{  osbool more;

   for (more = wimp_redraw_window (&Block.redraw); more;
         more = wimp_get_rectangle (&Block.redraw))
   {  /*Redraw contents of appropriate window*/
   }
}
