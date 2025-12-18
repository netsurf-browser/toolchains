#include "oslib/colourtrans.h"
#include "oslib/os.h"
#include "oslib/osspriteop.h"

extern osspriteop_area *Ram;
extern osspriteop_trans_tab *Pix_Trans;

void ex (void)

   /*ColourTrans can do this itself.*/

{  osspriteop_header *sptr;

   sptr = (osspriteop_header *) ((char *) Ram + Ram->first);

   colourtrans_select_table_for_sprite (Ram, (osspriteop_id) sptr,
         colourtrans_CURRENT_MODE, colourtrans_CURRENT_PALETTE,
         Pix_Trans, colourtrans_GIVEN_SPRITE);
}
