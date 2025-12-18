/*asmhelp.c - output a help file given a module defn*/

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

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <kernel.h>         /* TV 980115 */

#include "oslib/os.h"

#include "lookup.h"

#include "def.h"
#include "asmhelp.h"

os_error *asmhelp_output
(
   FILE     *file,
   char     *title,
   char     *author,
   lookup_t  needses,
   lookup_t  needsatends,
   lookup_t  consts,
   lookup_t  types,
   lookup_t  swis
)
{
   os_error *error = NULL;
   int       rc = 0;
   char     *cnst, *type, *swi;
   def_c     c;
   def_t     t;
   def_s     s;
   void     *context;

   NOT_USED (author)
   NOT_USED (needses)
   NOT_USED (needsatends)

   /*Emit the CONST declarations.*/
   context = 0;
   while (TRUE)
   {
      if ((error = lookup_enumerate (consts, &cnst, (void **) &c, &context))
            != NULL)
         goto finish;

      if (context == 0) break;

      if ((rc = fprintf
            (  file,
               "%%%s\n"
               "Defined in:   oslib/Hdr.%s\n"
               "Symbol for a constant value\n\n",
               cnst, title)) < 0)
         goto finish;
   }

   /*Emit the TYPE declarations.*/
   context = 0;
   while (TRUE)
   {
      if ((error = lookup_enumerate (types, &type, (void **) &t, &context))
            != NULL)
         goto finish;

      if (context == 0) break;

      if ((rc = fprintf (file,
            "%%%s\n"
            "Defined in:   oslib/Hdr.%s.h\n"
            "Symbol for the size of a data structure\n\n",
            type, title)) < 0)
         goto finish;
   }

   /*Emit the SWI declarations.*/
   context = 0;
   while (TRUE)
   {
      int i;

      if ((error = lookup_enumerate (swis, &swi, (void **) &s, &context)) !=
            NULL)
         goto finish;

      if (context == 0) break;

      for (i = 0; i < 10; i++)
         if (s->starred_constants [i])
         {
            /* SWI reason code.  */
            if ((rc = fprintf (file,
                  "%%%s\n"
                  "Defined in:   oslib/Hdr.%s\n"
                  "Symbol for a SWI reason code\n\n",
                  swi, title)) < 0)
               goto finish;
            break;
         }
      if (i == 10)
      {
         /* This might be a starred_swi, or a SWI entry which has no
            description.  Warn about the last case.  */
         if (!s->starred_swi)
         {
           /* Make sure we understand this condition.  */
           assert(s->description == NULL);
           fprintf(stderr, "Warning: SWI %s has no description.\n", swi);
         }

         /* SWI number.  */
         if ((rc = fprintf (file,
               "%%%s\n"
               "Defined in:   oslib/Hdr.%s\n"
               "Symbol for a SWI number\n\n",
               swi, title)) < 0)
            goto finish;
   }  }

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror ();              /* TV 980115 */
   return error;
}
