#include "oslib/font.h"

void apply_encoding (char *result, int size, char *original, char *encoding)

   /*There is a SWI to do this.*/

{  font_apply_fields (original, encoding, result, size);
}
