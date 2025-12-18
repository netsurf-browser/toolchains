#include "oslib/font.h"

void ex (char *file_name)

{  font_un_cache_file (file_name, FALSE);

   /*... replace old file with new one ...*/

   font_un_cache_file (file_name, TRUE);
}
