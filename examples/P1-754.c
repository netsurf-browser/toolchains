#include <stdlib.h>

#include "oslib/os.h"
#include "oslib/osspriteop.h"

extern osspriteop_area *My_Area;
extern char My_Sprite [];
extern os_error No_Memory;
extern osbool Interested;

void ex1 (void)

{  int r0, r1, r2, r3;

   osspriteop_switch_output_to_sprite (osspriteop_NAME, My_Area,
         (osspriteop_id) My_Sprite, NULL, &r0, &r1, &r2, &r3);

   /*... do whatever graphics we want ...*/

   osspriteop_unswitch_output (r0, r1, r2, r3);
}

void ex2 (void)

{  osspriteop_save_area *save_area;
   int c0, c1, c2, c3;

   if ((save_area = malloc (osspriteop_read_save_area_size (osspriteop_NAME,
         My_Area, (osspriteop_id) My_Sprite))) == NULL)
      os_generate_error (&No_Memory);

   save_area->a [0] = 0;

   while (Interested)
   {  osspriteop_switch_output_to_sprite (osspriteop_NAME, My_Area,
         (osspriteop_id) My_Sprite, save_area, &c0, &c1, &c2, &c3);

      /*... work on the sprite ...*/

      osspriteop_unswitch_output (c0, c1, c2, c3);

      /*... talk to the user ...*/
   }
}
