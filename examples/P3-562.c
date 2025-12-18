#include "oslib/os.h"
#include "oslib/osfind.h"
#include "oslib/pdriver.h"

void skeleton (void)

{  pdriver_info (...);

   osfind_openout (..., "printer:", ...);

   pdriver_select_job (...);

   if (... driver supports pdriver_declare_font ...)
   {  while (... fonts to be declared ...)
         pdriver_declare_font (...);
      pdriver_declare_font (...);
   }

   for (... each page to print ...)
   {  do pdriver_give_rectangle (...);
      while (... rectangles to declare ...);

      pdriver_draw_page (...);

      while (... more rectangles to print ...)
      {  ... plot returned rectangles using supported output calls ...;
         pdriver_get_rectangle (...);
      }
   }

   pdriver_end_job (...);

   osfind_close (...);
}
