#include "oslib/osfscontrol.h"

extern char *Filing_System_Name;

os_error *select (void)

{  return xosfscontrol_select_fs ((osfscontrol_id) Filing_System_Name);
}

os_error *remove (void)

{  return xosfscontrol_remove_fs (Filing_System_Name);
}
