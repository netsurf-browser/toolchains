diff -upr ./gcc-15.2.0/config.sub ./gcc/config.sub
--- config.sub	2025-08-08 07:51:39.866337996 +0100
+++ config.sub	2025-11-09 19:14:29.077010416 +0000
@@ -235,7 +235,7 @@ case $1 in
 				basic_machine=m68k-unknown
 				basic_os=
 				;;
-			amigaos | amigados)
+			amigaos | amigaosvasm | amigados)
 				basic_machine=m68k-unknown
 				basic_os=amigaos
 				;;
diff -upr ./gcc-15.2.0/configure ./gcc/configure
--- configure	2025-08-08 07:51:39.874338130 +0100
+++ configure	2025-11-09 19:14:29.089010599 +0000
@@ -11601,7 +11601,7 @@ fi
 # being built; programs in there won't even run.
 if test "${build}" = "${host}" && test -d ${srcdir}/gcc; then
   # Search for pre-installed headers if nothing else fits.
-  FLAGS_FOR_TARGET=$FLAGS_FOR_TARGET' -B$(build_tooldir)/bin/ -B$(build_tooldir)/lib/ -isystem $(build_tooldir)/include -isystem $(build_tooldir)/sys-include'
+  FLAGS_FOR_TARGET=$FLAGS_FOR_TARGET' -B$(build_tooldir)/bin/ -B$(build_tooldir)/lib/ -isystem $(build_tooldir)/include -isystem $(build_tooldir)/ndk-include -isystem $(build_tooldir)/sys-include'
 fi
 
 if test "x${use_gnu_ld}" = x &&
Only in ./gcc/gcc: amigacollect2.cc
Only in ./gcc/gcc: bbb-baserel.cc
diff -upr ./gcc-15.2.0/gcc/c/c-decl.cc ./gcc/gcc/c/c-decl.cc
--- gcc/c/c-decl.cc	2025-08-08 07:51:40.405346987 +0100
+++ gcc/c/c-decl.cc	2025-11-09 19:14:29.929023418 +0000
@@ -5559,7 +5559,59 @@ c_decl_attributes (tree *node, tree attr
   tree last_decl = lookup_last_decl (*node);
   if (last_decl == error_mark_node)
     last_decl = NULL_TREE;
-  return decl_attributes (node, attributes, flags, last_decl);
+
+  tree returned_attrs = decl_attributes (node, attributes, flags, last_decl);
+
+#ifdef TARGET_M68K
+  /* add an attribute to the function decl's type if there are asm register parameters. */
+  if (TREE_CODE (*node) == FUNCTION_DECL)
+    {
+      char const * synthetic = "";
+      for (tree params = TYPE_ARG_TYPES(TREE_TYPE(*node)); params; params = TREE_CHAIN(params))
+       {
+         tree asmattr = lookup_attribute("asmreg", TYPE_ATTRIBUTES(TREE_VALUE(params)));
+         if (asmattr)
+           synthetic = concat(synthetic, reg_names[TREE_INT_CST_LOW(TREE_VALUE(TREE_VALUE(asmattr)))], NULL);
+       }
+      if (strlen(synthetic) > 0)
+       {
+         tree t;
+         tree asmid = get_identifier("asmregs");
+         tree syntheticid = get_identifier(synthetic);
+         tree value = tree_cons(syntheticid, syntheticid, NULL_TREE);
+         tree newattr = tree_cons(asmid, value, NULL_TREE);
+
+         /* create a type copy with additional attribute. */
+         tree atype = copy_node (TREE_TYPE(*node));
+         tree attrs = TYPE_ATTRIBUTES(atype) = chainon(newattr, TYPE_ATTRIBUTES(atype));
+
+         tree m = TYPE_MAIN_VARIANT(TREE_TYPE(*node));
+
+         TYPE_POINTER_TO (atype) = 0;
+         TYPE_REFERENCE_TO (atype) = 0;
+
+         /* search if such variant exists. */
+         for (t = m; t; t = TYPE_NEXT_VARIANT(t))
+           if (comptypes (t, atype) == 1 && attribute_list_equal (TYPE_ATTRIBUTES(t), attrs))
+             break;
+
+         if (t)
+           TREE_TYPE(*node) = t;
+         else
+           {
+             TREE_TYPE(*node) = atype;
+             /* Add this type to the chain of variants of TYPE.  */
+             TYPE_NEXT_VARIANT (atype) = TYPE_NEXT_VARIANT (m);
+             TYPE_NEXT_VARIANT (m) = atype;
+           }
+
+         returned_attrs = TYPE_ATTRIBUTES(TREE_TYPE(*node));
+       }
+    }
+#endif
+
+  return returned_attrs;
+
 }
 
 
@@ -6405,6 +6457,29 @@ get_parm_array_spec (const struct c_parm
   return tree_cons (name, args, attrs);
 }
 
+#ifdef TARGET_M68K
+
+/* Create a new variant of TYPE, equivalent but distinct.
+ This is so the caller can modify it.  */
+
+static tree
+build_type_copy (tree type)
+  {
+    tree t, m = TYPE_MAIN_VARIANT (type);
+
+    t = copy_node (type);
+
+    TYPE_POINTER_TO (t) = 0;
+    TYPE_REFERENCE_TO (t) = 0;
+
+    /* Add this type to the chain of variants of TYPE.  */
+    TYPE_NEXT_VARIANT (t) = TYPE_NEXT_VARIANT (m);
+    TYPE_NEXT_VARIANT (m) = t;
+
+    return t;
+  }
+#endif
+
 /* Given a parsed parameter declaration, decode it into a PARM_DECL
    and push that on the current scope.  EXPR is a pointer to an
    expression that needs to be evaluated for the side effects of array
@@ -6423,6 +6498,59 @@ push_parm_decl (const struct c_parm *par
   decl_attributes (&decl, attrs, 0);
 
   decl = pushdecl (decl);
+ #ifdef TARGET_M68K
+   extern int decode_reg_name(char const *);
+
+   if (parm->asmspec)
+     {
+       tree atype = TREE_TYPE(decl);
+       const char *asmspec = TREE_STRING_POINTER(parm->asmspec);
+       if (*asmspec == '%')
+        ++asmspec;
+       int reg_number = decode_reg_name (asmspec);
+
+       /* First detect errors in declaring global registers.  */
+       if (reg_number == -1)
+        error ("%Jregister name not specified for %qD", decl, decl);
+       else if (reg_number < 0)
+        error ("%Jinvalid register name for %qD", decl, decl);
+       else if (TYPE_MODE (TREE_TYPE (decl)) == BLKmode)
+        error ("%Jdata type of %qD isn%'t suitable for a register", decl, decl);
+       else if (!targetm.hard_regno_mode_ok(reg_number, TYPE_MODE (TREE_TYPE (decl))))
+        error ("%Jregister specified for %qD isn%'t suitable for data type",
+               decl, decl);
+       /* Now handle properly declared static register variables.  */
+       else
+        {
+          /* Build tree for __attribute__ ((asm(regnum))). */
+          tree ttasm = get_identifier("asmreg");
+          tree value = tree_cons(ttasm, build_int_cst(NULL, reg_number), NULL_TREE);
+          tree t, attrs = tree_cons(ttasm, value, NULL_TREE);
+          /* First check whether such a type already exists - if yes, use
+           that one. This is very important, since otherwise
+           common_type() would think that it sees two different
+           types and would try to merge them - this could result in
+           warning messages. */
+          for (t = TYPE_MAIN_VARIANT(atype); t; t = TYPE_NEXT_VARIANT(t))
+            if (comptypes (t, atype) == 1
+                && attribute_list_equal (TYPE_ATTRIBUTES(t), attrs))
+              break;
+          if (t)
+            atype = t;
+          else
+            {
+              /* Create a new variant, with differing attributes.
+               (Hack! Type with differing attributes should no longer be
+               a variant of its main type. See comment above for
+               explanation why this was necessary). */
+              atype = build_type_copy (atype);
+              TYPE_ATTRIBUTES(atype) = chainon (attrs, TYPE_ATTRIBUTES(atype));
+            }
+          TREE_TYPE(decl) = atype;
+ //       printf("%s using %s, cdecl=%p, type=%p\n", IDENTIFIER_POINTER(DECL_NAME (decl)), asmspec, decl, atype);
+        }
+     }
+ #endif
 
   finish_decl (decl, input_location, NULL_TREE, NULL_TREE, NULL_TREE);
 }
diff -upr ./gcc-15.2.0/gcc/c/c-parser.cc ./gcc/gcc/c/c-parser.cc
--- gcc/c/c-parser.cc	2025-08-08 07:51:40.409347055 +0100
+++ gcc/c/c-parser.cc	2025-11-09 19:14:29.937023540 +0000
@@ -5401,9 +5401,26 @@ c_parser_parameter_declaration (c_parser
 			  ? id_declarator->id_loc
 			  : start_loc);
   location_t param_loc = make_location (caret_loc, start_loc, end_loc);
+  /**
+   * SBF: Add support for __asm("xy") register spec.
+   */
+#ifdef TARGET_M68K
+  tree asmspec = NULL_TREE;
+  if (c_parser_next_token_is_keyword (parser, RID_ASM))
+    {
+      asmspec = c_parser_simple_asm_expr (parser);
+//     printf("asmspec: %s\n", TREE_STRING_POINTER(asmspec));
+    }
+#endif
+  if (c_parser_next_token_is_keyword (parser, RID_ATTRIBUTE))
+    postfix_attrs = c_parser_gnu_attributes (parser);
 
-  return build_c_parm (specs, chainon (postfix_attrs, prefix_attrs),
-		       declarator, param_loc);
+  struct c_parm * cparm = build_c_parm (specs, chainon (postfix_attrs, prefix_attrs),
+	       declarator, param_loc);
+#ifdef TARGET_M68K
+  cparm->asmspec = asmspec;
+#endif
+  return cparm;
 }
 
 /* Parse a string literal in an asm expression.  It should not be
diff -upr ./gcc-15.2.0/gcc/c/c-tree.h ./gcc/gcc/c/c-tree.h
--- gcc/c/c-tree.h	2025-08-08 07:51:40.409347055 +0100
+++ gcc/c/c-tree.h	2025-11-09 19:14:29.937023540 +0000
@@ -539,6 +539,10 @@ struct c_declarator {
   location_t id_loc; /* Currently only set for cdk_id, cdk_array. */
   /* Except for cdk_id, the contained declarator.  For cdk_id, NULL.  */
   struct c_declarator *declarator;
+#ifdef TARGET_M68K
+  /* The optional asm spec to specify the register. */
+  tree asmspec;
+#endif
   union {
     /* For identifiers.  */
     struct {
@@ -589,6 +593,10 @@ struct c_parm {
   struct c_declarator *declarator;
   /* The location of the parameter.  */
   location_t loc;
+#ifdef TARGET_M68K
+  /* The optional asm spec to specify the register. */
+  tree asmspec;
+#endif
 };
 
 /* Used when parsing an enum.  Initialized by start_enum.  */
Only in ./gcc-15.2.0/gcc/cobol: cdf.cc
Only in ./gcc-15.2.0/gcc/cobol: cdf.h
Only in ./gcc-15.2.0/gcc/cobol: parse.cc
Only in ./gcc-15.2.0/gcc/cobol: parse.h
Only in ./gcc-15.2.0/gcc/cobol: scan.cc
diff -upr ./gcc-15.2.0/gcc/collect2.cc ./gcc/gcc/collect2.cc
--- gcc/collect2.cc	2025-08-08 07:51:40.428347372 +0100
+++ gcc/collect2.cc	2025-11-09 19:14:29.973024089 +0000
@@ -1048,7 +1048,9 @@ main (int argc, char **argv)
 
   /* Extract COMPILER_PATH and PATH into our prefix list.  */
   prefix_from_env ("COMPILER_PATH", &cpath);
-  prefix_from_env ("PATH", &path);
+#ifdef __amiga__
+  setenv("PATH", "GCC:bin", 0);
+#endif  prefix_from_env ("PATH", &path);
 
   /* Try to discover a valid linker/nm/strip to use.  */
 
@@ -1392,6 +1394,11 @@ main (int argc, char **argv)
 		    add_to_list (&static_libs, s);
 	      }
 #endif
+	      /* begin-GG-local: dynamic libraries */
+	      #ifdef COLLECT2_LIBNAME_HOOK
+	      	      COLLECT2_LIBNAME_HOOK(arg);
+	      #endif
+	      /* end-GG-local */
 	      break;
 
 #ifdef COLLECT_EXPORT_LIST
@@ -1494,6 +1501,11 @@ main (int argc, char **argv)
 		add_to_list (&static_libs, arg);
 	    }
 #endif
+	  /* begin-GG-local: dynamic libraries */
+#ifdef COLLECT2_LIBNAME_HOOK
+	  	  COLLECT2_LIBNAME_HOOK(arg);
+#endif
+	  /* end-GG-local */
 	}
     }
 
@@ -1614,6 +1626,11 @@ main (int argc, char **argv)
 
       fprintf (stderr, "\n");
     }
+  /* begin-GG-local: dynamic libraries */
+#ifdef COLLECT2_PRELINK_HOOK
+    COLLECT2_PRELINK_HOOK(ld1_argv, &strip_flag);
+#endif
+  /* end-GG-local */
 
   /* Load the program, searching all libraries and attempting to provide
      undefined symbols from repository information.
@@ -1652,6 +1669,9 @@ main (int argc, char **argv)
       }
   }
 
+  /* begin-GG-local: dynamic libraries */
+#ifndef COLLECT2_POSTLINK_HOOK
+
   /* Unless we have done it all already, examine the namelist and search for
      static constructors and destructors to call.  Write the constructor and
      destructor tables to a .s file and reload.  */
@@ -1678,6 +1698,10 @@ main (int argc, char **argv)
 				   frame_tables.number),
                          frame_tables.number);
     }
+#else /* COLLECT2_POSTLINK_HOOK */
+  COLLECT2_POSTLINK_HOOK(output_file);
+#endif
+/* end-GG-local */
 
   /* If the scan exposed nothing of special interest, there's no need to
      generate the glue code and relink so return now.  */
@@ -1815,6 +1839,12 @@ main (int argc, char **argv)
   scan_prog_file (output_file, PASS_SECOND, SCAN_ALL);
 #endif
 
+      /* begin-GG-local: dynamic libraries */
+#ifdef COLLECT2_EXTRA_CLEANUP
+            COLLECT2_EXTRA_CLEANUP();
+#endif
+      /* end-GG-local */
+
   return 0;
 }
 
Only in ./gcc/gcc/config/m68k: amigaos.cc
Only in ./gcc/gcc/config/m68k: amigaos.h
Only in ./gcc/gcc/config/m68k: amigaos.opt
Only in ./gcc/gcc/config/m68k: amigaos.opt.urls
Only in ./gcc/gcc/config/m68k: amigaos-protos.h
diff -upr ./gcc-15.2.0/gcc/config/m68k/constraints.md ./gcc/gcc/config/m68k/constraints.md
--- gcc/config/m68k/constraints.md	2025-08-08 07:51:40.572039754 +0100
+++ gcc/config/m68k/constraints.md	2025-11-09 19:14:30.237028118 +0000
@@ -37,7 +37,7 @@
        (match_test "ival >= -0x8000 && ival <= 0x7fff")))
 
 (define_constraint "K"
-  "Integer constant that moveq can't handle."
+  "Integer constant that moveq cannot handle."
   (and (match_code "const_int")
        (match_test "ival < -0x80 || ival >= 0x80")))
 
@@ -47,7 +47,7 @@
        (match_test "ival < 0 && ival >= -8")))
 
 (define_constraint "M"
-  "Integer constant that moveq+notb can't handle."
+  "Integer constant that moveq+notb cannot handle."
   (and (match_code "const_int")
        (match_test "ival < -0x100 || ival >= 0x100")))
 
@@ -95,7 +95,7 @@
   "Used for operands that satisfy 's' when -mpcrel is not in effect."
   (and (match_code "symbol_ref,label_ref,const")
        (match_test "!TARGET_PCREL")
-       (match_test "!flag_pic || LEGITIMATE_PIC_OPERAND_P (op)")))
+       (match_test "(flag_pic != 1 && flag_pic != 2) || LEGITIMATE_PIC_OPERAND_P (op)")))
 
 (define_memory_constraint "Q"
   "Means address register indirect addressing mode."
Only in ./gcc/gcc/config/m68k: m68k2.cc
Only in ./gcc/gcc/config/m68k: m68k_68000_10_costs.cc
Only in ./gcc/gcc/config/m68k: m68k_68020_costs.cc
Only in ./gcc/gcc/config/m68k: m68k_68030_costs.cc
Only in ./gcc/gcc/config/m68k: m68k_68040_costs.cc
Only in ./gcc/gcc/config/m68k: m68k_68080_costs.cc
Only in ./gcc/gcc/config/m68k: m68kamigaos.h
diff -upr ./gcc-15.2.0/gcc/config/m68k/m68k.cc ./gcc/gcc/config/m68k/m68k.cc
--- gcc/config/m68k/m68k.cc	2025-08-08 07:51:40.572349774 +0100
+++ gcc/config/m68k/m68k.cc	2025-11-09 19:14:30.237028118 +0000
@@ -183,9 +183,9 @@ static void m68k_output_dwarf_dtprel (FI
 static void m68k_trampoline_init (rtx, tree, rtx);
 static poly_int64 m68k_return_pops_args (tree, tree, poly_int64);
 static rtx m68k_delegitimize_address (rtx);
-static void m68k_function_arg_advance (cumulative_args_t,
+extern void m68k_function_arg_advance (cumulative_args_t,
 				       const function_arg_info &);
-static rtx m68k_function_arg (cumulative_args_t, const function_arg_info &);
+extern rtx m68k_function_arg (cumulative_args_t, const function_arg_info &);
 static bool m68k_cannot_force_const_mem (machine_mode mode, rtx x);
 static bool m68k_output_addr_const_extra (FILE *, rtx);
 static void m68k_init_sync_libfuncs (void) ATTRIBUTE_UNUSED;
@@ -364,6 +364,13 @@ static bool m68k_use_lra_p (void);
 #undef TARGET_ASM_FINAL_POSTSCAN_INSN
 #define TARGET_ASM_FINAL_POSTSCAN_INSN m68k_asm_final_postscan_insn
 
+#if defined(TARGET_AMIGAOS)
+#include "amigaos.h"
+#endif
+
+extern tree
+m68k_handle_type_attribute (tree *node, tree name, tree args, int flags, bool *no_add_attrs);
+
 #undef TARGET_ZERO_CALL_USED_REGS
 #define TARGET_ZERO_CALL_USED_REGS m68k_zero_call_used_regs
 
@@ -379,7 +386,14 @@ TARGET_GNU_ATTRIBUTES (m68k_attribute_ta
   { "interrupt_handler", 0, 0, true,  false, false, false,
     m68k_handle_fndecl_attribute, NULL },
   { "interrupt_thread", 0, 0, true,  false, false, false,
-    m68k_handle_fndecl_attribute, NULL }
+    m68k_handle_fndecl_attribute, NULL },
+	  { "asmreg", 1, 1, false, true, false, true, m68k_handle_type_attribute, NULL },
+	  { "asmregs", 1, 1, false,  true, true, true, NULL, NULL },
+	  { "regparm", 1, 1, false,  true, true, true, m68k_handle_type_attribute, NULL },
+	  { "stkparm", 0, 0, false,  true, true, true, m68k_handle_type_attribute, NULL},
+#ifdef SUBTARGET_ATTRIBUTES
+  SUBTARGET_ATTRIBUTES
+#endif
 });
 
 #undef TARGET_DOCUMENTATION_NAME
@@ -393,8 +407,13 @@ struct gcc_target targetm = TARGET_INITI
 /* FL_68881 controls the default setting of -m68881.  gcc has traditionally
    generated 68881 code for 68020 and 68030 targets unless explicitly told
    not to.  */
+#ifdef TARGET_AMIGAOS
+/* SBF: no CAS on the AMIGA and no 68881 per default. */
+#define FL_FOR_isa_20    (FL_FOR_isa_10 | FL_ISA_68020 | FL_BITFIELD )
+#else
 #define FL_FOR_isa_20    (FL_FOR_isa_10 | FL_ISA_68020 \
 			  | FL_BITFIELD | FL_68881 | FL_CAS)
+#endif
 #define FL_FOR_isa_40    (FL_FOR_isa_20 | FL_ISA_68040)
 #define FL_FOR_isa_cpu32 (FL_FOR_isa_10 | FL_ISA_68020)
 
@@ -642,7 +661,7 @@ m68k_option_override (void)
       else
 	m68k_symbolic_jump = "bra%.l %p0";
       /* Turn off function cse if we are doing PIC.  We always want
-	 function call to be done as `bsr foo@PLTPC'.  */
+	 function call to be done as %'bsr foo@PLTPC'.  */
       /* ??? It's traditional to do this for -mpcrel too, but it isn't
 	 clear how intentional that is.  */
       flag_no_function_cse = 1;
@@ -1183,7 +1202,7 @@ m68k_expand_prologue (void)
 			    current_frame.reg_mask, true, true));
     }
 
-  if (!TARGET_SEP_DATA
+  if (!TARGET_SEP_DATA && !TARGET_AMIGA
       && crtl->uses_pic_offset_table)
     emit_insn (gen_load_got (pic_offset_table_rtx));
 }
@@ -1443,23 +1462,6 @@ m68k_ok_for_sibcall_p (tree decl, tree e
   return false;
 }
 
-/* On the m68k all args are always pushed.  */
-
-static rtx
-m68k_function_arg (cumulative_args_t, const function_arg_info &)
-{
-  return NULL_RTX;
-}
-
-static void
-m68k_function_arg_advance (cumulative_args_t cum_v,
-			   const function_arg_info &arg)
-{
-  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);
-
-  *cum += (arg.promoted_size_in_bytes () + 3) & ~3;
-}
-
 /* Convert X to a legitimate function call memory reference and return the
    result.  */
 
@@ -2152,7 +2154,7 @@ m68k_legitimate_constant_address_p (rtx
   if (!CONSTANT_ADDRESS_P (x))
     return false;
 
-  if (flag_pic
+  if ((flag_pic == 1 || flag_pic == 2)
       && !(strict_p && TARGET_PCREL)
       && symbolic_operand (x, VOIDmode))
     return false;
@@ -5037,8 +5039,10 @@ print_operand (FILE *file, rtx op, int l
   else if (letter == 'p')
     {
       output_addr_const (file, op);
+#ifndef TARGET_AMIGAOS
       if (!(GET_CODE (op) == SYMBOL_REF && SYMBOL_REF_LOCAL_P (op)))
 	fprintf (file, "@PLTPC");
+#endif
     }
   else if (GET_CODE (op) == REG)
     {
@@ -5106,6 +5110,10 @@ m68k_get_reloc_decoration (enum m68k_rel
 	{
 	  if (flag_pic == 1 && TARGET_68020)
 	    return "@GOT.w";
+	  else if (flag_pic == 3)
+	    return ":W";
+	  else if (flag_pic == 4)
+	    return ":L";
 	  else
 	    return "@GOT";
 	}
@@ -5690,7 +5698,7 @@ m68k_output_mi_thunk (FILE *file, tree t
     {
       gcc_assert (flag_pic);
 
-      if (!TARGET_SEP_DATA)
+      if (!TARGET_SEP_DATA && !TARGET_AMIGA)
 	{
 	  /* Use the static chain register as a temporary (call-clobbered)
 	     GOT pointer for this function.  We can use the static chain
diff -upr ./gcc-15.2.0/gcc/config/m68k/m68kelf.h ./gcc/gcc/config/m68k/m68kelf.h
--- gcc/config/m68k/m68kelf.h	2025-08-08 07:51:40.573349790 +0100
+++ gcc/config/m68k/m68kelf.h	2025-11-09 19:14:30.241028179 +0000
@@ -104,7 +104,11 @@ do {								\
 #define DEBUGGER_REGNO(REGNO) (REGNO)
 
 #if 0
+<<<<<<< HEAD
 /* SVR4 m68k assembler is bitching on the `comm i,1,1' which askes for
+=======
+/* SVR4 m68k assembler is bitching on the %'comm i,1,1' which askes for 
+>>>>>>> bee2f3bc66c (migrate basic amiga-gcc features to gcc-13)
    1 byte alignment. Don't generate alignment for COMMON seems to be
    safer until we the assembler is fixed.  */
 #undef ASM_OUTPUT_ALIGNED_COMMON
diff -upr ./gcc-15.2.0/gcc/config/m68k/m68k.h ./gcc/gcc/config/m68k/m68k.h
--- gcc/config/m68k/m68k.h	2025-08-08 07:51:40.572349774 +0100
+++ gcc/config/m68k/m68k.h	2025-11-09 19:14:30.241028179 +0000
@@ -17,6 +17,14 @@ You should have received a copy of the G
 along with GCC; see the file COPYING3.  If not see
 <http://www.gnu.org/licenses/>.  */
 
+#define TARGET_M68K 1
+
+#ifdef TARGET_AMIGAOS
+#define TARGET_AMIGA 1
+#else
+#define TARGET_AMIGA 0
+#endif
+
 /* We need to have MOTOROLA always defined (either 0 or 1) because we use
    if-statements and ?: on it.  This way we have compile-time error checking
    for both the MOTOROLA and MIT code paths.  We do rely on the host compiler
@@ -308,7 +316,7 @@ along with GCC; see the file COPYING3.
 
 #define INT_TYPE_SIZE (TARGET_SHORT ? 16 : 32)
 
-/* Define these to avoid dependence on meaning of `int'.  */
+/* Define these to avoid dependence on meaning of %'int'.  */
 #define WCHAR_TYPE "long int"
 #define WCHAR_TYPE_SIZE 32
 
@@ -321,7 +329,7 @@ along with GCC; see the file COPYING3.
 /* For the m68k, we give the data registers numbers 0-7,
    the address registers numbers 010-017 (8-15),
    and the 68881 floating point registers numbers 020-027 (16-23).
-   We also have a fake `arg-pointer' register 030 (24) used for
+   We also have a fake %'arg-pointer' register 030 (24) used for
    register elimination.  */
 #define FIRST_PSEUDO_REGISTER 25
 
@@ -487,16 +495,36 @@ extern enum reg_class regno_reg_class[];
    XXX This macro is m68k specific and used only for m68kemb.h.  */
 #define NEEDS_UNTYPED_CALL 0
 
-/* On the m68k, all arguments are usually pushed on the stack.  */
-#define FUNCTION_ARG_REGNO_P(N) 0
 
 /* On the m68k, this is a single integer, which is a number of bytes
    of arguments scanned so far.  */
+/* SBF: int is enough public info. rest is handled internally. */
 #define CUMULATIVE_ARGS int
 
-/* On the m68k, the offset starts at 0.  */
+extern void m68k_init_cumulative_args (CUMULATIVE_ARGS *, tree, tree);
+extern int m68k_function_arg_reg(unsigned regno);
+
+/* Initialize a variable CUM of type CUMULATIVE_ARGS
+   for a call to a function whose data type is FNTYPE.
+   For a library call, FNTYPE is 0.  */
+#undef INIT_CUMULATIVE_ARGS
 #define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, INDIRECT, N_NAMED_ARGS) \
- ((CUM) = 0)
+  (m68k_init_cumulative_args(&(CUM), (FNTYPE), (INDIRECT)))
+
+/* 1 if N is a possible register number for function argument passing.  */
+#undef FUNCTION_ARG_REGNO_P
+#define FUNCTION_ARG_REGNO_P(N)    m68k_function_arg_reg(N)
+
+/* Max. number of data, address and float registers to be used for passing
+   integer, pointer and float arguments when TARGET_REGPARM.
+   It's 4, so d0-d3, a0-a3 and fp0-fp3 can be used.  */
+#undef M68K_MAX_REGPARM
+#define M68K_MAX_REGPARM 4
+
+/* The default number of data, address and float registers to use when
+   user specified '-mregparm' switch, not '-mregparm=<value>' option.  */
+#undef M68K_DEFAULT_REGPARM
+#define M68K_DEFAULT_REGPARM 2
 
 #define FUNCTION_PROFILER(FILE, LABELNO)  \
   asm_fprintf (FILE, "\tlea %LLP%d,%Ra0\n\tjsr mcount\n", (LABELNO))
@@ -851,8 +879,8 @@ __transfer_from_trampoline ()					\
    '#' for an immediate operand prefix (# in MIT and Motorola syntax
        but & in SGS syntax).
    '!' for the fpcr register (used in some float-to-fixed conversions).
-   '$' for the letter `s' in an op code, but only on the 68040.
-   '&' for the letter `d' in an op code, but only on the 68040.
+   '$' for the letter %'s' in an op code, but only on the 68040.
+   '&' for the letter %'d' in an op code, but only on the 68040.
    '/' for register prefix needed by longlong.h.
    '?' for m68k_library_id_string
 
diff -upr ./gcc-15.2.0/gcc/config/m68k/m68k.md ./gcc/gcc/config/m68k/m68k.md
--- gcc/config/m68k/m68k.md	2025-08-08 07:51:40.573349790 +0100
+++ gcc/config/m68k/m68k.md	2025-11-09 19:14:30.241028179 +0000
@@ -588,7 +588,7 @@
 
 ;; ColdFire/5200 only allows "<Q>" type addresses when the bit position is
 ;; specified as a constant, so we must disable all patterns that may extract
-;; from a MEM at a constant bit position if we can't use this as a constraint.
+;; from a MEM at a constant bit position if we cannot use this as a constraint.
 
 (define_insn "cbranchsi4_btst_mem_insn"
   [(set (pc)
@@ -2622,8 +2622,8 @@
 
 ;; These insns must use MATCH_DUP instead of the more expected
 ;; use of a matching constraint because the "output" here is also
-;; an input, so you can't use the matching constraint.  That also means
-;; that you can't use the "%", so you need patterns with the matched
+;; an input, so you cannot use the matching constraint.  That also means
+;; that you cannot use the "%", so you need patterns with the matched
 ;; operand in both positions.
 
 (define_insn ""
@@ -3644,7 +3644,7 @@
 
 ;; Prevent AND from being made with sp.  This doesn't exist in the machine
 ;; and reload will cause inefficient code.  Since sp is a FIXED_REG, we
-;; can't allocate pseudos into it.
+;; cannot allocate pseudos into it.
 
 (define_expand "andsi3"
   [(set (match_operand:SI 0 "not_sp_operand" "")
@@ -6280,7 +6280,7 @@
   "jmp %a0"
   [(set_attr "type" "jmp")])
 
-;; This should not be used unless the add/sub insns can't be.
+;; This should not be used unless the add/sub insns cannot be.
 
 (define_insn "*lea"
   [(set (match_operand:SI 0 "nonimmediate_operand" "=a")
diff -upr ./gcc-15.2.0/gcc/config/m68k/m68k.opt ./gcc/gcc/config/m68k/m68k.opt
--- gcc/config/m68k/m68k.opt	2025-08-08 07:51:40.573349790 +0100
+++ gcc/config/m68k/m68k.opt	2025-11-09 19:14:30.241028179 +0000
@@ -119,7 +119,7 @@ Target RejectNegative Alias(mcpu=, 547x)
 Generate code for a ColdFire v4e.
 
 mcpu=
-Target RejectNegative Joined Enum(target_device) Var(m68k_cpu_option) Init(unk_device)
+Target RejectNegative Joined Enum(target_device) Var(m68k_cpu_option) Init(m68000)
 Specify the target CPU.
 
 mcpu32
@@ -202,3 +202,42 @@ Support more than 8192 GOT entries on Co
 mxtls
 Target Mask(XTLS)
 Support TLS segment larger than 64K.
+
+mregparm=
+Target RejectNegative Var(m68k_regparm) Joined UInteger Init(-1)
+Pass arguments through registers.
+
+mregparm
+Target RejectNegative Alias(mregparm=, 2)
+Same as -mregparm=2
+
+msasregparm
+Target RejectNegative Var(sas_regparm,1)
+Same as -mregparm=2 but do not use alternate registers
+
+fdouble-indirect
+Target Var(flag_double_indirect) Init(1)
+Use double indirect addressing mode.
+
+fbbb=
+Target RejectNegative Var(help_columns,80) Var(string_bbb_opts) Joined 
+Enable Bebbo's optimizations.\n
++    enable all optimizations (default)\n
+a    commute add move instructions\n
+b    use register for base addresses\n
+c    convert load const and compare into a sub\n
+e    eliminate dead assignments + redundant loads\n
+f    shrink stack frame\n
+i    use post increment on addresses\n
+l    remove single used lea if possible\n
+n    pipeline instructions\n
+m    merge add and move statements\n
+p    propagate move assignment pairs out of loops\n
+r    register renaming to maybe save registers\n
+s    a strcpy optimization\n
+z    final pass with simple substitutions\n
+0    insert clear registers before storing a byte or word, if possible\n
+v    be verbose\n
+V    be very verbose\n
+x    dump insns\n
+Default: -fbbb=+ which yields -fbbb=abcefilmnprsz0
diff -upr ./gcc-15.2.0/gcc/config/m68k/m68k.opt.urls ./gcc/gcc/config/m68k/m68k.opt.urls
--- gcc/config/m68k/m68k.opt.urls	2025-08-08 07:51:40.573349790 +0100
+++ gcc/config/m68k/m68k.opt.urls	2025-11-09 19:14:30.241028179 +0000
@@ -1,109 +1 @@
-; Autogenerated by regenerate-opt-urls.py from gcc/config/m68k/m68k.opt and generated HTML
-
-m5200
-UrlSuffix(gcc/M680x0-Options.html#index-m5200)
-
-m5206e
-UrlSuffix(gcc/M680x0-Options.html#index-m5206e)
-
-m528x
-UrlSuffix(gcc/M680x0-Options.html#index-m528x)
-
-m5307
-UrlSuffix(gcc/M680x0-Options.html#index-m5307)
-
-m5407
-UrlSuffix(gcc/M680x0-Options.html#index-m5407)
-
-m68000
-UrlSuffix(gcc/M680x0-Options.html#index-m68000)
-
-m68010
-UrlSuffix(gcc/M680x0-Options.html#index-m68010)
-
-m68020
-UrlSuffix(gcc/M680x0-Options.html#index-m68020)
-
-m68020-40
-UrlSuffix(gcc/M680x0-Options.html#index-m68020-40)
-
-m68020-60
-UrlSuffix(gcc/M680x0-Options.html#index-m68020-60)
-
-m68030
-UrlSuffix(gcc/M680x0-Options.html#index-m68030)
-
-m68040
-UrlSuffix(gcc/M680x0-Options.html#index-m68040)
-
-m68060
-UrlSuffix(gcc/M680x0-Options.html#index-m68060)
-
-m68881
-UrlSuffix(gcc/M680x0-Options.html#index-m68881)
-
-malign-int
-UrlSuffix(gcc/M680x0-Options.html#index-malign-int)
-
-march=
-UrlSuffix(gcc/M680x0-Options.html#index-march-8)
-
-mbitfield
-UrlSuffix(gcc/M680x0-Options.html#index-mbitfield)
-
-mc68000
-UrlSuffix(gcc/M680x0-Options.html#index-mc68000)
-
-mc68020
-UrlSuffix(gcc/M680x0-Options.html#index-mc68020)
-
-mcfv4e
-UrlSuffix(gcc/M680x0-Options.html#index-mcfv4e)
-
-mcpu=
-UrlSuffix(gcc/M680x0-Options.html#index-mcpu-7)
-
-mcpu32
-UrlSuffix(gcc/M680x0-Options.html#index-mcpu32)
-
-mdiv
-UrlSuffix(gcc/M680x0-Options.html#index-mdiv-1)
-
-mhard-float
-UrlSuffix(gcc/M680x0-Options.html#index-mhard-float-2)
-
-; skipping UrlSuffix for 'mid-shared-library' due to finding no URLs
-
-mlong-jump-table-offsets
-UrlSuffix(gcc/M680x0-Options.html#index-mlong-jump-table-offsets)
-
-; skipping UrlSuffix for 'mlra' due to finding no URLs
-
-mnobitfield
-UrlSuffix(gcc/M680x0-Options.html#index-mnobitfield)
-
-mpcrel
-UrlSuffix(gcc/M680x0-Options.html#index-mpcrel)
-
-mrtd
-UrlSuffix(gcc/M680x0-Options.html#index-mrtd)
-
-; skipping UrlSuffix for 'msep-data' due to finding no URLs
-
-; skipping UrlSuffix for 'mshared-library-id=' due to finding no URLs
-
-mshort
-UrlSuffix(gcc/M680x0-Options.html#index-mshort)
-
-msoft-float
-UrlSuffix(gcc/M680x0-Options.html#index-msoft-float-6)
-
-mstrict-align
-UrlSuffix(gcc/M680x0-Options.html#index-mstrict-align-2)
-
-mtune=
-UrlSuffix(gcc/M680x0-Options.html#index-mtune-9)
-
-mxgot
-UrlSuffix(gcc/M680x0-Options.html#index-mxgot)
-
+; dummy
Only in ./gcc/gcc/config/m68k: t-amigaos
diff -upr ./gcc-15.2.0/gcc/config/m68k/t-linux ./gcc/gcc/config/m68k/t-linux
--- gcc/config/m68k/t-linux	2025-08-08 07:51:40.573349790 +0100
+++ gcc/config/m68k/t-linux	2025-11-09 19:14:30.241028179 +0000
@@ -29,3 +29,27 @@ sysroot-suffix.h: $(srcdir)/config/m68k/
 	$(SHELL) $(srcdir)/config/m68k/print-sysroot-suffix.sh \
 	  "$(SYSTEM_HEADER_DIR)/../.." "$(MULTILIB_MATCHES)" \
 	  "$(MULTILIB_OPTIONS)" > $@
+
+EXTRA_OBJS += m68k2.o m68k_68000_10_costs.o m68k_68020_costs.o m68k_68030_costs.o m68k_68040_costs.o m68k_68080_costs.o
+
+# Extra object file linked to the cc1* executables.
+amigaos.o: $(srcdir)/config/m68k/amigaos.cc $(CONFIG_H)
+	$(CXX) -c $(ALL_CFLAGS) $(ALL_CPPFLAGS) $(INCLUDES) $< $(OUTPUT_OPTION)
+
+m68k2.o: $(srcdir)/config/m68k/m68k2.cc $(CONFIG_H)
+	$(CXX) -c $(ALL_CFLAGS) $(ALL_CPPFLAGS) $(INCLUDES) $< $(OUTPUT_OPTION)
+
+m68k_68000_10_costs.o: $(srcdir)/config/m68k/m68k_68000_10_costs.cc $(CONFIG_H)
+	$(CXX) -c $(ALL_CFLAGS) $(ALL_CPPFLAGS) $(INCLUDES) $< $(OUTPUT_OPTION)
+
+m68k_68020_costs.o: $(srcdir)/config/m68k/m68k_68020_costs.cc $(CONFIG_H)
+	$(CXX) -c $(ALL_CFLAGS) $(ALL_CPPFLAGS) $(INCLUDES) $< $(OUTPUT_OPTION)
+
+m68k_68030_costs.o: $(srcdir)/config/m68k/m68k_68030_costs.cc $(CONFIG_H)
+	$(CXX) -c $(ALL_CFLAGS) $(ALL_CPPFLAGS) $(INCLUDES) $< $(OUTPUT_OPTION)
+
+m68k_68040_costs.o: $(srcdir)/config/m68k/m68k_68040_costs.cc $(CONFIG_H)
+	$(CXX) -c $(ALL_CFLAGS) $(ALL_CPPFLAGS) $(INCLUDES) $< $(OUTPUT_OPTION)
+
+m68k_68080_costs.o: $(srcdir)/config/m68k/m68k_68080_costs.cc $(CONFIG_H)
+	$(CXX) -c $(ALL_CFLAGS) $(ALL_CPPFLAGS) $(INCLUDES) $< $(OUTPUT_OPTION)
diff -upr ./gcc-15.2.0/gcc/config.gcc ./gcc/gcc/config.gcc
--- gcc/config.gcc	2025-08-08 07:51:40.435347488 +0100
+++ gcc/config.gcc	2025-11-09 19:14:29.985024272 +0000
@@ -2433,6 +2433,28 @@ m68k-*-elf* | fido-*-elf*)
 		;;
 	esac
 	;;
+m68k*-*-amigaosvasm*)
+	default_m68k_cpu=68000
+	tm_file="${tm_file} newlib-stdint.h m68k/m68kamigaos.h"
+	tm_defines="${tm_defines} MOTOROLA=1 TARGET_AMIGAOS TARGET_AMIGAOS_VASM TARGET_CPU_DEFAULT=0"
+	tmake_file="m68k/t-floatlib m68k/t-m68kbare m68k/t-amigaos"
+	tm_p_file="${tm_p_file} m68k/amigaos-protos.h"
+	extra_options="${extra_options} m68k/amigaos.opt"
+	gnu_ld=yes
+	;;
+m68k*-*-amigaos*)
+	default_m68k_cpu=68000
+	tm_file="${tm_file} newlib-stdint.h m68k/m68kamigaos.h"
+	tm_defines="${tm_defines} MOTOROLA=1 TARGET_AMIGAOS TARGET_CPU_DEFAULT=0"
+	tmake_file="m68k/t-floatlib m68k/t-m68kbare m68k/t-amigaos"
+	tm_p_file="${tm_p_file} m68k/amigaos-protos.h"
+	extra_options="${extra_options} m68k/amigaos.opt"
+	gnu_ld=yes
+	CFLAGS="-Os"
+  	case ${enable_threads} in
+	    yes | posix) thread_file='posix' ;;
+  	esac	
+	;;
 m68k*-*-netbsdelf*)
 	default_m68k_cpu=68020
 	default_cf_cpu=5475
diff -upr ./gcc-15.2.0/gcc/configure ./gcc/gcc/configure
--- gcc/configure	2025-08-08 07:52:54.960590695 +0100
+++ gcc/configure	2025-11-09 19:14:30.425030987 +0000
@@ -33918,7 +33918,8 @@ fi
 selftest_languages=
 for language in $all_selected_languages
 do
-	selftest_languages="$selftest_languages selftest-$language"
+#	selftest_languages="$selftest_languages selftest-$language"
+  selftest_languages=
 done
 
 # We link each language in with a set of hooks, reached indirectly via
diff -upr ./gcc-15.2.0/gcc/cp/g++spec.cc ./gcc/gcc/cp/g++spec.cc
--- gcc/cp/g++spec.cc	2025-08-08 07:51:40.705351992 +0100
+++ gcc/cp/g++spec.cc	2025-11-09 19:14:30.489031964 +0000
@@ -302,7 +302,7 @@ lang_specific_driver (struct cl_decoded_
 
   /* Add one for shared_libgcc or extra static library.  */
   num_args = (argc + added + need_math + need_experimental
-	      + (library > 0) * 4 + 1);
+	      + (library > 0) * 5 + 1 + 2);
   /* For libc++, on most platforms, the ABI library (usually called libc++abi)
      is provided as a separate DSO, which we must also append.
      However, a platform might have the ability to forward the ABI library
@@ -389,6 +389,38 @@ lang_specific_driver (struct cl_decoded_
 	  j++;
 	}
 #endif
+
+#if defined(TARGET_AMIGAOS)
+      /* SBF: force linking __init_eh and replace new operator. */
+      	{
+      	  bool addglue = true;
+      	  /* do not add glue if exceptions are disabled. */
+      	  for (int ii = 0; ii < argc; ++ii)
+      	    {
+      	      if (decoded_options[ii].opt_index == OPT_fexceptions)
+      		addglue = decoded_options[ii].value;
+      	    }
+      	  {
+      	    extern const char *
+      	    amiga_m68k_prefix_func (int argc, const char ** argv);
+      	    if (addglue)
+      	      {
+      		generate_option (OPT_Wl_, "-u,___init_eh", 1, CL_DRIVER,
+      					   &new_decoded_options[j]);
+      	      }
+      	    else
+      	      {
+      		char const * add =
+      		    "../lib/gcc/m68k-amigaos/" DEFAULT_TARGET_VERSION "/new_op.o";
+      		char const * p = amiga_m68k_prefix_func (1, &add);
+      		generate_option_input_file (p, &new_decoded_options[j]);
+      	      }
+      	    ++j;
+      	  }
+      	}
+#endif
+
+
       if (which_library == USE_LIBCXX)
 	{
 	  generate_option (OPT_l,
diff -upr ./gcc-15.2.0/gcc/cp/mangle.cc ./gcc/gcc/cp/mangle.cc
--- gcc/cp/mangle.cc	2025-08-08 07:51:40.707352026 +0100
+++ gcc/cp/mangle.cc	2025-11-09 19:14:30.493032025 +0000
@@ -4530,8 +4530,18 @@ mangle_decl (const tree decl)
     id = get_identifier ("<anon>");
   else
     {
+#ifdef TARGET_AMIGAOS
+      if (!(TREE_CODE (decl) != TYPE_DECL
+                 || !no_linkage_check (TREE_TYPE (decl), true)))
+       no_linkage_check (TREE_TYPE (decl), true);
+      gcc_assert (TREE_CODE (decl) != TYPE_DECL
+         || !no_linkage_check (TREE_TYPE (decl), true)
+         || (TYPE_STUB_DECL (TREE_TYPE (decl))
+             && TREE_PUBLIC (TYPE_STUB_DECL (TREE_TYPE (decl)))));
+#else
       gcc_assert (TREE_CODE (decl) != TYPE_DECL
 		  || !no_linkage_check (TREE_TYPE (decl), true));
+#endif
       if (abi_version_at_least (10))
 	if (tree fn = decl_function_context (decl))
 	  maybe_check_abi_tags (fn, decl);
diff -upr ./gcc-15.2.0/gcc/cp/parser.cc ./gcc/gcc/cp/parser.cc
--- gcc/cp/parser.cc	2025-08-08 07:51:40.717352193 +0100
+++ gcc/cp/parser.cc	2025-11-09 19:14:30.509032269 +0000
@@ -26441,6 +26441,12 @@ cp_parser_parameter_declaration (cp_pars
 	  template_parameter_pack_p = true;
 	}
     }
+  /**
+   * SBF: Add support for __asm("xy") register spec.
+   */
+#ifdef TARGET_M68K
+    tree pasmspec = cp_parser_asm_specification_opt (parser);
+#endif
 
   /* If the next token is an ellipsis, and we have not seen a declarator
      name, and if either the type of the declarator contains parameter
@@ -26641,6 +26647,44 @@ cp_parser_parameter_declaration (cp_pars
 					decl_spec_token_start->location,
 					input_location);
 
+#ifdef TARGET_M68K
+ if (pasmspec)
+   {
+     const char *asmspec = TREE_STRING_POINTER(pasmspec);
+     if (*asmspec == '%')
+       ++asmspec;
+     int offset = 1;
+     int reg_number = -1;
+     if (asmspec[0] == 'd')
+       reg_number = 0;
+     else if (asmspec[0] == 'a')
+       reg_number = 8;
+     else if (asmspec[0] == 'f' && asmspec[1] == 'p')
+       {
+         reg_number = 16;
+         offset = 2;
+       }
+     unsigned add = asmspec[offset] - '0';
+     if (reg_number < 0 || add > 7)
+          error("invalid register specified %s", asmspec);
+     reg_number += add;
+
+ /* Build tree for __attribute__ ((asmreg(regnum))). */
+     tree ttasm = get_identifier("asmreg");
+     tree value = tree_cons(ttasm, build_int_cst(NULL, reg_number), NULL_TREE);
+     tree attrs = tree_cons(ttasm, value, NULL_TREE);
+
+     /* search outmost declarator, e.g. int * needs the attribute at the pointer not the int. */
+     cp_declarator * d = declarator;
+     if (d)
+       {
+     while (d->kind != cdk_id && d->declarator)
+      d = d->declarator;
+     d->attributes = chainon(attrs, d->attributes);
+     }
+   }
+#endif
+
   return make_parameter_declarator (&decl_specifiers,
 				    declarator,
 				    default_argument,
Only in ./gcc-15.2.0/gcc/doc: cpp.1
Only in ./gcc-15.2.0/gcc/doc: cpp.info
Only in ./gcc-15.2.0/gcc/doc: cppinternals.info
Only in ./gcc-15.2.0/gcc/doc: fsf-funding.7
Only in ./gcc-15.2.0/gcc/doc: g++.1
Only in ./gcc-15.2.0/gcc/doc: gcc.1
Only in ./gcc-15.2.0/gcc/doc: gccgo.1
Only in ./gcc-15.2.0/gcc/doc: gccgo.info
Only in ./gcc-15.2.0/gcc/doc: gcc.info
Only in ./gcc-15.2.0/gcc/doc: gccinstall.info
Only in ./gcc-15.2.0/gcc/doc: gccint.info
Only in ./gcc-15.2.0/gcc/doc: gcov.1
Only in ./gcc-15.2.0/gcc/doc: gcov-dump.1
Only in ./gcc-15.2.0/gcc/doc: gcov-tool.1
Only in ./gcc-15.2.0/gcc/doc: gdc.1
Only in ./gcc-15.2.0/gcc/doc: gdc.info
Only in ./gcc-15.2.0/gcc/doc: gfdl.7
Only in ./gcc-15.2.0/gcc/doc: gfortran.1
Only in ./gcc-15.2.0/gcc/doc: gm2.1
Only in ./gcc-15.2.0/gcc/doc: gnat_rm.info
Only in ./gcc-15.2.0/gcc/doc: gnat-style.info
Only in ./gcc-15.2.0/gcc/doc: gnat_ugn.info
Only in ./gcc-15.2.0/gcc/doc: gpl.7
Only in ./gcc-15.2.0/gcc/doc: lto-dump.1
Only in ./gcc-15.2.0/gcc/doc: m2.info
diff -upr ./gcc-15.2.0/gcc/dwarf2out.cc ./gcc/gcc/dwarf2out.cc
--- gcc/dwarf2out.cc	2025-08-08 07:51:40.805353661 +0100
+++ gcc/dwarf2out.cc	2025-11-09 19:14:30.681034894 +0000
@@ -478,6 +478,26 @@ switch_to_eh_frame_section (bool back AT
       eh_frame_section = ((flags == SECTION_WRITE)
 			  ? data_section : readonly_data_section);
 #endif /* EH_FRAME_SECTION_NAME */
+
+#if defined(TARGET_AMIGAOS)
+      fputs(
+      "\t.section\t.data.__EH_FRAME_OBJECT__\n"
+      "\t.align 2\n"
+      "\t__EH_FRAME_OBJECT__:\n\t.long 0\n\t.long 0\n\t.long 0\n\t.long 0\n\t.long 0\n\t.long 0\n", asm_out_file);
+      fputs("\t.section\t.dlist___EH_FRAME_OBJECTS__\n"
+	    "\t.align 2\n"
+	    "\t.long\t__EH_FRAME_OBJECT__\n", asm_out_file);
+
+      fputs(
+      "\t.section\t.list___EH_FRAME_BEGINS__\n"
+      "\t.align 2\n"
+      "\t.long\t__EH_FRAME_BEGIN__\n", asm_out_file);
+      switch_to_section (eh_frame_section);
+      fputs(
+      "\t.text\n\t.align 2\n"
+      "\t__EH_FRAME_BEGIN__:\n", asm_out_file);
+
+#endif
     }
 
   switch_to_section (eh_frame_section);
diff -upr ./gcc-15.2.0/gcc/file-find.cc ./gcc/gcc/file-find.cc
--- gcc/file-find.cc	2025-08-08 07:51:40.811353761 +0100
+++ gcc/file-find.cc	2025-11-09 19:14:30.693035077 +0000
@@ -180,6 +180,10 @@ prefix_from_string (const char *p, struc
   startp = endp = p;
   while (1)
     {
+#ifdef __MSYS__
+      if (*endp == PATH_SEPARATOR && endp-startp == 1)
+	    ++endp;
+#endif
       if (*endp == PATH_SEPARATOR || *endp == 0)
 	{
 	  strncpy (nstore, startp, endp-startp);
Only in ./gcc-15.2.0/gcc/fortran: gfortran.info
diff -upr ./gcc-15.2.0/gcc/function.cc ./gcc/gcc/function.cc
--- gcc/function.cc	2025-08-08 07:51:40.853354462 +0100
+++ gcc/function.cc	2025-11-09 19:14:30.769036237 +0000
@@ -2265,7 +2265,11 @@ use_register_for_decl (const_tree decl)
   if (TREE_CODE (decl) == PARM_DECL && cfun->tail_call_marked)
     return true;
 
-  if (!DECL_REGISTER (decl))
+  if (!DECL_REGISTER (decl)
+#ifdef TARGET_M68K
+		&& (!DECL_INCOMING_RTL (decl) || !REG_P (DECL_INCOMING_RTL (decl)))
+#endif
+  )
     return false;
 
   /* When not optimizing, disregard register keyword for types that
diff -upr ./gcc-15.2.0/gcc/gcc.cc ./gcc/gcc/gcc.cc
--- gcc/gcc.cc	2025-08-08 07:51:40.855354495 +0100
+++ gcc/gcc.cc	2025-11-09 19:14:30.773036298 +0000
@@ -1228,7 +1228,10 @@ static const char *startfile_prefix_spec
 static const char *sysroot_spec = SYSROOT_SPEC;
 static const char *sysroot_suffix_spec = SYSROOT_SUFFIX_SPEC;
 static const char *sysroot_hdrs_suffix_spec = SYSROOT_HEADERS_SUFFIX_SPEC;
-static const char *self_spec = "";
+#ifndef SELF_SPEC
+#define SELF_SPEC ""
+#endif
+static const char *self_spec = SELF_SPEC;
 
 /* Standard options to cpp, cc1, and as, to reduce duplication in specs.
    There should be no need to override these in target dependent files,
@@ -4480,6 +4483,11 @@ driver_handle_option (struct gcc_options
       if (ENABLE_OFFLOADING)
 	forward_offload_option (opt_index, arg, validated);
 
+      if (0 == strcmp("m", arg)  || 0 == strcmp("pthread", arg))
+	{
+	  save_switch (concat ("-l", arg, NULL), 0, NULL, validated, true);
+	  return true;
+	}
       do_save = false;
       break;
 
@@ -6248,6 +6256,7 @@ do_spec_1 (const char *spec, int inswitc
 	     followed by the absolute directories
 	     that we search for startfiles.  */
 	  case 'D':
+	  case 'F':
 	    {
 	      struct spec_path_info info;
 
@@ -11442,3 +11451,48 @@ driver_get_configure_time_options (void
   obstack_free (&obstack, NULL);
   n_switches = 0;
 }
+
+#if defined(TARGET_AMIGAOS)
+
+static void normalize(char * path)
+{
+  // normalize
+  char *q, *p = path;
+//  printf("path: <%s>\t", path);
+  while ((q = strstr (p, "/../")))
+	{
+	  char *r = q - 1;
+	  while (r >= p && *r != '/' && *r != ':')
+	    --r;
+	  if (r < p)
+	    break;
+	  memmove (r + 1, q + 4, strlen (q + 4) + 1);
+	}
+//  printf("-> <%s>\n", path);
+}
+
+const char * amiga_m68k_prefix_func(int argc, const char ** argv) {
+  char * p;
+  if (standard_libexec_prefix)
+      p = make_relative_prefix(standard_libexec_prefix, "", "m68k-amigaos/");
+  else
+    p = concat("../../../../", "", NULL);
+
+  for (int i = 0; i < argc; ++i) {
+      char * q = concat(p, argv[i], NULL);
+      free(p);
+      p = q;
+  }
+
+  normalize(p);
+
+  if (!*p)  {
+      char * q = concat(p, ".", NULL);
+      free(p);
+      p = q;
+  }
+
+//  printf("amiga_m68k_prefix_func='%s'\n", p);
+  return p;
+}
+#endif
Only in ./gcc-15.2.0/gcc: gengtype-lex.cc
diff -upr ./gcc-15.2.0/gcc/incpath.cc ./gcc/gcc/incpath.cc
--- gcc/incpath.cc	2025-08-08 07:51:40.896355179 +0100
+++ gcc/incpath.cc	2025-11-09 19:14:30.853037519 +0000
@@ -444,12 +444,32 @@ add_cpp_dir_path (cpp_dir *p, incpath_ki
   tails[chain] = p;
 }
 
+static void normalize(char * path)
+{
+  // normalize
+  char *q, *p = path;
+//  printf("path: <%s>\t", path);
+  while ((q = strstr (p, "/../")))
+	{
+	  char *r = q - 1;
+	  while (r >= p && *r != '/' && *r != ':')
+	    --r;
+	  if (r < p)
+	    break;
+	  memmove (r + 1, q + 4, strlen (q + 4) + 1);
+	}
+//  printf("-> <%s>\n", path);
+}
+
 /* Add PATH to the include chain CHAIN. PATH must be malloc-ed and
    NUL-terminated.  */
 void
 add_path (char *path, incpath_kind chain, int cxx_aware, bool user_supplied_p)
 {
   cpp_dir *p;
+
+  normalize (path);
+
   size_t pathlen = strlen (path);
 
 #if defined (HAVE_DOS_BASED_FILE_SYSTEM)
diff -upr ./gcc-15.2.0/gcc/Makefile.in ./gcc/gcc/Makefile.in
--- gcc/Makefile.in	2025-08-08 07:51:40.073341450 +0100
+++ gcc/Makefile.in	2025-11-09 19:14:29.361014750 +0000
@@ -1403,6 +1403,7 @@ OBJS = \
 	auto-inc-dec.o \
 	auto-profile.o \
 	bb-reorder.o \
+	bbb-baserel.o \
 	bitmap.o \
 	builtins.o \
 	caller-save.o \
@@ -2547,7 +2548,7 @@ gcc-nm.cc: gcc-ar.cc
 	cp $^ $@
 
 COLLECT2_OBJS = collect2.o collect2-aix.o vec.o ggc-none.o \
-  collect-utils.o file-find.o hash-table.o selftest.o
+  collect-utils.o file-find.o hash-table.o selftest.o ${EXTRA_COLLECT2_OBJS}
 COLLECT2_LIBS = @COLLECT2_LIBS@
 collect2$(exeext): $(COLLECT2_OBJS) $(LIBDEPS)
 # Don't try modifying collect2 (aka ld) in place--it might be linking this.
diff -upr ./gcc-15.2.0/gcc/passes.def ./gcc/gcc/passes.def
--- gcc/passes.def	2025-08-08 07:51:41.015357163 +0100
+++ gcc/passes.def	2025-11-09 19:14:31.069040815 +0000
@@ -451,7 +451,8 @@ along with GCC; see the file COPYING3.
   NEXT_PASS (pass_warn_function_noreturn);
 
   NEXT_PASS (pass_expand);
-
+  NEXT_PASS (pass_bbb_baserel);
+  
   NEXT_PASS (pass_rest_of_compilation);
   PUSH_INSERT_PASSES_WITHIN (pass_rest_of_compilation)
       NEXT_PASS (pass_instantiate_virtual_regs);
Only in ./gcc-15.2.0/gcc/po: be.gmo
Only in ./gcc-15.2.0/gcc/po: da.gmo
Only in ./gcc-15.2.0/gcc/po: de.gmo
Only in ./gcc-15.2.0/gcc/po: el.gmo
Only in ./gcc-15.2.0/gcc/po: es.gmo
Only in ./gcc-15.2.0/gcc/po: fi.gmo
Only in ./gcc-15.2.0/gcc/po: fr.gmo
Only in ./gcc-15.2.0/gcc/po: hr.gmo
Only in ./gcc-15.2.0/gcc/po: id.gmo
Only in ./gcc-15.2.0/gcc/po: ja.gmo
Only in ./gcc-15.2.0/gcc/po: ka.gmo
Only in ./gcc-15.2.0/gcc/po: nl.gmo
Only in ./gcc-15.2.0/gcc/po: ru.gmo
Only in ./gcc-15.2.0/gcc/po: sr.gmo
Only in ./gcc-15.2.0/gcc/po: sv.gmo
Only in ./gcc-15.2.0/gcc/po: tr.gmo
Only in ./gcc-15.2.0/gcc/po: uk.gmo
Only in ./gcc-15.2.0/gcc/po: vi.gmo
Only in ./gcc-15.2.0/gcc/po: zh_CN.gmo
Only in ./gcc-15.2.0/gcc/po: zh_TW.gmo
diff -upr ./gcc-15.2.0/gcc/target-def.h ./gcc/gcc/target-def.h
--- gcc/target-def.h	2025-08-08 07:51:41.277361535 +0100
+++ gcc/target-def.h	2025-11-09 19:14:31.469046919 +0000
@@ -56,7 +56,7 @@
 #define TARGET_ASM_ALIGNED_PTI_OP NULL
 #define TARGET_ASM_UNALIGNED_PTI_OP NULL
 
-#if !defined(TARGET_ASM_CONSTRUCTOR) && !defined(USE_COLLECT2)
+#if !defined(TARGET_ASM_CONSTRUCTOR) && (!defined(USE_COLLECT2) || defined(TARGET_AMIGAOS))
 # ifdef CTORS_SECTION_ASM_OP
 #  define TARGET_ASM_CONSTRUCTOR default_ctor_section_asm_out_constructor
 # else
@@ -68,7 +68,7 @@
 # endif
 #endif
 
-#if !defined(TARGET_ASM_DESTRUCTOR) && !defined(USE_COLLECT2)
+#if !defined(TARGET_ASM_DESTRUCTOR) && (!defined(USE_COLLECT2) || defined(TARGET_AMIGAOS))
 # ifdef DTORS_SECTION_ASM_OP
 #  define TARGET_ASM_DESTRUCTOR default_dtor_section_asm_out_destructor
 # else
diff -upr ./gcc-15.2.0/gcc/tree-pass.h ./gcc/gcc/tree-pass.h
--- gcc/tree-pass.h	2025-08-08 07:51:44.703418687 +0100
+++ gcc/tree-pass.h	2025-11-09 19:14:35.265104849 +0000
@@ -559,6 +559,7 @@ extern gimple_opt_pass *make_pass_fixup_
 extern gimple_opt_pass *make_pass_backprop (gcc::context *ctxt);
 
 extern rtl_opt_pass *make_pass_expand (gcc::context *ctxt);
+extern rtl_opt_pass *make_pass_bbb_baserel (gcc::context *ctxt);
 extern rtl_opt_pass *make_pass_instantiate_virtual_regs (gcc::context *ctxt);
 extern rtl_opt_pass *make_pass_rtl_fwprop (gcc::context *ctxt);
 extern rtl_opt_pass *make_pass_rtl_fwprop_addr (gcc::context *ctxt);
Only in ./gcc/: .git
diff -upr ./gcc-15.2.0/.gitignore ./gcc/.gitignore
--- .gitignore	2025-08-08 07:51:39.854079770 +0100
+++ .gitignore	2025-11-09 19:14:29.053010049 +0000
@@ -74,3 +74,6 @@ stamp-*
 
 # ADDITIONS from GCCRS front-end
 libgrust/*/target/
+/.cproject
+/.project
+/.settings
Only in ./gcc-15.2.0/INSTALL: binaries.html
Only in ./gcc-15.2.0/INSTALL: build.html
Only in ./gcc-15.2.0/INSTALL: configure.html
Only in ./gcc-15.2.0/INSTALL: download.html
Only in ./gcc-15.2.0/INSTALL: finalinstall.html
Only in ./gcc-15.2.0/INSTALL: gfdl.html
Only in ./gcc-15.2.0/INSTALL: index.html
Only in ./gcc-15.2.0/INSTALL: prerequisites.html
Only in ./gcc-15.2.0/INSTALL: specific.html
Only in ./gcc-15.2.0/INSTALL: test.html
Only in ./gcc-15.2.0/: LAST_UPDATED
Only in ./gcc/libatomic/config: amigaos
diff -upr ./gcc-15.2.0/libatomic/configure.tgt ./gcc/libatomic/configure.tgt
--- libatomic/configure.tgt	2025-08-08 07:51:44.759419621 +0100
+++ libatomic/configure.tgt	2025-11-09 19:14:35.357106253 +0000
@@ -133,6 +133,10 @@ case "${target}" in
 	config_path="${config_path} linux/aarch64 posix"
 	;;
 
+  *-*-amiga*)
+	config_path="${config_path} amigaos/loadstore amigaos/lock"
+  	;;
+
   arm*-*-linux* | arm*-*-uclinux*)
 	# OS support for atomic primitives.
 	config_path="${config_path} linux/arm posix"
Only in ./gcc-15.2.0/libcpp/po: be.gmo
Only in ./gcc-15.2.0/libcpp/po: ca.gmo
Only in ./gcc-15.2.0/libcpp/po: da.gmo
Only in ./gcc-15.2.0/libcpp/po: de.gmo
Only in ./gcc-15.2.0/libcpp/po: el.gmo
Only in ./gcc-15.2.0/libcpp/po: eo.gmo
Only in ./gcc-15.2.0/libcpp/po: es.gmo
Only in ./gcc-15.2.0/libcpp/po: fi.gmo
Only in ./gcc-15.2.0/libcpp/po: fr.gmo
Only in ./gcc-15.2.0/libcpp/po: id.gmo
Only in ./gcc-15.2.0/libcpp/po: ja.gmo
Only in ./gcc-15.2.0/libcpp/po: ka.gmo
Only in ./gcc-15.2.0/libcpp/po: nl.gmo
Only in ./gcc-15.2.0/libcpp/po: pt_BR.gmo
Only in ./gcc-15.2.0/libcpp/po: ro.gmo
Only in ./gcc-15.2.0/libcpp/po: ru.gmo
Only in ./gcc-15.2.0/libcpp/po: sr.gmo
Only in ./gcc-15.2.0/libcpp/po: sv.gmo
Only in ./gcc-15.2.0/libcpp/po: tr.gmo
Only in ./gcc-15.2.0/libcpp/po: uk.gmo
Only in ./gcc-15.2.0/libcpp/po: vi.gmo
Only in ./gcc-15.2.0/libcpp/po: zh_CN.gmo
Only in ./gcc-15.2.0/libcpp/po: zh_TW.gmo
Only in ./gcc-15.2.0/libffi/doc: libffi.info
diff -upr ./gcc-15.2.0/libgcc/config/m68k/fpgnulib.c ./gcc/libgcc/config/m68k/fpgnulib.c
--- libgcc/config/m68k/fpgnulib.c	2025-08-08 07:51:44.893122938 +0100
+++ libgcc/config/m68k/fpgnulib.c	2025-11-09 19:14:35.561109366 +0000
@@ -49,6 +49,17 @@
 ** in with -msoft-float.
 */
 
+/* Prototypes for the above in case we use them.  */
+double __floatunsidf (unsigned long);
+double __floatsidf (long);
+float __floatsisf (long);
+double __extendsfdf2 (float);
+float __truncdfsf2 (double);
+long __fixdfsi (double);
+long __fixsfsi (float);
+double __truncxfdf2 (long double ld);
+long __cmpdf2(double, double);
+
 /* the following deal with IEEE single-precision numbers */
 #define EXCESS		126L
 #define SIGNBIT		0x80000000L
@@ -106,6 +117,7 @@ union long_double_long
 
 #ifndef EXTFLOAT
 
+#ifdef __UNORDSF2
 int
 __unordsf2(float a, float b)
 {
@@ -119,7 +131,9 @@ __unordsf2(float a, float b)
     return 1;
   return 0;
 }
+#endif
 
+#ifdef __UNORDDF2
 int
 __unorddf2(double a, double b)
 {
@@ -135,7 +149,9 @@ __unorddf2(double a, double b)
     return 1;
   return 0;
 }
+#endif
 
+#ifdef __FLOATUNSIDF
 /* convert unsigned int to double */
 double
 __floatunsidf (unsigned long a1)
@@ -168,7 +184,9 @@ __floatunsidf (unsigned long a1)
 
   return dl.d;
 }
+#endif
 
+#ifdef __FLOATSIDF
 /* convert int to double */
 double
 __floatsidf (long a1)
@@ -214,7 +232,9 @@ __floatsidf (long a1)
 
   return dl.d;
 }
+#endif
 
+#ifdef __FLOATUNSISF
 /* convert unsigned int to float */
 float
 __floatunsisf (unsigned long l)
@@ -222,7 +242,10 @@ __floatunsisf (unsigned long l)
   double foo = __floatunsidf (l);
   return foo;
 }
+#endif
+
 
+#ifdef __FLOATSISF
 /* convert int to float */
 float
 __floatsisf (long l)
@@ -230,7 +253,10 @@ __floatsisf (long l)
   double foo = __floatsidf (l);
   return foo;
 }
+#endif
 
+
+#ifdef __EXTENDSFDF2
 /* convert float to double */
 double
 __extendsfdf2 (float a1)
@@ -272,7 +298,9 @@ __extendsfdf2 (float a1)
 
   return dl.d;
 }
+#endif
 
+#ifdef __TRUNCDFSF2
 /* convert double to float */
 float
 __truncdfsf2 (double a1)
@@ -352,7 +380,9 @@ __truncdfsf2 (double a1)
   fl.l = PACK (SIGND (dl1), exp, mant);
   return (fl.f);
 }
+#endif
 
+#ifdef __FIXDFSI
 /* convert double to int */
 long
 __fixdfsi (double a1)
@@ -384,7 +414,9 @@ __fixdfsi (double a1)
 
   return (SIGND (dl1) ? -l : l);
 }
+#endif
 
+#ifdef __FIXSFSI
 /* convert float to int */
 long
 __fixsfsi (float a1)
@@ -392,6 +424,7 @@ __fixsfsi (float a1)
   double foo = a1;
   return __fixdfsi (foo);
 }
+#endif
 
 #else /* EXTFLOAT */
 
@@ -403,16 +436,7 @@ __fixsfsi (float a1)
 
    We assume all numbers are normalized, don't do any rounding, etc.  */
 
-/* Prototypes for the above in case we use them.  */
-double __floatunsidf (unsigned long);
-double __floatsidf (long);
-float __floatsisf (long);
-double __extendsfdf2 (float);
-float __truncdfsf2 (double);
-long __fixdfsi (double);
-long __fixsfsi (float);
-int __cmpdf2 (double, double);
-
+#if !defined(EXTFLOATCMP)
 int
 __unordxf2(long double a, long double b)
 {
@@ -643,6 +667,8 @@ __negxf2 (long double x1)
   return - (double) x1;
 }
 
+#else
+
 long
 __cmpxf2 (long double x1, long double x2)
 {
@@ -685,5 +711,38 @@ __gexf2 (long double x1, long double x2)
   return __cmpdf2 ((double) x1, (double) x2);
 }
 
+/* convert long double to double */
+double
+__truncxfdf2 (long double ld)
+{
+  register long exp;
+  register union double_long dl;
+  register union long_double_long ldl;
+
+  ldl.ld = ld;
+  /*printf ("xfdf in: %s\n", dumpxf (ld));*/
+
+  dl.l.upper = SIGNX (ldl);
+  if ((ldl.l.upper & ~SIGNBIT) == 0 && !ldl.l.middle && !ldl.l.lower)
+    {
+      dl.l.lower = 0;
+      return dl.d;
+    }
+
+  exp = EXPX (ldl) - EXCESSX + EXCESSD;
+  /* ??? quick and dirty: keep `exp' sane */
+  if (exp >= EXPDMASK)
+    exp = EXPDMASK - 1;
+  dl.l.upper |= exp << (32 - (EXPDBITS + 1));
+  /* +1-1: add one for sign bit, but take one off for explicit-integer-bit */
+  dl.l.upper |= (ldl.l.middle & MANTXMASK) >> (EXPDBITS + 1 - 1);
+  dl.l.lower = (ldl.l.middle & MANTXMASK) << (32 - (EXPDBITS + 1 - 1));
+  dl.l.lower |= ldl.l.lower >> (EXPDBITS + 1 - 1);
+
+  /*printf ("xfdf out: %g\n", dl.d);*/
+  return dl.d;
+}
+#endif /* EXTFLOATCMP */
+
 #endif /* !__mcoldfire__ */
 #endif /* EXTFLOAT */
Only in ./gcc/libgcc/config/m68k: gmon.c
Only in ./gcc/libgcc/config/m68k: gmon.h
Only in ./gcc/libgcc/config/m68k: new_op.c
Only in ./gcc/libgcc/config/m68k: t-floatlib-amiga
Only in ./gcc/libgcc/config/m68k: t-glue
diff -upr ./gcc-15.2.0/libgcc/config.host ./gcc/libgcc/config.host
--- libgcc/config.host	2025-08-08 07:51:44.833420855 +0100
+++ libgcc/config.host	2025-11-09 19:14:35.461107840 +0000
@@ -1020,6 +1020,10 @@ m32r-*-elf*)
 m32rle-*-elf*)
 	tmake_file=t-fdpbit
 	;;
+m68k-*-amiga*)
+	tmake_file="$tmake_file m68k/t-glue m68k/t-floatlib-amiga soft-fp"
+	CFLAGS="-Os"
+	;;
 m68k-*-elf* | fido-*-elf)
 	tmake_file="$tmake_file m68k/t-floatlib"
 	;;
Only in ./gcc-15.2.0/libgomp: libgomp.info
diff -upr ./gcc-15.2.0/libiberty/make-relative-prefix.c ./gcc/libiberty/make-relative-prefix.c
--- libiberty/make-relative-prefix.c	2025-08-08 07:51:45.464431382 +0100
+++ libiberty/make-relative-prefix.c	2025-11-09 19:14:36.577124871 +0000
@@ -1,5 +1,10 @@
 /* Relative (relocatable) prefix support.
+<<<<<<< HEAD
    Copyright (C) 1987-2025 Free Software Foundation, Inc.
+=======
+   Copyright (C) 1987, 1989, 1992, 1993, 1994, 1995, 1996, 1997, 1998,
+   1999, 2000, 2001, 2002, 2006, 2012 Free Software Foundation, Inc.
+>>>>>>> bee2f3bc66c (migrate basic amiga-gcc features to gcc-13)
 
 This file is part of libiberty.
 
@@ -60,12 +65,19 @@ relative prefix can be found, return @co
 #ifdef HAVE_SYS_STAT_H
 #include <sys/stat.h>
 #endif
-
+#include <sys/types.h>
+#include <dirent.h>
 #include <string.h>
 
 #include "ansidecl.h"
 #include "libiberty.h"
 
+/* This is for the FreeBSD specific bits to get current program path */
+#if defined(__FreeBSD__)
+#include <sys/sysctl.h>
+#include <err.h>
+#endif
+
 #ifndef R_OK
 #define R_OK 4
 #define W_OK 2
@@ -122,9 +134,6 @@ split_directories (const char *name, int
   const char *p, *q;
   int ch;
 
-  if (!*name)
-    return NULL;
-
   /* Count the number of directories.  Special case MSDOS disk names as part
      of the initial directory.  */
   p = name;
@@ -216,6 +225,32 @@ free_split_directories (char **dirs)
     }
 }
 
+
+#if defined(__FreeBSD__)
+static size_t
+bsd_get_current_executable_path(char *buf, size_t len)
+{
+  size_t llen;
+  int ret;
+  int mib[4];
+
+  mib[0] = CTL_KERN;
+  mib[1] = KERN_PROC;
+  mib[2] = KERN_PROC_PATHNAME;
+  mib[3] = -1;
+
+  llen = len;
+  ret = sysctl(mib, 4, buf, &llen, NULL, 0);
+  if (ret != 0) {
+    warn("%s: sysctl for exec path", __func__);
+    return 0;
+  }
+  return llen;
+}
+
+
+#endif /* __FreeBSD__ */
+
 /* Given three strings PROGNAME, BIN_PREFIX, PREFIX, return a string that gets
    to PREFIX starting with the directory portion of PROGNAME and a relative
    pathname of the difference between BIN_PREFIX and PREFIX.
@@ -235,175 +270,107 @@ make_relative_prefix_1 (const char *prog
   int i, n, common;
   int needed_len;
   char *ret = NULL, *ptr, *full_progname;
-  char *alloc_ptr = NULL;
+  char buf[1024], *p, *q, *t;
+  DIR * d;
 
   if (progname == NULL || bin_prefix == NULL || prefix == NULL)
     return NULL;
 
-  /* If there is no full pathname, try to find the program by checking in each
-     of the directories specified in the PATH environment variable.  */
-  if (lbasename (progname) == progname)
-    {
-      char *temp;
-
-      temp = getenv ("PATH");
-      if (temp)
-	{
-	  char *startp, *endp, *nstore;
-	  size_t prefixlen = strlen (temp) + 1;
-	  size_t len;
-	  if (prefixlen < 2)
-	    prefixlen = 2;
-
-	  len = prefixlen + strlen (progname) + 1;
-#ifdef HAVE_HOST_EXECUTABLE_SUFFIX
-	  len += strlen (HOST_EXECUTABLE_SUFFIX);
-#endif
-	  if (len < MAX_ALLOCA_SIZE)
-	    nstore = (char *) alloca (len);
-	  else
-	    alloc_ptr = nstore = (char *) malloc (len);
+#ifdef __amiga__
+//printf("prog=<%s> bin_prefix=<%s> prefix=<%s>\t", progname, bin_prefix, prefix);
+#endif
 
-	  startp = endp = temp;
-	  while (1)
-	    {
-	      if (*endp == PATH_SEPARATOR || *endp == 0)
-		{
-		  if (endp == startp)
-		    {
-		      nstore[0] = '.';
-		      nstore[1] = DIR_SEPARATOR;
-		      nstore[2] = '\0';
-		    }
-		  else
-		    {
-		      memcpy (nstore, startp, endp - startp);
-		      if (! IS_DIR_SEPARATOR (endp[-1]))
-			{
-			  nstore[endp - startp] = DIR_SEPARATOR;
-			  nstore[endp - startp + 1] = 0;
-			}
-		      else
-			nstore[endp - startp] = 0;
-		    }
-		  strcat (nstore, progname);
-		  if (! access (nstore, X_OK)
-#ifdef HAVE_HOST_EXECUTABLE_SUFFIX
-                      || ! access (strcat (nstore, HOST_EXECUTABLE_SUFFIX), X_OK)
-#endif
-		      )
-		    {
-#if defined (HAVE_SYS_STAT_H) && defined (S_ISREG)
-		      struct stat st;
-		      if (stat (nstore, &st) >= 0 && S_ISREG (st.st_mode))
-#endif
-			{
-			  progname = nstore;
-			  break;
-			}
-		    }
-
-		  if (*endp == 0)
-		    break;
-		  endp = startp = endp + 1;
-		}
-	      else
-		endp++;
-	    }
-	}
-    }
+  buf[0] = 0;
+#if defined(__amiga__)
+  strcpy(buf, "GCC:");
+  n = strlen("GCC:");
+#elif defined(__MSYS__)
+  n = GetModuleFileNameA(0, buf, 1023);
+#elif defined(__MACH__)
+  n = 1022;
+  n |= _NSGetExecutablePath(buf, &n);
+#elif defined(__sun) && defined(__SVR4)
+  n = readlink( "/proc/self/path/a.out", buf, 1023);
+#elif defined(__FreeBSD__)
+  n = bsd_get_current_executable_path(buf, 1023);
+#else
+  n = readlink( "/proc/self/exe", buf, 1023);
+#endif
+  if (n < 0 || n > 1022)
+    return NULL;
+  else if (n)
+    buf[n] = 0;
 
-  if (resolve_links)
-    full_progname = lrealpath (progname);
-  else
-    full_progname = strdup (progname);
-  if (full_progname == NULL)
-    goto bailout;
-
-  prog_dirs = split_directories (full_progname, &prog_num);
-  free (full_progname);
-  if (prog_dirs == NULL)
-    goto bailout;
-
-  bin_dirs = split_directories (bin_prefix, &bin_num);
-  if (bin_dirs == NULL)
-    goto bailout;
-
-  /* Remove the program name from comparison of directory names.  */
-  prog_num--;
-
-  /* If we are still installed in the standard location, we don't need to
-     specify relative directories.  Also, if argv[0] still doesn't contain
-     any directory specifiers after the search above, then there is not much
-     we can do.  */
-  if (prog_num == bin_num)
-    {
-      for (i = 0; i < bin_num; i++)
-	{
-	  if (strcmp (prog_dirs[i], bin_dirs[i]) != 0)
+  //puts(buf);
+#if !defined(__amiga__)
+  buf[1023] = 0;
+  for (p = buf; *p; ++p)
+    if (*p == '\\')
+      *p = '/';
+
+  // remove program and bin folder
+  i = 2;
+  while (p > buf) {
+      if (*--p == '/') {
+	  *p = 0;
+	  if (--i == 0)
 	    break;
-	}
+      }
+  }
+#endif
+  // find common path in bin_prefix and prefix
+  for (p = bin_prefix, q = prefix; *p && *p == *q; ++p, ++q)
+    {}
 
-      if (prog_num <= 0 || i == bin_num)
-	goto bailout;
-    }
+#if defined(__amiga__)
+  p = concat(buf, q, NULL);
+#else
+  p = concat(buf, "/", q, NULL);
+#endif
 
-  prefix_dirs = split_directories (prefix, &prefix_num);
-  if (prefix_dirs == NULL)
-    goto bailout;
-
-  /* Find how many directories are in common between bin_prefix & prefix.  */
-  n = (prefix_num < bin_num) ? prefix_num : bin_num;
-  for (common = 0; common < n; common++)
+  // normalize
+  while ((t = strstr(p, "/../")))
     {
-      if (strcmp (bin_dirs[common], prefix_dirs[common]) != 0)
+      char * r = t - 1;
+      while (r >= p && *r != '/')
+	--r;
+      if (r < p)
 	break;
+      memmove(r, t + 3, strlen(t + 3) + 1);
     }
 
-  /* If there are no common directories, there can be no relative prefix.  */
-  if (common == 0)
-    goto bailout;
-
-  /* Two passes: first figure out the size of the result string, and
-     then construct it.  */
-  needed_len = 0;
-  for (i = 0; i < prog_num; i++)
-    needed_len += strlen (prog_dirs[i]);
-  needed_len += sizeof (DIR_UP) * (bin_num - common);
-  for (i = common; i < prefix_num; i++)
-    needed_len += strlen (prefix_dirs[i]);
-  needed_len += 1; /* Trailing NUL.  */
-
-  ret = (char *) malloc (needed_len);
-  if (ret == NULL)
-    goto bailout;
-
-  /* Build up the pathnames in argv[0].  */
-  *ret = '\0';
-  for (i = 0; i < prog_num; i++)
-    strcat (ret, prog_dirs[i]);
-
-  /* Now build up the ..'s.  */
-  ptr = ret + strlen(ret);
-  for (i = common; i < bin_num; i++)
+
+#ifdef __amiga__
+//  printf("try:\t<%s>\n", p);
+  // remove trailing /
+  char * s = p + strlen(p);
+  while (s > p && *--s == '/')
+    *s = 0;
+#endif
+
+  d = opendir(p);
+#ifdef __amiga__
+  if (*s != ':')
     {
-      strcpy (ptr, DIR_UP);
-      ptr += sizeof (DIR_UP) - 1;
-      *(ptr++) = DIR_SEPARATOR;
+      *++s = '/';
+      *++s = 0;
+    }
+#endif
+  if (d) closedir(d);
+  else
+    {
+// printf("can't open dir: %s\n", p);
+      free(p);
+      strcpy(buf, prefix);
+      buf[q - prefix] = 0;
+      p = concat(buf, q, NULL);
     }
-  *ptr = '\0';
 
-  /* Put in directories to move over to prefix.  */
-  for (i = common; i < prefix_num; i++)
-    strcat (ret, prefix_dirs[i]);
-
- bailout:
-  free_split_directories (prog_dirs);
-  free_split_directories (bin_dirs);
-  free_split_directories (prefix_dirs);
-  free (alloc_ptr);
+#ifdef __amiga__
+//  printf("->\t<%s>\n", p);
+#endif
 
-  return ret;
+  return p;
 }
 
 
Only in ./gcc-15.2.0/libitm: libitm.info
Only in ./gcc-15.2.0/libquadmath: libquadmath.info
diff -upr ./gcc-15.2.0/libstdc++-v3/config/os/newlib/os_defines.h ./gcc/libstdc++-v3/config/os/newlib/os_defines.h
--- libstdc++-v3/config/os/newlib/os_defines.h	2025-08-08 07:51:45.685203420 +0100
+++ libstdc++-v3/config/os/newlib/os_defines.h	2025-11-09 19:14:36.905129876 +0000
@@ -56,6 +56,10 @@
 // See libstdc++/69506
 #define _GLIBCXX_USE_WEAK_REF 0
 
+#elif defined (__AMIGA__)
+#define _GLIBCXX_GTHREAD_USE_WEAK 0
+#define _GLIBCXX_USE_WEAK_REF 0
+
 #endif
 
 #endif
diff -upr ./gcc-15.2.0/libstdc++-v3/configure ./gcc/libstdc++-v3/configure
--- libstdc++-v3/configure	2025-08-08 07:51:45.690435151 +0100
+++ libstdc++-v3/configure	2025-11-09 19:14:36.913129999 +0000
@@ -9259,6 +9259,7 @@ $as_echo_n "checking for $compiler optio
             # adding the `-m68020' flag to GCC prevents building anything better,
             # like `-m68040'.
             lt_prog_compiler_pic='-m68020 -resident32 -malways-restore-a4'
+            enable_shared=no
         ;;
       esac
       ;;
@@ -22800,7 +22801,6 @@ fi
 
 done
 
-
 # Only do link tests if native. Else, hardcode.
 if $GLIBCXX_IS_NATIVE; then
 
@@ -38856,7 +38856,7 @@ $as_echo "#define HAVE_TLS 1" >>confdefs
 
   fi
     ;;
-  *-linux* | *-uclinux* | *-gnu* | *-kfreebsd*-gnu | *-cygwin* | *-solaris*)
+  *-linux* | *-uclinux* | *-gnu* | *-kfreebsd*-gnu | *-cygwin* | *-solaris* | *-amigaos)
 
   # All these tests are for C++; save the language and the compiler flags.
   # The CXXFLAGS thing is suspicious, but based on similar bits previously
diff -upr ./gcc-15.2.0/libstdc++-v3/configure.host ./gcc/libstdc++-v3/configure.host
--- libstdc++-v3/configure.host	2025-08-08 07:51:45.691435169 +0100
+++ libstdc++-v3/configure.host	2025-11-09 19:14:36.917130060 +0000
@@ -226,6 +226,11 @@ case "${host_os}" in
     os_include_dir="os/generic"
     atomicity_dir="cpu/generic"
     ;;
+  amiga*)
+    os_include_dir="os/newlib"
+    CFLAGS="${TARGET_C_FLAGS}"
+    CXXFLAGS="${TARGET_C_FLAGS}"
+  ;;
   bsd*)
     # Plain BSD attempts to share FreeBSD files.
     os_include_dir="os/bsd/freebsd"
diff -upr ./gcc-15.2.0/libstdc++-v3/include/Makefile.in ./gcc/libstdc++-v3/include/Makefile.in
--- libstdc++-v3/include/Makefile.in	2025-08-08 07:51:45.751777211 +0100
+++ libstdc++-v3/include/Makefile.in	2025-11-09 19:14:36.953130609 +0000
@@ -12,6 +12,8 @@
 # even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 # PARTICULAR PURPOSE.
 
+UNAME := $(shell uname)
+
 @SET_MAKE@
 VPATH = @srcdir@
 am__is_gnu_make = { \
@@ -1416,6 +1418,7 @@ thread_host_headers = \
 	${host_builddir}/gthr-posix.h \
 	${host_builddir}/gthr-default.h
 
+ifeq (,$(findstring MSYS,$(UNAME)))
 pch1_source = ${glibcxx_srcdir}/include/precompiled/stdc++.h
 pch1_output_builddir = ${host_builddir}/stdc++.h.gch
 pch1_output_anchor = ${host_builddir}/stdc++.h
@@ -1443,6 +1446,7 @@ pch_output_anchors = \
 PCHFLAGS = -x c++-header -nostdinc++ $(CXXFLAGS) $(VTV_PCH_CXXFLAGS)
 @GLIBCXX_BUILD_PCH_FALSE@pch_build = 
 @GLIBCXX_BUILD_PCH_TRUE@pch_build = ${pch_output}
+endif
 
 # List of all timestamp files.  By keeping only one copy of this list, both
 # CLEANFILES and all-local are kept up-to-date.
@@ -1936,6 +1940,7 @@ ${host_builddir}/gthr-default.h: ${tople
 	    -e 's,^#include "\(.*\)",#include <bits/\1>,g' \
 	    < $< > $@
 
+ifeq (,$(findstring MSYS,$(UNAME)))
 # Build two precompiled C++ includes, stdc++.h.gch/*.gch
 ${pch1a_output}: ${allstamped} ${host_builddir}/c++config.h ${pch1_source}
 	-mkdir -p ${pch1_output_builddir}
@@ -1955,6 +1960,7 @@ ${pch2_output}: ${pch2_source} ${pch1_ou
 ${pch3_output}: ${pch3_source} ${pch2_output}
 	-mkdir -p ${pch3_output_builddir}
 	$(CXX) $(PCHFLAGS) $(AM_CPPFLAGS) -O2 -g ${pch3_source} -o $@
+endif
 
 # AutoGen <bits/version.h>.
 .PHONY: update-version
@@ -2054,9 +2060,14 @@ install-headers:
 @GLIBCXX_HOSTED_TRUE@	for file in ${pstl_headers}; do \
 @GLIBCXX_HOSTED_TRUE@	  $(INSTALL_DATA) $${file} $(DESTDIR)${gxx_include_dir}/${pstl_builddir}; done
 
+ifeq (,$(findstring MSYS,$(UNAME)))
 # To remove directories.
 clean-local:
 	rm -rf ${pch_output_dirs}
+else
+clean-local:
+	echo nada
+endif	
 
 # Stop implicit '.o' make rules from ever stomping on extensionless
 # headers, in the improbable case where some foolish, crack-addled
Only in ./gcc-15.2.0/: MD5SUMS
Only in ./gcc-15.2.0/: NEWS
