#include "oslib/os.h"
#include "oslib/osspriteop.h"

extern osspriteop_area *Ram;
extern char Sprite_Name [];

void ex (int height, int width, os_mode mode, os_palette *palette)

{  osspriteop_header *sptr;
   os_sprite_palette *pal;
   int i;

   osspriteop_create_sprite (osspriteop_NAME, Ram, Sprite_Name,
         /*create_palette?*/ FALSE, height, width, mode);

   sptr = (osspriteop_header *) ((char *) Ram + Ram->first);
   pal = (os_sprite_palette *) ((char *) sptr + sizeof (osspriteop_header));
   sptr->image += 2048;
   sptr->mask  += 2048;
   sptr->size += 2048;
   Ram->used += 2048;
      
   for (i = 0; i < 255; i++)
      pal->entries [i].on = pal->entries [i].off =
            palette->entries [i] & ~0xFFu;
}
