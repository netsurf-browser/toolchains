#include "oslib/os.h"
#include "oslib/osfile.h"

os_error *ex (char *file_name)

{  int obj_type;
   os_error *error = NULL;

   if ((error = xosfile_read_stamped (file_name, &obj_type, NULL,
         NULL, NULL, NULL, NULL)) != NULL)
      goto finish;;

   if (obj_type != osfile_IS_FILE)
   {  error = xosfile_make_error (file_name, obj_type);
      goto finish;
   }

finish:
   return error;
}
