/*44 instructions*/

#include "oslib/colourtrans.h"
#include "oslib/font.h"
#include "oslib/os.h"
#include "oslib/pdriver.h"

void x (int page, char *t)

{  osbool more;
   os_box req;

   for (pdriver_draw_page (1,    /* number of copies */
                           &req, /* plotting rectangle */
                           page, /* page sequence number */
                           NULL, /* page number */
                           NULL  /* returned identification for rectangle */
                          );
         more;
         xpdriver_get_rectangle (&req, &more, NULL)
       )
   {  colourtrans_set_gcol (os_COLOUR_BLACK, colourtrans_SET_FG,
            os_ACTION_OVERWRITE, NULL);

      font_paint (0, t, NONE, 0, 0, NULL, NULL, 0);
   }
}
