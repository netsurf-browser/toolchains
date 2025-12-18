#include "oslib/os.h"

void read_variable (void)

{  char value [255];
   int context, len;

   context = 0;
   while (xos_read_var_val ("Sys$*", value, sizeof value - 1,
         context, os_VARTYPE_EXPANDED, &len, &context, NULL) == NULL)
   {  os_writen (value, len);
      os_new_line ();
   }
}

extern void Code_Veneer (void);
extern int Code_Size;

void create_code_variable (void)

   /*Must be done in assembler. The creation part could be done like this,
      assuming Code_Veneer is the assembler code.*/

{  os_set_var_val ("Mode", (void *) &Code_Veneer, Code_Size, 0,
         os_VARTYPE_CODE, NULL);
}
