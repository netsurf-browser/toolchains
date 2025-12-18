#include "oslib/os.h"
#include "oslib/wimp.h"

void delete_and_recreate (wimp_w w, wimp_i i)

{  /*to delete*/
   wimp_set_icon_state (w, i, wimp_ICON_NEEDS_HELP | wimp_ICON_DELETED,
         wimp_ICON_NEEDS_HELP | wimp_ICON_DELETED);

   /*to recreate*/
   wimp_set_icon_state (w, i, NONE,
         wimp_ICON_NEEDS_HELP | wimp_ICON_DELETED);
}
