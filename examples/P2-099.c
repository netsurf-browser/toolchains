#include "oslib/osfscontrol.h"

char *fts (int fti)

{  static char file_type [9];

   osfscontrol_read_file_type (fti,
         (bits *) &file_type [0], (bits *) &file_type [4]);
   file_type [8] = '\0';

   return file_type;
}
