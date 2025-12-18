#include "oslib/wimp.h"

#define ICON_COUNT 12

void ex (wimp_w w)

{  wimp_i icons [ICON_COUNT];

   wimp_which_icon (w, icons, wimp_ICON_SELECTED, wimp_ICON_SELECTED);
      /*|icons| contains all selected icons*/

   wimp_which_icon (w, icons, wimp_ICON_ESG | wimp_ICON_SELECTED,
         1 << wimp_ICON_ESG_SHIFT | wimp_ICON_SELECTED);
      /*|icons| contains selected icons with ESG 1*/
}
