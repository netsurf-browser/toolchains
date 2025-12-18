#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "oslib/os.h"

#include "lookup.h"
#include "x.h"

#include "def.h"

os_error *emit_destructors(FILE *file, lookup_t types)
{
   os_error *error = NULL;
   int       rc = 0;
   char     *type;
   def_t     t;
   void     *context = 0;

   /*Emit the TYPE declarations.*/
   while (TRUE)
   {
      /* step through the TYPES table */
      if ((error = lookup_enumerate (types, &type, (void **) &t, &context))
            != NULL)
         goto finish;
      if (context == 0) break;  /* end of table */

      if (t != NULL && t->tag == def_TYPE_STRUCT && t->data AS list.ellipsis && t->data AS list.count > 1)
      /* variable-size structure definition containing ellipsis */
      {
         char c_name [def_ID_LIMIT + 1];
         char macro_name [def_ID_LIMIT + 1];
         char *suffix;

         def_as_extern (c_name, type);
         def_as_macro (macro_name, type);

         /* start the macro definition line */
         if ((rc = fprintf (file, "#define os_lib_%s_new(n) \\\n",
               c_name)) < 0)
            goto finish;

         if ((suffix = strchr (macro_name, '_')) != NULL)
         {
            if ((rc = fprintf (file,
                  "  (%s*)malloc (%.*s_SIZEOF%s(n))\n\n",
                  c_name, (int)(suffix - macro_name), macro_name, suffix)) < 0)
               goto finish;
         }
         else
         {
            if ((rc = fprintf (file, "  (%s*)malloc (%s_SIZEOF(n))\n\n",
                 c_name, macro_name)) < 0)
               goto finish;
         }
      }
   }  /* end type declarations */

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror ();
   return error;
}

/* main entry */
os_error *vapi_h_output
(
   FILE     *file,
   lookup_t  types
)
{
   os_error *error = NULL;
   int       rc = 0;

   if ((error = emit_destructors(file, types)) != NULL) goto finish;

finish:
  if (rc < 0) error = (os_error*)_kernel_last_oserror ();
  return error;
}
