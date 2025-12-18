/*hdr.c - output an assembler header file given a module defn*/

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
#ifndef TRACE
#define TRACE 0
#endif

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "kernel.h"      /* TV 980115 */

#include "oslib/macros.h"
#include "oslib/os.h"
#include "oslib/types.h"

#include "lookup.h"
#include "trace.h"

#include "assembler.h"
#include "def.h"
#include "hdr.h"
#include "oslib_licence.h"

#undef NEEDSATEND

#define TAB 8
#define LABELLED_INST_COL 4
#define LABELLED_ARG_COL 5

#define UNLABELLED_INST_COL 1
#define UNLABELLED_ARG_COL  2

static const char * const Op [] = {":OR:", ":AND:", "+", ":EOR:"};

/* This gives the condition when to use the ':' for marking the end of an
   ELF label.  */
#define NeedELFLabel (label != NULL && GASSyntax)
/* Comment start character.  */
#define COMMENTCHAR ((GASSyntax) ? '@' : ';')

/* emit an instruction - optionally with label */
static int Emit
(
   FILE *file,
   const char *label,
   const char *inst
)
{
   return fprintf (file,
     NeedELFLabel ? "%-*s: %s\n" : "%-*s %s\n",
     (label != NULL) ? LABELLED_INST_COL*TAB - 1 : UNLABELLED_INST_COL*TAB - 1,
     (label != NULL) ? label : "",
     inst);
}


/* emit an instruction with a single string argument - optionally with label */
static int EmitS
(
   FILE *file,
   const char *label,
   const char *inst,
   const char *s
)
{
   return fprintf (file,
     NeedELFLabel ? "%-*s: %-*s %s\n" : "%-*s %-*s %s\n",
     (label != NULL) ? LABELLED_INST_COL*TAB - 1 : UNLABELLED_INST_COL*TAB - 1,
     (label != NULL) ? label : "",
     (label != NULL) ? (LABELLED_ARG_COL - LABELLED_INST_COL)*TAB - 1 : (UNLABELLED_ARG_COL - UNLABELLED_INST_COL)*TAB - 1,
     inst, s);
}


/* emit an instruction with two string arguments - optionally with label */
static int EmitSS
(
   FILE *file,
   const char *label,
   const char *inst,
   const char *s1,
   const char *s2
)
{
   return fprintf (file, 
     NeedELFLabel ? "%-*s: %-*s %s%s\n" :  "%-*s %-*s %s%s\n",
     (label != NULL) ? LABELLED_INST_COL*TAB - 1 : UNLABELLED_INST_COL*TAB - 1,
     (label != NULL) ? label : "",
     (label != NULL) ? (LABELLED_ARG_COL - LABELLED_INST_COL)*TAB - 1 : (UNLABELLED_ARG_COL - UNLABELLED_INST_COL)*TAB - 1,
     inst, s1, s2);
}


/* emit an instruction with three string arguments - optionally with label */
static int EmitSSS
(
   FILE *file,
   const char *label,
   const char *inst,
   const char *s1,
   const char *s2,
   const char *s3
)
{
   return fprintf (file,
     NeedELFLabel ? "%-*s: %-*s %s%s%s\n" : "%-*s %-*s %s%s%s\n",
     (label != NULL) ? LABELLED_INST_COL*TAB - 1 : UNLABELLED_INST_COL*TAB - 1,
     (label != NULL) ? label : "",
     (label != NULL) ? (LABELLED_ARG_COL - LABELLED_INST_COL)*TAB - 1 : (UNLABELLED_ARG_COL - UNLABELLED_INST_COL)*TAB - 1,
     inst, s1, s2, s3);
}


/* emit an instruction with a decimal numeric argument - optionally with label */
static int EmitD
(
   FILE *file,
   const char *label,
   const char *inst,
   int d
)
{
   return fprintf (file,
     NeedELFLabel ? "%-*s: %-*s %d\n" : "%-*s %-*s %d\n",
     (label != NULL) ? LABELLED_INST_COL*TAB - 1 : UNLABELLED_INST_COL*TAB - 1,
     (label != NULL) ? label : "",
     (label != NULL) ? (LABELLED_ARG_COL - LABELLED_INST_COL)*TAB - 1 : (UNLABELLED_ARG_COL - UNLABELLED_INST_COL)*TAB - 1,
     inst, d);
}


/* emit an instruction with a hex numeric argument - optionally with label */
static int EmitX
(
   FILE *file,
   const char *label,
   const char *inst,
   int x
)
{
   return fprintf (file,
     (GASSyntax) ? ((label != NULL && label[0]) ? "%-*s: %-*s 0x%X\n" : "%-*s %-*s 0x%X\n") : "%-*s %-*s &%X\n",
     (label != NULL) ? LABELLED_INST_COL*TAB - 1 : UNLABELLED_INST_COL*TAB - 1,
     (label != NULL) ? label : "",
     (label != NULL) ? (LABELLED_ARG_COL - LABELLED_INST_COL)*TAB - 1 : (UNLABELLED_ARG_COL - UNLABELLED_INST_COL)*TAB - 1,
     inst, x);
}

/* Prints a declaration of |v| as an object of type |t|.
   called by Emit_Decl
*/
static int Emit_Decl1
(
   FILE  *file,         // output file
   const char *type,    // object type
   const char  *v,      // object name
   osbool top_level,    // if true then equate, else assign storage
   const char *cs       // number of elements in array
)
{
	int  rc = 0;

	tracef( "Emit_Decl1: %s; %s; %s\n" _ v _ type _ top_level?"TRUE":"FALSE" );

	if (top_level)
	{
		if ((rc = EmitD (file, NULL, (GASSyntax) ? ".struct" : "^", 0)) < 0)
			goto finish;
		if ((rc = EmitSS (file, NULL, (GASSyntax) ? ".skip" : "#", cs, type)) < 0)
			goto finish;
		if ((rc = (GASSyntax) ? Emit (file, v, "") : EmitS (file, v, "*", "@")) < 0)
			goto finish;
	}
	else
	{
		if ((rc = EmitSS (file, v, (GASSyntax) ? ".skip" : "#", cs, type)) < 0)
			goto finish;
	}

finish:
	return rc;
}

/* Prints a declaration of |v| as an object of type |t|.*/
static int Emit_Decl
(
   FILE  *file,         // output file
   def_t  t,            // object type
   const char  *v,      // object name
   osbool top_level,    // if true then equate, else assign storage
   int    count         // number of elements in array
)
{
   int  rc = 0;
   char cs [1 + DEC_WIDTH + 1];     // temp char buffer

   // place array size in a char string
   if (count == 0)
      CLEAR (cs);
   else
      sprintf (cs, "%d*", count);

   switch (t->tag)
   {
      case def_TYPE_INT:
	if ((rc =  Emit_Decl1 (file, "Int", v, top_level, cs)) < 0)
            goto finish;
      break;

      case def_TYPE_SHORT:
	if ((rc =  Emit_Decl1 (file, "Short", v, top_level, cs)) < 0)
            goto finish;
      break;

      case def_TYPE_BYTE:
	if ((rc =  Emit_Decl1 (file, "Byte", v, top_level, cs)) < 0)
            goto finish;
      break;

      case def_TYPE_CHAR:
	if ((rc =  Emit_Decl1 (file, "Char", v, top_level, cs)) < 0)
            goto finish;
      break;

      case def_TYPE_BITS:
	if ((rc =  Emit_Decl1 (file, "Bits", v, top_level, cs)) < 0)
            goto finish;
      break;

      case def_TYPE_BOOL:
	if ((rc =  Emit_Decl1 (file, "Bool", v, top_level, cs)) < 0)
            goto finish;
      break;

      case def_TYPE_ASM:
      case def_TYPE_REF:
	if ((rc =  Emit_Decl1 (file, "Ptr", v, top_level, cs)) < 0)
            goto finish;
      break;

      case def_TYPE_STRING:
      case def_TYPE_DATA:
      case def_TYPE_VOID:
         if ((rc = 
               (GASSyntax) ?
                 Emit (file, v, "")
                 :
                 EmitS (file, v, top_level ? "*" : "#", "Skip")) < 0)
            goto finish;
      break;

      // define a structure
      case def_TYPE_STRUCT:
      {
         int i;

         if (top_level)
            if ((rc = EmitD (file, NULL, (GASSyntax) ? ".struct" : "^", 0)) < 0)
               goto finish;

         if (t->data AS list.base)
         {
            // the struct is based upon another
            if (GASSyntax)
            {
               char v1 [def_ID_LIMIT + sizeof("_MEMBERS")-1 + def_ID_LIMIT + sizeof(" Base")-1];
               sprintf (v1, "%s_MEMBERS %s", t->data AS list.base->data AS id, v);
               if ((rc = Emit (file, NULL, v1)) < 0)
                  goto finish;
            }
            else
            {
               char v1 [def_ID_LIMIT + sizeof("_MEMBERS")-1];
               sprintf (v1, "%s_MEMBERS", t->data AS list.base->data AS id);
               if ((rc = Emit (file, v, v1)) < 0)
                  goto finish;
            }
         }

         for (i = 0; i < t->data AS list.count; i++)
         {
            char v1 [def_ID_LIMIT + 1];

            if ((rc = sprintf (v1, "%s_%s", v, t->data AS
                  list.members [i]->name)) < 0)
               goto finish;

	    tracef( "Emit_Decl: %s\n" _ v1 );

            if ((rc = Emit_Decl (file, t->data AS list.members [i], v1,
                  FALSE, 0)) < 0)
               goto finish;
         }

         if (top_level)
            if ((rc = (GASSyntax) ? Emit (file, v, "") : EmitS (file, v, "*", "@")) < 0)
               goto finish;


         // emit a variable-sized structure as a macro
         if (t->data AS list.ellipsis && t->data AS list.count > 1)
         {
            char v1 [def_ID_LIMIT + sizeof("_MEMBERS")-1];
            char v2 [def_ID_LIMIT + sizeof("_MEMBERS")-1 + def_ID_LIMIT + sizeof(" Base")-1];

            // define a macro describing the members
            if (top_level)
            {
               if ((rc = Emit (file, 0, "")) < 0)
                  goto finish;

               if (GASSyntax)
               {
                  sprintf (v1, "%s_MEMBERS label", v);
                  if ((rc = Emit (file, NULL, ".altmacro")) < 0
                      || (rc = EmitS (file, NULL, ".macro", v1)) < 0)
                     goto finish;
               }
               else
               {
                  if ((rc = Emit (file, NULL, "MACRO")) < 0)
                     goto finish;

                  sprintf (v1, "%s_MEMBERS", v);
                  if ((rc = Emit (file, "$label", v1)) < 0)
                     goto finish;
               }
            }

            if (t->data AS list.base)
            {
               // the struct is based upon another
               if (GASSyntax)
               {
                  sprintf (v1, "%s_MEMBERS \\label", t->data AS list.base->data AS id);
                  Emit (file, NULL, v1);
               }
               else
               {
                  sprintf (v1, "%s_MEMBERS", t->data AS list.base->data AS id);
//                Emit (file, v, v1);     // TV 20021128
                  Emit (file, "$label", v1);
               }
            }

            if (top_level)
            {
               for (i = 0; i < t->data AS list.count - 1; i++)
               {
                  if ((rc = sprintf (v1,
                                     (GASSyntax) ? "\\label&_%s" : "$label._%s",
                                     t->data AS list.members [i]->name
                                    )
                      ) < 0)
                     goto finish;

                  // recurse to declare each list member
		  tracef( "Emit_Decl: %s\n" _ v1 );
                  if ((rc = Emit_Decl (file, t->data AS list.members [i], v1, FALSE, 0)) < 0)
                     goto finish;
               }

               // close the macro definition
               if ((rc = Emit (file, NULL, (GASSyntax) ? ".endm" : "MEND")) < 0)
                  goto finish;
            }

            // define the base structure in terms of the macro
            if (top_level)
            {
               if ((rc = sprintf (v1, "%sBase", v)) < 0)
                  goto finish;
               if ((rc = (GASSyntax) ?
                            sprintf (v2, "%s_MEMBERS %sBase", v, v)
                            :
                            sprintf (v2, "%s_MEMBERS", v)) < 0)
                  goto finish;

               if ((rc = Emit (file, NULL, "")) < 0)
                  goto finish;
               if ((rc = EmitD (file, NULL, (GASSyntax) ? ".struct" : "^", 0)) < 0)
                  goto finish;
               if ((rc = Emit (file, (GASSyntax) ? NULL : v1, v2)) < 0)
                  goto finish;
               if ((rc = (GASSyntax) ?
                     Emit (file, v1, "")
                     :
                     EmitS (file, v1, "*", "@")) < 0)
                  goto finish;
            }
         }
      }
      break;

      case def_TYPE_UNION:
      {
         int i;

         if (top_level)
            if ((rc = EmitD (file, NULL, (GASSyntax) ? ".struct" : "^", 0)) < 0)
               goto finish;

         /*Use the union name itself to hold the offset of the start of the
            union.*/
         if (!top_level)
            if ((rc = (GASSyntax) ?
                      Emit (file, v, "")
                      :
                      EmitS (file, v, "*", "@")) < 0)
               goto finish;

         for (i = 0; i < t->data AS list.count; i++)
         {
            char v1 [def_ID_LIMIT + 1];

            if (i > 0)
            {
               /*Set {VAR} back to the origin of the union.*/
               if (!top_level)
               {
                  if ((rc = EmitS (file, NULL, (GASSyntax) ? ".struct" : "^", v)) < 0)
                     goto finish;
               }
               else
               {
                  if ((rc = EmitD (file, NULL, (GASSyntax) ? ".struct" : "^", 0)) < 0)
                     goto finish;
               }
            }

            if ((rc = sprintf (v1, "%s_%s", v, t->data AS
                  list.members [i]->name)) < 0)
               goto finish;

	    tracef( "Emit_Decl: %s\n" _ v1 );
            if ((rc = Emit_Decl (file, t->data AS list.members [i], v1,
                  FALSE, 0)) < 0)
               goto finish;
         }

         if (top_level)
            if ((rc = (GASSyntax) ?
                      Emit (file, v, "")
                      :
                      EmitS (file, v, "*", "@")) < 0)
               goto finish;
      }
      break;

      case def_TYPE_ROW:
	 tracef( "Emit_Decl: %s\n" _ v );
#if 0
         if ((rc = Emit_Decl (file, t->data AS row.base, v, FALSE,
	 			(count == 0? 1: count)*t->data AS row.count
	    )) < 0)
#else
         if ((rc = Emit_Decl (file, t->data AS row.base, v, top_level,
	 			(count == 0? 1: count)*t->data AS row.count
	    )) < 0)
#endif
            goto finish;
      break;

      case def_TYPE_ID:
         if (GASSyntax)
         {
            if ((rc = Emit_Decl1 (file, t->data AS id, v, top_level, "")) < 0)
              goto finish;
         }
         else
         {
            if ((rc = EmitS (file, v, top_level ? "*" : "#", t->data AS id)) < 0)
               goto finish;
         }
      break;

      default:
      	break;
   }

   if (top_level)
      if ((rc = fprintf (file, "\n")) < 0)
         goto finish;

finish:
   return rc;
}

/* Prints a description of an object of type |t|.*/

static int Emit_Name
(
   FILE  *file,
   def_t  t
)
{
   int rc = 0;

   switch (t->tag)
   {
      case def_TYPE_INT:
         if ((rc = fprintf (file, "Int")) < 0)
            goto finish;
      break;

      case def_TYPE_SHORT:
         if ((rc = fprintf (file, "Short")) < 0)
            goto finish;
      break;

      case def_TYPE_BYTE:
         if ((rc = fprintf (file, "Byte")) < 0)
            goto finish;
      break;

      case def_TYPE_CHAR:
         if ((rc = fprintf (file, "Char")) < 0)
            goto finish;
      break;

      case def_TYPE_BITS:
         if ((rc = fprintf (file, "Bits")) < 0)
            goto finish;
      break;

      case def_TYPE_BOOL:
         if ((rc = fprintf (file, "Bool")) < 0)
            goto finish;
      break;

      case def_TYPE_REF:
         if ((rc = fprintf (file, "pointer to ")) < 0)
            goto finish;

         if ((rc = Emit_Name (file, t->data AS ref)) < 0)
            goto finish;
      break;

      case def_TYPE_STRING:
         if ((rc = fprintf (file, "String")) < 0)
            goto finish;
      break;

      case def_TYPE_ASM:
         if ((rc = fprintf (file, "pointer to code")) < 0)
            goto finish;
      break;

      case def_TYPE_DATA:
         if ((rc = fprintf (file, "data")) < 0)
            goto finish;
      break;

      case def_TYPE_VOID:
         if ((rc = fprintf (file, "unspecified")) < 0)
            goto finish;
      break;

      case def_TYPE_STRUCT:
      case def_TYPE_UNION:
      {
         int i;

         if ((rc = fprintf (file, "%s of (",
               t->tag == def_TYPE_STRUCT? "sequence": "one")) < 0)
            goto finish;

         for (i = 0; i < t->data AS list.count; i++)
         {
            if (i > 0)
               if ((rc = fprintf (file, ", ")) < 0)
                  goto finish;

            if ((rc = Emit_Name (file, t->data AS list.members [i])) < 0)
               goto finish;
         }

         if ((rc = fprintf (file, ")")) < 0)
            goto finish;
      }
      break;

      case def_TYPE_ROW:
         if ((rc = fprintf (file, "array of %d ", t->data AS row.count)) < 0)
            goto finish;

         if ((rc = Emit_Name (file, t->data AS row.base)) < 0)
            goto finish;
      break;

      case def_TYPE_ID:
         if ((rc = fprintf (file, "%s", t->data AS id)) < 0)
            goto finish;
      break;

      default:
      	break;
   }

finish:
   return rc;
}

static int Emit_Get (FILE *file, const char *name)
{  int  rc;

   if (GASSyntax)
   {
      if ((rc = fprintf (file, "#  ifndef Get_%s\n"
                               "#    define Get_%s\n"
#ifdef EXECUTE_ON_UNIX
      // FIXME: this is strange, different assembler header output depending on which host
      // these were created ?!
                               "#    include \"oslib/%s.Hdr\"\n"
#else
                               "#    include \"oslib.Hdr.%s\"\n"
#endif
                               "#  endif\n", name, name, name)) < 0)
         goto finish;
   }
   else
   {
      char s [FILENAME_MAX + 1], get [FILENAME_MAX + 1];
      char hdr [FILENAME_MAX + 1];

      if ((rc = sprintf (get, "%s%s", "Get_", name)) < 0)
         goto finish;
      if ((rc = sprintf (hdr, "%s%s", name, "_Hdr")) < 0)
         goto finish;

      if ((rc = EmitSS (file, NULL, "[", ":LNOT: :DEF: ", get)) < 0)
         goto finish;
      if ((rc = EmitS (file, NULL, "GBLS", get)) < 0)
         goto finish;
      if ((rc = Emit (file, NULL, "]")) < 0)
         goto finish;

#ifdef EXECUTE_ON_UNIX
      // FIXME: this is strange, different assembler header output depending on which host
      // these were created ?!
      if ((rc = sprintf (s, "\"GET oslib/%s.Hdr\"", name)) < 0)
#else
      if ((rc = sprintf (s, "\"GET oslib.Hdr.%s\"", name)) < 0)
#endif
         goto finish;

      if ((rc = EmitSS (file, NULL, "[", ":LNOT: :DEF: ", hdr)) < 0)
         goto finish;
      if ((rc = EmitS (file, get, "SETS", s)) < 0)
         goto finish;
      if ((rc = Emit (file, NULL, "|")) < 0)
         goto finish;
      if ((rc = EmitS (file, get, "SETS", "\"\"")) < 0)
         goto finish;
      if ((rc = Emit (file, NULL, "]")) < 0)
         goto finish;

      if ((rc = sprintf (s, "$Get_%s", name)) < 0)
         goto finish;
      if ((rc = Emit (file, NULL, s)) < 0)
         goto finish;
   }

   if ((rc = fprintf (file, "\n")) < 0)
      goto finish;

finish:
   return rc;
}

os_error *hdr_output
(
   FILE     *file,
   const char *title,
   const char *author,
   lookup_t  needses,
   lookup_t  needsatends,
   lookup_t  consts,
   lookup_t  types,
   lookup_t  swis
)
{
   os_error *error = NULL;
   char     *needs, *cnst, *type, *swi;
   def_c     c;
   def_t     t;
   def_s     s;
   void     *context;
   int       rc = 0;
   time_t    now;
   osbool    start;

#if defined NEEDSATEND
   char     *needsatend;
#else
   NOT_USED (needsatends)
#endif

   /*Emit the TITLE declaration.*/
   time (&now);
   if ((rc = fprintf (file,
         (GASSyntax) ?
         "@ GNU AS assembler header file for %s\n"
         "@ Written by DefMod (" __DATE__ ") on %s"
         "@ %s\n\n"
         :
         "; Assembler header file for %s\n"
         "; Written by DefMod (" __DATE__ ") on %s"
         "; %s\n\n",
         title, ctime (&now), author)) < 0)
      goto finish;

   /*Emit OSLib licence for produced headers and binaries.*/
   {
      const char *licence = oslib_license;
      while (*licence != '\0')
      {
         const char *nextLine;
         for (nextLine = licence; *nextLine != '\n'; ++nextLine)
            /* */;
         ++nextLine;
         if ((rc = fprintf (file, "%s%.*s", (GASSyntax) ? "@ " : "; ", (int)(nextLine - licence), licence)) < 0)
            goto finish;
         licence = nextLine;
      }
      if (oslib_license[0])
         if ((rc = fprintf (file, "\n")) < 0)
            goto finish;
   }

   /* guard against multiple inclusion */   
   if (GASSyntax)
   {
      if ((rc = fprintf (file, "#ifndef %s_Hdr\n"
                               "#  define %s_Hdr\n", title, title)) < 0)
         goto finish;
   }
   else
   {
      if ((rc = EmitSSS (file, NULL, "[", ":LNOT: :DEF: ", title, "_Hdr")) < 0)
         goto finish;
      if ((rc = EmitSS (file, NULL, "GBLS", title, "_Hdr")) < 0)
         goto finish;
   }
   if ((rc = fprintf (file, "\n")) < 0)
      goto finish;

   /*Make sure we have Hdr.Types.*/
   if ((rc = Emit_Get (file, "Types")) < 0)
      goto finish;

   /*Emit the NEEDS declarations.*/
   context = 0;
   while (TRUE)
   {
      if ((error = lookup_enumerate (needses, &needs, NULL, &context)) !=
            NULL)
         goto finish;
      if (context == 0) break;

      if ((rc = Emit_Get (file, needs)) < 0)
         goto finish;
   }

   /*Emit the CONST declarations.*/
   start = TRUE;
   context = 0;
   while (TRUE)
   {
      if ((error = lookup_enumerate (consts, &cnst, (void **) &c, &context))
            != NULL)
         goto finish;
      if (context == 0) break;

      if (start)
      {
         if ((rc = fprintf (file, "%cSymbols for constants\n", COMMENTCHAR)) < 0)
            goto finish;
         start = FALSE;
      }

      if (GASSyntax)
      {
         if ((rc = fprintf (file, "#  define %s 0x%x\n", cnst, c->value)) < 0)
            goto finish;
      }
      else
      switch (c->type->tag)
      {
         case def_TYPE_INT:
         case def_TYPE_SHORT:
         case def_TYPE_BYTE:
            if ((rc = EmitD (file, cnst, "*", c->value)) < 0)
               goto finish;
         break;

         case def_TYPE_CHAR:
         {
            char v1 [def_ID_LIMIT + 1];

            if (isprint (c->value))
            {
               if ((rc = sprintf (v1, "\"%c\"", c->value)) < 0)
                  goto finish;
            }
            else
            {
#if 0
               if ((rc = sprintf (v1, ":CHR: &%.2X", c->value)) < 0)
#else
               if ((rc = sprintf (v1, "&%.2X", c->value)) < 0)
#endif
                  goto finish;
            }
            if ((rc = EmitS (file, cnst, "*", v1)) < 0)
               goto finish;
         }
         break;

         case def_TYPE_BOOL:
         {
            char v1 [def_ID_LIMIT + 1];

            if ((rc = sprintf (v1, "{%s}", WHETHER (c->value))) < 0)
               goto finish;
            if ((rc = EmitS (file, cnst, "*", v1)) < 0)
               goto finish;
         }
         break;

         default:
            if ((rc = EmitX (file, cnst, "*", c->value)) < 0)
               goto finish;
         break;
      }
   }

   if (!start)
      if ((rc = fprintf (file, "\n")) < 0)
         goto finish;

   /*Emit the TYPE declarations.*/
   start = TRUE;
   context = 0;
   while (TRUE)
   {
      /* get entry from table; exit loop if no more */
      if ((error = lookup_enumerate (types, &type, (void **) &t, &context))
            != NULL)
         goto finish;
      if (context == 0) break;

      /* emit header if first entry */
      if (start)
      {
         if ((rc = fprintf (file, "%cSymbols for structure offsets and sizes\n", COMMENTCHAR)) < 0)
            goto finish;
         start = FALSE;
      }

      if (t != NULL && t->tag != def_TYPE_ABSTRACT)
      {
         /* emit declaration of |type| for non-abstract type |t| */
	 tracef( "Emit_Decl: %s\n" _ type );
         if ((rc = Emit_Decl (file, t, type, TRUE, 0)) < 0)
            goto finish;
      }
      else
      {
         /* for abstract type, equate symbol to its size */
         if (GASSyntax)
         {
            if ((rc = fprintf (file, "@ abstract type\n"
                                     "#  define %s 4\n\n", type)) < 0)
               goto finish;
         }
         else
         {
            if ((rc = EmitS (file, type, "*", "4 ;abstract type")) < 0)
               goto finish;
         }
      }
   }

   // emit a blank line between declarations */
   if (!start)
      if ((rc = fprintf (file, "\n")) < 0)
         goto finish;

   /*Emit the SWI definitions.*/
   start = TRUE;
   context = 0;
   while (TRUE)
   {
      char title [def_ID_LIMIT + 1] = "X";
      osbool first;
      int  i;

      if ((error = lookup_enumerate (swis, &swi, (void **) &s,
            &context)) != NULL)
         goto finish;
      if (context == 0) break;

      if (start)
      {
         if ((rc = fprintf (file, "%cSymbols for SWI's and SWI reason codes\n", COMMENTCHAR)) < 0)
            goto finish;
         start = FALSE;
      }

      strcat (title, swi);

      for (i = 0; i < 10; i++)
      {
         if (s->starred_constants [i])
         {
            /* SWI reason code.  */
            if (GASSyntax)
            {
               if ((rc = fprintf (file, "#  define %s 0x%x\n", title + 1, s->constants [i])) < 0)
                 goto finish;
            }
            else
            {
               if ((rc = EmitX (file, title + 1, "*", s->constants [i])) < 0)
                  goto finish;
            }
            break;
         }
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
         if (GASSyntax)
         {
            if ((rc = fprintf (file, ".set %s,0x%x\n"
                                     ".set %s,0x%x\n",
                               title, s->swi | (1 << 17), title + 1, s->swi)) < 0)
               goto finish;
         }
         else
         {
            if ((rc = EmitX (file, title, "*", s->swi | 1 << 17)) < 0
                || (rc = EmitX (file, title + 1, "*", s->swi)) < 0)
               goto finish;
         }
      }

      if (!s->absent)
      {
         first = TRUE;
         for (i = 0; i < 10; i++)
            if ((s->k & 1 << i) != NONE)
            {
               if (first)
               {
                  if ((rc = fprintf (file, "   %cEntry\n", COMMENTCHAR)) < 0)
                     goto finish;
                  first = FALSE;
               }

               if ((s->i & 1 << i) != NONE)
               {
                  /*there is an operation to involve*/
                  if ((rc = fprintf (file,
                        "   %c  R%d = &%X %s %s (",
                        COMMENTCHAR,
                        i, s->constants [i], Op [s->op [i]],
                        s->inputs [i]->name)) < 0)
                     goto finish;

                  if ((rc = Emit_Name (file, s->inputs [i])) < 0)
                     goto finish;

                  if ((rc = fprintf (file, ")\n")) < 0)
                     goto finish;
               }
               else
               {
                  if ((rc = fprintf (file,
                        "   %c  R%d = &%X\n",
                        COMMENTCHAR,
                        i, s->constants [i])) < 0)
                     goto finish;
               }
            }
            else if ((s->i & 1 << i) != NONE)
            {
               if (first)
               {
                  if ((rc = fprintf (file, "   %cEntry\n", COMMENTCHAR)) < 0)
                     goto finish;
                  first = FALSE;
               }

               if ((rc = fprintf (file, "   %c  R%d %s %s (",
                     COMMENTCHAR,
                     i,
                     (s->ri & 1 << i) == 0? "=": "->",
                     s->inputs [i]->name)) < 0)
                  goto finish;

               if ((rc = Emit_Name (file, s->inputs [i])) < 0)
                  goto finish;

               if ((rc = fprintf (file, ")\n")) < 0)
                  goto finish;
            }

         first = TRUE;
         for (i = 0; i < 10; i++)
            if ((s->o & 1 << i) != NONE)
            {
               if (first)
               {
                  if ((rc = fprintf (file, "   %cExit\n", COMMENTCHAR)) < 0)
                     goto finish;
                  first = FALSE;
               }

               if ((rc = fprintf (file,
                     "   %c  R%d %s %s (",
                     COMMENTCHAR,
                     i,
                     (s->ro & 1 << i) == 0? "=": "->",
                     s->outputs [i]->name)) < 0)
                  goto finish;

               if ((rc = Emit_Name (file, s->outputs [i])) < 0)
                  goto finish;

               if ((rc = fprintf (file, ")\n")) < 0)
                  goto finish;
            }
            else if ((s->c & 1 << i) != NONE)
            {
               if (first)
               {
                  if ((rc = fprintf (file, "   %cExit\n", COMMENTCHAR)) < 0)
                     goto finish;
                  first = FALSE;
               }

               if ((rc = fprintf (file,
                     "   %c  R%d corrupted\n", COMMENTCHAR, i)) < 0)
                  goto finish;
            }
      }

      if ((rc = fprintf (file, "\n")) < 0)
         goto finish;
   }

#if defined NEEDSATEND
   /*Emit the NEEDSATEND declarations.*/
   context = 0;
   while (TRUE)
   {
      if ((error = lookup_enumerate (needsatends, &needsatend, NULL, &context)) !=
            NULL)
         goto finish;
      if (context == 0) break;

      if ((rc = Emit_Get (file, needsatend)) < 0)
         goto finish;
   }
#endif

   if (GASSyntax)
   {
      if ((rc = fprintf (file, "#endif\n")) < 0)
         goto finish;
   }
   else
   {
      if ((rc = Emit (file, NULL, "]")) < 0)
         goto finish;
      if ((rc = Emit (file, NULL, "END")) < 0)
         goto finish;
   }

finish:
   if (rc < 0) error = (os_error *)_kernel_last_oserror ();      /* TV 980115 */
   return error;
}
