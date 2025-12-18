#include "oslib/os.h"

int xwind_size (void)

{  int value_list [2];

   static os_VDU_VAR_LIST (3) var_list = {os_MODEVAR_XEIG_FACTOR,
         os_MODEVAR_XWIND_LIMIT, -1};

   os_read_vdu_variables ((os_vdu_var_list *) &var_list, value_list);

   return value_list [1] + 1 << value_list [0];
}
