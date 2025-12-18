/*vapi_deps.c - output a deps file given a module defn*/

/*OSLib---efficient, type-safe, transparent, extensible,\n"
   register-safe API coverage of RISC OS*/
/*Copyright (c) 1994 Jonathan Coxhead*/

/*
      OSLib is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

      OSLib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
   along with this programme; if not, write to the Free Software
   Foundation, Inc, 675 Mass Ave, Cambridge, MA 02139, U S A.
*/

#include <stdio.h>

#include "oslib/os.h"

#include "lookup.h"
#include "def.h"

os_error *emit_dependencies(FILE *file, lookup_t needses)
{
   os_error *error = NULL;
   int       rc = 0;
   char     *needs;
   void     *context = 0;

   if ((rc = fprintf (file, "OSLibTypes\n")) < 0)
      goto finish;

   /*Emit the NEEDS declarations.*/
   while (TRUE)
   {
      if ((error = lookup_enumerate (needses, &needs, NULL, &context)) != NULL)
         goto finish;

      if (context == 0) break;

      if ((rc = fprintf (file, "OSLib%s\n", needs)) < 0)
         goto finish;
   }

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror ();
   return error;
}

/* main entry */
os_error *vapi_deps_output
(
   FILE     *file,
   lookup_t  needses
)
{
   os_error *error = NULL;
   int       rc = 0;

   if ((error = emit_dependencies(file, needses)) != NULL) goto finish;

finish:
  if (rc < 0) error = (os_error*)_kernel_last_oserror ();
  return error;
}
