#include "oslib/osfscontrol.h"

extern int Image_File_Type;

os_error *remove_image (void)

{  return xosfscontrol_deregister_image_fs (Image_File_Type);
}
