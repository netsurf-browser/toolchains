#include <string.h>

#include "oslib/macros.h"
#include "oslib/os.h"
#include "oslib/wimp.h"

os_error *write_icon (wimp_w w, wimp_i i, char *buffer, char *string)

{  os_error *error = NULL;
   wimp_caret caret;

   strcpy (buffer, string);
      /*assume that these are 0-terminated, rather than 13 as in BASIC*/

   if ((error = xwimp_get_caret_position (&caret)) != NULL)
      goto finish;

   if (w == caret.w && i == caret.i)
   {  int len = strlen (buffer);

      if ((error = xwimp_set_caret_position (caret.w, caret.i,
            caret.pos.x, caret.pos.y, -1, MIN (caret.index, len))) != NULL)
         goto finish;
   }

   if ((error = xwimp_set_icon_state (w, i, NONE, NONE)) != NULL)
      goto finish;

finish:
   return error;
}
