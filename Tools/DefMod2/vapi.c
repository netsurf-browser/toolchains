/*vapi.c - output a vapi file given a module defn*/

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
#include <string.h>
#include <ctype.h>

#include "oslib/os.h"

#include "x.h"
#include "lookup.h"
#include "oslib_licence.h"

#include "def.h"

lookup_t m_consts;
lookup_t m_needses;
lookup_t m_types;
char* m_module;
char* m_cheader;
os_error global_error;

os_error* convert_simple_type (def_type type, char** type_name)
{
   os_error *error = NULL;
   
   switch (type)
   {
      case def_TYPE_INT:
         *type_name = "int";
         break;
      case def_TYPE_SHORT:
         *type_name = "short";
         break;
      case def_TYPE_BYTE:
         *type_name = "byte";
         break;
      case def_TYPE_CHAR:
         *type_name = "char";
         break;
      case def_TYPE_BITS:
         *type_name = "bits";
         break;
      case def_TYPE_BOOL:
         *type_name = "bool";
         break;
      case def_TYPE_STRING:
         *type_name = "string";
         break;
      case def_TYPE_ASM:
         *type_name = "AsmRoutine";
         break;
      case def_TYPE_VOID:
         *type_name = "void";
         break;
      default:
      {
         global_error.errnum = 1;
         sprintf(global_error.errmess, "Unknown type: %i", type);
         error = &global_error;
         goto finish;
      }
   }

   finish:
   return error;
}

os_error *convert_vapi_name(char* vapi_name, const char* name)
{
   os_error *error = NULL;
   int       r = 0;
   int       w = 0;   
   int       module_len = strlen(m_module);

   if (strncmp(name, m_module, module_len) == 0 && name[module_len] == '_')
      r += module_len + 1;
   else
   {
      void* context = 0;
      char* needs;
      int len;
      os_error* error;
      while (TRUE)
      {
         if ((error = lookup_enumerate (m_needses, &needs, NULL, &context)) !=
               NULL)
            goto finish;

         if (context == 0) break;

         len = strlen(needs);
         if (strncmp(name, needs, len) == 0 && name[len] == '_')
         {
            r += len + 1;
            sprintf(vapi_name, "%s.", needs);
            w = strlen(vapi_name);
            break;
         }
      }
   }

   while (name[r] != '\0')
   {
      vapi_name[w] = name[r];
      w++;
      r++; 
   }
   vapi_name[w] = '\0';

finish:
   return error;
}

os_error *convert_vapi_enum_name(char* vapi_name, const char* name)
{
   return convert_vapi_name(vapi_name, name);
}

os_error* convert_vapi_struct_name(char* vapi_name, const char* prefix, const char* name)
{
   strcpy (vapi_name, prefix);
   return convert_vapi_name(vapi_name + strlen(prefix), name);
}

os_error* convert_vapi_type_name(char* vapi_name, const char* name)
{
   if (strcmp(name, "Void") == 0)
      strcpy(vapi_name, "char*");
   else if (strcmp(name, "Double") == 0)
      strcpy(vapi_name, "double");
   else if (strcmp(name, "JPEG_Image") == 0)
      strcpy(vapi_name, "OSLib.JPEG.Image"); // HACK - should check if name is defined and if so - prefix with "OSLib."
   else if (strcmp(name, "Gadget_Object") == 0)
      strcpy(vapi_name, "OSLib.Gadget.Object"); // HACK - should check if name is defined and if so - prefix with "OSLib."
   else if (strcmp(name, "Gadget_Flags") == 0)
      strcpy(vapi_name, "OSLib.Gadget.Flags"); // HACK - should check if name is defined and if so - prefix with "OSLib."
   else
      return convert_vapi_name(vapi_name, name);
   return NULL;
}

os_error* convert_vapi_function_name(char* vapi_name, const char* name)
{
   os_error *error = NULL;

   if ((error = convert_vapi_name(vapi_name, name)) != NULL) goto finish;

   char *dot = strrchr(vapi_name, '.');
   if (dot != NULL)
   {
      char* r = dot + 1;
      char* w = vapi_name;
      while (*r != '\0')
      {
         *w = *r;
         r++;
         w++;
      }
      *w = '\0';
   }

finish:
   return error;
}

os_error *convert_vapi_const_name(char* vapi_name, const char* name)
{
   return convert_vapi_function_name(vapi_name, name);
}

void convert_vapi_enum_member(char* vapi_name, const char* name, const char* enum_name)
{
   int r = 0;
   int w = 0;

   while (name[r] != '_'&& name[r] != '\0')
      r++;
   if (name[r] == '_')
      r++;

   if (strncmp (&name[r], enum_name, strlen(enum_name)) == 0 && isupper(name[r + strlen(enum_name)]))
      r += strlen(enum_name);    

   while (name[r] != '\0')
   {
      vapi_name[w] = name[r];
      w++;
      r++; 
   }
   vapi_name[w] = '\0';
}

void convert_vapi_struct_member(char* vapi_name, const char* name)
{
   int r = 0;
   int w = 0;
   bool nextcaps = TRUE;

   while (name[r] != '\0')
   {
      while (name[r] == '_')
      {
         nextcaps = TRUE;
         r++;
      }
      if (name[r] != '\0')
      {
         vapi_name[w] = name[r++];
         if (nextcaps)
         {
            vapi_name[w] = toupper(vapi_name[w]);
            nextcaps = FALSE;
         }
         w++;
      }
   }
   vapi_name[w] = '\0';
}

os_error *get_vapi_type(def_t t, bool ref, char* type)
{
   os_error* error = NULL;

   switch (t->tag)
   {
      case def_TYPE_INT:
      case def_TYPE_SHORT:
      case def_TYPE_BYTE:
      case def_TYPE_CHAR:
      case def_TYPE_BITS:
      case def_TYPE_BOOL:
      case def_TYPE_STRING:
      case def_TYPE_ASM:
      {
         char* simple_type;
         if ((error = convert_simple_type(t->tag, &simple_type)) != NULL) goto finish;
         strcpy(type, simple_type);
         if (ref && t->tag != def_TYPE_STRING)
         {
            strcat(type, "*");
         }
         break;
      }
      case def_TYPE_DATA:
         strcpy(type, "char*");
         break;          
      default:
         if ((error = convert_vapi_type_name(type, t->data.id)) != NULL) goto finish;
   }

finish:
   return error;
}

bool emit_as_const(const char* type)
{
   return
      strcmp(type, "Wimp_W") == 0 ||
      strcmp(type, "Gadget_Flags") == 0 ||
      strcmp(type, "Toolbox_Class") == 0;
}

bool emit_as_enum(const def_t t)
{
   return
      t->tag == def_TYPE_ID && !emit_as_const(t->data.id);
}

os_error* emit_const_type( FILE* file,
                           const char* type,
                           const def_t t
                         )
{
   os_error* error = NULL;
   int       rc = 0;
   char      vapi_enum_name [def_ID_LIMIT + 1], c_name [def_ID_LIMIT + 1], *simple_type;

   def_as_extern(c_name, type);
   if ((error = convert_vapi_enum_name (vapi_enum_name, type)) != NULL) goto finish;
   if ((error = convert_simple_type(t->tag, &simple_type)) != NULL) goto finish;

   if ((rc = fprintf (file,
                      "  [CCode (cname = \"%s\")]\n  public struct %s : %s\n  {\n  }\n\n",
                      c_name,
                      vapi_enum_name,
                      simple_type)) < 0)
      goto finish;

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror();
   return error;
}

os_error* emit_enum_type (FILE* file,
                          const char* type
                         )
{
   os_error* error = NULL;
   def_c     c;
   char*     cnst;
   void*     context;
   osbool    first;
   int       rc;   
   char      vapi_enum_name [def_ID_LIMIT + 1], c_name [def_ID_LIMIT + 1];

   // This type is defined in multiple .swi files; we cannot define it as an enum
   // as we don't know what is defined in the other files.
   // It will be emitted as consts.
   if (emit_as_const(type)) goto finish;

   def_as_extern(c_name, type);
   if ((error = convert_vapi_enum_name (vapi_enum_name, type)) != NULL) goto finish; 

   if ((rc = fprintf (file, "  [CCode (cname = \"%s\")]\n  public enum %s\n  {", c_name, vapi_enum_name)) < 0)
      goto finish;

   context = NULL;
   first = TRUE;
   while (TRUE)
   {
      char vapi_name [def_ID_LIMIT + 1];
      char c_name [def_ID_LIMIT + 1];
      
      if ( ( error = lookup_enumerate( m_consts, &cnst, (void **) &c, &context ))
            != NULL)
         goto finish;

      if (context == 0) break;

      if (  c->type->tag == def_TYPE_ID &&
         strcmp( c->type->data AS id, type ) == 0
         )
      {
         if (first)
            first = FALSE;
         else
            if ((rc = fprintf (file, ",")) < 0)
               goto finish;
         
         def_as_macro( c_name, cnst );
         convert_vapi_enum_member(vapi_name, cnst, vapi_enum_name);
         
         if ((rc = fprintf(file, "\n    [CCode (cname = \"%s\")]\n    %s", c_name, vapi_name)) < 0)
            goto finish;
      }
   }
   if (first)
   {
      if ((rc = fprintf(file, "\n    Default = 0")) < 0)
         goto finish;       
   }
   if ((rc = fprintf(file, "\n  }\n\n")) < 0)
      goto finish;

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror();
   return error;
}

os_error *emit_ref_struct_member (FILE* file,
                                  def_t t,
                                  const char* name,
                                  const char* typearray,
                                  const char* vararray)
{
   os_error* error = NULL;
   char*     type_name;
   int       rc = 0;
   char      vapi_struct_name [def_ID_LIMIT + 1];
   char*     ref = "";

   if (t->data.ref->tag == def_TYPE_REF)
   {
      t = t->data.ref;
      ref = "*";
   }

   switch (t->data.ref->tag)
   {
      case def_TYPE_STRING:
         type_name = "string";
         break;
      case def_TYPE_CHAR:
         type_name = "char*";
         break;
      case def_TYPE_BYTE:
         type_name = "byte*";
         break;
      case def_TYPE_BITS:
         type_name = "bits*";
         break;
      case def_TYPE_ID:
         if ((error = convert_vapi_type_name (vapi_struct_name, t->data.ref->data.id)) != NULL) goto finish;
         strcat(vapi_struct_name, "*"); 
         type_name = vapi_struct_name;
         break;
      case def_TYPE_DATA:
         type_name = "char*";
         break;    
      default:
      {
         global_error.errnum = 1;
         sprintf(global_error.errmess, "Unknown ref type: %i\n", t->data.ref->tag); 
         error = &global_error;
         goto finish;
      }
   }
   if (name == NULL)
   {
      x_LOCAL_ERROR(name_error, 1, "Empty name not supported");
      error = name_error;
      goto finish;
   }
   if ((rc = fprintf (file, "    public unowned %s%s%s %s%s;\n", type_name, ref, typearray, name, vararray)) < 0)
      goto finish;

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror();
   return error;
}

os_error *emit_simple_type_struct_member (FILE* file,
                                          const def_type type,
                                          const char* name,
                                          const char* typearray,
                                          const char* vararray)
{
   os_error* error = NULL;
   char*     type_name;
   int       rc = 0;

   if ((error = convert_simple_type(type, &type_name)) != NULL) goto finish;   

   if (name == NULL)
   {
      x_LOCAL_ERROR(name_error, 1, "Empty name not supported");
      error = name_error;
      goto finish;
   }
   if ((rc = fprintf (file, "    public unowned %s%s %s%s;\n", type_name, typearray, name, vararray)) < 0)
      goto finish;

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror();
   return error;   
}

os_error* emit_struct(FILE* file,
                      const def_t t,
                      const char* type,
                      const char* prefix
                     );

def_t get_array_dimensions(char *typearray,
                           char *vararray,
                           def_t t
                          )
{
   bool first = TRUE;
   char array[20];
   bool multidimensional = t->tag == def_TYPE_ROW && t->data.row.base->tag == def_TYPE_ROW;

   array[0] = '\0';
   while (t->tag == def_TYPE_ROW)
   {
      if (first)
      {
         first = FALSE;
         strcat(array, "[");
      }
      if (!multidimensional)
         sprintf(array + strlen(array), "%i", t->data.row.count);
      strcat(array, t->data.row.base->tag == def_TYPE_ROW ? "," : "]");
      t = t->data.row.base;
   }

   strcpy(multidimensional ? typearray : vararray, array);

   return t;
}

os_error* emit_struct_members(FILE* file,
                              const def_t t,
                              const char* type,
                              const char* prefix,
                              const char* c_prefix,
                              const bool skip_last
                             );

os_error* emit_struct_member(FILE* file,
                             def_t t,
                             const char* type,
                             const char* prefix,
                             const char* c_prefix, 
                             bool ellipsis 
                            )
{
   os_error* error = NULL;
   char      vapi_struct_name [def_ID_LIMIT + 1];
   char      c_name [def_ID_LIMIT + 1];
   char      vapi_member_name [def_ID_LIMIT + 1];
   int       rc = 0;
   char      typearray[20] = "", vararray[20] = "";
   char*     name;

   name = t->name;
   if (ellipsis)
      strcpy(typearray, "[]");
   else
      t = get_array_dimensions(typearray, vararray, t);

   strcpy (c_name, c_prefix);
   if (c_name[0] != '\0')
   {
     strcat (c_name, ".");
   }
   def_as_extern (c_name + strlen (c_name), name);

   strcpy (vapi_member_name, prefix);
   if (vapi_member_name[0] != '\0')
   {
     strcat (vapi_member_name, "_");
   }
   convert_vapi_struct_member (vapi_member_name + strlen (vapi_member_name), name);

   if (t->tag != def_TYPE_STRUCT && t->tag != def_TYPE_UNION)
   {
      char* arraylen = ellipsis ? ", array_length = false" : "";
      if ((rc = fprintf(file, "    [CCode (cname = \"%s\"%s)]\n", c_name, arraylen)) < 0) goto finish;
   }

   switch(t->tag)
   {
      case def_TYPE_INT:
      case def_TYPE_SHORT:
      case def_TYPE_BYTE:
      case def_TYPE_CHAR:
      case def_TYPE_BITS:
      case def_TYPE_BOOL:
      case def_TYPE_STRING:
      case def_TYPE_ASM:
         if ((error = emit_simple_type_struct_member (file, t->tag, vapi_member_name, typearray, vararray)) != NULL)
            goto finish;
         break;

      case def_TYPE_ID:
         if ((error = convert_vapi_type_name (vapi_struct_name, t->data.id)) != NULL) goto finish;
         if (type == NULL)
         {
            x_LOCAL_ERROR(type_error, 1, "Empty type not supported");
            error = type_error;
            goto finish;
         }
         if ((rc = fprintf (file, "    public unowned %s%s %s%s;\n", vapi_struct_name, typearray, vapi_member_name, vararray)) < 0)
           goto finish;
         break;
         
      case def_TYPE_REF:
         if ((error = emit_ref_struct_member (file, t, vapi_member_name, typearray, vararray)) != NULL)
            goto finish;
         break;      

      case def_TYPE_STRUCT:
      case def_TYPE_UNION:
      {
         char structprefix [def_ID_LIMIT + 1];
         strcpy (structprefix, prefix);
         if (structprefix[0] != '\0')
         {
            strcat (structprefix, "_");
         }
         convert_vapi_struct_member (structprefix + strlen (structprefix), t->name);

         if ((error = emit_struct_members (file, t, type, structprefix, c_name, FALSE)) != NULL)
            goto finish;

         break;
      }
      default:
         global_error.errnum = 1;
         sprintf(global_error.errmess, "Unknown type (%i) in emit_struct_member\n", t->tag);
         error = &global_error;
         fflush(file);
         goto finish;
   }

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror();
   return error;
}

os_error* emit_struct_members(FILE* file,
                              const def_t t,
                              const char* type,
                              const char* prefix,
                              const char* c_prefix,
                              const bool skip_last
                             )
{
   os_error* error = NULL;
   int       rc = 0, i;

   for (i = 0; i < t->data AS list.count - skip_last ? 1 : 0; i++)
   {
      bool ellipsis =
         (i == t->data AS list.count - 1 &&
          t->tag == def_TYPE_STRUCT &&
          t->data AS list.ellipsis);
      if ((error = emit_struct_member(file, t->data AS list.members [i], type, prefix, c_prefix, ellipsis)) != NULL) goto finish;
   }

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror();
   return error;
}

//os_error* find_type(char *findtype, def_t *t)
//{
//   os_error *error = NULL;
//   char     *type;
//   void     *context = 0;
//
//   /*Emit the TYPE declarations.*/
//   while (TRUE)
//   {
//      /* step through the TYPES table */
//      if ((error = lookup_enumerate (m_types, &type, (void **) t, &context))
//                    != NULL)
//         goto finish;
//      if (context == 0)  /* end of table */
//      {
//         global_error.errnum = 2;
//         sprintf(global_error.errmess, "Cannot find type '%s'\n", findtype);
//         error = &global_error;
//         break;
//      }
//
//      if (strcmp(type, findtype) == 0)
//         break;
//   }  /* end type declarations */
//
//finish:
//   return error;
//}

os_error* emit_struct(FILE* file,
                      const def_t t,
                      const char* type,
                      const char* prefix
                     )
{
   os_error* error = NULL;
   int       rc = 0;
   char      vapi_struct_name [def_ID_LIMIT + 1];
   char      c_name [def_ID_LIMIT + 1];
   char      base [def_ID_LIMIT + 1] = "";
   bool      class;

   if ((error = convert_vapi_struct_name (vapi_struct_name, prefix, type)) != NULL) goto finish;
   def_as_extern (c_name, type);

   class = t->tag == def_TYPE_STRUCT && t->data AS list.ellipsis && t->data AS list.count > 1;

   if (class)
   {
      if ((rc = fprintf(file, "  [CCode (cname = \"%s_base\")]\n  public struct %sBase\n  {\n",
                        c_name,
                        vapi_struct_name)) < 0)
         goto finish;

      if ((error = emit_struct_members(file, t, type, prefix, "", TRUE)) != NULL) goto finish;

      if ((rc = fprintf(file, "  }\n\n")) < 0)
         goto finish;
   }

   if (t->tag == def_TYPE_STRUCT && t->data AS list.base)
   {
      if (t->data AS list.base->tag != def_TYPE_ID)
      {
         global_error.errnum = 3;
         sprintf(global_error.errmess, "Unexpected type in type '%s'\n", type);
         error = &global_error;
         goto finish;
      }
      if ((error = convert_vapi_type_name (base, t->data AS list.base->data.id)) != NULL) goto finish;
      strcat(base, "Base");
      class = FALSE;
   }

   if ((rc = fprintf(file,
                     "%s  [CCode (cname = \"%s\"%s)]\n  public %s %s%s%s\n  {\n",
                     class ? "  [Compact]\n" : "",
                     c_name,
                     class ? ", cheader_filename = \"oslib/valavapi.h\", free_function = \"free\"" : "",
                     class ? "class" : "struct",
                     vapi_struct_name,
                     base[0] == '\0' ? "" : " : ",
                     base)) < 0)
      goto finish;

   if (class)
   {
      if ((rc = fprintf(file, "    public %s(int numEntries);\n", vapi_struct_name)) < 0) goto finish;
   }

   if ((error = emit_struct_members(file, t, type, prefix, "", FALSE)) != NULL) goto finish;

   if ((rc = fprintf(file, "  }\n\n")) < 0)
      goto finish;

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror();
   return error;
}

os_error* emit_abstract( FILE* file,
                         const def_t t,
                         const char* type
                       )
{
   os_error* error = NULL;
   int       rc = 0;
   char      vapi_type_name [def_ID_LIMIT + 1], c_name [def_ID_LIMIT + 1];

   def_as_extern(c_name, type);
   if ((error = convert_vapi_name (vapi_type_name, type)) != NULL) goto finish; 

   if ((rc = fprintf(file, "  [CCode (cname = \"%s\", has_type_id = false)]\n  public struct %s : bits\n  {\n  }\n\n",
       c_name, vapi_type_name)) < 0) goto finish;

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror();
   return error;
}

os_error* emit_type( FILE* file,
                     const char* type,
                     const def_t t
                   )
{
   os_error* error = NULL;

   if (t != NULL)
   {
      switch (t->tag)
      {
         case def_TYPE_STRUCT:
         case def_TYPE_UNION:
            if ((error = emit_struct (file, t, type, "")) != NULL) goto finish;
            break;
         case def_TYPE_ABSTRACT:
            if ((error = emit_abstract (file, t, type)) != NULL) goto finish;
            break;
         default:
            if (emit_as_const(type))
            {
               if ((error = emit_const_type (file, type, t)) != NULL) goto finish;
            }
            else
            {
               if ((error = emit_enum_type (file, type)) != NULL) goto finish;
            }
      }
   }
   
finish:
   return error;
}

os_error* emit_const( FILE* file,
                     const char* type,
                     def_t t
                    )
{
   os_error* error = NULL;
   int       rc;
   char      vapi_name [def_ID_LIMIT + 1], c_name [def_ID_LIMIT + 1], id_type [def_ID_LIMIT + 1], *type_name;

   if (t->tag != def_TYPE_ASM && !emit_as_enum(t))
   {
      def_as_macro(c_name, type);
      if ((error = convert_vapi_const_name (vapi_name, type)) != NULL) goto finish;
      bool ref = FALSE;
      if (t->tag == def_TYPE_REF)
      {
         ref = TRUE;
         t = t->data.ref;
      }
      if (t->tag != def_TYPE_ID)
      {
         if (t->tag == def_TYPE_DATA && ref)
         {
            type_name = "char";
         }
         else
         {
            if ((error = convert_simple_type (t->tag, &type_name)) != NULL) goto finish;
         }
      }
      else
      {
         convert_vapi_type_name(id_type, t->data.id);
         type_name = id_type;
      }

      if ((rc = fprintf(file, "  [CCode (cname=\"%s\")]\n", c_name)) < 0) goto finish;
      if ((rc = fprintf(file, "  public %s %s%s %s;\n", ref ? "static" : "const", type_name, ref ? "*" : "", vapi_name)) < 0) goto finish;
   }

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror();
   return error;
}


os_error* emit_function(FILE* file, const char* swi, def_s s, osbool xversion)
{
   os_error* error = NULL;
   char vapi_name [def_ID_LIMIT + 1];
   char vapi_type_name [def_ID_LIMIT + 1];
   char c_name [def_ID_LIMIT + 1];
   //char *type, *name;
   int result = s->value != NONE? def_bit_index (s->value, 0): -1;  /*number of register returned as result*/
   int i;
   osbool first;

   def_as_extern (c_name, swi);

   if ((error = convert_vapi_function_name (vapi_name, swi)) != NULL) goto finish;

   fprintf(file, "  [CCode (cname = \"%s%s\")]\n", xversion ? "x" : "", c_name);

   if (xversion)
   {
      sprintf(vapi_type_name, "OS.Error*");
   }
   else
   {
      if (result == def_FLAGS)
      {
         sprintf(vapi_type_name, "bits");
      }
      else if (result >= 0)
      {
         def_t t = s->outputs[result];
         osbool ref = FALSE;
         if (t->tag == def_TYPE_REF)
         {
            t = t->data.ref;
            ref = TRUE;
         }
         if ((error = get_vapi_type (t, ref, vapi_type_name)) != NULL) goto finish;
      }
      else
      {
         sprintf(vapi_type_name, "void");
      }
   }

   fprintf(file, "  public static %s %s%s", vapi_type_name, xversion ? "X" : "", vapi_name);

   first = TRUE;
   if (!def_using_block(s))
   {
      for (i = 0; i < 10; i++)
         if ((s->i & 1 << i) != 0)
         {
            def_t t = s->inputs[i];
            osbool ref = FALSE;
            if ((s->ri & 1 << i) != 0)
            {
               ref = TRUE;
            }
            if (t->tag == def_TYPE_REF)
            {
               t = t->data.ref;
               ref = TRUE;   
            }
            if ((error = get_vapi_type (t, ref, vapi_type_name)) < 0)
               goto finish;
            fprintf(file, "%s%s %s", first ? "\n    (" : ",\n     ", vapi_type_name, s->inputs [i]->name);
            first = FALSE;
         }
      for (i = 0; i < 10; i++)
         if ((s->o & 1 << i) != 0 && (xversion || i != result))
         {
            def_t t = s->outputs[i];
            osbool ref = FALSE;
            if ((s->ro & 1 << i) != 0)
            {
               ref = TRUE;
            }
            if (t->tag == def_TYPE_REF)
            {
               t = t->data.ref;
               ref = TRUE;   
            }
            if ((error = get_vapi_type (t, ref, vapi_type_name)) < 0)
               goto finish;
            fprintf(file, "%sout %s %s", first ? "\n    (" : ",\n     ", vapi_type_name, s->outputs [i]->name);
            first = FALSE;
         }
      if (s->f_out && (xversion || result != def_FLAGS))
      {
         fprintf(file, "%sout uint psr", first ? "\n    (" : ",\n     ");
         first = FALSE;
      }
      fprintf(file, "%s);\n\n", first ? " (" : "");   
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

               if ((error = get_vapi_type (s->inputs [i]->data AS list.members[cpt], FALSE, vapi_type_name)) != NULL)
                  goto finish;

               fprintf (file, "%s%s %s", first ? "\n    (" : ",\n     ", vapi_type_name, s->inputs [i]->data AS list.members[cpt]->name);
               first = FALSE;
            }
 
            break;
         }
      fprintf(file, "%s);\n\n", first ? " (" : "");   
   }

finish:
   return error;
}

os_error *emit_OSLibLicence(FILE *file)
{
   os_error *error = NULL;
   int       rc = 0;

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

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror ();
   return error;
}

os_error *emit_usings(FILE *file)
{
   os_error *error = NULL;
   int       rc = 0;
   char     *needs;
   void     *context = 0;

   if ((rc = fprintf (file, "using OSLib;\n")) < 0)
     goto finish;

   /*Emit the NEEDS declarations.*/
   while (TRUE)
   {
      if ((error = lookup_enumerate (m_needses, &needs, NULL, &context)) !=
            NULL)
         goto finish;

      if (context == 0) break;

      if ((rc = fprintf (file, "using OSLib.%s;\n", needs)) < 0)
         goto finish;
   }

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror ();
   return error;
}

os_error *emit_all_types(FILE *file)
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
      if ((error = lookup_enumerate (m_types, &type, (void **) &t, &context))
            != NULL)
         goto finish;
      if (context == 0) break;  /* end of table */

      if ((error = emit_type( file, type, t )) != NULL)
         goto finish;
   }  /* end type declarations */

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror ();
   return error;
}

os_error *emit_all_consts(FILE *file)
{
   os_error *error = NULL;
   int       rc = 0;
   char     *cnst;
   def_c     c;
   void     *context = 0;

   /*Emit const declarations.*/
   while (TRUE)
   {
      if ( ( error = lookup_enumerate( m_consts, &cnst, (void **) &c, &context ))
            != NULL)
         goto finish;

      if (context == 0) break;

      if ((error = emit_const(file, cnst, c->type)) != NULL)
         goto finish;
   }
   if ((rc = fprintf(file, "\n")) < 0) goto finish;

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror ();
   return error;
}

os_error *emit_all_functions(FILE *file,
                           lookup_t swis)
{
   os_error *error = NULL;
   int       rc = 0;
   char     *swi;
   def_s     s;
   void     *context = 0;

   /*Emit the SWI declarations.*/
   while (TRUE)
   {
      if ((error = lookup_enumerate (swis, &swi, (void **) &s, &context))
            != NULL)
         goto finish;
      if (context == 0) break;
      
      if (!s->absent)
      {
         if ((error = emit_function(file, swi, s, TRUE)) != NULL) goto finish;
         if ((error = emit_function(file, swi, s, FALSE)) != NULL) goto finish;
      }
   }

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror ();
   return error;
}

void set_cheader ()
{
   static char cheader[100];
   char        c_name[def_ID_LIMIT + 1];  

   strcpy (cheader, "cheader_filename = \"oslib/");
   def_as_extern (c_name, m_module);
   strcat (cheader, c_name);
   strcat (cheader, ".h\"");
   m_cheader = cheader;
}

/* main entry */
os_error *vapi_output
(
   FILE     *file,
   char     *title,     /* module title */
   char     *author,    /* unused */
   lookup_t  needses,   /* unused */
   lookup_t  needsatends,   /* unused */
   lookup_t  consts,
   lookup_t  types,
   lookup_t  swis
)
{
   os_error *error = NULL;
   int       rc = 0;

   m_consts  = consts;
   m_needses = needses;
   m_types   = types;
   m_module  = title;

   set_cheader();

   if ((error = emit_OSLibLicence(file)) != NULL) goto finish;

   if ((error = emit_usings(file)) != NULL) goto finish;

   if ((rc = fprintf (file,
         "\n"
         "[CCode (%s)]\n"
         "namespace OSLib.%s\n"
         "{\n", m_cheader, title)) < 0)
      goto finish;

   if ((error = emit_all_types(file)) != NULL) goto finish;

   if ((error = emit_all_consts(file)) != NULL) goto finish;

   if ((error = emit_all_functions(file, swis)) != NULL) goto finish;

   if ((rc = fprintf (file,
         "}\n")) < 0)
      goto finish;

finish:
   if (rc < 0) error = (os_error*)_kernel_last_oserror ();
   return error;
}
