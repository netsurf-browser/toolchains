/*cheader.c - output a cheader file given a module defn*/

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
#include <time.h>
#include <kernel.h>      /* TV 980115 */

#include "oslib/os.h"
#include "oslib/types.h"
#include "oslib/macros.h"

#include "lookup.h"

#include "def.h"
#include "cheader.h"
#include "oslib_licence.h"

static int strsplen
(
   const char *s
)
{
   const char *p = s;

   while (*p != '\0' && *p != ' ') p++;

   return p - s;
}
/*-----------------------------------------------------------------------*/

static char *Op
(
   def_op op
)
{
   switch (op)
   {
      case def_OP_DISJOINS:
         return "|=";
      break;

      case def_OP_CONJOINS:
         return "&=";
      break;

      case def_OP_ADDS:
         return "+=";
      break;

      case def_OP_EXCLUSIVELY_DISJOINS:
         return "^=";
      break;
   }

   return NULL;
}
/*-----------------------------------------------------------------------*/

/*Prints a declaration of |v| as an object of type |t|, using |tag| as the
   structure tag. If |var|, emit 'N' instead of 'UNKNOWN' and escape
   newlines.*/

static int Print_Decl
(
   FILE  *file,
   def_t  t,
   char  *tag,
   char  *v,
   osbool var,
   int    nest
)
{
   int rc = 0;

   switch (t->tag)
   {
      case def_TYPE_INT:
         if ((rc = fprintf (file, v == NULL? "int": "int %s", v)) < 0)
            goto finish;
      break;

      case def_TYPE_SHORT:
         if ((rc = fprintf (file, v == NULL? "short": "short %s", v)) < 0)
            goto finish;
      break;

      case def_TYPE_BYTE:
         if ((rc = fprintf (file, v == NULL? "byte": "byte %s", v)) < 0)
            goto finish;
      break;

      case def_TYPE_CHAR:
         if ((rc = fprintf (file, v == NULL? "char": "char %s", v)) < 0)
            goto finish;
      break;

      case def_TYPE_BITS:
         if ((rc = fprintf (file, v == NULL? "bits": "bits %s", v)) < 0)
            goto finish;
      break;

      case def_TYPE_BOOL:
         if ((rc = fprintf (file, v == NULL? "osbool": "osbool %s", v)) < 0)
            goto finish;
      break;

      case def_TYPE_REF:
      {
         char v1 [def_ID_LIMIT + 1];

         if (v == NULL || v [0] == '/')
         {
            if (v != NULL)
            {
               if ((rc = sprintf (v1, "*%s", v)) < 0)
                  goto finish;
            }
            else
            {
               if ((rc = sprintf (v1, "*")) < 0)
                  goto finish;
            }
         }
         else
         {
            if ((rc = sprintf (v1, "*%s", v)) < 0) /*was (%s) 2nd Mar
                  1994*/
               goto finish;
         }

         if ((rc = Print_Decl (file, t->data AS ref, NULL, v1, var,
               nest + 1)) < 0)
            goto finish;
      }
      break;

      case def_TYPE_STRING:
         if ((rc = fprintf (file, "char %s", v)) < 0)
            goto finish; /*v != NULL*/
      break;

      case def_TYPE_ASM:
         if ((rc = fprintf (file, v == NULL? "asm_routine": "asm_routine %s", v)) < 0)
            goto finish;
      break;

      case def_TYPE_DATA:
         if ((rc = fprintf (file, "byte %s", v)) < 0)
            goto finish; /*v != NULL*/
      break;

      case def_TYPE_STRUCT:
      case def_TYPE_UNION:
      {
         int i;

         if
         (  (  rc =
                  tag != NULL?
                     fprintf
                     (
                        file,
                        "%s %s%s\n%*s{  ",
                        t->tag == def_TYPE_STRUCT? "struct":
                        t->tag == def_TYPE_UNION?  "union": "list",
                        var? " \\": "",
                        tag,
                        3*(nest + 1), ""
                     ):
                     fprintf
                     (
                        file,
                        "%s%s\n%*s{  ",
                        t->tag == def_TYPE_STRUCT? "struct":
                        t->tag == def_TYPE_UNION?  "union": "list",
                        var? " \\": "",
                        3*(nest + 1), ""
                     )
            )
            < 0
         )
            goto finish;

         // variable-size data structure ending in ellipsis
         if (t->tag == def_TYPE_STRUCT && t->data AS list.base)
         {
            char base [def_ID_LIMIT + 1];
            def_as_macro (base,  t->data AS list.base->data AS id);
            if ((rc = fprintf (file, "%s_MEMBERS%s\n%*s   ",
                  base, var? " \\": "", 3*(nest + 1), "")) < 0)
               goto finish;
         }
         for (i = 0; i < t->data AS list.count; i++)
         {
            if (i == t->data AS list.count - 1 &&
                  t->tag == def_TYPE_STRUCT &&
                  t->data AS list.ellipsis)
            {
               char v1 [def_ID_LIMIT + 1];

               if ((rc = sprintf (v1, t->data AS list.members [i]->name [0]
                     == '*'? "(%s) [%s]": "%s [%s]",
                     t->data AS list.members [i]->name,
                     !var? "UNKNOWN": "N")) < 0)
                  goto finish;

               if ((rc = Print_Decl (file, t->data AS list.members [i],
                     NULL, v1, var, nest + 1)) < 0)
                  goto finish;
            }
            else
               if ((rc = Print_Decl (file, t->data AS list.members [i],
                     NULL, t->data AS list.members [i]->name, var,
                     nest + 1)) < 0)
                  goto finish;

            if ((rc = fprintf (file, ";%s\n%*s", var? " \\": "",
                  3*(nest + 1), "")) < 0)
               goto finish;

            if (i != t->data AS list.count - 1)
               if ((rc = fprintf (file, "   ")) < 0)
                  goto finish;
         }

         if (v != NULL)
         {
            if ((rc = fprintf (file, "}%s\n%*s%s", var? " \\": "",
                  3*(nest + 1), "", v)) < 0)
               goto finish;
         }
         else
         {
            if ((rc = fprintf (file, "}")) < 0)
               goto finish;
         }
      }
      break;

      case def_TYPE_ROW:
      {
         char v1 [def_ID_LIMIT + 1], v2 [MAX (DEC_WIDTH + 1, 8)];

         if (t->data AS row.count == 1)
            strcpy (v2, "UNKNOWN");
         else
            if ((rc = sprintf (v2, "%d", t->data AS row.count)) < 0)
               goto finish;

         if (v == NULL || v [0] == '/')
         {
            if (v != NULL)
            {
               if ((rc = sprintf (v1, "%s [%s]", v, v2)) < 0)
                  goto finish;
            }
            else
            {
               if ((rc = sprintf (v1, "[%s]", v2)) < 0)
                  goto finish;
            }
         }
         else
         {
            if ((rc = sprintf (v1, v [0] == '*'? "(%s) [%s]": "%s [%s]",
                  v, v2)) < 0)
               goto finish;
         }

         if ((rc = Print_Decl (file, t->data AS row.base, NULL, v1, var,
               nest + 1)) < 0)
            goto finish;
      }
      break;

      case def_TYPE_VOID:
         if ((rc = fprintf (file, v == NULL? "void": "void %s", v)) < 0)
            goto finish;
      break;

      case def_TYPE_ID:
      {
         char c_name [def_ID_LIMIT + 1];

         def_as_extern (c_name, t->data AS id);

         if ((rc = fprintf (file, v == NULL? "%s": "%s %s", c_name, v)) <
               0)
            goto finish;
      }
      break;

      default:
      	break;
   }

finish:
   return rc;
}
/*-----------------------------------------------------------------------*/
os_error *cheader_output
(
   FILE     *file,
   char     *title,
   char     *author,
   lookup_t  needses,
   lookup_t  needsatends,
   lookup_t  consts,
   lookup_t  types,
   lookup_t  swis,
   osbool    c_plus_plus
)
{
   os_error *error = NULL;
   char     *needs, *needsatend, *cnst, *type, *swi;
   def_c     c;
   def_t     t;
   def_s     s;
   char      c_name [def_ID_LIMIT + 1];
   void     *context;
   int       i, rc = 0;
   time_t    now;
   osbool    start;

   /*Emit the TITLE and AUTHOR declarations.*/
   def_as_extern (c_name, title);
   if
   (
      (  rc =
         fprintf
         (
            file,
            "#ifndef %s_H\n"
            "#define %s_H\n\n"
            "/* C%s header file for %s\n"
            " * Written by DefMod (%s) on %s"
            " * %s\n"
            " */\n\n",
            c_name,
            c_name,
            (c_plus_plus) ? "++" : "",
            title,
            __DATE__,
            ctime ((time (&now), &now)),
            author
         )
      )
      < 0
   )
      goto finish;

   /*Emit OSLib licence for produced headers and binaries.*/
   {
      osbool firstLine = TRUE;
      const char *licence = oslib_license;
      while (*licence != '\0')
      {
         const char *nextLine;
         for (nextLine = licence; *nextLine != '\n'; ++nextLine)
            /* */;
         ++nextLine;
         if ((rc = fprintf (file, firstLine ? "/* " : " * ")) < 0)
            goto finish;
         firstLine = FALSE;
         if ((rc = fprintf (file, "%.*s", (int)(nextLine - licence), licence)) < 0)
            goto finish;
         licence = nextLine;
      }
      if (!firstLine)
         if ((rc = fprintf (file, " */\n\n")) < 0)
            goto finish;
   }

   /*Make sure we have "types.h".*/
   if ((rc = fprintf (file,
         "#ifndef types_H\n"
         "#include \"oslib/types.h\"\n" /*no spaces for GCC etc. 30th Jan 1995*/
                                        /* OsLib path: 4 May 2000 */
                                        /* oslib subdirectory TV 20000930 */
         "#endif\n\n")) < 0)
      goto finish;

#if 0 /* not everything needs os.h */
   /*Make sure we have "os.h".*/
   if ((rc = fprintf (file,
         "#ifndef os_H\n"
         "#include \"oslib/os.h\"\n"
	 "#endif\n\n")) < 0)
      goto finish;
#endif

   /*Emit the NEEDS declarations.*/
   context = 0;
   while (TRUE)
   {
      if ((error = lookup_enumerate (needses, &needs, NULL, &context)) !=
            NULL)
         goto finish;

      if (context == 0) break;

      def_as_extern (c_name, needs);
      if ((rc = fprintf (file,
            "#ifndef %s_H\n"
            "#include \"oslib/%s.h\"\n" /*no spaces for GCC etc. 30th Jan 1995*/
                                        /* OsLib path: 4 May 2000 */
                                        /* oslib subdirectory TV 20000930 */
            "#endif\n\n",
            c_name, c_name)) < 0)
         goto finish;
   }

   /* Emit the C++ namespace declaration */
   /* TV 20000529 */
   if (
         (  rc = fprintf(  file,
                           "#if defined NAMESPACE_OSLIB\n"
                           "  namespace OSLib {\n"
                           "#endif\n\n"
                        )
         ) < 0
      )
      goto finish;

   /*Emit the constant declarations derived from the starred-swi parts.*/
   start = TRUE;
   context = 0;
   while (TRUE)
   {
      if ((error = lookup_enumerate (swis, &swi, (void **) &s, &context))
            != NULL)
         goto finish;
      if (context == 0) break;

      for (i = 0; i < 10; i++)
         if (s->starred_constants [i])
         {
            /* SWI reason code.  */
            if (start)
            {
               if ((rc = fprintf (file,
                     "/**********************************\n"
                     " * SWI names and SWI reason codes *\n"
                     " **********************************/\n"))
                     < 0)
                  goto finish;
               start = FALSE;
            }

            if ((rc = fprintf (file, "#undef  %s\n#define %-39s 0x%X\n",
                  swi, swi, s->constants [i])) < 0)
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
         if (start)
         {
            if ((rc = fprintf (file,
                  "/**********************************\n"
                  " * SWI names and SWI reason codes *\n"
                  " **********************************/\n"))
                  < 0)
               goto finish;
            start = FALSE;
         }

         /* SWI number.  */
         if ((rc = fprintf (file, "#undef  %s\n#define %-39s 0x%X\n", swi,
               swi, s->swi)) < 0 ||
               (rc = fprintf (file, "#undef  X%s\n#define X%-38s 0x%X\n",
               swi, swi, s->swi | 1 << 17)) < 0)
            goto finish;
      }
   }

   if (!start)
      if ((rc = fprintf (file, "\n")) < 0)
          goto finish;

   /*Emit the TYPE declarations. First, do |struct| forward references, so
      that we can refer to these things before they are defined fully.*/
   start = TRUE;
   context = 0;
   while (TRUE)
   {
      if ((error = lookup_enumerate (types, &type, (void **) &t,
            &context)) != NULL)
         goto finish;
      if (context == 0) break;

      if (t == NULL || t->tag == def_TYPE_STRUCT ||
            t->tag == def_TYPE_UNION || t->tag == def_TYPE_ABSTRACT)
      {
         if (start)
         {
            if ((rc = fprintf (file,
                  "/************************************\n"
                  " * Structure and union declarations *\n"
                  " ************************************/\n")) < 0)
               goto finish;
            start = FALSE;
         }

         def_as_extern (c_name, type);

         if (t == NULL || t->tag == def_TYPE_ABSTRACT)
         /* typedef abstract struct */
         {
            char c_name_ [def_ID_LIMIT + 1];

            sprintf (c_name_, "%s_", c_name);
            /* FIXME: Is '_' bit really needed, useful ? If not, drop c_name_ as well. */
            if ((rc = fprintf (file, "typedef struct %-32s *%s;\n",
                  c_name_, c_name)) < 0)
               goto finish;
         }
         else // typdef struct, union, or list
         {
            if (!c_plus_plus)
            {
               if ((rc = fprintf (file, "typedef %s %-32s %s;\n",
                     t->tag == def_TYPE_STRUCT? "struct":
                     t->tag == def_TYPE_UNION?  "union ": "list",
                     c_name, c_name)) < 0)
                  goto finish;

               if (  t -> tag == def_TYPE_STRUCT
                     && t -> data AS list.ellipsis
                  )
               {
                  def_as_extern (c_name, type);
                  strcat( c_name, "_base" );

                  if (  (  rc = fprintf(  file,
                                          "typedef %s %-32s %s;\n",
                                          t->tag == def_TYPE_STRUCT?
                                             "struct":
                                             t->tag == def_TYPE_UNION?
                                                "union ":
                                                "list",
                                          c_name,
                                          c_name
                                       )
                        ) < 0
                     )
                     goto finish;
               }
            }
            else
            {
               if ((rc = fprintf (file, "%-32s %s;\n",
                     t->tag == def_TYPE_STRUCT? "struct":
                     t->tag == def_TYPE_UNION?  "union ": "list",
                     c_name)) < 0)
                  goto finish;
            }
         }

         if (t != NULL && t->tag == def_TYPE_ABSTRACT &&
               t->description != NULL)
            if ((rc = fprintf (file, "      /*%s*/\n",
                  t->description)) < 0)
               goto finish;
      }
   }

   if (!start)
      if ((rc = fprintf (file, "\n")) < 0)
         goto finish;

   /* Emit non-abstract Type definitions */
   start = TRUE;
   context = 0;
   while (TRUE)
   {
      if ((error = lookup_enumerate (types, &type, (void **) &t, &context))
            != NULL)
         goto finish;
      if (context == 0) break;

      if (!(t == NULL || t->tag == def_TYPE_ABSTRACT))
      /* valid type definition */
      {
         if (start) /* header */
         {
            if ((rc = fprintf (file,
                  "/********************\n"
                  " * Type definitions *\n"
                  " ********************/\n")) < 0)
               goto finish;
            start = FALSE;
         }  /* end header */

         if (t->tag == def_TYPE_STRUCT || t->tag == def_TYPE_UNION)
         /* structure or union definition */
         {
            def_as_extern (c_name, type);

            if (t->description != NULL)
            {
               /*Start the description part.*/
               osbool first;

               if ((rc = fprintf (file,
                     "/* -----------------------------------"
                     "-------------------------------------\n")) < 0)
                  goto finish;

               if ((rc = fprintf (file,
                     " * Type:          %s\n"
                     " *\n"
                     " * Description:   %s\n",
                     c_name,
                     t->description)) < 0)
                  goto finish;

               first = TRUE;
               for (i = 0; i < t->data AS list.count; i++)
                  if (t->data AS list.members [i]->description != NULL)
                  {
                     if (first)
                        if ((rc = fprintf (file, " *\n")) < 0)
                           goto finish;

                     if ((rc = fprintf (file, " * %-14s %s - %s\n",
                           first? "Fields:": "",
                           t->data AS list.members [i]->name,
                           t->data AS list.members [i]->description))
                           < 0)
                        goto finish;

                     first = FALSE;
                  }

               /*Finish it off.*/
               if ((rc = fprintf (file, " */\n\n")) < 0)
                  goto finish;
            } /* End Description */

            if (t->tag == def_TYPE_STRUCT && t->data AS list.ellipsis &&
                  t->data AS list.count > 1)
            /* variable-size structure definition containing ellipsis */
            {
               char macro_name [def_ID_LIMIT + 1];
               int i;

               // create the macro name
               def_as_macro (macro_name, type);

               /* start the macro definition line */
               if ((rc = fprintf (file, "#define %s_MEMBERS",
                     macro_name)) < 0)
                  goto finish;

               if (t->data AS list.base)
               {
                  /* the macro is based upon another macro */
                  char base [def_ID_LIMIT + 1];
                  def_as_macro (base,  t->data AS list.base->data AS id);
                  if ((rc = fprintf (file, " \\\n   %s_MEMBERS", base)) < 0)
                     goto finish;
               }

               /* write each structure member to the macro */
               for (i = 0; i < t->data AS list.count - 1; i++)
               {
                  if ((rc = fprintf (file, " \\\n   ")) < 0)
                     goto finish;
                  if ((rc = Print_Decl (file, t->data AS list.members [i],
                        NULL, t->data AS list.members [i]->name, TRUE, 0)) < 0)
                     goto finish;
                  if ((rc = fprintf (file, ";")) < 0)
                     goto finish;
               }

               /* emit a definition of the base structure (TV 20001021) */
               if ( (rc = fprintf( file,
                                  "\n\n/* Base %s structure without variable part */"
                                  "\nstruct %s_base\n   {  %s_MEMBERS\n   };",
                                  c_name, c_name, macro_name
                                 )
                    ) < 0
                  )
                  goto finish;

               /* emit a structure definition consisting of the macro
               ** followed by an UNKNOWN array of the repeated member */
               if ((rc = fprintf (file,
                      "\n\n/* legacy structure */"
                      "\nstruct %s\n   {  %s_MEMBERS\n      ",
                     c_name, macro_name)) < 0)
                  goto finish;
               if ((rc = Print_Decl (file, t->data AS list.members [i],
                     NULL, t->data AS list.members [i]->name, TRUE, 0)) < 0)
                  goto finish;
               if ((rc = fprintf (file, " [UNKNOWN];\n   }")) < 0)
                  goto finish;

            } /* end variable-size structure definition */
            else
               if ((rc = Print_Decl (file, t, c_name, NULL, FALSE, 0)) < 0)
                  goto finish;
            if ((rc = fprintf (file, ";\n\n")) < 0)
               goto finish;

            /*Emit a macro to define a type of arbitrarily many of the
               repeating field, iff the repeating field is of fixed width
               (i e, def_VALUE_REGISTER or def_VALUE_FIXED).*/
            if (t->tag == def_TYPE_STRUCT && t->data AS list.ellipsis &&
                  t->data AS list.members
                  [t->data AS list.count - 1]->value != def_VALUE_VARIABLE)
            {
               char macro_name [def_ID_LIMIT + 1];
               int i = t->data AS list.count - 1;
               char *suffix;

               def_as_macro (macro_name, type);

               if ((rc = fprintf (file, "#define %s(N) \\\n   ",
                     macro_name)) < 0)
                  goto finish;

               if ((rc = fprintf (file, "struct \\\n      {  ")) < 0)
                  goto finish;
               if (t->data AS list.count > 1)
                  if ((rc = fprintf (file, "%s_MEMBERS \\\n         ",
                        macro_name)) < 0)
                     goto finish;
               if ((rc = Print_Decl (file, t->data AS list.members [i],
                     NULL, t->data AS list.members [i]->name, TRUE, 0)) < 0)
                  goto finish;
               if ((rc = fprintf (file, " [N]; \\\n      }")) < 0)
                  goto finish;

               if ((rc = fprintf (file, "\n\n")) < 0)
                  goto finish;

               /*Also emit a macro to calculate the size of such a type.*/
               if ((suffix = strchr (macro_name, '_')) != NULL)
               {
                  if ((rc = fprintf (file,
                        "#define %.*s_SIZEOF%s(N) \\\n   (",
                        (int)(suffix - macro_name), macro_name, suffix)) < 0)
                     goto finish;
               }
               else
               {
                  if ((rc = fprintf (file, "#define %s_SIZEOF(N) (",
                        macro_name)) < 0)
                     goto finish;
               }

               /*This calculates an offset (using |offsetof|).*/
               if ((i = t->data AS list.count) > 1)
               {
                  if ((rc = fprintf (file,
                        "offsetof (%s, %s) + \\\n"
                        "         (N)*sizeof ((%s *) NULL)->%s",
                        c_name, t->data AS list.members [i - 1]->name,
                        c_name, t->data AS list.members [i - 1]->name)) <
                        0)
                     goto finish;
               }
               else
               {
                  if ((rc = fprintf (file, "(N)*sizeof ((%s *) NULL)->%s",
                        c_name, t->data AS list.members [0]->name)) < 0)
                     goto finish;
               }

               if ((rc = fprintf (file, ")\n\n")) < 0)
                  goto finish;
            }
         }
         else
         {
            def_as_extern (c_name, type);

            if ((rc = fprintf (file, "typedef ")) < 0)
               goto finish;

            if ((rc = Print_Decl (file, t, NULL, c_name, FALSE, 0)) < 0)
               goto finish;

            if ((rc = fprintf (file, ";\n")) < 0)
               goto finish;

            if (t->description != NULL)
               if ((rc = fprintf (file, "      /*%s*/\n",
                     t->description)) < 0)
                  goto finish;

            if ((rc = fprintf (file, "\n")) < 0)
               goto finish;
         }
      } /* end valid type definition */
   }

   /*Emit the CONST declarations.*/
   start = TRUE;
   context = 0;
   while (TRUE)
   {
      if ((error = lookup_enumerate (consts, &cnst, (void **) &c,
            &context)) != NULL)
         goto finish;
      if (context == 0) break;

      if (start)
      {
         if ((rc = fprintf (file,
               "/************************\n"
               " * Constant definitions *\n"
               " ************************/\n")) < 0)
            goto finish;
         start = FALSE;
      }

      def_as_macro (c_name, cnst);
      if (!c_plus_plus)
      {
         if ((rc = fprintf (file, "#define %-39s ", c_name)) < 0)
            goto finish;
      }
      else
      {
         char v [def_ID_LIMIT + 1];

         if ((rc = sprintf (v, "const %s", c_name)) < 0)
            goto finish;

         if ((rc = Print_Decl (file, c->type, NULL, v, FALSE, 0)) < 0)
            goto finish;

         if ((rc = fprintf (file, " = ")) < 0)
            goto finish;
      }

      switch (c->type->tag)
      {
         case def_TYPE_INT:
            if ((rc = fprintf (file, "(%d)", c->value)) < 0)
               goto finish;
         break;

         case def_TYPE_SHORT:
            if ((rc = fprintf (file, "((short) %d)", c->value)) < 0)
               goto finish;
         break;

         case def_TYPE_BYTE:
            if ((rc = fprintf (file, "((byte) %d)", c->value)) < 0)
               goto finish;
         break;

         case def_TYPE_CHAR:
            if (isprint (c->value))
            {
               if ((rc = fprintf (file, "((char) '%c')", c->value)) < 0)
                  goto finish;
            }
            else
            {
               if ((rc = fprintf (file, "((char) '\\x%.2X')", c->value))
                     < 0)
                  goto finish;
            }
         break;

         case def_TYPE_BITS:
            if ((rc = fprintf (file, "0x%Xu", c->value)) < 0)
               goto finish;
         break;

         case def_TYPE_BOOL:
            if ((rc = fprintf (file, "%s", WHETHER (c->value))) < 0)
               goto finish;
         break;

         default:
            if ((rc = fprintf (file, "((")) < 0)
               goto finish;

            if ((rc = Print_Decl (file, c->type, NULL, NULL, FALSE,
                  0)) < 0)
               goto finish;

            if ((rc = fprintf (file, ") 0x%Xu)", c->value)) < 0)
                  /*was %d in either case 19th Apr 1995*/
               goto finish;
         break;
      }

      if ((rc = fprintf (file, (!c_plus_plus) ? "\n" : ";\n")) < 0)
         goto finish;

      if (c->description != NULL)
         if ((rc = fprintf (file, "      /*%s*/\n",
               c->description)) < 0)
            goto finish;
   }

   if (!start)
      if ((rc = fprintf (file, "\n")) < 0)
         goto finish;

   /*Emit the SWI declarations.*/
   start = TRUE;
   context = 0;
   while (TRUE)
   {
      osbool nonx, first;

      if ((error = lookup_enumerate (swis, &swi, (void **) &s, &context))
            != NULL)
         goto finish;
      if (context == 0) break;

      if (!s->absent)
      {
         int result = s->value != NONE? def_bit_index (s->value, 0): -1;
            /*number of register returned as result*/

         if (start)
         {
            if ((rc = fprintf (file,
                     "/*************************\n"
                     " * Function declarations *\n"
                     " *************************/\n\n")) < 0)
                  goto finish;

            /*Allow this C header file to be used from C++.*/
            if (!c_plus_plus)
               if ((rc = fprintf (file,
                     "#ifdef __cplusplus\n   extern \"C\" {\n#endif\n\n"))
                     < 0)
                  goto finish;

            start = FALSE;
         }

         def_as_extern (c_name, swi);

         /*Start the description part.*/
         if ((rc = fprintf (file,
               "/* -----------------------------------"
               "-------------------------------------\n"
               " * Function:      %s()\n"
               " *\n"
               " * Description:  ", c_name)) < 0)
            goto finish;

         if (s->description != NULL)
         {
            int   L, col;
            const char *p = s->description;

            assert(s->description[0]);
            L = strsplen (p);

            col = 17;

            do
            {
               if ((rc = fprintf (file, " %.*s", L, p)) < 0)
                  goto finish;
               col += L + 1, p += L; /*p -> null or space*/

               if (*p == ' ')
               {
                  p++; /*p -> next word to print, and there is one*/
                  L = strsplen (p);
                  if (col + 1 + L > 75)
                  {
                     if ((rc = fprintf (file, "\n *               ")) < 0)
                        goto finish;
                     col = 17;
                  }
               }
            }
            while (*p != '\0');

            if ((rc = fprintf (file, "\n")) < 0)
               goto finish;
         }
         else
         {
            for (i = 0; i < 10; i++)
               if (s->starred_constants [i])
               {
                  if ((rc = fprintf (file,
                        " Calls reason code %d of SWI 0x%X\n",
                        s->constants [i], s->swi)) < 0)
                     goto finish;
                  break;
               }
            /* When we don't have an explicit description, we have to
               invent one. If it is a starred_constant one, we can make
               one up using reason code, either we go for a very
               generic one based on SWI number.  */
            if (i == 10)
            {
               if ((rc = fprintf (file, " Calls SWI 0x%X\n", s->swi)) < 0)
                  goto finish;
            }
         }

         if (!def_using_block (s))
         {
            first = TRUE;
            for (i = 0; i < 10; i++)
               if ((s->i & 1 << i) != NONE)
               {
                  if (first)
                     if ((rc = fprintf (file, " *\n")) < 0)
                        goto finish;

                  if (s->inputs [i]->description != NULL)
                  {
                     if ((rc = fprintf (file, " * %-14s %s - %s\n",
                           first? "Input:": "",
                           s->inputs [i]->name,
                           s->inputs [i]->description)) < 0)
                        goto finish;
                  }
                  else
                  {
                     if ((rc = fprintf (file,
                           " * %-14s %s - value of R%d on entry\n",
                           first? "Input:": "",
                           s->inputs [i]->name, i)) < 0)
                        goto finish;
                  }

                  first = FALSE;
               }

            first = TRUE;
            for (i = 0; i < 10; i++)
               if ((s->o & 1 << i) != NONE)
               {
                  if (first)
                     if ((rc = fprintf (file, " *\n")) < 0)
                        goto finish;

                  if (s->outputs [i]->description != NULL)
                  {
                     if ((rc = fprintf (file, " * %-14s %s - %s%s\n",
                           first? "Output:": "",
                           s->outputs [i]->name,
                           s->outputs [i]->description,
                           i == result? " (X version only)": "")) < 0)
                        goto finish;
                  }
                  else
                  {
                     if ((rc = fprintf (file,
                           " * %-14s %s - value of R%d on exit%s\n",
                           first? "Output:": "",
                           s->outputs [i]->name, i,
                           i == result? " (X version only)": "")) < 0)
                        goto finish;
                  }

                  first = FALSE;
               }

            if (s->f_out)
            {
               if (first)
                  if ((rc = fprintf (file, " *\n")) < 0)
                     goto finish;

               if ((rc = fprintf (file,
                     " * %-14s %s - processor status register on exit%s\n",
                     first? "Output:": "", "psr",
                     def_FLAGS == result? " (X version only)": "")) < 0)
                  goto finish;
            }
         }
         else
         {
            /*First locate the register pointing to the block.*/
            first = TRUE;
            for (i = 0; i < 10; i++)
               if ((s->i & 1 << i) != NONE)
               {
                  int cpt;

                  for (cpt = 0; cpt < s->inputs [i]->data AS list.count;
                        cpt++)
                  {
                     if (first)
                        if ((rc = fprintf (file, " *\n")) < 0)
                           goto finish;

                     if ((rc = fprintf (file,
                           " * %-14s %s - component %d\n",
                           first? "Input:": "", s->inputs [i]->data AS
                           list.members [cpt]->name, cpt)) < 0)
                        goto finish;

                     first = FALSE;
                  }

                  break;
               }
         }

         if (s->value != NONE)
         {
            char name [10];

            if ((rc = fprintf (file, " *\n")) < 0)
               goto finish;

            if ((rc = sprintf (name, result != def_FLAGS? "R%d": "psr",
                  result)) < 0)
               goto finish;

            if ((rc = fprintf (file,
                  " * %-14s %s (non-X version only)\n",
                  "Returns:", name)) < 0)
               goto finish;
         }

         first = TRUE;
         for (i = 0; i < 10; i++)
         {
            if ((s->k & 1 << i) != NONE)
            {
               char *op = (s->i & 1 << i) == NONE? "=": Op (s->op [i]);

               if (first)
               {
                  if ((rc = fprintf (file, " *\n")) < 0)
                     goto finish;

                  if (s->description != NULL &&
                        strlen (s->description) != 0)
                  {
                     if ((rc = fprintf (file,
                           " * %-14s Calls SWI 0x%X with R%d %s 0x%X",
                           "Other notes:", s->swi, i, op,
                           s->constants [i])) < 0)
                        goto finish;
                  }
                  else
                  {
                     if ((rc = fprintf (file,
                           " * %-14s Before entry, R%d %s 0x%X",
                           "Other notes:", i, op,
                           s->constants [i])) < 0)
                        goto finish;
                  }
                  first = FALSE;
               }
               else
               {
                  if ((rc = fprintf (file, ", R%d %s 0x%X", i,
                        op, s->constants [i])) < 0)
                     goto finish;
               }
            }
         }

         if (!first)
         {
            if ((rc = fprintf (file, ".\n")) < 0)
               goto finish;
         }
         else if (s->description != NULL &&
               strlen (s->description) != 0)
         {
            if ((rc = fprintf (file, " *\n")) < 0)
               goto finish;

            if ((rc = fprintf (file, " * %-14s Calls SWI 0x%X.\n",
                  "Other notes:", s->swi)) < 0)
               goto finish;
         }

         if ((rc = fprintf (file, " */\n\n")) < 0)
            goto finish;

         /*Now write the two declarations of the function.*/
         for (nonx = FALSE; nonx <= TRUE; nonx++)
         {
            def_as_extern (c_name + (!nonx? 1: 0), swi);
            if (!nonx) c_name [0] = 'x';

            first = TRUE;
            if (!nonx)
            {
               if ((rc = fprintf (file, "extern os_error *%s (", c_name)) <
                     0)
                  goto finish;
            }
            else
            {
               if (s->value == NONE)
               {
                  /*No return value.*/
                  if (def_inlinable (s))
                  {
                     if ((rc = fprintf (file, "__swi (0x%X) "
                           "void %s (", s->swi, c_name)) < 0)
                        goto finish;
                  }
                  else
                  {
                     if ((rc = fprintf (file, "extern void %s (",
                           c_name)) < 0)
                        goto finish;
                  }
               }
               else if (s->value == 1 << def_FLAGS)
               {
                  /*Return value is |bits|. Can never be inlinable.*/
                  if ((rc = fprintf (file, "extern bits %s (", c_name)) <
                        0)
                     goto finish;
               }
               else
               {
                  /*Return value of type |s->outputs [r]|.*/
                  if (def_inlinable (s))
                  {
                     if ((rc = fprintf (file, "__swi (0x%X) ",
                           s->swi)) < 0)
                        goto finish;
                  }
                  else
                  {
                     if ((rc = fprintf (file, "extern ")) < 0)
                        goto finish;
                  }

                  if ((s->ro & s->value) != NONE)
                     /*Prepend a star.*/
                     memmove (c_name + 1, c_name, strlen (c_name) + 1),
                     c_name [0] = '*';

                  if ((rc = Print_Decl (file, s->outputs [result], NULL,
                        c_name, FALSE, 0)) < 0)
                     goto finish;

                  if ((rc = fprintf (file, " (")) < 0)
                     goto finish;
               }
            }

            if (!def_using_block (s))
            {
               for (i = 0; i < 10; i++)
                  if ((s->i & 1 << i) != 0)
                  {
                     char arg_name [def_ID_LIMIT + 1];

                     if (!first)
                     {
                        if ((rc = fprintf (file, ",\n      ")) < 0)
                           goto finish;
                     }
                     else
                        first = FALSE;

                     if ((s->ri & 1 << i) == 0)
                     {
                        if ((rc = sprintf (arg_name, "%s",
                              s->inputs [i]->name)) < 0)
                           goto finish;
                     }
                     else
                     {
                        if ((rc = sprintf (arg_name,
                              "const *%s" /*was *%s 2nd Mar 1994*/
                              /*was (%s) 2nd Mar 1994*/,
                              s->inputs [i]->name)) < 0)
                           goto finish;
                     }

                     if ((rc = Print_Decl (file, s->inputs [i], NULL,
                           arg_name, FALSE, 0)) < 0)
                        goto finish;
                  }

               for (i = 0; i < 10; i++)
                  if ((s->o & 1 << i) != 0 && (!nonx || i != result))
                  {
                     char arg_name [def_ID_LIMIT + 1];

                     if (!first)
                     {
                        if ((rc = fprintf (file, ",\n      ")) < 0)
                           goto finish;
                     }
                     else
                        first = FALSE;

                     if ((s->ro & 1 << i) == 0)
                     {
                        if ((rc = sprintf (arg_name, "%c%s",
                              !c_plus_plus? '*': '&',
                              s->outputs [i]->name)) < 0)
                           goto finish;
                     }
                     else
                     {
                        if ((rc = sprintf (arg_name, "*%c%s" /*was *(%c%s)
                              2nd Mar 1994*/, !c_plus_plus? '*': '&',
                              s->outputs [i]->name)) < 0)
                           goto finish;
                     }

                     if ((rc = Print_Decl (file, s->outputs [i], NULL,
                           arg_name, FALSE, 0)) < 0)
                        goto finish;
                  }

               if (s->f_out && (!nonx || def_FLAGS != result))
               {
                  struct def_t t;

                  if (!first)
                  {
                     if ((rc = fprintf (file, ",\n      ")) < 0)
                        goto finish;
                  }
                  else
                     first = FALSE;

                  t.tag = def_TYPE_BITS;
                  if ((rc = Print_Decl (file, &t, NULL,
                        !c_plus_plus? "*psr": "&psr", FALSE, 0)) < 0)
                     goto finish;
               }
            }
            else
            {
               /*First locate the register pointing to the block.*/
               for (i = 0; i < 10; i++)
                  if ((s->i & 1 << i) != 0)
                  {
                     int cpt;

                     for (cpt = 0; cpt < s->inputs [i]->data AS list.count;
                           cpt++)
                     {
                        if (!first)
                        {  if ((rc = fprintf (file, ",\n      ")) < 0)
                              goto finish;
                        }
                        else
                           first = FALSE;

                        if ((rc = Print_Decl (file,
                              s->inputs [i]->data AS list.members [cpt],
                              NULL, s->inputs [i]->data AS list.members
                              [cpt]->name, FALSE, 0)) < 0)
                           goto finish;
                     }

                     break;
                  }
            }

            if ((rc = fprintf (file, first? "void);\n": ");\n")) < 0)
               goto finish;
         }

         if ((rc = fprintf (file, "\n")) < 0)
            goto finish;
      }
   }

   if (!start)
   {
      /*Allow this C header file to be used from C++.*/
      if (!c_plus_plus)
      {
         if ((rc = fprintf (file,
               "#ifdef __cplusplus\n   }\n#endif\n\n")) < 0)
            goto finish;
      }
   }

   /* end namespace declaration */
   /* TV 20000529 */
   if (
         (  rc = fprintf ( file,
                           "#if defined NAMESPACE_OSLIB\n"
                           "  } \n"
                           "#endif\n\n"
                         )
         ) < 0
      )
      goto finish;


   /*Emit the NEEDSATEND declarations.*/
   context = 0;
   while (TRUE)
   {
      if ((error = lookup_enumerate (needsatends, &needsatend, NULL, &context)) !=
            NULL)
         goto finish;

      if (context == 0) break;

      def_as_extern (c_name, needsatend);
      if ((rc = fprintf (file,
            "#ifndef %s_H\n"
            "#include \"oslib/%s.h\"\n" /*no spaces for GCC etc. 30th Jan 1995*/
                                        /* OsLib path: 4 May 2000 */
                                        /* oslib subdirectory TV 20000930 */
            "#endif\n\n",
            c_name, c_name)) < 0)
         goto finish;
   }


   /* final #endif statement */
   if ((rc = fprintf (file, "#endif\n")) < 0)
      goto finish;

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror ();                /* TV 980115 */
   return error;
}
