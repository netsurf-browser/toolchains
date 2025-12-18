#include "oslib/os.h"
#include "oslib/osspriteop.h"
#include "oslib/wimp.h"

int main (void)

{  osspriteop_area *rom;

   wimp_base_of_sprites (&rom, NULL);
   osspriteop_save_sprite_file (osspriteop_USER_AREA, rom, "WSprite");

   return 0;
}
