
--- config.sub	2016-08-31 09:49:03.000000000 +0100
+++ config.sub	2025-11-02 00:56:37.158266391 +0000
@@ -2,7 +2,7 @@
 # Configuration validation subroutine script.
 #   Copyright 1992-2016 Free Software Foundation, Inc.
 
-timestamp='2016-01-01'
+timestamp='2017-04-21'
 
 # This file is free software; you can redistribute it and/or modify it
 # under the terms of the GNU General Public License as published by
@@ -500,7 +500,7 @@ case $basic_machine in
 	amiga | amiga-*)
 		basic_machine=m68k-unknown
 		;;
-	amigaos | amigados)
+	amigaos | amigaosvasm | amigados)
 		basic_machine=m68k-unknown
 		os=-amigaos
 		;;
@@ -1380,7 +1380,7 @@ case $os in
 	      | -amigaos* | -amigados* | -msdos* | -newsos* | -unicos* | -aof* \
 	      | -aos* | -aros* | -cloudabi* | -sortix* \
 	      | -nindy* | -vxsim* | -vxworks* | -ebmon* | -hms* | -mvs* \
-	      | -clix* | -riscos* | -uniplus* | -iris* | -rtu* | -xenix* \
+	      | -clix* | -riscos* | -uniplus* | -iris* | -rt* | -xenix* \
 	      | -hiux* | -386bsd* | -knetbsd* | -mirbsd* | -netbsd* \
 	      | -bitrig* | -openbsd* | -solidbsd* \
 	      | -ekkobsd* | -kfreebsd* | -freebsd* | -riscix* | -lynxos* \

--- configure	2017-01-09 21:01:26.000000000 +0000
+++ configure	2025-11-02 00:56:37.162266452 +0000
@@ -6650,7 +6650,13 @@ case " $configdirs " in
   *" lto-plugin "* | *" libcc1 "*)
     # When these are to be built as shared libraries, the same applies to
     # libiberty.
-    extra_host_libiberty_configure_flags=--enable-shared
+     case $host in
+        *-amigaos*)
+        ;;
+        *)
+         extra_host_libiberty_configure_flags=--enable-shared
+        ;;
+      esac
     ;;
   *" bfd "*)
     # When bfd is to be built as a shared library, the same applies to
@@ -7644,7 +7650,7 @@ fi
 # being built; programs in there won't even run.
 if test "${build}" = "${host}" && test -d ${srcdir}/gcc; then
   # Search for pre-installed headers if nothing else fits.
-  FLAGS_FOR_TARGET=$FLAGS_FOR_TARGET' -B$(build_tooldir)/bin/ -B$(build_tooldir)/lib/ -isystem $(build_tooldir)/include -isystem $(build_tooldir)/sys-include'
+  FLAGS_FOR_TARGET=$FLAGS_FOR_TARGET' -B$(build_tooldir)/bin/ -B$(build_tooldir)/lib/ -isystem $(build_tooldir)/include -isystem $(build_tooldir)/ndk-include -isystem $(build_tooldir)/sys-include'
 fi
 
 if test "x${use_gnu_ld}" = x &&

--- configure.ac	2017-01-09 21:01:26.000000000 +0000
+++ configure.ac	2025-11-02 00:56:37.166266512 +0000
@@ -3243,7 +3243,7 @@ fi
 # being built; programs in there won't even run.
 if test "${build}" = "${host}" && test -d ${srcdir}/gcc; then
   # Search for pre-installed headers if nothing else fits.
-  FLAGS_FOR_TARGET=$FLAGS_FOR_TARGET' -B$(build_tooldir)/bin/ -B$(build_tooldir)/lib/ -isystem $(build_tooldir)/include -isystem $(build_tooldir)/sys-include'
+  FLAGS_FOR_TARGET=$FLAGS_FOR_TARGET' -B$(build_tooldir)/bin/ -B$(build_tooldir)/lib/ -isystem $(build_tooldir)/include -isystem $(build_tooldir)/ndk-include -isystem $(build_tooldir)/sys-include'
 fi
 
 if test "x${use_gnu_ld}" = x &&
Only in ../gcc: .cproject
Only in ./gcc-6.5.0/gcc/ada/doc/share: _static

--- gcc/alias.c	2018-06-25 18:37:40.000000000 +0100
+++ gcc/alias.c	2025-11-02 00:56:37.698274566 +0000
@@ -1860,8 +1860,21 @@ rtx_equal_for_memref_p (const_rtx x, con
 }
 
 static rtx
+find_base_term_0(rtx x);
+static int inside;
+static rtx
 find_base_term (rtx x)
 {
+  if (inside > 42)
+    return 0;
+  ++inside;
+  rtx r = find_base_term_0(x);
+  --inside;
+  return r;
+}
+static rtx
+find_base_term_0 (rtx x)
+{
   cselib_val *val;
   struct elt_loc_list *l, *f;
   rtx ret;
@@ -3340,7 +3353,9 @@ init_alias_analysis (void)
 
 		  if (set != 0
 		      && REG_P (SET_DEST (set))
-		      && REGNO (SET_DEST (set)) >= FIRST_PSEUDO_REGISTER)
+		      && REGNO (SET_DEST (set)) >= FIRST_PSEUDO_REGISTER
+	          /* SBF: ignore regs marked as REG_INC. */
+		      && !find_reg_note(insn, REG_INC, SET_DEST (set)))
 		    {
 		      unsigned int regno = REGNO (SET_DEST (set));
 		      rtx src = SET_SRC (set);
Only in ../gcc/gcc: amigacollect2.c

--- gcc/auto-inc-dec.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/auto-inc-dec.c	2025-11-02 00:56:37.698274566 +0000
@@ -584,6 +584,123 @@ attempt_change (rtx new_addr, rtx inc_re
   return true;
 }
 
+/* SBF: Search an add after the mem use which matches:
+ * replace the tmp register with the found register
+ * let the inc_insn to use that register.
+ * and the post_inc is modified to use the mode size.
+ * the sum stays the same
+ *
+ * b = a + x
+ * *b = ...
+ * a = a + x + 4
+ * ->
+ * a = a + x
+ * *a = ...
+ * a = a + 4
+ *
+ * that's a post_inc now
+ */
+
+static bool
+convert_to_post_inc (rtx_insn *last_insn, rtx *inc_reg, int size)
+{
+  int add1, add2;
+  rtx set, reg, src, mint;
+  rtx_insn *insn, *next;
+
+  /* SBF: check if a next insn using the inc_insn's src matches.*/
+  if (!CONST_INT_P(inc_insn.reg1) || last_insn != mem_insn.insn)
+    return false;
+
+  for (insn = NEXT_INSN (mem_insn.insn); insn; insn = next)
+    {
+      df_insn_info *insn_info;
+      df_ref use;
+
+      next = NEXT_INSN (insn);
+      if (DEBUG_INSN_P(insn) || NOTE_P(insn))
+	continue;
+
+      /* stop at label, jump. */
+      if (LABEL_P(insn) || JUMP_P(insn))
+	return false;
+
+      set = single_set (insn);
+      if (!set)
+	return false;
+
+      if (GET_CODE (set) != SET)
+	return false;
+
+      reg = SET_DEST(set);
+      if (REG_P(reg) && REGNO(reg) == REGNO(inc_insn.reg0))
+	break;
+
+      /* must not use the reg. */
+      insn_info = DF_INSN_INFO_GET(insn);
+      FOR_EACH_INSN_INFO_USE (use, insn_info)
+	if (DF_REF_REG (use) == inc_insn.reg0)
+	  return false;
+    }
+  /* found nothing. */
+  if (!insn)
+    return false;
+
+  /* src must be PLUS (reg, const_int). */
+  src = SET_SRC(set);
+  if (GET_CODE(src) != PLUS || XEXP(src, 0) != inc_insn.reg0
+      || !CONST_INT_P(XEXP (src, 1)))
+    return false;
+
+  add2 = INTVAL(XEXP (src, 1));
+  add1 = INTVAL(inc_insn.reg1);
+  if (add1 + size != add2)
+    return false;
+
+
+  /* found a candidate. */
+  if (dump_file)
+    {
+      fprintf(dump_file, "converting from PRE_ADD:\n");
+      dump_insn_slim (dump_file, inc_insn.insn);
+      dump_insn_slim (dump_file, mem_insn.insn);
+      dump_insn_slim (dump_file, insn);
+    }
+
+  mint = GEN_INT(size);
+
+  validate_change (inc_insn.insn, &SET_DEST(PATTERN (inc_insn.insn)), inc_insn.reg0,
+		   true);
+  validate_change (mem_insn.insn, &XEXP(*mem_insn.mem_loc, 0),
+		   inc_insn.reg0, true);
+  validate_change (insn, &SET_SRC(set),
+		   gen_rtx_PLUS(SImode, inc_insn.reg0, mint), true);
+
+  if (!apply_change_group ())
+    return false;
+
+
+
+  if (dump_file)
+    {
+      fprintf(dump_file, "converted to POST_INC:\n");
+      dump_insn_slim (dump_file, inc_insn.insn);
+      dump_insn_slim (dump_file, mem_insn.insn);
+      dump_insn_slim (dump_file, insn);
+    }
+
+  *inc_reg = mem_insn.reg0 = inc_insn.reg0;
+
+  inc_insn.insn = insn;
+  inc_insn.pat = set;
+  inc_insn.reg1 = mint;
+  inc_insn.reg1_val = size;
+  inc_insn.form = FORM_POST_INC;
+  last_insn = insn;
+
+  return true;
+}
+
 
 /* Try to combine the instruction in INC_INSN with the instruction in
    MEM_INSN.  First the form is determined using the DECISION_TABLE
@@ -629,7 +746,8 @@ try_merge (void)
 
   /* Look to see if the inc register is dead after the memory
      reference.  If it is, do not do the combination.  */
-  if (find_regno_note (last_insn, REG_DEAD, REGNO (inc_reg)))
+  if (find_regno_note (last_insn, REG_DEAD, REGNO (inc_reg))
+      && !convert_to_post_inc (last_insn, &inc_reg, size))
     {
       if (dump_file)
 	fprintf (dump_file, "dead failure %d\n", REGNO (inc_reg));
@@ -743,6 +861,237 @@ get_next_ref (int regno, basic_block bb,
   return insn;
 }
 
+/*
+ * Convert the form mem[a+x] to something more useful.
+ * This is the case if a was not yet seen and there is a ladder at least
+ * N matching offets.
+ *   [a+z]
+ *   [a+z+i]
+ *   [a+z+i+j]
+ *   [a+z+i+j+k]
+ *   where i,j,k, ... are candidates for post_inc.
+ *
+ *   if a is not reg_dead or z is not 0, introduce a tmp reg b
+ *   b = a+z
+ *   [b]
+ *   [b+i]
+ *   [b+i+j]
+ *   [b+i+j+k]
+ *
+ *   then convert it into
+ *   [b]
+ *   b = b + i
+ *   [b]
+ *   b = b + j
+ *   [b]
+ *   b = b + k
+ *   [b]
+ *
+ *   try dest first, a 2nd pass will try src
+ */
+#define AUTO_INC_CONVERSION_THRESHOLD 3
+#define MAX_STACK 1000
+#define MAX_ADD_REGS 32
+static int regs_added;
+
+static void
+convert_mem_offset_to_add (rtx_insn *insn, basic_block bb, bool use_src)
+{
+  rtx_insn *insn_stack[MAX_STACK];
+  rtx *loc_stack[MAX_STACK];
+  rtx *x;
+  rtx reg, pat;
+  int offset, count;
+
+  if (regs_added == MAX_ADD_REGS)
+    return;
+
+  pat = single_set (insn);
+  if (!pat)
+    return;
+
+  x = &SET_DEST(pat);
+  if (use_src || !MEM_P(*x) || GET_CODE (XEXP (*x, 0)) != PLUS
+  || !REG_P (reg = XEXP(XEXP (*x, 0), 0))
+  || !CONST_INT_P(XEXP(XEXP (*x, 0), 1)))
+    {
+      x = &SET_SRC(pat);
+      if (!MEM_P(*x) || GET_CODE (XEXP (*x, 0)) != PLUS
+      || !REG_P (reg = XEXP(XEXP (*x, 0), 0))
+      || !CONST_INT_P(XEXP(XEXP (*x, 0), 1)))
+	return;
+      use_src = true;
+    }
+
+  /* do nothing if there is an overlap. */
+  if (reg_overlap_mentioned_p (
+      reg, x == &SET_DEST(pat) ? SET_SRC(pat) : SET_DEST(pat)))
+    return;
+
+  offset = INTVAL(XEXP (XEXP (*x, 0), 1));
+  insn_stack[0] = insn;
+  loc_stack[0] = x;
+  count = 1;
+
+  /* search backwards matching candidates. */
+  while (count < MAX_STACK)
+    {
+      int prev_offset;
+
+      insn = PREV_INSN (insn);
+      if (!insn || BLOCK_FOR_INSN (insn) != bb)
+	break;
+
+      if (!NONDEBUG_INSN_P(insn))
+	continue;
+
+      /* Do not modify stuff across JUMP/LABEL. */
+      if (JUMP_P (insn) || LABEL_P(insn))
+	break;
+
+      /* no need to inspect insns not mentioning the reg. */
+      pat = PATTERN (insn);
+      if (!reg_overlap_mentioned_p (reg, pat))
+	continue;
+
+      pat = single_set (insn);
+      if (!pat)
+	return;
+
+      /* find the matching side: SRC or DEST. */
+      if (reg_overlap_mentioned_p (reg, SET_SRC(pat)))
+	{
+	  /* used on both sides -> can't handle. */
+	  if (reg_overlap_mentioned_p (reg, SET_DEST(pat)))
+	    return;
+
+	  x = &SET_SRC(pat);
+	}
+      else
+	x = &SET_DEST(pat);
+
+      /* same form: [a + x] ? */
+      if (!MEM_P(*x) || GET_CODE (XEXP (*x, 0)) != PLUS
+      || !REG_P (reg = XEXP(XEXP (*x, 0), 0))
+      || !CONST_INT_P(XEXP(XEXP (*x, 0), 1)))
+	return;
+
+      prev_offset = INTVAL(XEXP (XEXP (*x, 0), 1));
+
+      /* candidate for post_inc ? */
+      if (prev_offset + GET_MODE_SIZE(GET_MODE (*x)) != offset)
+	return;
+
+      /* record insn and location. */
+      insn_stack[count] = insn;
+      loc_stack[count] = x;
+      ++count;
+      offset = prev_offset;
+    }
+
+  if (count == 1)
+    return;
+
+  /* we have a valid list of insns with locations in the local stack. */
+
+  /* is an additional register needed?
+   * it's needed if the offset does not start at 0
+   * or the register is not dead at end.
+   */
+  insn = insn_stack[0];
+  if (offset != 0 || !find_regno_note (insn, REG_DEAD, REGNO(reg))
+      || get_next_ref (REGNO(reg), bb, reg_next_use))
+    {
+      rtx new_reg, move;
+
+      if (count <= AUTO_INC_CONVERSION_THRESHOLD)
+	{
+	  /* not enough beef... but try handle the other side.*/
+	  if (!use_src)
+	    convert_mem_offset_to_add (insn, bb, true);
+	  return;
+	}
+
+      /* introduce a new temp register. */
+      new_reg = gen_reg_rtx (SImode);
+
+      if (dump_file)
+	fprintf (dump_file, "use new reg %d\n", REGNO(new_reg));
+
+      /* prepend the move. */
+      if (offset == 0)
+	move = gen_rtx_SET(new_reg, reg);
+      else
+	move = gen_rtx_SET(new_reg,
+			   gen_rtx_PLUS (SImode, reg, GEN_INT (offset)));
+
+      if (dump_file)
+	{
+	  fprintf (dump_file, "from:\n");
+	  for (int i = count - 1; i >= 0; --i)
+	    dump_insn_slim (dump_file, insn_stack[i]);
+	}
+
+      /* last get's the plane register into the MEM. */
+      validate_change (insn_stack[count - 1], loc_stack[count - 1],
+		       replace_equiv_address_nv (*loc_stack[count - 1], new_reg),
+		       true);
+
+      /* update all insns. */
+      for (int i = 0; i < count - 1; ++i)
+	validate_change (insn_stack[i], loc_stack[i],
+		         replace_equiv_address_nv (*loc_stack[i],
+				       gen_rtx_PLUS(SImode, new_reg,
+						    GEN_INT(INTVAL (XEXP (XEXP (*loc_stack[i], 0), 1)) - offset))), true);
+
+      if (!apply_change_group ())
+	return;
+
+      /* update reg and add reg_dead. */
+      reg = new_reg;
+
+      add_reg_note (insn_stack[0], REG_DEAD, reg);
+
+      insn = emit_insn_before (move, insn_stack[count - 1]);
+
+      if (dump_file)
+	{
+	  fprintf (dump_file, "intermediate:");
+	  dump_insn_slim (dump_file, insn);
+	  for (int i = count - 1; i >= 0; --i)
+	    dump_insn_slim (dump_file, insn_stack[i]);
+	}
+      ++regs_added;
+    }
+
+  if (dump_file)
+    fprintf (dump_file, "converted:\n");
+  /* reg is a reg_dead register and
+   * at the first insn == insn_stack[count - 1], the offset is 0.
+   * convert the insn to use the register direct and append a PLUS.
+   */
+  for (int i = count - 1; i >= 0; --i)
+    {
+      machine_mode mode = GET_MODE(*loc_stack[i]);
+      if (i < count - 1)
+	validate_change (insn_stack[i], loc_stack[i],
+		         replace_equiv_address_nv (*loc_stack[i], reg),
+			 false);
+      if (i > 0)
+	insn = emit_insn_after (
+	    gen_rtx_SET(
+		reg, gen_rtx_PLUS(SImode, reg, GEN_INT (GET_MODE_SIZE (mode)))),
+	    insn_stack[i]);
+      if (dump_file)
+	{
+	  dump_insn_slim (dump_file, insn_stack[i]);
+	  dump_insn_slim (dump_file, insn);
+	}
+    }
+
+  if (!use_src)
+    convert_mem_offset_to_add (insn, bb, true);
+}
 
 /* Return true if INSN is of a form "a = b op c" where a and b are
    regs.  op is + if c is a reg and +|- if c is a const.  Fill in
@@ -1323,6 +1672,8 @@ merge_in_block (int max_reg, basic_block
       if (dump_file)
 	dump_insn_slim (dump_file, insn);
 
+      convert_mem_offset_to_add(insn, bb, false);
+
       /* Does this instruction increment or decrement a register?  */
       if (parse_add_or_inc (insn, true))
 	{
@@ -1417,9 +1768,25 @@ merge_in_block (int max_reg, basic_block
     {
       /* In this case, we must clear these vectors since the trick of
 	 testing if the stale insn in the block will not work.  */
-      memset (reg_next_use, 0, max_reg * sizeof (rtx));
-      memset (reg_next_inc_use, 0, max_reg * sizeof (rtx));
-      memset (reg_next_def, 0, max_reg * sizeof (rtx));
+
+      if (regs_added)
+	{
+	  regs_added = 0;
+	  max_reg = max_reg_num () + MAX_ADD_REGS;
+	  free (reg_next_use);
+	  free (reg_next_inc_use);
+	  free (reg_next_def);
+
+	  reg_next_use = XCNEWVEC (rtx_insn *, max_reg);
+	  reg_next_inc_use = XCNEWVEC (rtx_insn *, max_reg);
+	  reg_next_def = XCNEWVEC (rtx_insn *, max_reg);
+	}
+      else
+	{
+	  memset (reg_next_use, 0, max_reg * sizeof (rtx));
+	  memset (reg_next_inc_use, 0, max_reg * sizeof (rtx));
+	  memset (reg_next_def, 0, max_reg * sizeof (rtx));
+	}
       df_recompute_luids (bb);
       merge_in_block (max_reg, bb);
     }
@@ -1470,7 +1837,7 @@ pass_inc_dec::execute (function *fun ATT
     return 0;
 
   basic_block bb;
-  int max_reg = max_reg_num ();
+  int max_reg = max_reg_num () + MAX_ADD_REGS;
 
   if (!initialized)
     init_decision_table ();

--- gcc/BASE-VER	2018-10-26 10:54:34.000000000 +0100
+++ gcc/BASE-VER	2025-11-02 00:56:37.178266694 +0000
@@ -1 +1 @@
-6.5.0
+6.5.0b
Only in ../gcc/gcc: bbb-baserel.c
Only in ../gcc/gcc: bbb-opts.c

--- gcc/c/c-decl.c	2018-06-25 18:08:02.000000000 +0100
+++ gcc/c/c-decl.c	2025-11-02 00:56:37.718274869 +0000
@@ -51,6 +51,8 @@ along with GCC; see the file COPYING3.
 #include "c-family/c-ada-spec.h"
 #include "cilk.h"
 #include "builtins.h"
+#include "output.h"
+#include "tm_p.h"
 
 /* In grokdeclarator, distinguish syntactic contexts of declarators.  */
 enum decl_context
@@ -1791,6 +1793,30 @@ locate_old_decl (tree decl)
     inform (input_location, "previous declaration of %q+D was here", decl);
 }
 
+#if defined(TARGET_M68K)
+/**
+ * Filter __stkparm__ storage attributes from type's attributes.
+ */
+static tree strip_m68k_stkparm_attrs(tree t1)
+{
+	tree filtered = NULL;
+
+    for (;t1; t1 = TREE_CHAIN (t1))
+	{
+    	  char const * name = IDENTIFIER_POINTER(TREE_PURPOSE (t1));
+    	  if (0 == strcmp("__stkparm__", name))
+    	    continue;
+
+	  tree a1 = copy_node (t1);
+	  TREE_CHAIN (a1) = filtered;
+	  filtered = a1;
+	}
+
+	return filtered;
+}
+#endif
+
+
 /* Subroutine of duplicate_decls.  Compare NEWDECL to OLDDECL.
    Returns true if the caller should proceed to merge the two, false
    if OLDDECL should simply be discarded.  As a side effect, issues
@@ -1858,7 +1884,21 @@ diagnose_mismatched_decls (tree newdecl,
 	     This is for the ffs and fprintf builtins.  */
 	  tree trytype = match_builtin_function_types (newtype, oldtype);
 
+#if defined(TARGET_M68K)
+	  bool ok = false;
+	  if (trytype)
+	    {
+	      tree saved_attrs = TYPE_ATTRIBUTES(newtype);
+	      tree stkp = lookup_attribute ("stkparm", saved_attrs);
+	      if (stkp)
+		TYPE_ATTRIBUTES(newtype) = strip_m68k_stkparm_attrs(saved_attrs);
+	      ok = comptypes (newtype, trytype);
+	      TYPE_ATTRIBUTES(newtype) = saved_attrs;
+	    }
+	  if (ok)
+#else
 	  if (trytype && comptypes (newtype, trytype))
+#endif
 	    *oldtypep = oldtype = trytype;
 	  else
 	    {
@@ -4439,7 +4479,58 @@ c_decl_attributes (tree *node, tree attr
 	attributes = tree_cons (get_identifier ("omp declare target"),
 				NULL_TREE, attributes);
     }
-  return decl_attributes (node, attributes, flags);
+
+  tree returned_attrs = decl_attributes (node, attributes, flags);
+
+#if defined(TARGET_M68K)
+  /* add an attribute to the function decl's type if there are asm register parameters. */
+  if (TREE_CODE (*node) == FUNCTION_DECL)
+    {
+      char const * synthetic = "";
+      for (tree params = TYPE_ARG_TYPES(TREE_TYPE(*node)); params; params = TREE_CHAIN(params))
+	{
+	  tree asmattr = lookup_attribute("asmreg", TYPE_ATTRIBUTES(TREE_VALUE(params)));
+	  if (asmattr)
+	    synthetic = concat(synthetic, reg_names[TREE_INT_CST_LOW(TREE_VALUE(TREE_VALUE(asmattr)))], NULL);
+	}
+      if (strlen(synthetic) > 0)
+	{
+	  tree t;
+	  tree asmid = get_identifier("asmregs");
+	  tree syntheticid = get_identifier(synthetic);
+	  tree value = tree_cons(syntheticid, syntheticid, NULL_TREE);
+	  tree newattr = tree_cons(asmid, value, NULL_TREE);
+
+	  /* create a type copy with additional attribute. */
+	  tree atype = copy_node (TREE_TYPE(*node));
+	  tree attrs = TYPE_ATTRIBUTES(atype) = chainon(newattr, TYPE_ATTRIBUTES(atype));
+
+	  tree m = TYPE_MAIN_VARIANT(TREE_TYPE(*node));
+
+	  TYPE_POINTER_TO (atype) = 0;
+	  TYPE_REFERENCE_TO (atype) = 0;
+
+	  /* search if such variant exists. */
+	  for (t = m; t; t = TYPE_NEXT_VARIANT(t))
+	    if (comptypes (t, atype) == 1 && attribute_list_equal (TYPE_ATTRIBUTES(t), attrs))
+	      break;
+
+	  if (t)
+	    TREE_TYPE(*node) = t;
+	  else
+	    {
+	      TREE_TYPE(*node) = atype;
+	      /* Add this type to the chain of variants of TYPE.  */
+	      TYPE_NEXT_VARIANT (atype) = TYPE_NEXT_VARIANT (m);
+	      TYPE_NEXT_VARIANT (m) = atype;
+	    }
+
+	  returned_attrs = TYPE_ATTRIBUTES(TREE_TYPE(*node));
+	}
+    }
+#endif
+
+  return returned_attrs;
 }
 
 
@@ -5024,6 +5115,30 @@ grokparm (const struct c_parm *parm, tre
   return decl;
 }
 
+#if defined(TARGET_M68K)
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
+
 /* Given a parsed parameter declaration, decode it into a PARM_DECL
    and push that on the current scope.  EXPR is a pointer to an
    expression that needs to be evaluated for the side effects of array
@@ -5041,6 +5156,58 @@ push_parm_decl (const struct c_parm *par
 
   decl = pushdecl (decl);
 
+#if defined(TARGET_M68K)
+  if (parm->asmspec)
+    {
+      tree atype = TREE_TYPE(decl);
+      const char *asmspec = TREE_STRING_POINTER(parm->asmspec);
+      if (*asmspec == '%')
+	++asmspec;
+      int reg_number = decode_reg_name (asmspec);
+
+      /* First detect errors in declaring global registers.  */
+      if (reg_number == -1)
+	error ("%s register name not specified for %qD", asmspec, decl);
+      else if (reg_number < 0)
+	error ("%s invalid register name for %qD", asmspec, decl);
+      else if (TYPE_MODE (TREE_TYPE (decl)) == BLKmode)
+	error ("%s data type of %qD isn%'t suitable for a register", asmspec, decl);
+      else if (!HARD_REGNO_MODE_OK(reg_number, TYPE_MODE (TREE_TYPE (decl))))
+	error ("%s register specified for %qD isn%'t suitable for data type",
+	       asmspec, decl);
+      /* Now handle properly declared static register variables.  */
+      else
+	{
+	  /* Build tree for __attribute__ ((asm(regnum))). */
+	  tree ttasm = get_identifier("asmreg");
+	  tree value = tree_cons(ttasm, build_int_cst(NULL, reg_number), NULL_TREE);
+	  tree t, attrs = tree_cons(ttasm, value, NULL_TREE);
+	  /* First check whether such a type already exists - if yes, use
+	   that one. This is very important, since otherwise
+	   common_type() would think that it sees two different
+	   types and would try to merge them - this could result in
+	   warning messages. */
+	  for (t = TYPE_MAIN_VARIANT(atype); t; t = TYPE_NEXT_VARIANT(t))
+	    if (comptypes (t, atype) == 1
+		&& attribute_list_equal (TYPE_ATTRIBUTES(t), attrs))
+	      break;
+	  if (t)
+	    atype = t;
+	  else
+	    {
+	      /* Create a new variant, with differing attributes.
+	       (Hack! Type with differing attributes should no longer be
+	       a variant of its main type. See comment above for
+	       explanation why this was necessary). */
+	      atype = build_type_copy (atype);
+	      TYPE_ATTRIBUTES(atype) = chainon (attrs, TYPE_ATTRIBUTES(atype));
+	    }
+	  TREE_TYPE(decl) = atype;
+//	  printf("%s using %s, cdecl=%p, type=%p\n", IDENTIFIER_POINTER(DECL_NAME (decl)), asmspec, decl, atype);
+	}
+    }
+#endif
+
   finish_decl (decl, input_location, NULL_TREE, NULL_TREE, NULL_TREE);
 }
 

--- gcc/c/c-fold.c	2016-04-04 09:11:46.000000000 +0100
+++ gcc/c/c-fold.c	2025-11-02 00:56:37.718274869 +0000
@@ -591,6 +591,10 @@ c_fully_fold_internal (tree expr, bool i
 tree
 decl_constant_value_for_optimization (tree exp)
 {
+#if defined(TARGET_M68K)
+  /* SBF: m68k doesn't like that early optimization. */
+  return exp;
+#else
   tree ret;
 
   if (!optimize
@@ -606,4 +610,5 @@ decl_constant_value_for_optimization (tr
   if (ret != exp && TREE_STATIC (exp))
     ret = unshare_expr (ret);
   return ret;
+#endif
 }

--- gcc/c/c-parser.c	2017-12-10 21:37:08.000000000 +0000
+++ gcc/c/c-parser.c	2025-11-02 00:56:37.722274930 +0000
@@ -3837,10 +3837,26 @@ c_parser_parameter_declaration (c_parser
       c_parser_skip_until_found (parser, CPP_COMMA, NULL);
       return NULL;
     }
+  /**
+   * SBF: Add support for __asm("xy") register spec.
+   */
+#if defined(TARGET_M68K)
+  tree asmspec = NULL_TREE;
+  if (c_parser_next_token_is_keyword (parser, RID_ASM))
+    {
+      asmspec = c_parser_simple_asm_expr (parser);
+//	printf("asmspec: %s\n", TREE_STRING_POINTER(asmspec));
+    }
+#endif
   if (c_parser_next_token_is_keyword (parser, RID_ATTRIBUTE))
     postfix_attrs = c_parser_attributes (parser);
-  return build_c_parm (specs, chainon (postfix_attrs, prefix_attrs),
+
+  struct c_parm * cparm = build_c_parm (specs, chainon (postfix_attrs, prefix_attrs),
 		       declarator);
+#if defined(TARGET_M68K)
+  cparm->asmspec = asmspec;
+#endif
+  return cparm;
 }
 
 /* Parse a string literal in an asm expression.  It should not be
@@ -3892,6 +3908,7 @@ c_parser_asm_string_literal (c_parser *p
 static tree
 c_parser_simple_asm_expr (c_parser *parser)
 {
+  extern int in_assembler_directive;
   tree str;
   gcc_assert (c_parser_next_token_is_keyword (parser, RID_ASM));
   /* ??? Follow the C++ parser rather than using the
@@ -3903,7 +3920,13 @@ c_parser_simple_asm_expr (c_parser *pars
       parser->lex_untranslated_string = false;
       return NULL_TREE;
     }
+
+  // SBF: set in_assembler_directive to enable multi-line strings. And yes, it's a HACK.
+  in_assembler_directive = 1;
   str = c_parser_asm_string_literal (parser);
+  // SBF: in_assembler_directive disabled
+  in_assembler_directive = 0;
+
   parser->lex_untranslated_string = false;
   if (!c_parser_require (parser, CPP_CLOSE_PAREN, "expected %<)%>"))
     {

--- gcc/c/c-tree.h	2016-05-19 21:33:51.000000000 +0100
+++ gcc/c/c-tree.h	2025-11-02 00:56:37.722274930 +0000
@@ -453,6 +453,10 @@ struct c_parm {
   tree attrs;
   /* The declarator.  */
   struct c_declarator *declarator;
+#if defined(TARGET_M68K)
+  /* The optional asm spec to specify the register. */
+  tree asmspec;
+#endif
 };
 
 /* Used when parsing an enum.  Initialized by start_enum.  */

--- gcc/c-family/c-common.c	2018-06-25 18:54:30.000000000 +0100
+++ gcc/c-family/c-common.c	2025-11-02 00:56:37.710274748 +0000
@@ -284,7 +284,9 @@ struct fname_var_t
 };
 
 /* The three ways of getting then name of the current function.  */
-
+#ifdef __amiga__
+__near
+#endif
 const struct fname_var_t fname_vars[] =
 {
   /* C99 compliant __func__, must be first.  */
@@ -6557,6 +6559,9 @@ attribute_takes_identifier_p (const_tree
     return true;
   else if (!strcmp ("mode", spec->name)
 	   || !strcmp ("format", spec->name)
+#ifdef TARGET_M68K
+	   || !strcmp ("regparm", spec->name)
+#endif
 	   || !strcmp ("cleanup", spec->name))
     return true;
   else

--- gcc/c-family/c-cppbuiltin.c	2017-09-10 10:05:31.000000000 +0100
+++ gcc/c-family/c-cppbuiltin.c	2025-11-02 00:56:37.710274748 +0000
@@ -18,6 +18,7 @@ along with GCC; see the file COPYING3.
 <http://www.gnu.org/licenses/>.  */
 
 #include "config.h"
+#include "configargs.h"
 #include "system.h"
 #include "coretypes.h"
 #include "target.h"

--- gcc/c-family/c-format.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/c-family/c-format.c	2025-11-02 00:56:37.710274748 +0000
@@ -669,7 +669,9 @@ static const format_flag_pair strfmon_fl
   { 0, 0, 0, 0 }
 };
 
-
+#ifdef __amiga__
+__near
+#endif
 static const format_char_info print_char_table[] =
 {
   /* C89 conversion specifiers.  */
@@ -693,6 +695,9 @@ static const format_char_info print_char
   { NULL,  0, STD_C89, NOLENGTHS, NULL, NULL, NULL }
 };
 
+#ifdef __amiga__
+__near
+#endif
 static const format_char_info asm_fprintf_char_table[] =
 {
   /* C89 conversion specifiers.  */
@@ -713,6 +718,9 @@ static const format_char_info asm_fprint
   { NULL,  0, STD_C89, NOLENGTHS, NULL, NULL, NULL }
 };
 
+#ifdef __amiga__
+__near
+#endif
 static const format_char_info gcc_diag_char_table[] =
 {
   /* C89 conversion specifiers.  */
@@ -734,6 +742,9 @@ static const format_char_info gcc_diag_c
   { NULL,  0, STD_C89, NOLENGTHS, NULL, NULL, NULL }
 };
 
+#ifdef __amiga__
+__near
+#endif
 static const format_char_info gcc_tdiag_char_table[] =
 {
   /* C89 conversion specifiers.  */
@@ -757,6 +768,9 @@ static const format_char_info gcc_tdiag_
   { NULL,  0, STD_C89, NOLENGTHS, NULL, NULL, NULL }
 };
 
+#ifdef __amiga__
+__near
+#endif
 static const format_char_info gcc_cdiag_char_table[] =
 {
   /* C89 conversion specifiers.  */
@@ -780,6 +794,9 @@ static const format_char_info gcc_cdiag_
   { NULL,  0, STD_C89, NOLENGTHS, NULL, NULL, NULL }
 };
 
+#ifdef __amiga__
+__near
+#endif
 static const format_char_info gcc_cxxdiag_char_table[] =
 {
   /* C89 conversion specifiers.  */
@@ -806,6 +823,9 @@ static const format_char_info gcc_cxxdia
   { NULL,  0, STD_C89, NOLENGTHS, NULL, NULL, NULL }
 };
 
+#ifdef __amiga__
+__near
+#endif
 static const format_char_info gcc_gfc_char_table[] =
 {
   /* C89 conversion specifiers.  */
@@ -826,6 +846,9 @@ static const format_char_info gcc_gfc_ch
   { NULL,  0, STD_C89, NOLENGTHS, NULL, NULL, NULL }
 };
 
+#ifdef __amiga__
+__near
+#endif
 static const format_char_info scan_char_table[] =
 {
   /* C89 conversion specifiers.  */
@@ -874,6 +897,9 @@ static const format_char_info time_char_
   { NULL,		0, STD_C89, NOLENGTHS, NULL, NULL, NULL }
 };
 
+#ifdef __amiga__
+__near
+#endif
 static const format_char_info monetary_char_table[] =
 {
   { "in", 0, STD_C89, { T89_D, BADLEN, BADLEN, BADLEN, BADLEN, T89_LD, BADLEN, BADLEN, BADLEN, BADLEN, BADLEN, BADLEN }, "=^+(!-w#p", "", NULL },
@@ -881,6 +907,9 @@ static const format_char_info monetary_c
 };
 
 /* This must be in the same order as enum format_type.  */
+#ifdef __amiga__
+__near
+#endif
 static const format_kind_info format_types_orig[] =
 {
   { "gnu_printf",   printf_length_specs,  print_char_table, " +#0-'I", NULL,

--- gcc/c-family/c-pch.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/c-family/c-pch.c	2025-11-02 00:56:37.714274809 +0000
@@ -32,7 +32,9 @@ along with GCC; see the file COPYING3.
 /* This is a list of flag variables that must match exactly, and their
    names for the error message.  The possible values for *flag_var must
    fit in a 'signed char'.  */
-
+#ifdef __amiga__
+__near
+#endif
 static const struct c_pch_matching
 {
   int *flag_var;

--- gcc/cfgcleanup.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/cfgcleanup.c	2025-11-02 00:56:37.730275051 +0000
@@ -931,6 +931,9 @@ merge_memattrs (rtx x, rtx y)
 	  MEM_VOLATILE_P (x) = 1;
 	  MEM_VOLATILE_P (y) = 1;
 	}
+
+      if (x->in_struct != y->in_struct)
+	x->in_struct = y->in_struct = 0;
     }
 
   fmt = GET_RTX_FORMAT (code);
@@ -1401,7 +1404,13 @@ flow_find_cross_jump (basic_block bb1, b
 	  afterlast_dir = last_dir;
 	  last_dir = dir;
 	  if (active_insn_p (i1))
-	    ninsns++;
+	    if (!single_set(i1)
+		|| GET_CODE(PATTERN(i1)) != SET
+		|| GET_CODE(SET_DEST(PATTERN(i1))) != CC0
+		)
+	      ++ninsns;
+	    else
+	      --ninsns;
 	}
 
       i1 = PREV_INSN (i1);
@@ -1657,8 +1666,9 @@ outgoing_edges_match (int mode, basic_bl
 	 we require the existing branches to have probabilities that are
 	 roughly similar.  */
       if (match
-	  && optimize_bb_for_speed_p (bb1)
-	  && optimize_bb_for_speed_p (bb2))
+//	  && optimize_bb_for_speed_p (bb1)
+//	  && optimize_bb_for_speed_p (bb2)
+	  )
 	{
 	  int prob2;
 
@@ -2001,6 +2011,14 @@ try_crossjump_to_edge (int mode, edge e1
 	{
 	  rtx_insn *insn;
 
+#if defined(TARGET_AMIGAOS)
+	  /* SBF: we need replicated labels, if the labels are too far away,
+	   * since on 68000 there are only 8 bits for the offset.
+	   */
+	  if (!TARGET_68020 && !TARGET_68040 && !TARGET_68080)
+	    return false;
+#endif
+
 	  /* Replace references to LABEL1 with LABEL2.  */
 	  for (insn = get_insns (); insn; insn = NEXT_INSN (insn))
 	    {
@@ -2016,8 +2034,9 @@ try_crossjump_to_edge (int mode, edge e1
   /* Avoid splitting if possible.  We must always split when SRC2 has
      EH predecessor edges, or we may end up with basic blocks with both
      normal and EH predecessor edges.  */
-  if (newpos2 == BB_HEAD (src2)
+  if ((newpos2 == BB_HEAD (src2)
       && !(EDGE_PRED (src2, 0)->flags & EDGE_EH))
+      )
     redirect_to = src2;
   else
     {
@@ -3045,8 +3064,8 @@ cleanup_cfg (int mode)
 	  if ((mode & CLEANUP_EXPENSIVE) && !reload_completed
 	      && !delete_trivially_dead_insns (get_insns (), max_reg_num ()))
 	    break;
-	  if ((mode & CLEANUP_CROSSJUMP) && crossjumps_occured)
-	    run_fast_dce ();
+//	  if ((mode & CLEANUP_CROSSJUMP) && crossjumps_occured)
+//	    run_fast_dce ();
 	}
       else
 	break;

--- gcc/cfgexpand.c	2018-06-25 18:51:44.000000000 +0100
+++ gcc/cfgexpand.c	2025-11-02 00:56:37.730275051 +0000
@@ -1530,6 +1530,7 @@ defer_stack_allocation (tree var, bool t
      with every other variable.  The only reason to want to defer them
      at all is that, after sorting, we can more efficiently pack
      small variables in the stack frame.  Continue to defer at -O2.  */
+  /* SBF: do not check optimize level for now. */   
   if (toplevel && optimize < 2)
     return false;
 
@@ -2732,6 +2733,10 @@ tree_conflicts_with_clobbers_p (tree t,
 {
   /* Conflicts between asm-declared register variables and the clobber
      list are not allowed.  */
+  /*
+   * SBF: Why?
+   */
+#if 0 && !defined(TARGET_M68K)
   tree overlap = tree_overlaps_hard_reg_set (t, clobbered_regs);
 
   if (overlap)
@@ -2744,7 +2749,7 @@ tree_conflicts_with_clobbers_p (tree t,
       DECL_REGISTER (overlap) = 0;
       return true;
     }
-
+#endif
   return false;
 }
 
@@ -3253,13 +3258,17 @@ expand_asm_stmt (gasm *stmt)
 		 tripping over the under-construction body.  */
 	      for (unsigned k = 0; k < noutputs; ++k)
 		if (reg_overlap_mentioned_p (clobbered_reg, output_rvec[k]))
-		  internal_error ("asm clobber conflict with output operand");
+		  error ("asm clobber conflict with output operand");
 
+/**
+ * SBF: Why?
+ */
+#if 0 && !defined(TARGET_M68K)
 	      for (unsigned k = 0; k < ninputs - ninout; ++k)
 		if (reg_overlap_mentioned_p (clobbered_reg, input_rvec[k]))
-		  internal_error ("asm clobber conflict with input operand");
+		  error ("asm clobber conflict with input operand");
+#endif
 	    }
-
 	  XVECEXP (body, 0, i++) = gen_rtx_CLOBBER (VOIDmode, clobbered_reg);
 	}
 

--- gcc/ChangeLog	2018-10-26 10:54:34.000000000 +0100
+++ gcc/ChangeLog	2025-11-02 00:56:37.182266755 +0000
@@ -1,3 +1,10 @@
+2018-11-07  Max Filippov  <jcmvbkbc@gmail.com>
+
+	Backport from mainline
+	2018-11-05  Max Filippov  <jcmvbkbc@gmail.com>
+
+	* config/xtensa/uclinux.h (XTENSA_ALWAYS_PIC): Change to 0.
+
 2018-10-26  Release Manager
 
 	* GCC 6.5.0 released.

--- gcc/collect2.c	2018-07-01 15:57:03.000000000 +0100
+++ gcc/collect2.c	2025-11-02 00:56:37.738275172 +0000
@@ -1089,6 +1089,9 @@ main (int argc, char **argv)
 
   /* Extract COMPILER_PATH and PATH into our prefix list.  */
   prefix_from_env ("COMPILER_PATH", &cpath);
+#ifdef __amiga__
+  setenv("PATH", "GCC:bin", 0);
+#endif
   prefix_from_env ("PATH", &path);
 
   /* Try to discover a valid linker/nm/strip to use.  */
@@ -1307,6 +1310,8 @@ main (int argc, char **argv)
 		  /* Do not pass LTO flag to the linker. */
 		  ld1--;
 		  ld2--;
+
+		  lto_mode = LTO_MODE_LTO;
 #else
 		  error ("LTO support has not been enabled in this "
 			 "configuration");
@@ -1414,6 +1419,11 @@ main (int argc, char **argv)
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
@@ -1516,6 +1526,11 @@ main (int argc, char **argv)
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
 
@@ -1636,6 +1651,11 @@ main (int argc, char **argv)
 
       fprintf (stderr, "\n");
     }
+  /* begin-GG-local: dynamic libraries */
+#ifdef COLLECT2_PRELINK_HOOK
+    COLLECT2_PRELINK_HOOK(ld1_argv, &strip_flag);
+#endif
+  /* end-GG-local */
 
   /* Load the program, searching all libraries and attempting to provide
      undefined symbols from repository information.
@@ -1655,7 +1675,7 @@ main (int argc, char **argv)
        expect the relevant tables to be dragged together with their associated
        functions from precise cross reference insertions by the compiler.  */
 
-    if (early_exit || ld1_filter != SCAN_NOTHING)
+    if (early_exit || (ld1_filter != SCAN_NOTHING && lto_mode != LTO_MODE_LTO))
       do_tlink (ld1_argv, object_lst);
 
     if (early_exit)
@@ -1676,6 +1696,8 @@ main (int argc, char **argv)
       }
   }
 
+  /* begin-GG-local: dynamic libraries */
+#ifndef COLLECT2_POSTLINK_HOOK
   /* Unless we have done it all already, examine the namelist and search for
      static constructors and destructors to call.  Write the constructor and
      destructor tables to a .s file and reload.  */
@@ -1702,6 +1724,10 @@ main (int argc, char **argv)
 				   frame_tables.number),
                          frame_tables.number);
     }
+#else /* COLLECT2_POSTLINK_HOOK */
+  COLLECT2_POSTLINK_HOOK(output_file);
+#endif
+/* end-GG-local */
 
   /* If the scan exposed nothing of special interest, there's no need to
      generate the glue code and relink so return now.  */
@@ -1744,6 +1770,11 @@ main (int argc, char **argv)
 
       maybe_unlink (c_file);
       maybe_unlink (o_file);
+      /* begin-GG-local: dynamic libraries */
+#ifdef COLLECT2_EXTRA_CLEANUP
+            COLLECT2_EXTRA_CLEANUP();
+#endif
+      /* end-GG-local */
       return 0;
     }
 
@@ -1849,6 +1880,11 @@ main (int argc, char **argv)
   maybe_unlink (export_file);
 #endif
 
+  /* begin-GG-local: dynamic libraries */
+#ifdef COLLECT2_EXTRA_CLEANUP
+    COLLECT2_EXTRA_CLEANUP();
+#endif
+  /* end-GG-local */
   return 0;
 }
 
@@ -2302,28 +2338,66 @@ maybe_lto_object_file (const char *prog_
   static unsigned char coffmagic[2] = { 0x4c, 0x01 };
   static unsigned char coffmagic_x64[2] = { 0x64, 0x86 };
   static unsigned char machomagic[4][4] = {
-    { 0xcf, 0xfa, 0xed, 0xfe },
-    { 0xce, 0xfa, 0xed, 0xfe },
-    { 0xfe, 0xed, 0xfa, 0xcf },
+      { 0xcf, 0xfa, 0xed, 0xfe },
+      { 0xce, 0xfa, 0xed, 0xfe },
+      { 0xfe, 0xed, 0xfa, 0xcf },
     { 0xfe, 0xed, 0xfa, 0xce }
   };
 
-  f = fopen (prog_name, "rb");
-  if (f == NULL)
-    return false;
-  if (fread (buf, sizeof (buf), 1, f) != 1)
-    buf[0] = 0;
-  fclose (f);
-
-  if (memcmp (buf, elfmagic, sizeof (elfmagic)) == 0
-      || memcmp (buf, coffmagic, sizeof (coffmagic)) == 0
-      || memcmp (buf, coffmagic_x64, sizeof (coffmagic_x64)) == 0)
-    return true;
-  for (i = 0; i < 4; i++)
-    if (memcmp (buf, machomagic[i], sizeof (machomagic[i])) == 0)
-      return true;
+  bool r = false;
+  do
+    { // while (0)
+
+      f = fopen (prog_name, "rb");
+      if (f == NULL)
+	break;
+      if (fread (buf, sizeof(buf), 1, f) != 1)
+	break;
+
+      if (memcmp (buf, elfmagic, sizeof(elfmagic)) == 0
+	  || memcmp (buf, coffmagic, sizeof(coffmagic)) == 0
+	  || memcmp (buf, coffmagic_x64, sizeof(coffmagic_x64)) == 0)
+	{
+	  r = true;
+	  break;
+	}
+      for (i = 0; i < 4; i++)
+	if (memcmp (buf, machomagic[i], sizeof(machomagic[i])) == 0)
+	  {
+	    r = true;
+	    break;
+	  }
+
+#if defined(TARGET_AMIGAOS)
+      /* SBF: check amiga hunk for gnu lto section name. */
+      if (buf[0] == 0 && buf[1] == 0 && buf[2] == 3 && buf[3] == 0xe7)
+	{
+	  // skip file name
+	  if (fread (buf, sizeof(buf), 1, f) != 1)
+	    break;
+	  unsigned len = (buf[1] << 16) | (buf[2] << 8) | buf[3];
+	  if (fseek(f, len * 4 + 0x34, SEEK_CUR))
+	    break;
 
-  return false;
+	  if (fread (buf, sizeof(buf), 1, f) != 1)
+	    break;
+	  // symbol
+	  if (buf[0] != 0x82)
+	    break;
+	  len = (buf[1] << 16) | (buf[2] << 8) | buf[3];
+	  if (len != 3 && len != 4)
+	    break;
+	  char name[16];
+	  if (fread (name, sizeof(name), 1, f) != 1)
+	    break;
+	  r = 0 == strcmp("___gnu_lto_v1", name) || 0 == strcmp("__gnu_lto_v1", name);
+	}
+#endif
+    }
+  while (0);
+  if (f)
+    fclose (f);
+  return r;
 }
 
 /* Generic version to scan the name list of the loaded program for

--- gcc/combine.c	2018-10-12 18:32:38.000000000 +0100
+++ gcc/combine.c	2025-11-02 00:56:37.738275172 +0000
@@ -1998,7 +1998,9 @@ can_combine_p (rtx_insn *insn, rtx_insn
 
       if (REG_P (src)
 	  && ((REGNO (dest) < FIRST_PSEUDO_REGISTER
-	       && ! HARD_REGNO_MODE_OK (REGNO (dest), GET_MODE (dest)))
+	      /* SBF: don't allow a hard reg to vanish by a combine */
+	       // && ! HARD_REGNO_MODE_OK (REGNO (dest), GET_MODE (dest))
+	       )
 	      /* Don't extend the life of a hard register unless it is
 		 user variable (if we have few registers) or it can't
 		 fit into the desired register (meaning something special
@@ -5357,8 +5359,9 @@ subst (rtx x, rtx from, rtx to, int in_d
 		  && (((code == SUBREG || code == ZERO_EXTRACT)
 		       && REG_P (new_rtx))
 		      || code == STRICT_LOW_PART))
-		;
-
+		{
+		  // intentionally empty
+		}
 	      else if (COMBINE_RTX_EQUAL_P (XEXP (x, i), from))
 		{
 		  /* In general, don't install a subreg involving two

--- gcc/common/common-target.h	2016-01-04 14:30:50.000000000 +0000
+++ gcc/common/common-target.h	2025-11-02 00:56:37.742275233 +0000
@@ -37,7 +37,7 @@ enum opt_levels
   OPT_LEVELS_2_PLUS, /* -O2 and above, including -Os.  */
   OPT_LEVELS_2_PLUS_SPEED_ONLY, /* -O2 and above, but not -Os or -Og.  */
   OPT_LEVELS_3_PLUS, /* -O3 and above.  */
-  OPT_LEVELS_3_PLUS_AND_SIZE, /* -O3 and above and -Os.  */
+  OPT_LEVELS_3_PLUS_AND_SPEED_ONLY, /* -O3 and above and not -Os or -Og.  */
   OPT_LEVELS_SIZE, /* -Os only.  */
   OPT_LEVELS_FAST /* -Ofast only.  */
 };

--- gcc/common.opt	2017-06-22 12:17:56.000000000 +0100
+++ gcc/common.opt	2025-11-02 00:56:37.742275233 +0000
@@ -1424,6 +1424,10 @@ floop-nest-optimize
 Common Report Var(flag_loop_nest_optimize) Optimization
 Enable the loop nest optimizer.
 
+floop-size-optimize
+Common Report Var(flag_loop_size_optimize) Optimization Init(-1)
+Enable loop for size optimization.
+
 fstrict-volatile-bitfields
 Common Report Var(flag_strict_volatile_bitfields) Init(-1) Optimization
 Force bitfield accesses to match their type width.
@@ -1920,6 +1924,10 @@ fprofile-reorder-functions
 Common Report Var(flag_profile_reorder_functions)
 Enable function reordering that improves code placement.
 
+fprune-stack-vars
+Common Report Var(flag_prune_stack_vars)
+Try to prune read only stack variable which contain a copied value which could be used instead.
+
 frandom-seed
 Common Var(common_deferred_options) Defer
 
@@ -2275,7 +2283,7 @@ EnumValue
 Enum(tls_model) String(local-exec) Value(TLS_MODEL_LOCAL_EXEC)
 
 ftoplevel-reorder
-Common Report Var(flag_toplevel_reorder) Init(2)
+Common Report Var(flag_toplevel_reorder) Init(0)
 Reorder top level functions, variables, and asms.
 
 ftracer
@@ -2966,6 +2974,10 @@ pie
 Driver RejectNegative Negative(no-pie)
 Create a position independent executable.
 
+pthread
+Driver RejectNegative
+add -lpthread
+
 z
 Driver Joined Separate
 
Only in ../gcc/gcc/config/aarch64: host-aarch64-darwin.c
Only in ../gcc/gcc/config/aarch64: x-darwin

--- gcc/config/host-darwin.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/config/host-darwin.c	2025-11-02 00:56:37.830276565 +0000
@@ -1,5 +1,5 @@
 /* Darwin host-specific hook definitions.
-   Copyright (C) 2003-2016 Free Software Foundation, Inc.
+   Copyright (C) 2003-2021 Free Software Foundation, Inc.
 
    This file is part of GCC.
 
@@ -24,7 +24,10 @@
 #include "config/host-darwin.h"
 
 /* Yes, this is really supposed to work.  */
-static char pch_address_space[1024*1024*1024] __attribute__((aligned (4096)));
+/* This allows for a pagesize of 16384, which we have on Darwin20, but should
+   continue to work OK for pagesize 4096 which we have on earlier versions.
+   The size is 1 (binary) Gb.  */
+static char pch_address_space[65536*16384] __attribute__((aligned (16384)));
 
 /* Return the address of the PCH address space, if the PCH will fit in it.  */
 
@@ -58,7 +61,8 @@ darwin_gt_pch_use_address (void *addr, s
   sz = (sz + pagesize - 1) / pagesize * pagesize;
 
   if (munmap (pch_address_space + sz, sizeof (pch_address_space) - sz) != 0)
-    fatal_error (input_location, "couldn%'t unmap pch_address_space: %m");
+    fatal_error (input_location,
+		 "could not unmap %<pch_address_space%>: %m");
 
   if (ret)
     {
Only in ../gcc/gcc/config/m68k: amigaos.c
Only in ../gcc/gcc/config/m68k: amigaos.h
Only in ../gcc/gcc/config/m68k: amigaos.opt
Only in ../gcc/gcc/config/m68k: amigaos-protos.h

--- gcc/config/m68k/constraints.md	2016-01-04 14:30:50.000000000 +0000
+++ gcc/config/m68k/constraints.md	2025-11-02 00:56:37.882277352 +0000
@@ -1,5 +1,5 @@
 ;; Constraint definitions for m68k
-;; Copyright (C) 2007-2016 Free Software Foundation, Inc.
+;; Copyright (C) 2007-2015 Free Software Foundation, Inc.
 
 ;; This file is part of GCC.
 
@@ -91,6 +91,12 @@
 			|| GET_CODE (XEXP (op, 0)) == LABEL_REF
 			|| GET_CODE (XEXP (op, 0)) == CONST)")))
 
+(define_constraint "Z"
+  "Used for Amiga library calls."
+  (and (match_code "plus")
+       (match_test "ADDRESS_REG_P (XEXP (op, 0)) 
+                 && CONST_INT_P (XEXP (op, 1))")))
+
 (define_constraint "T"
   "Used for operands that satisfy 's' when -mpcrel is not in effect."
   (and (match_code "symbol_ref,label_ref,const")
Only in ../gcc/gcc/config/m68k: host-amigaos.c
Only in ../gcc/gcc/config/m68k: m68080.md
Only in ../gcc/gcc/config/m68k: m68k2.c
Only in ../gcc/gcc/config/m68k: m68k_68000_10_costs.c
Only in ../gcc/gcc/config/m68k: m68k_68020_costs.c
Only in ../gcc/gcc/config/m68k: m68k_68030_costs.c
Only in ../gcc/gcc/config/m68k: m68k_68040_costs.c
Only in ../gcc/gcc/config/m68k: m68k_68080_costs.c
Only in ../gcc/gcc/config/m68k: m68kamigaos.h

--- gcc/config/m68k/m68k.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/config/m68k/m68k.c	2025-11-02 00:56:37.886277413 +0000
@@ -32,9 +32,12 @@ along with GCC; see the file COPYING3.
 #include "varasm.h"
 #include "regs.h"
 #include "insn-config.h"
+#include "insn-modes.h"
+#include "machmode.h"
 #include "conditions.h"
 #include "output.h"
 #include "insn-attr.h"
+#include "insn-attr-common.h"
 #include "recog.h"
 #include "diagnostic-core.h"
 #include "flags.h"
@@ -53,6 +56,7 @@ along with GCC; see the file COPYING3.
 #include "lcm.h"
 #include "cfgbuild.h"
 #include "cfgcleanup.h"
+#include "cfgloop.h"
 /* ??? Need to add a dependency between m68k.o and sched-int.h.  */
 #include "sched-int.h"
 #include "insn-codes.h"
@@ -115,29 +119,17 @@ struct m68k_frame
 };
 
 /* Current frame information calculated by m68k_compute_frame_layout().  */
-static struct m68k_frame current_frame;
+struct m68k_frame current_frame;
 
-/* Structure describing an m68k address.
-
-   If CODE is UNKNOWN, the address is BASE + INDEX * SCALE + OFFSET,
-   with null fields evaluating to 0.  Here:
-
-   - BASE satisfies m68k_legitimate_base_reg_p
-   - INDEX satisfies m68k_legitimate_index_reg_p
-   - OFFSET satisfies m68k_legitimate_constant_address_p
-
-   INDEX is either HImode or SImode.  The other fields are SImode.
-
-   If CODE is PRE_DEC, the address is -(BASE).  If CODE is POST_INC,
-   the address is (BASE)+.  */
-struct m68k_address {
-  enum rtx_code code;
-  rtx base;
-  rtx index;
+struct m68k_address_part {
+  rtx * mem_loc;
+  rtx * base_loc;
+  rtx * index_loc;
   rtx offset;
   int scale;
 };
 
+
 static int m68k_sched_adjust_cost (rtx_insn *, rtx, rtx_insn *, int);
 static int m68k_sched_issue_rate (void);
 static int m68k_sched_variable_issue (FILE *, int, rtx_insn *, int);
@@ -148,6 +140,9 @@ static void m68k_sched_dfa_pre_advance_c
 static void m68k_sched_dfa_post_advance_cycle (void);
 static int m68k_sched_first_cycle_multipass_dfa_lookahead (void);
 
+static bool m68k_sched_macro_fusion_p(void);
+static bool m68k_sched_macro_fusion_pair_p(rtx_insn *prev, rtx_insn *curr);
+
 static bool m68k_can_eliminate (const int, const int);
 static void m68k_conditional_register_usage (void);
 static bool m68k_legitimate_address_p (machine_mode, rtx, bool);
@@ -167,6 +162,7 @@ static bool m68k_ok_for_sibcall_p (tree,
 static bool m68k_tls_symbol_p (rtx);
 static rtx m68k_legitimize_address (rtx, rtx, machine_mode);
 static bool m68k_rtx_costs (rtx, machine_mode, int, int, int *, bool);
+static int m68k_address_cost(rtx x, machine_mode mode, addr_space_t t, bool speed);
 #if M68K_HONOR_TARGET_STRICT_ALIGNMENT
 static bool m68k_return_in_memory (const_tree, const_tree);
 #endif
@@ -174,19 +170,32 @@ static void m68k_output_dwarf_dtprel (FI
 static void m68k_trampoline_init (rtx, tree, rtx);
 static int m68k_return_pops_args (tree, tree, int);
 static rtx m68k_delegitimize_address (rtx);
-static void m68k_function_arg_advance (cumulative_args_t, machine_mode,
-				       const_tree, bool);
-static rtx m68k_function_arg (cumulative_args_t, machine_mode,
-			      const_tree, bool);
 static bool m68k_cannot_force_const_mem (machine_mode mode, rtx x);
 static bool m68k_output_addr_const_extra (FILE *, rtx);
 static void m68k_init_sync_libfuncs (void) ATTRIBUTE_UNUSED;
-
+
+static unsigned m68k_loop_unroll_adjust(unsigned n, struct loop * l);
+
+static rtx_insn * m68k_gen_doloop_begin(rtx reg, rtx label);
+static rtx_insn * m68k_gen_doloop_end(rtx reg, rtx label);
+
+static bool
+m68k_use_by_pieces_infrastructure_p (unsigned HOST_WIDE_INT size,
+				     unsigned int align,
+				     enum by_pieces_operation op,
+				     bool speed_p);
+
+static section * m68k_select_section (tree, int, unsigned HOST_WIDE_INT);
+
 /* Initialize the GCC target structure.  */
 
 #if INT_OP_GROUP == INT_OP_DOT_WORD
 #undef TARGET_ASM_ALIGNED_HI_OP
+#ifndef TARGET_AMIGAOS_VASM
 #define TARGET_ASM_ALIGNED_HI_OP "\t.word\t"
+#else
+#define TARGET_ASM_ALIGNED_HI_OP "\tdc.w\t"
+#endif
 #endif
 
 #if INT_OP_GROUP == INT_OP_NO_DOT
@@ -220,6 +229,9 @@ static void m68k_init_sync_libfuncs (voi
 #undef TARGET_ASM_FILE_START_APP_OFF
 #define TARGET_ASM_FILE_START_APP_OFF true
 
+#undef TARGET_USE_BY_PIECES_INFRASTRUCTURE_P
+#define TARGET_USE_BY_PIECES_INFRASTRUCTURE_P m68k_use_by_pieces_infrastructure_p
+
 #undef TARGET_LEGITIMIZE_ADDRESS
 #define TARGET_LEGITIMIZE_ADDRESS m68k_legitimize_address
 
@@ -247,6 +259,12 @@ static void m68k_init_sync_libfuncs (voi
 #undef TARGET_SCHED_DFA_POST_ADVANCE_CYCLE
 #define TARGET_SCHED_DFA_POST_ADVANCE_CYCLE m68k_sched_dfa_post_advance_cycle
 
+#undef TARGET_SCHED_MACRO_FUSION_P
+#define TARGET_SCHED_MACRO_FUSION_P m68k_sched_macro_fusion_p
+
+#undef TARGET_SCHED_MACRO_FUSION_PAIR_P
+#define TARGET_SCHED_MACRO_FUSION_PAIR_P m68k_sched_macro_fusion_pair_p
+
 #undef TARGET_SCHED_FIRST_CYCLE_MULTIPASS_DFA_LOOKAHEAD
 #define TARGET_SCHED_FIRST_CYCLE_MULTIPASS_DFA_LOOKAHEAD	\
   m68k_sched_first_cycle_multipass_dfa_lookahead
@@ -260,6 +278,9 @@ static void m68k_init_sync_libfuncs (voi
 #undef TARGET_RTX_COSTS
 #define TARGET_RTX_COSTS m68k_rtx_costs
 
+#undef TARGET_ADDRESS_COST
+#define TARGET_ADDRESS_COST m68k_address_cost
+
 #undef TARGET_ATTRIBUTE_TABLE
 #define TARGET_ATTRIBUTE_TABLE m68k_attribute_table
 
@@ -306,12 +327,6 @@ static void m68k_init_sync_libfuncs (voi
 #undef TARGET_DELEGITIMIZE_ADDRESS
 #define TARGET_DELEGITIMIZE_ADDRESS m68k_delegitimize_address
 
-#undef TARGET_FUNCTION_ARG
-#define TARGET_FUNCTION_ARG m68k_function_arg
-
-#undef TARGET_FUNCTION_ARG_ADVANCE
-#define TARGET_FUNCTION_ARG_ADVANCE m68k_function_arg_advance
-
 #undef TARGET_LEGITIMATE_CONSTANT_P
 #define TARGET_LEGITIMATE_CONSTANT_P m68k_legitimate_constant_p
 
@@ -322,32 +337,128 @@ static void m68k_init_sync_libfuncs (voi
 #undef TARGET_ATOMIC_TEST_AND_SET_TRUEVAL
 #define TARGET_ATOMIC_TEST_AND_SET_TRUEVAL 128
 
+#undef TARGET_LOOP_UNROLL_ADJUST
+#define TARGET_LOOP_UNROLL_ADJUST m68k_loop_unroll_adjust
+
+#undef TARGET_HAVE_DOLOOP_END
+#define TARGET_HAVE_DOLOOP_END hook_bool_void_true
+
+#undef TARGET_GEN_DOLOOP_END
+#define TARGET_GEN_DOLOOP_END m68k_gen_doloop_end
+
+#undef TARGET_HAVE_DOLOOP_BEGIN
+#define TARGET_HAVE_DOLOOP_BEGIN hook_bool_void_true
+
+#undef TARGET_GEN_DOLOOP_BEGIN
+#define TARGET_GEN_DOLOOP_BEGIN m68k_gen_doloop_begin
+
+#undef  TARGET_ASM_SELECT_SECTION
+#define TARGET_ASM_SELECT_SECTION	m68k_select_section
+
+/*
+   On the m68k, this is a structure:
+   num_of_regs: number of data, address and float registers to use for
+     arguments passing (if it's 2, than pass arguments in d0, d1, a0, a1,
+     fp0 and fp1). 0 - pass everything on stack. vararg calls are
+     always passed entirely on stack.
+   regs_already_used: bitmask of the already used registers.
+   last_arg_reg - register number of the most recently passed argument.
+     -1 if passed on stack.
+   last_arg_len - number of registers used by the most recently passed
+     argument.
+*/
+
+extern void m68k_function_arg_advance (cumulative_args_t, machine_mode, const_tree, bool);
+extern rtx m68k_function_arg (cumulative_args_t, machine_mode, const_tree, bool);
+extern cumulative_args_t m68k_pack_cumulative_args (CUMULATIVE_ARGS *);
+extern tree m68k_handle_type_attribute(tree *, tree, tree, int, bool*);
+
+
+/* Update the data in CUM to advance over an argument
+   of mode MODE and data type TYPE.
+   (TYPE is null for libcalls where that information may not be available.)  */
+
+#undef TARGET_FUNCTION_ARG_ADVANCE
+#define TARGET_FUNCTION_ARG_ADVANCE m68k_function_arg_advance
+
+/* A C expression that controls whether a function argument is passed
+   in a register, and which register. */
+
+#undef TARGET_FUNCTION_ARG
+#define TARGET_FUNCTION_ARG m68k_function_arg
+
+#undef TARGET_PACK_CUMULATIVE_ARGS
+#define TARGET_PACK_CUMULATIVE_ARGS(CUM)  (m68k_pack_cumulative_args(&(CUM)))
+
+
+extern int m68k_comp_type_attributes (const_tree, const_tree);
+#undef  TARGET_COMP_TYPE_ATTRIBUTES
+#define TARGET_COMP_TYPE_ATTRIBUTES m68k_comp_type_attributes
+
+#undef TARGET_STATIC_CHAIN
+#define TARGET_STATIC_CHAIN m68k_static_chain_rtx
+extern rtx
+m68k_static_chain_rtx(const_tree fntype,
+			       bool incoming ATTRIBUTE_UNUSED);
+
+
+#if defined(TARGET_AMIGAOS)
+#include "amigaos.h"
+#endif
+
 static const struct attribute_spec m68k_attribute_table[] =
 {
-  /* { name, min_len, max_len, decl_req, type_req, fn_type_req, handler,
-       affects_type_identity } */
-  { "interrupt", 0, 0, true,  false, false, m68k_handle_fndecl_attribute,
-    false },
-  { "interrupt_handler", 0, 0, true,  false, false,
-    m68k_handle_fndecl_attribute, false },
-  { "interrupt_thread", 0, 0, true,  false, false,
-    m68k_handle_fndecl_attribute, false },
+  /* { name, min_len, max_len, decl_req, type_req, fn_type_req, handler, affects_type_identity } */
+  { "interrupt", 0, 0, true,  false, false, m68k_handle_fndecl_attribute,false },
+  { "interrupt_handler", 0, 0, true,  false, false, m68k_handle_fndecl_attribute, false },
+  { "interrupt_thread", 0, 0, true,  false, false, m68k_handle_fndecl_attribute, false },
+  { "asmreg", 1, 1, false, true, false, m68k_handle_type_attribute, true },
+  { "asmregs", 1, 1, false,  true, true, 0, true },
+  { "regparm", 1, 1, false,  true, true, m68k_handle_type_attribute, true },
+  { "stkparm", 0, 0, false,  true, true, m68k_handle_type_attribute, true },
+
+#ifdef SUBTARGET_ATTRIBUTES
+  SUBTARGET_ATTRIBUTES
+#endif
   { NULL,                0, 0, false, false, false, NULL, false }
 };
 
+#undef TARGET_SCHED_REORDER
+#define TARGET_SCHED_REORDER m68k_target_sched_reorder
+
+static int
+m68k_target_sched_reorder (FILE *, int, rtx_insn **, int *, int);
+
+#undef TARGET_SCHED_REORDER2
+#define TARGET_SCHED_REORDER2 m68k_target_sched_reorder
+
 struct gcc_target targetm = TARGET_INITIALIZER;
-
+
 /* Base flags for 68k ISAs.  */
 #define FL_FOR_isa_00    FL_ISA_68000
 #define FL_FOR_isa_10    (FL_FOR_isa_00 | FL_ISA_68010)
-/* FL_68881 controls the default setting of -m68881.  gcc has traditionally
+/* "FL_68881 controls the default setting of -m68881.  gcc has traditionally
    generated 68881 code for 68020 and 68030 targets unless explicitly told
-   not to.  */
+   not to."
+
+   This is not true at least for the AMIGA.
+   gcc 2.93 does not set the 68881 flag.
+
+   */
+#if defined(TARGET_AMIGAOS)
+#define FL_FOR_isa_20    (FL_FOR_isa_10 | FL_ISA_68020 \
+			  | FL_BITFIELD)
+#else
 #define FL_FOR_isa_20    (FL_FOR_isa_10 | FL_ISA_68020 \
 			  | FL_BITFIELD | FL_68881 | FL_CAS)
+#endif
 #define FL_FOR_isa_40    (FL_FOR_isa_20 | FL_ISA_68040)
 #define FL_FOR_isa_cpu32 (FL_FOR_isa_10 | FL_ISA_68020)
 
+#define FL_FOR_isa_60    (FL_FOR_isa_20 | FL_ISA_68060)
+#define FL_FOR_isa_80    (FL_FOR_isa_20 | FL_ISA_68080)
+
+
 /* Base flags for ColdFire ISAs.  */
 #define FL_FOR_isa_a     (FL_COLDFIRE | FL_ISA_A)
 #define FL_FOR_isa_aplus (FL_FOR_isa_a | FL_ISA_APLUS | FL_CF_USP)
@@ -363,6 +474,8 @@ enum m68k_isa
   isa_10,
   isa_20,
   isa_40,
+  isa_60,
+  isa_80,
   isa_cpu32,
   /* ColdFire instruction set variants.  */
   isa_a,
@@ -422,7 +535,7 @@ static const struct m68k_target_selectio
 #undef M68K_MICROARCH
   { NULL,       unk_device, NULL,  unk_arch,  isa_max, 0 }
 };
-
+
 /* The entries associated with the -mcpu, -march and -mtune settings,
    or null for options that have not been used.  */
 const struct m68k_target_selection *m68k_cpu_entry;
@@ -453,7 +566,7 @@ const char *m68k_symbolic_jump;
 /* Enum variable that corresponds to m68k_symbolic_call values.  */
 enum M68K_SYMBOLIC_CALL m68k_symbolic_call_var;
 
-
+
 /* Implement TARGET_OPTION_OVERRIDE.  */
 
 static void
@@ -543,9 +656,9 @@ m68k_option_override (void)
   /* Set the type of FPU.  */
   m68k_fpu = (!TARGET_HARD_FLOAT ? FPUTYPE_NONE
 	      : (m68k_cpu_flags & FL_COLDFIRE) != 0 ? FPUTYPE_COLDFIRE
-	      : FPUTYPE_68881);
+		  : FPUTYPE_68881);
 
-  /* Sanity check to ensure that msep-data and mid-sahred-library are not
+  /* Sanity check to ensure that msep-data and mid-shared-library are not
    * both specified together.  Doing so simply doesn't make sense.
    */
   if (TARGET_SEP_DATA && TARGET_ID_SHARED_LIBRARY)
@@ -556,24 +669,30 @@ m68k_option_override (void)
    * -fpic but it hasn't been tested properly.
    */
   if (TARGET_SEP_DATA || TARGET_ID_SHARED_LIBRARY)
-    flag_pic = 2;
+    flag_pic = TARGET_68020 ? 2 : 1;
 
   /* -mpcrel -fPIC uses 32-bit pc-relative displacements.  Raise an
      error if the target does not support them.  */
   if (TARGET_PCREL && !TARGET_68020 && flag_pic == 2)
     error ("-mpcrel -fPIC is not currently supported on selected cpu");
 
+#if !defined(TARGET_AMIGAOS)
   /* ??? A historic way of turning on pic, or is this intended to
      be an embedded thing that doesn't have the same name binding
      significance that it does on hosted ELF systems?  */
   if (TARGET_PCREL && flag_pic == 0)
     flag_pic = 1;
+#endif
 
-  if (!flag_pic)
+  /* SBF: use normal jumps/calls with baserel(32) modes. */
+  if (!flag_pic || flag_pic > 2)
     {
       m68k_symbolic_call_var = M68K_SYMBOLIC_CALL_JSR;
-
-      m68k_symbolic_jump = "jra %a0";
+#ifndef TARGET_AMIGAOS_VASM
+      m68k_symbolic_jump = flag_pic ? "jbra %a0" : "jmp %a0";
+#else
+      m68k_symbolic_jump = "jmp %a0";
+#endif
     }
   else if (TARGET_ID_SHARED_LIBRARY)
     /* All addresses must be loaded from the GOT.  */
@@ -601,7 +720,7 @@ m68k_option_override (void)
   switch (m68k_symbolic_call_var)
     {
     case M68K_SYMBOLIC_CALL_JSR:
-      m68k_symbolic_call = "jsr %a0";
+      m68k_symbolic_call = flag_pic ? "jbsr %a0" : "jsr %a0";
       break;
 
     case M68K_SYMBOLIC_CALL_BSR_C:
@@ -639,6 +758,9 @@ m68k_option_override (void)
       stack_limit_rtx = NULL_RTX;
     }
 
+  if (m68k_regparm > 0 && m68k_regparm > M68K_MAX_REGPARM)
+    error ("-mregparm=x with 1 <= x <= %d\n", M68K_MAX_REGPARM);
+
   SUBTARGET_OVERRIDE_OPTIONS;
 
   /* Setup scheduling options.  */
@@ -650,6 +772,8 @@ m68k_option_override (void)
     m68k_sched_cpu = CPU_CFV3;
   else if (TUNE_CFV4)
     m68k_sched_cpu = CPU_CFV4;
+  else if (TUNE_68080)
+    m68k_sched_cpu = CPU_M68080;
   else
     {
       m68k_sched_cpu = CPU_UNKNOWN;
@@ -659,7 +783,7 @@ m68k_option_override (void)
       flag_live_range_shrinkage = 0;
     }
 
-  if (m68k_sched_cpu != CPU_UNKNOWN)
+  if (m68k_sched_cpu != CPU_UNKNOWN && m68k_sched_cpu != CPU_M68080)
     {
       if ((m68k_cpu_flags & (FL_CF_EMAC | FL_CF_EMAC_B)) != 0)
 	m68k_sched_mac = MAC_CF_EMAC;
@@ -707,7 +831,7 @@ m68k_cpp_cpu_family (const char *prefix)
     return NULL;
   return concat ("__m", prefix, "_family_", m68k_cpu_entry->family, NULL);
 }
-
+
 /* Return m68k_fk_interrupt_handler if FUNC has an "interrupt" or
    "interrupt_handler" attribute and interrupt_thread if FUNC has an
    "interrupt_thread" attribute.  Otherwise, return
@@ -719,7 +843,7 @@ m68k_get_function_kind (tree func)
   tree a;
 
   gcc_assert (TREE_CODE (func) == FUNCTION_DECL);
-  
+
   a = lookup_attribute ("interrupt", DECL_ATTRIBUTES (func));
   if (a != NULL_TREE)
     return m68k_fk_interrupt_handler;
@@ -810,7 +934,7 @@ m68k_compute_frame_layout (void)
 	      mask |= 1 << (regno - FP0_REG);
 	      saved++;
 	    }
-      current_frame.foffset = saved * TARGET_FP_REG_SIZE;
+      current_frame.foffset = saved * (flag_no_x_mode ? 8 : TARGET_FP_REG_SIZE);
       current_frame.offset += current_frame.foffset;
     }
   current_frame.fpu_no = saved;
@@ -862,12 +986,31 @@ m68k_initial_elimination_offset (int fro
 static bool
 m68k_save_reg (unsigned int regno, bool interrupt_handler)
 {
+  tree attrs = TYPE_ATTRIBUTES (TREE_TYPE (current_function_decl));
+  if (lookup_attribute ("entrypoint", attrs))
+    return false;
+
+  if (regno != 15 && lookup_attribute ("saveallregs", attrs))
+    return true;
+
   if (flag_pic && regno == PIC_REG)
     {
       if (crtl->saves_all_registers)
 	return true;
+      /* always save if __saveds is used or an options forces setting of a4. */
+      if (flag_pic > 2)
+	{
+	  tree attr = lookup_attribute ("saveds", attrs);
+	  if (attr
+#if defined(TARGET_AMIGAOS)
+	      || TARGET_RESTORE_A4 || TARGET_ALWAYS_RESTORE_A4
+#endif
+	      )
+	    return true;
+	}
+      /* SBF: do not save the PIC_REG with baserel(32) modes.*/
       if (crtl->uses_pic_offset_table)
-	return true;
+	return flag_pic < 3;
       /* Reload may introduce constant pool references into a function
 	 that thitherto didn't need a PIC register.  Note that the test
 	 above will not catch that case because we will only set
@@ -978,6 +1121,8 @@ m68k_set_frame_related (rtx_insn *insn)
 
 /* Emit RTL for the "prologue" define_expand.  */
 
+extern void m68k_emit_regparm_clobbers(void);
+
 void
 m68k_expand_prologue (void)
 {
@@ -986,6 +1131,8 @@ m68k_expand_prologue (void)
 
   m68k_compute_frame_layout ();
 
+  m68k_emit_regparm_clobbers();
+
   if (flag_stack_usage_info)
     current_function_static_stack_size
       = current_frame.size + current_frame.offset;
@@ -1021,6 +1168,11 @@ m68k_expand_prologue (void)
 
   if (frame_pointer_needed)
     {
+#if defined(TARGET_AMIGAOS)
+      if (HAVE_ALTERNATE_FRAME_SETUP_F (fsize_with_regs))
+	ALTERNATE_FRAME_SETUP_F (fsize_with_regs);
+      else
+#endif
       if (fsize_with_regs == 0 && TUNE_68040)
 	{
 	  /* On the 68040, two separate moves are faster than link.w 0.  */
@@ -1030,6 +1182,10 @@ m68k_expand_prologue (void)
 	  m68k_set_frame_related (emit_move_insn (frame_pointer_rtx,
 						  stack_pointer_rtx));
 	}
+#if defined(TARGET_AMIGAOS)
+  else if (HAVE_ALTERNATE_FRAME_SETUP (fsize_with_regs))
+    ALTERNATE_FRAME_SETUP (fsize_with_regs);
+#endif
       else if (fsize_with_regs < 0x8000 || TARGET_68020)
 	m68k_set_frame_related
 	  (emit_insn (gen_link (frame_pointer_rtx,
@@ -1059,11 +1215,28 @@ m68k_expand_prologue (void)
     {
       gcc_assert (current_frame.fpu_no >= MIN_FMOVEM_REGS);
       if (TARGET_68881)
-	m68k_set_frame_related
-	  (m68k_emit_movem (stack_pointer_rtx,
+	{
+	  if (!flag_no_x_mode )
+	    m68k_set_frame_related
+	      (m68k_emit_movem (stack_pointer_rtx,
 			    current_frame.fpu_no * -GET_MODE_SIZE (XFmode),
 			    current_frame.fpu_no, FP0_REG,
 			    current_frame.fpu_mask, true, true));
+	  else
+	    {
+	      /* Store each register separately in the same order moveml does.  */
+	      int i;
+
+	      for (i = 16; i-- > 0; )
+		if (current_frame.fpu_mask & (1 << i))
+		  {
+		    src = gen_rtx_REG (DFmode, FP0_REG + i);
+		    dest = gen_frame_mem (DFmode,
+					  gen_rtx_PRE_DEC (Pmode, stack_pointer_rtx));
+		    m68k_set_frame_related (emit_insn (gen_movsi (dest, src)));
+		  }
+	    }
+	}
       else
 	{
 	  int offset;
@@ -1127,11 +1300,16 @@ m68k_expand_prologue (void)
 			    current_frame.reg_mask, true, true));
     }
 
+  /* SBF: do not load the PIC_REG with baserel(32) */
   if (!TARGET_SEP_DATA
-      && crtl->uses_pic_offset_table)
+      && crtl->uses_pic_offset_table && flag_pic < 3)
     emit_insn (gen_load_got (pic_offset_table_rtx));
+
+#if defined(TARGET_AMIGAOS)
+  amigaos_restore_a4 ();
+#endif
 }
-
+
 /* Return true if a simple (return) instruction is sufficient for this
    instruction (i.e. if no epilogue is needed).  */
 
@@ -1288,15 +1466,52 @@ m68k_expand_epilogue (bool sibcall_p)
 			       current_frame.fpu_mask, false, false);
 	    }
 	  else
-	    m68k_emit_movem (stack_pointer_rtx, 0,
+	    {
+	      if (!flag_no_x_mode)
+		m68k_emit_movem (stack_pointer_rtx, 0,
 			     current_frame.fpu_no, FP0_REG,
 			     current_frame.fpu_mask, false, true);
+	      else
+		{
+		  /* Restore each register separately in the same order moveml does.  */
+		  int i;
+		  rtx src, dest;
+
+		  for (i = 0; i < 16; ++i)
+		    if (current_frame.fpu_mask & (1 << i))
+		      {
+			dest = gen_rtx_REG (DFmode, FP0_REG + i);
+			src = gen_frame_mem (DFmode,
+					      gen_rtx_POST_INC (Pmode, stack_pointer_rtx));
+			m68k_set_frame_related (emit_insn (gen_movsi (dest, src)));
+		      }
+		}
+	    }
 	}
       else
-	m68k_emit_movem (frame_pointer_rtx,
+	{
+          if (!flag_no_x_mode)
+	    m68k_emit_movem (frame_pointer_rtx,
 			 -(current_frame.foffset + fsize),
 			 current_frame.fpu_no, FP0_REG,
 			 current_frame.fpu_mask, false, false);
+	  else
+	    {
+	      /* Restore each register separately in the same order moveml does.  */
+	      int i, j = -(current_frame.foffset + fsize);
+	      rtx src, dest;
+
+	      for (i = 0; i < 16; ++i)
+		if (current_frame.fpu_mask & (1 << i))
+		  {
+		    dest = gen_rtx_REG (DFmode, FP0_REG + i);
+		    src = gen_frame_mem (DFmode,
+					  gen_rtx_PLUS(Pmode, frame_pointer_rtx, GEN_INT (j)));
+		    m68k_set_frame_related (emit_insn (gen_movsi (dest, src)));
+		    j += 8;
+		  }
+	    }
+	}
     }
 
   if (frame_pointer_needed)
@@ -1314,15 +1529,15 @@ m68k_expand_epilogue (bool sibcall_p)
   if (!sibcall_p)
     emit_jump_insn (ret_rtx);
 }
-
-/* Return true if X is a valid comparison operator for the dbcc 
-   instruction.  
+
+/* Return true if X is a valid comparison operator for the dbcc
+   instruction.
 
    Note it rejects floating point comparison operators.
    (In the future we could use Fdbcc).
 
    It also rejects some comparisons when CC_NO_OVERFLOW is set.  */
-   
+
 int
 valid_dbcc_comparison_p_2 (rtx x, machine_mode mode ATTRIBUTE_UNUSED)
 {
@@ -1373,13 +1588,17 @@ m68k_reg_present_p (const_rtx parallel,
   return false;
 }
 
+extern bool m68k_is_ok_for_sibcall(tree decl, tree exp);
 /* Implement TARGET_FUNCTION_OK_FOR_SIBCALL_P.  */
 
 static bool
 m68k_ok_for_sibcall_p (tree decl, tree exp)
 {
   enum m68k_function_kind kind;
-  
+
+  if (!m68k_is_ok_for_sibcall(decl, exp))
+    return false;
+
   /* We cannot use sibcalls for nested functions because we use the
      static chain register for indirect calls.  */
   if (CALL_EXPR_STATIC_CHAIN (exp))
@@ -1415,30 +1634,8 @@ m68k_ok_for_sibcall_p (tree decl, tree e
      the same.  */
   if (decl && m68k_get_function_kind (decl) == kind)
     return true;
-  
-  return false;
-}
-
-/* On the m68k all args are always pushed.  */
-
-static rtx
-m68k_function_arg (cumulative_args_t cum ATTRIBUTE_UNUSED,
-		   machine_mode mode ATTRIBUTE_UNUSED,
-		   const_tree type ATTRIBUTE_UNUSED,
-		   bool named ATTRIBUTE_UNUSED)
-{
-  return NULL_RTX;
-}
-
-static void
-m68k_function_arg_advance (cumulative_args_t cum_v, machine_mode mode,
-			   const_tree type, bool named ATTRIBUTE_UNUSED)
-{
-  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);
 
-  *cum += (mode != BLKmode
-	   ? (GET_MODE_SIZE (mode) + 3) & ~3
-	   : (int_size_in_bytes (type) + 3) & ~3);
+  return false;
 }
 
 /* Convert X to a legitimate function call memory reference and return the
@@ -1545,61 +1742,186 @@ m68k_legitimize_address (rtx x, rtx oldx
   return x;
 }
 
- 
-/* Output a dbCC; jCC sequence.  Note we do not handle the 
+
+/* Output a dbCC; jCC sequence.  Note we do not handle the
    floating point version of this sequence (Fdbcc).  We also
    do not handle alternative conditions when CC_NO_OVERFLOW is
    set.  It is assumed that valid_dbcc_comparison_p and flags_in_68881 will
    kick those out before we get here.  */
 
+extern rtx_insn * current_insn;
 void
 output_dbcc_and_branch (rtx *operands)
 {
-  switch (GET_CODE (operands[3]))
+  char label_follows = false;
+  char label_is_inner_loop = false;
+  rtx_insn * insn;
+
+  /* SBFF:
+   * This might affect some nested loops.
+   * In this case it's bad to pull up the dbf.
+   * => add a check if operand[2]== 2nd jmp label is found before any other label
+   */
+  for (insn = PREV_INSN(current_insn); insn; insn = PREV_INSN (insn))
     {
-      case EQ:
-	output_asm_insn ("dbeq %0,%l1\n\tjeq %l2", operands);
-	break;
+      if (LABEL_P(insn))
+	{
+      label_is_inner_loop = operands[2]->u2.insn_uid == insn->u2.insn_uid;
+	  break;
+	}
+    }
 
-      case NE:
-	output_asm_insn ("dbne %0,%l1\n\tjne %l2", operands);
+  // add a check if the 2nd jmp label follows this dbcc
+  if (!label_is_inner_loop)
+  for (insn = NEXT_INSN(current_insn); insn; insn = NEXT_INSN (insn))
+    {
+      if (LABEL_P(insn))
+	{
+	  label_follows = operands[2]->u2.insn_uid == insn->u2.insn_uid;
+	  break;
+	}
+      if (INSN_P(insn))
 	break;
+    }
 
-      case GT:
-	output_asm_insn ("dbgt %0,%l1\n\tjgt %l2", operands);
-	break;
+  if (label_is_inner_loop)
+	    switch (GET_CODE (operands[3]))
+	      {
+		case EQ:
+		  output_asm_insn ("jeq %l2\n\tdbf %0,%l1", operands);
+		  break;
+
+		case NE:
+			  output_asm_insn ("jne %l2\n\tdbf %0,%l1", operands);
+		  break;
+
+		case GT:
+			  output_asm_insn ("jgt %l2\n\tdbf %0,%l1", operands);
+		  break;
+
+		case GTU:
+			  output_asm_insn ("jhi %l2\n\tdbf %0,%l1", operands);
+		  break;
+
+		case LT:
+			  output_asm_insn ("jlt %l2\n\tdbf %0,%l1", operands);
+		  break;
+
+		case LTU:
+			  output_asm_insn ("jcs %l2\n\tdbf %0,%l1", operands);
+		  break;
+
+		case GE:
+			  output_asm_insn ("jge %l2\n\tdbf %0,%l1", operands);
+		  break;
+
+		case GEU:
+			  output_asm_insn ("jcc %l2\n\tdbf %0,%l1", operands);
+		  break;
+
+		case LE:
+			  output_asm_insn ("jle %l2\n\tdbf %0,%l1", operands);
+		  break;
+
+		case LEU:
+			  output_asm_insn ("jls %l2\n\tdbf %0,%l1", operands);
+		  break;
 
-      case GTU:
-	output_asm_insn ("dbhi %0,%l1\n\tjhi %l2", operands);
-	break;
+		default:
+		  gcc_unreachable ();
+	      }
+  else if (label_follows)
+    switch (GET_CODE (operands[3]))
+      {
+	case EQ:
+	  output_asm_insn ("dbeq %0,%l1\n\tjeq %l2", operands);
+	  break;
 
-      case LT:
-	output_asm_insn ("dblt %0,%l1\n\tjlt %l2", operands);
-	break;
+	case NE:
+	  output_asm_insn ("dbne %0,%l1\n\tjne %l2", operands);
+	  break;
 
-      case LTU:
-	output_asm_insn ("dbcs %0,%l1\n\tjcs %l2", operands);
-	break;
+	case GT:
+	  output_asm_insn ("dbgt %0,%l1\n\tjgt %l2", operands);
+	  break;
 
-      case GE:
-	output_asm_insn ("dbge %0,%l1\n\tjge %l2", operands);
-	break;
+	case GTU:
+	  output_asm_insn ("dbhi %0,%l1\n\tjhi %l2", operands);
+	  break;
 
-      case GEU:
-	output_asm_insn ("dbcc %0,%l1\n\tjcc %l2", operands);
-	break;
+	case LT:
+	  output_asm_insn ("dblt %0,%l1\n\tjlt %l2", operands);
+	  break;
 
-      case LE:
-	output_asm_insn ("dble %0,%l1\n\tjle %l2", operands);
-	break;
+	case LTU:
+	  output_asm_insn ("dbcs %0,%l1\n\tjcs %l2", operands);
+	  break;
 
-      case LEU:
-	output_asm_insn ("dbls %0,%l1\n\tjls %l2", operands);
-	break;
+	case GE:
+	  output_asm_insn ("dbge %0,%l1\n\tjge %l2", operands);
+	  break;
 
-      default:
-	gcc_unreachable ();
-    }
+	case GEU:
+	  output_asm_insn ("dbcc %0,%l1\n\tjcc %l2", operands);
+	  break;
+
+	case LE:
+	  output_asm_insn ("dble %0,%l1\n\tjle %l2", operands);
+	  break;
+
+	case LEU:
+	  output_asm_insn ("dbls %0,%l1\n\tjls %l2", operands);
+	  break;
+
+	default:
+	  gcc_unreachable ();
+      }
+  else
+    switch (GET_CODE (operands[3]))
+      {
+	case EQ:
+	  output_asm_insn ("dbeq %0,%l1\n\tjeq %l2", operands);
+	  break;
+
+	case NE:
+	  output_asm_insn ("dbne %0,%l1\n\tjne %l2", operands);
+	  break;
+
+	case GT:
+	  output_asm_insn ("dbgt %0,%l1\n\tjgt %l2", operands);
+	  break;
+
+	case GTU:
+	  output_asm_insn ("dbhi %0,%l1\n\tjhi %l2", operands);
+	  break;
+
+	case LT:
+	  output_asm_insn ("dblt %0,%l1\n\tjlt %l2", operands);
+	  break;
+
+	case LTU:
+	  output_asm_insn ("dbcs %0,%l1\n\tjcs %l2", operands);
+	  break;
+
+	case GE:
+	  output_asm_insn ("dbge %0,%l1\n\tjge %l2", operands);
+	  break;
+
+	case GEU:
+	  output_asm_insn ("dbcc %0,%l1\n\tjcc %l2", operands);
+	  break;
+
+	case LE:
+	  output_asm_insn ("dble %0,%l1\n\tjle %l2", operands);
+	  break;
+
+	case LEU:
+	  output_asm_insn ("dbls %0,%l1\n\tjls %l2", operands);
+	  break;
+
+	default:
+	  gcc_unreachable ();
+      }
 
   /* If the decrement is to be done in SImode, then we have
      to compensate for the fact that dbcc decrements in HImode.  */
@@ -1796,13 +2118,21 @@ output_btst (rtx *operands, rtx countop,
 	      && next_insn_tests_no_inequality (insn))
 	    {
 	    cc_status.flags = CC_NOT_NEGATIVE | CC_Z_IN_NOT_N | CC_NO_OVERFLOW;
+#ifndef TARGET_AMIGAOS_VASM
 	    return "move%.w %1,%%ccr";
+#else
+	    return "move%.w %1,ccr";
+#endif
 	    }
 	  if (count == 2 && DATA_REG_P (operands[1])
 	      && next_insn_tests_no_inequality (insn))
 	    {
 	    cc_status.flags = CC_NOT_NEGATIVE | CC_INVERTED | CC_NO_OVERFLOW;
+#ifndef TARGET_AMIGAOS_VASM
 	    return "move%.w %1,%%ccr";
+#else
+	    return "move%.w %1,ccr";
+#endif
 	    }
 	  /* count == 1 followed by bvc/bvs and
 	     count == 0 followed by bcc/bcs are also possible, but need
@@ -1813,7 +2143,7 @@ output_btst (rtx *operands, rtx countop,
     }
   return "btst %0,%1";
 }
-
+
 /* Return true if X is a legitimate base register.  STRICT_P says
    whether we need strict checking.  */
 
@@ -1836,7 +2166,15 @@ m68k_legitimate_base_reg_p (rtx x, bool
 bool
 m68k_legitimate_index_reg_p (rtx x, bool strict_p)
 {
-  if (!strict_p && GET_CODE (x) == SUBREG)
+  if (GET_CODE(x) == SIGN_EXTEND)
+    {
+      x = XEXP(x, 0);
+      if (GET_MODE(x) != HImode && GET_MODE(x) != SImode)
+	return false;
+    }
+  /* Allow SUBREG everywhere we allow REG.  This results in better code.  */
+  if (//!strict_p &&
+      GET_CODE (x) == SUBREG)
     x = SUBREG_REG (x);
 
   return (REG_P (x)
@@ -1898,7 +2236,7 @@ m68k_illegitimate_symbolic_constant_p (r
 	  && !offset_within_block_p (base, INTVAL (offset)))
 	return true;
     }
-  return m68k_tls_reference_p (x, false);
+  return m68k_tls_reference_p (x, false) || !amigaos_legitimate_src(x);
 }
 
 /* Implement TARGET_CANNOT_FORCE_CONST_MEM.  */
@@ -1918,13 +2256,18 @@ m68k_legitimate_constant_address_p (rtx
 {
   rtx base, offset;
 
+  if (GET_CODE(x) == PLUS && SYMBOL_REF_P(XEXP(x, 0)) && GET_CODE(XEXP(x, 1)) == CONST_INT)
+    return true;
+
   if (!CONSTANT_ADDRESS_P (x))
     return false;
 
-  if (flag_pic
+  if (flag_pic && flag_pic < 3
       && !(strict_p && TARGET_PCREL)
       && symbolic_operand (x, VOIDmode))
-    return false;
+    {
+      return false;
+    }
 
   if (M68K_OFFSETS_MUST_BE_WITHIN_SECTIONS_P && reach > 1)
     {
@@ -1954,6 +2297,431 @@ m68k_jump_table_ref_p (rtx x)
   return insn && JUMP_TABLE_DATA_P (insn);
 }
 
+/**
+ * Return true, if x is a valid offset for a decomposed address.
+ */
+static bool is_valid_offset(rtx x)
+{
+  if (GET_CODE(x) == CONST)
+    x = XEXP(x, 0);
+  if (GET_CODE(x) == CONST_INT || GET_CODE(x) == UNSPEC)
+    return true;
+  if ((GET_CODE(x) == SYMBOL_REF || GET_CODE(x) == LABEL_REF) && TARGET_68020)
+    return true;
+  if (GET_CODE(x) != PLUS)
+    return false;
+  return is_valid_offset(XEXP(x, 0)) && is_valid_offset(XEXP(x, 1));
+}
+
+static bool
+decompose_one(rtx * loc, struct m68k_address_part *address);
+
+/**
+ * decode the nested rtx structure into a flat structure.
+ *
+ * this function does not validate anything.
+ * it relies on the fact that gcc provides max one
+ * - index
+ * - base
+ * - offset
+ * per level
+ *
+ */
+static bool
+decompose_one(rtx * loc, struct m68k_address_part *address)
+{
+  rtx x = *loc;
+
+  while (GET_CODE(x) == CONST)
+    {
+      loc = &XEXP(x, 0);
+      x = *loc;
+    }
+
+  if (REG_P(x))
+    {
+      // try to use the base slot
+      if (!address->base_loc)
+	{
+	  address->base_loc = loc;
+	  return true;
+	}
+
+      // try to use the index slot
+      if (!address->index_loc)
+	{
+	  address->index_loc = loc;
+	  address->scale = 1;
+	  return true;
+	}
+      return false;
+    }
+
+  /** index register with scale */
+  if (GET_CODE(x) == MULT || GET_CODE(x) == ASHIFT || GET_CODE(x) == SIGN_EXTEND || GET_CODE(x) == SUBREG
+//      || (GET_CODE(x) == PLUS && REG_P(XEXP(x,0)) && XEXP(x,0) == XEXP(x,1)))// *2 as x+x
+    )
+    {
+      int scale = 1;
+      rtx r = x;
+
+      if (GET_CODE(x) == PLUS)
+	{
+          loc = &XEXP(r, 0);
+	  r = *loc;
+	  scale = 2;
+	}
+      else if (GET_CODE(x) == ASHIFT)
+	{
+	  loc = &XEXP(x, 0);
+	  r = *loc;
+	  rtx n = XEXP(x, 1);
+	  if (GET_CODE(n) != CONST_INT)
+	    return false;
+
+	  scale = 1 << INTVAL(n);
+	}
+      else if (GET_CODE(x) == MULT)
+      	{
+      	  loc = &XEXP(x, 0);
+      	  r = *loc;
+      	  rtx n = XEXP(x, 1);
+      	  if (GET_CODE(n) != CONST_INT)
+      	    return false;
+
+      	  scale = INTVAL(n);
+      	}
+
+      if (scale != 1 && scale != 2 && scale != 4 && scale != 8)
+	return false;
+
+      if (GET_CODE(r) == SIGN_EXTEND)
+	{
+	  r = XEXP(r, 0);
+	  if (GET_MODE(r) != HImode && GET_MODE(r) != SImode)
+	    return false;
+	}
+
+      // only one index allowed
+      if (address->index_loc)
+	return false;
+
+      address->index_loc = loc;
+      address->scale = scale;
+      return true;
+    }
+
+  if (GET_CODE(x) == PLUS)
+    {
+      // the single & is mandatory since address must be fully filled
+      return decompose_one(&XEXP(x,0), address)
+	  &  decompose_one(&XEXP(x,1), address);
+    }
+
+  // add const_int / symbol_refs...
+  if (is_valid_offset(x))
+    {
+      rtx da = address->offset;
+
+      if (da)
+	{
+	  if (CONST_INT_P (x) && CONST_INT_P (da))
+	    x = GEN_INT (INTVAL(x) + INTVAL (da));
+	  else
+	    x = gen_rtx_PLUS(SImode, da, x);
+	}
+
+      address->offset = x;
+      return true;
+    }
+
+  // contains a double indirect address.
+  if (MEM_P(x))
+    {
+      if (!flag_double_indirect)
+	return false;
+      // plus (mem) (mem)  does not work either
+      if (address->mem_loc)
+	return false;
+
+      address->mem_loc = loc;
+      ++address;
+      if (address->mem_loc == (rtx *)-1)
+	return false;
+      return decompose_one(&XEXP(x,0), address);
+    }
+  return false;
+}
+
+int decompose_mem(int reach, rtx *_x, struct m68k_address * address, int strict_p)
+{
+  struct m68k_address_part ap_data[5];
+  memset(ap_data, 0, sizeof(ap_data));
+  ap_data[4].mem_loc = (rtx *)-1;
+
+  rtx x = *_x;
+
+  // not an address
+  if (GET_CODE(x) == SIGN_EXTEND)
+    return false;
+
+  struct m68k_address_part * ap = &ap_data[0];
+  bool r = true;
+
+  if (PRE_DEC == GET_CODE(x) || POST_INC == GET_CODE(x))
+    {
+      address->code = GET_CODE(x);
+      address->base_loc = &XEXP(x, 0);
+      address->base = XEXP(x, 0);
+      return m68k_legitimate_base_reg_p(x, strict_p);
+    }
+
+  r &= decompose_one(_x, ap);
+  if (!r)
+      return false;
+
+  // now convert ap[0] / ap[1] into the address
+  if (ap->mem_loc)
+    {
+      m68k_address_part * ap2 = &ap[1];
+      address->code = MEM;
+      address->mem_loc = ap->mem_loc;
+
+      // outer base is never set
+      if (ap->base_loc && !ap->index_loc)
+	{
+	  ap->index_loc = ap->base_loc;
+	  ap->scale = 1;
+	  ap->base_loc = NULL;
+	}
+
+      if ( ap2->mem_loc
+	  || (ap->index_loc && ap2->index_loc)
+	  || ap->base_loc)
+	{
+	  address->code = POST_MODIFY; // this is a marker for reload: must not appear there
+	  r = false;
+	}
+
+      if (ap->index_loc)
+	{
+	  address->outer_index_loc = ap->index_loc;
+	  address->outer_index = *ap->index_loc;
+	  address->outer_scale = ap->scale;
+	  r &= m68k_legitimate_index_reg_p(address->outer_index, strict_p);
+	}
+      if (ap2->base_loc)
+	{
+	  address->base_loc = ap2->base_loc;
+	  address->base = *ap2->base_loc;
+	  if (!m68k_legitimate_base_reg_p(*ap2->base_loc, strict_p))
+	    {
+	      if (!ap2->index_loc && !ap->index_loc)
+		{
+		  // use index instead
+		  ap2->index_loc = ap2->base_loc;
+		  ap2->base_loc = NULL;
+		  ap2->scale = 1;
+
+		  address->base_loc = NULL;
+		  address->base = NULL;
+		}
+	      else
+	      if (ap2->index_loc && ap2->scale == 1 && m68k_legitimate_base_reg_p(*ap2->index_loc, strict_p))
+		{
+		  // swap
+		  address->base_loc = ap2->index_loc;
+		  address->base = *ap2->index_loc;
+
+		  ap2->index_loc = ap2->base_loc;
+		  ap2->base_loc = address->base_loc;
+		}
+	      else
+		r = false;
+	    }
+	}
+      if (ap2->index_loc)
+	{
+	  address->index_loc = ap2->index_loc;
+	  address->index = *ap2->index_loc;
+	  address->scale = ap2->scale;
+	  r &= m68k_legitimate_index_reg_p(address->index, strict_p);
+	}
+
+      address->outer_offset = ap->offset;
+      address->offset = ap2->offset;
+    }
+  else
+    {
+      if (ap->index_loc && ap->scale <= 1 && !ap->base_loc)
+	{
+	  ap->base_loc = ap->index_loc;
+	  ap->index_loc = 0;
+	}
+      // only ap is used -> simply transfer the set values
+      if (ap->base_loc)
+	{
+	  address->base_loc = ap->base_loc;
+	  address->base = *ap->base_loc;
+
+	  if (!m68k_legitimate_base_reg_p(address->base, strict_p))
+	    {
+	      if (ap->index_loc && ap->scale == 1 && m68k_legitimate_base_reg_p(*ap->index_loc, strict_p))
+		    {
+		      // swap
+		      address->base_loc = ap->index_loc;
+		      address->base = *ap->index_loc;
+
+		      ap->index_loc = ap->base_loc;
+		    }
+		  else
+		    r = false;
+		}
+	}
+      if (ap->index_loc)
+	{
+	  address->index_loc = ap->index_loc;
+	  address->index = *ap->index_loc;
+	  address->scale = ap->scale;
+	  r &= m68k_legitimate_index_reg_p(address->index, strict_p);
+	}
+      address->offset = ap->offset;
+    }
+
+    // force use of a single address register.
+    if (address->index && address->scale == 1 && !address->base)
+      {
+	address->base = address->index;
+	address->base_loc = address->index_loc;
+
+	address->index = 0;
+	address->index_loc = 0;
+      }
+
+//  static int nnn;
+//  fprintf(stderr, "%08d %d ", ++nnn, r);
+//  debug_rtx(x);
+
+  // disallow indirect mem addresses for too large stuff - handling overlaps is too tough.
+  if (reach > 4 && address->code == MEM)
+    return false;
+
+//  // double indirect is slower...
+//  if (address->code == MEM && optimize_function_for_speed_p(cfun))
+//	return false;
+
+  if (!TARGET_68020)
+    {
+      if (!address->base && address->index)
+	{
+	  // necessary to support reload.
+	  address->base = address->index;
+	  address->base_loc = address->index_loc;
+	  address->index = 0;
+	  address->index_loc = 0;
+	  return false;
+	}
+
+      // 68k has no support for indirect
+      if (address->code) {
+	  address->code = POST_MODIFY;
+	return false;
+      }
+      // 68k has no support for a missing base register
+      if (!address->base)
+	return false;
+
+      // only const_int offsets in range, if base and index are set
+      if (address->index && address->offset && !(
+	    (GET_CODE(address->offset) == CONST_INT && IN_RANGE (INTVAL (address->offset), -0x8000, 0x8000 - reach))
+	 || GET_CODE(address->offset) == UNSPEC
+	 || (GET_CODE(address->offset) == PLUS && GET_CODE(XEXP(address->offset,0)) == UNSPEC)
+	  ))
+	return false;
+
+      // also only scale 1 is supported.
+      if (address->index && address->scale > 1)
+	return false;
+
+      if (address->index)
+	{
+	  // index requires an offset.
+	  if (!address->offset)
+	    address->offset = CONST0_RTX(SImode);
+	  // with a small offset.
+	  if (GET_CODE(address->offset) != CONST_INT || !IN_RANGE (INTVAL (address->offset), -0x80, 0x80 - reach))
+	    return false;
+	}
+
+      if (address->offset && GET_CODE(address->offset) == CONST_INT && !IN_RANGE (INTVAL (address->offset), -0x8000, 0x8000 - reach))
+	return false;
+    }
+
+  return r;
+}
+
+extern bool m68k_is_SI_memory_operand_to_HI_allowed(rtx x)
+{
+  struct m68k_address address;
+  memset (&address, 0, sizeof (address));
+  return decompose_mem(4, &x, &address, true);
+
+}
+
+static rtx address_to_rtx(struct m68k_address * address)
+{
+  rtx x = address->index;
+  if (address->base)
+    {
+      if (x)
+	x = gen_rtx_PLUS(SImode, x, address->base);
+      else
+	x = address->base;
+    }
+  if (address->offset)
+    {
+      if (x)
+	x = gen_rtx_PLUS(SImode, x, address->offset);
+      else
+	x = address->offset;
+    }
+  if (address->code == MEM)
+    {
+      x = gen_rtx_MEM(SImode, x);
+
+      if (address->outer_index)
+	x = gen_rtx_PLUS(SImode, x, address->outer_index);
+
+      if (address->outer_offset)
+	x = gen_rtx_PLUS(SImode, x, address->outer_offset);
+    }
+  return x;
+}
+
+extern rtx m68k_SI_memory_operand_to_HI(rtx x)
+{
+  struct m68k_address address;
+  memset (&address, 0, sizeof (address));
+  if (!decompose_mem(4, &x, &address, true))
+    return 0;
+
+  rtx * p = address.code == MEM ? address.outer_index_loc : address.index_loc;
+
+  if (address.offset)
+    {
+      if (GET_CODE(*p) == CONST_INT)
+	*p = GEN_INT(INTVAL(*p) + 2);
+      else
+	*p = gen_rtx_PLUS(SImode, *p, GEN_INT(2));
+    }
+  else
+    *p = GEN_INT(2);
+
+  return gen_rtx_MEM(HImode, address_to_rtx(&address));
+}
+
+
+
 /* Return true if X is a legitimate address for values of mode MODE.
    STRICT_P says whether strict checking is needed.  If the address
    is valid, describe its components in *ADDRESS.  */
@@ -2021,7 +2789,7 @@ m68k_decompose_address (machine_mode mod
   /* Check for (xxx).w and (xxx).l.  Also, in the TARGET_PCREL case,
      check for (d16,PC) or (bd,PC,Xn) with a suppressed index register.
      All these modes are variations of mode 7.  */
-  if (m68k_legitimate_constant_address_p (x, reach, strict_p))
+  if (m68k_legitimate_constant_address_p (x, reach, strict_p) && !amiga_is_const_pic_ref(x))
     {
       address->offset = x;
       return true;
@@ -2044,63 +2812,9 @@ m68k_decompose_address (machine_mode mod
 
   /* Everything hereafter deals with (d8,An,Xn.SIZE*SCALE) or
      (bd,An,Xn.SIZE*SCALE) addresses.  */
+  /* SBF: or with all other addresses which can be handled by 68020+ ^^ */
 
-  if (TARGET_68020)
-    {
-      /* Check for a nonzero base displacement.  */
-      if (GET_CODE (x) == PLUS
-	  && m68k_legitimate_constant_address_p (XEXP (x, 1), reach, strict_p))
-	{
-	  address->offset = XEXP (x, 1);
-	  x = XEXP (x, 0);
-	}
-
-      /* Check for a suppressed index register.  */
-      if (m68k_legitimate_base_reg_p (x, strict_p))
-	{
-	  address->base = x;
-	  return true;
-	}
-
-      /* Check for a suppressed base register.  Do not allow this case
-	 for non-symbolic offsets as it effectively gives gcc freedom
-	 to treat data registers as base registers, which can generate
-	 worse code.  */
-      if (address->offset
-	  && symbolic_operand (address->offset, VOIDmode)
-	  && m68k_decompose_index (x, strict_p, address))
-	return true;
-    }
-  else
-    {
-      /* Check for a nonzero base displacement.  */
-      if (GET_CODE (x) == PLUS
-	  && GET_CODE (XEXP (x, 1)) == CONST_INT
-	  && IN_RANGE (INTVAL (XEXP (x, 1)), -0x80, 0x80 - reach))
-	{
-	  address->offset = XEXP (x, 1);
-	  x = XEXP (x, 0);
-	}
-    }
-
-  /* We now expect the sum of a base and an index.  */
-  if (GET_CODE (x) == PLUS)
-    {
-      if (m68k_legitimate_base_reg_p (XEXP (x, 0), strict_p)
-	  && m68k_decompose_index (XEXP (x, 1), strict_p, address))
-	{
-	  address->base = XEXP (x, 0);
-	  return true;
-	}
-
-      if (m68k_legitimate_base_reg_p (XEXP (x, 1), strict_p)
-	  && m68k_decompose_index (XEXP (x, 0), strict_p, address))
-	{
-	  address->base = XEXP (x, 1);
-	  return true;
-	}
-    }
-  return false;
+  return decompose_mem(reach, &x, address, strict_p);
 }
 
 /* Return true if X is a legitimate address for values of mode MODE.
@@ -2110,7 +2824,6 @@ bool
 m68k_legitimate_address_p (machine_mode mode, rtx x, bool strict_p)
 {
   struct m68k_address address;
-
   return m68k_decompose_address (mode, x, strict_p, &address);
 }
 
@@ -2131,7 +2844,11 @@ m68k_legitimate_mem_p (rtx x, struct m68
 bool
 m68k_legitimate_constant_p (machine_mode mode, rtx x)
 {
-  return mode != XFmode && !m68k_illegitimate_symbolic_constant_p (x);
+  return mode != XFmode && !m68k_illegitimate_symbolic_constant_p (x)
+#if defined(TARGET_AMIGAOS)
+      &&  amigaos_legitimate_src (x)
+#endif
+      ;
 }
 
 /* Return true if X matches the 'Q' constraint.  It must be a memory
@@ -2395,19 +3112,19 @@ m68k_wrap_symbol_into_got_ref (rtx x, en
 /* Legitimize PIC addresses.  If the address is already
    position-independent, we return ORIG.  Newly generated
    position-independent addresses go to REG.  If we need more
-   than one register, we lose.  
+   than one register, we lose.
 
    An address is legitimized by making an indirect reference
    through the Global Offset Table with the name of the symbol
-   used as an offset.  
+   used as an offset.
 
-   The assembler and linker are responsible for placing the 
+   The assembler and linker are responsible for placing the
    address of the symbol in the GOT.  The function prologue
    is responsible for initializing a5 to the starting address
    of the GOT.
 
    The assembler is also responsible for translating a symbol name
-   into a constant displacement from the start of the GOT.  
+   into a constant displacement from the start of the GOT.
 
    A quick example may make things a little clearer:
 
@@ -2427,9 +3144,9 @@ m68k_wrap_symbol_into_got_ref (rtx x, en
 
 	movel   a5@(_foo:w), a0
 	movel   #12345, a0@
-   
 
-   That (in a nutshell) is how *all* symbol and label references are 
+
+   That (in a nutshell) is how *all* symbol and label references are
    handled.  */
 
 rtx
@@ -2437,12 +3154,13 @@ legitimize_pic_address (rtx orig, machin
 		        rtx reg)
 {
   rtx pic_ref = orig;
+  if (flag_pic >= 3)
+    return orig;
 
   /* First handle a simple SYMBOL_REF or LABEL_REF */
   if (GET_CODE (orig) == SYMBOL_REF || GET_CODE (orig) == LABEL_REF)
     {
       gcc_assert (reg);
-
       pic_ref = m68k_wrap_symbol_into_got_ref (orig, RELOC_GOT, reg);
       pic_ref = m68k_move_to_reg (pic_ref, orig, reg);
     }
@@ -2458,15 +3176,13 @@ legitimize_pic_address (rtx orig, machin
 
       /* legitimize both operands of the PLUS */
       gcc_assert (GET_CODE (XEXP (orig, 0)) == PLUS);
-      
+
       base = legitimize_pic_address (XEXP (XEXP (orig, 0), 0), Pmode, reg);
       orig = legitimize_pic_address (XEXP (XEXP (orig, 0), 1), Pmode,
 				     base == reg ? 0 : reg);
 
       if (GET_CODE (orig) == CONST_INT)
 	pic_ref = plus_constant (Pmode, base, INTVAL (orig));
-      else
-	pic_ref = gen_rtx_PLUS (Pmode, base, orig);
     }
 
   return pic_ref;
@@ -2520,13 +3236,13 @@ m68k_call_tls_get_addr (rtx x, rtx eqv,
      is the simpliest way of generating a call.  The difference between
      __tls_get_addr() and libcall is that the result is returned in D0
      instead of A0.  To workaround this, we use m68k_libcall_value_in_a0_p
-     which temporarily switches returning the result to A0.  */ 
+     which temporarily switches returning the result to A0.  */
 
   m68k_libcall_value_in_a0_p = true;
   a0 = emit_library_call_value (m68k_get_tls_get_addr (), NULL_RTX, LCT_PURE,
 				Pmode, 1, x, Pmode);
   m68k_libcall_value_in_a0_p = false;
-  
+
   insns = get_insns ();
   end_sequence ();
 
@@ -2554,7 +3270,7 @@ m68k_get_m68k_read_tp (void)
 /* Emit instruction sequence that calls __m68k_read_tp.
    A pseudo register with result of __m68k_read_tp call is returned.  */
 
-static rtx 
+static rtx
 m68k_call_m68k_read_tp (void)
 {
   rtx a0;
@@ -2568,7 +3284,7 @@ m68k_call_m68k_read_tp (void)
      is the simpliest way of generating a call.  The difference between
      __m68k_read_tp() and libcall is that the result is returned in D0
      instead of A0.  To workaround this, we use m68k_libcall_value_in_a0_p
-     which temporarily switches returning the result to A0.  */ 
+     which temporarily switches returning the result to A0.  */
 
   /* Emit the call sequence.  */
   m68k_libcall_value_in_a0_p = true;
@@ -2607,7 +3323,7 @@ m68k_legitimize_tls_address (rtx orig)
 	rtx eqv;
 	rtx a0;
 	rtx x;
- 
+
 	/* Attach a unique REG_EQUIV, to allow the RTL optimizers to
 	   share the LDM result with other LD model accesses.  */
 	eqv = gen_rtx_UNSPEC (Pmode, gen_rtvec (1, const0_rtx),
@@ -2714,7 +3430,7 @@ m68k_tls_reference_p (rtx x, bool legiti
     }
 }
 
-
+
 
 #define USE_MOVQ(i)	((unsigned) ((i) + 128) <= 255)
 
@@ -2749,7 +3465,7 @@ m68k_const_method (HOST_WIDE_INT i)
   if (USE_MOVQ ((u >> 16) | (u << 16)))
     return SWAP;
 
-  if (TARGET_ISAB)
+  if (TARGET_ISAB || TARGET_68080)
     {
       /* Try using MVZ/MVS with an immediate value to load constants.  */
       if (i >= 0 && i <= 65535)
@@ -2762,178 +3478,60 @@ m68k_const_method (HOST_WIDE_INT i)
   return MOVL;
 }
 
-/* Return the cost of moving constant I into a data register.  */
-
-static int
-const_int_cost (HOST_WIDE_INT i)
-{
-  switch (m68k_const_method (i))
-    {
-    case MOVQ:
-      /* Constants between -128 and 127 are cheap due to moveq.  */
-      return 0;
-    case MVZ:
-    case MVS:
-    case NOTB:
-    case NOTW:
-    case NEGW:
-    case SWAP:
-      /* Constants easily generated by moveq + not.b/not.w/neg.w/swap.  */
-      return 1;
-    case MOVL:
-      return 2;
-    default:
-      gcc_unreachable ();
-    }
-}
+extern bool
+m68k_68000_10_costs (rtx x, machine_mode mode, int outer_code,
+		int opno, int *total, bool speed );
+
+extern bool
+m68k_68020_costs (rtx x, machine_mode mode, int outer_code,
+		int opno, int *total, bool speed );
+
+extern bool
+m68k_68030_costs (rtx x, machine_mode mode, int outer_code,
+		int opno, int *total, bool speed );
+
+extern bool
+m68k_68040_costs (rtx x, machine_mode mode, int outer_code,
+		int opno, int *total, bool speed );
+
+extern bool
+m68k_68080_costs (rtx x, machine_mode mode, int outer_code,
+		int opno, int *total, bool speed );
 
 static bool
 m68k_rtx_costs (rtx x, machine_mode mode, int outer_code,
-		int opno ATTRIBUTE_UNUSED,
-		int *total, bool speed ATTRIBUTE_UNUSED)
+		int opno,
+		int *total, bool speed )
 {
-  int code = GET_CODE (x);
-
-  switch (code)
-    {
-    case CONST_INT:
-      /* Constant zero is super cheap due to clr instruction.  */
-      if (x == const0_rtx)
-	*total = 0;
-      else
-        *total = const_int_cost (INTVAL (x));
-      return true;
-
-    case CONST:
-    case LABEL_REF:
-    case SYMBOL_REF:
-      *total = 3;
-      return true;
-
-    case CONST_DOUBLE:
-      /* Make 0.0 cheaper than other floating constants to
-         encourage creating tstsf and tstdf insns.  */
-      if (outer_code == COMPARE
-          && (x == CONST0_RTX (SFmode) || x == CONST0_RTX (DFmode)))
-	*total = 4;
-      else
-	*total = 5;
-      return true;
-
-    /* These are vaguely right for a 68020.  */
-    /* The costs for long multiply have been adjusted to work properly
-       in synth_mult on the 68020, relative to an average of the time
-       for add and the time for shift, taking away a little more because
-       sometimes move insns are needed.  */
-    /* div?.w is relatively cheaper on 68000 counted in COSTS_N_INSNS
-       terms.  */
-#define MULL_COST				\
-  (TUNE_68060 ? 2				\
-   : TUNE_68040 ? 5				\
-   : (TUNE_CFV2 && TUNE_EMAC) ? 3		\
-   : (TUNE_CFV2 && TUNE_MAC) ? 4		\
-   : TUNE_CFV2 ? 8				\
-   : TARGET_COLDFIRE ? 3 : 13)
-
-#define MULW_COST				\
-  (TUNE_68060 ? 2				\
-   : TUNE_68040 ? 3				\
-   : TUNE_68000_10 ? 5				\
-   : (TUNE_CFV2 && TUNE_EMAC) ? 3		\
-   : (TUNE_CFV2 && TUNE_MAC) ? 2		\
-   : TUNE_CFV2 ? 8				\
-   : TARGET_COLDFIRE ? 2 : 8)
-
-#define DIVW_COST				\
-  (TARGET_CF_HWDIV ? 11				\
-   : TUNE_68000_10 || TARGET_COLDFIRE ? 12 : 27)
-
-    case PLUS:
-      /* An lea costs about three times as much as a simple add.  */
-      if (mode == SImode
-	  && GET_CODE (XEXP (x, 1)) == REG
-	  && GET_CODE (XEXP (x, 0)) == MULT
-	  && GET_CODE (XEXP (XEXP (x, 0), 0)) == REG
-	  && GET_CODE (XEXP (XEXP (x, 0), 1)) == CONST_INT
-	  && (INTVAL (XEXP (XEXP (x, 0), 1)) == 2
-	      || INTVAL (XEXP (XEXP (x, 0), 1)) == 4
-	      || INTVAL (XEXP (XEXP (x, 0), 1)) == 8))
-	{
-	    /* lea an@(dx:l:i),am */
-	    *total = COSTS_N_INSNS (TARGET_COLDFIRE ? 2 : 3);
-	    return true;
-	}
-      return false;
-
-    case ASHIFT:
-    case ASHIFTRT:
-    case LSHIFTRT:
-      if (TUNE_68060)
-	{
-          *total = COSTS_N_INSNS(1);
-	  return true;
-	}
-      if (TUNE_68000_10)
-        {
-	  if (GET_CODE (XEXP (x, 1)) == CONST_INT)
-	    {
-	      if (INTVAL (XEXP (x, 1)) < 16)
-	        *total = COSTS_N_INSNS (2) + INTVAL (XEXP (x, 1)) / 2;
-	      else
-	        /* We're using clrw + swap for these cases.  */
-	        *total = COSTS_N_INSNS (4) + (INTVAL (XEXP (x, 1)) - 16) / 2;
-	    }
-	  else
-	    *total = COSTS_N_INSNS (10); /* Worst case.  */
-	  return true;
-        }
-      /* A shift by a big integer takes an extra instruction.  */
-      if (GET_CODE (XEXP (x, 1)) == CONST_INT
-	  && (INTVAL (XEXP (x, 1)) == 16))
-	{
-	  *total = COSTS_N_INSNS (2);	 /* clrw;swap */
-	  return true;
-	}
-      if (GET_CODE (XEXP (x, 1)) == CONST_INT
-	  && !(INTVAL (XEXP (x, 1)) > 0
-	       && INTVAL (XEXP (x, 1)) <= 8))
-	{
-	  *total = COSTS_N_INSNS (TARGET_COLDFIRE ? 1 : 3);	 /* lsr #i,dn */
-	  return true;
-	}
-      return false;
-
-    case MULT:
-      if ((GET_CODE (XEXP (x, 0)) == ZERO_EXTEND
-	   || GET_CODE (XEXP (x, 0)) == SIGN_EXTEND)
-	  && mode == SImode)
-        *total = COSTS_N_INSNS (MULW_COST);
-      else if (mode == QImode || mode == HImode)
-        *total = COSTS_N_INSNS (MULW_COST);
-      else
-        *total = COSTS_N_INSNS (MULL_COST);
-      return true;
+  bool r;
+  if (TUNE_68000_10)
+    r =  m68k_68000_10_costs(x, mode, outer_code, opno, total, speed);
+  else
+  if (m68k_tune == u68020)
+    r = m68k_68020_costs(x, mode, outer_code, opno, total, speed);
+  else
+  if (m68k_tune == u68030)
+    r = m68k_68030_costs(x, mode, outer_code, opno, total, speed);
+  else
+  if (m68k_tune == u68040 || m68k_tune == u68020_40)
+    r = m68k_68040_costs(x, mode, outer_code, opno, total, speed);
+  else
+    r = m68k_68080_costs(x, mode, outer_code, opno, total, speed);
 
-    case DIV:
-    case UDIV:
-    case MOD:
-    case UMOD:
-      if (mode == QImode || mode == HImode)
-        *total = COSTS_N_INSNS (DIVW_COST);	/* div.w */
-      else if (TARGET_CF_HWDIV)
-        *total = COSTS_N_INSNS (18);
-      else
-	*total = COSTS_N_INSNS (43);		/* div.l */
-      return true;
+//  fprintf(stderr, "cost: %d\t", *total);
+//  debug(x);
 
-    case ZERO_EXTRACT:
-      if (outer_code == COMPARE)
-        *total = 0;
-      return false;
+  return r;
+}
 
-    default:
-      return false;
-    }
+int m68k_address_cost(rtx x, machine_mode mode, addr_space_t t ATTRIBUTE_UNUSED, bool speed)
+{
+  static class rtx_def mem;
+  mem.code = MEM;
+  mem.u.fld[0].rt_rtx = x;
+  int total = 0;
+  m68k_rtx_costs(&mem, mode, SET, 0, &total, speed);
+  return total;
 }
 
 /* Return an instruction to move CONST_INT OPERANDS[1] into data register
@@ -2983,7 +3581,7 @@ output_move_const_into_data_reg (rtx *op
 bool
 valid_mov3q_const (HOST_WIDE_INT i)
 {
-  return TARGET_ISAB && (i == -1 || IN_RANGE (i, 1, 7));
+  return (TARGET_68080 || TARGET_ISAB) && (i == -1 || IN_RANGE (i, 1, 7));
 }
 
 /* Return an instruction to move CONST_INT OPERANDS[1] into OPERANDS[0].
@@ -2997,16 +3595,21 @@ output_move_simode_const (rtx *operands)
 
   dest = operands[0];
   src = INTVAL (operands[1]);
-  if (src == 0
-      && (DATA_REG_P (dest) || MEM_P (dest))
+  if (src == 0 && (DATA_REG_P (dest) ))         // For clear DN MOVEQ is best
+    return "moveq #0,%0";
+  else if (src == 0 && MEM_P (dest)             // For memory use CLR
       /* clr insns on 68000 read before writing.  */
       && ((TARGET_68010 || TARGET_COLDFIRE)
-	  || !(MEM_P (dest) && MEM_VOLATILE_P (dest))))
+	  || !MEM_VOLATILE_P (dest)))           // but not for IO register on 68000
     return "clr%.l %0";
   else if (GET_MODE (dest) == SImode && valid_mov3q_const (src))
     return "mov3q%.l %1,%0";
-  else if (src == 0 && ADDRESS_REG_P (dest))
-    return "sub%.l %0,%0";
+  else if (src == 0 && ADDRESS_REG_P (dest))    // For AN always use SUBA
+    return "suba%.l %0,%0";
+  else if (DATA_REG_P (dest) && IN_RANGE (src, -0x80, 0x7f))
+    return "moveq %1,%0";
+  else if (TARGET_68080 && DATA_REG_P (dest) && IN_RANGE (src, -0x8000, 0x7fff))
+    return "moviw%.l %1,%0";
   else if (DATA_REG_P (dest))
     return output_move_const_into_data_reg (operands);
   else if (ADDRESS_REG_P (dest) && IN_RANGE (src, -0x8000, 0x7fff))
@@ -3016,15 +3619,20 @@ output_move_simode_const (rtx *operands)
       return "move%.w %1,%0";
     }
   else if (MEM_P (dest)
-	   && GET_CODE (XEXP (dest, 0)) == PRE_DEC
-	   && REGNO (XEXP (XEXP (dest, 0), 0)) == STACK_POINTER_REGNUM
-	   && IN_RANGE (src, -0x8000, 0x7fff))
+           && GET_CODE (XEXP (dest, 0)) == PRE_DEC
+           && REGNO (XEXP (XEXP (dest, 0), 0)) == STACK_POINTER_REGNUM
+           && IN_RANGE (src, -0x8000, 0x7fff))
     {
       if (valid_mov3q_const (src))
         return "mov3q%.l %1,%-";
       return "pea %a1";
     }
-  return "move%.l %1,%0";
+  else if (TARGET_68080 && IN_RANGE (src, -0x8000, 0x7fff)){
+    return "moviw%.l %1,%0";
+  }else if (TARGET_68080 && DATA_REG_P (dest) && IN_RANGE (src, 0, 0xFfff)){
+    return "movzw%.l %1,%0";
+  }
+    return "move%.l %1,%0";
 }
 
 const char *
@@ -3077,7 +3685,7 @@ output_move_qimode (rtx *operands)
 {
   /* 68k family always modifies the stack pointer by at least 2, even for
      byte pushes.  The 5200 (ColdFire) does not do this.  */
-  
+
   /* This case is generated by pushqi1 pattern now.  */
   gcc_assert (!(GET_CODE (operands[0]) == MEM
 		&& GET_CODE (XEXP (operands[0], 0)) == PRE_DEC
@@ -4014,16 +4622,16 @@ m68k_output_movem (rtx *operands, rtx pa
   if (FP_REGNO_P (REGNO (XEXP (XVECEXP (pattern, 0, first), store_p))))
     {
       if (store_p)
-	return "fmovem %1,%a0";
+	return "fmovem %P1,%a0";
       else
-	return "fmovem %a0,%1";
+	return "fmovem %a0,%O1";
     }
   else
     {
       if (store_p)
-	return "movem%.l %1,%a0";
+	return "movem%.l %M1,%a0";
       else
-	return "movem%.l %a0,%1";
+	return "movem%.l %a0,%N1";
     }
 }
 
@@ -4033,7 +4641,7 @@ m68k_output_movem (rtx *operands, rtx pa
 static rtx
 find_addr_reg (rtx addr)
 {
-  while (GET_CODE (addr) == PLUS)
+  while (GET_CODE (addr) == PLUS || GET_CODE (addr) == MULT)
     {
       if (GET_CODE (XEXP (addr, 0)) == REG)
 	addr = XEXP (addr, 0);
@@ -4103,19 +4711,23 @@ output_addsi3 (rtx *operands)
 	      return "subq%.l #8,%0\n\tsubq%.l %2,%0";
 	    }
 	}
-      if (ADDRESS_REG_P (operands[0])
-	  && INTVAL (operands[2]) >= -0x8000
+      if (INTVAL (operands[2]) >= -0x8000
 	  && INTVAL (operands[2]) < 0x8000)
 	{
-	  if (TUNE_68040)
-	    return "add%.w %2,%0";
-	  else
-	    return MOTOROLA ? "lea (%c2,%0),%0" : "lea %0@(%c2),%0";
+	  if (ADDRESS_REG_P (operands[0]))
+	    {
+	      if (TUNE_68040)
+		return "add%.w %2,%0";
+	      else
+		return MOTOROLA ? "lea (%c2,%0),%0" : "lea %0@(%c2),%0";
+	    }
+	  if (TUNE_68080)
+	    return "addiw%.l %2,%0";
 	}
     }
   return "add%.l %2,%0";
 }
-
+
 /* Store in cc_status the expressions that the condition codes will
    describe after execution of an instruction whose pattern is EXP.
    Do not alter them if the instruction would not alter the cc's.  */
@@ -4131,16 +4743,22 @@ notice_update_cc (rtx exp, rtx insn)
   if (GET_CODE (exp) == SET)
     {
       if (GET_CODE (SET_SRC (exp)) == CALL)
-	CC_STATUS_INIT; 
+	CC_STATUS_INIT;
       else if (ADDRESS_REG_P (SET_DEST (exp)))
 	{
 	  if (cc_status.value1 && modified_in_p (cc_status.value1, insn))
 	    cc_status.value1 = 0;
 	  if (cc_status.value2 && modified_in_p (cc_status.value2, insn))
-	    cc_status.value2 = 0; 
+	    cc_status.value2 = 0;
 	}
       /* fmoves to memory or data registers do not set the condition
-	 codes.  Normal moves _do_ set the condition codes, but not in
+	 codes.  */
+      else if (FP_REG_P (SET_SRC (exp))
+	  && (MEM_P (SET_DEST (exp)) || FP_REG_P(SET_DEST (exp))))
+	{
+	  // nada
+	}
+      /*Normal moves _do_ set the condition codes, but not in
 	 a way that is appropriate for comparison with 0, because -0.0
 	 would be treated as a negative nonzero number.  Note that it
 	 isn't appropriate to conditionalize this restriction on
@@ -4151,7 +4769,7 @@ notice_update_cc (rtx exp, rtx insn)
 	       && (FP_REG_P (SET_SRC (exp))
 		   || GET_CODE (SET_SRC (exp)) == FIX
 		   || FLOAT_MODE_P (GET_MODE (SET_DEST (exp)))))
-	CC_STATUS_INIT; 
+	CC_STATUS_INIT;
       /* A pair of move insns doesn't produce a useful overall cc.  */
       else if (!FP_REG_P (SET_DEST (exp))
 	       && !FP_REG_P (SET_SRC (exp))
@@ -4159,11 +4777,13 @@ notice_update_cc (rtx exp, rtx insn)
 	       && (GET_CODE (SET_SRC (exp)) == REG
 		   || GET_CODE (SET_SRC (exp)) == MEM
 		   || GET_CODE (SET_SRC (exp)) == CONST_DOUBLE))
-	CC_STATUS_INIT; 
+	CC_STATUS_INIT;
       else if (SET_DEST (exp) != pc_rtx)
 	{
 	  cc_status.flags = 0;
 	  cc_status.value1 = SET_DEST (exp);
+	  if (GET_CODE(cc_status.value1) == STRICT_LOW_PART)
+	    cc_status.value1 = XEXP (cc_status.value1, 0);
 	  cc_status.value2 = SET_SRC (exp);
 	}
     }
@@ -4208,7 +4828,7 @@ notice_update_cc (rtx exp, rtx insn)
 	   ends with a move insn moving r2 in r2's mode.
 	   Thus, the cc's are set for r2.
 	   This can set N bit spuriously.  */
-	cc_status.flags |= CC_NOT_NEGATIVE; 
+	cc_status.flags |= CC_NOT_NEGATIVE;
 
       default:
 	break;
@@ -4236,35 +4856,104 @@ notice_update_cc (rtx exp, rtx insn)
 	cc_status.flags |= CC_REVERSED;
     }
 }
-
+
 const char *
-output_move_const_double (rtx *operands)
+print_fp_const(const char * cmd, const char * prec, rtx x)
 {
-  int code = standard_68881_constant_p (operands[1]);
+  static char p[2];
+  static char buf[160];
+  static char buf2[120];
 
-  if (code != 0)
+  const REAL_VALUE_TYPE * r = CONST_DOUBLE_REAL_VALUE (x);
+
+  HOST_WIDE_INT i;
+  if (real_isinteger(r, &i) && i - (short)i == 0)
     {
-      static char buf[40];
+      sprintf (buf, "%sw #%d,%%0", cmd, (short)i);
+      return buf;
+    }
 
-      sprintf (buf, "fmovecr #0x%x,%%0", code & 0xff);
+  if (exact_real_truncate (SFmode, r))
+    p[0] = 's';
+  else
+  if (exact_real_truncate (DFmode, r))
+    p[0] = 'd';
+  else
+    p[0] = prec[0];
+
+  if (p[0] != 's' && real_isinteger(r, &i) && i - (int)i == 0)
+    {
+      sprintf (buf, "%sl #%ld,%%0", cmd, (long)i);
       return buf;
     }
-  return "fmove%.d %1,%0";
+
+  real_to_decimal(buf2, r, 120, 100, 1);
+
+#ifndef TARGET_AMIGAOS_VASM
+  if (0 == strcmp("SNaN", 1+buf2))
+    sprintf (buf, "%ss #0x7fc00000,%%0", cmd);
+  else
+    if (0 == strcmp("QNaN", 1+buf2))
+      sprintf (buf, "%ss #0x7f800000,%%0", cmd);
+    else
+      {
+	int len = strlen(buf2);
+	switch (p[0]) {
+	  case 's':
+	    if (len > 8)
+	      {
+	        long l;
+	        REAL_VALUE_TO_TARGET_SINGLE (*r, l);
+	        sprintf (buf, "%s%s #0x%lx,%%0", cmd, p, l & 0xFFFFFFFF, buf2);
+	        return buf;
+	    }
+	    break;
+	  case 'd':
+	    if (len > 12)
+	      {
+		long l[2];
+		REAL_VALUE_TO_TARGET_DOUBLE (*r, l);
+		sprintf (buf, "%s%s #0x%lx%08lx,%%0", cmd, p, l[0] & 0xFFFFFFFF, l[1] & 0xFFFFFFFF, buf2);
+		return buf;
+	      }
+	    break;
+	  default:
+	    if (len > 16)
+	      {
+	        long l[3];
+	        REAL_VALUE_TO_TARGET_LONG_DOUBLE (*r, l);
+	        sprintf (buf, "%s%s #0x%lx%08lx%08lx,%%0", cmd, p, l[0] & 0xFFFFFFFF, l[1] & 0xFFFFFFFF, l[2] & 0xFFFFFFFF, buf2);
+	        return buf;
+	      }
+	    break;
+	}
+	sprintf (buf, "%s%s #0e%s,%%0", cmd, p, buf2);
+      }
+#else
+  sprintf (buf, "%s%s #%s,%%0", cmd, p, buf2);
+#endif
+  return buf;
 }
 
 const char *
-output_move_const_single (rtx *operands)
+output_move_const_double (rtx *operands)
 {
+  static char buf[40];
   int code = standard_68881_constant_p (operands[1]);
 
   if (code != 0)
     {
-      static char buf[40];
-
       sprintf (buf, "fmovecr #0x%x,%%0", code & 0xff);
       return buf;
     }
-  return "fmove%.s %f1,%0";
+
+  return print_fp_const("fmove.", "d", operands[1]);
+}
+
+const char *
+output_move_const_single (rtx *operands)
+{
+  return output_move_const_double (operands);
 }
 
 /* Return nonzero if X, a CONST_DOUBLE, has a value that we can get
@@ -4273,7 +4962,7 @@ output_move_const_single (rtx *operands)
    to get the desired constant.  */
 
 /* This code has been fixed for cross-compilation.  */
-  
+
 static int inited_68881_table = 0;
 
 static const char *const strings_68881[7] = {
@@ -4326,8 +5015,8 @@ standard_68881_constant_p (rtx x)
   int i;
 
   /* fmovecr must be emulated on the 68040 and 68060, so it shouldn't be
-     used at all on those chips.  */
-  if (TUNE_68040_60)
+     used at all on those chips.  Also 68080 is faster with the real constants. */
+  if (TUNE_68040_60 || TUNE_68080)
     return 0;
 
   if (! inited_68881_table)
@@ -4341,7 +5030,7 @@ standard_68881_constant_p (rtx x)
       if (real_identical (r, &values_68881[i]))
         return (codes_68881[i]);
     }
-  
+
   if (GET_MODE (x) == SFmode)
     return 0;
 
@@ -4375,7 +5064,7 @@ floating_exact_log2 (rtx x)
 
   return 0;
 }
-
+
 /* A C compound statement to output to stdio stream STREAM the
    assembler syntax for an instruction operand X.  X is an RTL
    expression.
@@ -4424,7 +5113,68 @@ floating_exact_log2 (rtx x)
 void
 print_operand (FILE *file, rtx op, int letter)
 {
-  if (letter == '.')
+  machine_mode op_mode = op ? GET_MODE(op) : Pmode;
+  if (flag_no_x_mode && op_mode == XFmode)
+    op_mode = DFmode;
+
+
+  if (letter == 'N')
+    { // movem regs,ax
+      unsigned regbits = INTVAL (op);
+      unsigned regno;
+      for (regno = 0; regbits; ++regno, regbits >>= 1)
+	{
+	  if (regbits & 1)
+	    {
+	      fprintf (file, "%s", reg_names[regno]);
+	      if (regbits > 1)
+		fprintf (file, "/");
+	    }
+	}
+    }
+  else if (letter == 'M')
+    { // movem regs,ax
+      unsigned regbits = INTVAL (op);
+      unsigned regno;
+      for (regno = 15; regbits; --regno, regbits >>= 1)
+	{
+	  if (regbits & 1)
+	    {
+	      fprintf (file, "%s", reg_names[regno]);
+	      if (regbits > 1)
+		fprintf (file, "/");
+	    }
+	}
+    }
+  else if (letter == 'O')
+    { // movem regs,ax
+      unsigned regbits = INTVAL (op);
+      unsigned regno;
+      for (regno = 0; regbits; ++regno, regbits >>= 1)
+	{
+	  if (regbits & 1)
+	    {
+	      fprintf (file, "%s", reg_names[FP0_REG + 7 - regno]);
+	      if (regbits > 1)
+		fprintf (file, "/");
+	    }
+	}
+    }
+  else if (letter == 'P')
+    { // fmovem regs,ax
+      unsigned regbits = INTVAL (op);
+      unsigned regno;
+      for (regno = 7; regbits; --regno, regbits >>= 1)
+	{
+	  if (regbits & 1)
+	    {
+	      fprintf (file, "%s", reg_names[FP0_REG + 7 - regno]);
+	      if (regbits > 1)
+		fprintf (file, "/");
+	    }
+	}
+    }
+  else if (letter == '.')
     {
       if (MOTOROLA)
 	fprintf (file, ".");
@@ -4441,22 +5191,24 @@ print_operand (FILE *file, rtx op, int l
     asm_fprintf (file, "%Rfpcr");
   else if (letter == '$')
     {
-      if (TARGET_68040)
+      if ((TARGET_68040 || TARGET_68060 || TARGET_68080))
 	fprintf (file, "s");
     }
   else if (letter == '&')
     {
-      if (TARGET_68040)
+      if ((TARGET_68040 || TARGET_68060 || TARGET_68080))
 	fprintf (file, "d");
     }
   else if (letter == '/')
     asm_fprintf (file, "%R");
   else if (letter == '?')
-    asm_fprintf (file, m68k_library_id_string);
+    asm_fprintf (file, "%s", m68k_library_id_string);
   else if (letter == 'p')
     {
       output_addr_const (file, op);
-      if (!(GET_CODE (op) == SYMBOL_REF && SYMBOL_REF_LOCAL_P (op)))
+      /* SBF: do not add @PLTPC with baserel(32). */
+      if (flag_pic < 3
+          && !(GET_CODE (op) == SYMBOL_REF && SYMBOL_REF_LOCAL_P (op)))
 	fprintf (file, "@PLTPC");
     }
   else if (GET_CODE (op) == REG)
@@ -4470,39 +5222,57 @@ print_operand (FILE *file, rtx op, int l
     }
   else if (GET_CODE (op) == MEM)
     {
-      output_address (GET_MODE (op), XEXP (op, 0));
+      output_address (op_mode, XEXP (op, 0));
       if (letter == 'd' && ! TARGET_68020
 	  && CONSTANT_ADDRESS_P (XEXP (op, 0))
 	  && !(GET_CODE (XEXP (op, 0)) == CONST_INT
 	       && INTVAL (XEXP (op, 0)) < 0x8000
-	       && INTVAL (XEXP (op, 0)) >= -0x8000))
-	fprintf (file, MOTOROLA ? ".l" : ":l");
+	       && INTVAL (XEXP (op, 0)) >= -0x8000)
+#if defined(TARGET_AMIGAOS)
+/* SBF: Do not append some 'l' with baserel(32). */
+	       && !amiga_is_const_pic_ref(XEXP(op, 0))
+#endif
+	       )
+		fprintf (file, MOTOROLA ? ".l" : ":l");
     }
-  else if (GET_CODE (op) == CONST_DOUBLE && GET_MODE (op) == SFmode)
+  else if (GET_CODE (op) == CONST_DOUBLE && op_mode == SFmode)
     {
       long l;
       REAL_VALUE_TO_TARGET_SINGLE (*CONST_DOUBLE_REAL_VALUE (op), l);
+#ifndef TARGET_AMIGAOS_VASM
       asm_fprintf (file, "%I0x%lx", l & 0xFFFFFFFF);
+#else
+      asm_fprintf (file, "%I$%lx", l & 0xFFFFFFFF);
+#endif
     }
-  else if (GET_CODE (op) == CONST_DOUBLE && GET_MODE (op) == XFmode)
+  else if (GET_CODE (op) == CONST_DOUBLE && op_mode == XFmode)
     {
       long l[3];
       REAL_VALUE_TO_TARGET_LONG_DOUBLE (*CONST_DOUBLE_REAL_VALUE (op), l);
+#ifndef TARGET_AMIGAOS_VASM
       asm_fprintf (file, "%I0x%lx%08lx%08lx", l[0] & 0xFFFFFFFF,
 		   l[1] & 0xFFFFFFFF, l[2] & 0xFFFFFFFF);
+#else
+      asm_fprintf (file, "%I$%lx%08lx%08lx", l[0] & 0xFFFFFFFF,
+		   l[1] & 0xFFFFFFFF, l[2] & 0xFFFFFFFF);
+#endif
     }
-  else if (GET_CODE (op) == CONST_DOUBLE && GET_MODE (op) == DFmode)
+  else if (GET_CODE (op) == CONST_DOUBLE && op_mode == DFmode)
     {
       long l[2];
       REAL_VALUE_TO_TARGET_DOUBLE (*CONST_DOUBLE_REAL_VALUE (op), l);
+#ifndef TARGET_AMIGAOS_VASM
       asm_fprintf (file, "%I0x%lx%08lx", l[0] & 0xFFFFFFFF, l[1] & 0xFFFFFFFF);
+#else
+      asm_fprintf (file, "%I$%lx%08lx", l[0] & 0xFFFFFFFF, l[1] & 0xFFFFFFFF);
+#endif
     }
   else
     {
       /* Use `print_operand_address' instead of `output_addr_const'
 	 to ensure that we print relevant PIC stuff.  */
       asm_fprintf (file, "%I");
-      if (TARGET_PCREL
+      if ((TARGET_PCREL || flag_pic > 2)
 	  && (GET_CODE (op) == SYMBOL_REF || GET_CODE (op) == CONST))
 	print_operand_address (file, op);
       else
@@ -4521,28 +5291,48 @@ m68k_get_reloc_decoration (enum m68k_rel
   switch (reloc)
     {
     case RELOC_GOT:
-      if (MOTOROLA)
+      /* SBF: add the proper extension for baserel relocs with baserel(32). */
+  if (TARGET_AMIGA)
+	{
+#ifndef TARGET_AMIGAOS_VASM
+	  if (flag_pic == 1)
+	    return ".w";
+	  else if (flag_pic == 3)
+	    return ":W";
+	  else if (flag_pic == 4)
+	    return ":L";
+	  else
+	    return "";
+#else
+	  if (flag_pic == 1)
+            return ".w";
+          else if (flag_pic == 3)
+            return ".w";
+          else if (flag_pic == 4)
+            return ".l";
+          else
+            return "";
+
+#endif
+	}
+	if (MOTOROLA)
 	{
 	  if (flag_pic == 1 && TARGET_68020)
 	    return "@GOT.w";
 	  else
 	    return "@GOT";
 	}
-      else
-	{
-	  if (TARGET_68020)
+	if (TARGET_68020)
+	  switch (flag_pic)
 	    {
-	      switch (flag_pic)
-		{
-		case 1:
-		  return ":w";
-		case 2:
-		  return ":l";
-		default:
-		  return "";
-		}
+	    case 1:
+	      return ":w";
+	    case 2:
+	      return ":l";
+	    default:
+	      break;
 	    }
-	}
+	return "";
 
     case RELOC_TLSGD:
       return "@TLSGD";
@@ -4626,7 +5416,7 @@ m68k_delegitimize_address (rtx orig_x)
   unspec = XEXP (addr.offset, 0);
   if (GET_CODE (unspec) == PLUS && CONST_INT_P (XEXP (unspec, 1)))
     unspec = XEXP (unspec, 0);
-  if (GET_CODE (unspec) != UNSPEC 
+  if (GET_CODE (unspec) != UNSPEC
       || (XINT (unspec, 1) != UNSPEC_RELOC16
 	  && XINT (unspec, 1) != UNSPEC_RELOC32))
     return orig_x;
@@ -4647,8 +5437,8 @@ m68k_delegitimize_address (rtx orig_x)
     x = replace_equiv_address_nv (orig_x, x);
   return x;
 }
-  
-
+
+
 /* A C compound statement to output to stdio stream STREAM the
    assembler syntax for an instruction operand that is a memory
    reference whose address is ADDR.  ADDR is an RTL expression.
@@ -4661,18 +5451,65 @@ m68k_delegitimize_address (rtx orig_x)
    It is possible for PIC to generate a (plus (label_ref...) (reg...))
    and we handle that just like we would a (plus (symbol_ref...) (reg...)).
 
-   This routine is responsible for distinguishing between -fpic and -fPIC 
+   This routine is responsible for distinguishing between -fpic and -fPIC
    style relocations in an address.  When generating -fpic code the
    offset is output in word mode (e.g. movel a5@(_foo:w), a0).  When generating
    -fPIC code the offset is output in long mode (e.g. movel a5@(_foo:l), a0) */
-
+static void
+print_operand_address2 (FILE *file, rtx addr, int offset);
 void
 print_operand_address (FILE *file, rtx addr)
 {
-  struct m68k_address address;
+  print_operand_address2(file, addr, 0);
+}
+
+static void
+print_index(FILE * file, rtx x, int scale)
+{
+  if (GET_CODE(x) == SIGN_EXTEND)
+    x = XEXP(x, 0);
+  if (SUBREG_P(x))
+    x = alter_subreg (&x, true);
+  int regno = REGNO(x);
+
+  fprintf (file, "%s.%c",
+	       M68K_REGNAME (regno),
+	       GET_MODE(x) == HImode ? 'w' : 'l');
+  if (scale != 1)
+    fprintf (file, "*%d", scale);
+}
 
+static void
+append_outer_address(FILE * file, struct m68k_address address)
+{
+  putc (']', file);
+
+  /* Print the ",index" component, if any.  */
+  if (address.outer_index)
+    {
+      putc (',', file);
+      print_index(file, address.outer_index, address.outer_scale);
+    }
+
+  if (address.outer_offset)
+    {
+      putc (',', file);
+      output_addr_const (file, address.outer_offset);
+    }
+}
+
+static void
+print_operand_address2 (FILE *file, rtx addr, int offset)
+{
+  struct m68k_address address;
   if (!m68k_decompose_address (QImode, addr, true, &address))
-    gcc_unreachable ();
+    {
+      debug_rtx(addr);
+      m68k_decompose_address (QImode, addr, true, &address);
+      gcc_unreachable ();
+    }
+
+  bool ket = address.code == MEM;
 
   if (address.code == PRE_DEC)
     fprintf (file, MOTOROLA ? "-(%s)" : "%s@-",
@@ -4680,12 +5517,13 @@ print_operand_address (FILE *file, rtx a
   else if (address.code == POST_INC)
     fprintf (file, MOTOROLA ? "(%s)+" : "%s@+",
 	     M68K_REGNAME (REGNO (address.base)));
-  else if (!address.base && !address.index)
+  else if (!address.base && !address.index && !address.code)
     {
       /* A constant address.  */
       gcc_assert (address.offset == addr);
       if (GET_CODE (addr) == CONST_INT)
 	{
+	  if (offset) fprintf(file, "%d+", offset);
 	  /* (xxx).w or (xxx).l.  */
 	  if (IN_RANGE (INTVAL (addr), -0x8000, 0x7fff))
 	    fprintf (file, MOTOROLA ? "%d.w" : "%d:w", (int) INTVAL (addr));
@@ -4695,9 +5533,19 @@ print_operand_address (FILE *file, rtx a
       else if (TARGET_PCREL)
 	{
 	  /* (d16,PC) or (bd,PC,Xn) (with suppressed index register).  */
-	  fputc ('(', file);
+	  putc ('(', file);
+	  if (ket) putc ('[', file);
+	  if (offset) fprintf(file, "%d+", offset);
 	  output_addr_const (file, addr);
-	  asm_fprintf (file, flag_pic == 1 ? ":w,%Rpc)" : ":l,%Rpc)");
+#if defined(TARGET_AMIGAOS)
+	  asm_fprintf (file, ",%Rpc");
+#else
+	  asm_fprintf (file, flag_pic == 1 ? ":w,%Rpc)" : ":l,%Rpc");
+#endif
+	  if (ket)
+	    append_outer_address(file, address);
+
+	  putc (')', file);
 	}
       else
 	{
@@ -4709,11 +5557,26 @@ print_operand_address (FILE *file, rtx a
 	      && XSTR (addr, 0)[strlen (XSTR (addr, 0)) - 2] == '.')
 	    {
 	      putc ('(', file);
+	      if (ket) putc ('[', file);
+	      if (offset) fprintf(file, "%d+", offset);
 	      output_addr_const (file, addr);
+#if defined(TARGET_AMIGAOS)
+	      if (SYMBOL_REF_FUNCTION_P(addr))
+		{
+		  if (flag_smallcode)
+		    asm_fprintf(file, ":w,pc");
+		}
+#endif
+	      if (ket)
+		append_outer_address(file, address);
+
 	      putc (')', file);
 	    }
-	  else
+	  else {
+	    if (offset) fprintf(file, "%d+", offset);
 	    output_addr_const (file, addr);
+	  }
+
 	}
     }
   else
@@ -4729,29 +5592,34 @@ print_operand_address (FILE *file, rtx a
 		 : -1);
       if (MOTOROLA)
 	{
+	  putc ('(', file);
+	  if (ket) putc ('[', file);
+	  if (offset) fprintf(file, "%d+", offset);
+
 	  /* Print the "offset(base" component.  */
 	  if (labelno >= 0)
-	    asm_fprintf (file, "%LL%d(%Rpc,", labelno);
+	    asm_fprintf (file, "%LL%d,%Rpc", labelno);
 	  else
 	    {
 	      if (address.offset)
 		output_addr_const (file, address.offset);
-
-	      putc ('(', file);
 	      if (address.base)
-		fputs (M68K_REGNAME (REGNO (address.base)), file);
+		{
+		  if (address.offset)
+		    putc (',', file);
+		  fputs (M68K_REGNAME (REGNO (address.base)), file);
+		}
 	    }
 	  /* Print the ",index" component, if any.  */
 	  if (address.index)
 	    {
-	      if (address.base)
+	      if (labelno >= 0 || address.offset || address.base)
 		putc (',', file);
-	      fprintf (file, "%s.%c",
-		       M68K_REGNAME (REGNO (address.index)),
-		       GET_MODE (address.index) == HImode ? 'w' : 'l');
-	      if (address.scale != 1)
-		fprintf (file, "*%d", address.scale);
+	      print_index(file, address.index, address.scale);
 	    }
+	  if (ket)
+	    append_outer_address(file, address);
+
 	  putc (')', file);
 	}
       else /* !MOTOROLA */
@@ -4785,7 +5653,7 @@ print_operand_address (FILE *file, rtx a
 	}
     }
 }
-
+
 /* Check for cases where a clr insns can be omitted from code using
    strict_low_part sets.  For example, the second clrl here is not needed:
    clrl d0; movw a0@+,d0; use d0; clrl d0; movw a0@+; use d0; ...
@@ -5018,8 +5886,12 @@ output_call (rtx x)
 {
   if (symbolic_operand (x, VOIDmode))
     return m68k_symbolic_call;
-  else
-    return "jsr %a0";
+
+#if defined (TARGET_AMIGAOS)
+  if (flag_smallcode)
+    return "jbsr %a0";
+#endif
+  return "jsr %a0";
 }
 
 /* Likewise sibling calls.  */
@@ -5029,8 +5901,12 @@ output_sibcall (rtx x)
 {
   if (symbolic_operand (x, VOIDmode))
     return m68k_symbolic_jump;
-  else
-    return "jmp %a0";
+
+#if defined (TARGET_AMIGAOS)
+  if (flag_smallcode)
+    return "jbra %a0";
+#endif
+  return "jmp %a0";
 }
 
 static void
@@ -5155,7 +6031,9 @@ m68k_hard_regno_rename_ok (unsigned int
 
 /* Value is true if hard register REGNO can hold a value of machine-mode
    MODE.  On the 68000, we let the cpu registers can hold any mode, but
-   restrict the 68881 registers to floating-point modes.  */
+   restrict the 68881 registers to floating-point modes.
+   SBF: Disallow the frame pointer register, if the frame pointer is used.
+   */
 
 bool
 m68k_regno_mode_ok (int regno, machine_mode mode)
@@ -5168,8 +6046,13 @@ m68k_regno_mode_ok (int regno, machine_m
     }
   else if (ADDRESS_REGNO_P (regno))
     {
+      if (mode == QImode)
+	return false;
+      if (TARGET_68881 && (GET_MODE_CLASS (mode) == MODE_FLOAT
+	   || GET_MODE_CLASS (mode) == MODE_COMPLEX_FLOAT))
+	return false;
       if (regno + GET_MODE_SIZE (mode) / 4 <= 16)
-	return true;
+	return !frame_pointer_needed || regno != FRAME_POINTER_REGNUM;
     }
   else if (FP_REGNO_P (regno))
     {
@@ -5190,6 +6073,16 @@ m68k_secondary_reload_class (enum reg_cl
 			     machine_mode mode, rtx x)
 {
   int regno;
+#if defined(TARGET_AMIGAOS)
+  /* SBF: check for baserel's const pic_ref
+   * and return ADDR_REGS or NO_REGS
+   */
+  if (!MEM_P(x) && amiga_is_const_pic_ref(x))
+    return rclass == ADDR_REGS ? NO_REGS : ADDR_REGS;
+#endif
+
+  if (rclass == ADDR_REGS)
+	  return NO_REGS;
 
   regno = true_regnum (x);
 
@@ -5969,6 +6862,7 @@ m68k_sched_issue_rate (void)
     case CPU_CFV1:
     case CPU_CFV2:
     case CPU_CFV3:
+    case CPU_M68080:
       return 1;
 
     case CPU_CFV4:
@@ -6043,7 +6937,7 @@ m68k_sched_variable_issue (FILE *sched_d
 
 	case CPU_CFV3:
 	  insn_size = sched_get_attr_size_int (insn);
-	  
+
 	  /* ColdFire V3 and V4 cores have instruction buffers that can
 	     accumulate up to 8 instructions regardless of instructions'
 	     sizes.  So we should take care not to "prefetch" 24 one-word
@@ -6072,6 +6966,7 @@ m68k_sched_variable_issue (FILE *sched_d
 
 	  break;
 
+	case CPU_M68080:
 	case CPU_CFV4:
 	  gcc_assert (!sched_ib.enabled_p);
 	  insn_size = 0;
@@ -6145,10 +7040,11 @@ m68k_sched_md_init_global (FILE *sched_d
   /* ColdFire V4 has a set of features to keep its instruction buffer full
      (e.g., a separate memory bus for instructions) and, hence, we do not model
      buffer for this CPU.  */
-  sched_ib.enabled_p = (m68k_sched_cpu != CPU_CFV4);
+  sched_ib.enabled_p = (m68k_sched_cpu != CPU_CFV4) &&  (m68k_sched_cpu != CPU_M68080);
 
   switch (m68k_sched_cpu)
     {
+    case CPU_M68080:
     case CPU_CFV4:
       sched_ib.filled = 0;
 
@@ -6223,7 +7119,8 @@ m68k_sched_md_init (FILE *sched_dump ATT
       sched_ib.records.adjust_index = 0;
       break;
 
-    case CPU_CFV4:
+      case CPU_M68080:
+      case CPU_CFV4:
       gcc_assert (!sched_ib.enabled_p);
       sched_ib.size = 0;
       break;
@@ -6442,6 +7339,47 @@ m68k_sched_indexed_address_bypass_p (rtx
     }
 }
 
+static bool m68k_sched_macro_fusion_p(void) {
+  return 1;
+}
+
+/*
+ * if current insn is a compare
+ * check prev for same register usage.
+ */
+static bool m68k_sched_macro_fusion_pair_p(rtx_insn *prev, rtx_insn *curr)
+{
+  if (!NONJUMP_INSN_P(prev) || !NONJUMP_INSN_P(curr))
+    return 0;
+
+  rtx cset = single_set(curr);
+  if (!cset || GET_CODE(SET_DEST(cset)) != CC0)
+    return 0;
+
+  rtx csrc = SET_SRC(cset);
+  if (GET_CODE(csrc) != COMPARE)
+    return 0;
+
+  rtx pset = single_set(prev);
+  if (!pset)
+    return 0;
+
+  rtx pdest = SET_DEST(pset);
+  if (!REG_P(pdest) && !reg_overlap_mentioned_p(pdest, csrc))
+    return 0;
+
+  rtx psrc = SET_SRC(pset);
+  if (GET_CODE(psrc) == MINUS && GET_CODE(XEXP(psrc, 1)) == CONST_INT
+      && INTVAL(XEXP(psrc, 1)) == 1)
+    return 1;
+  if (GET_CODE(psrc) == PLUS && GET_CODE(XEXP(psrc, 1)) == CONST_INT
+      && INTVAL(XEXP(psrc, 1)) == -1)
+    return 1;
+
+  return 0;
+}
+
+
 /* We generate a two-instructions program at M_TRAMP :
 	movea.l &CHAIN_VALUE,%a0
 	jmp FNADDR
@@ -6487,6 +7425,9 @@ m68k_return_pops_args (tree fundecl, tre
 	  ? size : 0);
 }
 
+rtx singbas;
+rtx doubbas;
+
 /* Make sure everything's fine if we *don't* have a given processor.
    This assumes that putting a register in fixed_regs will keep the
    compiler's mitts completely off it.  We don't bother to zero it out
@@ -6506,6 +7447,9 @@ m68k_conditional_register_usage (void)
     }
   if (flag_pic)
     fixed_regs[PIC_REG] = call_used_regs[PIC_REG] = 1;
+
+  singbas = gen_rtx_fmt_s0 (SYMBOL_REF, SImode, "MathIeeeSingTransBase");
+  doubbas = gen_rtx_fmt_s0 (SYMBOL_REF, SImode, "MathIeeeDoubTransBase");
 }
 
 static void
@@ -6524,4 +7468,401 @@ m68k_epilogue_uses (int regno ATTRIBUTE_
 	      == m68k_fk_interrupt_handler));
 }
 
+static unsigned m68k_loop_unroll_adjust(unsigned n ATTRIBUTE_UNUSED, struct loop * l ATTRIBUTE_UNUSED) {
+#if 0
+  /* count float mul/div operations, since these have a delay */
+  rtx_insn * insn;
+  unsigned fop_count = 0;
+  for (insn = l->header->il.x.head_; insn; insn = NEXT_INSN(insn))
+    {
+      if (!NONJUMP_INSN_P(insn))
+	continue;
+
+      rtx set = single_set(insn);
+      if (!set)
+	continue;
+
+      rtx src = SET_SRC(set);
+      if (GET_MODE(src) != DFmode && GET_MODE(src) != SFmode)
+	continue;
+
+      if (GET_CODE(src) == DIV || GET_CODE(src) == MULT)
+	++fop_count;
+    }
+
+  if (fop_count)
+    return 4 / fop_count;
+#endif
+  return n;
+}
+
+static int
+is_ea_insn(rtx set)
+{
+  if (!ADDRESS_REG_P(SET_DEST(set)))
+    return false;
+
+//  MOVE.L #im,An
+//  MOVE.L Reg,An
+  rtx src = SET_SRC(set);
+  if (REG_P(src) || CONST_INT_P(src))
+    return true;
+
+//  SUBQ #,An
+//  SUBA #im,An
+//  SUBA Reg,An
+  if (GET_CODE(src) == MINUS)
+    return rtx_equal_p(XEXP(src, 0), SET_DEST(set))
+	&& (REG_P(XEXP(src, 1)) || CONST_INT_P(XEXP(src, 1)));
+
+  if (GET_CODE(src) != PLUS)
+    return false;
+
+//  ADDQ #,An
+//  ADDA #im,An
+//  ADDA Reg,An
+  if (rtx_equal_p(XEXP(src, 0), SET_DEST(set))
+	&& (REG_P(XEXP(src, 1)) || CONST_INT_P(XEXP(src, 1))))
+    return true;
+
+  //  LEA (ea),An
+  return m68k_legitimate_address_p (GET_MODE(src), src, reload_completed);
+}
+
+/**
+ * if top insn is an EA insn
+ * make a non fp mode insn 2nd and return 2.
+ * or if a non fp mode insn is top make an EA insn 2nd and return 2.
+ * otherwise return 1.
+ */
+static int
+m68k_target_sched_reorder (FILE *file ATTRIBUTE_UNUSED, int verbose ATTRIBUTE_UNUSED, rtx_insn **ready, int *n_readyp, int clock ATTRIBUTE_UNUSED)
+{
+  int n = *n_readyp;
+  if (n > 1)
+    {
+      rtx_insn * top = ready[--n];
+      rtx set0 = single_set(top);
+      if (set0)
+	{
+	  machine_mode mode0 = GET_MODE(SET_DEST(set0));
+	  if (is_ea_insn(set0))
+	    {
+	      int i = --n;
+	      for (;i >= 0; --i)
+		{
+		  rtx set = single_set(ready[i]);
+		  if (!set)
+		    continue;
+		  machine_mode mode = GET_MODE(SET_DEST(set));
+		  if (mode != DFmode && mode != SFmode && !is_ea_insn(set))
+		    {
+		      if (i != n)
+			{
+			  rtx_insn * tmp = ready[i];
+			  ready[i] = ready[n];
+			  ready[n] = tmp;
+			}
+		      return 2;
+		    }
+		}
+	    }
+	  else if (mode0 != DFmode && mode0 != SFmode)
+	    {
+	      int i = --n;
+	      for (;i >= 0; --i)
+		{
+		  rtx set = single_set(ready[i]);
+		  if (!set)
+		    continue;
+		  //machine_mode mode = GET_MODE(SET_DEST(set));
+		  if (is_ea_insn(set))
+		    {
+		      if (i != n)
+			{
+			  // move ea insn in front of current.
+			  rtx_insn * tmp = ready[i];
+			  ready[i] = ready[n];
+			  ready[n] = ready[n + 1];
+			  ready[n + 1] = tmp;
+			}
+		      return 2;
+		    }
+		}
+	    }
+	}
+    }
+  return 1;
+}
+
+/**
+ * SBF: add the clobber since peephole2 may undo some shifting.
+ * To avoid effects on regular code, the (clobber (pc)) is emitted.
+ * It does not hurt if it remains in the code.
+ */
+static rtx_insn *
+m68k_gen_doloop_begin(rtx reg ATTRIBUTE_UNUSED, rtx label ATTRIBUTE_UNUSED)
+{
+  rtx x = gen_rtx_CLOBBER(VOIDmode, pc_rtx);
+  rtx_insn *seq;
+  start_sequence ();
+  emit_insn(x);
+  seq = get_insns ();
+  end_sequence ();
+  return seq;
+}
+
+static rtx_insn *
+m68k_gen_doloop_end(rtx reg, rtx label)
+{
+  rtx x = 0;
+  if (GET_MODE (reg) == SImode)
+    x = gen_dbne_si(reg, label);
+  else if (GET_MODE (reg) == HImode)
+    x = gen_dbne_hi(reg, label);
+  else
+    return 0;
+
+  rtx_insn *seq;
+  start_sequence ();
+  emit_jump_insn(x);
+  seq = get_insns ();
+  end_sequence ();
+  return seq;
+}
+
+
+/* Implement TARGET_USE_MOVE_BY_PIECES_INFRASTRUCTURE_P.
+ */
+static bool
+m68k_use_by_pieces_infrastructure_p (unsigned HOST_WIDE_INT size,
+				     unsigned int align ATTRIBUTE_UNUSED,
+				     enum by_pieces_operation op ATTRIBUTE_UNUSED,
+				     bool speed_p ATTRIBUTE_UNUSED)
+{
+  /* no need for small items. */
+  if (align == 16) align = 32;
+  return size * 8 / align < 2;
+}
+
+int
+m68k_emit_setmemsi(rtx blkdest, rtx val, rtx length, rtx alignment)
+{
+  int align = INTVAL(alignment);
+  int size = INTVAL(length);
+  int n = optimize_size ? 4 : 16;
+  rtx regdst = XEXP(blkdest, 0);
+  rtx src, dst;
+  int rest = 0;
+
+  int value = INTVAL(val) & 0xff;
+  if (value != 0)
+    {
+      if (align == 1 && TUNE_68000_10)
+        {
+	  src = gen_reg_rtx(QImode);
+	  emit_move_insn (src, GEN_INT((signed char )value));
+        }
+      else
+	{
+	  src = gen_reg_rtx(SImode);
+	  emit_move_insn(src, GEN_INT(value * 0x1010101));
+	}
+    }
+  else
+    src = val;
+
+  /* SBF: allocate tmp reg.
+   * auto-inc-dec may benefit - maybe not.
+   */
+  dst = gen_reg_rtx(SImode);
+  rtx_insn * dinsn = emit_move_insn(dst, regdst);
+  add_reg_note (dinsn, REG_INC, dst);
+
+  regdst = dst;
+
+  /* move bytes. */
+  if (align == 1 && TUNE_68000_10)
+    {
+      dst = gen_rtx_MEM(QImode, gen_rtx_POST_INC(SImode, regdst));
+    }
+  else
+    {
+      rest = size % 4;
+      size /= 4;
+      dst = gen_rtx_MEM(SImode, gen_rtx_POST_INC(SImode, regdst));
+    }
+
+  int nloops = size / n - 1;
+
+  if (nloops > 160)
+    return false;
+
+  int single = size % n;
+
+  if (nloops == 0)
+    single += n;
+  else if (nloops > 0)
+    {
+      rtx counter = gen_reg_rtx (nloops < 0x10000 ? HImode : SImode);
+      rtx looplabel = gen_label_rtx ();
+      emit_move_insn (counter,
+		      GEN_INT(nloops < 0x10000 ? (short)nloops : nloops));
+      emit_label (looplabel);
+      while (n-- > 0)
+	{
+	  rtx_insn *insn = emit_move_insn (dst, src);
+	  add_reg_note (insn, REG_INC, regdst);
+	}
+      emit_jump_insn (
+	  nloops < 0x10000 ?
+	      gen_dbne_hi (counter, looplabel) :
+	      gen_dbne_si (counter, looplabel));
+    }
+
+  while (single-- > 0)
+    {
+      rtx_insn *insn = emit_move_insn (dst, src);
+      add_reg_note (insn, REG_INC, regdst);
+    }
+
+  // move trailing data
+  if (rest & 2)
+    {
+      dst = gen_rtx_MEM (HImode, gen_rtx_POST_INC(SImode, regdst));
+      rtx_insn *insn = emit_move_insn (dst,
+				       GEN_INT(value + (signed char )value * 0x100));
+      add_reg_note (insn, REG_INC, regdst);
+    }
+  if (rest & 1)
+    {
+      dst = gen_rtx_MEM (QImode, gen_rtx_POST_INC(SImode, regdst));
+      rtx_insn *insn = emit_move_insn (dst, GEN_INT((signed char )value));
+      add_reg_note (insn, REG_INC, regdst);
+    }
+
+  return true;
+}
+
+int
+m68k_emit_movmemsi(rtx blkdest, rtx blksrc, rtx length, rtx alignment)
+{
+  int align = INTVAL(alignment);
+  int size = INTVAL(length);
+  int n = optimize_size ? 4 : 16;
+
+  rtx regsrc = XEXP(blksrc, 0);
+  rtx regdst = XEXP(blkdest, 0);
+  rtx src, dst;
+  int rest = 0;
+
+  // tmp regs for auto inc
+  src = gen_reg_rtx (SImode);
+  rtx_insn *sinsn = emit_move_insn (src, regsrc);
+  add_reg_note (sinsn, REG_INC, src);
+  regsrc = src;
+
+  dst = gen_reg_rtx (SImode);
+  rtx_insn *dinsn = emit_move_insn (dst, regdst);
+  add_reg_note (dinsn, REG_INC, dst);
+  regdst = dst;
+
+  /* move bytes. */
+  if (align == 1 && TUNE_68000_10)
+    {
+      src = gen_rtx_MEM (QImode, gen_rtx_POST_INC(SImode, regsrc));
+      dst = gen_rtx_MEM (QImode, gen_rtx_POST_INC(SImode, regdst));
+    }
+  else
+    {
+      align = 4;
+      rest = size % 4;
+      size /= 4;
+      src = gen_rtx_MEM (SImode, gen_rtx_POST_INC(SImode, regsrc));
+      dst = gen_rtx_MEM (SImode, gen_rtx_POST_INC(SImode, regdst));
+    }
+
+  int nloops = size / n - 1;
+
+  if (nloops > 160)
+    return false;
+
+  int single = size % n;
+
+  rtx add = GEN_INT(align);
+
+  if (nloops == 0)
+    single += n;
+  else if (nloops > 0)
+    {
+      rtx counter = gen_reg_rtx (nloops < 0x10000 ? HImode : SImode);
+      rtx looplabel = gen_label_rtx ();
+      emit_move_insn (counter,
+		      GEN_INT(nloops < 0x10000 ? (short )nloops : nloops));
+      emit_label (looplabel);
+      while (n-- > 0)
+	{
+	  rtx_insn *insn = emit_move_insn (dst, src);
+	  add_reg_note (insn, REG_INC, regsrc);
+	  add_reg_note (insn, REG_INC, regdst);
+	}
+      emit_jump_insn (
+	  nloops < 0x10000 ?
+	      gen_dbne_hi (counter, looplabel) :
+	      gen_dbne_si (counter, looplabel));
+    }
+
+  while (single-- > 0)
+    {
+      rtx_insn *insn = emit_move_insn (dst, src);
+      add_reg_note (insn, REG_INC, regsrc);
+      add_reg_note (insn, REG_INC, regdst);
+    }
+
+  // move trailing data
+  if (rest & 2)
+    {
+      src = gen_rtx_MEM (HImode, gen_rtx_POST_INC(SImode, regsrc));
+      dst = gen_rtx_MEM (HImode, gen_rtx_POST_INC(SImode, regdst));
+      rtx_insn *insn = emit_move_insn (dst, src);
+      add_reg_note (insn, REG_INC, regsrc);
+      add_reg_note (insn, REG_INC, regdst);
+    }
+  if (rest & 1)
+    {
+      src = gen_rtx_MEM (QImode, gen_rtx_POST_INC(SImode, regsrc));
+      dst = gen_rtx_MEM (QImode, gen_rtx_POST_INC(SImode, regdst));
+      rtx_insn *insn = emit_move_insn (dst, src);
+      add_reg_note (insn, REG_INC, regsrc);
+      add_reg_note (insn, REG_INC, regdst);
+    }
+
+  return true;
+}
+
+static section *
+m68k_select_section (tree decl, int reloc ATTRIBUTE_UNUSED,
+		    unsigned HOST_WIDE_INT align ATTRIBUTE_UNUSED)
+{
+  if (decl->base.code == VAR_DECL)
+    {
+      if (decl->base.constant_flag || decl->base.readonly_flag)
+      	return text_section;
+
+      char const * secname = DECL_SECTION_NAME(decl);
+      if (secname == 0)
+	{
+	  tree type = decl->decl_minimal.common.typed.type;
+	  if (type->base.code == ARRAY_TYPE)
+	    type = type->typed.type;
+	  if (type->base.readonly_flag)
+	    return text_section;
+
+	  return data_section;
+	}
+    }
+
+  return default_select_section (decl, reloc, align);
+}
+
 #include "gt-m68k.h"

--- gcc/config/m68k/m68k-devices.def	2016-01-04 14:30:50.000000000 +0000
+++ gcc/config/m68k/m68k-devices.def	2025-11-02 00:56:37.886277413 +0000
@@ -75,7 +75,8 @@ M68K_DEVICE ("68010", m68010,   "68010",
 M68K_DEVICE ("68020", m68020,   "68020", "68020", 68020,    isa_20,    FL_MMU | FL_UCLINUX)
 M68K_DEVICE ("68030", m68030,   "68030", "68020", 68030,    isa_20,    FL_MMU | FL_UCLINUX)
 M68K_DEVICE ("68040", m68040,   "68040", "68040", 68040,    isa_40,    FL_MMU)
-M68K_DEVICE ("68060", m68060,   "68060", "68060", 68060,    isa_40,    FL_MMU)
+M68K_DEVICE ("68060", m68060,   "68060", "68060", 68060,    isa_60,    FL_MMU)
+M68K_DEVICE ("68080", m68080,   "68080", "68080", 68080,    isa_80,    0)
 M68K_DEVICE ("68302", m68302,   "68302", "68000", 68000,    isa_00,    FL_MMU)
 M68K_DEVICE ("68332", m68332,   "68332", "cpu32", cpu32,    isa_cpu32, FL_MMU)
 M68K_DEVICE ("cpu32", cpu32,    "cpu32", "cpu32", cpu32,    isa_cpu32, FL_MMU)

--- gcc/config/m68k/m68kemb.h	2016-01-04 14:30:50.000000000 +0000
+++ gcc/config/m68k/m68kemb.h	2025-11-02 00:56:37.890277473 +0000
@@ -32,12 +32,14 @@
 #define NEEDS_UNTYPED_CALL 1
 
 /* Target OS builtins.  */
+#ifndef TARGET_OS_CPP_BUILTINS
 #define TARGET_OS_CPP_BUILTINS()		\
   do						\
     {						\
       builtin_define ("__embedded__");		\
     }						\
   while (0)
+#endif
 
 /* Override the default LIB_SPEC from gcc.c.  We don't currently support
    profiling, or libg.a.  */

--- gcc/config/m68k/m68k.h	2016-01-04 14:30:50.000000000 +0000
+++ gcc/config/m68k/m68k.h	2025-11-02 00:56:37.886277413 +0000
@@ -26,6 +26,23 @@ along with GCC; see the file COPYING3.
 # define MOTOROLA 0  /* Use the MIT assembly syntax.  */
 #endif
 
+#define TARGET_M68K 1
+
+/* TARGET_AMIGAOS is used in defined(...) */
+#if defined (TARGET_AMIGAOS)
+#define TARGET_AMIGA 1
+#endif
+/* TARGET_AMIGA is used in boolean expressions => need 0 as default. */
+#ifndef TARGET_AMIGA
+#define TARGET_AMIGA 0
+#define DOUBLE_INDIRECT_JUMP 0
+#define PIC_REG 14
+#define TARGET_RESTORE_A4 0
+#define TARGET_ALWAYS_RESTORE_A4 0
+#define amiga_is_const_pic_ref(a) (0)
+#define amigaos_legitimate_src(a) (1)
+#endif
+
 /* Handle --with-cpu default option from configure script.  */
 #define OPTION_DEFAULT_SPECS						\
   { "cpu",   "%{!m68020-40:%{!m68020-60:\
@@ -86,7 +103,9 @@ along with GCC; see the file COPYING3.
 	case u68060:							\
 	  builtin_define_std ("mc68060");				\
 	  break;							\
-									\
+	case u68080:							\
+	  builtin_define_std ("mc68080");				\
+	  break;							\
 	case u68020_60:							\
 	  builtin_define_std ("mc68060");				\
 	  /* Fall through.  */						\
@@ -204,7 +223,11 @@ along with GCC; see the file COPYING3.
 #define INT_OP_DC	3	/* dc.b, dc.w, dc.l */
 
 /* Set the default.  */
+#ifndef TARGET_AMIGAOS_VASM
 #define INT_OP_GROUP INT_OP_DOT_WORD
+#else
+#define INT_OP_GROUP INT_OP_DC
+#endif
 
 /* Bit values used by m68k-devices.def to identify processor capabilities.  */
 #define FL_BITFIELD  (1 << 0)    /* Support bitfield instructions.  */
@@ -220,18 +243,23 @@ along with GCC; see the file COPYING3.
 #define FL_ISA_68010 (1 << 10)
 #define FL_ISA_68020 (1 << 11)
 #define FL_ISA_68040 (1 << 12)
-#define FL_ISA_A     (1 << 13)
-#define FL_ISA_APLUS (1 << 14)
-#define FL_ISA_B     (1 << 15)
-#define FL_ISA_C     (1 << 16)
-#define FL_FIDOA     (1 << 17)
-#define FL_CAS	     (1 << 18)	/* Support cas insn.  */
+#define FL_ISA_68060 (1 << 13)
+#define FL_ISA_68080 (1 << 14)
+#define FL_ISA_A     (1 << 15)
+#define FL_ISA_APLUS (1 << 16)
+#define FL_ISA_B     (1 << 17)
+#define FL_ISA_C     (1 << 18)
+#define FL_FIDOA     (1 << 19)
+#define FL_CAS	     (1 << 20)	/* Support cas insn.  */
 #define FL_MMU 	     0   /* Used by multilib machinery.  */
 #define FL_UCLINUX   0   /* Used by multilib machinery.  */
 
+#define TARGET_68000		((m68k_cpu_flags & FL_ISA_68000) != 0)
 #define TARGET_68010		((m68k_cpu_flags & FL_ISA_68010) != 0)
 #define TARGET_68020		((m68k_cpu_flags & FL_ISA_68020) != 0)
 #define TARGET_68040		((m68k_cpu_flags & FL_ISA_68040) != 0)
+#define TARGET_68060		((m68k_cpu_flags & FL_ISA_68060) != 0)
+#define TARGET_68080		((m68k_cpu_flags & FL_ISA_68080) != 0)
 #define TARGET_COLDFIRE		((m68k_cpu_flags & FL_COLDFIRE) != 0)
 #define TARGET_COLDFIRE_FPU	(m68k_fpu == FPUTYPE_COLDFIRE)
 #define TARGET_68881		(m68k_fpu == FPUTYPE_68881)
@@ -246,21 +274,28 @@ along with GCC; see the file COPYING3.
 #define TARGET_ISAC		((m68k_cpu_flags & FL_ISA_C) != 0)
 
 /* Some instructions are common to more than one ISA.  */
-#define ISA_HAS_MVS_MVZ	(TARGET_ISAB || TARGET_ISAC)
+#define ISA_HAS_MVS_MVZ	(TARGET_ISAB || TARGET_ISAC || TARGET_68080)
 #define ISA_HAS_FF1	(TARGET_ISAAPLUS || TARGET_ISAC)
 #define ISA_HAS_TAS	(!TARGET_COLDFIRE || TARGET_ISAB || TARGET_ISAC)
 
 #define TUNE_68000	(m68k_tune == u68000)
 #define TUNE_68010	(m68k_tune == u68010)
 #define TUNE_68000_10	(TUNE_68000 || TUNE_68010)
+#define TUNE_68020	(m68k_tune == u68020 \
+			 || m68k_tune == u68020_40 \
+			 || m68k_tune == u68020_60)
 #define TUNE_68030	(m68k_tune == u68030 \
 			 || m68k_tune == u68020_40 \
 			 || m68k_tune == u68020_60)
 #define TUNE_68040	(m68k_tune == u68040 \
 			 || m68k_tune == u68020_40 \
 			 || m68k_tune == u68020_60)
-#define TUNE_68060	(m68k_tune == u68060 || m68k_tune == u68020_60)
 #define TUNE_68040_60	(TUNE_68040 || TUNE_68060)
+#define TUNE_68060	(m68k_tune == u68060)
+#define TUNE_68080	(m68k_tune == u68080)
+#define TUNE_68020_80	(TUNE_68020 || TUNE_68030 || TUNE_68040 || TUNE_68060 || TUNE_68080)
+#define TUNE_68040_80	(TUNE_68040 || TUNE_68060 || TUNE_68080)
+#define TUNE_68060_80	(TUNE_68060 || TUNE_68080)
 #define TUNE_CPU32	(m68k_tune == ucpu32)
 #define TUNE_CFV1       (m68k_tune == ucfv1)
 #define TUNE_CFV2	(m68k_tune == ucfv2)
@@ -385,7 +420,6 @@ along with GCC; see the file COPYING3.
   16, 17, 18, 19, 20, 21, 22, 23\
 }
 
-
 /* On the m68k, ordinary registers hold 32 bits worth;
    for the 68881 registers, a single register is always enough for
    anything that can be stored in them at all.  */
@@ -428,13 +462,6 @@ along with GCC; see the file COPYING3.
  */
 #define ARG_POINTER_REGNUM 24
 
-#define STATIC_CHAIN_REGNUM A0_REG
-#define M68K_STATIC_CHAIN_REG_NAME REGISTER_PREFIX "a0"
-
-/* Register in which address to store a structure value
-   is passed to a function.  */
-#define M68K_STRUCT_VALUE_REGNUM A1_REG
-
 
 
 /* The m68k has three kinds of registers, so eight classes would be
@@ -449,7 +476,7 @@ enum reg_class {
 #define N_REG_CLASSES (int) LIM_REG_CLASSES
 
 #define REG_CLASS_NAMES \
- { "NO_REGS", "DATA_REGS",              \
+ { "NO_REGS", "DATA_REGS", \
    "ADDR_REGS", "FP_REGS",              \
    "GENERAL_REGS", "DATA_OR_FP_REGS",   \
    "ADDR_OR_FP_REGS", "ALL_REGS" }
@@ -497,31 +524,88 @@ extern enum reg_class regno_reg_class[];
 
 #define FIRST_PARM_OFFSET(FNDECL) 8
 
-/* On the m68k the return value defaults to D0.  */
-#define FUNCTION_VALUE(VALTYPE, FUNC)  \
-  gen_rtx_REG (TYPE_MODE (VALTYPE), D0_REG)
-
-/* On the m68k the return value defaults to D0.  */
-#define LIBCALL_VALUE(MODE)  gen_rtx_REG (MODE, D0_REG)
+/* SBF: same as linux.h */
 
-/* On the m68k, D0 is usually the only register used.  */
-#define FUNCTION_VALUE_REGNO_P(N) ((N) == D0_REG)
+/* 1 if N is a possible register number for a function value.  For
+   m68k/SVR4 allow d0, a0, or fp0 as return registers, for integral,
+   pointer, or floating types, respectively.  Reject fp0 if not using
+   a 68881 coprocessor.  */
+
+#undef FUNCTION_VALUE_REGNO_P
+#define FUNCTION_VALUE_REGNO_P(N) \
+  ((N) == D0_REG || (N) == A0_REG || (TARGET_68881 && (N) == FP0_REG))
 
 /* Define this to be true when FUNCTION_VALUE_REGNO_P is true for
-   more than one register.
-   XXX This macro is m68k specific and used only for m68kemb.h.  */
-#define NEEDS_UNTYPED_CALL 0
+   more than one register.  */
+
+#undef NEEDS_UNTYPED_CALL
+#define NEEDS_UNTYPED_CALL 1
+
+/* Define how to generate (in the callee) the output value of a
+   function and how to find (in the caller) the value returned by a
+   function.  VALTYPE is the data type of the value (as a tree).  If
+   the precise function being called is known, FUNC is its
+   FUNCTION_DECL; otherwise, FUNC is 0.  For m68k/SVR4 generate the
+   result in d0, a0, or fp0 as appropriate.
+
+   SBF: we need the libcall calling convention.
+   */
+
+#undef FUNCTION_VALUE
+#define FUNCTION_VALUE(VALTYPE, FUNC)					\
+		m68k_libcall_value (TYPE_MODE (VALTYPE))
+
+/* Define how to find the value returned by a library function
+   assuming the value has mode MODE.
+   For m68k/SVR4 look for integer values in d0, pointer values in d0
+   (returned in both d0 and a0), and floating values in fp0.  */
+
+#undef LIBCALL_VALUE
+#define LIBCALL_VALUE(MODE)						\
+  m68k_libcall_value (MODE)
 
-/* On the m68k, all arguments are usually pushed on the stack.  */
-#define FUNCTION_ARG_REGNO_P(N) 0
 
-/* On the m68k, this is a single integer, which is a number of bytes
-   of arguments scanned so far.  */
+/* SBF: int is enough public info. rest is handled internally. */
 #define CUMULATIVE_ARGS int
+extern void m68k_init_cumulative_args (CUMULATIVE_ARGS *, tree, tree);
+extern int m68k_function_arg_reg(unsigned regno);
 
-/* On the m68k, the offset starts at 0.  */
+/* Initialize a variable CUM of type CUMULATIVE_ARGS
+   for a call to a function whose data type is FNTYPE.
+   For a library call, FNTYPE is 0.  */
+#undef INIT_CUMULATIVE_ARGS
 #define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, INDIRECT, N_NAMED_ARGS) \
- ((CUM) = 0)
+  (m68k_init_cumulative_args(&(CUM), (FNTYPE), (INDIRECT)))
+
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
+
+/* 1 if N is a possible register number for function argument passing.  */
+#undef FUNCTION_ARG_REGNO_P
+#define FUNCTION_ARG_REGNO_P(N)    m68k_function_arg_reg(N)
+
+/* Register in which address to store a structure value is passed to a
+   function.  The default in m68k.h is a1.  For m68k/SVR4 it is a0.  */
+
+#undef M68K_STRUCT_VALUE_REGNUM
+#define M68K_STRUCT_VALUE_REGNUM A0_REG
+
+/* The static chain regnum defaults to a0, but we use that for
+   structure return, so have to use a1 for the static chain.  */
+
+#undef STATIC_CHAIN_REGNUM
+#define STATIC_CHAIN_REGNUM A1_REG
+#undef M68K_STATIC_CHAIN_REG_NAME
+#define M68K_STATIC_CHAIN_REG_NAME REGISTER_PREFIX "a1"
 
 #define FUNCTION_PROFILER(FILE, LABELNO)  \
   asm_fprintf (FILE, "\tlea %LLP%d,%Ra0\n\tjsr mcount\n", (LABELNO))
@@ -614,11 +698,11 @@ __transfer_from_trampoline ()					\
 
 #define REGNO_OK_FOR_INDEX_P(REGNO)			\
   (INT_REGNO_P (REGNO)					\
-   || INT_REGNO_P (reg_renumber[REGNO]))
+   || (reg_renumber && INT_REGNO_P (reg_renumber[REGNO])))
 
 #define REGNO_OK_FOR_BASE_P(REGNO)			\
   (ADDRESS_REGNO_P (REGNO)				\
-   || ADDRESS_REGNO_P (reg_renumber[REGNO]))
+   || (reg_renumber && ADDRESS_REGNO_P (reg_renumber[REGNO])))
 
 #define REGNO_OK_FOR_INDEX_NONSTRICT_P(REGNO)		\
   (INT_REGNO_P (REGNO)					\
@@ -727,9 +811,49 @@ do { if (cc_prev_status.flags & CC_IN_68
   if (cc_prev_status.flags & CC_NO_OVERFLOW)			\
     return NO_OV;						\
   return NORMAL; } while (0)
+
+#ifdef TARGET_AMIGAOS_VASM
+#define ASM_OUTPUT_ASCII(MYFILE, MYSTRING, MYLENGTH) \
+  do {                                                                        \
+    FILE *_hide_asm_out_file = (MYFILE);                                      \
+    const unsigned char *_hide_p = (const unsigned char *) (MYSTRING);        \
+    int _hide_thissize = (MYLENGTH);                                          \
+    {                                                                         \
+      FILE *asm_out_file = _hide_asm_out_file;                                \
+      const unsigned char *p = _hide_p;                                       \
+      int thissize = _hide_thissize;                                          \
+      int i;                                                                  \
+      fprintf (asm_out_file, "\tdc.b \"");                                    \
+                                                                              \
+      for (i = 0; i < thissize; i++)                                          \
+        {                                                                     \
+          int c = p[i];                                                       \
+          if (c == '\"' || c == '\\')                                         \
+            putc ('\\', asm_out_file);                                        \
+          if (ISPRINT (c))                                                    \
+            putc (c, asm_out_file);                                           \
+          else                                                                \
+            {                                                                 \
+              fprintf (asm_out_file, "\\%o", c);                              \
+              /* After an octal-escape, if a digit follows,                   \
+                 terminate one string constant and start another.             \
+                 The VAX assembler fails to stop reading the escape           \
+                 after three digits, so this is the only way we               \
+                 can get it to parse the data properly.  */                   \
+              if (i < thissize - 1 && ISDIGIT (p[i + 1]))                     \
+                fprintf (asm_out_file, "\"\n\tdc.b \"");                      \
+          }                                                                   \
+        }                                                                     \
+      fprintf (asm_out_file, "\"\n");                                         \
+    }                                                                         \
+  }                                                                           \
+  while (0)
+#endif
 
+
 /* Control the assembler format that we output.  */
 
+#ifndef TARGET_AMIGAOS_VASM
 #define ASM_APP_ON "#APP\n"
 #define ASM_APP_OFF "#NO_APP\n"
 #define TEXT_SECTION_ASM_OP "\t.text"
@@ -739,6 +863,17 @@ do { if (cc_prev_status.flags & CC_IN_68
 #define LOCAL_LABEL_PREFIX ""
 #define USER_LABEL_PREFIX "_"
 #define IMMEDIATE_PREFIX "#"
+#else
+#define ASM_APP_ON ""
+#define ASM_APP_OFF ""
+#define TEXT_SECTION_ASM_OP "\tsection .text,code"
+#define DATA_SECTION_ASM_OP "\tsection .data,data"
+#define GLOBAL_ASM_OP "\txdef\t"
+#define REGISTER_PREFIX ""
+#define LOCAL_LABEL_PREFIX "_."
+#define USER_LABEL_PREFIX "_"
+#define IMMEDIATE_PREFIX "#"
+#endif
 
 #define REGISTER_NAMES \
 {REGISTER_PREFIX"d0", REGISTER_PREFIX"d1", REGISTER_PREFIX"d2",	\
@@ -832,6 +967,7 @@ do { if (cc_prev_status.flags & CC_IN_68
    pointer for code and global references.  We still use pc-relative
    references to data, as this avoids a relocation.  */
 #define ASM_PREFERRED_EH_DATA_FORMAT(CODE, GLOBAL)			   \
+  (TARGET_68000 || TARGET_68010) ? DW_EH_PE_aligned :                      \
   (flag_pic								   \
    && !((TARGET_ID_SHARED_LIBRARY || TARGET_SEP_DATA)			   \
 	&& ((GLOBAL) || (CODE)))					   \
@@ -839,7 +975,7 @@ do { if (cc_prev_status.flags & CC_IN_68
    : DW_EH_PE_absptr)
 
 #define ASM_OUTPUT_LABELREF(FILE,NAME)	\
-  asm_fprintf (FILE, "%U%s", NAME)
+  asm_fprintf (FILE, (*(NAME)) == '@' ? "%s" : "%U%s", NAME)
 
 #define ASM_GENERATE_INTERNAL_LABEL(LABEL,PREFIX,NUM)	\
   sprintf (LABEL, "*%s%s%ld", LOCAL_LABEL_PREFIX, PREFIX, (long)(NUM))
@@ -858,11 +994,17 @@ do { if (cc_prev_status.flags & CC_IN_68
 
 /* The m68k does not use absolute case-vectors, but we must define this macro
    anyway.  */
-#define ASM_OUTPUT_ADDR_VEC_ELT(FILE, VALUE)  \
+#ifndef TARGET_AMIGAOS_VASM
+#define ASM_OUTPUT_ADDR_VEC_ELT(FILE, VALUE)	\
   asm_fprintf (FILE, "\t.long %LL%d\n", VALUE)
-
-#define ASM_OUTPUT_ADDR_DIFF_ELT(FILE, BODY, VALUE, REL)  \
+#define ASM_OUTPUT_ADDR_DIFF_ELT(FILE, BODY, VALUE, REL)	\
   asm_fprintf (FILE, "\t.word %LL%d-%LL%d\n", VALUE, REL)
+#else
+#define ASM_OUTPUT_ADDR_VEC_ELT(FILE, VALUE)	\
+ asm_fprintf (FILE, "\tdc.l %LL%d\n", VALUE)
+#define ASM_OUTPUT_ADDR_DIFF_ELT(FILE, BODY, VALUE, REL)  \
+  asm_fprintf (FILE, "\tdc.w %LL%d-%LL%d\n", VALUE, REL)
+#endif
 
 /* We don't have a way to align to more than a two-byte boundary, so do the
    best we can and don't complain.  */
@@ -872,13 +1014,24 @@ do { if (cc_prev_status.flags & CC_IN_68
 
 #ifdef HAVE_GAS_BALIGN_AND_P2ALIGN
 /* Use "move.l %a4,%a4" to advance within code.  */
+#ifndef TARGET_AMIGAOS_VASM
 #define ASM_OUTPUT_ALIGN_WITH_NOP(FILE,LOG)			\
   if ((LOG) > 0)						\
     fprintf ((FILE), "\t.balignw %u,0x284c\n", 1 << (LOG));
+#else
+#define ASM_OUTPUT_ALIGN_WITH_NOP(FILE,LOG)			\
+  if ((LOG) > 0)						\
+    fprintf ((FILE), "\tcnop 0,%u\n", 1 << (LOG));
+#endif
 #endif
 
+#ifndef TARGET_AMIGAOS_VASM
 #define ASM_OUTPUT_SKIP(FILE,SIZE)  \
   fprintf (FILE, "\t.skip %u\n", (int)(SIZE))
+#else
+#define ASM_OUTPUT_SKIP(FILE,SIZE)  \
+  fprintf (FILE, "\tds.b %u\n", (int)(SIZE))
+#endif
 
 #define ASM_OUTPUT_COMMON(FILE, NAME, SIZE, ROUNDED)  \
 ( fputs (".comm ", (FILE)),			\
@@ -971,3 +1124,64 @@ extern int m68k_sched_address_bypass_p (
 extern int m68k_sched_indexed_address_bypass_p (rtx_insn *, rtx_insn *);
 
 #define CPU_UNITS_QUERY 1
+
+#if 1
+extern void default_stabs_asm_out_constructor (rtx, int);
+extern void default_stabs_asm_out_destructor (rtx, int);
+#endif
+
+/* Structure describing an m68k address.
+
+   If CODE is UNKNOWN, the address is BASE + INDEX * SCALE + OFFSET,
+   with null fields evaluating to 0.  Here:
+
+   - BASE satisfies m68k_legitimate_base_reg_p
+   - INDEX satisfies m68k_legitimate_index_reg_p
+   - OFFSET satisfies m68k_legitimate_constant_address_p
+   - OUTER_INDEX satisfies m68k_legitimate_index_reg_p
+   - OUTER_OFFSET satisfies m68k_legitimate_constant_address_p
+
+   INDEX is either HImode or SImode.  The other fields are SImode.
+
+   If CODE is PRE_DEC, the address is -(BASE).  If CODE is POST_INC,
+   the address is (BASE)+.
+
+   If CODE is MEM, then it's a double indirect address
+   and the outer_index or outer_offset may be used.
+
+   MEM_LOC contains the address of the inner MEM. This is needed by reload
+   if reload needs to reload the inner MEM, if OFFSET plus OUTER_OFFSET are in use.
+
+   BASE_LOC contains the address of BASE - needed by reload.
+   INDEX_LOC contains the address of INDEX - also needed by reload.
+*/
+struct m68k_address {
+  int code;
+  rtx * mem_loc;
+  rtx base;
+  rtx * base_loc;
+  rtx index;
+  rtx * index_loc;
+  int scale;
+  rtx offset;
+  rtx outer_index;
+  rtx * outer_index_loc;
+  int outer_scale;
+  rtx outer_offset;
+};
+
+int decompose_mem(int reach, rtx * x, struct m68k_address * address, int strict_p);
+
+
+const char *
+print_fp_const(const char * cmd, const char * prec, rtx x);
+
+int
+m68k_emit_movmemsi(rtx dest, rtx src, rtx length, rtx alignment);
+
+int
+m68k_emit_setmemsi(rtx dest, rtx val, rtx length, rtx alignment);
+
+
+#define BRANCH_COST(speed_p, predictable_p) 1
+

--- gcc/config/m68k/m68k-isas.def	2016-01-04 14:30:50.000000000 +0000
+++ gcc/config/m68k/m68k-isas.def	2025-11-02 00:56:37.886277413 +0000
@@ -34,6 +34,7 @@ M68K_ISA ("68020",    m68020,     68020,
 M68K_ISA ("68030",    m68030,     68030, isa_20,    FL_FOR_isa_20)
 M68K_ISA ("68040",    m68040,     68040, isa_40,    FL_FOR_isa_40)
 M68K_ISA ("68060",    m68060,     68060, isa_40,    FL_FOR_isa_40)
+M68K_ISA ("68080",    m68080,     68080, isa_80,    FL_FOR_isa_80)
 M68K_ISA ("cpu32",    cpu32,      cpu32, isa_20,    FL_FOR_isa_cpu32)
 M68K_ISA ("isaa",     mcf5206e,   cfv2,  isa_a,     FL_FOR_isa_a | FL_CF_HWDIV)
 M68K_ISA ("isaaplus", mcf5271,    cfv2,  isa_aplus, FL_FOR_isa_aplus | FL_CF_HWDIV)

--- gcc/config/m68k/m68k.md	2016-02-22 14:22:07.000000000 +0000
+++ gcc/config/m68k/m68k.md	2025-11-02 00:56:37.886277413 +0000
@@ -26,7 +26,6 @@
 ;;- removed opcodes and addressing modes off.
 ;;- 
 
-
 ;;- instruction definitions
 
 ;;- @@The original PO technology requires these to be ordered by speed,
@@ -116,6 +115,18 @@
    (UNSPEC_TIE 5)
    (UNSPEC_RELOC16 6)
    (UNSPEC_RELOC32 7)
+   (UNSPEC_TAN 8)
+   (UNSPEC_ASIN 9)
+   (UNSPEC_ACOS 10)
+   (UNSPEC_ATAN 11)
+   (UNSPEC_EXP 12)
+   (UNSPEC_EXPM1 13)
+   (UNSPEC_EXP10 14)
+   (UNSPEC_EXP2 15)
+   (UNSPEC_LOG 16)
+   (UNSPEC_LOG1P 17)
+   (UNSPEC_LOG10 18)
+   (UNSPEC_LOG2	19)
   ])
 
 ;; UNSPEC_VOLATILE usage:
@@ -128,15 +139,19 @@
    (UNSPECV_TAS_2	4)
   ])
 
-;; Registers by name.
+;; Registers by name. SBF: Do not define PIC_REG here!
 (define_constants
   [(D0_REG		0)
+   (D1_REG		1)
+   (D2_REG		2)
+   (D7_REG		7)
    (A0_REG		8)
    (A1_REG		9)
-   (PIC_REG		13)
    (A6_REG		14)
    (SP_REG		15)
    (FP0_REG		16)
+   (FP1_REG		17)
+   (FP7_REG		23)
   ])
 
 (include "predicates.md")
@@ -149,7 +164,7 @@
 ;; ::::::::::::::::::::
 
 ;; Processor type.
-(define_attr "cpu" "cfv1, cfv2, cfv3, cfv4, unknown"
+(define_attr "cpu" "cfv1, cfv2, cfv3, cfv4, m68080, unknown"
   (const (symbol_ref "m68k_sched_cpu")))
 
 ;; MAC type.
@@ -275,7 +290,36 @@
 ;; Allowable 68881 constant constraints
 (define_mode_attr const [(SF "F") (DF "G") (XF "")])
 
+;; Argument 0 is the destination
+;; Argument 1 is the source
+;; Argument 2 is the length
+;; Argument 3 is the alignment
+(define_expand "movmemsi"
+  [(parallel [(set (match_operand:BLK 0 "general_operand")
+		   (match_operand:BLK 1 "general_operand"))
+	      (use (match_operand:SI 2 "const_int_operand"))
+	      (use (match_operand:SI 3 "const_int_operand"))])]
+  ""
+{
+	if (m68k_emit_movmemsi(operands[0], operands[1], operands[2], operands[3]))
+	  DONE;
+	else
+	  FAIL;
+})
 
+(define_expand "setmemsi"
+  [(parallel [(set (match_operand:BLK 0 "" "")
+		   (match_operand:SI 1 "const_int_operand" ""))
+	      (use (match_operand:SI 2 "const_int_operand" ""))
+	      (use (match_operand:SI 3 "const_int_operand" ""))])]
+  ""
+{
+  if (m68k_emit_setmemsi (operands[0], operands[2], operands[1], operands[3]))
+    DONE;
+  else
+    FAIL;
+})
+
 (define_insn_and_split "*movdf_internal"
   [(set (match_operand:DF 0 "push_operand"   "=m, m")
 	(match_operand:DF 1 "general_operand" "f, ro<>E"))]
@@ -502,7 +546,31 @@
   [(set (cc0)
         (compare (match_operand:SI 0 "nonimmediate_operand" "rKT,rKs,mr,ma,>")
                  (match_operand:SI 1 "general_operand" "mr,ma,KTr,Ksr,>")))]
-  "!TARGET_COLDFIRE"
+  "!TARGET_COLDFIRE && !TUNE_68040_80"
+{
+  if (GET_CODE (operands[0]) == MEM && GET_CODE (operands[1]) == MEM)
+    return "cmpm%.l %1,%0";
+  if (REG_P (operands[1])
+      || (!REG_P (operands[0]) && GET_CODE (operands[0]) != MEM))
+    {
+      cc_status.flags |= CC_REVERSED; /*|*/
+      return "cmp%.l %d0,%d1";
+    }
+  if (GET_CODE (operands[1]) == CONST_INT
+      && INTVAL (operands[1]) < 0x8000
+      && INTVAL (operands[1]) >= -0x8000)
+    {
+	  if (ADDRESS_REG_P (operands[0]))
+        return "cmp%.w %1,%0";
+    }
+  return "cmp%.l %d1,%d0";
+})
+
+(define_insn ""
+  [(set (cc0)
+        (compare (match_operand:SI 0 "nonimmediate_operand" "rnT,rKs,mr,ma,>")
+                 (match_operand:SI 1 "general_operand" "mr,ma,nTr,Ksr,>")))]
+  "!TARGET_COLDFIRE && TUNE_68040_80"
 {
   if (GET_CODE (operands[0]) == MEM && GET_CODE (operands[1]) == MEM)
     return "cmpm%.l %1,%0";
@@ -512,14 +580,19 @@
       cc_status.flags |= CC_REVERSED; /*|*/
       return "cmp%.l %d0,%d1";
     }
-  if (ADDRESS_REG_P (operands[0])
-      && GET_CODE (operands[1]) == CONST_INT
+  if (GET_CODE (operands[1]) == CONST_INT
       && INTVAL (operands[1]) < 0x8000
       && INTVAL (operands[1]) >= -0x8000)
-    return "cmp%.w %1,%0";
+    {
+	  if (ADDRESS_REG_P (operands[0]))
+        return "cmp%.w %1,%0";
+      if (TUNE_68080)
+        return "cmpiw%.l %1,%0";
+    }
   return "cmp%.l %d1,%d0";
 })
 
+
 (define_insn "*cmpsi_cf"
   [(set (cc0)
 	(compare (match_operand:SI 0 "nonimmediate_operand" "mrKs,r")
@@ -818,13 +891,13 @@
    pea %a1"
   [(set_attr "type" "clr_l,mov3q_l,pea")])
 
-;This is never used.
-;(define_insn "swapsi"
-;  [(set (match_operand:SI 0 "nonimmediate_operand" "+r")
-;	(match_operand:SI 1 "general_operand" "+r"))
-;   (set (match_dup 1) (match_dup 0))]
-;  ""
-;  "exg %1,%0")
+;This is used if spill registers are sold out.
+(define_insn "swapsi"
+  [(set (match_operand 0 "nonimmediate_operand" "+r")
+	(match_operand 1 "nonimmediate_operand" "+r"))
+   (set (match_dup 1) (match_dup 0))]
+  ""
+  "exg %1,%0")
 
 ;; Special case of fullword move when source is zero for 68000_10.
 ;; moveq is faster on the 68000.
@@ -840,14 +913,15 @@
    (set_attr "opy" "*,0,*")])
 
 ;; Special case of fullword move when source is zero for 68040_60.
-;; On the '040, 'subl an,an' takes 2 clocks while lea takes only 1
+;; NOPE: On the '040, 'subl an,an' takes 2 clocks while lea takes only 1
+;; also takes 1 cycle!
 (define_insn "*movsi_const0_68040_60"
   [(set (match_operand:SI 0 "movsi_const0_operand" "=a,g")
 	(const_int 0))]
-  "TUNE_68040_60"
+  "TUNE_68040_80"
 {
   if (which_alternative == 0)
-    return MOTOROLA ? "lea 0.w,%0" : "lea 0:w,%0";
+    return "sub%.l %0,%0";
   else if (which_alternative == 1)
     return "clr%.l %0";
   else
@@ -862,7 +936,7 @@
 (define_insn "*movsi_const0"
   [(set (match_operand:SI 0 "movsi_const0_operand" "=a,g")
 	(const_int 0))]
-  "!(TUNE_68000_10 || TUNE_68040_60)"
+  "!(TUNE_68000_10 || TUNE_68040_80)"
   "@
    sub%.l %0,%0
    clr%.l %0"
@@ -959,7 +1033,7 @@
   ;; We don't allow f-regs since fixed point cannot go in them.
   [(set (match_operand:SI 0 "nonimmediate_operand" "=g,d,a<")
         (match_operand:SI 1 "general_src_operand" "damSnT,n,i"))]
-  "!TARGET_COLDFIRE && reload_completed"
+  "!TARGET_COLDFIRE && (reload_completed || TUNE_68040_80)"
 {
   return output_move_simode (operands);
 })
@@ -971,7 +1045,7 @@
   [(set (match_operand:SI 0 "nonimmediate_operand" "=g,d,a<")
         (match_operand:SI 1 "general_src_operand" "damSKT,n,i"))]
 
-  "!TARGET_COLDFIRE"
+  "!TARGET_COLDFIRE && !TUNE_68040_80"
 {
   return output_move_simode (operands);
 })
@@ -1186,11 +1260,7 @@
     {
       if (ADDRESS_REG_P (operands[0]))
 	{
-	  /* On the '040, 'subl an,an' takes 2 clocks while lea takes only 1 */
-	  if (TUNE_68040_60)
-	    return MOTOROLA ? "lea 0.w,%0" : "lea 0:w,%0";
-	  else
-	    return "sub%.l %0,%0";
+	  return "sub%.l %0,%0";
 	}
       /* moveq is faster on the 68000.  */
       if (DATA_REG_P (operands[0]) && TUNE_68000_10)
@@ -1338,7 +1408,9 @@
         return "fmove%.d %f1,%0";
     }
   return output_move_double (operands);
-})
+}
+  [(set_attr "type" "fmove")])
+
 
 (define_insn_and_split "movdf_cf_soft"
   [(set (match_operand:DF 0 "nonimmediate_operand" "=r,g")
@@ -1514,7 +1586,7 @@
   "")
 
 ;; movdi can apply to fp regs in some cases
-(define_insn ""
+(define_insn "*movdi"
   ;; Let's see if it really still needs to handle fp regs, and, if so, why.
   [(set (match_operand:DI 0 "nonimmediate_operand" "=rm,r,&ro<>")
 	(match_operand:DI 1 "general_operand" "rF,m,roi<>F"))]
@@ -1551,6 +1623,10 @@
       else
         return "fmove%.d %f1,%0";
     }
+  else if (TARGET_68080 && MEM_P (operands[0]) && GET_CODE (operands[1]) == CONST_INT && INTVAL (operands[1]) == 0)
+    {
+		return "clr%.q %0";		
+	}
   return output_move_double (operands);
 })
 
@@ -1566,7 +1642,7 @@
 ;; so we will prefer it to them.
 
 (define_insn "pushasi"
-  [(set (match_operand:SI 0 "push_operand" "=m")
+  [(set (match_operand:SI 0 "push_operand" "=<")
 	(match_operand:SI 1 "address_operand" "p"))]
   ""
   "pea %a1"
@@ -1765,12 +1841,27 @@
   ""
   "#")
 
+;; early split on 68000 late on 68020 with the quick clr.l dx
+(define_split
+  [(set (match_operand:SI 0 "register_operand" "")
+	(zero_extend (match_operand 1 "nonimmediate_src_operand" "")))]
+  "!ISA_HAS_MVS_MVZ
+   && ((TUNE_68000_10 && !reload_completed) || (!TUNE_68000_10 && reload_completed))
+   && REG_P (operands[0]) // no subregs!
+   && !reg_mentioned_p (operands[0], operands[1])"
+  [(set (match_dup 0) (const_int 0))
+   (set (strict_low_part (match_dup 2))
+	(match_dup 1))]
+{
+  operands[2] = gen_lowpart (GET_MODE (operands[1]), operands[0]);
+})
+
 ;; these two pattern split everything else which isn't matched by
 ;; something else above
 (define_split
   [(set (match_operand 0 "register_operand" "")
 	(zero_extend (match_operand 1 "nonimmediate_src_operand" "")))]
-  "!ISA_HAS_MVS_MVZ
+  "(!ISA_HAS_MVS_MVZ || TARGET_68080)
    && reload_completed
    && reg_mentioned_p (operands[0], operands[1])"
   [(set (strict_low_part (match_dup 2))
@@ -1786,7 +1877,7 @@
 (define_split
   [(set (match_operand 0 "register_operand" "")
 	(zero_extend (match_operand 1 "nonimmediate_src_operand" "")))]
-  "!ISA_HAS_MVS_MVZ && reload_completed"
+  "(!ISA_HAS_MVS_MVZ || TARGET_68080) && reload_completed"
   [(set (match_dup 0)
 	(const_int 0))
    (set (strict_low_part (match_dup 2))
@@ -1964,7 +2055,7 @@
   "")
 
 (define_insn ""
-  [(set (match_operand:DF 0 "nonimmediate_operand" "=*fdm,f")
+  [(set (match_operand:DF 0 "nonimmediate_operand" "=*fm,f")
 	(float_extend:DF
 	  (match_operand:SF 1 "general_operand" "f,dmF")))]
   "TARGET_68881"
@@ -2027,7 +2118,7 @@
   [(set (match_operand:SF 0 "nonimmediate_operand" "=f")
 	(float_truncate:SF
 	  (match_operand:DF 1 "general_operand" "fmG")))]
-  "TARGET_68881 && TARGET_68040"
+  "TARGET_68881 && (TARGET_68040 || TARGET_68060 || TARGET_68080)"
 {
   if (FP_REG_P (operands[1]))
     return "f%$move%.x %1,%0";
@@ -2136,6 +2227,24 @@
   "TARGET_68881 && TUNE_68040"
 {
   CC_STATUS_INIT;
+  // if there is an overlap, find regs which aren't used in operands[0]
+  if (MEM_P (operands[0])) {
+      int regno = -1;
+	  if (reg_overlap_mentioned_p (operands[2], operands[0])) {
+	      regno = (REGNO (operands[2]) + 1) % 8;
+	      if (regno == REGNO (operands[3]))
+	        regno = (regno + 1) % 8;
+		  operands[2] = gen_rtx_REG(SImode, regno);	      
+  return "move.l %2,-(a7)\;fmovem%.l %!,%2\;moveq #16,%3\;or%.l %2,%3\;and%.w #-33,%3\;fmovem%.l %3,%!\;fmove%.l %1,%0\;fmovem%.l %2,%!\;move.l (a7)+,%2";
+	  } 
+	  if (reg_overlap_mentioned_p (operands[3], operands[0])) {
+	      regno = (REGNO (operands[3]) + 1) % 8;
+	      if (regno == REGNO (operands[2]))
+	        regno = (regno + 1) % 8;
+		  operands[3] = gen_rtx_REG(SImode, regno);	      
+  return "move.l %3,-(a7)\;fmovem%.l %!,%2\;moveq #16,%3\;or%.l %2,%3\;and%.w #-33,%3\;fmovem%.l %3,%!\;fmove%.l %1,%0\;fmovem%.l %2,%!\;move.l (a7)+,%3";
+	  }
+  }
   return "fmovem%.l %!,%2\;moveq #16,%3\;or%.l %2,%3\;and%.w #-33,%3\;fmovem%.l %3,%!\;fmove%.l %1,%0\;fmovem%.l %2,%!";
 })
 
@@ -2147,6 +2256,24 @@
   "TARGET_68881 && TUNE_68040"
 {
   CC_STATUS_INIT;
+  // if there is an overlap, find regs which aren't used in operands[0]
+  if (MEM_P (operands[0]) ){
+      int regno = -1;
+	  if (reg_overlap_mentioned_p (operands[2], operands[0])) {
+	      regno = (REGNO (operands[2]) + 1) % 8;
+	      if (regno == REGNO (operands[3]))
+	        regno = (regno + 1) % 8;
+		  operands[2] = gen_rtx_REG(SImode, regno);	      
+  return "move.l %2,-(a7)\;fmovem%.l %!,%2\;moveq #16,%3\;or%.l %2,%3\;and%.w #-33,%3\;fmovem%.l %3,%!\;fmove%.w %1,%0\;fmovem%.l %2,%!\;move.l (a7)+,%2";
+	  } 
+	  if (reg_overlap_mentioned_p (operands[3], operands[0])) {
+	      regno = (REGNO (operands[3]) + 1) % 8;
+	      if (regno == REGNO (operands[2]))
+	        regno = (regno + 1) % 8;
+		  operands[3] = gen_rtx_REG(SImode, regno);	      
+  return "move.l %3,-(a7)\;fmovem%.l %!,%2\;moveq #16,%3\;or%.l %2,%3\;and%.w #-33,%3\;fmovem%.l %3,%!\;fmove%.w %1,%0\;fmovem%.l %2,%!\;move.l (a7)+,%3";
+	  }
+  }
   return "fmovem%.l %!,%2\;moveq #16,%3\;or%.l %2,%3\;and%.w #-33,%3\;fmovem%.l %3,%!\;fmove%.w %1,%0\;fmovem%.l %2,%!";
 })
 
@@ -2158,6 +2285,24 @@
   "TARGET_68881 && TUNE_68040"
 {
   CC_STATUS_INIT;
+  // if there is an overlap, find regs which aren't used in operands[0]
+  if (MEM_P (operands[0])) {
+      int regno = -1;
+	  if (reg_overlap_mentioned_p (operands[2], operands[0])) {
+	      regno = (REGNO (operands[2]) + 1) % 8;
+	      if (regno == REGNO (operands[3]))
+	        regno = (regno + 1) % 8;
+		  operands[2] = gen_rtx_REG(SImode, regno);	      
+  return "move.l %2,-(a7)\;fmovem%.l %!,%2\;moveq #16,%3\;or%.l %2,%3\;and%.w #-33,%3\;fmovem%.l %3,%!\;fmove%.b %1,%0\;fmovem%.l %2,%!\;move.l (a7)+,%2";
+	  } 
+	  if (reg_overlap_mentioned_p (operands[3], operands[0])) {
+	      regno = (REGNO (operands[3]) + 1) % 8;
+	      if (regno == REGNO (operands[2]))
+	        regno = (regno + 1) % 8;
+		  operands[3] = gen_rtx_REG(SImode, regno);	      
+  return "move.l %3,-(a7)\;fmovem%.l %!,%2\;moveq #16,%3\;or%.l %2,%3\;and%.w #-33,%3\;fmovem%.l %3,%!\;fmove%.b %1,%0\;fmovem%.l %2,%!\;move.l (a7)+,%3";
+	  }
+  }
   return "fmovem%.l %!,%2\;moveq #16,%3\;or%.l %2,%3\;and%.w #-33,%3\;fmovem%.l %3,%!\;fmove%.b %1,%0\;fmovem%.l %2,%!";
 })
 
@@ -2468,9 +2613,9 @@
 ;; This is needed since they are not themselves reloaded,
 ;; so commutativity won't apply to them.
 (define_insn "*addsi3_internal"
-  [(set (match_operand:SI 0 "nonimmediate_operand" "=m,?a,?a,d,a")
-        (plus:SI (match_operand:SI 1 "general_operand" "%0,a,rJK,0,0")
-                 (match_operand:SI 2 "general_src_operand" "dIKLT,rJK,a,mSrIKLT,mSrIKLs")))]
+  [(set (match_operand:SI 0 "nonimmediate_operand" "=m,d,?a,?a,?a")
+        (plus:SI (match_operand:SI 1 "general_operand" "%0,0,0,a,rJK")
+                 (match_operand:SI 2 "general_src_operand" "dIKLT,mSrIKLT,mSrIKLs,rJK,a")))]
 
 
   "! TARGET_COLDFIRE"
@@ -2576,7 +2721,7 @@
 	      return "subq%.w #8,%0\;subq%.w %2,%0";
 	    }
 	}
-      if (ADDRESS_REG_P (operands[0]) && !TUNE_68040)
+      if (ADDRESS_REG_P (operands[0]))
 	return MOTOROLA ? "lea (%c2,%0),%0" : "lea %0@(%c2),%0";
     }
   return "add%.w %2,%0";
@@ -2631,7 +2776,7 @@
 	      return "subq%.w #8,%0\;subq%.w %1,%0";
 	    }
 	}
-      if (ADDRESS_REG_P (operands[0]) && !TUNE_68040)
+      if (ADDRESS_REG_P (operands[0]))
 	return MOTOROLA ? "lea (%c1,%0),%0" : "lea %0@(%c1),%0";
     }
   return "add%.w %1,%0";
@@ -2680,7 +2825,7 @@
 	      return "subq%.w #8,%0\;subq%.w %1,%0";
 	    }
 	}
-      if (ADDRESS_REG_P (operands[0]) && !TUNE_68040)
+      if (ADDRESS_REG_P (operands[0]))
 	return MOTOROLA ? "lea (%c1,%0),%0" : "lea %0@(%c1),%0";
     }
   return "add%.w %1,%0";
@@ -2760,7 +2905,8 @@
 	(plus:FP (match_operand:FP 1 "general_operand" "")
 		 (match_operand:FP 2 "general_operand" "")))]
   "TARGET_HARD_FLOAT"
-  "")
+  ""
+[(set_attr "type" "falu")])
 
 (define_insn "add<mode>3_floatsi_68881"
   [(set (match_operand:FP 0 "nonimmediate_operand" "=f")
@@ -2797,6 +2943,8 @@
 {
   if (FP_REG_P (operands[2]))
     return "f<FP:round>add%.x %2,%0";
+  if (GET_CODE (operands[2]) == CONST_DOUBLE)
+    return print_fp_const("f<FP:round>add%.", "<FP:prec>", operands[2]);
   return "f<FP:round>add%.<FP:prec> %f2,%0";
 }
   [(set_attr "type" "falu")
@@ -3021,6 +3169,8 @@
 {
   if (FP_REG_P (operands[2]))
     return "f<FP:round>sub%.x %2,%0";
+  if (GET_CODE (operands[2]) == CONST_DOUBLE)
+    return print_fp_const("f<FP:round>sub%.", "<FP:prec>", operands[2]);  
   return "f<FP:round>sub%.<FP:prec> %f2,%0";
 }
   [(set_attr "type" "falu")
@@ -3088,7 +3238,6 @@
   [(set (match_operand:SI 0 "nonimmediate_operand" "=d")
 	(mult:SI (match_operand:SI 1 "general_operand" "%0")
                  (match_operand:SI 2 "general_src_operand" "dmSTK")))]
-
   "TARGET_68020"
   "muls%.l %2,%0"
   [(set_attr "type" "mul_l")
@@ -3128,7 +3277,7 @@
   [(set_attr "type" "mul_w")
    (set_attr "opy" "2")])
 
-;; We need a separate DEFINE_EXPAND for u?mulsidi3 to be able to use the
+;; We need a separate DEFINE_EXPAND for umulsidi3 to be able to use the
 ;; proper matching constraint.  This is because the matching is between
 ;; the high-numbered word of the DImode operand[0] and operand[1].
 (define_expand "umulsidi3"
@@ -3143,7 +3292,7 @@
   "TARGET_68020 && !TUNE_68060 && !TARGET_COLDFIRE"
   "")
 
-(define_insn ""
+(define_insn "*umulsidi3_68020"
   [(set (match_operand:SI 0 "register_operand" "=d")
 	(mult:SI (match_operand:SI 1 "register_operand" "%0")
 		  (match_operand:SI 2 "nonimmediate_operand" "dm")))
@@ -3307,7 +3456,7 @@
 		 (match_operand:FP 1 "general_operand" "0")))]
   "TARGET_68881"
 {
-  return TARGET_68040
+  return (TARGET_68040 || TARGET_68060 || TARGET_68080)
 	 ? "f<FP:round>mul%.l %2,%0"
 	 : "f<FP:round_mul>mul%.l %2,%0";
 }
@@ -3320,7 +3469,7 @@
 		 (match_operand:FP 1 "general_operand" "0")))]
   "TARGET_68881"
 {
-  return TARGET_68040
+  return (TARGET_68040 || TARGET_68060 || TARGET_68080)
 	 ? "f<FP:round>mul%.w %2,%0"
 	 : "f<FP:round_mul>mul%.w %2,%0";
 }
@@ -3333,7 +3482,7 @@
 		 (match_operand:FP 1 "general_operand" "0")))]
   "TARGET_68881"
 {
-  return TARGET_68040
+  return (TARGET_68040 || TARGET_68060 || TARGET_68080)
 	 ? "f<FP:round>mul%.b %2,%0"
 	 : "f<FP:round_mul>mul%.b %2,%0";
 }
@@ -3347,7 +3496,7 @@
   "TARGET_68881"
 {
   if (GET_CODE (operands[2]) == CONST_DOUBLE
-      && floating_exact_log2 (operands[2]) && !TUNE_68040_60)
+      && floating_exact_log2 (operands[2]) && !TUNE_68040_80)
     {
       int i = floating_exact_log2 (operands[2]);
       operands[2] = GEN_INT (i);
@@ -3355,8 +3504,11 @@
     }
   if (REG_P (operands[2]))
     return "f%&mul%.x %2,%0";
+  if (GET_CODE (operands[2]) == CONST_DOUBLE)
+    return print_fp_const("f%&mul%.", "<FP:prec>", operands[2]);
   return "f%&mul%.d %f2,%0";
-})
+}
+[(set_attr "type" "fmul")])
 
 (define_insn "mulsf_68881"
   [(set (match_operand:SF 0 "nonimmediate_operand" "=f")
@@ -3365,13 +3517,14 @@
   "TARGET_68881"
 {
   if (FP_REG_P (operands[2]))
-    return (TARGET_68040
+    return ((TARGET_68040 || TARGET_68060 || TARGET_68080)
 	    ? "fsmul%.x %2,%0"
 	    : "fsglmul%.x %2,%0");
-  return (TARGET_68040
+  return ((TARGET_68040 || TARGET_68060 || TARGET_68080)
 	  ? "fsmul%.s %f2,%0"
 	  : "fsglmul%.s %f2,%0");
-})
+}
+[(set_attr "type" "fmul")])
 
 (define_insn "mulxf3_68881"
   [(set (match_operand:XF 0 "nonimmediate_operand" "=f")
@@ -3380,7 +3533,8 @@
   "TARGET_68881"
 {
   return "fmul%.x %f2,%0";
-})
+}
+[(set_attr "type" "fmul")])
 
 (define_insn "fmul<mode>3_cf"
   [(set (match_operand:FP 0 "nonimmediate_operand" "=f")
@@ -3410,7 +3564,7 @@
 		(float:FP (match_operand:SI 2 "general_operand" "dmi"))))]
   "TARGET_68881"
 {
-  return TARGET_68040
+  return (TARGET_68040 || TARGET_68060 || TARGET_68080)
 	 ? "f<FP:round>div%.l %2,%0"
 	 : "f<FP:round_mul>div%.l %2,%0";
 })
@@ -3421,7 +3575,7 @@
 		(float:FP (match_operand:HI 2 "general_operand" "dmn"))))]
   "TARGET_68881"
 {
-  return TARGET_68040
+  return (TARGET_68040 || TARGET_68060 || TARGET_68080)
 	 ? "f<FP:round>div%.w %2,%0"
 	 : "f<FP:round_mul>div%.w %2,%0";
 })
@@ -3432,7 +3586,7 @@
 		(float:FP (match_operand:QI 2 "general_operand" "dmn"))))]
   "TARGET_68881"
 {
-  return TARGET_68040
+  return (TARGET_68040 || TARGET_68060 || TARGET_68080)
 	 ? "f<FP:round>div%.b %2,%0"
 	 : "f<FP:round_mul>div%.b %2,%0";
 })
@@ -3444,13 +3598,18 @@
   "TARGET_68881"
 {
   if (FP_REG_P (operands[2]))
-    return (TARGET_68040
+    return ((TARGET_68040 || TARGET_68060 || TARGET_68080)
 	    ? "f<FP:round>div%.x %2,%0"
 	    : "f<FP:round_mul>div%.x %2,%0");
-  return (TARGET_68040
+  if (GET_CODE (operands[2]) == CONST_DOUBLE)
+    return print_fp_const(TARGET_68040 ? "f<FP:round>div."
+    				: "f<FP:round_mul>div.", "<FP:prec>", operands[2]);
+  return ((TARGET_68040 || TARGET_68060 || TARGET_68080)
 	  ? "f<FP:round>div%.<FP:prec> %f2,%0"
 	  : "f<FP:round_mul>div%.<FP:prec> %f2,%0");
-})
+}
+  [(set_attr "type" "fdiv")])
+
 
 (define_insn "div<mode>3_cf"
   [(set (match_operand:FP 0 "nonimmediate_operand" "=f")
@@ -3549,7 +3708,8 @@
     return "divu%.l %2,%0";
   else
     return "divul%.l %2,%3:%0";
-})
+}
+  [(set_attr "type" "div_l")])
 
 (define_insn "divmodhi4"
   [(set (match_operand:HI 0 "nonimmediate_operand" "=d")
@@ -3627,16 +3787,25 @@
   [(set (match_operand:SI 0 "not_sp_operand" "=m,d")
 	(and:SI (match_operand:SI 1 "general_operand" "%0,0")
 		(match_operand:SI 2 "general_src_operand" "dKT,dmSM")))]
-  "!TARGET_COLDFIRE"
+  "!TARGET_COLDFIRE && !TUNE_68080"
 {
   return output_andsi3 (operands);
 })
 
-(define_insn "andsi3_5200"
+(define_insn "andsi3_68080"
   [(set (match_operand:SI 0 "not_sp_operand" "=m,d")
 	(and:SI (match_operand:SI 1 "general_operand" "%0,0")
-		(match_operand:SI 2 "general_src_operand" "d,dmsK")))]
-  "TARGET_COLDFIRE"
+		(match_operand:SI 2 "general_src_operand" "dKT,dmSMi")))]
+  "TUNE_68080"
+{
+  return output_andsi3 (operands);
+})
+
+(define_insn "andsi3_5200_68080"
+  [(set (match_operand:SI 0 "not_sp_operand" "=m,d")
+        (and:SI (match_operand:SI 1 "general_operand" "%0,0")
+                (match_operand:SI 2 "general_src_operand" "d,dmsK")))]
+  "TARGET_COLDFIRE || TARGET_68080"
 {
   if (ISA_HAS_MVS_MVZ
       && DATA_REG_P (operands[0])
@@ -3730,7 +3899,16 @@
   [(set (match_operand:SI 0 "nonimmediate_operand" "=m,d")
 	(ior:SI (match_operand:SI 1 "general_operand" "%0,0")
                 (match_operand:SI 2 "general_src_operand" "dKT,dmSMT")))]
-  "! TARGET_COLDFIRE"
+  "! TARGET_COLDFIRE && !TUNE_68080"
+{
+  return output_iorsi3 (operands);
+})
+
+(define_insn "iorsi3_68080"
+  [(set (match_operand:SI 0 "nonimmediate_operand" "=m,d")
+	(ior:SI (match_operand:SI 1 "general_operand" "%0,0")
+                (match_operand:SI 2 "general_src_operand" "dKT,dmSMTi")))]
+  "TUNE_68080"
 {
   return output_iorsi3 (operands);
 })
@@ -4573,12 +4751,12 @@
 
 ;; On the 68000, this makes faster code in a special case.
 
-(define_insn "ashlsi_17_24"
+(define_insn "ashlsi_18_24"
   [(set (match_operand:SI 0 "register_operand" "=d")
 	(ashift:SI (match_operand:SI 1 "register_operand" "0")
 		   (match_operand:SI 2 "const_int_operand" "n")))]
   "TUNE_68000_10
-   && INTVAL (operands[2]) > 16
+   && INTVAL (operands[2]) > 17
    && INTVAL (operands[2]) <= 24"
 {
   CC_STATUS_INIT;
@@ -4587,6 +4765,32 @@
   return "lsl%.w %2,%0\;swap %0\;clr%.w %0";
 })
 
+(define_insn "ashlsi_17"
+  [(set (match_operand:SI 0 "register_operand" "=d")
+	(ashift:SI (match_operand:SI 1 "register_operand" "0")
+		   (match_operand:SI 2 "const_int_operand" "n")))]
+  "TUNE_68000_10
+   && INTVAL (operands[2]) == 17"
+{
+  CC_STATUS_INIT;
+
+  operands[2] = GEN_INT (INTVAL (operands[2]) - 16);
+  return "add.w %0,%0\;swap %0\;clr%.w %0";
+})
+
+(define_insn "ashlsi_9_16"
+  [(set (match_operand:SI 0 "register_operand" "=d")
+	(ashift:SI (match_operand:SI 1 "register_operand" "0")
+		   (match_operand:SI 2 "const_int_operand" "n")))]
+  "INTVAL (operands[2]) <= 16 && INTVAL (operands[2]) > 8"
+{
+  CC_STATUS_INIT;
+
+  operands[2] = GEN_INT (INTVAL (operands[2]) - 8);
+  return "lsl.l #8,%0\;lsl.l %2,%0";
+})
+
+
 (define_insn "ashlsi3"
   [(set (match_operand:SI 0 "register_operand" "=d")
 	(ashift:SI (match_operand:SI 1 "register_operand" "0")
@@ -4604,30 +4808,64 @@
 (define_insn "ashlhi3"
   [(set (match_operand:HI 0 "register_operand" "=d")
 	(ashift:HI (match_operand:HI 1 "register_operand" "0")
-		   (match_operand:HI 2 "general_operand" "dI")))]
+		   (match_operand:SI 2 "general_operand" "dn")))]
   "!TARGET_COLDFIRE"
-  "lsl%.w %2,%0")
+  {
+    if (CONST_INT_P (operands[2]))
+      {
+        int i = INTVAL (operands[2]) & 63;
+     	if (i > 15)
+      	  return "clr%.w %0";
+        if (i > 8)
+        {
+          operands[2] = GEN_INT (i - 8); 
+      	  return "lsl%.w #8,%0\;lsl%.w %2,%0";
+        }
+      }
+    return "lsl%.w %2,%0";
+  })
 
 (define_insn ""
   [(set (strict_low_part (match_operand:HI 0 "register_operand" "+d"))
 	(ashift:HI (match_dup 0)
-		   (match_operand:HI 1 "general_operand" "dI")))]
+		   (match_operand:SI 1 "general_operand" "dn")))]
   "!TARGET_COLDFIRE"
-  "lsl%.w %1,%0")
+  {
+    if (CONST_INT_P (operands[1]))
+      {
+        int i = INTVAL (operands[1]) & 63;
+     	if (i > 15)
+      	  return "clr%.w %0";
+        if (i > 8)
+        {
+          operands[2] = GEN_INT (i - 8); 
+      	  return "lsl%.w #8,%0\;lsl%.w %1,%0";
+        }
+      }
+    return "lsl%.w %1,%0";
+  })
 
 (define_insn "ashlqi3"
   [(set (match_operand:QI 0 "register_operand" "=d")
 	(ashift:QI (match_operand:QI 1 "register_operand" "0")
-		   (match_operand:QI 2 "general_operand" "dI")))]
+		   (match_operand:SI 2 "general_operand" "dn")))]
   "!TARGET_COLDFIRE"
-  "lsl%.b %2,%0")
+  {
+    if (CONST_INT_P (operands[2]) && (INTVAL (operands[2]) & 63) > 7)
+      return "clr%.b %0";
+    return "lsl%.b %2,%0";
+  })
 
 (define_insn ""
   [(set (strict_low_part (match_operand:QI 0 "register_operand" "+d"))
 	(ashift:QI (match_dup 0)
-		   (match_operand:QI 1 "general_operand" "dI")))]
+		   (match_operand:SI 1 "general_operand" "dn")))]
   "!TARGET_COLDFIRE"
-  "lsl%.b %1,%0")
+  {
+    if (CONST_INT_P (operands[1]) && (INTVAL (operands[1]) & 63) > 7)
+      return "clr%.b %0";
+    return "lsl%.b %1,%0";
+  })
 
 ;; On most 68k models, this makes faster code in a special case.
 
@@ -4652,6 +4890,19 @@
   return "swap %0\;asr%.w %2,%0\;ext%.l %0";
 })
 
+(define_insn "ashrsi_9_16"
+  [(set (match_operand:SI 0 "register_operand" "=d")
+	(ashiftrt:SI (match_operand:SI 1 "register_operand" "0")
+		     (match_operand:SI 2 "const_int_operand" "n")))]
+  "INTVAL (operands[2]) <= 16 && INTVAL (operands[2]) > 8"
+{
+  CC_STATUS_INIT;
+
+  operands[2] = GEN_INT (INTVAL (operands[2]) - 8);
+  return "asr.l #8,%0\;asr.l %2,%0";
+})
+
+
 (define_insn "subreghi1ashrdi_const32"
   [(set (match_operand:HI 0 "nonimmediate_operand" "=rm")
     (subreg:HI (ashiftrt:DI (match_operand:DI 1 "general_operand" "ro")
@@ -4848,30 +5099,63 @@
 (define_insn "ashrhi3"
   [(set (match_operand:HI 0 "register_operand" "=d")
 	(ashiftrt:HI (match_operand:HI 1 "register_operand" "0")
-		     (match_operand:HI 2 "general_operand" "dI")))]
+		     (match_operand:SI 2 "general_operand" "dn")))]
   "!TARGET_COLDFIRE"
-  "asr%.w %2,%0")
+  {
+    if (CONST_INT_P (operands[2]))
+      {
+        int i = INTVAL (operands[2]) & 63;
+     	if (i > 15)
+      	  return "asr%.w #8,%0\;asr%.w #8,%0";
+        if (i > 8)
+        {
+          operands[2] = GEN_INT (i - 8); 
+      	  return "asr%.w #8,%0\;asr%.w %2,%0";
+        }
+      }
+    return "asr%.w %2,%0";
+  })
 
 (define_insn ""
   [(set (strict_low_part (match_operand:HI 0 "register_operand" "+d"))
 	(ashiftrt:HI (match_dup 0)
-		     (match_operand:HI 1 "general_operand" "dI")))]
+		     (match_operand:SI 1 "general_operand" "dn")))]
   "!TARGET_COLDFIRE"
-  "asr%.w %1,%0")
+  {
+    if (CONST_INT_P (operands[1]))
+      {
+        int i = INTVAL (operands[1]) & 63;
+     	if (i > 15)
+      	  return "asr%.w #8,%0\;asr%.w #8,%0";
+        if (i > 8)
+        {
+          operands[2] = GEN_INT (i - 8); 
+      	  return "asr%.w #8,%0\;asr%.w %1,%0";
+        }
+      }
+    return "asr%.w %1,%0";
+  })
 
 (define_insn "ashrqi3"
   [(set (match_operand:QI 0 "register_operand" "=d")
 	(ashiftrt:QI (match_operand:QI 1 "register_operand" "0")
-		     (match_operand:QI 2 "general_operand" "dI")))]
+		     (match_operand:SI 2 "general_operand" "dn")))]
   "!TARGET_COLDFIRE"
-  "asr%.b %2,%0")
-
+  {
+    if (CONST_INT_P (operands[2]) && (INTVAL (operands[2]) & 63) > 7)
+      return "asr%.b #8,%0";
+    return "asr%.b %2,%0";
+  })
 (define_insn ""
   [(set (strict_low_part (match_operand:QI 0 "register_operand" "+d"))
 	(ashiftrt:QI (match_dup 0)
-		     (match_operand:QI 1 "general_operand" "dI")))]
+		     (match_operand:SI 1 "general_operand" "dn")))]
   "!TARGET_COLDFIRE"
-  "asr%.b %1,%0")
+  {
+    if (CONST_INT_P (operands[1]) && (INTVAL (operands[1]) & 63) > 7)
+      return "asr%.b #8,%0";
+    return "asr%.b %1,%0";
+  })
 
 ;; logical shift instructions
 
@@ -5144,30 +5428,64 @@
 (define_insn "lshrhi3"
   [(set (match_operand:HI 0 "register_operand" "=d")
 	(lshiftrt:HI (match_operand:HI 1 "register_operand" "0")
-		     (match_operand:HI 2 "general_operand" "dI")))]
+		     (match_operand:SI 2 "general_operand" "dn")))]
   "!TARGET_COLDFIRE"
-  "lsr%.w %2,%0")
+  {
+    if (CONST_INT_P (operands[2]))
+      {
+        int i = INTVAL (operands[2]) & 63;
+     	if (i > 15)
+      	  return "clr%.w %0";
+        if (i > 8)
+        {
+          operands[2] = GEN_INT (i - 8); 
+      	  return "lsr%.w #8,%0\;lsr%.w %2,%0";
+        }
+      }
+    return "lsr%.w %2,%0";
+  })
 
 (define_insn ""
   [(set (strict_low_part (match_operand:HI 0 "register_operand" "+d"))
 	(lshiftrt:HI (match_dup 0)
-		     (match_operand:HI 1 "general_operand" "dI")))]
+		     (match_operand:SI 1 "general_operand" "dn")))]
   "!TARGET_COLDFIRE"
-  "lsr%.w %1,%0")
+  {
+    if (CONST_INT_P (operands[1]))
+      {
+        int i = INTVAL (operands[1]) & 63;
+     	if (i > 15)
+      	  return "clr%.w %0";
+        if (i > 8)
+        {
+          operands[2] = GEN_INT (i - 8); 
+      	  return "lsr%.w #8,%0\;lsr%.w %1,%0";
+        }
+      }
+    return "lsr%.w %1,%0";
+  })
 
 (define_insn "lshrqi3"
   [(set (match_operand:QI 0 "register_operand" "=d")
 	(lshiftrt:QI (match_operand:QI 1 "register_operand" "0")
-		     (match_operand:QI 2 "general_operand" "dI")))]
+		     (match_operand:SI 2 "general_operand" "dn")))]
   "!TARGET_COLDFIRE"
-  "lsr%.b %2,%0")
+  {
+    if (CONST_INT_P (operands[2]) && (INTVAL (operands[2]) & 63) > 7)
+      return "clr%.b %0";
+    return "lsr%.b %2,%0";
+  })
 
 (define_insn ""
   [(set (strict_low_part (match_operand:QI 0 "register_operand" "+d"))
 	(lshiftrt:QI (match_dup 0)
-		     (match_operand:QI 1 "general_operand" "dI")))]
+		     (match_operand:SI 1 "general_operand" "dn")))]
   "!TARGET_COLDFIRE"
-  "lsr%.b %1,%0")
+  {
+    if (CONST_INT_P (operands[1]) && (INTVAL (operands[1]) & 63) > 7)
+      return "clr%.b %0";
+    return "lsr%.b %1,%0";
+  })
 
 ;; rotate instructions
 
@@ -5505,7 +5823,23 @@
 
 (define_insn "*extv_bfexts_mem"
   [(set (match_operand:SI 0 "register_operand" "=d")
-	(sign_extract:SI (match_operand:QI 1 "memory_operand" "o")
+	(sign_extract:SI (match_operand:SI 1 "memory_operand" "oQU")
+			 (match_operand:SI 2 "nonmemory_operand" "dn")
+			 (match_operand:SI 3 "nonmemory_operand" "dn")))]
+  "TARGET_68020 && TARGET_BITFIELD"
+  "bfexts %1{%b3:%b2},%0")
+
+(define_insn "*extv_bfexts_mem1"
+  [(set (match_operand:SI 0 "register_operand" "=d")
+	(sign_extract:SI (match_operand:HI 1 "memory_operand" "oQU")
+			 (match_operand:SI 2 "nonmemory_operand" "dn")
+			 (match_operand:SI 3 "nonmemory_operand" "dn")))]
+  "TARGET_68020 && TARGET_BITFIELD"
+  "bfexts %1{%b3:%b2},%0")
+
+(define_insn "*extv_bfexts_mem2"
+  [(set (match_operand:SI 0 "register_operand" "=d")
+	(sign_extract:SI (match_operand:QI 1 "memory_operand" "oQU")
 			 (match_operand:SI 2 "nonmemory_operand" "dn")
 			 (match_operand:SI 3 "nonmemory_operand" "dn")))]
   "TARGET_68020 && TARGET_BITFIELD"
@@ -5521,7 +5855,45 @@
 
 (define_insn "*extzv_bfextu_mem"
   [(set (match_operand:SI 0 "register_operand" "=d")
-	(zero_extract:SI (match_operand:QI 1 "memory_operand" "o")
+	(zero_extract:SI (match_operand:SI 1 "memory_operand" "oQU")
+			 (match_operand:SI 2 "nonmemory_operand" "dn")
+			 (match_operand:SI 3 "nonmemory_operand" "dn")))]
+  "TARGET_68020 && TARGET_BITFIELD"
+{
+  if (GET_CODE (operands[2]) == CONST_INT)
+    {
+      if (INTVAL (operands[2]) != 32)
+	cc_status.flags |= CC_NOT_NEGATIVE;
+    }
+  else
+    {
+      CC_STATUS_INIT;
+    }
+  return "bfextu %1{%b3:%b2},%0";
+})
+
+(define_insn "*extzv_bfextu_mem1"
+  [(set (match_operand:SI 0 "register_operand" "=d")
+	(zero_extract:SI (match_operand:HI 1 "memory_operand" "oQU")
+			 (match_operand:SI 2 "nonmemory_operand" "dn")
+			 (match_operand:SI 3 "nonmemory_operand" "dn")))]
+  "TARGET_68020 && TARGET_BITFIELD"
+{
+  if (GET_CODE (operands[2]) == CONST_INT)
+    {
+      if (INTVAL (operands[2]) != 32)
+	cc_status.flags |= CC_NOT_NEGATIVE;
+    }
+  else
+    {
+      CC_STATUS_INIT;
+    }
+  return "bfextu %1{%b3:%b2},%0";
+})
+
+(define_insn "*extzv_bfextu_mem2"
+  [(set (match_operand:SI 0 "register_operand" "=d")
+	(zero_extract:SI (match_operand:QI 1 "memory_operand" "oQU")
 			 (match_operand:SI 2 "nonmemory_operand" "dn")
 			 (match_operand:SI 3 "nonmemory_operand" "dn")))]
   "TARGET_68020 && TARGET_BITFIELD"
@@ -5539,7 +5911,37 @@
 })
 
 (define_insn "*insv_bfchg_mem"
-  [(set (zero_extract:SI (match_operand:QI 0 "memory_operand" "+o")
+  [(set (zero_extract:SI (match_operand:SI 0 "memory_operand" "+oQU")
+			 (match_operand:SI 1 "nonmemory_operand" "dn")
+			 (match_operand:SI 2 "nonmemory_operand" "dn"))
+        (xor:SI (zero_extract:SI (match_dup 0) (match_dup 1) (match_dup 2))
+		(match_operand 3 "const_int_operand" "n")))]
+  "TARGET_68020 && TARGET_BITFIELD
+   && (INTVAL (operands[3]) == -1
+       || (GET_CODE (operands[1]) == CONST_INT
+           && (~ INTVAL (operands[3]) & ((1 << INTVAL (operands[1]))- 1)) == 0))"
+{
+  CC_STATUS_INIT;
+  return "bfchg %0{%b2:%b1}";
+})
+
+(define_insn "*insv_bfchg_mem1"
+  [(set (zero_extract:SI (match_operand:HI 0 "memory_operand" "+oQU")
+			 (match_operand:SI 1 "nonmemory_operand" "dn")
+			 (match_operand:SI 2 "nonmemory_operand" "dn"))
+        (xor:SI (zero_extract:SI (match_dup 0) (match_dup 1) (match_dup 2))
+		(match_operand 3 "const_int_operand" "n")))]
+  "TARGET_68020 && TARGET_BITFIELD
+   && (INTVAL (operands[3]) == -1
+       || (GET_CODE (operands[1]) == CONST_INT
+           && (~ INTVAL (operands[3]) & ((1 << INTVAL (operands[1]))- 1)) == 0))"
+{
+  CC_STATUS_INIT;
+  return "bfchg %0{%b2:%b1}";
+})
+
+(define_insn "*insv_bfchg_mem2"
+  [(set (zero_extract:SI (match_operand:QI 0 "memory_operand" "+oQU")
 			 (match_operand:SI 1 "nonmemory_operand" "dn")
 			 (match_operand:SI 2 "nonmemory_operand" "dn"))
         (xor:SI (zero_extract:SI (match_dup 0) (match_dup 1) (match_dup 2))
@@ -5554,7 +5956,29 @@
 })
 
 (define_insn "*insv_bfclr_mem"
-  [(set (zero_extract:SI (match_operand:QI 0 "memory_operand" "+o")
+  [(set (zero_extract:SI (match_operand:SI 0 "memory_operand" "+oQU")
+			 (match_operand:SI 1 "nonmemory_operand" "dn")
+			 (match_operand:SI 2 "nonmemory_operand" "dn"))
+	(const_int 0))]
+  "TARGET_68020 && TARGET_BITFIELD"
+{
+  CC_STATUS_INIT;
+  return "bfclr %0{%b2:%b1}";
+})
+
+(define_insn "*insv_bfclr_mem1"
+  [(set (zero_extract:SI (match_operand:HI 0 "memory_operand" "+oQU")
+			 (match_operand:SI 1 "nonmemory_operand" "dn")
+			 (match_operand:SI 2 "nonmemory_operand" "dn"))
+	(const_int 0))]
+  "TARGET_68020 && TARGET_BITFIELD"
+{
+  CC_STATUS_INIT;
+  return "bfclr %0{%b2:%b1}";
+})
+
+(define_insn "*insv_bfclr_mem2"
+  [(set (zero_extract:SI (match_operand:QI 0 "memory_operand" "+oQU")
 			 (match_operand:SI 1 "nonmemory_operand" "dn")
 			 (match_operand:SI 2 "nonmemory_operand" "dn"))
 	(const_int 0))]
@@ -5565,7 +5989,29 @@
 })
 
 (define_insn "*insv_bfset_mem"
-  [(set (zero_extract:SI (match_operand:QI 0 "memory_operand" "+o")
+  [(set (zero_extract:SI (match_operand:SI 0 "memory_operand" "+oQU")
+			 (match_operand:SI 1 "general_operand" "dn")
+			 (match_operand:SI 2 "general_operand" "dn"))
+	(const_int -1))]
+  "TARGET_68020 && TARGET_BITFIELD"
+{
+  CC_STATUS_INIT;
+  return "bfset %0{%b2:%b1}";
+})
+
+(define_insn "*insv_bfset_mem1"
+  [(set (zero_extract:SI (match_operand:HI 0 "memory_operand" "+oQU")
+			 (match_operand:SI 1 "general_operand" "dn")
+			 (match_operand:SI 2 "general_operand" "dn"))
+	(const_int -1))]
+  "TARGET_68020 && TARGET_BITFIELD"
+{
+  CC_STATUS_INIT;
+  return "bfset %0{%b2:%b1}";
+})
+
+(define_insn "*insv_bfset_mem2"
+  [(set (zero_extract:SI (match_operand:QI 0 "memory_operand" "+oQU")
 			 (match_operand:SI 1 "general_operand" "dn")
 			 (match_operand:SI 2 "general_operand" "dn"))
 	(const_int -1))]
@@ -5595,6 +6041,22 @@
 }")
 
 (define_insn "*insv_bfins_mem"
+  [(set (zero_extract:SI (match_operand:SI 0 "memory_operand" "+o")
+			 (match_operand:SI 1 "nonmemory_operand" "dn")
+			 (match_operand:SI 2 "nonmemory_operand" "dn"))
+	(match_operand:SI 3 "register_operand" "d"))]
+  "TARGET_68020 && TARGET_BITFIELD"
+  "bfins %3,%0{%b2:%b1}")
+
+(define_insn "*insv_bfins_mem1"
+  [(set (zero_extract:SI (match_operand:HI 0 "memory_operand" "+o")
+			 (match_operand:SI 1 "nonmemory_operand" "dn")
+			 (match_operand:SI 2 "nonmemory_operand" "dn"))
+	(match_operand:SI 3 "register_operand" "d"))]
+  "TARGET_68020 && TARGET_BITFIELD"
+  "bfins %3,%0{%b2:%b1}")
+
+(define_insn "*insv_bfins_mem2"
   [(set (zero_extract:SI (match_operand:QI 0 "memory_operand" "+o")
 			 (match_operand:SI 1 "nonmemory_operand" "dn")
 			 (match_operand:SI 2 "nonmemory_operand" "dn"))
@@ -5677,7 +6139,55 @@
 
 (define_insn "*tst_bftst_mem"
   [(set (cc0)
-	(compare (zero_extract:SI (match_operand:QI 0 "memory_operand" "o")
+	(compare (zero_extract:SI (match_operand:SI 0 "memory_operand" "oQU")
+				  (match_operand:SI 1 "const_int_operand" "n")
+				  (match_operand:SI 2 "general_operand" "dn"))
+	         (const_int 0)))]
+  "TARGET_68020 && TARGET_BITFIELD"
+{
+  if (operands[1] == const1_rtx
+      && GET_CODE (operands[2]) == CONST_INT)
+    {
+      int width = GET_CODE (operands[0]) == REG ? 31 : 7;
+      return output_btst (operands,
+			  GEN_INT (width - INTVAL (operands[2])),
+			  operands[0], insn, 1000);
+      /* Pass 1000 as SIGNPOS argument so that btst will
+         not think we are testing the sign bit for an `and'
+	 and assume that nonzero implies a negative result.  */
+    }
+  if (INTVAL (operands[1]) != 32)
+    cc_status.flags = CC_NOT_NEGATIVE;
+  return "bftst %0{%b2:%b1}";
+})
+
+(define_insn "*tst_bftst_mem1"
+  [(set (cc0)
+	(compare (zero_extract:SI (match_operand:HI 0 "memory_operand" "oQU")
+				  (match_operand:SI 1 "const_int_operand" "n")
+				  (match_operand:SI 2 "general_operand" "dn"))
+	         (const_int 0)))]
+  "TARGET_68020 && TARGET_BITFIELD"
+{
+  if (operands[1] == const1_rtx
+      && GET_CODE (operands[2]) == CONST_INT)
+    {
+      int width = GET_CODE (operands[0]) == REG ? 31 : 7;
+      return output_btst (operands,
+			  GEN_INT (width - INTVAL (operands[2])),
+			  operands[0], insn, 1000);
+      /* Pass 1000 as SIGNPOS argument so that btst will
+         not think we are testing the sign bit for an `and'
+	 and assume that nonzero implies a negative result.  */
+    }
+  if (INTVAL (operands[1]) != 32)
+    cc_status.flags = CC_NOT_NEGATIVE;
+  return "bftst %0{%b2:%b1}";
+})
+
+(define_insn "*tst_bftst_mem2"
+  [(set (cc0)
+	(compare (zero_extract:SI (match_operand:QI 0 "memory_operand" "oQU")
 				  (match_operand:SI 1 "const_int_operand" "n")
 				  (match_operand:SI 2 "general_operand" "dn"))
 	         (const_int 0)))]
@@ -6690,7 +7200,7 @@
 })
 
 ;; Decrement-and-branch insns.
-(define_insn "*dbne_hi"
+(define_insn "dbne_hi"
   [(set (pc)
 	(if_then_else
 	 (ne (match_operand:HI 0 "nonimmediate_operand" "+d*g")
@@ -6710,7 +7220,7 @@
   return "subq%.w #1,%0\;cmp%.w #-1,%0\;jne %l1";
 })
 
-(define_insn "*dbne_si"
+(define_insn "dbne_si"
   [(set (pc)
 	(if_then_else
 	 (ne (match_operand:SI 0 "nonimmediate_operand" "+d*g")
@@ -6724,7 +7234,13 @@
 {
   CC_STATUS_INIT;
   if (DATA_REG_P (operands[0]))
+	{
+    if (TUNE_68080)
+            return "dbral %0,%l1";                 // APOLLO 68080 DBRAL
+    if (TUNE_68060)
+            return "subq%.l #1,%0;jcc %l1";
     return "dbra %0,%l1\;clr%.w %0\;subq%.l #1,%0\;jcc %l1";
+	}
   if (GET_CODE (operands[0]) == MEM)
     return "subq%.l #1,%0\;jcc %l1";
   return "subq%.l #1,%0\;cmp%.l #-1,%0\;jne %l1";
@@ -6782,10 +7298,14 @@
 {
   CC_STATUS_INIT;
   if (DATA_REG_P (operands[0]))
-    return "dbra %0,%l1\;clr%.w %0\;subq%.l #1,%0\;jcc %l1";
+	{
+	  if (TUNE_68060_80)
+		  return "subq%.l #1,%0\;jcc %l1";
+      return "dbra %0,%l1\;clr%.w %0\;subq%.l #1,%0\;jcc %l1";
+	}
   if (GET_CODE (operands[0]) == MEM)
     return "subq%.l #1,%0\;jcc %l1";
-  return "subq%.l #1,%0\;cmp%.l #-1,%0\;jne %l1";
+  return "subq%.l #1,%0\;cmp%.w #-1,%0\;jne %l1";
 })
 
 (define_expand "sibcall"
@@ -6833,9 +7353,9 @@
   operands[0] = m68k_legitimize_call_address (operands[0]);
 })
 
-(define_insn "*call"
-  [(call (mem:QI (match_operand:SI 0 "call_operand" "a,W"))
-	 (match_operand:SI 1 "general_operand" "g,g"))]
+(define_insn "acall"
+  [(call (mem:QI (match_operand:SI 0 "call_operand" "a,W,Z"))
+	 (match_operand:SI 1 "general_operand" "g,g,g"))]
   ;; Operand 1 not really used on the m68000.
   "!SIBLING_CALL_P (insn)"
 {
@@ -6843,6 +7363,16 @@
 }
   [(set_attr "type" "jsr")])
 
+(define_insn "amiga_call"
+  [(set (match_operand 0 "register_operand" "=d")
+      (call (mem:QI (plus:SI (match_operand:SI 1 "register_operand" "a")
+                             (match_operand:SI 2 "general_operand" "J")))
+	        (match_operand:SI 3 "general_operand" "")))]
+  ;; Operand 3 not really used on the m68000.
+  "!reload_completed || REGNO(operands[0]) == 0"
+  "jsr (%n2,%1)"
+  [(set_attr "type" "jsr")])
+
 ;; Call subroutine, returning value in operand 0
 ;; (which must be a hard register).
 (define_expand "call_value"
@@ -7132,7 +7662,7 @@
   [(set (reg:SI SP_REG) (plus:SI (reg:SI SP_REG) (const_int 4)))
    (set (match_operand:DF 0 "register_operand" "")
 	(match_operand:DF 1 "register_operand" ""))]
-  "FP_REG_P (operands[0]) && !FP_REG_P (operands[1])"
+  "reload_completed && FP_REG_P (operands[0]) && !FP_REG_P (operands[1])"
   [(set (mem:SI (reg:SI SP_REG)) (match_dup 1))
    (set (mem:SI (pre_dec:SI (reg:SI SP_REG))) (match_dup 2))
    (set (match_dup 0) (mem:DF (post_inc:SI (reg:SI SP_REG))))]
@@ -7146,7 +7676,7 @@
   [(set (reg:SI SP_REG) (plus:SI (reg:SI SP_REG) (const_int 4)))
    (set (match_operand:SF 0 "push_operand" "")
 	(match_operand:SF 1 "general_operand" ""))]
-  "!reg_mentioned_p (stack_pointer_rtx, operands[0])"
+  "reload_completed && !reg_mentioned_p (stack_pointer_rtx, operands[0])"
   [(set (match_dup 0) (match_dup 1))]
   "operands[0] = replace_equiv_address (operands[0], stack_pointer_rtx);")
 
@@ -7155,7 +7685,7 @@
 				 (match_operand:SI 0 "const_int_operand" "")))
    (set (match_operand:SF 1 "push_operand" "")
 	(match_operand:SF 2 "general_operand" ""))]
-  "INTVAL (operands[0]) > 4
+  "reload_completed && INTVAL (operands[0]) > 4
    && !reg_mentioned_p (stack_pointer_rtx, operands[2])"
   [(set (reg:SI SP_REG) (plus:SI (reg:SI SP_REG) (match_dup 0)))
    (set (match_dup 1) (match_dup 2))]
@@ -7173,7 +7703,7 @@
   [(set (reg:SI SP_REG) (plus:SI (reg:SI SP_REG) (const_int 4)))
    (set (match_operand:SI 0 "push_operand" "")
 	(match_operand:SI 1 "general_operand" ""))]
-  "!reg_mentioned_p (stack_pointer_rtx, operands[1])"
+  "reload_completed && !reg_mentioned_p (stack_pointer_rtx, operands[1])"
   [(set (match_dup 0) (match_dup 1))]
   "operands[0] = replace_equiv_address (operands[0], stack_pointer_rtx);")
 
@@ -7182,7 +7712,7 @@
   [(match_scratch:SI 2 "d")
    (set (match_operand:SI 0 "memory_operand" "")
 	(match_operand:SI 1 "const_int_operand" ""))]
-  "GET_CODE (XEXP (operands[0], 0)) != PRE_DEC
+  "reload_completed && !TUNE_68040_80 && GET_CODE (XEXP (operands[0], 0)) != PRE_DEC
    && INTVAL (operands[1]) != 0
    && IN_RANGE (INTVAL (operands[1]), -0x80, 0x7f)
    && !valid_mov3q_const (INTVAL (operands[1]))"
@@ -7195,7 +7725,7 @@
    (set (reg:SI SP_REG) (plus:SI (reg:SI SP_REG) (const_int 12)))
    (set (match_operand:SI 0 "push_operand" "")
 	(match_operand:SI 1 "const_int_operand" ""))]
-  "INTVAL (operands[1]) != 0
+  "reload_completed && INTVAL (operands[1]) != 0
    && IN_RANGE (INTVAL (operands[1]), -0x80, 0x7f)
    && !valid_mov3q_const (INTVAL (operands[1]))"
   [(set (reg:SI SP_REG) (plus:SI (reg:SI SP_REG) (const_int 8)))
@@ -7204,16 +7734,18 @@
   "operands[0] = replace_equiv_address (operands[0], stack_pointer_rtx);")
 
 ;; Changing pea X.w into a move.l is no real win here.
+;; SBF: also disable converting pea for baserel insns!
 (define_peephole2
   [(set (reg:SI SP_REG) (plus:SI (reg:SI SP_REG)
 				 (match_operand:SI 0 "const_int_operand" "")))
    (set (match_operand:SI 1 "push_operand" "")
 	(match_operand:SI 2 "general_operand" ""))]
-  "INTVAL (operands[0]) > 4
+  "reload_completed && INTVAL (operands[0]) > 4
    && !reg_mentioned_p (stack_pointer_rtx, operands[2])
    && !(CONST_INT_P (operands[2]) && INTVAL (operands[2]) != 0
 	&& IN_RANGE (INTVAL (operands[2]), -0x8000, 0x7fff)
-	&& !valid_mov3q_const (INTVAL (operands[2])))"
+	&& !valid_mov3q_const (INTVAL (operands[2])))
+   && !amiga_is_const_pic_ref(operands[2])"
   [(set (reg:SI SP_REG) (plus:SI (reg:SI SP_REG) (match_dup 0)))
    (set (match_dup 1) (match_dup 2))]
 {
@@ -7228,7 +7760,7 @@
   [(set (reg:SI SP_REG) (plus:SI (reg:SI SP_REG) (const_int -4)))
    (set (match_operand:QI 0 "memory_operand" "")
 	(match_operand:QI 1 "register_operand" ""))]
-  "!reg_mentioned_p (stack_pointer_rtx, operands[1])
+  "reload_completed && !reg_mentioned_p (stack_pointer_rtx, operands[1])
    && GET_CODE (XEXP (operands[0], 0)) == PLUS
    && rtx_equal_p (XEXP (XEXP (operands[0], 0), 0), stack_pointer_rtx)
    && CONST_INT_P (XEXP (XEXP (operands[0], 0), 1))
@@ -7244,7 +7776,7 @@
   [(set (match_operand:QI 0 "push_operand" "")
 	(match_operand:QI 1 "register_operand" ""))
    (set (reg:SI SP_REG) (plus:SI (reg:SI SP_REG) (const_int -3)))]
-  "!reg_mentioned_p (stack_pointer_rtx, operands[1])"
+  "reload_completed && !reg_mentioned_p (stack_pointer_rtx, operands[1])"
   [(set (match_dup 0) (match_dup 1))]
 {
   operands[0] = adjust_automodify_address (operands[0], SImode,
@@ -7256,7 +7788,7 @@
   [(set (match_operand:HI 0 "push_operand" "")
 	(match_operand:HI 1 "register_operand" ""))
    (set (reg:SI SP_REG) (plus:SI (reg:SI SP_REG) (const_int -2)))]
-  "!reg_mentioned_p (stack_pointer_rtx, operands[1])"
+  "reload_completed && !reg_mentioned_p (stack_pointer_rtx, operands[1])"
   [(set (match_dup 0) (match_dup 1))]
 {
   operands[0] = adjust_automodify_address (operands[0], SImode,
@@ -7271,7 +7803,7 @@
 	(const_int 0))
    (set (strict_low_part (match_operand:HI 1 "register_operand" ""))
 	(match_operand:HI 2 "general_operand" ""))]
-  "REGNO (operands[0]) == REGNO (operands[1])
+  "reload_completed && REGNO (operands[0]) == REGNO (operands[1])
    && strict_low_part_peephole_ok (HImode, insn, operands[0])"
   [(set (strict_low_part (match_dup 1)) (match_dup 2))]
   "")
@@ -7281,7 +7813,7 @@
 	(const_int 0))
    (set (strict_low_part (match_operand:QI 1 "register_operand" ""))
 	(match_operand:QI 2 "general_operand" ""))]
-  "REGNO (operands[0]) == REGNO (operands[1])
+  "reload_completed && REGNO (operands[0]) == REGNO (operands[1])
    && strict_low_part_peephole_ok (QImode, insn, operands[0])"
   [(set (strict_low_part (match_dup 1)) (match_dup 2))]
   "")
@@ -7318,7 +7850,7 @@
      (set (match_dup 0)
 	  (plus:HI (match_dup 0)
 		   (const_int -1)))])]
-  "!TARGET_COLDFIRE && DATA_REG_P (operands[0]) && ! flags_in_68881 ()"
+  "reload_completed && !TARGET_COLDFIRE && DATA_REG_P (operands[0]) && ! flags_in_68881 ()"
 {
   CC_STATUS_INIT;
   output_dbcc_and_branch (operands);
@@ -7340,7 +7872,7 @@
      (set (match_dup 0)
 	  (plus:SI (match_dup 0)
 		   (const_int -1)))])]
-  "!TARGET_COLDFIRE && DATA_REG_P (operands[0]) && ! flags_in_68881 ()"
+  "reload_completed && !TARGET_COLDFIRE && DATA_REG_P (operands[0]) && ! flags_in_68881 ()"
 {
   CC_STATUS_INIT;
   output_dbcc_and_branch (operands);
@@ -7363,7 +7895,7 @@
      (set (match_dup 0)
 	  (plus:HI (match_dup 0)
 		   (const_int -1)))])]
-  "!TARGET_COLDFIRE && DATA_REG_P (operands[0]) && ! flags_in_68881 ()"
+  "reload_completed && !TARGET_COLDFIRE && DATA_REG_P (operands[0]) && ! flags_in_68881 ()"
 {
   CC_STATUS_INIT;
   output_dbcc_and_branch (operands);
@@ -7386,7 +7918,7 @@
      (set (match_dup 0)
 	  (plus:SI (match_dup 0)
 		   (const_int -1)))])]
-  "!TARGET_COLDFIRE && DATA_REG_P (operands[0]) && ! flags_in_68881 ()"
+  "reload_completed && !TARGET_COLDFIRE && DATA_REG_P (operands[0]) && ! flags_in_68881 ()"
 {
   CC_STATUS_INIT;
   output_dbcc_and_branch (operands);
@@ -7482,30 +8014,6 @@
   "TARGET_68881"
   "fmove%.s %f1,%0")
 
-(define_insn "sin<mode>2"
-  [(set (match_operand:FP 0 "nonimmediate_operand" "=f")
-	(unspec:FP
-	  [(match_operand:FP 1 "general_operand" "f<FP:dreg>m")] UNSPEC_SIN))]
-  "TARGET_68881 && flag_unsafe_math_optimizations"
-{
-  if (FP_REG_P (operands[1]))
-    return "fsin%.x %1,%0";
-  else
-    return "fsin%.<FP:prec> %1,%0";
-})
-
-(define_insn "cos<mode>2"
-  [(set (match_operand:FP 0 "nonimmediate_operand" "=f")
-	(unspec:FP
-	  [(match_operand:FP 1 "general_operand" "f<FP:dreg>m")] UNSPEC_COS))]
-  "TARGET_68881 && flag_unsafe_math_optimizations"
-{
-  if (FP_REG_P (operands[1]))
-    return "fcos%.x %1,%0";
-  else
-    return "fcos%.<FP:prec> %1,%0";
-})
-
 ;; Unconditional traps are assumed to have (const_int 1) for the condition.
 (define_insn "trap"
   [(trap_if (const_int 1) (const_int 7))]
@@ -7599,6 +8107,7 @@
   "#"
   [(set_attr "type" "ib")])
 
+(include "m68080.md")
 (include "cf.md")
 (include "sync.md")
 
@@ -7621,7 +8130,7 @@
    (set (mem:QI (plus:SI (match_operand:SI 3 "register_operand" "")
 			 (match_operand:SI 4 "register_operand" "")))
 	(const_int 0))]
-  "(optimize_size || !TUNE_68060)
+  "reload_completed && (optimize_size || !TUNE_68060)
    && (operands[0] == operands[3] || operands[0] == operands[4])
    && ADDRESS_REG_P (operands[1])
    && ADDRESS_REG_P ((operands[0] == operands[3]) ? operands[4] : operands[3])
@@ -7643,12 +8152,12 @@
    (set (pc) (if_then_else (gtu (cc0) (const_int 0))
 			   (match_operand 4 "pc_or_label_operand")
 			   (match_operand 5 "pc_or_label_operand")))]
-  "INTVAL (operands[1]) <= 255
+  "reload_completed && INTVAL (operands[1]) <= 255
    && operands[0] == operands[3]
    && peep2_reg_dead_p (2, operands[0])
    && peep2_reg_dead_p (2, operands[2])
    && (operands[4] == pc_rtx || operands[5] == pc_rtx)
-   && (optimize_size || TUNE_68040_60)
+   && (optimize_size || TUNE_68040_80)
    && DATA_REG_P (operands[2])"
   [(set (match_dup 7) (lshiftrt:SI (match_dup 7) (match_dup 6)))
    (set (cc0) (compare (match_dup 7) (const_int 0)))
@@ -7666,10 +8175,10 @@
    (set (pc) (if_then_else (gtu (cc0) (const_int 0))
 			   (match_operand 2 "pc_or_label_operand")
 			   (match_operand 3 "pc_or_label_operand")))]
-  "INTVAL (operands[1]) <= 255
+  "reload_completed && INTVAL (operands[1]) <= 255
    && peep2_reg_dead_p (1, operands[0])
    && (operands[2] == pc_rtx || operands[3] == pc_rtx)
-   && (optimize_size || TUNE_68040_60)
+   && (optimize_size || TUNE_68040_80)
    && DATA_REG_P (operands[0])"
   [(set (match_dup 0) (lshiftrt:SI (match_dup 0) (match_dup 4)))
    (set (cc0) (compare (match_dup 0) (const_int 0)))
@@ -7685,12 +8194,12 @@
    (set (pc) (if_then_else (leu (cc0) (const_int 0))
 			   (match_operand 4 "pc_or_label_operand")
 			   (match_operand 5 "pc_or_label_operand")))]
-  "INTVAL (operands[1]) <= 255
+  "reload_completed && INTVAL (operands[1]) <= 255
    && operands[0] == operands[3]
    && peep2_reg_dead_p (2, operands[0])
    && peep2_reg_dead_p (2, operands[2])
    && (operands[4] == pc_rtx || operands[5] == pc_rtx)
-   && (optimize_size || TUNE_68040_60)
+   && (optimize_size || TUNE_68040_80)
    && DATA_REG_P (operands[2])"
   [(set (match_dup 7) (lshiftrt:SI (match_dup 7) (match_dup 6)))
    (set (cc0) (compare (match_dup 7) (const_int 0)))
@@ -7708,10 +8217,10 @@
    (set (pc) (if_then_else (leu (cc0) (const_int 0))
 			   (match_operand 2 "pc_or_label_operand")
 			   (match_operand 3 "pc_or_label_operand")))]
-  "INTVAL (operands[1]) <= 255
+  "reload_completed && INTVAL (operands[1]) <= 255
    &&  peep2_reg_dead_p (1, operands[0])
    && (operands[2] == pc_rtx || operands[3] == pc_rtx)
-   && (optimize_size || TUNE_68040_60)
+   && (optimize_size || TUNE_68040_80)
    && DATA_REG_P (operands[0])"
   [(set (match_dup 0) (lshiftrt:SI (match_dup 0) (match_dup 4)))
    (set (cc0) (compare (match_dup 0) (const_int 0)))
@@ -7719,3 +8228,1133 @@
 			   (match_dup 2) (match_dup 3)))]
   "{ operands[4] = GEN_INT (exact_log2 (INTVAL (operands[1]) + 1)); }")
 
+;; use double indirect jumps
+;; e.g. combined
+;; (set (reg/f:SI a1)
+;;      (mem/f:SI (plus:SI (mult:SI (reg:SI d0)
+;;                                  (const_int 4 ))
+;;                         (symbol_ref:SI ("jump_table.1194") ))
+;;	))
+;; (set (pc) (reg/f:SI a1 )) 
+;;
+;; to
+;;
+;; (set (pc) (mem/f:SI (plus:SI (mult:SI (reg:SI d0)
+;;                                  (const_int 4 ))
+;;			))
+;;  ))
+(define_peephole2
+  [(set (match_operand:SI 0 "register_operand")
+        (match_operand:SI 1 "memory_operand"))
+   (set (pc)
+        (match_operand:SI 2 "register_operand"))]
+  "flag_double_indirect && TARGET_68020 && operands[0] == operands[2]"
+  [(set (pc) (match_dup 1))]
+  ""
+)
+
+;; and the insn for double indirect jumps  
+(define_insn "flag_double_indirect"
+  [(set (pc) (match_operand:SI 0 "memory_operand" "m"))]
+  "TARGET_68020 && flag_double_indirect"
+  "jmp %a0"
+  [(set_attr "type" "jmp")])
+
+;; commutate these insns if unrelated
+;; (set (reg/f:SI a0)
+;;      (reg/v:SI a6)) 
+;; (set (reg/v:SI a6)
+;;      (plus:SI (reg/f:SI a0)
+;;               (const_int 2 )))
+;; (set (reg:HI d0)
+;;      (mem:HI (reg/f:SI a0)))
+;; if a0 is a temp register, based on a6, a0 can be optimized away and auto inc for a6 is applied
+(define_peephole2
+  [(set (match_operand:SI 0 "register_operand")
+        (match_operand:SI 1 "register_operand"))
+   (set (match_operand:SI 2 "register_operand")
+        (plus:SI (match_operand:SI 3 "register_operand")
+                 (match_operand 4 "const_int_operand")))
+   (set (match_operand:HI 5 "register_operand")
+        (mem:HI (match_operand:SI 6 "register_operand")))]
+  "reload_completed && operands[1] == operands[2] && operands[0] == operands[3] &&
+   operands[0] != operands[5] && operands[1] != operands[5] &&
+   INTVAL(operands[4]) == 2 && operands[6] == operands[0]"
+  [(set (match_dup 0) (match_dup 1))
+   (set (match_dup 5) (mem:HI (post_inc:SI (match_dup 1))))]
+  ""
+)
+
+;; also work with
+;; (set (reg d0)
+;;      (mem (post_inc (reg/f:SI a0))))
+(define_peephole2
+  [(set (match_operand:SI 0 "register_operand")
+        (match_operand:SI 1 "register_operand"))
+   (set (match_operand:SI 2 "register_operand")
+        (plus:SI (match_operand:SI 3 "register_operand")
+                 (match_operand 4 "const_int_operand")))
+   (set (match_operand:HI 5 "register_operand")
+        (mem:HI (post_inc:SI (match_operand:SI 6 "register_operand"))))]
+  "reload_completed && operands[1] == operands[2] && operands[0] == operands[3] &&
+   operands[0] != operands[5] && operands[1] != operands[5] &&
+   INTVAL(operands[4]) == 2 && operands[6] == operands[0]"
+  [(set (match_dup 5) (mem:HI (post_inc:SI (match_dup 1))))
+   (set (match_dup 0) (match_dup 1))]
+  ""
+)
+
+;; convert
+;;        addq.l #1,a1
+;;        move.b d4,(-1,a1)
+;; into
+;;        move.b d4,(a1)+
+;;
+;; works with mode sizes 1, 2 and 4.
+;;
+(define_peephole2
+  [(set (match_operand:SI 0 "register_operand")
+        (plus:SI (match_dup 0) (match_operand 2 "const_int_operand")))
+   (set (mem:QI (plus:SI (match_dup 0) (match_operand 4 "const_int_operand")))
+        (match_operand:QI 5 "register_operand")
+   )]
+   "reload_completed && operands[0] != operands[5] && INTVAL(operands[2]) == 1 && INTVAL(operands[2]) == -INTVAL(operands[4])
+   "[(set (mem:QI (post_inc:SI (match_dup 0))) (match_dup 5))]
+)
+(define_peephole2
+  [(set (match_operand:SI 0 "register_operand")
+        (plus:SI (match_dup 0) (match_operand 2 "const_int_operand")))
+   (set (mem:HI (plus:SI (match_dup 0) (match_operand 4 "const_int_operand")))
+        (match_operand:HI 5 "register_operand")
+   )]
+   "reload_completed && operands[0] != operands[5] && INTVAL(operands[2]) == 2 && INTVAL(operands[2]) == -INTVAL(operands[4])
+   "[(set (mem:HI (post_inc:SI (match_dup 0))) (match_dup 5))]
+)
+(define_peephole2
+  [(set (match_operand:SI 0 "register_operand")
+        (plus:SI (match_dup 0) (match_operand 2 "const_int_operand")))
+   (set (mem:SI (plus:SI (match_dup 0) (match_operand 4 "const_int_operand")))
+        (match_operand:SI 5 "register_operand")
+   )]
+   "reload_completed && operands[0] != operands[5] && INTVAL(operands[2]) == 4 && INTVAL(operands[2]) == -INTVAL(operands[4])
+   "[(set (mem:SI (post_inc:SI (match_dup 0))) (match_dup 5))]
+)
+
+
+;;
+;;        (set (reg/f:SI 8 a0 [35])
+;;             (plus:SI (reg/f:SI 14 a6 [orig:31 SysBase.0_2 ] [31])
+;;                      (const_int -12 [0xfffffffffffffff4]))) in.c:5 142 {*addsi3_internal}
+;;        (set (reg:SI 0 d0)
+;;             (call (mem:QI (reg/f:SI 8 a0 [35]) [0 *_3 S1 A8])
+;;                   (const_int 0 [0]))) in.c:5 467 {*non_symbolic_call_value}
+(define_peephole2
+  [(set (match_operand:SI 0 "register_operand")
+        (plus:SI (match_operand:SI 1 "register_operand")
+                 (match_operand 2 "const_int_operand")))
+   (set (match_operand 3 "")
+        (call (mem (match_dup 0))
+              (match_operand 5 "const_int_operand")))]
+  "reload_completed && 
+   INTVAL(operands[2]) < 0x8000 && INTVAL(operands[2]) > -0x7fff && ADDRESS_REG_P(operands[1])
+&& peep2_reg_dead_p (2, operands[0])
+  "
+  [
+   (set (match_dup 3) (call (mem:QI (plus:SI (match_dup 1) (match_dup 2))) (match_dup 5)))
+  ]
+  ""
+)
+
+;; peepholes to omit a tmp register for various modes
+(define_peephole2
+  [(set (match_operand:QI 0 "register_operand")
+        (match_operand:QI 1 "memory_src_operand"))
+   (set (match_operand:QI 2 "memory_operand")
+        (match_dup 0))]
+  "peep2_reg_dead_p (2, operands[0])
+   && !(
+		   (GET_CODE (XEXP (operands[1], 0)) == POST_INC || GET_CODE(XEXP (operands[1], 0)) == PRE_DEC)
+		   && reg_overlap_mentioned_p (XEXP (XEXP (operands[1], 0), 0), operands[2])
+	   )
+  && !reg_overlap_mentioned_p (operands[0], operands[2])
+  "
+  [(set (match_dup 2) (match_dup 1))]
+  ""
+)
+
+(define_peephole2
+  [(set (match_operand:HI 0 "register_operand")
+        (match_operand:HI 1 "memory_src_operand"))
+   (set (match_operand:HI 2 "memory_operand")
+        (match_dup 0))]
+  "peep2_reg_dead_p (2, operands[0])
+   && !(
+		   (GET_CODE (XEXP (operands[1], 0)) == POST_INC || GET_CODE(XEXP (operands[1], 0)) == PRE_DEC)
+		   && reg_overlap_mentioned_p (XEXP (XEXP (operands[1], 0), 0), operands[2])
+	   )
+  && !reg_overlap_mentioned_p (operands[0], operands[2])
+  "
+  [(set (match_dup 2) (match_dup 1))]
+  ""
+)
+
+(define_peephole2
+  [(set (match_operand:SI 0 "register_operand")
+        (match_operand:SI 1 "memory_src_operand"))
+   (set (match_operand:SI 2 "memory_operand")
+        (match_dup 0))]
+  "peep2_reg_dead_p (2, operands[0])
+   && !(
+		   (GET_CODE (XEXP (operands[1], 0)) == POST_INC || GET_CODE(XEXP (operands[1], 0)) == PRE_DEC)
+		   && reg_overlap_mentioned_p (XEXP (XEXP (operands[1], 0), 0), operands[2])
+	   )
+  && !reg_overlap_mentioned_p (operands[0], operands[2])
+  "
+  [(set (match_dup 2) (match_dup 1))]
+  ""
+)
+;; use the const int in the assignment to benefit from moveq
+(define_peephole2
+  [(set (match_operand:SI 0 "register_operand")
+        (match_operand:SI 1 "general_src_operand"))
+   (set (match_dup 0)
+        (and:SI (match_dup 0) (match_operand:SI 2 "const_int_operand")))]
+  "(DATA_REG_P(operands[1]) || MEM_P(operands[1])) && !reg_overlap_mentioned_p(operands[0], operands[1])"
+  [(set (match_dup 0) (match_dup 2))
+   (set (match_dup 0) (and:SI (match_dup 0) (match_dup 1)))]
+  ""
+)
+
+(define_peephole2
+  [(set (match_operand:SI 0 "register_operand")
+        (match_operand:SI 1 "general_src_operand"))
+   (set (match_dup 0)
+        (ior:SI (match_dup 0) (match_operand:SI 2 "const_int_operand")))]
+  "(DATA_REG_P(operands[1]) || MEM_P(operands[1])) && !reg_overlap_mentioned_p(operands[0], operands[1])"
+  [(set (match_dup 0) (match_dup 2))
+   (set (match_dup 0) (ior:SI (match_dup 0) (match_dup 1)))]
+  ""
+)
+
+(define_peephole2
+  [(set (match_operand:SI 0 "register_operand")
+        (match_operand:SI 1 "general_src_operand"))
+   (set (match_dup 0)
+        (plus:SI (match_dup 0) (match_operand:SI 2 "const_int_operand")))]
+  "(DATA_REG_P(operands[1]) || MEM_P(operands[1])) && !reg_overlap_mentioned_p(operands[0], operands[1])
+   && (INTVAL(operands[2]) > 8 || INTVAL(operands[2]) < -8)
+  "
+  [(set (match_dup 0) (match_dup 2))
+   (set (match_dup 0) (plus:SI (match_dup 0) (match_dup 1)))]
+  ""
+)
+
+;; @ 2nd peephole2 pass - after sched2
+(define_peephole2
+  [(set (cc0) (match_operand 0))
+   (set (match_operand:QI 1 "register_operand")
+        (match_operand 2))
+   (set (match_operand:HI 3 "register_operand")
+        (sign_extend:HI (match_dup 1)))
+   (set (match_operand:SI 4 "register_operand")
+        (sign_extend:SI (match_dup 3)))
+   (set (match_dup 4) (neg:SI (match_dup 4)))]
+  "current_pass->static_pass_number == 2 && !reg_overlap_mentioned_p(operands[1], operands[0])"
+  [(set (match_dup 4) (match_dup 3))
+   (set (cc0) (match_dup 0))
+   (set (match_dup 1) (match_dup 2))
+   (set (match_dup 5) (neg:QI (match_dup 5)))
+  ]
+  "operands[3] = const0_rtx;
+   operands[5] = gen_rtx_REG (QImode, REGNO (operands[4]));"
+)
+
+(define_peephole2
+  [(match_scratch:SI 5 "d")
+   (set (strict_low_part (match_operand 0 "register_operand")) (match_operand 1))
+   (set (match_operand:SI 2 "register_operand" ) (and:SI (match_dup 2) (match_operand 7)))
+   (set (match_operand 3) (match_operand 4))]
+  "  MEM_P (operands[4])
+  && CONST_INT_P (operands[7]) 
+  && ((GET_MODE (operands[0]) == QImode && INTVAL(operands[7]) == 0xff)
+   || (GET_MODE (operands[0]) == HImode && INTVAL(operands[7]) == 0xffff))
+  && peep2_reg_dead_p (3, operands[2]) && peep2_reg_dead_p (3, operands[0])
+  && !reg_mentioned_p (operands[2], operands[3])
+  "
+  [(set (match_dup 5) (const_int 0))
+   (set (strict_low_part (match_dup 6)) (match_dup 1))
+   (set (match_dup 3) (match_dup 4))]
+  "
+  extern void replace_reg (rtx x, unsigned regno, rtx newreg, int offset);
+    operands[6] = gen_lowpart (GET_MODE (operands[1]), operands[5]);
+    replace_reg (operands[4], REGNO (operands[2]), operands[5], 0);
+  "
+)
+
+;; used in the additional peephole2 pass after combine!
+;; needs a patch of reload in case of spilling.
+;; combine temp reg away with auto_inc memory ref.
+(define_peephole2
+  [(set (match_operand 0 "register_operand" "")
+	  (mem (match_operand:SI 1 "register_operand" "")))
+   (set (match_operand 2 "memory_operand")
+	  (plus (match_dup 0) (match_operand 3 "general_src_operand" "")))]
+  "!reload_completed 
+   && GET_CODE(XEXP(operands[2],0)) == POST_INC && rtx_equal_p(operands[1], XEXP(XEXP(operands[2],0),0))
+   && peep2_reg_dead_p (2, operands[0])"
+  [(set (match_dup 2) (match_dup 4))]
+  "operands[4] = gen_rtx_PLUS(GET_MODE(operands[0]), gen_rtx_raw_MEM(GET_MODE(operands[0]), operands[1]), operands[3]);
+  ")
+
+(define_peephole2
+  [(set (match_operand 0 "register_operand" "")
+	  (mem (match_operand:SI 1 "register_operand" "")))
+   (set (match_operand 2 "memory_operand")
+	  (and (match_dup 0) (match_operand 3 "general_src_operand" "")))]
+  "!reload_completed && !rtx_equal_p(operands[0], operands[1]) 
+   && GET_CODE(XEXP(operands[2],0)) == POST_INC && rtx_equal_p(operands[1], XEXP(XEXP(operands[2],0),0))
+   && peep2_reg_dead_p (2, operands[0])"
+  [(set (match_dup 2) (match_dup 4))]
+  "operands[4] = gen_rtx_AND(GET_MODE(operands[0]), gen_rtx_raw_MEM(GET_MODE(operands[0]), operands[1]), operands[3]);
+  ")
+
+(define_peephole2
+  [(set (match_operand 0 "register_operand" "")
+	  (mem (match_operand:SI 1 "register_operand" "")))
+   (set (match_operand 2 "memory_operand")
+	  (ior (match_dup 0) (match_operand 3 "general_src_operand" "")))]
+  "!reload_completed && !rtx_equal_p(operands[0], operands[1]) 
+   && GET_CODE(XEXP(operands[2],0)) == POST_INC && rtx_equal_p(operands[1], XEXP(XEXP(operands[2],0),0))
+   && peep2_reg_dead_p (2, operands[0])"
+  [(set (match_dup 2) (match_dup 4))]
+  "operands[4] = gen_rtx_IOR(GET_MODE(operands[0]), gen_rtx_raw_MEM(GET_MODE(operands[0]), operands[1]), operands[3]);
+  ")
+
+(define_peephole2
+  [(set (match_operand 0 "register_operand" "")
+	  (mem (match_operand:SI 1 "register_operand" "")))
+   (set (match_operand 2 "memory_operand")
+	  (xor (match_dup 0) (match_operand 3 "general_src_operand" "")))]
+  "!reload_completed && !rtx_equal_p(operands[0], operands[1]) 
+   && GET_CODE(XEXP(operands[2],0)) == POST_INC && rtx_equal_p(operands[1], XEXP(XEXP(operands[2],0),0))
+   && peep2_reg_dead_p (2, operands[0])"
+  [(set (match_dup 2) (match_dup 4))]
+  "operands[4] = gen_rtx_XOR(GET_MODE(operands[0]), gen_rtx_raw_MEM(GET_MODE(operands[0]), operands[1]), operands[3]);
+  ")
+
+;; swap: reload can't handle the swapsi insn.
+;; workaround:
+;; 2 dummy insns used before reload:
+;; one to load each register, one to store each register
+;; reload will add necessary reloads
+(define_insn "swap_dummy1"
+  [(set (cc0) (compare (match_operand 0 "register_operand" "r")
+		               (match_operand 1 "register_operand" "r")))
+   (clobber (cc0))]
+"0")
+(define_insn "swap_dummy2"
+  [(set (match_operand 0 "register_operand" "=r") (cc0))
+   (set (match_operand 1 "register_operand" "=r") (cc0))]
+"0")
+
+;; a peephole to create the dummy insns
+(define_peephole2
+  [(set (match_operand 0 "register_operand" "") (match_operand 1 "register_operand" ""))
+   (set (match_dup 1) (match_operand 2 "register_operand" ""))
+   (set (match_dup 2) (match_dup 0))]
+  "0 && !reload_completed && peep2_reg_dead_p (3, operands[0])"
+   [(parallel [(set (cc0) (compare (match_dup 1) (match_dup 2)))
+			   (clobber (cc0))])
+	(parallel [(set (match_dup 1) (cc0))
+	           (set (match_dup 2) (cc0))])]
+  "")
+
+;; and a final peephole to resolve the dummy insns.
+(define_peephole2
+   [(parallel [(set (cc0) (compare (match_operand 0 "register_operand" "") (match_operand 1 "register_operand" "")))
+			   (clobber (cc0))])
+	(parallel [(set (match_dup 0) (cc0))
+			   (set (match_dup 1) (cc0))])]
+  "0 && reload_completed"
+   [(parallel [(set (match_dup 0) (match_dup 1))
+			   (set (match_dup 1) (match_dup 0))])]
+  "")
+
+  
+;; a late peephole to convert directly.
+(define_peephole2
+[(set (match_operand:SI 0 "register_operand" "") (match_operand:SI 1 "register_operand" ""))
+ (set (match_dup 1) (match_operand:SI 2 "register_operand" ""))
+ (set (match_dup 2) (match_dup 0))]
+ "reload_completed && peep2_reg_dead_p (3, operands[0])"
+  [(parallel [(set (match_dup 2) (match_dup 1))
+			   (set (match_dup 1) (match_dup 2))])]
+ "")
+
+;; loop optimizer tends to generate this due to pointer arithmetic
+;; eliminate the shifts
+(define_peephole2
+  [(set (match_operand:SI 0 "register_operand" "")
+		(plus:SI (ashift:SI (match_operand:SI 1 "register_operand" "")
+		                    (match_operand 3 "const_int_operand" ""))
+		         (match_operand 4 "const_int_operand" "")))
+   (set (match_operand:SI 2 "register_operand" "")
+		(lshiftrt:SI (match_dup 0) (match_dup 3)))
+   (clobber (pc))]
+  "-INTVAL(operands[4]) == 1<<INTVAL(operands[3]) && peep2_reg_dead_p (2, operands[0])"
+  [(set (match_dup 2) (match_dup 1))
+   (set (match_dup 2) (plus:SI (match_dup 2) (const_int -1)))]
+  "")
+
+(define_peephole2
+  [(set (match_operand:SI 0 "register_operand" "")
+		(plus:SI (ashift:SI (sign_extend:SI (subreg:HI (match_operand:SI 1 "register_operand" "") 2))
+		                    (match_operand 3 "const_int_operand" ""))
+		         (match_operand 4 "const_int_operand" "")))
+   (set (match_operand:SI 2 "register_operand" "")
+		(lshiftrt:SI (match_dup 0) (match_dup 3)))
+   (clobber (pc))]
+  "-INTVAL(operands[4]) == 1<<INTVAL(operands[3]) && peep2_reg_dead_p (2, operands[0])"
+  [(set (match_dup 2) (sign_extend:SI (subreg:HI (match_dup 1) 2)))
+   (set (match_dup 2) (plus:SI (match_dup 2) (const_int -1)))]
+  "")
+
+;; ============================================================
+;; combine post_inc to use larger modes
+;; ============================================================
+		
+;; combine post_inc if possible .b #x,(a0)+, #y,(a0)+ -> .w #z,(a0)+
+(define_peephole2
+  [(set (mem:QI (post_inc:SI (match_operand:SI 0 "register_operand" "")))
+		(match_operand:QI 1 "const_int_operand" ""))
+   (set (mem:QI (post_inc:SI (match_dup 0)))
+   		(match_operand:QI 2 "const_int_operand" ""))
+   ]
+  "reload_completed && !TUNE_68000_10 && (operands[3] = SET_DEST(PATTERN(insn))) && !operands[3]->volatil"
+  [(set (mem:HI (post_inc:SI (match_dup 0))) (match_dup 4))]
+{
+    operands[4] = GEN_INT((INTVAL(operands[1])<<8) | (INTVAL(operands[2]) & 0xff));
+})
+
+;; combine post_inc if possible .b #x,(a0)+, #y,(a0) -> .w #z,(a0)
+(define_peephole2
+  [(set (mem:QI (post_inc:SI (match_operand:SI 0 "register_operand" "")))
+		(match_operand:QI 1 "const_int_operand" ""))
+   (set (mem:QI (match_dup 0))
+   		(match_operand:QI 2 "const_int_operand" ""))
+   ]
+  "reload_completed && !TUNE_68000_10 && (operands[3] = SET_DEST(PATTERN(insn))) && !operands[3]->volatil
+	&& peep2_reg_dead_p (2, operands[0])"
+  [(set (mem:HI (match_dup 0)) (match_dup 4))]
+{
+    operands[4] = GEN_INT((INTVAL(operands[1])<<8) | (INTVAL(operands[2]) & 0xff));
+})
+
+;; combine post_inc if possible .w #x,(a0)+, #y,(a0)+ -> .l #z,(a0)+
+(define_peephole2
+  [(set (mem:HI (post_inc:SI (match_operand:SI 0 "register_operand" "")))
+		(match_operand:HI 1 "const_int_operand" ""))
+   (set (mem:HI (post_inc:SI (match_dup 0)))
+   		(match_operand:HI 2 "const_int_operand" ""))
+   ]
+  "reload_completed && (operands[3] = SET_DEST(PATTERN(insn))) && !operands[3]->volatil"
+  [(set (mem:SI (post_inc:SI (match_dup 0))) (match_dup 4))]
+{
+    operands[4] = GEN_INT((INTVAL(operands[1])<<16) | (INTVAL(operands[2]) & 0xffff));
+})
+
+;; combine post_inc if possible .w #x,(a0)+, #y,(a0) -> .l #z,(a0)
+(define_peephole2
+  [(set (mem:HI (post_inc:SI (match_operand:SI 0 "register_operand" "")))
+		(match_operand:HI 1 "const_int_operand" ""))
+   (set (mem:HI (match_dup 0))
+   		(match_operand:HI 2 "const_int_operand" ""))
+   ]
+   "reload_completed && (operands[3] = SET_DEST(PATTERN(insn))) && !operands[3]->volatil
+ 	&& peep2_reg_dead_p (2, operands[0])"
+  [(set (mem:SI (match_dup 0)) (match_dup 4))]
+{
+    operands[4] = GEN_INT((INTVAL(operands[1])<<16) | (INTVAL(operands[2]) & 0xffff));
+})
+
+;; combine post_inc if possible (a0)+,(a1)+
+(define_peephole2
+  [(set (mem:QI (post_inc:SI (match_operand:SI 0 "register_operand" "")))
+		(mem:QI (post_inc:SI (match_operand:SI 1 "register_operand" ""))))
+   (set (mem:QI (post_inc:SI (match_dup 0)))
+		(mem:QI (post_inc:SI (match_dup 1))))]
+  "reload_completed && !TUNE_68000_10 && (operands[3] = SET_DEST(PATTERN(insn))) && !operands[3]->volatil"
+  [(set (mem:HI (post_inc:SI (match_dup 0))) (mem:HI (post_inc:SI (match_dup 1))))]
+{})
+
+;; combine post_inc if possible (a0)+,(a1)+
+(define_peephole2
+  [(set (mem:HI (post_inc:SI (match_operand:SI 0 "register_operand" "")))
+		(mem:HI (post_inc:SI (match_operand:SI 1 "register_operand" ""))))
+   (set (mem:HI (post_inc:SI (match_dup 0)))
+		(mem:HI (post_inc:SI (match_dup 1))))]
+  "reload_completed && (operands[3] = SET_DEST(PATTERN(insn))) && !operands[3]->volatil"
+  [(set (mem:SI (post_inc:SI (match_dup 0))) (mem:SI (post_inc:SI (match_dup 1))))]
+{})
+
+;; combine post_inc if possible (a0)+,(a1)+ with (a0),(a1)
+(define_peephole2
+  [(set (mem:QI (post_inc:SI (match_operand:SI 0 "register_operand" "")))
+		(mem:QI (post_inc:SI (match_operand:SI 1 "register_operand" ""))))
+   (set (mem:QI (match_dup 0))
+		(mem:QI (match_dup 1)))]
+  "reload_completed && !TUNE_68000_10 && peep2_reg_dead_p (2, operands[0]) && peep2_reg_dead_p (2, operands[1]) && (operands[3] = SET_DEST(PATTERN(insn))) && !operands[3]->volatil"
+  [(set (mem:HI (match_dup 0)) (mem:HI (match_dup 1)))]
+{})
+
+;; combine post_inc if possible (a0)+,(a1)+ with (a0),(a1)
+(define_peephole2
+  [(set (mem:HI (post_inc:SI (match_operand:SI 0 "register_operand" "")))
+		(mem:HI (post_inc:SI (match_operand:SI 1 "register_operand" ""))))
+   (set (mem:HI (match_dup 0))
+		(mem:HI (match_dup 1)))]
+  "reload_completed && peep2_reg_dead_p (2, operands[0]) && peep2_reg_dead_p (2, operands[1]) && (operands[3] = SET_DEST(PATTERN(insn))) && !operands[3]->volatil"
+  [(set (mem:SI (match_dup 0)) (mem:SI (match_dup 1)))]
+{})
+
+
+;; combine if n tests a single bit
+;; and #n,dx
+;; tst dx
+;; ->
+;; btst #m,dx
+(define_peephole2
+   [(set (match_operand 0 "register_operand" "") 
+	     (and (match_dup 0) (match_operand 1 "const_int_operand" "")))
+    (set (cc0) (compare (match_dup 0) (const_int 0)))]
+"DATA_REG_P (operands[0]) && exact_log2 (INTVAL (operands[1])) >= 0 && peep2_reg_dead_p (2, operands[0])"
+   [(set (cc0)
+         (compare (zero_extract:SI (match_dup 0) (const_int 1) (match_dup 2))
+   				  (const_int 0)))]
+{
+  if (GET_MODE (operands[0]) != SImode)
+    {
+	  rtx reg = gen_rtx_REG (SImode, REGNO (operands[0]));
+	  set_reg_attrs_from_value (reg, operands[0]);
+	  operands[0] = reg;
+    }
+  operands[2] = GEN_INT (31 - exact_log2 (INTVAL (operands[1])));
+})
+
+;; combine load mem->reg plus btst into a single btst.
+;; move (ax,n),d0
+;; btst #x,d0
+;; -> 
+;; btst #y(ax,m)
+;; adjust y and m
+(define_peephole2
+  [(set (match_operand 0 "register_operand" "") (match_operand 1 "memory_operand" ""))
+			  
+   (set (cc0)
+		(compare (zero_extract:SI (match_operand:SI 2 "register_operand" "")
+		                          (const_int 1)
+				                  (match_operand 3 "const_int_operand" ""))
+				 (const_int 0)))]
+  "REGNO (operands[0]) == REGNO (operands[2])
+  && ((GET_CODE (XEXP (operands[1], 0)) == PLUS 
+    && REG_P (XEXP (XEXP (operands[1], 0), 0))
+    && CONST_INT_P (XEXP (XEXP (operands[1], 0), 1))) 
+	|| REG_P(XEXP (operands[1], 0)))
+  && peep2_reg_dead_p (2, operands[0])
+  "
+  [(set (cc0)
+		(compare (zero_extract:SI (match_dup 3) (const_int 1) (match_dup 4)) 
+				 (const_int 0)))]
+{
+	rtx reg = XEXP (operands[1], 0);
+	int offset;
+	int bit;
+	if (REG_P (reg))
+	  offset = 0;
+	else
+	  {
+		offset = INTVAL (XEXP (reg, 1));
+		reg = XEXP (reg, 0);
+	  }
+	offset += GET_MODE_SIZE (GET_MODE (operands[1])) - 1;
+	bit = 31 - INTVAL (operands[3]);
+	offset -= bit >> 3;
+	bit = 7 - (bit & 7);
+	if (offset)
+	  operands[3] = change_address (operands[1], QImode, gen_rtx_PLUS (SImode, reg, GEN_INT (offset)));
+	else
+      operands[3] = change_address (operands[1], QImode, reg);
+	operands[4] = GEN_INT(bit);
+})
+
+;; combine insns to spare the tmp register.
+(define_peephole2
+  [(set (match_operand 0 "register_operand" "") (match_operand 1 "register_operand" ""))
+   (set (cc0)
+		(compare (zero_extract:SI (match_operand:SI 2 "register_operand" "")
+		                          (const_int 1)
+				                  (match_operand 3 "const_int_operand" ""))
+				 (const_int 0)))]
+  "DATA_REG_P (operands[1]) && peep2_reg_dead_p (2, operands[0]) && REGNO (operands[0]) == REGNO (operands[2])"
+  [(set (cc0)
+		(compare (zero_extract:SI (match_dup 1) (const_int 1) (match_dup 3)) 
+				 (const_int 0)))]
+{
+  if (GET_MODE (operands[1]) != SImode)
+    {
+	  rtx reg = gen_rtx_REG (SImode, REGNO (operands[1]));
+	  set_reg_attrs_from_value (reg, operands[1]);
+	  operands[1] = reg;
+    }
+})
+
+;; eliminate temp registers, if tmp is not a FP register.
+;; move (ax),rx
+;; add  reg,rx
+;; move reg,(ax)+
+;; ->
+;; add reg,(ax)+
+;; ensure that reg != rx
+(define_peephole2
+  [(set (match_operand 0 "register_operand" "") (match_operand 1 "memory_operand" ""))
+   (set (match_dup 0) (plus (match_dup 0) (match_operand 2 "register_operand" "")))
+   (set (match_operand 3 "memory_operand" "") (match_dup 0))]
+  "DATA_REG_P (operands[2])
+&& operands[0] != operands[2]  
+&& !FP_REG_P (operands[0])
+&& REG_P (XEXP (operands[1], 0)) && peep2_reg_dead_p (3, operands[0])
+
+&& (XEXP (operands[1], 0) == XEXP (operands[3], 0)
+ || (GET_CODE (XEXP (operands[3], 0)) == POST_INC && XEXP (operands[1], 0) == XEXP (XEXP (operands[3], 0), 0)))
+   "
+  [(set (match_dup 3) (match_dup 4))]
+{
+	operands[4] = gen_rtx_PLUS (GET_MODE (operands[0]), operands[3], operands[2]);
+})
+
+;; eliminate temp registers - before reload
+;; move (ax),rx
+;; add  reg,rx
+;; move reg,(ax)+
+;; ->
+;; add reg,(ax)+
+(define_peephole2
+  [(set (match_operand 0 "register_operand" "") (match_operand 1 "memory_operand" ""))
+   (set (match_operand 3 "memory_operand" "") (plus (match_operand 2 "register_operand" "") (match_dup 0)))]
+  "REG_P (XEXP (operands[1], 0)) && peep2_reg_dead_p (2, operands[0])
+&& operands[0] != operands[2]  
+&& (XEXP (operands[1], 0) == XEXP (operands[3], 0)
+ || (GET_CODE (XEXP (operands[3], 0)) == POST_INC && XEXP (operands[1], 0) == XEXP (XEXP (operands[3], 0), 0)))
+   "
+ [(set (match_dup 3) (match_dup 4))]
+{
+	operands[4] = gen_rtx_PLUS (GET_MODE (operands[0]), operands[1], operands[2]);
+})
+
+;; eliminate temp registers - before reload
+;; move (ax),rx
+;; sub  reg,rx
+;; move reg,(ax)+
+;; ->
+;; sub reg,(ax)+
+(define_peephole2
+  [(set (match_operand 0 "register_operand" "") (match_operand 1 "memory_operand" ""))
+   (set (match_operand 3 "memory_operand" "") (minus (match_operand 2 "register_operand" "") (match_dup 0)))]
+  "REG_P (XEXP (operands[1], 0)) && peep2_reg_dead_p (2, operands[0])
+&& (XEXP (operands[1], 0) == XEXP (operands[3], 0)
+ || (GET_CODE (XEXP (operands[3], 0)) == POST_INC && XEXP (operands[1], 0) == XEXP (XEXP (operands[3], 0), 0)))
+   "
+ [(set (match_dup 3) (match_dup 4))]
+{
+	operands[4] = gen_rtx_MINUS (GET_MODE (operands[0]), operands[1], operands[2]);
+})
+
+
+;; eliminate temp registers - before reload
+;; move (ax),rx
+;; or  reg,rx
+;; move reg,(ax)+
+;; ->
+;; or reg,(ax)+
+(define_peephole2
+  [(set (match_operand 0 "register_operand" "") (match_operand 1 "memory_operand" ""))
+   (set (match_operand 3 "memory_operand" "") (ior (match_operand 2 "register_operand" "") (match_dup 0)))]
+  "REG_P (XEXP (operands[1], 0)) && peep2_reg_dead_p (2, operands[0])
+&& (XEXP (operands[1], 0) == XEXP (operands[3], 0)
+ || (GET_CODE (XEXP (operands[3], 0)) == POST_INC && XEXP (operands[1], 0) == XEXP (XEXP (operands[3], 0), 0)))
+   "
+ [(set (match_dup 3) (match_dup 4))]
+{
+	operands[4] = gen_rtx_IOR (GET_MODE (operands[0]), operands[1], operands[2]);
+})
+
+;; eliminate temp registers - before reload
+;; move (ax),rx
+;; xor  reg,rx
+;; move reg,(ax)+
+;; ->
+;; xor reg,(ax)+
+(define_peephole2
+  [(set (match_operand 0 "register_operand" "") (match_operand 1 "memory_operand" ""))
+   (set (match_operand 3 "memory_operand" "") (xor (match_operand 2 "register_operand" "") (match_dup 0)))]
+  "REG_P (XEXP (operands[1], 0)) && peep2_reg_dead_p (2, operands[0])
+&& (XEXP (operands[1], 0) == XEXP (operands[3], 0)
+ || (GET_CODE (XEXP (operands[3], 0)) == POST_INC && XEXP (operands[1], 0) == XEXP (XEXP (operands[3], 0), 0)))
+   "
+ [(set (match_dup 3) (match_dup 4))]
+{
+	operands[4] = gen_rtx_XOR (GET_MODE (operands[0]), operands[1], operands[2]);
+})
+
+;; eliminate temp registers - before reload
+;; move (ax),rx
+;; and  reg,rx
+;; move reg,(ax)+
+;; ->
+;; and reg,(ax)+
+(define_peephole2
+  [(set (match_operand 0 "register_operand" "") (match_operand 1 "memory_operand" ""))
+   (set (match_operand 3 "memory_operand" "") (and (match_operand 2 "register_operand" "") (match_dup 0)))]
+  "REG_P (XEXP (operands[1], 0)) && peep2_reg_dead_p (2, operands[0])
+&& (XEXP (operands[1], 0) == XEXP (operands[3], 0)
+ || (GET_CODE (XEXP (operands[3], 0)) == POST_INC && XEXP (operands[1], 0) == XEXP (XEXP (operands[3], 0), 0)))
+   "
+ [(set (match_dup 3) (match_dup 4))]
+{
+	operands[4] = gen_rtx_AND (GET_MODE (operands[0]), operands[1], operands[2]);
+})
+
+
+(define_int_iterator MONADS
+	[UNSPEC_SIN
+	 UNSPEC_COS
+	 UNSPEC_TAN
+	 UNSPEC_ASIN
+	 UNSPEC_ACOS
+	 UNSPEC_ATAN
+	 UNSPEC_EXP
+	 UNSPEC_EXPM1
+	 UNSPEC_EXP10
+	 UNSPEC_EXP2
+	 UNSPEC_LOG
+	 UNSPEC_LOG1P
+	 UNSPEC_LOG10
+	 UNSPEC_LOG2
+	 ])
+
+(define_int_attr monads
+	[(UNSPEC_SIN "sin")
+	 (UNSPEC_COS "cos")
+	 (UNSPEC_TAN "tan")
+	 (UNSPEC_ASIN "asin")
+	 (UNSPEC_ACOS "acos")
+	 (UNSPEC_ATAN "atan")
+	 (UNSPEC_EXP "exp")
+	 (UNSPEC_EXPM1 "expm1")
+	 (UNSPEC_EXP10 "exp10")
+	 (UNSPEC_EXP2 "exp2")
+	 (UNSPEC_LOG "log")
+	 (UNSPEC_LOG1P "log1p")
+	 (UNSPEC_LOG10 "log10")
+	 (UNSPEC_LOG2 "log2")
+	 ])
+
+(define_int_attr monadm
+	[(UNSPEC_SIN "sin")
+	 (UNSPEC_COS "cos")
+	 (UNSPEC_TAN "tan")
+	 (UNSPEC_ASIN "asin")
+	 (UNSPEC_ACOS "acos")
+	 (UNSPEC_ATAN "atan")
+	 (UNSPEC_EXP "etox")
+	 (UNSPEC_EXPM1 "etoxm1")
+	 (UNSPEC_EXP10 "tentox")
+	 (UNSPEC_EXP2 "twotox")
+	 (UNSPEC_LOG "logn")
+	 (UNSPEC_LOG1P "lognp1")
+	 (UNSPEC_LOG10 "log10")
+	 (UNSPEC_LOG2 "log2")
+	 ])
+
+(define_int_attr monoffs
+	[(UNSPEC_SIN "0x24")
+	 (UNSPEC_COS "0x2a")
+	 (UNSPEC_TAN "0x30")
+	 (UNSPEC_ASIN "0x72")
+	 (UNSPEC_ACOS "0x78")
+	 (UNSPEC_ATAN "0x1e")
+	 (UNSPEC_EXP "0x4e")
+	 (UNSPEC_EXPM1 "0x4e")
+	 (UNSPEC_EXP10 "0x5a")
+	 (UNSPEC_EXP2 "0x5a")
+	 (UNSPEC_LOG "0x54")
+	 (UNSPEC_LOG1P "0x54")
+	 (UNSPEC_LOG10 "0x7e")
+	 (UNSPEC_LOG2 "0x54")
+	 ])
+
+(define_expand "<monads><mode>2"
+  [(set (match_operand:FP 0 "nonimmediate_operand" "")
+        (unspec:FP [(match_operand:FP 1 "general_operand" "")] MONADS)
+   )]
+  "TARGET_68881 && flag_unsafe_math_optimizations && ((!TARGET_68060 && !TARGET_68040) || TARGET_AMIGA)"
+  {
+    extern rtx singbas;
+	extern rtx doubbas;
+    
+  	if (TARGET_68060 || TARGET_68040)
+  	{
+  	  rtx d0, d1;
+  	  if (0 == strcmp("SF", "<MODE>"))
+  	    {
+  	      if (0 == strcmp("lognp1", "<monadm>"))
+  	        {
+  	          rtx x = gen_reg_rtx(SFmode);
+		      gen_addsf3(x, operands[1], GEN_INT(1));
+		      operands[1] = x;
+		    }
+  	    
+  	      d0 = gen_raw_REG (SFmode, D0_REG);
+  	      d1 = gen_raw_REG (SFmode, D1_REG);
+
+		  if (0 == strcmp("tentox", "<monadm>") || 0 == strcmp("twotox", "<monadm>"))
+		    {
+		      if (0 == strcmp("tentox", "<monadm>"))
+		        emit_move_insn (d0, GEN_INT(0x41200000)); // 10
+		      else
+		        emit_move_insn (d0, GEN_INT(0x40000000)); // 2
+  	          
+	          emit_move_insn (d1, operands[1]);	          
+		    }
+  	      else
+  	        {
+	          emit_move_insn (d0, operands[1]);
+  	        }
+	      
+	      rtx a6 = gen_raw_REG (SImode, A6_REG);
+	      rtx bas;
+	      if (flag_pic >= 3)
+	        bas = gen_rtx_MEM (SImode, gen_rtx_CONST (Pmode,
+	                gen_rtx_PLUS (Pmode, gen_rtx_REG (Pmode, PIC_REG), gen_rtx_CONST (Pmode,
+	                  gen_rtx_UNSPEC (Pmode, gen_rtvec (2, singbas, GEN_INT (0)), UNSPEC_RELOC16)
+	          ))));
+	      else
+	        bas = gen_rtx_MEM (SImode, singbas);
+	      bas->unchanging = 1;
+	      emit_move_insn (a6, bas);
+      
+		  rtx_insn * call = emit_call_insn (gen_rtx_SET (gen_raw_REG (SFmode, D0_REG), gen_acall (gen_rtx_PLUS(SImode, a6, GEN_INT(<monoffs>)), const0_rtx)));
+		  
+		  if (0 == strcmp("tentox", "<monadm>") || 0 == strcmp("twotox", "<monadm>")) // use d0, d1
+		    add_function_usage_to (call, gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_USE(SFmode, d0), 
+		    	gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_USE(SFmode, d1),
+		    			gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER(VOIDmode, d1), 
+		    				gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER(VOIDmode, gen_raw_REG (SImode, A0_REG)),
+		    					gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER(VOIDmode, gen_raw_REG (SImode, A1_REG)),
+				    				gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER(VOIDmode, gen_raw_REG (XFmode, FP0_REG)),
+		    							gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER(VOIDmode, gen_raw_REG (XFmode, FP1_REG)), 0))))))));
+		  else // use d0
+		    add_function_usage_to (call, gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_USE(SFmode, d0),
+		    		gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER(VOIDmode, d1),
+	    				gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER(VOIDmode, gen_raw_REG (SImode, A0_REG)),
+	    					gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER(VOIDmode, gen_raw_REG (SImode, A1_REG)),
+				    			gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER(VOIDmode, gen_raw_REG (XFmode, FP0_REG)),
+				    				gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER(VOIDmode, gen_raw_REG (XFmode, FP1_REG)), 0)))))));
+		  
+		  emit_move_insn (operands[0], d0);
+		  
+		  if (0 == strcmp("etoxm1", "<monadm>"))
+		    gen_subsf3(operands[0], operands[0], GEN_INT(1));
+		  else if (0 == strcmp("log2", "<monadm>"))
+		    gen_mulsf3(operands[0], operands[0], GEN_INT(0x3fb8aa3b)); // 1/log(2)   3ff71547652b82fe
+      	  DONE;
+  	    }
+  	    
+      if (0 == strcmp("lognp1", "<monadm>"))
+        {
+          rtx x = gen_reg_rtx(DFmode);
+	      gen_adddf3(x, operands[1], GEN_INT(1));
+	      operands[1] = x;
+	    }
+    
+      d0 = gen_raw_REG (DFmode, D0_REG);
+
+	  if (0 == strcmp("tentox", "<monadm>") || 0 == strcmp("twotox", "<monadm>"))
+	    {
+	      if (0 == strcmp("tentox", "<monadm>"))
+	        emit_move_insn (d0, GEN_INT(0x41200000)); // 10
+	      else
+	        emit_move_insn (d0, GEN_INT(0x40000000)); // 2
+          d1 = gen_raw_REG (DFmode, D2_REG);
+          emit_move_insn (d1, operands[1]);	          
+	    }
+      else
+        {
+          emit_move_insn (d0, operands[1]);
+        }
+      
+      rtx a6 = gen_raw_REG (SImode, A6_REG);
+      rtx bas;
+      if (flag_pic >= 3)
+        bas = gen_rtx_MEM (SImode, gen_rtx_CONST (Pmode,
+                gen_rtx_PLUS (Pmode, gen_rtx_REG (Pmode, PIC_REG), gen_rtx_CONST (Pmode,
+                  gen_rtx_UNSPEC (Pmode, gen_rtvec (2, doubbas, GEN_INT (0)), UNSPEC_RELOC16)
+          ))));
+      else
+        bas = gen_rtx_MEM (SImode, doubbas);
+      bas->unchanging = 1;
+      emit_move_insn (a6, bas);
+        
+	  rtx_insn * call = emit_call_insn (gen_rtx_SET (gen_raw_REG (DFmode, D0_REG), gen_acall (gen_rtx_PLUS(SImode, a6, GEN_INT(<monoffs>)), const0_rtx)));
+	  
+	  if (0 == strcmp("tentox", "<monadm>") || 0 == strcmp("twotox", "<monadm>")) // use d0, d2
+	    add_function_usage_to (call, gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_USE(DFmode, d0), 
+	    	gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_USE(DFmode, gen_raw_REG (DFmode, D2_REG)), 
+	    				gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER(VOIDmode, gen_raw_REG (SImode, A0_REG)),
+	    					gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER(VOIDmode, gen_raw_REG (SImode, A1_REG)),
+	    						gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER(VOIDmode, gen_raw_REG (XFmode, FP0_REG)), 
+	    							gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER(VOIDmode, gen_raw_REG (XFmode, FP1_REG)), 0)))))));
+	  else	 
+	    add_function_usage_to (call, gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_USE(DFmode, d0), 
+	    				gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER(VOIDmode, gen_raw_REG (SImode, A0_REG)),
+	    					gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER(VOIDmode, gen_raw_REG (SImode, A1_REG)),
+			    			gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER(VOIDmode, gen_raw_REG (XFmode, FP0_REG)), 
+	    						gen_rtx_EXPR_LIST (VOIDmode, gen_rtx_CLOBBER(VOIDmode, gen_raw_REG (XFmode, FP1_REG)), 0))))));
+	  
+	  emit_move_insn (operands[0], d0);
+	  
+	  if (0 == strcmp("etoxm1", "<monadm>"))
+	    gen_subdf3(operands[0], operands[0], GEN_INT(1));
+	  else if (0 == strcmp("log2", "<monadm>"))
+	    gen_muldf3(operands[0], operands[0], GEN_INT(0x3ff71547652b82fe)); // 1/log(2)   
+  	  DONE;  	    
+  	}
+  })
+  
+(define_insn "<monads><mode>2_68881"
+  [(set (match_operand:FP 0 "nonimmediate_operand" "=f")
+	(unspec:FP
+	  [(match_operand:FP 1 "general_operand" "f<FP:dreg>m")] MONADS))]
+  "TARGET_68881 && flag_unsafe_math_optimizations"
+{
+  if (FP_REG_P (operands[1]))
+    return "f<monadm>%.x %1,%0";
+  else
+    return "f<monadm>%.<FP:prec> %1,%0";
+})
+
+
+(define_expand "sincos<mode>3"
+  [(parallel
+    [(set (match_operand:FP 0 "nonimmediate_operand" "")
+        (unspec:FP [(match_operand:FP 2 "general_operand" "")] UNSPEC_COS)
+     )
+     (set (match_operand:FP 1 "nonimmediate_operand" "f<FP:dreg>m")
+          (unspec:FP [(match_dup 2)] UNSPEC_SIN)
+     )])]
+  "TARGET_68881 && flag_unsafe_math_optimizations && !TARGET_68060 && !TARGET_68040"
+  "")
+  
+(define_insn "sincos<mode>2_68881"
+  [(set (match_operand:FP 0 "nonimmediate_operand" "=f")
+        (unspec:FP [(match_operand:FP 2 "general_operand" "")] UNSPEC_COS)
+   )
+   (set (match_operand:FP 1 "nonimmediate_operand" "=f")
+        (unspec:FP [(match_dup 2)] UNSPEC_SIN)
+   )]
+  "TARGET_68881 && flag_unsafe_math_optimizations"
+{
+  if (FP_REG_P (operands[2]))
+    return "fsincos%.x %2,%0,%1";
+  else
+    return "fsincos%.<FP:prec> %2,%0,%1";
+})
+
+;; combine clr if possible .l #0,x(a0), #0,x+4(a0) -> .q #0,x(a0)
+;; or .w #0,x(a0), #0,x+2(a0) -> .l #0,x(a0)
+(define_peephole2
+  [(set (mem (plus (match_operand:SI 0 "register_operand" "") (match_operand:SI 2 "const_int_operand" "")))
+		(match_operand 1 "const_int_operand" ""))
+   (set (mem (plus (match_dup 0) (match_operand:SI 3 "const_int_operand" "")))
+   		(match_dup 1))
+   ]
+  "(operands[4] = SET_DEST(PATTERN(insn))) && 
+  (TARGET_68080 || GET_MODE_SIZE(GET_MODE(operands[4])) == 2) && 
+  current_pass->static_pass_number >= 2 && 
+   0 == INTVAL (operands[1]) && 
+   !operands[4]->volatil &&
+   (GET_MODE(operands[4]) == HImode || GET_MODE(operands[4]) == SImode) && 
+   INTVAL (operands[2]) + GET_MODE_SIZE(GET_MODE(operands[4])) == INTVAL (operands[3])"
+  [(set (match_dup 5) (match_dup 1))]
+{
+  operands[5] = gen_rtx_MEM(GET_MODE(operands[4]) == HImode ? SImode : DImode,
+                            gen_rtx_PLUS(SImode, operands[0], operands[2]));
+})
+
+;; combine post_inc if possible .l #0,(a0)+, #0,(a0)+ -> .q #0,(a0)+
+(define_peephole2
+  [(set (mem:SI (post_inc:SI (match_operand:SI 0 "register_operand" "")))
+		(match_operand:SI 1 "const_int_operand" ""))
+   (set (mem:SI (post_inc:SI (match_dup 0)))
+   		(match_dup 1))
+   ]
+  "TARGET_68080 && current_pass->static_pass_number >= 2 && !INTVAL (operands[1]) && (operands[3] = SET_DEST(PATTERN(insn))) && !operands[3]->volatil"
+  [(set (mem:DI (post_inc:SI (match_dup 0))) (match_dup 1))]
+{
+})
+
+;; unroll loops unfortunately adds additional registers which are used once
+;; eliminate these
+;; lea 8(a0),a1
+;; clr.l (a1)
+;; ->
+;; clr.l 8(a0)
+;; lea 8(a0),a1
+(define_peephole2
+  [(set (match_operand:SI 0 "register_operand" "")
+		(plus (match_operand:SI 1 "register_operand" "") (match_operand:SI 2 "const_int_operand" "")))
+   (set (match_operand 3 "memory_operand" "")
+   		(match_operand 4 "general_src_operand" ""))
+   ]
+  "operands[0] != operands[1] && ADDRESS_REG_P(operands[1]) && 
+   MEM_P(operands[3]) && 
+   ADDRESS_REG_P(XEXP(operands[3], 0)) &&
+   XEXP(operands[3], 0) == operands[0] && 
+   ((!TARGET_68000 && !TARGET_68010) || IN_RANGE (INTVAL (operands[2]), -32768, 32767)) &&
+   !reg_overlap_mentioned_p(operands[0], operands[4])
+  "
+  [(set (match_dup 5) (match_dup 4))
+   (set (match_dup 0) (plus:SI (match_dup 1) (match_dup 2)))]
+{
+  operands[5] = gen_rtx_MEM (GET_MODE(operands[3]), gen_rtx_PLUS(SImode, operands[1], operands[2]));
+})
+
+;; also move adds after next insn if possible
+;; lea 16(a0),a1
+;; clr 24(a0)
+;; -->
+;; clr 24(a0)
+;; lea 16(a0),a1
+(define_peephole2
+  [(set (match_operand:SI 0 "register_operand" "")
+		(plus (match_operand:SI 1 "register_operand" "") (match_operand:SI 2 "const_int_operand" "")))
+   (set (match_operand 3 "memory_operand" "") 
+   		(match_operand 4 "general_src_operand" ""))
+   ]
+  "operands[0] != operands[1] &&
+   offsettable_nonstrict_memref_p(operands[3]) &&
+   ((!TARGET_68000 && !TARGET_68010) || IN_RANGE (INTVAL (operands[2]), -32768, 32767)) && 
+   !reg_overlap_mentioned_p(operands[0], operands[3]) && !reg_overlap_mentioned_p(operands[0], operands[4]) && 
+   !reg_overlap_mentioned_p(operands[1], operands[4])
+  "
+  [(set (match_dup 3) (match_dup 4))
+   (set (match_dup 0) (plus:SI (match_dup 1) (match_dup 2)))]
+{
+})
+
+;; same for move loaded register ff offset is adjustable
+;; lea 16(a1),a0
+;; clr 4(a0)
+;; -->
+;; clr 20(a1)
+;; lea 16(a1),a0
+(define_peephole2
+  [(set (match_operand:SI 0 "register_operand" "")
+		(plus (match_operand:SI 1 "register_operand" "") (match_operand:SI 2 "const_int_operand" "")))
+   (set (match_operand 3 "memory_operand" "")
+   		(match_operand 4 "general_src_operand" ""))
+   ]
+  "operands[0] != operands[1] && ADDRESS_REG_P(operands[1]) && 
+   MEM_P(operands[3]) && GET_CODE(XEXP(operands[3], 0)) == PLUS && 
+   ADDRESS_REG_P(XEXP(XEXP(operands[3], 0), 0)) &&
+   CONST_INT_P(XEXP(XEXP(operands[3], 0), 0)) &&
+   ((!TARGET_68000 && !TARGET_68010) || IN_RANGE (INTVAL (operands[2]), -32768, 32767)) && 
+   !reg_overlap_mentioned_p(operands[0], operands[3]) && !reg_overlap_mentioned_p(operands[0], operands[4])
+   "
+  [(set (match_dup 3) (match_dup 4))
+   (set (match_dup 0) (plus:SI (match_dup 1) (match_dup 2)))]
+{
+  operands[3] = gen_rtx_MEM (GET_MODE(operands[3]), gen_rtx_PLUS(SImode, operands[1], 
+                             GEN_INT (INTVAL(operands[2]) + INTVAL(XEXP(XEXP(operands[3], 0), 0)))));
+})   
+
+;; now combine two adds if it's a temp register and source is an ADDRESS_REG
+;; lea 16(a0),a1
+;; lea 112(a1),a0
+;; -->
+;; lea 128(a0),a0
+(define_peephole2
+  [(set (match_operand:SI 0 "register_operand" "")
+		(plus (match_operand:SI 1 "register_operand" "") (match_operand:SI 2 "const_int_operand" "")))
+   (set (match_dup 1)
+		(plus (match_dup 0) (match_operand:SI 3 "const_int_operand" "")))
+   ]
+  "operands[0] != operands[1] && ADDRESS_REG_P (operands[1]) && 
+   ((!TARGET_68000 && !TARGET_68010) || IN_RANGE (INTVAL (operands[2]), -32768, 32767)) &&
+   peep2_reg_dead_p (2, operands[0])
+  "
+  [(set (match_dup 1) (plus:SI (match_dup 1) (match_dup 4)))]
+{
+  operands[4] = GEN_INT (INTVAL (operands[2]) + INTVAL (operands[3]));
+})
+
+;; no check, it's only generated via peephole2
+(define_insn "mov2si_68080"
+  [(set (match_operand:SI 0 "nonimmediate_operand" "=da,o")
+        (match_operand:SI 1 "general_src_operand" "o,da"))
+   (set (match_operand:SI 2 "nonimmediate_operand" "=da,o")
+        (match_operand:SI 3 "general_src_operand" "o,da"))]
+  "TUNE_68080 && reload_completed"
+  "@
+   move2%.l %1,%0:%2
+   move2%.l %1:%3,%0")
+
+;; combine two move.l into move2.l (n,ax),r1:r2
+(define_peephole2
+  [(set (match_operand:SI 0 "register_operand" "")
+        (match_operand:SI 1 "memory_operand" ""))
+   (set (match_operand:SI 2 "register_operand" "")
+        (match_operand:SI 3 "memory_operand" ""))]
+  "TUNE_68080 && reload_completed && flag_experimental && REGNO(operands[0]) < 16 && REGNO(operands[2]) < 16 &&
+   ((ADDRESS_REG_P(XEXP(operands[1], 0)) &&                  // (ax)
+    GET_CODE(XEXP(operands[3], 0)) == PLUS &&                // (4,ax)
+    XEXP(XEXP(operands[3], 0), 0) == XEXP(operands[1], 0) && // same reg used
+    GET_CODE(XEXP(XEXP(operands[3], 0), 1)) == CONST_INT &&  // addend is int 
+    INTVAL(XEXP(XEXP(operands[3], 0), 1)) == 4) ||           // offset is 4
+   (GET_CODE(XEXP(operands[1], 0)) == PLUS &&
+    ADDRESS_REG_P(XEXP(XEXP(operands[1], 0), 0)) && 
+    GET_CODE(XEXP(XEXP(operands[1], 0), 1)) == CONST_INT &&
+    GET_CODE(XEXP(operands[3], 0)) == PLUS &&
+    XEXP(XEXP(operands[3], 0), 0) == XEXP(XEXP(operands[1], 0), 0) &&
+    GET_CODE(XEXP(XEXP(operands[3], 0), 1)) == CONST_INT &&
+    INTVAL(XEXP(XEXP(operands[3], 0), 1)) == 4 + INTVAL(XEXP(XEXP(operands[1], 0), 1))))"
+  [(parallel[
+     (set (match_dup 0) (match_dup 1))
+     (set (match_dup 2) (match_dup 3))
+   ])]
+   {})
+   
+;; combine two move.l into move2.l r1:r2,(n,ax)
+(define_peephole2
+  [(set (match_operand:SI 1 "memory_operand" "")
+        (match_operand:SI 0 "register_operand" ""))
+   (set (match_operand:SI 3 "memory_operand" "")
+        (match_operand:SI 2 "register_operand" ""))]
+  "TUNE_68080 && reload_completed && flag_experimental && REGNO(operands[0]) < 16 && REGNO(operands[2]) < 16 &&
+   ((ADDRESS_REG_P(XEXP(operands[1], 0)) && 
+    GET_CODE(XEXP(operands[3], 0)) == PLUS &&
+    XEXP(XEXP(operands[3], 0), 0) == XEXP(operands[1],0) &&
+    GET_CODE(XEXP(XEXP(operands[3], 0), 1)) == CONST_INT &&
+    INTVAL(XEXP(XEXP(operands[3], 0), 1)) == 4) ||
+   (GET_CODE(XEXP(operands[1], 0)) == PLUS &&
+    ADDRESS_REG_P(XEXP(XEXP(operands[1], 0), 0)) && 
+    GET_CODE(XEXP(XEXP(operands[1], 0), 1)) == CONST_INT &&
+    GET_CODE(XEXP(operands[3], 0)) == PLUS &&
+    XEXP(XEXP(operands[3], 0), 0) == XEXP(XEXP(operands[1], 0), 0) &&
+    GET_CODE(XEXP(XEXP(operands[3], 0), 1)) == CONST_INT &&
+    INTVAL(XEXP(XEXP(operands[3], 0), 1)) == 4 + INTVAL(XEXP(XEXP(operands[1], 0), 1))))"
+  [(parallel[
+     (set (match_dup 1) (match_dup 0))
+     (set (match_dup 3) (match_dup 2))
+   ])]
+   {})

--- gcc/config/m68k/m68k-microarchs.def	2016-01-04 14:30:50.000000000 +0000
+++ gcc/config/m68k/m68k-microarchs.def	2025-11-02 00:56:37.886277413 +0000
@@ -33,9 +33,11 @@ M68K_MICROARCH ("68010",    m68010,  680
 M68K_MICROARCH ("68020",    m68020,  68020,    isa_20,  FL_FOR_isa_20)
 M68K_MICROARCH ("68020-40", m68020,  68020_40, isa_20,  FL_FOR_isa_20)
 M68K_MICROARCH ("68020-60", m68020,  68020_60, isa_20,  FL_FOR_isa_20)
+M68K_MICROARCH ("68020-80", m68020,  68020_80, isa_20,  FL_FOR_isa_20)
 M68K_MICROARCH ("68030",    m68030,  68030,    isa_20,  FL_FOR_isa_20)
 M68K_MICROARCH ("68040",    m68040,  68040,    isa_40,  FL_FOR_isa_40)
-M68K_MICROARCH ("68060",    m68060,  68060,    isa_40,  FL_FOR_isa_40)
+M68K_MICROARCH ("68060",    m68060,  68060,    isa_60,  FL_FOR_isa_60)
+M68K_MICROARCH ("68080",    m68080,  68080,    isa_80,  FL_FOR_isa_80)
 M68K_MICROARCH ("cpu32",    cpu32,   cpu32,    isa_20,  FL_FOR_isa_cpu32)
 M68K_MICROARCH ("cfv1",     mcf51qe, cfv1,     isa_c,   FL_FOR_isa_c)
 M68K_MICROARCH ("cfv2",     mcf5206, cfv2,     isa_a,   FL_FOR_isa_a)

--- gcc/config/m68k/m68k.opt	2016-01-04 14:30:50.000000000 +0000
+++ gcc/config/m68k/m68k.opt	2025-11-02 00:56:37.886277413 +0000
@@ -77,6 +77,10 @@ m68060
 Target RejectNegative Alias(mcpu=, 68060)
 Generate code for a 68060.
 
+m68080
+Target RejectNegative Alias(mcpu=, 68080)
+Generate code for a 68080.
+
 m68302
 Target RejectNegative Alias(mcpu=, 68302)
 Generate code for a 68302.
@@ -193,3 +197,56 @@ Support more than 8192 GOT entries on Co
 mxtls
 Target Report Mask(XTLS)
 Support TLS segment larger than 64K.
+
+fno-truncz-same-mode
+Target RejectNegative Var(flag_no_truncz_same_mode,1)
+emit round to zero insns (e.g. fintrz.x fp0,fp0)
+
+mschulz
+Target RejectNegative Var(flag_no_x_mode,1)
+Don't use fmove.x, used fmove.d instead
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
+Target Var(flag_double_indirect) Init(0)
+Use double indirect addressing mode.
+
+fbbb=
+Target RejectNegative Report Var(help_columns,80) Var(string_bbb_opts) Joined 
+Enable Bebbo's optimizations.\n
++    enable all optimizations (default)\n
+a    commute add move instructions\n
+b    use register for base addresses\n
+c    convert load const and compare into a sub\n
+e    eliminate dead assignments + redundant loads\n
+f    shrink stack frame\n
+h    optimize shift instructions\n
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
+Default: -fbbb=+ which yields -fbbb=abcefhilmnprsz0
+
+
+mexperimental
+Target RejectNegative Var(flag_experimental,1)
+experimental code, e.g. using new 68080 instructions, registers, etc.p.p.

--- gcc/config/m68k/m68k-tables.opt	2016-01-04 14:30:50.000000000 +0000
+++ gcc/config/m68k/m68k-tables.opt	2025-11-02 00:56:37.886277413 +0000
@@ -43,6 +43,9 @@ EnumValue
 Enum(target_device) String(68060) Value(m68060)
 
 EnumValue
+Enum(target_device) String(68080) Value(m68080)
+
+EnumValue
 Enum(target_device) String(68302) Value(m68302)
 
 EnumValue
@@ -380,6 +383,9 @@ EnumValue
 Enum(uarch_type) String(68020-60) Value(u68020_60)
 
 EnumValue
+Enum(uarch_type) String(68020-80) Value(u68020_80)
+
+EnumValue
 Enum(uarch_type) String(68030) Value(u68030)
 
 EnumValue
@@ -389,6 +395,9 @@ EnumValue
 Enum(uarch_type) String(68060) Value(u68060)
 
 EnumValue
+Enum(uarch_type) String(68080) Value(u68080)
+
+EnumValue
 Enum(uarch_type) String(cpu32) Value(ucpu32)
 
 EnumValue
@@ -429,17 +438,20 @@ EnumValue
 Enum(m68k_isa) String(68060) Value(5)
 
 EnumValue
-Enum(m68k_isa) String(cpu32) Value(6)
+Enum(m68k_isa) String(68080) Value(6)
+
+EnumValue
+Enum(m68k_isa) String(cpu32) Value(7)
 
 EnumValue
-Enum(m68k_isa) String(isaa) Value(7)
+Enum(m68k_isa) String(isaa) Value(8)
 
 EnumValue
-Enum(m68k_isa) String(isaaplus) Value(8)
+Enum(m68k_isa) String(isaaplus) Value(9)
 
 EnumValue
-Enum(m68k_isa) String(isab) Value(9)
+Enum(m68k_isa) String(isab) Value(10)
 
 EnumValue
-Enum(m68k_isa) String(isac) Value(10)
+Enum(m68k_isa) String(isac) Value(11)
 

--- gcc/config/m68k/math-68881.h	2004-02-09 00:48:13.000000000 +0000
+++ gcc/config/m68k/math-68881.h	2025-11-02 00:56:37.890277473 +0000
@@ -37,7 +37,7 @@
    September 1993, Use #undef before HUGE_VAL instead of #ifdef/#endif.  */
 
 /* Changed by Ian Lance Taylor:
-   September 1994, use extern inline instead of static inline.  */
+   September 1994, use inline instead of static inline.  */
 
 #ifndef __math_68881
 #define __math_68881
@@ -64,7 +64,7 @@
 })
 #endif
 
-__inline extern double
+__inline double
 sin (double x)
 {
   double value;
@@ -75,7 +75,7 @@ sin (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 cos (double x)
 {
   double value;
@@ -86,7 +86,7 @@ cos (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 tan (double x)
 {
   double value;
@@ -97,7 +97,7 @@ tan (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 asin (double x)
 {
   double value;
@@ -108,7 +108,7 @@ asin (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 acos (double x)
 {
   double value;
@@ -119,7 +119,7 @@ acos (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 atan (double x)
 {
   double value;
@@ -130,7 +130,7 @@ atan (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 atan2 (double y, double x)
 {
   double pi, pi_over_2;
@@ -187,7 +187,7 @@ atan2 (double y, double x)
     }
 }
 
-__inline extern double
+__inline double
 sinh (double x)
 {
   double value;
@@ -198,7 +198,7 @@ sinh (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 cosh (double x)
 {
   double value;
@@ -209,7 +209,7 @@ cosh (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 tanh (double x)
 {
   double value;
@@ -220,7 +220,7 @@ tanh (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 atanh (double x)
 {
   double value;
@@ -231,7 +231,7 @@ atanh (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 exp (double x)
 {
   double value;
@@ -242,7 +242,7 @@ exp (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 expm1 (double x)
 {
   double value;
@@ -253,7 +253,7 @@ expm1 (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 log (double x)
 {
   double value;
@@ -264,7 +264,7 @@ log (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 log1p (double x)
 {
   double value;
@@ -275,7 +275,7 @@ log1p (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 log10 (double x)
 {
   double value;
@@ -286,7 +286,7 @@ log10 (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 sqrt (double x)
 {
   double value;
@@ -297,13 +297,13 @@ sqrt (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 hypot (double x, double y)
 {
   return sqrt (x*x + y*y);
 }
 
-__inline extern double
+__inline double
 pow (double x, double y)
 {
   if (x > 0)
@@ -352,7 +352,7 @@ pow (double x, double y)
     }
 }
 
-__inline extern double
+__inline double
 fabs (double x)
 {
   double value;
@@ -363,7 +363,7 @@ fabs (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 ceil (double x)
 {
   int rounding_mode, round_up;
@@ -385,7 +385,7 @@ ceil (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 floor (double x)
 {
   int rounding_mode, round_down;
@@ -408,7 +408,7 @@ floor (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 rint (double x)
 {
   int rounding_mode, round_nearest;
@@ -430,7 +430,7 @@ rint (double x)
   return value;
 }
 
-__inline extern double
+__inline double
 fmod (double x, double y)
 {
   double value;
@@ -442,7 +442,7 @@ fmod (double x, double y)
   return value;
 }
 
-__inline extern double
+__inline double
 drem (double x, double y)
 {
   double value;
@@ -454,7 +454,7 @@ drem (double x, double y)
   return value;
 }
 
-__inline extern double
+__inline double
 scalb (double x, int n)
 {
   double value;
@@ -466,7 +466,7 @@ scalb (double x, int n)
   return value;
 }
 
-__inline extern double
+__inline double
 logb (double x)
 {
   double exponent;
@@ -477,7 +477,7 @@ logb (double x)
   return exponent;
 }
 
-__inline extern double
+__inline double
 ldexp (double x, int n)
 {
   double value;
@@ -489,7 +489,7 @@ ldexp (double x, int n)
   return value;
 }
 
-__inline extern double
+__inline double
 frexp (double x, int *exp)
 {
   double float_exponent;
@@ -514,7 +514,7 @@ frexp (double x, int *exp)
   return mantissa;
 }
 
-__inline extern double
+__inline double
 modf (double x, double *ip)
 {
   double temp;

--- gcc/config/m68k/predicates.md	2016-01-20 07:54:26.000000000 +0000
+++ gcc/config/m68k/predicates.md	2025-11-02 00:56:37.890277473 +0000
@@ -30,6 +30,10 @@
 	  || GET_CODE (XEXP (op, 0)) == LABEL_REF
 	  || GET_CODE (XEXP (op, 0)) == CONST))
     return 1;
+#if defined(TARGET_AMIGAOS)
+  if (flag_pic >= 3 && amiga_is_const_pic_ref(op))
+    return 0;
+#endif
   return general_operand (op, mode);
 })
 
@@ -150,9 +154,10 @@
 
     case CONST:
       op = XEXP (op, 0);
-      return ((GET_CODE (XEXP (op, 0)) == SYMBOL_REF
-	       || GET_CODE (XEXP (op, 0)) == LABEL_REF)
-	      && GET_CODE (XEXP (op, 1)) == CONST_INT);
+      return (GET_CODE(op) == PLUS &&
+               (GET_CODE (XEXP (op, 0)) == SYMBOL_REF
+	         || GET_CODE (XEXP (op, 0)) == LABEL_REF)
+	       && GET_CODE (XEXP (op, 1)) == CONST_INT);
 
 #if 0 /* Deleted, with corresponding change in m68k.h,
 	 so as to fit the specs.  No CONST_DOUBLE is ever symbolic.  */
@@ -169,7 +174,14 @@
 (define_predicate "const_call_operand"
   (ior (match_operand 0 "const_int_operand")
        (and (match_test "m68k_symbolic_call != NULL")
-	    (match_operand 0 "symbolic_operand"))))
+	        (match_operand 0 "symbolic_operand")
+	   )
+       (and (match_code "plus")
+            (match_test "REG_P(XEXP(op, 0)) && REGNO(XEXP(op, 0)) >= 8 && REGNO(XEXP(op, 0)) < 16")
+            (match_test "GET_CODE(XEXP(op, 1)) == CONST_INT")
+       )
+  )
+)
 
 ;; An operand that can be used as the address in a call insn.
 (define_predicate "call_operand"
@@ -260,7 +272,7 @@
 (define_predicate "pow2_m1_operand"
   (match_code "const_int")
 {
-  return (GET_CODE (op) == CONST_INT && exact_log2 (INTVAL (op) + 1) >= 0);
+  return (GET_CODE (op) == CONST_INT && exact_log2 (INTVAL (op) + 1) > 0);
 })
 
 ;; Used to detect valid targets for conditional branches
Only in ../gcc/gcc/config/m68k: t-amigaos

--- gcc/config/m68k/t-linux	2016-01-04 14:30:50.000000000 +0000
+++ gcc/config/m68k/t-linux	2025-11-02 00:56:37.890277473 +0000
@@ -29,3 +29,23 @@ sysroot-suffix.h: $(srcdir)/config/m68k/
 	$(SHELL) $(srcdir)/config/m68k/print-sysroot-suffix.sh \
 	  "$(SYSTEM_HEADER_DIR)/../.." "$(MULTILIB_MATCHES)" \
 	  "$(MULTILIB_OPTIONS)" > $@
+
+EXTRA_OBJS += m68k2.o m68k_68000_10_costs.o m68k_68020_costs.o m68k_68030_costs.o m68k_68040_costs.o m68k_68080_costs.o
+
+m68k2.o: $(srcdir)/config/m68k/m68k2.c $(CONFIG_H)
+	$(CXX) -c $(ALL_CFLAGS) $(ALL_CPPFLAGS) $(INCLUDES) $< $(OUTPUT_OPTION)
+
+m68k_68000_10_costs.o: $(srcdir)/config/m68k/m68k_68000_10_costs.c $(CONFIG_H)
+	$(CXX) -c $(ALL_CFLAGS) $(ALL_CPPFLAGS) $(INCLUDES) $< $(OUTPUT_OPTION)
+
+m68k_68020_costs.o: $(srcdir)/config/m68k/m68k_68020_costs.c $(CONFIG_H)
+	$(CXX) -c $(ALL_CFLAGS) $(ALL_CPPFLAGS) $(INCLUDES) $< $(OUTPUT_OPTION)
+
+m68k_68030_costs.o: $(srcdir)/config/m68k/m68k_68030_costs.c $(CONFIG_H)
+	$(CXX) -c $(ALL_CFLAGS) $(ALL_CPPFLAGS) $(INCLUDES) $< $(OUTPUT_OPTION)
+
+m68k_68040_costs.o: $(srcdir)/config/m68k/m68k_68040_costs.c $(CONFIG_H)
+	$(CXX) -c $(ALL_CFLAGS) $(ALL_CPPFLAGS) $(INCLUDES) $< $(OUTPUT_OPTION)
+
+m68k_68080_costs.o: $(srcdir)/config/m68k/m68k_68080_costs.c $(CONFIG_H)
+	$(CXX) -c $(ALL_CFLAGS) $(ALL_CPPFLAGS) $(INCLUDES) $< $(OUTPUT_OPTION)
\ No newline at end of file
Only in ../gcc/gcc/config/m68k: t-m68kelf
Only in ../gcc/gcc/config/m68k: x-amigaos
Only in ../gcc/gcc/config/m68k: xm-amigaos.h

--- gcc/config/xtensa/uclinux.h	2016-01-04 14:30:50.000000000 +0000
+++ gcc/config/xtensa/uclinux.h	2025-11-02 00:56:38.022279472 +0000
@@ -59,8 +59,8 @@ along with GCC; see the file COPYING3.
 #undef LOCAL_LABEL_PREFIX
 #define LOCAL_LABEL_PREFIX	"."
 
-/* Always enable "-fpic" for Xtensa Linux.  */
-#define XTENSA_ALWAYS_PIC 1
+/* Don't enable "-fpic" for Xtensa uclinux.  */
+#define XTENSA_ALWAYS_PIC 0
 
 #undef TARGET_LIBC_HAS_FUNCTION
 #define TARGET_LIBC_HAS_FUNCTION no_c99_libc_has_function

--- gcc/config.gcc	2017-11-21 09:41:34.000000000 +0000
+++ gcc/config.gcc	2025-11-02 00:56:37.746275293 +0000
@@ -1940,6 +1940,28 @@ m68k-*-elf* | fido-*-elf*)
 		;;
 	esac
 	;;
+m68k*-*-amigaosvasm*)
+	default_m68k_cpu=68000
+	tm_file="${tm_file}  dbx.h newlib-stdint.h m68k/m68kamigaos.h"
+	tm_defines="${tm_defines} MOTOROLA=1 TARGET_AMIGAOS TARGET_AMIGAOS_VASM TARGET_CPU_DEFAULT=0"
+	tmake_file="m68k/t-floatlib m68k/t-m68kbare m68k/t-amigaos"
+	tm_p_file="${tm_p_file} m68k/amigaos-protos.h"
+	extra_options="${extra_options} m68k/amigaos.opt"
+	gnu_ld=yes
+	;;
+m68k*-*-amigaos*)
+	default_m68k_cpu=68000
+	tm_file="${tm_file}  dbx.h newlib-stdint.h m68k/m68kamigaos.h"
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

--- gcc/config.host	2017-01-09 21:01:26.000000000 +0000
+++ gcc/config.host	2025-11-02 00:56:37.746275293 +0000
@@ -253,6 +253,10 @@ case ${host} in
     out_host_hook_obj="${out_host_hook_obj} host-i386-darwin.o"
     host_xmake_file="${host_xmake_file} i386/x-darwin"
     ;;
+  arm-*-darwin*)
+    out_host_hook_obj="${out_host_hook_obj} host-aarch64-darwin.o"
+    host_xmake_file="${host_xmake_file} aarch64/x-darwin"
+    ;;
   powerpc-*-darwin*)
     out_host_hook_obj="${out_host_hook_obj} host-ppc-darwin.o"
     host_xmake_file="${host_xmake_file} rs6000/x-darwin"

--- gcc/configure	2018-03-08 08:55:59.000000000 +0000
+++ gcc/configure	2025-11-02 00:56:38.026279532 +0000
@@ -11846,11 +11846,20 @@ EOF
 gcc_config_arguments_str=`cat conftest.out`
 rm -f conftest.out
 
+if test x$thread_file = xposix
+then
+	posix_def=1
+else
+    posix_def=0
+fi
+
 cat > configargs.h <<EOF
 /* Generated automatically. */
 static const char configuration_arguments[] = "$gcc_config_arguments_str";
 static const char thread_model[] = "$thread_file";
 
+#define __POSIX_THREADS__ $posix_def
+
 static const struct {
   const char *name, *value;
 } configure_default_options[] = $configure_default_options;
@@ -29424,9 +29433,15 @@ else
   PICFLAG=
 fi
 
-
-
-
+case $host_os in
+    amigaos*)
+      case $host_cpu in
+      m68k)
+        PICFLAG=
+      ;;
+    esac
+  ;;
+esac
 
 # Check whether --enable-libquadmath-support was given.
 if test "${enable_libquadmath_support+set}" = set; then :

--- gcc/configure.ac	2018-03-08 08:55:59.000000000 +0000
+++ gcc/configure.ac	2025-11-02 00:56:38.026279532 +0000
@@ -1733,11 +1733,20 @@ EOF
 gcc_config_arguments_str=`cat conftest.out`
 rm -f conftest.out
 
+if test x$thread_file = xposix
+then
+	posix_def=1
+else
+    posix_def=0
+fi
+
 cat > configargs.h <<EOF
 /* Generated automatically. */
 static const char configuration_arguments[] = "$gcc_config_arguments_str";
 static const char thread_model[] = "$thread_file";
 
+#define __POSIX_THREADS__ $posix_def
+
 static const struct {
   const char *name, *value;
 } configure_default_options[] = $configure_default_options;

--- gcc/coretypes.h	2016-01-04 14:30:50.000000000 +0000
+++ gcc/coretypes.h	2025-11-02 00:56:38.030279593 +0000
@@ -52,9 +52,9 @@ typedef const struct bitmap_head *const_
 struct simple_bitmap_def;
 typedef struct simple_bitmap_def *sbitmap;
 typedef const struct simple_bitmap_def *const_sbitmap;
-struct rtx_def;
-typedef struct rtx_def *rtx;
-typedef const struct rtx_def *const_rtx;
+class rtx_def;
+typedef class rtx_def *rtx;
+typedef const class rtx_def *const_rtx;
 
 /* Subclasses of rtx_def, using indentation to show the class
    hierarchy, along with the relevant invariant.

--- gcc/cp/cp-gimplify.c	2018-10-12 18:27:56.000000000 +0100
+++ gcc/cp/cp-gimplify.c	2025-11-02 00:56:38.050279895 +0000
@@ -1946,6 +1946,11 @@ cp_fold_maybe_rvalue (tree x, bool rval)
       x = cp_fold (x);
       if (rval && DECL_P (x))
 	{
+#if defined(TARGET_M68K)
+/* SBF: do not fold on m68k. */
+	  if (VAR_P (x))
+	    break;
+#endif
 	  tree v = decl_constant_value (x);
 	  if (v != x && v != error_mark_node)
 	    {

--- gcc/cp/decl.c	2018-06-25 18:33:40.000000000 +0100
+++ gcc/cp/decl.c	2025-11-02 00:56:38.058280017 +0000
@@ -3956,6 +3956,9 @@ initialize_predefined_identifiers (void)
   const predefined_identifier *pid;
 
   /* A table of identifiers to create at startup.  */
+#ifdef __amiga__
+__near
+#endif
   static const predefined_identifier predefined_identifiers[] = {
     { "C++", &lang_name_cplusplus, 0 },
     { "C", &lang_name_c, 0 },
@@ -6718,6 +6721,12 @@ cp_finish_decl (tree decl, tree init, bo
 	}
     }
 
+  if (VAR_P (decl) && DECL_REGISTER (decl) && asmspec)
+	{
+	  set_user_assembler_name (decl, asmspec);
+	  DECL_HARD_REGISTER (decl) = 1;
+	}
+
   if (processing_template_decl)
     {
       bool type_dependent_p;
@@ -7909,6 +7918,23 @@ grokfndecl (tree ctype,
   int staticp = ctype && TREE_CODE (type) == FUNCTION_TYPE;
   tree t;
 
+#if defined(TARGET_AMIGAOS)
+  /* SBF: Add support for asm("xy") register spec. */
+  if (m68k_regparm > 0)
+    {
+      tree asm1 = lookup_attribute("asmregs", *attrlist);
+      tree stack1 = lookup_attribute("stkparm", *attrlist);
+      tree reg1 = lookup_attribute("regparm", *attrlist);
+      if (!asm1 && !stack1 && !reg1)
+	{
+	  tree ttasm = get_identifier("regparm");
+	  tree value = tree_cons(ttasm, build_int_cst(NULL, m68k_regparm), NULL_TREE);
+	  tree attrs = tree_cons(ttasm, value, NULL_TREE);
+	  *attrlist = chainon(attrs, *attrlist);
+	}
+    }
+#endif
+
   // Was the concept specifier present?
   bool concept_p = inlinep & 4;
 

--- gcc/cp/error.c	2017-06-24 11:50:08.000000000 +0100
+++ gcc/cp/error.c	2025-11-02 00:56:38.058280017 +0000
@@ -40,6 +40,9 @@ along with GCC; see the file COPYING3.
    debugger or by the front-end for things like
    __PRETTY_FUNCTION__.  */
 static cxx_pretty_printer actual_pretty_printer;
+#ifdef __amiga__
+__near
+#endif
 static cxx_pretty_printer * const cxx_pp = &actual_pretty_printer;
 
 /* Translate if being used for diagnostics, but not for dump files or

--- gcc/cp/g++spec.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/cp/g++spec.c	2025-11-02 00:56:38.058280017 +0000
@@ -262,7 +262,7 @@ lang_specific_driver (struct cl_decoded_
 #endif
 
   /* Add one for shared_libgcc or extra static library.  */
-  num_args = argc + added + need_math + (library > 0) * 4 + 1;
+  num_args = argc + added + need_math + (library > 0) * 5 + 1;
   new_decoded_options = XNEWVEC (struct cl_decoded_option, num_args);
 
   i = 0;
@@ -342,11 +342,41 @@ lang_specific_driver (struct cl_decoded_
 	  j++;
 	}
 #endif
+
+#if defined(TARGET_AMIGAOS)
+/* SBF: force linking __init_eh and replace new operator. */
+	{
+	  bool addglue = true;
+	  /* do not add glue if exceptions are disabled. */
+	  for (int ii = 0; ii < argc; ++ii)
+	    {
+	      if (decoded_options[ii].opt_index == OPT_fexceptions)
+		addglue = decoded_options[ii].value;
+	    }
+	  {
+	    extern const char *
+	    amiga_m68k_prefix_func (int argc, const char ** argv);
+	    if (addglue)
+	      {
+		generate_option (OPT_Wl_, "-u,___init_eh", 1, CL_DRIVER,
+					   &new_decoded_options[j]);
+	      }
+	    else
+	      {
+		generate_option (OPT_Wl_, "-l,new_op", 1, CL_DRIVER,
+					   &new_decoded_options[j]);
+	      }
+	    ++j;
+	  }
+	}
+#endif
+
       generate_option (OPT_l,
 		       saw_profile_flag ? LIBSTDCXX_PROFILE : LIBSTDCXX, 1,
 		       CL_DRIVER, &new_decoded_options[j]);
       added_libraries++;
       j++;
+
       /* Add target-dependent static library, if necessary.  */
       if ((static_link || library > 1) && LIBSTDCXX_STATIC != NULL)
 	{

--- gcc/cp/mangle.c	2016-12-07 22:51:48.000000000 +0000
+++ gcc/cp/mangle.c	2025-11-02 00:56:38.062280077 +0000
@@ -3655,8 +3655,14 @@ mangle_decl (const tree decl)
     id = get_identifier ("<anon>");
   else
     {
+      if (!(TREE_CODE (decl) != TYPE_DECL
+		  || !no_linkage_check (TREE_TYPE (decl), true)))
+	no_linkage_check (TREE_TYPE (decl), true);
+
       gcc_assert (TREE_CODE (decl) != TYPE_DECL
-		  || !no_linkage_check (TREE_TYPE (decl), true));
+		  || !no_linkage_check (TREE_TYPE (decl), true)
+		  || (TYPE_STUB_DECL (TREE_TYPE (decl))
+		      && TREE_PUBLIC (TYPE_STUB_DECL (TREE_TYPE (decl)))));
       if (abi_version_at_least (10))
 	if (tree fn = decl_function_context (decl))
 	  maybe_check_abi_tags (fn, decl);

--- gcc/cp/parser.c	2018-06-26 12:33:48.000000000 +0100
+++ gcc/cp/parser.c	2025-11-02 00:56:38.070280198 +0000
@@ -20570,6 +20570,11 @@ cp_parser_parameter_declaration (cp_pars
 	}
     }
 
+#if defined(TARGET_AMIGAOS)
+  /* SBF: Add support for asm("xy") register spec. */
+    tree pasmspec = cp_parser_asm_specification_opt (parser);
+#endif
+
   /* If the next token is an ellipsis, and we have not seen a declarator
      name, and if either the type of the declarator contains parameter
      packs but it is not a TYPE_PACK_EXPANSION or is null (this happens
@@ -20674,6 +20679,44 @@ cp_parser_parameter_declaration (cp_pars
   else
     default_argument = NULL_TREE;
 
+#if defined(TARGET_AMIGAOS)
+  /* SBF: Add support for asm("xy") register spec. */
+  if (pasmspec)
+    {
+      const char *asmspec = TREE_STRING_POINTER(pasmspec);
+      if (*asmspec == '%')
+        ++asmspec;
+      int offset = 1;
+      int reg_number = -1;
+      if (asmspec[0] == 'd')
+        reg_number = 0;
+      else if (asmspec[0] == 'a')
+        reg_number = 8;
+      else if (asmspec[0] == 'f' && asmspec[1] == 'p')
+        {
+          reg_number = 16;
+          offset = 2;
+        }
+      unsigned add = asmspec[offset] - '0';
+      if (reg_number < 0 || add > 7)
+	    error("invalid register specified %s", asmspec);
+      reg_number += add;
+
+  /* Build tree for __attribute__ ((asmreg(regnum))). */
+      tree ttasm = get_identifier("asmreg");
+      tree value = tree_cons(ttasm, build_int_cst(NULL, reg_number), NULL_TREE);
+      tree attrs = tree_cons(ttasm, value, NULL_TREE);
+
+      /* search outmost declarator, e.g. int * needs the attribute at the pointer not the int. */
+      if (declarator) {
+	cp_declarator * d = declarator;
+	while (d->kind != cdk_id && d->declarator)
+	  d = d->declarator;
+	d->attributes = chainon(attrs, d->attributes);
+      }
+    }
+#endif
+
   return make_parameter_declarator (&decl_specifiers,
 				    declarator,
 				    default_argument,

--- gcc/cp/pt.c	2018-06-25 18:42:51.000000000 +0100
+++ gcc/cp/pt.c	2025-11-02 00:56:38.074280259 +0000
@@ -15303,6 +15303,7 @@ tsubst_expr (tree t, tree args, tsubst_f
 		else
 		  {
 		    int const_init = false;
+		    tree asmspec_tree = NULL_TREE;
 		    maybe_push_decl (decl);
 		    if (VAR_P (decl)
 			&& DECL_PRETTY_FUNCTION_P (decl))
@@ -15319,7 +15320,19 @@ tsubst_expr (tree t, tree args, tsubst_f
 		    if (VAR_P (decl))
 		      const_init = (DECL_INITIALIZED_BY_CONSTANT_EXPRESSION_P
 				    (pattern_decl));
-		    cp_finish_decl (decl, init, const_init, NULL_TREE, 0);
+
+		    if (VAR_P (decl) && DECL_HARD_REGISTER (pattern_decl))
+		      {
+			tree id = DECL_ASSEMBLER_NAME (pattern_decl);
+			const char *asmspec = IDENTIFIER_POINTER (id);
+			gcc_assert (asmspec[0] == '*');
+			asmspec_tree
+			  = build_string (IDENTIFIER_LENGTH (id) - 1,
+					  asmspec + 1);
+			TREE_TYPE (asmspec_tree) = char_array_type_node;
+		      }
+
+		    cp_finish_decl (decl, init, const_init, asmspec_tree, 0);
 		  }
 	      }
 	  }

--- gcc/cp/rtti.c	2016-02-19 19:16:31.000000000 +0000
+++ gcc/cp/rtti.c	2025-11-02 00:56:38.074280259 +0000
@@ -1511,6 +1511,9 @@ emit_support_tinfos (void)
 {
   /* Dummy static variable so we can put nullptr in the array; it will be
      set before we actually start to walk the array.  */
+#ifdef __amiga__
+__near
+#endif
   static tree *const fundamentals[] =
   {
     &void_type_node,

--- gcc/cprop.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/cprop.c	2025-11-02 00:56:38.082280380 +0000
@@ -735,7 +735,7 @@ try_replace_reg (rtx from, rtx to, rtx_i
   bool check_rtx_costs = true;
   bool speed = optimize_bb_for_speed_p (BLOCK_FOR_INSN (insn));
   int old_cost = set ? set_rtx_cost (set, speed) : 0;
-
+#if 0
   if (!set
       || CONSTANT_P (SET_SRC (set))
       || (note != 0
@@ -743,7 +743,7 @@ try_replace_reg (rtx from, rtx to, rtx_i
 	  && (GET_CODE (XEXP (note, 0)) == CONST
 	      || CONSTANT_P (XEXP (note, 0)))))
     check_rtx_costs = false;
-
+#endif
   /* Usually we substitute easy stuff, so we won't copy everything.
      We however need to take care to not duplicate non-trivial CONST
      expressions.  */
@@ -1185,7 +1185,12 @@ do_local_cprop (rtx x, rtx_insn *insn)
   if (REG_P (x)
       && (cprop_reg_p (x)
           || (GET_CODE (PATTERN (insn)) != USE
-	      && asm_noperands (PATTERN (insn)) < 0)))
+	      && asm_noperands (PATTERN (insn)) < 0))
+      /* SBF: ignore regs marked as REG_INC. */
+      && !find_reg_note (insn, REG_INC, x)
+      /* SBF: don't replace hard regs. */
+      && !HARD_REGISTER_NUM_P (REGNO (x))
+    )
     {
       cselib_val *val = cselib_lookup (x, GET_MODE (x), 0, VOIDmode);
       struct elt_loc_list *l;

--- gcc/cse.c	2016-02-04 09:56:13.000000000 +0000
+++ gcc/cse.c	2025-11-02 00:56:38.082280380 +0000
@@ -2829,6 +2829,8 @@ canon_reg (rtx x, rtx_insn *insn)
     case LABEL_REF:
     case ADDR_VEC:
     case ADDR_DIFF_VEC:
+    case POST_INC:
+    case PRE_DEC:
       return x;
 
     case REG:
@@ -3635,12 +3637,10 @@ fold_rtx (rtx x, rtx_insn *insn)
 	      inner_const = equiv_constant (fold_rtx (XEXP (y, 1), 0));
 	      if (!inner_const || !CONST_INT_P (inner_const))
 		break;
-
 	      /* Don't associate these operations if they are a PLUS with the
 		 same constant and it is a power of two.  These might be doable
 		 with a pre- or post-increment.  Similarly for two subtracts of
 		 identical powers of two with post decrement.  */
-
 	      if (code == PLUS && const_arg1 == inner_const
 		  && ((HAVE_PRE_INCREMENT
 			  && exact_log2 (INTVAL (const_arg1)) >= 0)
@@ -3650,8 +3650,11 @@ fold_rtx (rtx x, rtx_insn *insn)
 			  && exact_log2 (- INTVAL (const_arg1)) >= 0)
 		      || (HAVE_POST_DECREMENT
 			  && exact_log2 (- INTVAL (const_arg1)) >= 0)))
-		break;
-
+		{
+		  /* SBF: fold if defined once and multiple uses. */
+		  if (DF_REG_USE_COUNT(REGNO(folded_arg0)) <= 2 || DF_REG_DEF_COUNT(REGNO(folded_arg0)) > 1)
+		    break;
+		}
 	      /* ??? Vector mode shifts by scalar
 		 shift operand are not supported yet.  */
 	      if (is_shift && VECTOR_MODE_P (mode))
@@ -5422,7 +5425,9 @@ cse_insn (rtx_insn *insn)
 
 	  /* Record the actual constant value in a REG_EQUAL note,
 	     making a new one if one does not already exist.  */
-	  set_unique_reg_note (insn, REG_EQUAL, src_const);
+	  /* SBF: ignore regs marked as REG_INC. */
+	  if (!find_reg_note(insn, REG_INC, dest))
+	    set_unique_reg_note (insn, REG_EQUAL, src_const);
 	  df_notes_rescan (insn);
 	}
 
@@ -5875,14 +5880,20 @@ cse_insn (rtx_insn *insn)
 	  dest = SUBREG_REG (XEXP (dest, 0));
 
 	if (REG_P (dest) || GET_CODE (dest) == SUBREG)
-	  /* Registers must also be inserted into chains for quantities.  */
-	  if (insert_regs (dest, sets[i].src_elt, 1))
-	    {
-	      /* If `insert_regs' changes something, the hash code must be
-		 recalculated.  */
-	      rehash_using_reg (dest);
-	      sets[i].dest_hash = HASH (dest, GET_MODE (dest));
-	    }
+	  {
+	    /* SBF: ignore regs marked as REG_INC. */
+	    if (find_reg_note (insn, REG_INC, dest))
+	      continue;
+
+	    /* Registers must also be inserted into chains for quantities.  */
+  	  if (insert_regs (dest, sets[i].src_elt, 1))
+	      {
+	        /* If `insert_regs' changes something, the hash code must be
+  		 recalculated.  */
+	        rehash_using_reg (dest);
+	        sets[i].dest_hash = HASH (dest, GET_MODE (dest));
+	      }
+	  }
 
 	elt = insert (dest, sets[i].src_elt,
 		      sets[i].dest_hash, GET_MODE (dest));

--- gcc/DATESTAMP	2018-10-26 01:16:31.000000000 +0100
+++ gcc/DATESTAMP	2025-11-02 00:56:37.270268087 +0000
@@ -1 +1 @@
-20181026
+251015095727

--- gcc/dbxout.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/dbxout.c	2025-11-02 00:56:38.086280440 +0000
@@ -247,6 +247,13 @@ static GTY(()) const char *lastfile;
    base_input_file.  */
 static GTY(()) int lastfile_is_base;
 
+#if defined(TARGET_AMIGAOS)
+static unsigned files_num;
+static unsigned files_size;
+static unsigned files_current;
+static const char ** files_name;
+#endif
+
 /* Typical USG systems don't have stab.h, and they also have
    no use for DBX-format debugging info.  */
 
@@ -836,6 +843,23 @@ dbxout_finish_complex_stabs (tree sym, s
 	  chunk += chunklen + 1;
 	  len   -= chunklen + 1;
 
+#if defined(TARGET_AMIGAOS)
+	  /* SBF: symbol is inside unspec */
+	  if (addr && GET_CODE(addr) == PLUS)
+	    {
+	      rtx x = XEXP(addr, 1);
+	      addr = 0;
+	      if (GET_CODE(x) == CONST)
+		{
+		  x = XEXP(x, 0);
+		  if (GET_CODE(x) == UNSPEC)
+		    {
+		      addr = XVECEXP (x, 0, 0);
+		    }
+		}
+	    }
+#endif
+
 	  /* Only put a line number on the last stab in the sequence.  */
 	  DBX_FINISH_STABS (sym, code, len == 0 ? line : 0,
 			    addr, label, number);
@@ -1037,7 +1061,8 @@ dbxout_init (const char *input_file_name
   dbxout_stab_value_zero ();
 #endif
 
-  base_input_file = lastfile = input_file_name;
+  dbxout_source_file(input_file_name);
+  base_input_file = input_file_name;
 
   next_type_number = 1;
 
@@ -1218,6 +1243,39 @@ static void dbxout_block (tree, int, tre
 static void
 dbxout_source_file (const char *filename)
 {
+
+#if defined TARGET_AMIGAOS && !defined TARGET_AMIGAOS_VASM
+  if (filename && (lastfile == 0 || strcmp (filename, lastfile)))
+    {
+      // search file name
+      unsigned pos;
+      for (pos = 0; pos < files_num; ++pos)
+	{
+	  if (0 == strcmp(filename, files_name[pos]))
+	    break;
+	}
+
+      files_current = pos + 1;
+
+      // store file name
+      if (pos == files_num)
+	{
+	  if (pos == files_size)
+	    {
+	      files_size += files_size + 4;
+	      files_name = (const char**)xrealloc(files_name, files_size * sizeof(char const *));
+	    }
+	  files_name[files_num++] = filename;
+
+	  fputs ("\t.file ", asm_out_file);
+	  fprint_ul (asm_out_file, files_current);
+	  fputs (" \"", asm_out_file);
+	  fputs (filename, asm_out_file);
+	  fputs ("\"\n", asm_out_file);
+	}
+    }
+#endif
+
   if (lastfile == 0 && lastfile_is_base)
     {
       lastfile = base_input_file;
@@ -1268,6 +1326,14 @@ dbxout_source_line (unsigned int lineno,
 {
   dbxout_source_file (filename);
 
+#if defined TARGET_AMIGAOS && !defined TARGET_AMIGAOS_VASM
+    fputs ("\t.loc ", asm_out_file);
+    fprint_ul (asm_out_file, files_current);
+    fputc (' ', asm_out_file);
+    fprint_ul (asm_out_file, lineno);
+    fputs (" 0\n", asm_out_file);
+#endif
+
 #ifdef DBX_OUTPUT_SOURCE_LINE
   DBX_OUTPUT_SOURCE_LINE (asm_out_file, lineno, dbxout_source_line_counter);
 #else
@@ -3793,7 +3859,13 @@ void
 default_stabs_asm_out_destructor (rtx symbol ATTRIBUTE_UNUSED,
 				  int priority ATTRIBUTE_UNUSED)
 {
-#if defined DBX_DEBUGGING_INFO || defined XCOFF_DEBUGGING_INFO
+#if defined(TARGET_AMIGAOS)
+  fprintf (asm_out_file,
+	   "\t.section\t.list___DTOR_LIST__\n"
+	   "\t.long\t_%s\n"
+	   "\t.text\n", XSTR (symbol, 0));
+switch_to_section (text_section);	   
+#elif defined DBX_DEBUGGING_INFO || defined XCOFF_DEBUGGING_INFO
   /* Tell GNU LD that this is part of the static destructor set.
      This will work for any system that uses stabs, most usefully
      aout systems.  */
@@ -3810,7 +3882,13 @@ void
 default_stabs_asm_out_constructor (rtx symbol ATTRIBUTE_UNUSED,
 				   int priority ATTRIBUTE_UNUSED)
 {
-#if defined DBX_DEBUGGING_INFO || defined XCOFF_DEBUGGING_INFO
+#if defined(TARGET_AMIGAOS)
+  fprintf (asm_out_file,
+	   "\t.section\t.list___CTOR_LIST__\n"
+	   "\t.long\t_%s\n"
+	   "\t.text\n", XSTR (symbol, 0));
+switch_to_section (text_section);	   
+#elif defined DBX_DEBUGGING_INFO || defined XCOFF_DEBUGGING_INFO
   /* Tell GNU LD that this is part of the static destructor set.
      This will work for any system that uses stabs, most usefully
      aout systems.  */

--- gcc/dce.c	2018-06-25 18:36:14.000000000 +0100
+++ gcc/dce.c	2025-11-02 00:56:38.086280440 +0000
@@ -123,7 +123,8 @@ deletable_insn_p (rtx_insn *insn, bool f
   /* If INSN sets a global_reg, leave it untouched.  */
   FOR_EACH_INSN_DEF (def, insn)
     if (HARD_REGISTER_NUM_P (DF_REF_REGNO (def))
-	&& global_regs[DF_REF_REGNO (def)])
+//	&& global_regs[DF_REF_REGNO (def)]
+		       )
       return false;
     /* Initialization of pseudo PIC register should never be removed.  */
     else if (DF_REF_REG (def) == pic_offset_table_rtx

--- gcc/df-scan.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/df-scan.c	2025-11-02 00:56:38.090280501 +0000
@@ -1807,6 +1807,12 @@ df_ref_change_reg_with_loc_1 (struct df_
 	  df_ref *ref_ptr;
 	  struct df_insn_info *insn_info = DF_REF_INSN_INFO (the_ref);
 
+	  if (DF_REF_FLAGS_IS_SET(the_ref, DF_HARD_REG_LIVE))
+	    {
+	      --df->hard_regs_live_count[DF_REF_REGNO(the_ref)];
+	      ++df->hard_regs_live_count[new_regno];
+	    }
+
 	  DF_REF_REGNO (the_ref) = new_regno;
 	  DF_REF_REG (the_ref) = regno_reg_rtx[new_regno];
 
Only in ../gcc/gcc: dllyang.c
Only in ../gcc/gcc: dllying.c
Only in ./gcc-6.5.0/gcc/doc: aot-compile.1
Only in ./gcc-6.5.0/gcc/doc: cpp.1
Only in ./gcc-6.5.0/gcc/doc: cpp.info
Only in ./gcc-6.5.0/gcc/doc: cppinternals.info
Only in ./gcc-6.5.0/gcc/doc: fsf-funding.7
Only in ./gcc-6.5.0/gcc/doc: g++.1
Only in ./gcc-6.5.0/gcc/doc: gc-analyze.1
Only in ./gcc-6.5.0/gcc/doc: gcc.1
Only in ./gcc-6.5.0/gcc/doc: gcc.info
Only in ./gcc-6.5.0/gcc/doc: gccinstall.info
Only in ./gcc-6.5.0/gcc/doc: gccint.info
Only in ./gcc-6.5.0/gcc/doc: gcj.1
Only in ./gcc-6.5.0/gcc/doc: gcj-dbtool.1
Only in ./gcc-6.5.0/gcc/doc: gcj.info
Only in ./gcc-6.5.0/gcc/doc: gcov.1
Only in ./gcc-6.5.0/gcc/doc: gcov-dump.1
Only in ./gcc-6.5.0/gcc/doc: gcov-tool.1
Only in ./gcc-6.5.0/gcc/doc: gfdl.7
Only in ./gcc-6.5.0/gcc/doc: gfortran.1
Only in ./gcc-6.5.0/gcc/doc: gij.1
Only in ./gcc-6.5.0/gcc/doc: gpl.7
Only in ./gcc-6.5.0/gcc/doc: grmic.1
Only in ./gcc-6.5.0/gcc/doc: jcf-dump.1
Only in ./gcc-6.5.0/gcc/doc: jv-convert.1
Only in ./gcc-6.5.0/gcc/doc: rebuild-gcj-db.1

--- gcc/dwarf2out.c	2018-06-25 18:44:31.000000000 +0100
+++ gcc/dwarf2out.c	2025-11-02 00:56:38.142281288 +0000
@@ -451,7 +451,7 @@ switch_to_eh_frame_section (bool back AT
 						       /*global=*/1);
 	  lsda_encoding = ASM_PREFERRED_EH_DATA_FORMAT (/*code=*/0,
 							/*global=*/0);
-	  flags = ((! flag_pic
+	  flags = (( (!flag_pic || flag_pic > 2)
 		    || ((fde_encoding & 0x70) != DW_EH_PE_absptr
 			&& (fde_encoding & 0x70) != DW_EH_PE_aligned
 			&& (per_encoding & 0x70) != DW_EH_PE_absptr
@@ -469,6 +469,26 @@ switch_to_eh_frame_section (bool back AT
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
@@ -2054,9 +2074,9 @@ output_loc_operands (dw_loc_descr_ref lo
 	/* Make sure the offset has been computed and that we can encode it as
 	   an operand.  */
 	gcc_assert (die_offset > 0
-		    && die_offset <= (loc->dw_loc_opc == DW_OP_call2)
+		    && die_offset <= ((loc->dw_loc_opc == DW_OP_call2)
 				     ? 0xffff
-				     : 0xffffffff);
+				     : 0xffffffff));
 	dw2_asm_output_data ((loc->dw_loc_opc == DW_OP_call2) ? 2 : 4,
 			     die_offset, NULL);
       }

--- gcc/emit-rtl.c	2018-06-25 18:39:11.000000000 +0100
+++ gcc/emit-rtl.c	2025-11-02 00:56:38.142281288 +0000
@@ -2123,7 +2123,11 @@ change_address_1 (rtx memref, machine_mo
   if (validate && !lra_in_progress)
     {
       if (reload_in_progress || reload_completed)
-	gcc_assert (memory_address_addr_space_p (mode, addr, as));
+	{
+	  bool r = memory_address_addr_space_p (mode, addr, as);
+	  if (!r) debug_rtx(addr);
+	  gcc_assert (r);
+	}
       else
 	addr = memory_address_addr_space (mode, addr, as);
     }

--- gcc/except.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/except.c	2025-11-02 00:56:38.142281288 +0000
@@ -142,6 +142,7 @@ along with GCC; see the file COPYING3.
 #include "cfgloop.h"
 #include "builtins.h"
 #include "tree-hash-traits.h"
+#include "target-def.h"
 
 static GTY(()) int call_site_base;
 
@@ -2850,14 +2851,14 @@ switch_to_exception_section (const char
 		 it linkonce if we have COMDAT groups to tie them together.  */
 	      if (DECL_COMDAT_GROUP (current_function_decl) && HAVE_COMDAT_GROUP)
 		flags |= SECTION_LINKONCE;
-	      sprintf (section_name, ".gcc_except_table.%s", fnname);
+	      sprintf (section_name, TARGET_GCC_EXCEPT_TABLE_S, fnname);
 	      s = get_section (section_name, flags, current_function_decl);
 	      free (section_name);
 	    }
 	  else
 #endif
 	    exception_section
-	      = s = get_section (".gcc_except_table", flags, NULL);
+	      = s = get_section (TARGET_GCC_EXCEPT_TABLE, flags, NULL);
 	}
       else
 	exception_section

--- gcc/expmed.c	2018-10-12 18:31:33.000000000 +0100
+++ gcc/expmed.c	2025-11-02 00:56:38.146281349 +0000
@@ -570,7 +570,12 @@ simple_mem_bitfield_p (rtx op0, unsigned
 	  && bitsize == GET_MODE_BITSIZE (mode)
 	  && (!SLOW_UNALIGNED_ACCESS (mode, MEM_ALIGN (op0))
 	      || (bitnum % GET_MODE_ALIGNMENT (mode) == 0
-		  && MEM_ALIGN (op0) >= GET_MODE_ALIGNMENT (mode))));
+		  && (
+				  MEM_ALIGN (op0) >= GET_MODE_ALIGNMENT (mode)
+#if defined(TARGET_M68K)
+  ||(!TUNE_68000 && !TUNE_68010)
+#endif
+				  ))));
 }
 
 /* Try to use instruction INSV to store VALUE into a field of OP0.

--- gcc/file-find.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/file-find.c	2025-11-02 00:56:38.150281409 +0000
@@ -22,6 +22,11 @@ along with GCC; see the file COPYING3.
 #include "filenames.h"
 #include "file-find.h"
 
+#ifdef __amiga__
+#undef PATH_SEPARATOR
+#define PATH_SEPARATOR ';'
+#endif
+
 static bool debug = false;
 
 void
@@ -81,6 +86,9 @@ find_a_file (struct path_prefix *pprefix
 	strcpy (temp, pl->prefix);
 	strcat (temp, name);
 
+	if (debug)
+	  fprintf (stderr, "stat <%s>\n", temp);
+
 	if (stat (temp, &st) >= 0
 	    && ! S_ISDIR (st.st_mode)
 	    && access (temp, mode) == 0)
@@ -180,6 +188,13 @@ prefix_from_string (const char *p, struc
   startp = endp = p;
   while (1)
     {
+#ifdef __MSYS__
+      if (*endp == PATH_SEPARATOR && endp-startp == 1)
+	{
+	      ++endp;
+    	  continue;
+	}
+#endif
       if (*endp == PATH_SEPARATOR || *endp == 0)
 	{
 	  strncpy (nstore, startp, endp-startp);

--- gcc/final.c	2018-03-07 09:49:47.000000000 +0000
+++ gcc/final.c	2025-11-02 00:56:38.150281409 +0000
@@ -2151,6 +2151,7 @@ call_from_call_insn (rtx_call_insn *insn
    SEEN is used to track the end of the prologue, for emitting
    debug information.  We force the emission of a line note after
    both NOTE_INSN_PROLOGUE_END and NOTE_INSN_FUNCTION_BEG.  */
+rtx_insn * current_insn;
 
 rtx_insn *
 final_scan_insn (rtx_insn *insn, FILE *file, int optimize_p ATTRIBUTE_UNUSED,
@@ -2160,6 +2161,7 @@ final_scan_insn (rtx_insn *insn, FILE *f
   rtx set;
 #endif
   rtx_insn *next;
+  current_insn = insn;
 
   insn_counter++;
 
@@ -2723,6 +2725,11 @@ final_scan_insn (rtx_insn *insn, FILE *f
 			== CONST0_RTX (GET_MODE (XEXP (SET_SRC (set), 0))))
 		      src2 = XEXP (SET_SRC (set), 0);
 		  }
+
+		rtx note = 0;
+		rtx_insn * ninsn = 0;
+		rtx nset = 0, nss = 0;;
+
 		if ((cc_status.value1 != 0
 		     && rtx_equal_p (src1, cc_status.value1))
 		    || (cc_status.value2 != 0
@@ -2730,7 +2737,17 @@ final_scan_insn (rtx_insn *insn, FILE *f
 		    || (src2 != 0 && cc_status.value1 != 0
 		        && rtx_equal_p (src2, cc_status.value1))
 		    || (src2 != 0 && cc_status.value2 != 0
-			&& rtx_equal_p (src2, cc_status.value2)))
+			&& rtx_equal_p (src2, cc_status.value2))
+
+		/** compare with zero can be omitted, if reg is zero and mode is smaller,
+		 *  if next insn is a jmp EQ
+		 */
+		    || (src2 != 0 && GET_CODE(XEXP(src1, 1)) == CONST_INT && INTVAL((XEXP(src1, 1))) == 0
+			&& cc_status.value1 && REG_P(cc_status.value1) && src2 && REG_P(src2) && REGNO(cc_status.value1) == REGNO(src2)
+			&& (note = find_reg_note(insn, REG_BIT_MASK, 0)) && INTVAL(XEXP(note,0)) < (1 << (8*GET_MODE_SIZE(GET_MODE(src2))))
+			&& JUMP_P(ninsn = NEXT_INSN(insn)) && (nset = single_set(ninsn)) && (GET_CODE(nss = XEXP(SET_SRC(nset),0)) == EQ || GET_CODE(nss) == NE)
+			)
+		    )
 		  {
 		    /* Don't delete insn if it has an addressing side-effect.  */
 		    if (! FIND_REG_INC_NOTE (insn, NULL_RTX)
@@ -2890,6 +2907,8 @@ final_scan_insn (rtx_insn *insn, FILE *f
 		SET_PREV_INSN (insn) = PREV_INSN (next);
 		SET_NEXT_INSN (insn) = next;
 		SET_PREV_INSN (next) = insn;
+
+		current_insn = insn;
 	      }
 
 	    /* PEEPHOLE might have changed this.  */
@@ -2918,7 +2937,11 @@ final_scan_insn (rtx_insn *insn, FILE *f
 	  }
 
 	if (! constrain_operands_cached (insn, 1))
-	  fatal_insn_not_found (insn);
+	  {
+	    debug_rtx(insn);
+	    constrain_operands_cached (insn, 1);
+	    fatal_insn_not_found (insn);
+	  }
 
 	/* Some target machines need to prescan each insn before
 	   it is output.  */
@@ -3622,6 +3645,13 @@ do_assembler_dialects (const char *p, in
 void
 output_asm_insn (const char *templ, rtx *operands)
 {
+  extern bool be_very_verbose;
+  extern bool dump_cycles;
+  extern void append_reg_usage(FILE *, rtx_insn *);
+
+  extern bool dump_reg_track;
+  void append_reg_cache (FILE * f, rtx_insn * insn);
+
   const char *p;
   int c;
 #ifdef ASSEMBLER_DIALECT
@@ -3778,6 +3808,11 @@ output_asm_insn (const char *templ, rtx
 	putc (c, asm_out_file);
       }
 
+  if (be_very_verbose || dump_cycles)
+    append_reg_usage(asm_out_file, current_insn);
+  if (dump_reg_track)
+    append_reg_cache(asm_out_file, current_insn);
+
   /* Write out the variable names for operands, if we know them.  */
   if (flag_verbose_asm)
     output_asm_operand_names (operands, oporder, ops);
@@ -3948,18 +3983,30 @@ output_addr_const (FILE *file, rtx x)
       /* Some assemblers need integer constants to appear last (eg masm).  */
       if (CONST_INT_P (XEXP (x, 0)))
 	{
+#if defined(TARGET_AMIGAOS)
+	  output_addr_const (file, XEXP (x, 0));
+	  fprintf (file, "+");
+	  output_addr_const (file, XEXP (x, 1));
+#else
 	  output_addr_const (file, XEXP (x, 1));
 	  if (INTVAL (XEXP (x, 0)) >= 0)
 	    fprintf (file, "+");
 	  output_addr_const (file, XEXP (x, 0));
+#endif
 	}
       else
 	{
+#if defined(TARGET_AMIGAOS)
+	  output_addr_const (file, XEXP (x, 1));
+	  fprintf (file, "+");
+	  output_addr_const (file, XEXP (x, 0));
+#else
 	  output_addr_const (file, XEXP (x, 0));
 	  if (!CONST_INT_P (XEXP (x, 1))
 	      || INTVAL (XEXP (x, 1)) >= 0)
 	    fprintf (file, "+");
 	  output_addr_const (file, XEXP (x, 1));
+#endif
 	}
       break;
 
@@ -3995,6 +4042,7 @@ output_addr_const (FILE *file, rtx x)
       if (targetm.asm_out.output_addr_const_extra (file, x))
 	break;
 
+      debug_rtx(current_output_insn);
       output_operand_lossage ("invalid expression as operand");
     }
 }

--- gcc/fold-const.c	2018-10-17 18:54:26.000000000 +0100
+++ gcc/fold-const.c	2025-11-02 00:56:38.154281470 +0000
@@ -3860,7 +3860,7 @@ make_bit_field_ref (location_t loc, tree
   bftype = type;
   if (TYPE_PRECISION (bftype) != bitsize
       || TYPE_UNSIGNED (bftype) == !unsignedp)
-    bftype = build_nonstandard_integer_type (bitsize, 0);
+    bftype = build_nonstandard_integer_type (bitsize, TYPE_UNSIGNED (bftype)); // SBF: keep the signedness
 
   result = build3_loc (loc, BIT_FIELD_REF, bftype, inner,
 		       size_int (bitsize), bitsize_int (bitpos));
@@ -4032,6 +4032,15 @@ optimize_bit_field_compare (location_t l
       rhs = build_int_cst (type, 0);
     }
 
+#if defined(TARGET_M68K)
+  /* SBF: do no conversion.
+   * if the same component/bitfield reference is used afterwards
+   * a later optimizer may combine these.
+   *
+   * With the transformed version it's not possible.
+   */
+  lhs = build2_loc (loc, code, compare_type, lhs, rhs);
+#else
   /* Make a new bitfield reference, shift the constant over the
      appropriate number of bits and mask it with the computed mask
      (in case this was a signed field).  If we changed it, make a new one.  */
@@ -4043,9 +4052,10 @@ optimize_bit_field_compare (location_t l
 				  fold_convert_loc (loc, unsigned_type, rhs),
 				  size_int (lbitpos)),
 		     mask);
-
   lhs = build2_loc (loc, code, compare_type,
 		    build2 (BIT_AND_EXPR, unsigned_type, lhs, mask), rhs);
+#endif
+
   return lhs;
 }
 
@@ -8132,7 +8142,8 @@ fold_unary_ignore_overflow_loc (location
    ARG0 and ARG1 are the NOP_STRIPed results of OP0 and OP1.
    Return the folded expression if folding is successful.  Otherwise,
    return NULL_TREE.  */
-static tree
+// static
+tree
 fold_truth_andor (location_t loc, enum tree_code code, tree type,
 		  tree arg0, tree arg1, tree op0, tree op1)
 {
Only in ./gcc-6.5.0/gcc/fortran: gfortran.info

--- gcc/function.c	2018-06-25 18:38:29.000000000 +0100
+++ gcc/function.c	2025-11-02 00:56:38.206282257 +0000
@@ -39,9 +39,9 @@ along with GCC; see the file COPYING3.
 #include "rtl.h"
 #include "tree.h"
 #include "gimple-expr.h"
+#include "tm_p.h"
 #include "cfghooks.h"
 #include "df.h"
-#include "tm_p.h"
 #include "stringpool.h"
 #include "expmed.h"
 #include "optabs.h"
@@ -1478,7 +1478,6 @@ instantiate_virtual_regs_in_rtx (rtx *lo
 		}
 	      iter.skip_subrtxes ();
 	      break;
-
 	    case PLUS:
 	      new_rtx = instantiate_new_reg (XEXP (x, 0), &offset);
 	      if (new_rtx)
@@ -1486,10 +1485,9 @@ instantiate_virtual_regs_in_rtx (rtx *lo
 		  XEXP (x, 0) = new_rtx;
 		  *loc = plus_constant (GET_MODE (x), x, offset, true);
 		  changed = true;
-		  iter.skip_subrtxes ();
+//		  iter.skip_subrtxes (); // may contain a mem instead of the const.
 		  break;
 		}
-
 	      /* FIXME -- from old code */
 	      /* If we have (plus (subreg (virtual-reg)) (const_int)), we know
 		 we can commute the PLUS and SUBREG because pointers into the
@@ -1633,14 +1631,20 @@ instantiate_virtual_regs_in_insn (rtx_in
 
   /* In the general case, we expect virtual registers to appear only in
      operands, and then only as either bare registers or inside memories.  */
+  /* SBF: that's not true, since there is also lea. */
   for (i = 0; i < recog_data.n_operands; ++i)
     {
+      rtx addr;
       x = recog_data.operand[i];
       switch (GET_CODE (x))
 	{
+	case PLUS:
+	  /* SBF: handle all plus, it might be a lea insn. */
+	  instantiate_virtual_regs_in_rtx (&x);
+	  continue;
 	case MEM:
 	  {
-	    rtx addr = XEXP (x, 0);
+	    addr = XEXP (x, 0);
 
 	    if (!instantiate_virtual_regs_in_rtx (&addr))
 	      continue;

--- gcc/gcc.c	2018-06-25 18:11:36.000000000 +0100
+++ gcc/gcc.c	2025-11-02 00:56:38.210282318 +0000
@@ -28,6 +28,7 @@ Once it knows which kind of compilation
 compilation is specified by a string called a "spec".  */
 
 #include "config.h"
+#include "configargs.h"
 #include "system.h"
 #include "coretypes.h"
 #include "multilib.h" /* before tm.h */
@@ -207,9 +208,6 @@ int is_cpp_driver;
 /* Flag set to nonzero if an @file argument has been supplied to gcc.  */
 static bool at_file_supplied;
 
-/* Definition of string containing the arguments given to configure.  */
-#include "configargs.h"
-
 /* Flag saying to print the command line options understood by gcc and its
    sub-processes.  */
 
@@ -239,7 +237,9 @@ FILE *report_times_to_file = NULL;
 
 /* Nonzero means place this string before uses of /, so that include
    and library files can be found in an alternate location.  */
-
+#ifdef __amiga__
+#define TARGET_SYSTEM_ROOT "GCC:"
+#endif
 #ifdef TARGET_SYSTEM_ROOT
 #define DEFAULT_TARGET_SYSTEM_ROOT (TARGET_SYSTEM_ROOT)
 #else
@@ -511,6 +511,8 @@ or with constant text in a single argume
 	assembler has done its job.
  %D	Dump out a -L option for each directory in startfile_prefixes.
 	If multilib_dir is set, extra entries are generated with it affixed.
+ %F	Dump out a -L option for each directory in startfile_prefixes.
+	Always ignore if multilib_dir is set.
  %l     process LINK_SPEC as a spec.
  %L     process LIB_SPEC as a spec.
  %M     Output multilib_os_dir.
@@ -1042,6 +1044,10 @@ proper position among the other output f
 # define SYSROOT_SPEC "--sysroot=%R"
 #endif
 
+#ifndef SELF_SPEC
+# define SELF_SPEC ""
+#endif
+
 #ifndef SYSROOT_SUFFIX_SPEC
 # define SYSROOT_SUFFIX_SPEC ""
 #endif
@@ -1075,7 +1081,7 @@ static const char *startfile_prefix_spec
 static const char *sysroot_spec = SYSROOT_SPEC;
 static const char *sysroot_suffix_spec = SYSROOT_SUFFIX_SPEC;
 static const char *sysroot_hdrs_suffix_spec = SYSROOT_HEADERS_SUFFIX_SPEC;
-static const char *self_spec = "";
+static const char *self_spec = SELF_SPEC;
 
 /* Standard options to cpp, cc1, and as, to reduce duplication in specs.
    There should be no need to override these in target dependent files,
@@ -2232,8 +2238,8 @@ read_specs (const char *filename, bool m
       /* The colon shouldn't be missing.  */
       if (*p1 != ':')
 	fatal_error (input_location,
-		     "specs file malformed after %ld characters",
-		     (long) (p1 - buffer));
+		     "specs file malformed after %ld characters: %s",
+		     (long) (p1 - buffer), p1);
 
       /* Skip back over trailing whitespace.  */
       p2 = p1;
@@ -2246,8 +2252,8 @@ read_specs (const char *filename, bool m
       p = skip_whitespace (p1 + 1);
       if (p[1] == 0)
 	fatal_error (input_location,
-		     "specs file malformed after %ld characters",
-		     (long) (p - buffer));
+		     "specs file malformed after %ld characters: %s",
+		     (long) (p - buffer), p);
 
       p1 = p;
       /* Find next blank line or end of string.  */
@@ -3923,9 +3929,13 @@ driver_handle_option (struct gcc_options
       /* POSIX allows separation of -l and the lib arg; canonicalize
 	 by concatenating -l with its arg */
       add_infile (concat ("-l", arg, NULL), "*");
+      if (0 == strcmp("m", arg)  || 0 == strcmp("pthread", arg))
+	{
+	  save_switch (concat ("-l", arg, NULL), 0, NULL, validated, true);
+	  return true;
+	}
       do_save = false;
       break;
-
     case OPT_L:
       /* Similarly, canonicalize -L for linkers that may not accept
 	 separate arguments.  */
@@ -4958,6 +4968,23 @@ do_self_spec (const char *spec)
     }
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
 /* Callback for processing %D and %I specs.  */
 
 struct spec_path_info {
@@ -4984,6 +5011,8 @@ spec_path (char *path, void *data)
       memcpy (path + len, info->append, info->append_len + 1);
     }
 
+  normalize(path);
+
   if (!is_directory (path, true))
     return NULL;
 
@@ -5187,6 +5216,7 @@ do_spec_1 (const char *spec, int inswitc
 	     followed by the absolute directories
 	     that we search for startfiles.  */
 	  case 'D':
+	  case 'F':
 	    {
 	      struct spec_path_info info;
 
@@ -5204,7 +5234,7 @@ do_spec_1 (const char *spec, int inswitc
 #endif
 	      info.separate_options = false;
 
-	      for_each_path (&startfile_prefixes, true, 0, spec_path, &info);
+	      for_each_path (&startfile_prefixes, c == 'D', 0, spec_path, &info);
 	    }
 	    break;
 
@@ -10107,3 +10137,24 @@ driver_get_configure_time_options (void
   obstack_free (&obstack, NULL);
   n_switches = 0;
 }
+
+#if defined(TARGET_AMIGAOS)
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
+//  printf("amiga_m68k_prefix_func='%s'\n", p);
+  return p;
+}
+#endif

--- gcc/gcse.c	2017-03-22 10:41:18.000000000 +0000
+++ gcc/gcse.c	2025-11-02 00:56:38.210282318 +0000
@@ -2268,6 +2268,13 @@ pre_insert_copy_insn (struct gcse_expr *
         new_insn = emit_insn_after (new_insn, insn);
     }
 
+  /* SBF: move REG_INC note. */
+  if (NEXT_INSN(insn) == new_insn && find_reg_note(insn, REG_INC, old_reg))
+    {
+      remove_note(insn, find_reg_note(insn, REG_INC, old_reg));
+      add_reg_note (new_insn, REG_INC, old_reg);
+    }
+
   gcse_create_count++;
 
   if (dump_file)
@@ -2469,7 +2476,8 @@ pre_delete (void)
 	    /* We only delete insns that have a single_set.  */
 	    if (bitmap_bit_p (pre_delete_map[bb->index], indx)
 		&& (set = single_set (insn)) != 0
-                && dbg_cnt (pre_insn))
+                && dbg_cnt (pre_insn)
+		&& !find_reg_note(insn, REG_INC, SET_DEST (set)))
 	      {
 		/* Create a pseudo-reg to store the result of reaching
 		   expressions into.  Get the mode for the new pseudo from
@@ -4075,7 +4083,10 @@ pass_rtl_pre::gate (function *fun)
 {
   return optimize > 0 && flag_gcse
     && !fun->calls_setjmp
+#if !defined(TARGET_M68K)
+/* SBF: also for -Os on 68k, since hoist is not good */
     && optimize_function_for_speed_p (fun)
+#endif
     && dbg_cnt (pre);
 }
 
@@ -4118,6 +4129,10 @@ public:
 bool
 pass_rtl_hoist::gate (function *)
 {
+#if defined(TARGET_M68K)
+/* SBF: hoist is not good on m68k */
+  return false;
+#else
   return optimize > 0 && flag_gcse
     && !cfun->calls_setjmp
     /* It does not make sense to run code hoisting unless we are optimizing
@@ -4125,6 +4140,7 @@ pass_rtl_hoist::gate (function *)
        bigger if we did PRE (when optimizing for space, we don't run PRE).  */
     && optimize_function_for_size_p (cfun)
     && dbg_cnt (hoist);
+#endif
 }
 
 } // anon namespace

--- gcc/genconditions.c	2016-01-22 16:44:10.000000000 +0000
+++ gcc/genconditions.c	2025-11-02 00:56:38.214282378 +0000
@@ -94,6 +94,7 @@ write_header (void)
 #include \"resource.h\"\n\
 #include \"diagnostic-core.h\"\n\
 #include \"reload.h\"\n\
+#include \"tree-pass.h\"\n\
 #include \"tm-constrs.h\"\n");
 
   if (saw_eh_return)

--- gcc/gengtype.c	2016-01-13 02:51:11.000000000 +0000
+++ gcc/gengtype.c	2025-11-02 00:56:38.218282439 +0000
@@ -4106,6 +4106,17 @@ finish_root_table (struct flist *flp, co
 	for (fnum = 0; bitmap != 0; fnum++, bitmap >>= 1)
 	  if (bitmap & 1)
 	    {
+	      char const * mf = get_output_file_name (CONST_CAST (input_file*, fli2->file));
+	      if (0 == strcmp(mf, "gt-c-c-decl.h") ||
+		  0 == strcmp(mf, "gt-c-family-c-common.h") ||
+		  0 == strcmp(mf, "gt-c-c-parser.h") || 1
+		  )
+		{
+		  oprintf (base_files[fnum], "\n#ifdef __amiga__\n");
+		  oprintf (base_files[fnum], "__attribute((section(\".data\")))\n");
+		  oprintf (base_files[fnum], "#endif\n");
+		}
+
 	      oprintf (base_files[fnum],
 		       "extern const struct %s gt_%s_", tname, pfx);
 	      put_mangled_filename (base_files[fnum], fli2->file);
@@ -4116,8 +4127,19 @@ finish_root_table (struct flist *flp, co
   {
     size_t fnum;
     for (fnum = 0; base_files && fnum < num_lang_dirs; fnum++)
-      oprintf (base_files[fnum],
-	       "EXPORTED_CONST struct %s * const %s[] = {\n", tname, name);
+      {
+	if (0 == strcmp(name, "gt_ggc_rtab") ||
+	    0 == strcmp(name, "gt_pch_scalar_rtab") ||
+	    0 == strcmp(name, "gt_ggc_r_gt_c_c_decl_h")
+	    || 1)
+	  {
+            oprintf (base_files[fnum], "#ifdef __amiga__\n");
+            oprintf (base_files[fnum], "__attribute((section(\".data\")))\n");
+            oprintf (base_files[fnum], "#endif\n");
+	  }
+        oprintf (base_files[fnum],
+	         "EXPORTED_CONST struct %s * const %s[] = {\n", tname, name);
+      }
   }
 
 
@@ -4552,6 +4574,15 @@ write_roots (pair_p variables, bool emit
       if (!fli->started_p)
 	{
 	  fli->started_p = 1;
+	  char const * mf = get_output_file_name (CONST_CAST (input_file*, v->line.file));
+	  if (0 == strcmp(mf, "gt-c-c-decl.h") ||
+	      0 == strcmp(mf, "gt-c-c-parser.h") || 1
+	      )
+	    {
+	      oprintf (f, "\n#ifdef __amiga__\n");
+	      oprintf (f, "__attribute((section(\".data\")))\n");
+	      oprintf (f, "#endif\n");
+	    }
 
 	  oprintf (f, "EXPORTED_CONST struct ggc_root_tab gt_ggc_r_");
 	  put_mangled_filename (f, v->line.file);
@@ -4586,6 +4617,9 @@ write_roots (pair_p variables, bool emit
 	{
 	  fli->started_p = 1;
 
+          oprintf (f, "#ifdef __amiga__\n");
+          oprintf (f, "__attribute((section(\".data\")))\n");
+          oprintf (f, "#endif\n");
 	  oprintf (f, "EXPORTED_CONST struct ggc_root_tab gt_ggc_rd_");
 	  put_mangled_filename (f, v->line.file);
 	  oprintf (f, "[] = {\n");
@@ -4660,6 +4694,14 @@ write_roots (pair_p variables, bool emit
 	{
 	  fli->started_p = 1;
 
+	  char const * mf = get_output_file_name (CONST_CAST (input_file*, v->line.file));
+	  if (0 == strcmp(mf, "gt-c-c-decl.h") || 1)
+	    {
+	      oprintf (f, "\n#ifdef __amiga__\n");
+	      oprintf (f, "__attribute((section(\".data\")))\n");
+	      oprintf (f, "#endif\n");
+	    }
+
 	  oprintf (f, "EXPORTED_CONST struct ggc_root_tab gt_pch_rs_");
 	  put_mangled_filename (f, v->line.file);
 	  oprintf (f, "[] = {\n");
@@ -5032,6 +5074,14 @@ parse_program_options (int argc, char **
 	    srcdir = optarg;
 	  else
 	    fatal ("missing source directory");
+#ifdef __CYGWIN__
+	  if (0 == strncmp("/cygdrive/", srcdir, 10))
+	    {
+	      9[(char *)srcdir] = srcdir[10];
+	      10[(char *)srcdir] = ':';
+	      srcdir += 9;
+	    }
+#endif
 	  srcdir_len = strlen (srcdir);
 	  break;
 	case 'B':		/* --backupdir */
@@ -5104,6 +5154,18 @@ input_file_by_name (const char* name)
   f->inpoutf = NULL;
   f->inpisplugin = false;
   strcpy (f->inpname, name);
+
+#ifdef __CYGWIN__
+    if (strstr(f->inpname, "/cygdrive/") == f->inpname)
+      {
+	int l = strlen(&f->inpname[11]) + 1;
+	char * p = f->inpname;
+	p[0] = p[10];
+	p[1] = ':';
+	memmove(&p[2], &p[11], l);
+      }
+#endif
+
   slot = htab_find_slot (input_file_htab, f, INSERT);
   gcc_assert (slot != NULL);
   if (*slot)
Only in ./gcc-6.5.0/gcc: gengtype-lex.c

--- gcc/genrecog.c	2016-03-15 00:42:43.000000000 +0000
+++ gcc/genrecog.c	2025-11-02 00:56:38.222282499 +0000
@@ -4173,6 +4173,7 @@ write_header (void)
 #include \"reload.h\"\n\
 #include \"regs.h\"\n\
 #include \"tm-constrs.h\"\n\
+#include \"tree-pass.h\"\n\
 \n");
 
   puts ("\n\

--- gcc/ggc-common.c	2016-01-28 08:39:05.000000000 +0000
+++ gcc/ggc-common.c	2025-11-02 00:56:38.222282499 +0000
@@ -29,6 +29,7 @@ along with GCC; see the file COPYING3.
 #include "params.h"
 #include "hosthooks.h"
 #include "plugin.h"
+#include <sys/resource.h>
 
 /* When set, ggc_collect will do collection.  */
 bool ggc_force_collect;
@@ -729,6 +730,7 @@ static double
 ggc_rlimit_bound (double limit)
 {
 #if defined(HAVE_GETRLIMIT)
+// && !defined __amiga__
   struct rlimit rlim;
 # if defined (RLIMIT_AS)
   /* RLIMIT_AS is what POSIX says is the limit on mmap.  Presumably

--- gcc/ggc.h	2016-01-04 14:30:50.000000000 +0000
+++ gcc/ggc.h	2025-11-02 00:56:38.222282499 +0000
@@ -70,9 +70,20 @@ struct ggc_root_tab {
 };
 #define LAST_GGC_ROOT_TAB { NULL, 0, 0, NULL, NULL }
 /* Pointers to arrays of ggc_root_tab, terminated by NULL.  */
+#ifndef __MAKE_MSYS2__
+#ifdef __amiga__
+__near
+#endif
 extern const struct ggc_root_tab * const gt_ggc_rtab[];
+#ifdef __amiga__
+__near
+#endif
 extern const struct ggc_root_tab * const gt_ggc_deletable_rtab[];
+#ifdef __amiga__
+__near
+#endif
 extern const struct ggc_root_tab * const gt_pch_scalar_rtab[];
+#endif
 
 /* If EXPR is not NULL and previously unmarked, mark it and evaluate
    to true.  Otherwise evaluate to false.  */

--- gcc/gimplify.c	2018-10-12 18:29:35.000000000 +0100
+++ gcc/gimplify.c	2025-11-02 00:56:38.234282681 +0000
@@ -4657,6 +4657,9 @@ gimplify_modify_expr (tree *expr_p, gimp
   gimple *assign;
   location_t loc = EXPR_LOCATION (*expr_p);
   gimple_stmt_iterator gsi;
+  gimple * last;
+
+  last = gimple_seq_last (*pre_p);
 
   gcc_assert (TREE_CODE (*expr_p) == MODIFY_EXPR
 	      || TREE_CODE (*expr_p) == INIT_EXPR);
@@ -4880,6 +4883,50 @@ gimplify_modify_expr (tree *expr_p, gimp
   gsi = gsi_last (*pre_p);
   maybe_fold_stmt (&gsi);
 
+  /* SBF: check if a post increment can be reordered...
+   * p1: b = a + 4;
+   * p2: x1 = *a;
+   * ==>
+   * p2: x1 = *a;
+   * p1: b = a + 4;
+   *
+   * or
+   *
+   * p1: b = a + 4;
+   * p2: *a = x2;
+   * ==>
+   * p2: *a = x2;
+   * p1: b = a + 4;
+   */
+  gimple * p2 = gimple_seq_last_stmt(*pre_p);
+  if (p2->code == GIMPLE_ASSIGN && p2->num_ops == 2 && p2->prev && p2->prev != p2)
+    {
+      gimple * p1 = p2->prev;
+      if (p1->code == GIMPLE_ASSIGN && p1->num_ops == 3)
+	{
+	  tree b = gimple_assign_lhs(p1);
+	  tree var = gimple_assign_rhs1(p1); /* must be the same as in p2. */
+	  tree x1 = gimple_assign_lhs(p2);
+	  tree x2 = gimple_assign_rhs1(p2);
+	  if (b != x2 && b != var && (TREE_CODE(b) == VAR_DECL || TREE_CODE(x2) == VAR_DECL || TREE_CODE(b) == PARM_DECL || TREE_CODE(x2) == PARM_DECL) &&
+	      ((TREE_CODE(x1) == VAR_DECL && TREE_CODE(x2) == MEM_REF &&
+		TREE_OPERAND(x2, 0) != b && TREE_OPERAND(x2, 0) == var) ||
+	       (TREE_CODE(x1) == MEM_REF && (TREE_CODE(x2) == INTEGER_CST || (TREE_CODE(x2) == VAR_DECL && TREE_OPERAND(x1, 0) != b))
+		   && TREE_OPERAND(x1, 0) == var)))
+	    {
+	      gimple_stmt_iterator to = gsi_last (*pre_p);
+	      gimple_stmt_iterator from = to;
+	      from.ptr = p1;
+//	      fprintf(stderr, "swap\n");
+//	      extern void debug (gimple *ptr);
+//	      debug(p1);
+//	      debug(p2);
+	      gsi_remove (&from, false);
+	      gsi_insert_after (&to, p1, GSI_NEW_STMT);
+	    }
+	}
+    }
+
   if (want_value)
     {
       *expr_p = TREE_THIS_VOLATILE (*to_p) ? *from_p : unshare_expr (*to_p);

--- gcc/ginclude/stddef.h	2016-01-04 14:30:50.000000000 +0000
+++ gcc/ginclude/stddef.h	2025-11-02 00:56:38.234282681 +0000
@@ -325,6 +325,7 @@ typedef __rune_t        rune_t;
 #define __WCHAR_TYPE__ int
 #endif
 #ifndef __cplusplus
+#define _WCHAR_T_ int
 typedef __WCHAR_TYPE__ wchar_t;
 #endif
 #endif

--- gcc/graphite.h	2018-08-14 16:15:39.000000000 +0100
+++ gcc/graphite.h	2025-11-02 00:56:38.254282984 +0000
@@ -27,6 +27,7 @@ along with GCC; see the file COPYING3.
 #include <isl/ctx.h>
 #include <isl/val_gmp.h>
 #include <isl/set.h>
+#include <isl/val.h>
 #include <isl/union_set.h>
 #include <isl/map.h>
 #include <isl/union_map.h>

--- gcc/hwint.h	2017-12-15 11:32:27.000000000 +0000
+++ gcc/hwint.h	2025-11-02 00:56:38.258283044 +0000
@@ -295,7 +295,7 @@ abs_hwi (HOST_WIDE_INT x)
 inline unsigned HOST_WIDE_INT
 absu_hwi (HOST_WIDE_INT x)
 {
-  return x >= 0 ? (unsigned HOST_WIDE_INT)x : -(unsigned HOST_WIDE_INT)x;
+  return x >= 0 ? (unsigned HOST_WIDE_INT)x : -(signed HOST_WIDE_INT)x;
 }
 
 #endif /* ! GCC_HWINT_H */

--- gcc/incpath.c	2016-11-07 19:17:04.000000000 +0000
+++ gcc/incpath.c	2025-11-02 00:56:38.262283105 +0000
@@ -251,6 +251,18 @@ remove_duplicates (cpp_reader *pfile, st
 
       cur = *pcur;
 
+      // normalize
+      char *q, *p = cur->name;
+      while ((q = strstr (p, "/../")))
+	{
+	  char *r = q - 1;
+	  while (r >= p && *r != '/' && *r != ':')
+	    --r;
+	  if (r < p)
+	    break;
+	  memmove (r + 1, q + 4, strlen (q + 4) + 1);
+	}
+
       if (stat (cur->name, &st))
 	{
 	  /* Dirs that don't exist or have denied permissions are 
@@ -323,7 +335,14 @@ add_sysroot_to_chain (const char *sysroo
 
   for (p = heads[chain]; p != NULL; p = p->next)
     if (p->name[0] == '=' && p->user_supplied_p)
-      p->name = concat (sysroot, p->name + 1, NULL);
+      {
+	char *q = p->name + 1;
+#ifdef __amiga__
+        while (*q == '/')
+          ++q;
+#endif
+	p->name = concat (sysroot, q, NULL);
+      }
 }
 
 /* Merge the four include chains together in the order quote, bracket,
@@ -421,6 +440,23 @@ void
 add_path (char *path, int chain, int cxx_aware, bool user_supplied_p)
 {
   cpp_dir *p;
+  char * q;
+  size_t l = strlen(path);
+  if (l > 1 && (path[0] == '"' || path[0] == '\'') && path[l - 1] == path[0])
+    {
+      l -= 2;
+      memmove(path, path + 1, l);
+      path[l] = 0;
+    }
+  while ((q = strstr(path, "/../")))
+    {
+      char * r = q - 1;
+      while (r >= path && *r != '/')
+	--r;
+      if (r < path)
+	break;
+      memmove(r, q + 3, strlen(q + 3) + 1);
+    }
 
 #if defined (HAVE_DOS_BASED_FILE_SYSTEM)
   /* Remove unnecessary trailing slashes.  On some versions of MS
@@ -449,7 +485,6 @@ add_path (char *path, int chain, int cxx
     p->sysp = 0;
   p->construct = 0;
   p->user_supplied_p = user_supplied_p;
-
   add_cpp_dir_path (p, chain);
 }
 

--- gcc/ipa-chkp.c	2016-06-20 12:35:02.000000000 +0100
+++ gcc/ipa-chkp.c	2025-11-02 00:56:38.262283105 +0000
@@ -23,6 +23,8 @@ along with GCC; see the file COPYING3.
 #include "system.h"
 #include "coretypes.h"
 #include "backend.h"
+#include "tm_p.h"
+#include "target.h"
 #include "tree.h"
 #include "gimple.h"
 #include "tree-pass.h"

--- gcc/ira-build.c	2016-07-18 14:01:12.000000000 +0100
+++ gcc/ira-build.c	2025-11-02 00:56:38.274283287 +0000
@@ -2897,6 +2897,8 @@ setup_min_max_conflict_allocno_ids (void
 	 (the reason for this is the allocnos with bigger conflict id
 	 have their range starts not smaller than allocnos with
 	 smaller ids.  */
+      if (filled_area_start > ira_max_point)
+	filled_area_start = ira_max_point;
       for (j = min; j < filled_area_start; j++)
 	last_lived[j] = i;
       filled_area_start = min;

--- gcc/ira.c	2017-10-18 22:15:24.000000000 +0100
+++ gcc/ira.c	2025-11-02 00:56:38.278283347 +0000
@@ -364,6 +364,9 @@ along with GCC; see the file COPYING3.
 
 
 #include "config.h"
+#define INCLUDE_VECTOR
+#define INCLUDE_SET
+#define INCLUDE_MAP
 #include "system.h"
 #include "coretypes.h"
 #include "backend.h"
@@ -391,6 +394,10 @@ along with GCC; see the file COPYING3.
 #include "rtl-iter.h"
 #include "shrink-wrap.h"
 #include "print-rtl.h"
+#include "langhooks.h"
+#include <vector>
+#include <set>
+#include <map>
 
 struct target_ira default_target_ira;
 struct target_ira_int default_target_ira_int;
@@ -5090,6 +5097,423 @@ move_unallocated_pseudos (void)
       }
 }
 
+
+#if defined(TARGET_M68K)
+/**
+ * This is the partial structure from m68k.c - with all fields used here.
+ */
+extern struct m68k_frame {
+  /* Stack pointer to frame pointer offset.  */
+  HOST_WIDE_INT offset;
+
+  /* Offset of FPU registers.  */
+  HOST_WIDE_INT foffset;
+
+  /* Frame size in bytes (rounded up).  */
+  HOST_WIDE_INT size;
+
+} current_frame;
+
+/**
+ * Test a src or dst, if it needs a fix and fix it.
+ */
+static void fix_one(rtx_insn * insn, rtx * mem_loc, int opno, int size, int offset)
+{
+  tree name;
+  rtx mem = *mem_loc;
+  if (MEM_P(mem) && GET_CODE(XEXP(mem, 0)) == PLUS
+      && REG_P(XEXP(XEXP(mem, 0), 0))
+      && REGNO(XEXP(XEXP(mem, 0), 0)) == SP_REG
+      && CONST_INT_P(XEXP(XEXP(mem, 0), 1))
+      && MEM_OFFSET_KNOWN_P (mem)
+      && MEM_EXPR (mem)
+      && MEM_OFFSET(mem)
+      && MEM_EXPR(mem)->base.code == VAR_DECL
+      && (name = MEM_EXPR (mem)->var_decl.common.common.common.common.name)
+      && 0 == strcmp("%sfp", (char *)name->identifier.id.str)
+      )
+    {
+      int n = INTVAL(XEXP(XEXP(mem, 0), 1));
+      int m = MEM_OFFSET(mem);
+      int add = size + offset - n + m;
+      if (add > 0)
+	{
+//	  fprintf(stderr, "add=%d, size=%d, offset=%d, n=%d, m=%d\n", add, size, offset, n, -m);
+//	  // debug_rtx (insn);
+	  mem = copy_rtx_if_shared(mem);
+	  XEXP(XEXP(mem, 0), 1) = GEN_INT(n + add);
+	  *mem_loc = mem;
+//	  // debug_rtx (insn);
+	}
+    }
+}
+/**
+ * This is a hack!
+ *
+ * Reload seems to fail adjusting a spill reg on stack
+ * if it is used while function args are pushed
+ * and -fomit-frame-pointer is active.
+ *
+ * since I did not manage to find the real location to fix this,
+ * I wrote this patcher, which validates the spf related offsets
+ * and fixes these.
+ */
+static void
+fix_stack_regs(rtx_insn * first)
+{
+  rtx_insn * insn;
+
+  int offset = 0;
+  int size = current_frame.offset + current_frame.size;
+
+  for (insn = first; insn; insn = NEXT_INSN (insn))
+    {
+      if (CALL_P(insn))
+	{
+	  offset = 0;
+	  continue;
+	}
+
+      if (!NONJUMP_INSN_P (insn))
+	continue;
+
+      int last_offset = offset;
+      // update the offset
+      rtx note = find_reg_note(insn, REG_ARGS_SIZE, 0);
+      if (note)
+	offset = INTVAL(XEXP(note, 0));
+
+      if (last_offset == 0)
+	continue;
+
+      rtx set = single_set(insn);
+      if (!set)
+	continue;
+
+      if (GET_CODE(set) == COMPARE)
+	set = XEXP(set, 1);
+
+      fix_one(insn, &XEXP(set, 0), 0, size, last_offset);
+      fix_one(insn, &XEXP(set, 1), 1, size, last_offset);
+    }
+}
+#endif
+
+/**
+ * SBF:
+ * Search artifical regs which end up as as spilled variables plus
+ * - pushed once to the stack
+ * - read only
+ * - where reading the original isn't more expensive as reading from the stack
+ *
+ * Then delete the assignment and replace the spilled variable with the original.
+ *
+ * If at least one variable is affected, restart ira.
+ *
+ * If the register in original mem ref ends up spilled, undo that replacement.
+ *
+ */
+
+// the registers which live longer
+static std::set<rtx> prolonged_regs;
+// already resurrected - don't try again
+static std::set<rtx> forbidden_regs;
+// removed insn - needed for resurrection, DEST contains the register to use during restiore
+static std::vector<std::pair<rtx, rtx_insn *> > reg2deleted_insn;
+// modified insns
+static std::vector<std::pair<rtx, rtx_insn *> > dst2modified_insn;
+// REG_EQUAL notes
+static std::vector<std::pair<rtx_insn *, rtx> > insn2req_equals;
+
+static int prune_pass;
+
+static void
+init_prune_stack_vars ()
+{
+  prolonged_regs.clear();
+  forbidden_regs.clear();
+  reg2deleted_insn.clear();
+  dst2modified_insn.clear();
+  insn2req_equals.clear();
+  prune_pass = 0;
+
+  rtx_insn *insn;
+  for (insn = get_insns (); insn; insn = NEXT_INSN (insn))
+    if (NONJUMP_INSN_P(insn))
+      {
+	rtx note = find_reg_note (insn, REG_EQUAL, NULL_RTX);
+	if (note)
+	  insn2req_equals.push_back(std::make_pair(insn, note));
+      }
+}
+
+/**
+ * Search eliminable artifical spilled variables and replace them.
+ */
+static bool
+prune_stack_vars (bool loops_p)
+{
+  bool changed = false;
+  int regno, max_regno;
+  max_regno = max_reg_num ();
+
+
+  if (ira_dump_file )
+    fprintf(ira_dump_file, "pruning stack variables pass %d in: %s\n", prune_pass + 1, lang_hooks.decl_printable_name (current_function_decl, 2));
+
+  /*
+   * Check the longer living registers which are used in the replacement mem.
+   * If these end up spilled, it's better to live with the spilled variable,
+   * => undo the change.
+   */
+  std::set<rtx>::iterator i = prolonged_regs.begin();
+  for(;i != prolonged_regs.end(); ++i)
+    {
+      rtx prolonged_reg = *i;
+      if (REGNO(prolonged_reg) < FIRST_PSEUDO_REGISTER)
+	continue;
+
+      /* no double undo. */
+      if (forbidden_regs.find(prolonged_reg) != forbidden_regs.end())
+	continue;
+
+      if (ira_regno_allocno_map[REGNO(prolonged_reg)] && ira_regno_allocno_map[REGNO(prolonged_reg)]->hard_regno < 0)
+	{
+	  changed = true;
+	  if (ira_dump_file)
+	    {
+	      fprintf(ira_dump_file, "***************************************************\n");
+	      fprintf(ira_dump_file, "whoops: %d did not get a hard register... undoing :-)\n", REGNO(prolonged_reg));
+	    }
+
+	  /* once undone, prevent working with / undoing them again and prevent endless loops. */
+	  forbidden_regs.insert(prolonged_reg);
+
+	  std::vector<std::pair<rtx, rtx_insn *> >::iterator j = reg2deleted_insn.begin();
+	  for(; j != reg2deleted_insn.end(); ++j)
+	    {
+	      std::pair<rtx, rtx_insn *> p = *j;
+	      if (p.first != prolonged_reg)
+		continue;
+
+	      /* undo the deletion and restore the insn. */
+	      rtx pattern = PATTERN(p.second);
+	      rtx_insn * undel = emit_insn_after(pattern, p.second);
+
+	      rtx src = SET_SRC(pattern);
+	      rtx dst = SET_DEST(pattern);
+	      std::vector<std::pair<rtx, rtx_insn *> >::iterator  k = dst2modified_insn.begin();
+	      for(; k != dst2modified_insn.end(); ++k)
+		{
+		  std::pair<rtx, rtx_insn *> q = *k;
+		  if (q.first != dst)
+		    continue;
+
+		  /* undo the replacement. */
+		  rtx_insn * insn = q.second;
+		  replace_rtx(insn, src, dst, true);
+		  df_insn_rescan(insn);
+		}
+	    }
+	}
+    }
+
+  if (prune_pass <= 3)
+    {
+      // iterate over all registers.
+      for (regno = FIRST_PSEUDO_REGISTER; regno < max_regno; regno++)
+	{
+	  // this only applies to spilled variables
+	  if (!ira_regno_allocno_map[regno] || ira_regno_allocno_map[regno]->hard_regno >= 0)
+	    continue;
+
+	  if (ira_reg_equiv[regno].constant != NULL_RTX)
+	    continue;
+
+	  // which are defined once
+	  if (DF_REG_DEF_COUNT(regno) != 1)
+	    continue;
+
+	  df_ref defref;
+	  defref = DF_REG_DEF_CHAIN(regno);
+
+	  // no handling for parallel insn's for now.
+	  rtx_insn * def = defref->base.insn_info->insn;
+	  rtx set = single_set(def);
+	  if (!set)
+	    continue;
+
+	  // only consider MEM <-> MEM replacements
+	  rtx src = SET_SRC(set);
+	  if (!MEM_P(src))
+	    continue;
+
+	  rtx dst = SET_DEST(set);
+	  if (!REG_P(dst))
+	    continue;
+
+	  // only temp / artificial vars!
+	  tree var = REG_EXPR(dst);
+	  if (!var)
+	    continue;
+	  if (var->base.code != SSA_NAME)// !DECL_ARTIFICIAL (var))
+	    continue;
+
+	  /**
+	   * Handle MEM of
+	   *  - reg
+	   *  - plus(reg, int)
+	   *  - symbol_ref
+	   *  - plus(symbol_ref, int)
+	   */
+	  rtx address = XEXP(src, 0);
+	  if (!( REG_P(address)
+	      || SYMBOL_REF_P(address)
+	      || (GET_CODE(address) == PLUS
+		  && (REG_P(XEXP(address, 0))
+		      || SYMBOL_REF_P(XEXP(address,0))
+		  )
+		  && CONST_INT_P(XEXP(address, 1)))
+	     ))
+	    continue;
+
+	  rtx address_reg = 0;
+	  if (REG_P(address))
+	    address_reg = address;
+	  else if (GET_CODE(address) == PLUS && REG_P(XEXP(address, 0)))
+	    address_reg = XEXP(address, 0);
+
+	  /* we tried already to prolong the live time of this register. don't touch again. */
+	  if (address_reg && forbidden_regs.find(address_reg) != forbidden_regs.end())
+	    continue;
+
+
+	  df_ref ref;
+	  machine_mode mode = GET_MODE(dst);
+	  bool skip = false;
+	  if (address_reg)
+	    {
+	      if (DF_REG_DEF_COUNT(REGNO(address_reg)) != 1)
+		skip = true;
+	      else
+	      /* check that there is no use as dest and all uses refer to this regno. */
+	      for(ref = DF_REG_USE_CHAIN(REGNO(address_reg));ref; ref = DF_REF_NEXT_REG (ref))
+		{
+		  if (!ref->base.insn_info)
+		    continue;
+
+		  rtx_insn * insn = ref->base.insn_info->insn;
+		  rtx iset = single_set(insn);
+		  if (!iset || rtx_equal_p(src, SET_DEST(iset)))
+		    {
+		      skip = true;
+		      break;
+		    }
+
+		  rtx isrc = SET_SRC(iset);
+		  /* does it feed other vars as well? */
+		  if (rtx_equal_p(src, isrc) && !rtx_equal_p(dst, SET_DEST(iset)))
+		    {
+		      skip = true;
+		      break;
+		    }
+		}
+	    }
+
+	  if (skip)
+	    {
+	      // do not try again
+	      if (address_reg)
+		forbidden_regs.insert(address_reg);
+	      continue;
+	    }
+
+	  if (address_reg)
+	    {
+	      if (ira_dump_file)
+		fprintf(ira_dump_file, "use %d instead of ", REGNO(address_reg));
+	      prolonged_regs.insert(address_reg);
+	      reg2deleted_insn.push_back(std::make_pair(address_reg, def));
+	    }
+
+	  if (internal_flag_ira_verbose > 0 && ira_dump_file != NULL)
+	    {
+	      fprintf(ira_dump_file, "ref %d:\t", regno);
+	      df_refs_chain_dump(DF_REG_DEF_CHAIN(regno), true, ira_dump_file);
+	      df_refs_chain_dump(DF_REG_USE_CHAIN(regno), true, ira_dump_file);
+	      fprintf(ira_dump_file, "\n");
+	    }
+
+	  /* drop the assignment. */
+	  if (internal_flag_ira_verbose > 0 && ira_dump_file != NULL)
+	    {
+	      fprintf(ira_dump_file, "eliminating insn:\n");
+	      print_inline_rtx(ira_dump_file, def, 2);
+	    }
+
+	  /* replace the variable in all locations. */
+	  bool ok = true;
+	  for(ref = DF_REG_USE_CHAIN(regno);ref; ref = DF_REF_NEXT_REG (ref))
+	    {
+	      rtx_insn * insn = ref->base.insn_info->insn;
+	      if (internal_flag_ira_verbose > 0 && ira_dump_file != NULL)
+		{
+		  fprintf(ira_dump_file, "from:\n");
+		  print_inline_rtx(ira_dump_file, insn, 2);
+		}
+	      validate_replace_rtx_group(dst, src, insn);
+	      df_insn_rescan(insn);
+	      if (internal_flag_ira_verbose > 0 && ira_dump_file != NULL)
+		{
+		  fprintf(ira_dump_file, "to:\n");
+		  print_inline_rtx(ira_dump_file, insn, 2);
+		}
+	      if (address_reg)
+		dst2modified_insn.push_back(std::make_pair(dst, insn));
+	    }
+
+	  if (ok && apply_change_group())
+	    {
+   	      changed = true;
+	      SET_INSN_DELETED(def)
+	    }
+	  else
+	    {
+	      cancel_changes(0);
+	      forbidden_regs.insert(address_reg);
+	    }
+	}
+    }
+
+  if (changed)
+    {
+      /* make stats visible. */
+      if (internal_flag_ira_verbose > 0 && ira_dump_file != NULL)
+	calculate_allocation_cost ();
+
+      /* the lifetime of all registers must be reconsidered - reset what's needed. */
+      regstat_free_n_sets_and_refs ();
+      regstat_free_ri ();
+      if (loops_p)
+	loop_optimizer_finalize ();
+      free_dominance_info (CDI_DOMINATORS);
+
+      /* plus restore the REG_EQUAL notes which were recorded during init_prune_stack_vars() ! */
+      std::vector<std::pair<rtx_insn *, rtx> >::iterator i2r = insn2req_equals.begin();
+      for (;i2r != insn2req_equals.end(); ++i2r)
+	{
+	  rtx_insn * insn = i2r->first;
+	  REG_NOTES (insn) = i2r->second;
+	}
+
+      df_mark_solutions_dirty();
+      df_analyze ();
+    }
+
+  ++ prune_pass;
+  return changed;
+}
+
 /* If the backend knows where to allocate pseudos for hard
    register initial values, register these allocations now.  */
 static void
@@ -5183,8 +5607,8 @@ ira (FILE *f)
     }
 
 #ifndef IRA_NO_OBSTACK
-  gcc_obstack_init (&ira_obstack);
-#endif
+    gcc_obstack_init (&ira_obstack);
+  #endif
   bitmap_obstack_initialize (&ira_bitmap_obstack);
 
   /* LRA uses its own infrastructure to handle caller save registers.  */
@@ -5207,92 +5631,101 @@ ira (FILE *f)
   df_note_add_problem ();
 
   /* DF_LIVE can't be used in the register allocator, too many other
-     parts of the compiler depend on using the "classic" liveness
-     interpretation of the DF_LR problem.  See PR38711.
-     Remove the problem, so that we don't spend time updating it in
-     any of the df_analyze() calls during IRA/LRA.  */
-  if (optimize > 1)
+   parts of the compiler depend on using the "classic" liveness
+   interpretation of the DF_LR problem.  See PR38711.
+   Remove the problem, so that we don't spend time updating it in
+   any of the df_analyze() calls during IRA/LRA.  */
+  if (optimize >= 2)          //
     df_remove_problem (df_live);
-  gcc_checking_assert (df_live == NULL);
+  gcc_checking_assert(df_live == NULL);
 
   if (flag_checking)
     df->changeable_flags |= DF_VERIFY_SCHEDULED;
 
   df_analyze ();
 
-  init_reg_equiv ();
-  if (ira_conflicts_p)
+  if (flag_prune_stack_vars)
+    init_prune_stack_vars ();
+  do
     {
-      calculate_dominance_info (CDI_DOMINATORS);
-
-      if (split_live_ranges_for_shrink_wrap ())
-	df_analyze ();
-
-      free_dominance_info (CDI_DOMINATORS);
-    }
-
-  df_clear_flags (DF_NO_INSN_RESCAN);
-
-  indirect_jump_optimize ();
-  if (delete_trivially_dead_insns (get_insns (), max_reg_num ()))
-    df_analyze ();
-
-  regstat_init_n_sets_and_refs ();
-  regstat_compute_ri ();
+      init_reg_equiv ();
+      if (ira_conflicts_p)
+	{
+	  calculate_dominance_info (CDI_DOMINATORS);
 
-  /* If we are not optimizing, then this is the only place before
-     register allocation where dataflow is done.  And that is needed
-     to generate these warnings.  */
-  if (warn_clobbered)
-    generate_setjmp_warnings ();
-
-  /* Determine if the current function is a leaf before running IRA
-     since this can impact optimizations done by the prologue and
-     epilogue thus changing register elimination offsets.  */
-  crtl->is_leaf = leaf_function_p ();
-
-  if (resize_reg_info () && flag_ira_loop_pressure)
-    ira_set_pseudo_classes (true, ira_dump_file);
-
-  update_equiv_regs ();
-  setup_reg_equiv ();
-  setup_reg_equiv_init ();
-
-  allocated_reg_info_size = max_reg_num ();
-
-  /* It is not worth to do such improvement when we use a simple
-     allocation because of -O0 usage or because the function is too
-     big.  */
-  if (ira_conflicts_p)
-    find_moveable_pseudos ();
+	  if (split_live_ranges_for_shrink_wrap ())
+	    df_analyze ();
 
-  max_regno_before_ira = max_reg_num ();
-  ira_setup_eliminable_regset ();
+	  free_dominance_info (CDI_DOMINATORS);
+	}
 
-  ira_overall_cost = ira_reg_cost = ira_mem_cost = 0;
-  ira_load_cost = ira_store_cost = ira_shuffle_cost = 0;
-  ira_move_loops_num = ira_additional_jumps_num = 0;
+      df_clear_flags (DF_NO_INSN_RESCAN);
 
-  ira_assert (current_loops == NULL);
-  if (flag_ira_region == IRA_REGION_ALL || flag_ira_region == IRA_REGION_MIXED)
-    loop_optimizer_init (AVOID_CFG_MODIFICATIONS | LOOPS_HAVE_RECORDED_EXITS);
+      indirect_jump_optimize ();
+      if (delete_trivially_dead_insns (get_insns (), max_reg_num ()))
+	df_analyze ();
 
-  if (internal_flag_ira_verbose > 0 && ira_dump_file != NULL)
-    fprintf (ira_dump_file, "Building IRA IR\n");
-  loops_p = ira_build ();
+      regstat_init_n_sets_and_refs ();
+      regstat_compute_ri ();
 
-  ira_assert (ira_conflicts_p || !loops_p);
+      /* If we are not optimizing, then this is the only place before
+       register allocation where dataflow is done.  And that is needed
+       to generate these warnings.  */
+      if (warn_clobbered)
+	generate_setjmp_warnings ();
+
+      /* Determine if the current function is a leaf before running IRA
+       since this can impact optimizations done by the prologue and
+       epilogue thus changing register elimination offsets.  */
+      crtl->is_leaf = leaf_function_p ();
+
+      if (resize_reg_info () && flag_ira_loop_pressure)
+	ira_set_pseudo_classes (true, ira_dump_file);
+
+      update_equiv_regs ();
+      setup_reg_equiv ();
+      setup_reg_equiv_init ();
+
+      allocated_reg_info_size = max_reg_num ();
+
+      /* It is not worth to do such improvement when we use a simple
+       allocation because of -O0 usage or because the function is too
+       big.  */
+      if (ira_conflicts_p)
+	find_moveable_pseudos ();
+
+      max_regno_before_ira = max_reg_num ();
+      ira_setup_eliminable_regset ();
+
+      ira_overall_cost = ira_reg_cost = ira_mem_cost = 0;
+      ira_load_cost = ira_store_cost = ira_shuffle_cost = 0;
+      ira_move_loops_num = ira_additional_jumps_num = 0;
+
+      ira_assert(current_loops == NULL);
+      if (flag_ira_region == IRA_REGION_ALL
+	  || flag_ira_region == IRA_REGION_MIXED)
+	loop_optimizer_init (
+	    AVOID_CFG_MODIFICATIONS | LOOPS_HAVE_RECORDED_EXITS);
+
+      if (internal_flag_ira_verbose > 0 && ira_dump_file != NULL)
+	fprintf (ira_dump_file, "Building IRA IR\n");
+      loops_p = ira_build ();
+
+      ira_assert(ira_conflicts_p || !loops_p);
+
+      saved_flag_ira_share_spill_slots = flag_ira_share_spill_slots;
+      if (too_high_register_pressure_p () || cfun->calls_setjmp)
+	/* It is just wasting compiler's time to pack spilled pseudos into
+	 stack slots in this case -- prohibit it.  We also do this if
+	 there is setjmp call because a variable not modified between
+	 setjmp and longjmp the compiler is required to preserve its
+	 value and sharing slots does not guarantee it.  */
+	flag_ira_share_spill_slots = FALSE;
 
-  saved_flag_ira_share_spill_slots = flag_ira_share_spill_slots;
-  if (too_high_register_pressure_p () || cfun->calls_setjmp)
-    /* It is just wasting compiler's time to pack spilled pseudos into
-       stack slots in this case -- prohibit it.  We also do this if
-       there is setjmp call because a variable not modified between
-       setjmp and longjmp the compiler is required to preserve its
-       value and sharing slots does not guarantee it.  */
-    flag_ira_share_spill_slots = FALSE;
+      ira_color ();
 
-  ira_color ();
+    }
+  while (flag_prune_stack_vars && prune_stack_vars (loops_p));
 
   ira_max_point_before_emit = ira_max_point;
 
@@ -5303,9 +5736,9 @@ ira (FILE *f)
   max_regno = max_reg_num ();
   if (ira_conflicts_p)
     {
-      if (! loops_p)
+      if (!loops_p)
 	{
-	  if (! ira_use_lra_p)
+	  if (!ira_use_lra_p)
 	    ira_initiate_assign ();
 	}
       else
@@ -5318,17 +5751,18 @@ ira (FILE *f)
 	      ira_allocno_iterator ai;
 
 	      FOR_EACH_ALLOCNO (a, ai)
-                {
-                  int old_regno = ALLOCNO_REGNO (a);
-                  int new_regno = REGNO (ALLOCNO_EMIT_DATA (a)->reg);
-
-                  ALLOCNO_REGNO (a) = new_regno;
-
-                  if (old_regno != new_regno)
-                    setup_reg_classes (new_regno, reg_preferred_class (old_regno),
-                                       reg_alternate_class (old_regno),
-                                       reg_allocno_class (old_regno));
-                }
+		{
+		  int old_regno = ALLOCNO_REGNO(a);
+		  int new_regno = REGNO(ALLOCNO_EMIT_DATA (a)->reg);
+
+		  ALLOCNO_REGNO (a) = new_regno;
+
+		  if (old_regno != new_regno)
+		    setup_reg_classes (new_regno,
+				       reg_preferred_class (old_regno),
+				       reg_alternate_class (old_regno),
+				       reg_allocno_class (old_regno));
+		}
 
 	    }
 	  else
@@ -5338,18 +5772,18 @@ ira (FILE *f)
 	      ira_flattening (max_regno_before_ira, ira_max_point_before_emit);
 	    }
 	  /* New insns were generated: add notes and recalculate live
-	     info.  */
+	   info.  */
 	  df_analyze ();
 
 	  /* ??? Rebuild the loop tree, but why?  Does the loop tree
-	     change if new insns were generated?  Can that be handled
-	     by updating the loop tree incrementally?  */
+	   change if new insns were generated?  Can that be handled
+	   by updating the loop tree incrementally?  */
 	  loop_optimizer_finalize ();
 	  free_dominance_info (CDI_DOMINATORS);
-	  loop_optimizer_init (AVOID_CFG_MODIFICATIONS
-			       | LOOPS_HAVE_RECORDED_EXITS);
+	  loop_optimizer_init (
+	      AVOID_CFG_MODIFICATIONS | LOOPS_HAVE_RECORDED_EXITS);
 
-	  if (! ira_use_lra_p)
+	  if (!ira_use_lra_p)
 	    {
 	      setup_allocno_assignment_flags ();
 	      ira_initiate_assign ();
@@ -5398,6 +5832,7 @@ ira (FILE *f)
 		  max_regno * sizeof (struct ira_spilled_reg_stack_slot));
 	}
     }
+
   allocate_initial_values ();
 
   /* See comment for find_moveable_pseudos call.  */
@@ -5456,6 +5891,10 @@ do_reload (void)
       build_insn_chain ();
 
       need_dce = reload (get_insns (), ira_conflicts_p);
+
+#if defined(TARGET_M68K)
+      fix_stack_regs(get_insns ());
+#endif
     }
 
   timevar_pop (TV_RELOAD);
@@ -5509,7 +5948,7 @@ do_reload (void)
   df_scan_alloc (NULL);
   df_scan_blocks ();
 
-  if (optimize > 1)
+  if (optimize >= 2)//
     {
       df_live_add_problem ();
       df_live_set_all_dirty ();

--- gcc/ira-color.c	2016-03-31 18:51:13.000000000 +0100
+++ gcc/ira-color.c	2025-11-02 00:56:38.274283287 +0000
@@ -4126,6 +4126,7 @@ coalesce_spill_slots (ira_allocno_t *spi
   return merged_p;
 }
 
+
 /* Sort pseudo-register numbers in array PSEUDO_REGNOS of length N for
    subsequent assigning stack slots to them in the reload pass.  To do
    this we coalesce spilled allocnos first to decrease the number of

--- gcc/ira-costs.c	2017-02-15 07:04:05.000000000 +0000
+++ gcc/ira-costs.c	2025-11-02 00:56:38.274283287 +0000
@@ -1245,6 +1245,8 @@ record_address_regs (machine_mode mode,
 	  {
 	    i = cost_classes[k];
 	    add_cost = (move_in_cost[i][rclass] * scale) / 2;
+	    if (ADDR_REGS != i)
+	      ++add_cost;
 	    if (INT_MAX - add_cost < pp_costs[k])
 	      pp_costs[k] = INT_MAX;
 	    else 
@@ -1461,6 +1463,12 @@ scan_one_insn (rtx_insn *insn)
 
       COSTS (costs, num)->mem_cost
 	-= ira_memory_move_cost[GET_MODE (reg)][cl][1] * frequency;
+#if defined(TARGET_M68K)
+      /* SBF: can result in negative costs which is no good.
+       * => keep*/
+      if (COSTS (costs, num)->mem_cost < ira_memory_move_cost[GET_MODE (reg)][cl][1] * frequency / 2)
+	COSTS (costs, num)->mem_cost = ira_memory_move_cost[GET_MODE (reg)][cl][1] * frequency / 2;
+#endif
       record_address_regs (GET_MODE (SET_SRC (set)),
 			   MEM_ADDR_SPACE (SET_SRC (set)),
 			   XEXP (SET_SRC (set), 0), 0, MEM, SCRATCH,

--- gcc/loop-doloop.c	2017-05-05 22:44:21.000000000 +0100
+++ gcc/loop-doloop.c	2025-11-02 00:56:38.306283771 +0000
@@ -302,6 +302,24 @@ doloop_valid_p (struct loop *loop, struc
       goto cleanup;
     }
 
+#if defined(TARGET_AMIGAOS)
+  /* SBF: avoid const pic stuff, yields invalid insns. */
+  if (desc->noloop_assumptions)
+    {
+      rtx cmp = XEXP (desc->noloop_assumptions, 0);
+      if (COMPARISON_P (cmp))
+	{
+	  rtx b = XEXP (cmp, 0);
+	  extern bool amigaos_legitimate_src (rtx);
+	  if (!amigaos_legitimate_src (b))
+	    {
+	      result = false;
+	      goto cleanup;
+	    }
+	}
+    }
+#endif
+
   for (i = 0; i < loop->num_nodes; i++)
     {
       bb = body[i];

--- gcc/loop-iv.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/loop-iv.c	2025-11-02 00:56:38.306283771 +0000
@@ -1421,7 +1421,10 @@ find_single_def_src (unsigned int regno)
     }
   if (!function_invariant_p (src))
     return NULL_RTX;
-
+#if defined(TARGET_AMIGAOS)
+  if (amiga_is_const_pic_ref (src))
+    return NULL_RTX;
+#endif
   return src;
 }
 

--- gcc/lto-compress.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/lto-compress.c	2025-11-02 00:56:38.314283892 +0000
@@ -266,7 +266,11 @@ lto_end_uncompression (struct lto_compre
   gcc_assert (!stream->is_compression);
   timevar_push (TV_IPA_LTO_DECOMPRESS);
 
+#if defined(TARGET_AMIGAOS)
+  while (remaining > 3)
+#else
   while (remaining > 0)
+#endif
     {
       z_stream in_stream;
       size_t out_bytes;

--- gcc/Makefile.in	2017-05-05 22:56:33.000000000 +0100
+++ gcc/Makefile.in	2025-11-02 00:56:37.274268147 +0000
@@ -385,7 +385,17 @@ enable_host_shared = @enable_host_shared
 
 enable_as_accelerator = @enable_as_accelerator@
 
+ifeq ($(DLL),)
 CPPLIB = ../libcpp/libcpp.a
+else
+CPPLIB = libcpp.dll
+
+LDFLAGS += -Xlinker --allow-multiple-definition
+
+DLLYANG = dllyang.o
+DLLYING = dllying.o
+
+endif
 CPPINC = -I$(srcdir)/../libcpp/include
 
 # Where to find decNumber
@@ -393,12 +403,20 @@ enable_decimal_float = @enable_decimal_f
 DECNUM = $(srcdir)/../libdecnumber
 DECNUMFMT = $(srcdir)/../libdecnumber/$(enable_decimal_float)
 DECNUMINC = -I$(DECNUM) -I$(DECNUMFMT) -I../libdecnumber
+ifeq ($(DLL),)
 LIBDECNUMBER = ../libdecnumber/libdecnumber.a
+else
+LIBDECNUMBER = libdecnumber.dll
+endif
 
 # The backtrace library.
 BACKTRACE = $(srcdir)/../libbacktrace
 BACKTRACEINC = -I$(BACKTRACE)
+ifeq ($(DLL),)
 LIBBACKTRACE = ../libbacktrace/.libs/libbacktrace.a
+else
+LIBBACKTRACE = libbacktrace.dll
+endif
 
 # Target to use when installing include directory.  Either
 # install-headers-tar, install-headers-cpio or install-headers-cp.
@@ -1018,12 +1036,29 @@ ifeq ($(enable_host_shared),yes)
 LIBIBERTY = ../libiberty/pic/libiberty.a
 BUILD_LIBIBERTY = $(build_libobjdir)/libiberty/pic/libiberty.a
 else
+
+ifeq ($(DLL),)
 LIBIBERTY = ../libiberty/libiberty.a
+else
+LIBIBERTY = libiberty.dll
+endif
+
+ifeq ($(DLL),)
+LIBCOMMON = libcommon.a
+LIBCOMMON_TARGET = libcommon-target.a
+LIBBACKEND = libbackend.a
+else
+LIBCOMMON = libcommon.dll
+LIBCOMMON_TARGET = libcommon-target.dll
+LIBBACKEND = libbackend.dll
+endif
+
+
 BUILD_LIBIBERTY = $(build_libobjdir)/libiberty/libiberty.a
 endif
 
 # Dependencies on the intl and portability libraries.
-LIBDEPS= libcommon.a $(CPPLIB) $(LIBIBERTY) $(LIBINTL_DEP) $(LIBICONV_DEP) \
+LIBDEPS= $(LIBCOMMON) $(CPPLIB) $(LIBIBERTY) $(LIBINTL_DEP) $(LIBICONV_DEP) \
 	$(LIBDECNUMBER) $(LIBBACKTRACE)
 
 # Likewise, for use in the tools that must run on this machine
@@ -1032,7 +1067,7 @@ BUILD_LIBDEPS= $(BUILD_LIBIBERTY)
 
 # How to link with both our special library facilities
 # and the system's installed libraries.
-LIBS = @LIBS@ libcommon.a $(CPPLIB) $(LIBINTL) $(LIBICONV) $(LIBBACKTRACE) \
+LIBS = @LIBS@ $(LIBCOMMON) $(CPPLIB) $(LIBINTL) $(LIBICONV) $(LIBBACKTRACE) \
 	$(LIBIBERTY) $(LIBDECNUMBER) $(HOST_LIBS)
 BACKENDLIBS = $(ISLLIBS) $(GMPLIBS) $(PLUGINLIBS) $(HOST_LIBS) \
 	$(ZLIB)
@@ -1171,7 +1206,7 @@ C_COMMON_OBJS = c-family/c-common.o c-fa
   c-family/c-ppoutput.o c-family/c-pragma.o c-family/c-pretty-print.o \
   c-family/c-semantics.o c-family/c-ada-spec.o \
   c-family/c-cilkplus.o \
-  c-family/array-notation-common.o c-family/cilk.o c-family/c-ubsan.o
+  c-family/array-notation-common.o c-family/cilk.o c-family/c-ubsan.o $(DLLYING)
 
 # Language-independent object files.
 # We put the *-match.o and insn-*.o files first so that a parallel make
@@ -1199,6 +1234,8 @@ OBJS = \
 	auto-inc-dec.o \
 	auto-profile.o \
 	bb-reorder.o \
+	bbb-baserel.o \
+	bbb-opts.o \
 	bitmap.o \
 	bt-load.o \
 	builtins.o \
@@ -1537,7 +1574,8 @@ OBJS = \
 	xcoffout.o \
 	$(out_object_file) \
 	$(EXTRA_OBJS) \
-	$(host_hook_obj)
+	$(host_hook_obj) \
+	$(DLLYING) $(DLLYANG)
 
 # Objects in libcommon.a, potentially used by all host binaries and with
 # no target dependencies.
@@ -1564,7 +1602,7 @@ ALL_HOST_BACKEND_OBJS = $(GCC_OBJS) $(OB
 # compilation or not.
 ALL_HOST_OBJS = $(ALL_HOST_FRONTEND_OBJS) $(ALL_HOST_BACKEND_OBJS)
 
-BACKEND = libbackend.a main.o libcommon-target.a libcommon.a \
+BACKEND = $(LIBBACKEND) main.o $(LIBCOMMON_TARGET) $(LIBCOMMON) \
 	$(CPPLIB) $(LIBDECNUMBER)
 
 # This is defined to "yes" if Tree checking is enabled, which roughly means
@@ -1588,8 +1626,8 @@ MOSTLYCLEANFILES = insn-flags.h insn-con
  gcc-ranlib$(exeext) \
  gcov-iov$(build_exeext) gcov$(exeext) gcov-dump$(exeext) \
  gcov-tool$(exeect) \
- gengtype$(exeext) *.[0-9][0-9].* *.[si] *-checksum.c libbackend.a \
- libcommon-target.a libcommon.a libgcc.mk
+ gengtype$(exeext) *.[0-9][0-9].* *.[si] *-checksum.c $(LIBBACKEND) \
+ $(LIBCOMMON_TARGET) libcommon.a libgcc.mk
 
 # This symlink makes the full installation name of the driver be available
 # from within the *build* directory, for use when running the JIT library
@@ -1849,32 +1887,53 @@ libbackend.a: $(OBJS)
 	$(AR) $(AR_FLAGS) libbackend.a $(OBJS)
 	-$(RANLIB) $(RANLIB_FLAGS) libbackend.a
 
+libbackend.dll: $(OBJS) libcommon.dll libcommon-target.dll libdecnumber.dll libiberty.dll
+	$(CXX) -shared -o $@ $(OBJS) libiberty.dll libcommon.dll libcommon-target.dll libdecnumber.dll -lz -lmpc -lmpfr -lgmp dllyang.o -Xlinker --allow-multiple-definition 
+
 libcommon-target.a: $(OBJS-libcommon-target)
 	-rm -rf libcommon-target.a
 	$(AR) $(AR_FLAGS) libcommon-target.a $(OBJS-libcommon-target)
 	-$(RANLIB) $(RANLIB_FLAGS) libcommon-target.a
 
+libcommon-target.dll: $(OBJS-libcommon-target)
+	$(CXX) -shared -o $@ $(OBJS-libcommon-target) libiberty.dll libcommon.dll
+
 libcommon.a: $(OBJS-libcommon)
 	-rm -rf libcommon.a
 	$(AR) $(AR_FLAGS) libcommon.a $(OBJS-libcommon)
 	-$(RANLIB) $(RANLIB_FLAGS) libcommon.a
+	
+libcommon.dll: $(OBJS-libcommon) libiberty.dll
+	$(MAKE) -C ../libbacktrace/ libbacktrace.dll DLL=1 
+	cp ../libbacktrace/libbacktrace.dll .
+	$(CXX) -shared -o $@ $(OBJS-libcommon) libiberty.dll libbacktrace.dll ../libcpp/libcpp.a -liconv
+	make -C ../libcpp/ libcpp.dll DLL=1
+	cp ../libcpp/libcpp.dll .
+
+libdecnumber.dll:
+	$(MAKE) -C ../libdecnumber libdecnumber.dll DLL=1
+	cp ../libdecnumber/libdecnumber.dll .
+
+libiberty.dll:
+	$(MAKE) -C ../libiberty/ libiberty.dll DLL=1 
+	cp ../libiberty/libiberty.dll .
 
 # We call this executable `xgcc' rather than `gcc'
 # to avoid confusion if the current directory is in the path
 # and CC is `gcc'.  It is renamed to `gcc' when it is installed.
-xgcc$(exeext): $(GCC_OBJS) c/gccspec.o libcommon-target.a $(LIBDEPS) \
+xgcc$(exeext): $(GCC_OBJS) c/gccspec.o $(LIBCOMMON_TARGET) $(LIBDEPS) \
 	$(EXTRA_GCC_OBJS)
 	+$(LINKER) $(ALL_LINKERFLAGS) $(LDFLAGS) -o $@ $(GCC_OBJS) \
-	  c/gccspec.o $(EXTRA_GCC_OBJS) libcommon-target.a \
+	  c/gccspec.o $(EXTRA_GCC_OBJS) $(LIBCOMMON_TARGET) \
 	  $(EXTRA_GCC_LIBS) $(LIBS)
 
 # cpp is to cpp0 as e.g. g++ is to cc1plus: Just another driver.
 # It is part of c-family because the handled extensions are hard-coded
 # and only contain c-family extensions (see known_suffixes).
-cpp$(exeext): $(GCC_OBJS) c-family/cppspec.o libcommon-target.a $(LIBDEPS) \
+cpp$(exeext): $(GCC_OBJS) c-family/cppspec.o $(LIBCOMMON_TARGET) $(LIBDEPS) \
 	$(EXTRA_GCC_OBJS)
 	+$(LINKER) $(ALL_LINKERFLAGS) $(LDFLAGS) -o $@ $(GCC_OBJS) \
-	  c-family/cppspec.o $(EXTRA_GCC_OBJS) libcommon-target.a \
+	  c-family/cppspec.o $(EXTRA_GCC_OBJS) $(LIBCOMMON_TARGET) \
 	  $(EXTRA_GCC_LIBS) $(LIBS)
 
 # Dump a specs file to make -B./ read these specs over installed ones.
@@ -1986,7 +2045,7 @@ gcc-nm.c: gcc-ar.c
 	cp $^ $@
 
 COLLECT2_OBJS = collect2.o collect2-aix.o tlink.o vec.o ggc-none.o \
-  collect-utils.o file-find.o hash-table.o
+  collect-utils.o file-find.o hash-table.o $(EXTRA_COLLECT2_OBJS)
 COLLECT2_LIBS = @COLLECT2_LIBS@
 collect2$(exeext): $(COLLECT2_OBJS) $(LIBDEPS)
 # Don't try modifying collect2 (aka ld) in place--it might be linking this.
@@ -1998,9 +2057,9 @@ CFLAGS-collect2.o += -DTARGET_MACHINE=\"
 	@TARGET_SYSTEM_ROOT_DEFINE@
 
 LTO_WRAPPER_OBJS = lto-wrapper.o collect-utils.o ggc-none.o
-lto-wrapper$(exeext): $(LTO_WRAPPER_OBJS) libcommon-target.a $(LIBDEPS)
+lto-wrapper$(exeext): $(LTO_WRAPPER_OBJS) $(LIBCOMMON_TARGET) $(LIBDEPS)
 	+$(LINKER) $(ALL_COMPILERFLAGS) $(LDFLAGS) -o T$@ \
-	   $(LTO_WRAPPER_OBJS) libcommon-target.a $(LIBS)
+	   $(LTO_WRAPPER_OBJS) $(LIBCOMMON_TARGET) $(LIBS)
 	mv -f T$@ $@
 
 # Files used by all variants of C or by the stand-alone pre-processor.
@@ -3270,7 +3329,7 @@ endif
 install-strip: install
 
 # Handle cpp installation.
-install-cpp: installdirs cpp$(exeext)
+install-cpp: installdirs cpp$(exeext) all.cross
 	-if test "$(enable_as_accelerator)" != "yes" ; then \
 	  rm -f $(DESTDIR)$(bindir)/$(CPP_INSTALL_NAME)$(exeext); \
 	  $(INSTALL_PROGRAM) -m 755 cpp$(exeext) $(DESTDIR)$(bindir)/$(CPP_INSTALL_NAME)$(exeext); \

--- gcc/match.pd	2017-06-22 08:30:03.000000000 +0100
+++ gcc/match.pd	2025-11-02 00:56:38.318283953 +0000
@@ -1543,7 +1543,8 @@ DEFINE_INT_AND_FLOAT_ROUND_FN (RINT)
 
     /* A truncation to an unsigned type (a zero-extension) should be
        canonicalized as bitwise and of a mask.  */
-    (if (GIMPLE /* PR70366: doing this in GENERIC breaks -Wconversion.  */
+    /* SBF: keep the different types widening_mul relies on that! * /
+    (if (GIMPLE /* PR70366: doing this in GENERIC breaks -Wconversion.  * /
 	 && final_int && inter_int && inside_int
 	 && final_prec == inside_prec
 	 && final_prec > inter_prec
@@ -1552,6 +1553,7 @@ DEFINE_INT_AND_FLOAT_ROUND_FN (RINT)
 	                      (inside_type,
 			       wi::mask (inter_prec, false,
 					 TYPE_PRECISION (inside_type))); })))
+	*/
 
     /* If we are converting an integer to a floating-point that can
        represent it exactly and back to an integer, we can skip the
@@ -3041,3 +3043,41 @@ DEFINE_INT_AND_FLOAT_ROUND_FN (RINT)
  (SIGNBIT @0)
  (if (!HONOR_SIGNED_ZEROS (@0))
   (convert (lt @0 { build_real (TREE_TYPE (@0), dconst0); }))))
+
+
+/* use mul 2 instead of 2 adds. 
+  n + n = n * 2
+*/
+(simplify
+  (plus:c @0 @0)
+  (if (!FLOAT_TYPE_P (type))
+    (mult @0 { build_int_cst (integer_type_node, 2); }))
+)
+
+/* use mul 2
+  a + n + n = a + n * 2
+ */
+(simplify
+  (plus:c (plus:c @0 @1) @1)
+  (if (!FLOAT_TYPE_P (type))
+    (plus @0 (mult @1 { build_int_cst (integer_type_node, 2); })))
+)
+
+
+/* use mul 2 instead of 3 muls. 
+  n * n * n * n = (n * n) * (n * n)
+                => t = (n * n)
+                = t * t
+*/
+(simplify
+  (mult:c (mult:c (mult:c @0 @0) @0) @0)
+    (with
+      { tree tem = fold_build2 (MULT_EXPR, type, @0, @0); }
+      (mult { tem; } { tem; })))
+
+(simplify
+  (mult:c (mult:c (mult:c (mult:c @0 @1) @1) @1) @1)
+    (with
+      { tree tem = fold_build2 (MULT_EXPR, type, @1, @1); }
+      (mult @0 (mult { tem; } { tem; }))))
+      
\ No newline at end of file

--- gcc/optabs.c	2016-02-16 15:15:40.000000000 +0000
+++ gcc/optabs.c	2025-11-02 00:56:38.334284195 +0000
@@ -4829,7 +4829,11 @@ expand_fix (rtx to, rtx from, int unsign
 	    if (fmode != GET_MODE (from))
 	      from = convert_to_mode (fmode, from, 0);
 
-	    if (must_trunc)
+	    if (must_trunc
+#if defined(TARGET_M68K)
+		&& !flag_no_truncz_same_mode
+#endif
+		)
 	      {
 		rtx temp = gen_reg_rtx (GET_MODE (from));
 		from = expand_unop (GET_MODE (from), ftrunc_optab, from,

--- gcc/opts.c	2018-03-07 09:49:47.000000000 +0000
+++ gcc/opts.c	2025-11-02 00:56:38.334284195 +0000
@@ -381,8 +381,8 @@ maybe_default_option (struct gcc_options
       enabled = (level >= 3);
       break;
 
-    case OPT_LEVELS_3_PLUS_AND_SIZE:
-      enabled = (level >= 3 || size);
+    case OPT_LEVELS_3_PLUS_AND_SPEED_ONLY:
+      enabled = (level >= 3 && !size && !debug);
       break;
 
     case OPT_LEVELS_SIZE:
@@ -452,7 +452,7 @@ static const struct default_options defa
     { OPT_LEVELS_1_PLUS, OPT_fipa_reference, NULL, 1 },
     { OPT_LEVELS_1_PLUS, OPT_fipa_profile, NULL, 1 },
     { OPT_LEVELS_1_PLUS, OPT_fmerge_constants, NULL, 1 },
-    { OPT_LEVELS_1_PLUS, OPT_freorder_blocks, NULL, 1 },
+    { OPT_LEVELS_1_PLUS_SPEED_ONLY, OPT_freorder_blocks, NULL, 1 },
     { OPT_LEVELS_1_PLUS, OPT_fshrink_wrap, NULL, 1 },
     { OPT_LEVELS_1_PLUS, OPT_fsplit_wide_types, NULL, 1 },
     { OPT_LEVELS_1_PLUS, OPT_ftree_ccp, NULL, 1 },
@@ -476,6 +476,7 @@ static const struct default_options defa
     { OPT_LEVELS_1_PLUS_NOT_DEBUG, OPT_fssa_phiopt, NULL, 1 },
     { OPT_LEVELS_1_PLUS, OPT_ftree_builtin_call_dce, NULL, 1 },
 
+
     /* -O2 optimizations.  */
     { OPT_LEVELS_2_PLUS, OPT_finline_small_functions, NULL, 1 },
     { OPT_LEVELS_2_PLUS, OPT_findirect_inlining, NULL, 1 },
@@ -496,8 +497,13 @@ static const struct default_options defa
 #endif
     { OPT_LEVELS_2_PLUS, OPT_fstrict_aliasing, NULL, 1 },
     { OPT_LEVELS_2_PLUS, OPT_fstrict_overflow, NULL, 1 },
-    { OPT_LEVELS_2_PLUS_SPEED_ONLY, OPT_freorder_blocks_algorithm_, NULL,
-      REORDER_BLOCKS_ALGORITHM_STC },
+    { OPT_LEVELS_2_PLUS, OPT_freorder_blocks_algorithm_, NULL,
+//#ifdef TARGET_M68K
+//      REORDER_BLOCKS_ALGORITHM_SIMPLE
+//#else
+      REORDER_BLOCKS_ALGORITHM_STC
+//#endif
+    },
     { OPT_LEVELS_2_PLUS, OPT_freorder_functions, NULL, 1 },
     { OPT_LEVELS_2_PLUS, OPT_ftree_vrp, NULL, 1 },
     { OPT_LEVELS_2_PLUS, OPT_ftree_pre, NULL, 1 },
@@ -524,14 +530,16 @@ static const struct default_options defa
     { OPT_LEVELS_3_PLUS, OPT_ftree_loop_distribute_patterns, NULL, 1 },
     { OPT_LEVELS_3_PLUS, OPT_fpredictive_commoning, NULL, 1 },
     { OPT_LEVELS_3_PLUS, OPT_fsplit_paths, NULL, 1 },
-    /* Inlining of functions reducing size is a good idea with -Os
+    /* Inlining of functions is ALWAYS a good idea with -O3
        regardless of them being declared inline.  */
-    { OPT_LEVELS_3_PLUS_AND_SIZE, OPT_finline_functions, NULL, 1 },
+    { OPT_LEVELS_3_PLUS_AND_SPEED_ONLY, OPT_finline_functions, NULL, 1 },
     { OPT_LEVELS_1_PLUS_NOT_DEBUG, OPT_finline_functions_called_once, NULL, 1 },
     { OPT_LEVELS_3_PLUS, OPT_funswitch_loops, NULL, 1 },
     { OPT_LEVELS_3_PLUS, OPT_fgcse_after_reload, NULL, 1 },
+#if !defined(TARGET_M68K)
     { OPT_LEVELS_3_PLUS, OPT_ftree_loop_vectorize, NULL, 1 },
     { OPT_LEVELS_3_PLUS, OPT_ftree_slp_vectorize, NULL, 1 },
+#endif
     { OPT_LEVELS_3_PLUS, OPT_fvect_cost_model_, NULL, VECT_COST_MODEL_DYNAMIC },
     { OPT_LEVELS_3_PLUS, OPT_fipa_cp_clone, NULL, 1 },
     { OPT_LEVELS_3_PLUS, OPT_ftree_partial_pre, NULL, 1 },
@@ -1024,6 +1032,11 @@ wrap_help (const char *help,
 	    {
 	      if (i >= room && len != remaining)
 		break;
+	      if (help[i] == '\n')
+		{
+		  len = i;
+		  break;
+		}
 	      if (help[i] == ' ')
 		len = i;
 	      else if ((help[i] == '-' || help[i] == '/')
@@ -1035,7 +1048,7 @@ wrap_help (const char *help,
 
       printf ("  %-*.*s %.*s\n", col_width, item_width, item, len, help);
       item_width = 0;
-      while (help[len] == ' ')
+      while (help[len] == ' ' || help[len] == '\n')
 	len++;
       help += len;
       remaining -= len;

--- gcc/params.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/params.c	2025-11-02 00:56:38.334284195 +0000
@@ -45,6 +45,9 @@ static bool params_finished;
 #undef DEFPARAMENUM5
 #undef DEFPARAM
 
+#ifdef __amiga__
+__near
+#endif
 static const param_info lang_independent_params[] = {
 #define DEFPARAM(ENUM, OPTION, HELP, DEFAULT, MIN, MAX) \
   { OPTION, DEFAULT, MIN, MAX, HELP, NULL },

--- gcc/passes.c	2016-03-31 11:42:55.000000000 +0100
+++ gcc/passes.c	2025-11-02 00:56:38.338284256 +0000
@@ -2269,6 +2269,29 @@ override_gate_status (opt_pass *pass, tr
 }
 
 
+void dump_insns(char const * name)
+{
+  rtx_insn *insn, *next;
+  fprintf(stderr, "====================================\npass: %s\n", name);
+  for (insn = get_insns(); insn; insn = next)
+    {
+      next = NEXT_INSN(insn);
+	    debug_rtx(insn);
+#if 0
+      if (NONJUMP_INSN_P (insn))
+	{
+	  rtx set= single_set (insn);
+	  if (!set)
+	    continue;
+
+	  if (amiga_is_const_pic_ref(SET_SRC(set)) && MEM_P(SET_DEST(set)))
+	    debug_rtx(insn);
+	}
+#endif
+    }
+}
+
+
 /* Execute PASS. */
 
 bool
@@ -2278,6 +2301,9 @@ execute_one_pass (opt_pass *pass)
 
   bool gate_status;
 
+  if (string_bbb_opts && strchr (string_bbb_opts, 'Y'))
+    dump_insns(pass->name);
+
   /* IPA passes are executed on whole program, so cfun should be NULL.
      Other passes need function context set.  */
   if (pass->type == SIMPLE_IPA_PASS || pass->type == IPA_PASS)

--- gcc/passes.def	2017-03-22 14:31:55.000000000 +0000
+++ gcc/passes.def	2025-11-02 00:56:38.338284256 +0000
@@ -386,6 +386,7 @@ along with GCC; see the file COPYING3.
   NEXT_PASS (pass_gen_hsail);
 
   NEXT_PASS (pass_expand);
+  NEXT_PASS (pass_bbb_baserel);
 
   NEXT_PASS (pass_rest_of_compilation);
   PUSH_INSERT_PASSES_WITHIN (pass_rest_of_compilation)
@@ -424,6 +425,7 @@ along with GCC; see the file COPYING3.
       NEXT_PASS (pass_initialize_regs);
       NEXT_PASS (pass_ud_rtl_dce);
       NEXT_PASS (pass_combine);
+      NEXT_PASS (pass_peephole2);
       NEXT_PASS (pass_if_after_combine);
       NEXT_PASS (pass_partition_blocks);
       NEXT_PASS (pass_outof_cfg_layout_mode);
@@ -458,10 +460,12 @@ along with GCC; see the file COPYING3.
 	  NEXT_PASS (pass_cprop_hardreg);
 	  NEXT_PASS (pass_fast_rtl_dce);
 	  NEXT_PASS (pass_reorder_blocks);
+	  NEXT_PASS (pass_bbb_optimizations);
 	  NEXT_PASS (pass_branch_target_load_optimize2);
 	  NEXT_PASS (pass_leaf_regs);
 	  NEXT_PASS (pass_split_before_sched2);
 	  NEXT_PASS (pass_sched2);
+	  NEXT_PASS (pass_peephole2);
 	  NEXT_PASS (pass_stack_regs);
 	  PUSH_INSERT_PASSES_WITHIN (pass_stack_regs)
 	      NEXT_PASS (pass_split_before_regstack);
Only in ./gcc-6.5.0/gcc/po: be.gmo
Only in ./gcc-6.5.0/gcc/po: da.gmo
Only in ./gcc-6.5.0/gcc/po: de.gmo
Only in ./gcc-6.5.0/gcc/po: el.gmo
Only in ./gcc-6.5.0/gcc/po: es.gmo
Only in ./gcc-6.5.0/gcc/po: fi.gmo
Only in ./gcc-6.5.0/gcc/po: fr.gmo
Only in ./gcc-6.5.0/gcc/po: hr.gmo
Only in ./gcc-6.5.0/gcc/po: id.gmo
Only in ./gcc-6.5.0/gcc/po: ja.gmo
Only in ./gcc-6.5.0/gcc/po: nl.gmo
Only in ./gcc-6.5.0/gcc/po: ru.gmo
Only in ./gcc-6.5.0/gcc/po: sr.gmo
Only in ./gcc-6.5.0/gcc/po: sv.gmo
Only in ./gcc-6.5.0/gcc/po: tr.gmo
Only in ./gcc-6.5.0/gcc/po: uk.gmo
Only in ./gcc-6.5.0/gcc/po: vi.gmo
Only in ./gcc-6.5.0/gcc/po: zh_CN.gmo
Only in ./gcc-6.5.0/gcc/po: zh_TW.gmo

--- gcc/read-md.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/read-md.c	2025-11-02 00:56:38.654289040 +0000
@@ -171,7 +171,7 @@ fprint_md_ptr_loc (FILE *outf, const voi
 {
   const struct ptr_loc *loc = get_md_ptr_loc (ptr);
   if (loc != 0)
-    fprintf (outf, "#line %d \"%s\"\n", loc->lineno, loc->filename);
+    fprintf (outf, "//line %d \"%s\"\n", loc->lineno, loc->filename);
 }
 
 /* Special fprint_md_ptr_loc for writing to STDOUT.  */

--- gcc/recog.c	2016-01-29 18:47:17.000000000 +0000
+++ gcc/recog.c	2025-11-02 00:56:38.658289100 +0000
@@ -1356,6 +1356,12 @@ memory_operand (rtx op, machine_mode mod
   if (mode != VOIDmode && GET_MODE (op) != mode)
     return 0;
 
+#if defined(TARGET_AMIGAOS)
+  /* SBF: allow direct mem ref to a4. */
+  if (MEM_P(op) && amiga_is_const_pic_ref(XEXP(op, 0)))
+    return true;
+#endif
+
   inner = op;
   if (GET_CODE (inner) == SUBREG)
     inner = SUBREG_REG (inner);
@@ -2187,7 +2193,11 @@ extract_constrain_insn (rtx_insn *insn)
 {
   extract_insn (insn);
   if (!constrain_operands (reload_completed, get_enabled_alternatives (insn)))
-    fatal_insn_not_found (insn);
+    {
+      debug_rtx(insn);
+      constrain_operands (reload_completed, get_enabled_alternatives (insn));
+      fatal_insn_not_found (insn);
+    }
 }
 
 /* Do cached extract_insn, constrain_operands and complain about failures.
@@ -3252,6 +3262,7 @@ peep2_attempt (basic_block bb, rtx_insn
   /* If we are splitting an RTX_FRAME_RELATED_P insn, do not allow it to
      match more than one insn, or to be split into more than one insn.  */
   old_insn = peep2_insn_data[peep2_current].insn;
+
   if (RTX_FRAME_RELATED_P (old_insn))
     {
       bool any_note = false;
@@ -3384,6 +3395,33 @@ peep2_attempt (basic_block bb, rtx_insn
       break;
     }
 
+  /* SBF: keep REG_INC notes. */
+  while ((as_note = find_reg_note (old_insn, REG_INC, NULL)))
+    {
+      add_reg_note(attempt, REG_INC, XEXP (as_note, 0));
+      remove_note(old_insn, as_note);
+    }
+
+  /* SBF: move REG_DEAD notes. */
+  for (i = match_len; i >= 0; --i)
+    {
+      int j = peep2_buf_position (peep2_current + i);
+      old_insn = peep2_insn_data[j].insn;
+      while ((as_note = find_reg_note (old_insn, REG_DEAD, NULL)))
+	{
+	  rtx reg = XEXP (as_note, 0);
+	  rtx_insn * p, * last = 0;
+	  for (p = attempt; p; p = NEXT_INSN (p))
+	      if (reg_mentioned_p(reg, p))
+		last = p;
+
+	  if (last)
+	    add_reg_note(last, REG_DEAD, reg);
+
+	  remove_note(old_insn, as_note);
+	}
+    }
+
   /* If we matched any instruction that had a REG_ARGS_SIZE, then
      move those notes over to the new sequence.  */
   as_note = NULL;
@@ -3613,7 +3651,7 @@ peephole2_optimize (void)
 
 	  /* Match the peephole.  */
 	  head = peep2_insn_data[peep2_current].insn;
-	  attempt = peephole2_insns (PATTERN (head), head, &match_len);
+	  attempt = PATTERN (head) ? peephole2_insns (PATTERN (head), head, &match_len) : NULL;
 	  if (attempt != NULL)
 	    {
 	      rtx_insn *last = peep2_attempt (bb, head, match_len, attempt);
@@ -3788,6 +3826,7 @@ if_test_bypass_p (rtx_insn *out_insn, rt
   return true;
 }
 
+
 static unsigned int
 rest_of_handle_peephole2 (void)
 {
@@ -3826,7 +3865,12 @@ public:
   virtual bool gate (function *) { return (optimize > 0 && flag_peephole2); }
   virtual unsigned int execute (function *)
     {
-      return rest_of_handle_peephole2 ();
+      static unsigned peephole2_pass;
+      unsigned pn = static_pass_number;
+      static_pass_number = peephole2_pass++;
+      unsigned r = rest_of_handle_peephole2 ();
+      static_pass_number = pn;
+      return r;
     }
 
 }; // class pass_peephole2

--- gcc/recog.h	2016-01-04 14:30:50.000000000 +0000
+++ gcc/recog.h	2025-11-02 00:56:38.658289100 +0000
@@ -296,23 +296,23 @@ struct insn_gen_fn
 
   typedef f0 stored_funcptr;
 
-  rtx_insn * operator () (void) const { return ((f0)func) (); }
-  rtx_insn * operator () (rtx a0) const { return ((f1)func) (a0); }
-  rtx_insn * operator () (rtx a0, rtx a1) const { return ((f2)func) (a0, a1); }
-  rtx_insn * operator () (rtx a0, rtx a1, rtx a2) const { return ((f3)func) (a0, a1, a2); }
-  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3) const { return ((f4)func) (a0, a1, a2, a3); }
-  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4) const { return ((f5)func) (a0, a1, a2, a3, a4); }
-  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5) const { return ((f6)func) (a0, a1, a2, a3, a4, a5); }
-  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6) const { return ((f7)func) (a0, a1, a2, a3, a4, a5, a6); }
-  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6, rtx a7) const { return ((f8)func) (a0, a1, a2, a3, a4, a5, a6, a7); }
-  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6, rtx a7, rtx a8) const { return ((f9)func) (a0, a1, a2, a3, a4, a5, a6, a7, a8); }
-  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6, rtx a7, rtx a8, rtx a9) const { return ((f10)func) (a0, a1, a2, a3, a4, a5, a6, a7, a8, a9); }
-  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6, rtx a7, rtx a8, rtx a9, rtx a10) const { return ((f11)func) (a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10); }
-  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6, rtx a7, rtx a8, rtx a9, rtx a10, rtx a11) const { return ((f12)func) (a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11); }
-  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6, rtx a7, rtx a8, rtx a9, rtx a10, rtx a11, rtx a12) const { return ((f13)func) (a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12); }
-  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6, rtx a7, rtx a8, rtx a9, rtx a10, rtx a11, rtx a12, rtx a13) const { return ((f14)func) (a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13); }
-  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6, rtx a7, rtx a8, rtx a9, rtx a10, rtx a11, rtx a12, rtx a13, rtx a14) const { return ((f15)func) (a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14); }
-  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6, rtx a7, rtx a8, rtx a9, rtx a10, rtx a11, rtx a12, rtx a13, rtx a14, rtx a15) const { return ((f16)func) (a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15); }
+  rtx_insn * operator () (void) const { return ((f0)(void *)func) (); }
+  rtx_insn * operator () (rtx a0) const { return ((f1)(void *)func) (a0); }
+  rtx_insn * operator () (rtx a0, rtx a1) const { return ((f2)(void *)func) (a0, a1); }
+  rtx_insn * operator () (rtx a0, rtx a1, rtx a2) const { return ((f3)(void *)func) (a0, a1, a2); }
+  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3) const { return ((f4)(void *)func) (a0, a1, a2, a3); }
+  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4) const { return ((f5)(void *)func) (a0, a1, a2, a3, a4); }
+  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5) const { return ((f6)(void *)func) (a0, a1, a2, a3, a4, a5); }
+  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6) const { return ((f7)(void *)func) (a0, a1, a2, a3, a4, a5, a6); }
+  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6, rtx a7) const { return ((f8)(void *)func) (a0, a1, a2, a3, a4, a5, a6, a7); }
+  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6, rtx a7, rtx a8) const { return ((f9)(void *)func) (a0, a1, a2, a3, a4, a5, a6, a7, a8); }
+  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6, rtx a7, rtx a8, rtx a9) const { return ((f10)(void *)func) (a0, a1, a2, a3, a4, a5, a6, a7, a8, a9); }
+  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6, rtx a7, rtx a8, rtx a9, rtx a10) const { return ((f11)(void *)func) (a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10); }
+  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6, rtx a7, rtx a8, rtx a9, rtx a10, rtx a11) const { return ((f12)(void *)func) (a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11); }
+  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6, rtx a7, rtx a8, rtx a9, rtx a10, rtx a11, rtx a12) const { return ((f13)(void *)func) (a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12); }
+  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6, rtx a7, rtx a8, rtx a9, rtx a10, rtx a11, rtx a12, rtx a13) const { return ((f14)(void *)func) (a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13); }
+  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6, rtx a7, rtx a8, rtx a9, rtx a10, rtx a11, rtx a12, rtx a13, rtx a14) const { return ((f15)(void *)func) (a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14); }
+  rtx_insn * operator () (rtx a0, rtx a1, rtx a2, rtx a3, rtx a4, rtx a5, rtx a6, rtx a7, rtx a8, rtx a9, rtx a10, rtx a11, rtx a12, rtx a13, rtx a14, rtx a15) const { return ((f16)(void *)func) (a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15); }
 
   // This is for compatibility of code that invokes functions like
   //   (*funcptr) (arg)

--- gcc/reg-notes.def	2016-01-04 14:30:50.000000000 +0000
+++ gcc/reg-notes.def	2025-11-02 00:56:38.658289100 +0000
@@ -211,3 +211,6 @@ REG_NOTE (RETURNED)
    The decl might not be available in the call due to splitting of the call
    insn.  This note is a SYMBOL_REF.  */
 REG_NOTE (CALL_DECL)
+
+/* Used to track mask of the used bits.  This note is a CONST_INT.  */
+REG_NOTE (BIT_MASK)

--- gcc/regrename.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/regrename.c	2025-11-02 00:56:38.658289100 +0000
@@ -374,44 +374,45 @@ find_rename_reg (du_head_p this_head, en
     = (enum reg_class) targetm.preferred_rename_class (super_class);
 
   /* Pick and check the register from the tied chain iff the tied chain
-     is not renamed.  */
+   is not renamed.  */
   if (this_head->tied_chain && !this_head->tied_chain->renamed
       && check_new_reg_p (old_reg, this_head->tied_chain->regno,
 			  this_head, *unavailable))
     return this_head->tied_chain->regno;
 
   /* If PREFERRED_CLASS is not NO_REGS, we iterate in the first pass
-     over registers that belong to PREFERRED_CLASS and try to find the
-     best register within the class.  If that failed, we iterate in
-     the second pass over registers that don't belong to the class.
-     If PREFERRED_CLASS is NO_REGS, we iterate over all registers in
-     ascending order without any preference.  */
+   over registers that belong to PREFERRED_CLASS and try to find the
+   best register within the class.  If that failed, we iterate in
+   the second pass over registers that don't belong to the class.
+   If PREFERRED_CLASS is NO_REGS, we iterate over all registers in
+   ascending order without any preference.  */
   has_preferred_class = (preferred_class != NO_REGS);
   for (pass = (has_preferred_class ? 0 : 1); pass < 2; pass++)
     {
       int new_reg;
-      for (new_reg = 0; new_reg < FIRST_PSEUDO_REGISTER; new_reg++)
-	{
-	  if (has_preferred_class
+  for (new_reg = 0; new_reg < FIRST_PSEUDO_REGISTER; new_reg++)
+    {
+      if (has_preferred_class
 	      && (pass == 0)
 	      != TEST_HARD_REG_BIT (reg_class_contents[preferred_class],
-				    new_reg))
-	    continue;
+				   new_reg))
+	continue;
 
-	  if (!check_new_reg_p (old_reg, new_reg, this_head, *unavailable))
-	    continue;
+      if (!check_new_reg_p (old_reg, new_reg, this_head, *unavailable))
+	continue;
 
-	  if (!best_rename)
-	    return new_reg;
+      if (!best_rename)
+	return new_reg;
 
-	  /* In the first pass, we force the renaming of registers that
-	     don't belong to PREFERRED_CLASS to registers that do, even
-	     though the latters were used not very long ago.  */
-	  if ((pass == 0
+      /* In the first pass, we force the renaming of registers that
+       don't belong to PREFERRED_CLASS to registers that do, even
+       though the latters were used not very long ago.
+       Also use a register if no best_new_reg was found till now  */
+	  if (((pass == 0 || !has_preferred_class)
 	      && !TEST_HARD_REG_BIT (reg_class_contents[preferred_class],
 				     best_new_reg))
 	      || tick[best_new_reg] > tick[new_reg])
-	    best_new_reg = new_reg;
+	  best_new_reg = new_reg;
 	}
       if (pass == 0 && best_new_reg != old_reg)
 	break;
@@ -897,7 +898,7 @@ regrename_analyze (bitmap bb_mask)
 	      if (!range_overlaps_hard_reg_set_p (live, chain->regno,
 						  chain->nregs))
 		continue;
-	      
+
 	      n_succs_used++;
 
 	      dest_ri = (struct bb_rename_info *)e->dest->aux;
@@ -921,7 +922,7 @@ regrename_analyze (bitmap bb_mask)
 			  printed = true;
 			  fprintf (dump_file,
 				   "  merging chains %d (->%d) and %d (->%d) [%s]\n",
-				   k, incoming_chain->id, j, chain->id, 
+				   k, incoming_chain->id, j, chain->id,
 				   reg_names[incoming_chain->regno]);
 			}
 
@@ -954,7 +955,7 @@ regrename_analyze (bitmap bb_mask)
    numbering in its subpatterns.  */
 
 bool
-regrename_do_replace (struct du_head *head, int reg)
+regrename_do_replace (struct du_head *head, int regno)
 {
   struct du_chain *chain;
   unsigned int base_regno = head->regno;
@@ -962,19 +963,20 @@ regrename_do_replace (struct du_head *he
 
   for (chain = head->first; chain; chain = chain->next_use)
     {
-      unsigned int regno = ORIGINAL_REGNO (*chain->loc);
-      struct reg_attrs *attr = REG_ATTRS (*chain->loc);
-      int reg_ptr = REG_POINTER (*chain->loc);
+      unsigned int orig_regno = ORIGINAL_REGNO(*chain->loc);
+      struct reg_attrs *attr = REG_ATTRS(*chain->loc);
+      int reg_ptr = REG_POINTER(*chain->loc);
 
       if (DEBUG_INSN_P (chain->insn) && REGNO (*chain->loc) != base_regno)
-	validate_change (chain->insn, &(INSN_VAR_LOCATION_LOC (chain->insn)),
-			 gen_rtx_UNKNOWN_VAR_LOC (), true);
+	validate_change (chain->insn, &(INSN_VAR_LOCATION_LOC(chain->insn)),
+	gen_rtx_UNKNOWN_VAR_LOC (),
+			 true);
       else
 	{
-	  validate_change (chain->insn, chain->loc, 
-			   gen_raw_REG (GET_MODE (*chain->loc), reg), true);
-	  if (regno >= FIRST_PSEUDO_REGISTER)
-	    ORIGINAL_REGNO (*chain->loc) = regno;
+	  validate_change (chain->insn, chain->loc,
+			   gen_raw_REG (GET_MODE(*chain->loc), regno), true);
+	  if (orig_regno >= FIRST_PSEUDO_REGISTER)
+	    ORIGINAL_REGNO (*chain->loc) = orig_regno;
 	  REG_ATTRS (*chain->loc) = attr;
 	  REG_POINTER (*chain->loc) = reg_ptr;
 	}
@@ -983,10 +985,29 @@ regrename_do_replace (struct du_head *he
   if (!apply_change_group ())
     return false;
 
-  mode = GET_MODE (*head->first->loc);
+  mode = GET_MODE(*head->first->loc);
   head->renamed = 1;
-  head->regno = reg;
-  head->nregs = hard_regno_nregs[reg][mode];
+  head->regno = regno;
+  head->nregs = hard_regno_nregs[regno][mode];
+
+  /* SBF: also update the current df info, move from base_regno -> regno. */
+  if (base_regno < FIRST_PSEUDO_REGISTER && regno < FIRST_PSEUDO_REGISTER)
+    for (chain = head->first; chain; chain = chain->next_use)
+      {
+	if (DEBUG_INSN_P (chain->insn) && VAR_LOC_UNKNOWN_P(INSN_VAR_LOCATION_LOC(chain->insn)))
+	  continue;
+	/* undo regno patch - will be patched again */
+	if (REGNO (*chain->loc) == regno)
+	  SET_REGNO(*chain->loc, base_regno);
+	df_ref_change_reg_with_loc (*chain->loc, regno);
+
+	SET_REGNO(*chain->loc, regno);
+      }
+
+  /* Mark the old regno as no longer used. */
+  if (!df->hard_regs_live_count[base_regno])
+    df_set_regs_ever_live (base_regno, false);
+
   return true;
 }
 
@@ -1912,7 +1933,6 @@ const pass_data pass_data_regrename =
   0, /* todo_flags_start */
   TODO_df_finish, /* todo_flags_finish */
 };
-
 class pass_regrename : public rtl_opt_pass
 {
 public:
@@ -1923,7 +1943,7 @@ public:
   /* opt_pass methods: */
   virtual bool gate (function *)
     {
-      return (optimize > 0 && (flag_rename_registers));
+      return (optimize > 0 && (flag_rename_registers) && !TARGET_AMIGA);
     }
 
   virtual unsigned int execute (function *) { return regrename_optimize (); }

--- gcc/reload1.c	2016-03-18 08:25:57.000000000 +0000
+++ gcc/reload1.c	2025-11-02 00:56:38.662289161 +0000
@@ -739,6 +739,100 @@ will_delete_init_insn_p (rtx_insn *insn)
   return false;
 }
 
+#if defined(TARGET_AMIGAOS)
+
+extern rtx
+alter_subreg (rtx *xp, bool final_p);
+
+static bool
+darn_reload_did_not_catch_these(rtx *loc, rtx set, rtx_insn *insn)
+{
+  rtx x = *loc;
+  rtx ad = x;
+  if (GET_CODE(x) == PLUS && (REG_P(XEXP(x, 0)) || SUBREG_P(XEXP(x, 0))))
+    x = *(loc = &XEXP(x, 0));
+  rtx reg = x;
+  if (SUBREG_P(x))
+    reg = XEXP(x, 0), alter_subreg(&x, true);
+  // handle the case that a memory_loc was created with a data register.
+  if (REG_P(reg) && !ADDRESS_REGNO_P (REGNO (reg))
+      && (GET_MODE_SIZE(GET_MODE(reg)) > GET_MODE_SIZE(GET_MODE(x))
+       || !targetm.legitimate_address_p(GET_MODE(SET_DEST(set)), ad, true)))
+    {
+      // if there is a data register at dest - without overlap, use it
+      if (ADDRESS_REG_P(SET_DEST(set)) && !reg_overlap_mentioned_p(SET_DEST(set), SET_SRC(set)))
+	{
+	  int regno = REGNO(SET_DEST(set));
+	  rtx areg = gen_rtx_REG(SImode, regno);
+	  emit_insn_before(gen_rtx_SET(areg, x), insn);
+	  *loc = areg;
+//	  fprintf(stderr, "+");
+	  return true;
+	}
+
+      // try all address regs
+	int regno;
+	for (regno = 0; regno < FIRST_PSEUDO_REGISTER; ++regno)
+	  if (ADDRESS_REGNO_P(regno))
+	    {
+	      rtx areg = gen_rtx_REG(SImode, regno);
+
+	      if (!reg_overlap_mentioned_p(areg, set))
+		{
+		  rtx pat = gen_swapsi(areg, x);
+    		  emit_insn_before (pat, insn);
+    		  *loc = areg;
+    		  if (REG_P(SET_DEST(set)) && REGNO(SET_DEST(set)) == REGNO(reg))
+    		    SET_DEST(set) = gen_rtx_REG(GET_MODE(SET_DEST(set)), regno);
+    		  emit_insn_after (pat, insn);
+//    		  fprintf(stderr, ":");
+    		  return true;
+		}
+	    }
+    }
+  return false;
+}
+
+static void
+fix_invalid_addresses (rtx_insn * insn)
+{
+  rtx set = single_set(insn);
+  if (set)
+    {
+      rtx dst = SET_DEST(set);
+      rtx * src = &SET_SRC(set);
+
+      // handle invalid lea
+      if (ADDRESS_REG_P(dst) && GET_CODE(*src) == PLUS)
+	{
+	  rtx x = XEXP(*src, 0);
+	  if (REG_P(x) && !ADDRESS_REGNO_P (REGNO (x)))
+	    {
+	      emit_insn_before(gen_rtx_SET(dst, x), insn);
+	      *src = copy_rtx(*src);
+	      XEXP(*src, 0) = dst;
+	    }
+	}
+      else
+	{
+	  if (GET_CODE(*src) == COMPARE)
+	    src = &XEXP(*src, 0);
+	  if (MEM_P(*src))
+	    darn_reload_did_not_catch_these(&XEXP(*src, 0), set, insn);
+	  if (MEM_P(dst))
+	    {
+	      darn_reload_did_not_catch_these(&XEXP(SET_DEST(set), 0), set, insn);
+	      if (GET_CODE(XEXP(dst, 0)) == PRE_DEC
+		  && GET_CODE(*src) == PLUS
+		  && REG_P(XEXP(*src, 0))
+		  && !ADDRESS_REGNO_P (REGNO (XEXP(*src, 0)))) // pea
+		darn_reload_did_not_catch_these(src, set, insn);
+	    }
+	}
+    }
+}
+#endif
+
 /* Main entry point for the reload pass.
 
    FIRST is the first insn of the function being compiled.
@@ -1240,6 +1334,10 @@ reload (rtx_insn *first, int global)
 	if (AUTO_INC_DEC)
 	  add_auto_inc_notes (insn, PATTERN (insn));
 
+#if defined(TARGET_AMIGAOS)
+	fix_invalid_addresses (insn);
+#endif
+
 	/* Simplify (subreg (reg)) if it appears as an operand.  */
 	cleanup_subreg_operands (insn);
 
@@ -1846,6 +1944,20 @@ find_reg (struct insn_chain *chain, int
 	  SET_HARD_REG_BIT (used_by_other_reload, rld[other].regno + j);
     }
 
+  /* SBF: hack to prevent different registers in auto inc with operation insns:
+   * (set (mem:HI (post_inc:SI (reg:SI 6)))
+   *      (ior:HI (mem:HI (reg:SI 6))
+   *              (reg/v:HI 1)))
+   *
+   * needs a reload into a address register.
+   * => don't reload into two different registers
+   * => use the register of the previos reload.
+   */
+  if (order == 1 && rld->in == rld->out && rld->in == rl->in
+      && rld->rclass == rl->rclass
+      && rld->when_needed == RELOAD_OTHER && rl->when_needed == RELOAD_FOR_OPERAND_ADDRESS)
+      best_reg = rld->regno;
+  else
   for (i = 0; i < FIRST_PSEUDO_REGISTER; i++)
     {
 #ifdef REG_ALLOC_ORDER
@@ -2012,9 +2124,14 @@ find_reload_regs (struct insn_chain *cha
 	  {
 	    if (dump_file)
 	      fprintf (dump_file, "reload failure for reload %d\n", r);
+
+#if defined(TARGET_AMIGAOS)
+	    rld[r].in = 0; // mark as optional - bbb pass will fix this later
+#else
 	    spill_failure (chain->insn, rld[r].rclass);
 	    failure = 1;
 	    return;
+#endif
 	  }
     }
 
@@ -2191,6 +2308,8 @@ alter_reg (int i, int from_reg, bool don
 				  min_align > inherent_align
 				  || total_size > inherent_size ? -1 : 0);
 
+	  MEM_IN_STRUCT_P (x) = 1; // mark as stack slot == restrict
+
 	  stack_slot = x;
 
 	  /* Cancel the big-endian correction done in assign_stack_local.
@@ -3295,6 +3414,13 @@ eliminate_regs_in_insn (rtx_insn *insn,
 		  }
 	      }
 
+	    /* SBF: there can't be assignments to the FRAME_POINTER - keep it. */
+	    if (ep->from == FRAME_POINTER_REGNUM && !frame_pointer_needed)
+	      {
+		// warning_for_asm (insn, "keeping assignment to %s", reg_names[FRAME_POINTER_REGNUM]);
+		goto done;
+	      }
+
 	    /* In this case this insn isn't serving a useful purpose.  We
 	       will delete it in reload_as_needed once we know that this
 	       elimination is, in fact, being done.
@@ -3621,6 +3747,11 @@ elimination_costs_in_insn (rtx_insn *ins
 
   if (! insn_is_asm && icode < 0)
     {
+      if (!(DEBUG_INSN_P (insn)
+		  || GET_CODE (PATTERN (insn)) == USE
+		  || GET_CODE (PATTERN (insn)) == CLOBBER
+		  || GET_CODE (PATTERN (insn)) == ASM_INPUT))
+	debug(insn);
       gcc_assert (DEBUG_INSN_P (insn)
 		  || GET_CODE (PATTERN (insn)) == USE
 		  || GET_CODE (PATTERN (insn)) == CLOBBER
@@ -4546,6 +4677,10 @@ fixup_eh_region_note (rtx_insn *insn, rt
    We update these for the reloads that we perform,
    as the insns are scanned.  */
 
+#if AUTO_INC_DEC
+rtx_insn *old_prev;
+#endif
+
 static void
 reload_as_needed (int live_known)
 {
@@ -4574,7 +4709,7 @@ reload_as_needed (int live_known)
       rtx_insn *insn = chain->insn;
       rtx_insn *old_next = NEXT_INSN (insn);
 #if AUTO_INC_DEC
-      rtx_insn *old_prev = PREV_INSN (insn);
+      old_prev = PREV_INSN (insn);
 #endif
 
       if (will_delete_init_insn_p (insn))
@@ -6227,6 +6362,15 @@ allocate_reload_reg (struct insn_chain *
 
       i = last_spill_reg;
 
+      /**
+       * SBF: This is an REG_INC for the same reg in in/out.
+       * Use the last_spill_reg.
+       */
+      if (r == 1 && rld->in == rld->out && rld->in == rld[1].in
+          && rld->rclass == rld[1].rclass
+          && rld->when_needed == RELOAD_OTHER && rld[1].when_needed == RELOAD_FOR_OPERAND_ADDRESS)
+	break;
+
       for (count = 0; count < n_spills; count++)
 	{
 	  int rclass = (int) rld[r].rclass;
@@ -7974,9 +8118,19 @@ do_input_reload (struct insn_chain *chai
       /* The insn might have already some references to stackslots
 	 replaced by MEMs, while reload_out_reg still names the
 	 original pseudo.  */
-      && (dead_or_set_p (insn, spill_reg_stored_to[REGNO (reg_rtx)])
-	  || rtx_equal_p (spill_reg_stored_to[REGNO (reg_rtx)], rl->out_reg)))
-    delete_output_reload (insn, j, REGNO (reg_rtx), reg_rtx);
+      && (
+/**
+ * SBF: a later reload might rely on this reload which is marked dead for now...
+ * ... then this reload is missing.
+ */
+#if 0
+	  dead_or_set_p (insn, spill_reg_stored_to[REGNO (reg_rtx)])
+	  ||
+#endif	  
+	  rtx_equal_p (spill_reg_stored_to[REGNO (reg_rtx)], rl->out_reg)))
+    {
+      delete_output_reload (insn, j, REGNO (reg_rtx), reg_rtx);
+    }
 }
 
 /* Do output reloading for reload RL, which is for the insn described by

--- gcc/reload.c	2016-02-14 01:37:40.000000000 +0000
+++ gcc/reload.c	2025-11-02 00:56:38.662289161 +0000
@@ -5010,6 +5010,147 @@ find_reloads_address (machine_mode mode,
     }
   while (0);
 #endif
+#if defined(TARGET_AMIGAOS)
+  /**
+   * SBF: check the base register here,
+   * since later no information exists, which reg is the base reg
+   * and a data reg could end up in the base reg slot.
+   * => reload the data reg
+   */
+//#define DX (insn->u2.insn_uid == 722)
+#define DX 0
+  if (GET_CODE(ad) == PLUS || GET_CODE(ad) == MEM)
+    {
+      extern bool m68k_legitimate_index_reg_p (rtx x, bool strict_p);
+      extern bool m68k_legitimate_base_reg_p (rtx x, bool strict_p);
+
+      struct m68k_address address;
+      memset(&address, 0, sizeof(address));
+      bool r = decompose_mem(GET_MODE_SIZE(mode), loc, &address, true);
+      if (DX) fprintf(stderr, "insn %d %d %d\t", insn->u2.insn_uid, r, address.code);
+
+//      gcc_assert(addres s.code != POST_MODIFY);
+
+      if (DX)
+	debug(insn);
+
+      enum reload_type utype = type; // RELOAD_FOR_OTHER_ADDRESS;
+//      enum reload_type utype = address.code == MEM ? (opnum ? RELOAD_FOR_INPUT : RELOAD_FOR_OPERAND_ADDRESS) : type;
+      int fixed = 0;
+
+      // check the insn asm if double indirect is possible
+      if (address.mem_loc != 0 && address.code != POST_MODIFY)
+	{
+	  char const * p = insn_data[INSN_CODE (insn)].operand[opnum].constraint;
+	  if (*p == '%' && p[1] == '0')
+	    p = insn_data[INSN_CODE (insn)].operand[0].constraint;
+	  while (*p && *p != 'm' && *p != 'g' && *p != 'f')
+	    ++p;
+	  if (!*p)
+	    address.code = POST_MODIFY;
+        }
+
+      if (address.code != POST_MODIFY && address.base && !m68k_legitimate_base_reg_p(address.base, true))
+	{
+	  rtx * base_loc = address.base_loc;
+	  if (DX) fprintf(stderr, "insn %d: reload base (%d)", insn->u2.insn_uid, address.code), debug_rtx(*base_loc);
+	  find_reloads_address_1 (mode, as,
+				  *base_loc, 0, PLUS,
+				  GET_CODE (ad),
+				  base_loc, opnum,
+				  utype, 0, insn);
+	  fixed = 1;
+	}
+      if (address.code != POST_MODIFY && address.index_loc && !m68k_legitimate_index_reg_p(*address.index_loc, true))
+	{
+	  rtx * index_loc = address.index_loc;
+	  if (GET_CODE(*index_loc) == SIGN_EXTEND)
+	    index_loc = &XEXP(*index_loc, 0);
+	  if (GET_CODE(*index_loc) == SUBREG)
+	    index_loc = &XEXP(*index_loc, 0);
+	  if (DX) fprintf(stderr, "insn %d: reload index ", insn->u2.insn_uid), debug_rtx(*index_loc);
+	  find_reloads_address_1 (mode, as,
+				  *index_loc, 1, PLUS,
+				  GET_CODE (ad),
+				  index_loc, opnum,
+				  utype, 0, insn);
+	  fixed = 1;
+	}
+
+      if (address.outer_index_loc && !m68k_legitimate_index_reg_p(*address.outer_index_loc, true))
+	{
+	  rtx * index_loc = address.outer_index_loc;
+	  if (GET_CODE(*index_loc) == SIGN_EXTEND)
+	    index_loc = &XEXP(*index_loc, 0);
+	  if (GET_CODE(*index_loc) == SUBREG)
+	    index_loc = &XEXP(*index_loc, 0);
+	  if (DX) fprintf(stderr, "insn %d: reload outer index ", insn->u2.insn_uid), debug_rtx(*index_loc);
+	  find_reloads_address_1 (mode, as,
+				  *index_loc, 1, PLUS,
+				  GET_CODE (ad),
+				  index_loc, opnum,
+				  utype, 0, insn);
+	  fixed = 1;
+	}
+
+      // 68000 has only support for small offsets if base and index are used.
+      if (!TARGET_68020 && address.offset && address.base &&
+	  ( (address.index && (GET_CODE(address.offset) != CONST_INT || !IN_RANGE (INTVAL (address.offset), -0x80, 0x80 - GET_MODE_SIZE(GET_MODE(ad)))))
+	  ||(!address.index && (GET_CODE(address.offset) != CONST_INT || !IN_RANGE (INTVAL (address.offset), -0x8000, 0x8000 - GET_MODE_SIZE(GET_MODE(ad)))))
+	      ))
+	{
+	  if (address.index)
+	    push_reload (XEXP(ad, 0), NULL_RTX, &XEXP(ad, 0), (rtx*) 0,
+			   ADDR_REGS,
+			   GET_MODE (ad), VOIDmode, 0, 0, opnum, utype);
+	  if (!address.index || GET_CODE(address.offset) != CONST_INT || !IN_RANGE (INTVAL (address.offset), -0x8000, 0x8000 - GET_MODE_SIZE(GET_MODE(ad))))
+	    push_reload (ad, NULL_RTX, loc, (rtx*) 0,
+			   ADDR_REGS,
+			   GET_MODE (ad), VOIDmode, 0, 0, opnum, utype);
+	  return -1; // reloaded
+	}
+
+      /* SBF: if both indexes are in use we reload the inner mem into an address reg.
+       * This yields a valid address for the outer part since a outer_index/outer_offset
+       * is combinable with an address register.
+       */
+      if (address.code == POST_MODIFY || (address.index && address.outer_index)
+	  || (!TARGET_68020 && address.code == MEM)
+	  )
+	{
+	  // last case result into a base_reg replaced with a mem -> use base_loc
+	  rtx x = *address.mem_loc;
+	  if (DX) fprintf(stderr, "insn %d: reload mem ", insn->u2.insn_uid), debug_rtx(x);
+
+	  // too many mem inside - recurse
+	  if (address.code == POST_MODIFY)
+	    {
+	      rtx tem = x;
+	      find_reloads_address (GET_MODE (x), &tem, XEXP (x, 0), &XEXP (x, 0),
+				    opnum, ADDR_TYPE (type),
+				    ind_levels == 0 ? 0 : ind_levels - 1, insn);
+
+	      /* If tem was changed, then we must create a new memory reference to
+		 hold it and store it back into memrefloc.  */
+	      if (tem != x )
+		{
+		  *address.mem_loc = copy_rtx (*address.mem_loc);
+		  copy_replacements (tem, XEXP (*address.mem_loc, 0));
+		  x = *address.mem_loc;
+		}
+	    }
+
+	  push_reload (x, NULL_RTX, address.mem_loc, (rtx*) 0,
+		       ADDR_REGS,
+		       GET_MODE (x), VOIDmode, 0, 0, opnum, utype);
+
+	  fixed = 1;
+	}
+
+      if (fixed)
+	return -1;
+    }
+#endif
 
   /* The address is not valid.  We have to figure out why.  First see if
      we have an outer AND and remove it if so.  Then analyze what's inside.  */
@@ -5841,9 +5982,11 @@ find_reloads_address_1 (machine_mode mod
 		  && (regno < FIRST_PSEUDO_REGISTER
 		      || (equiv
 			  && memory_operand (equiv, GET_MODE (equiv))
-			  && ! (icode != CODE_FOR_nothing
-				&& insn_operand_matches (icode, 0, equiv)
-				&& insn_operand_matches (icode, 1, equiv))))
+// SBF: WTF this results in replacing POST_INC with a plain reg...
+//			  && ! (icode != CODE_FOR_nothing
+//				&& insn_operand_matches (icode, 0, equiv)
+//				&& insn_operand_matches (icode, 1, equiv))
+				))
 		  /* Using RELOAD_OTHER means we emit this and the reload we
 		     made earlier in the wrong order.  */
 		  && !reloaded_inner_of_autoinc)
@@ -6248,6 +6391,39 @@ subst_reloads (rtx_insn *insn)
     {
       struct replacement *r = &replacements[i];
       rtx reloadreg = rld[r->what].reg_rtx;
+
+#ifdef TARGET_AMIGAOS
+      if (!reloadreg && !rld[r->what].optional && rld[r->what].rclass == ADDR_REGS)
+      {
+    	  rtx a = *r->where;
+    	  const char *fmt = GET_RTX_FORMAT(GET_CODE(a));
+
+    	  while (!REG_P(a) && *fmt == 'e') {
+    		  a = XEXP(a, 0);
+    		  fmt = GET_RTX_FORMAT(GET_CODE(a));
+    	  }
+
+    	  if (REG_P(a) && REGNO(a) < FIRST_PSEUDO_REGISTER) {
+    		  extern rtx_insn *old_prev;
+    		  unsigned regno = REGNO(a);
+    		  unsigned swapregno = CALL_P(insn) ? 13 : 15;
+    		  machine_mode m = GET_MODE(a);
+    		  rtx from = gen_rtx_REG(m, regno);
+    		  rtx to   = gen_rtx_REG(m, swapregno);
+
+    		  rld[r->what].optional = 1;
+
+    		  emit_insn_after (gen_swapsi(from, to), old_prev);
+
+    		  debug_rtx(insn);
+		      validate_replace_rtx_group (from, to, insn);
+    		  debug_rtx(insn);
+
+    		  emit_insn_after  (gen_swapsi(from, to), insn);
+    	  }
+      }
+#endif
+
       if (reloadreg)
 	{
 #ifdef DEBUG_RELOAD
@@ -6300,7 +6476,13 @@ subst_reloads (rtx_insn *insn)
 	}
       /* If reload got no reg and isn't optional, something's wrong.  */
       else
-	gcc_assert (rld[r->what].optional);
+      {
+    	  if (!rld[r->what].optional) {
+    		  debug_rtx(insn);
+    		  fprintf(stderr, "no free registers left\n");
+		  gcc_assert (rld[r->what].optional);
+    	  }
+      }
     }
 }
 

--- gcc/rtlanal.c	2018-01-16 16:30:43.000000000 +0000
+++ gcc/rtlanal.c	2025-11-02 00:56:38.666289221 +0000
@@ -1747,6 +1747,8 @@ reg_overlap_mentioned_p (const_rtx x, co
  recurse:
   switch (GET_CODE (x))
     {
+    case COMPARE:
+    case SET:
     case STRICT_LOW_PART:
     case ZERO_EXTRACT:
     case SIGN_EXTRACT:

--- gcc/rtl.h	2016-12-07 23:00:40.000000000 +0000
+++ gcc/rtl.h	2025-11-02 00:56:38.666289221 +0000
@@ -348,7 +348,8 @@ struct GTY((desc("0"), tag("0"),
      barrier.
      1 in a CONCAT is VAL_NEEDS_RESOLUTION in var-tracking.c.  */
   unsigned int volatil : 1;
-  /* 1 in a REG if the register is used only in exit code a loop.
+  /* 1 in a MEM if the access is restricted, also for stack spills.
+     1 in a REG if the register is used only in exit code a loop.
      1 in a SUBREG expression if was generated from a variable with a
      promoted mode.
      1 in a CODE_LABEL if the label is used for nonlocal gotos
@@ -2294,6 +2295,10 @@ do {								        \
 #define ASM_OPERANDS_SOURCE_LOCATION(RTX) XCUINT (RTX, 6, ASM_OPERANDS)
 #define ASM_INPUT_SOURCE_LOCATION(RTX) XCUINT (RTX, 1, ASM_INPUT)
 
+/* 1 if RTX is a mem that is marked as restrict or a stack spill.  */
+#define MEM_IN_STRUCT_P(RTX) \
+  (RTL_FLAG_CHECK1 ("MEM_IN_STRUCT_P", (RTX), MEM)->in_struct)
+
 /* 1 if RTX is a mem that is statically allocated in read-only memory.  */
 #define MEM_READONLY_P(RTX) \
   (RTL_FLAG_CHECK1 ("MEM_READONLY_P", (RTX), MEM)->unchanging)
@@ -2367,6 +2372,7 @@ do {								        \
 /* Copy the attributes that apply to memory locations from RHS to LHS.  */
 #define MEM_COPY_ATTRIBUTES(LHS, RHS)				\
   (MEM_VOLATILE_P (LHS) = MEM_VOLATILE_P (RHS),			\
+   MEM_IN_STRUCT_P (LHS) = MEM_IN_STRUCT_P (RHS),               \
    MEM_NOTRAP_P (LHS) = MEM_NOTRAP_P (RHS),			\
    MEM_READONLY_P (LHS) = MEM_READONLY_P (RHS),			\
    MEM_KEEP_ALIAS_SET_P (LHS) = MEM_KEEP_ALIAS_SET_P (RHS),	\

--- gcc/sel-sched-ir.c	2016-03-15 15:42:07.000000000 +0000
+++ gcc/sel-sched-ir.c	2025-11-02 00:56:38.674289342 +0000
@@ -3086,6 +3086,17 @@ finish_global_and_expr_insn (insn_t insn
     }
 }
 
+#ifdef __amiga__
+__near
+#endif
+const static struct sched_scan_info_def ssi0 =
+	{
+	  NULL, /* extend_bb */
+	  finish_global_and_expr_for_bb, /* init_bb */
+	  NULL, /* extend_insn */
+	  finish_global_and_expr_insn /* init_insn */
+	};
+
 /* Finalize per instruction data for the whole region.  */
 void
 sel_finish_global_and_expr (void)
@@ -3101,13 +3112,7 @@ sel_finish_global_and_expr (void)
 
     /* Clear AV_SETs and INSN_EXPRs.  */
     {
-      const struct sched_scan_info_def ssi =
-	{
-	  NULL, /* extend_bb */
-	  finish_global_and_expr_for_bb, /* init_bb */
-	  NULL, /* extend_insn */
-	  finish_global_and_expr_insn /* init_insn */
-	};
+      struct sched_scan_info_def ssi = ssi0;
 
       sched_scan (&ssi, bbs);
     }

--- gcc/simplify-rtx.c	2018-06-25 18:13:04.000000000 +0100
+++ gcc/simplify-rtx.c	2025-11-02 00:56:38.678289403 +0000
@@ -4156,6 +4156,58 @@ simplify_const_binary_operation (enum rt
       return immed_wide_int_const (result, mode);
     }
 
+#if defined(TARGET_AMIGAOS)
+  /**
+   * Subtraction of two pic refs is possible
+   * if those refer to the same symbol.
+   */
+  if (mode == SImode && code == MINUS
+      && amiga_is_const_pic_ref(op0)
+      && amiga_is_const_pic_ref(op1))
+    {
+      rtx a = op0;
+      rtx b = op1;
+      while (GET_CODE(a) == CONST) a = XEXP(a,0);
+      while (GET_CODE(b) == CONST) b = XEXP(b,0);
+      if (GET_CODE(a) == PLUS && GET_CODE(b) == PLUS
+	  && REG_P(XEXP(a,0)) && REG_P(XEXP(b,0))
+	  && REGNO(XEXP(a,0)) == REGNO(XEXP(b,0)))
+	{
+	  a = XEXP(a,1);
+	  b = XEXP(b,1);
+	  while (GET_CODE(a) == CONST) a = XEXP(a,0);
+	  while (GET_CODE(b) == CONST) b = XEXP(b,0);
+	  rtx ua = 0;
+	  rtx ub = 0;
+	  if (GET_CODE(a) == UNSPEC)
+	    {
+	      ua = a;
+	      a = GEN_INT(0);
+	    }
+	  else
+	  if (GET_CODE(a) == PLUS && GET_CODE(XEXP(a,0)) == UNSPEC)
+	    {
+	      ua = XEXP(a,0);
+	      a = XEXP(a,1);
+	    }
+	  if (GET_CODE(b) == UNSPEC)
+	    {
+	      ub = b;
+	      b = GEN_INT(0);
+	    }
+	  else
+	  if (GET_CODE(b) == PLUS && GET_CODE(XEXP(b,0)) == UNSPEC)
+	    {
+	      ub = XEXP(b,0);
+	      b = XEXP(b,1);
+	    }
+	  if (ua && ub && XVECEXP(ua,0,0) == XVECEXP(ub,0,0) &&
+	      GET_CODE(a) == CONST_INT && GET_CODE(b) == CONST_INT)
+	    return GEN_INT(INTVAL(a) - INTVAL(b));
+	}
+    }
+#endif
+
   return NULL_RTX;
 }
 

--- gcc/system.h	2016-04-22 12:09:22.000000000 +0100
+++ gcc/system.h	2025-11-02 00:56:38.682289463 +0000
@@ -57,6 +57,7 @@ along with GCC; see the file COPYING3.
    redefs are pointless here so we can override them.  */
 
 #undef fopen 
+#undef fdopen
 #undef freopen 
 
 #define fopen(PATH, MODE) fopen_unlocked (PATH, MODE)

--- gcc/target-def.h	2016-01-04 14:30:50.000000000 +0000
+++ gcc/target-def.h	2025-11-02 00:56:38.682289463 +0000
@@ -47,7 +47,7 @@
 #define TARGET_ASM_UNALIGNED_TI_OP NULL
 #endif /* OBJECT_FORMAT_ELF */
 
-#if !defined(TARGET_ASM_CONSTRUCTOR) && !defined(USE_COLLECT2)
+#if !defined(TARGET_ASM_CONSTRUCTOR) && (!defined(USE_COLLECT2) || defined(TARGET_AMIGAOS))
 # ifdef CTORS_SECTION_ASM_OP
 #  define TARGET_ASM_CONSTRUCTOR default_ctor_section_asm_out_constructor
 # else
@@ -59,7 +59,7 @@
 # endif
 #endif
 
-#if !defined(TARGET_ASM_DESTRUCTOR) && !defined(USE_COLLECT2)
+#if !defined(TARGET_ASM_DESTRUCTOR) && (!defined(USE_COLLECT2) || defined(TARGET_AMIGAOS))
 # ifdef DTORS_SECTION_ASM_OP
 #  define TARGET_ASM_DESTRUCTOR default_dtor_section_asm_out_destructor
 # else
@@ -108,3 +108,11 @@
 #include "hooks.h"
 #include "targhooks.h"
 #include "insn-target-def.h"
+
+#ifndef TARGET_GCC_EXCEPT_TABLE
+#define TARGET_GCC_EXCEPT_TABLE ".gcc_except_table"
+#endif
+
+#ifndef TARGET_GCC_EXCEPT_TABLE_S
+#define TARGET_GCC_EXCEPT_TABLE_S ".gcc_except_table.%s"
+#endif

--- gcc/testsuite/gcc.c-torture/execute/20030125-1.c	2015-10-23 11:01:47.000000000 +0100
+++ gcc/testsuite/gcc.c-torture/execute/20030125-1.c	2025-11-02 00:56:39.098295761 +0000
@@ -1,6 +1,8 @@
 /* Verify whether math functions are simplified.  */
 /* { dg-require-effective-target c99_runtime } */
 /* { dg-require-weak } */
+/* { dg-skip-if "No math insns" { *-*-amigaos } "*" "" } */
+
 double sin(double);
 double floor(double);
 float 

--- gcc/testsuite/gcc.c-torture/execute/20080424-1.c	2008-04-24 17:08:11.000000000 +0100
+++ gcc/testsuite/gcc.c-torture/execute/20080424-1.c	2025-11-02 00:56:39.098295761 +0000
@@ -1,7 +1,7 @@
 /* PR tree-optimization/36008 */
 
 extern void abort (void);
-
+int d[48] = {0};
 int g[48][3][3];
 
 void __attribute__ ((noinline))

--- gcc/testsuite/gcc.c-torture/execute/960321-1.c	1998-12-16 22:24:51.000000000 +0000
+++ gcc/testsuite/gcc.c-torture/execute/960321-1.c	2025-11-02 00:56:39.106295882 +0000
@@ -1,3 +1,6 @@
+#if defined(__baserel__) && !defined(__baserel32__)
+__far
+#endif
 char a[10] = "deadbeef";
 
 char

--- gcc/testsuite/gcc.c-torture/execute/conversion.c	2008-08-12 14:19:40.000000000 +0100
+++ gcc/testsuite/gcc.c-torture/execute/conversion.c	2025-11-02 00:56:39.114296004 +0000
@@ -543,7 +543,7 @@ main()
 {
   test_integer_to_float();
   test_float_to_integer();
-#if __GNUC__
+#if __GNUC__ && !defined(__AMIGA__)
   test_longlong_integer_to_float();
   test_float_to_longlong_integer();
 #endif

--- gcc/testsuite/gcc.c-torture/execute/loop-ivopts-1.c	2005-02-18 23:01:33.000000000 +0000
+++ gcc/testsuite/gcc.c-torture/execute/loop-ivopts-1.c	2025-11-02 00:56:39.114296004 +0000
@@ -1,4 +1,5 @@
 /* From PR 18977.  */
+char dummy[32] = {1}; // add front padding data for amiga baserel mode
 void foo(float * x);
 
 int main()

--- gcc/testsuite/gcc.c-torture/execute/pr20621-1.c	2015-01-01 01:44:11.000000000 +0000
+++ gcc/testsuite/gcc.c-torture/execute/pr20621-1.c	2025-11-02 00:56:39.118296064 +0000
@@ -1,6 +1,11 @@
 /* When generating o32 MIPS PIC, main's $gp save slot was out of range
    of a single load instruction.  */
-struct big { int i[sizeof (int) >= 4 && sizeof (void *) >= 4 ? 0x4000 : 4]; };
+#ifdef __mc68000__
+#define N 0x1FF0
+#else
+#define N 0x4000
+#endif
+struct big { int i[sizeof (int) >= 4 && sizeof (void *) >= 4 ? N : 4]; };
 struct big gb;
 int foo (struct big b, int x) { return b.i[x]; }
 #if defined(STACK_SIZE) && STACK_SIZE <= 0x10000

--- gcc/testsuite/gcc.c-torture/execute/pr39228.c	2014-09-03 13:27:52.000000000 +0100
+++ gcc/testsuite/gcc.c-torture/execute/pr39228.c	2025-11-02 00:56:39.118296064 +0000
@@ -1,5 +1,6 @@
 /* { dg-add-options ieee } */
 /* { dg-skip-if "No Inf/NaN support" { spu-*-* } "*" "" } */
+/* { dg-skip-if "No Inf/NaN support" { *-*-amigaos } "*" "" } */
 
 extern void abort (void);
 

--- gcc/testsuite/gcc.c-torture/execute/pr51581-1.c	2014-08-25 17:12:35.000000000 +0100
+++ gcc/testsuite/gcc.c-torture/execute/pr51581-1.c	2025-11-02 00:56:39.118296064 +0000
@@ -5,6 +5,10 @@
 extern void abort (void);
 
 #define N 4096
+
+#if defined(__baserel__) && !defined(__baserel32__)
+__far
+#endif
 int a[N], c[N];
 unsigned int b[N], d[N];
 

--- gcc/testsuite/gcc.c-torture/execute/pr51581-2.c	2014-08-25 17:12:35.000000000 +0100
+++ gcc/testsuite/gcc.c-torture/execute/pr51581-2.c	2025-11-02 00:56:39.118296064 +0000
@@ -4,6 +4,9 @@
 extern void abort (void);
 
 #define N 4096
+#if defined(__baserel__) && !defined(__baserel32__)
+__far
+#endif
 int a[N], c[N];
 unsigned int b[N], d[N];
 

--- gcc/testsuite/gcc.c-torture/execute/pr60822.c	2014-08-25 17:12:35.000000000 +0100
+++ gcc/testsuite/gcc.c-torture/execute/pr60822.c	2025-11-02 00:56:39.118296064 +0000
@@ -12,6 +12,9 @@ Avg(struct X *p, int s)
     return (s * (long long)(p->a + p->b)) >> 17;
 }
 
+#if defined(__baserel__) && !defined(__baserel32__)
+__far
+#endif
 struct X x;
 
 int main()

--- gcc/testsuite/lib/target-supports.exp	2018-03-27 14:07:22.000000000 +0100
+++ gcc/testsuite/lib/target-supports.exp	2025-11-02 00:56:39.758305753 +0000
@@ -6503,6 +6503,9 @@ proc check_effective_target_lto { } {
     if { [istarget nvptx-*-*] } {
 	return 0;
     }
+    if { [istarget *-*-amigaos] } {
+	return 0;
+    }
     return [check_no_compiler_messages lto object {
 	void foo (void) { }
     } "-flto"]

--- gcc/testsuite/lib/ubsan-dg.exp	2016-01-04 14:30:50.000000000 +0000
+++ gcc/testsuite/lib/ubsan-dg.exp	2025-11-02 00:56:39.762305814 +0000
@@ -18,6 +18,9 @@
 # code, 0 otherwise.
 
 proc check_effective_target_fsanitize_undefined {} {
+    if { [istarget *-*-amigaos] } {
+	return 0;
+    }
     return [check_runtime fsanitize_undefined {
 	int main (void) { return 0; }
     } "-fsanitize=undefined"]

--- gcc/toplev.c	2017-09-15 10:19:36.000000000 +0100
+++ gcc/toplev.c	2025-11-02 00:56:39.778306056 +0000
@@ -207,6 +207,19 @@ get_src_pwd (void)
       src_pwd = getpwd ();
       if (!src_pwd)
 	src_pwd = ".";
+#ifdef __CYGWIN__
+      else
+	  if (strstr(src_pwd, "/cygdrive/") == src_pwd)
+	    {
+	      int l = strlen(&src_pwd[11]) + 1;
+	      char * p = concat(src_pwd, "", NULL);
+	      p[0] = src_pwd[10];
+	      p[1] = ':';
+	      memmove(&p[2], &src_pwd[11], l);
+	      src_pwd = p;
+	    }
+#endif
+
     }
 
    return src_pwd;
@@ -2044,6 +2057,7 @@ toplev::~toplev ()
       delete g_timer;
       g_timer = NULL;
     }
+  diagnostic_finish (global_dc);
 }
 
 /* Potentially call timevar_init (which will create g_timevars if it
@@ -2130,8 +2144,6 @@ toplev::main (int argc, char **argv)
      emit some diagnostics here.  */
   invoke_plugin_callbacks (PLUGIN_FINISH, NULL);
 
-  diagnostic_finish (global_dc);
-
   finalize_plugins ();
   location_adhoc_data_fini (line_table);
 

--- gcc/tree.c	2018-06-21 08:24:06.000000000 +0100
+++ gcc/tree.c	2025-11-02 00:56:39.846307086 +0000
@@ -4803,6 +4803,29 @@ build_decl_attribute_variant (tree ddecl
   return ddecl;
 }
 
+#if defined(TARGET_AMIGAOS)
+/**
+ * Filter __chip__ and __fast__ storage attributes from type's attributes.
+ */
+tree strip_amiga_storage_attrs(tree t1)
+{
+	tree filtered = NULL;
+
+    for (;t1; t1 = TREE_CHAIN (t1))
+	{
+    	  char const * name = IDENTIFIER_POINTER(TREE_PURPOSE (t1));
+    	  if (0 == strcmp("__chip__", name) || 0 == strcmp("__far__", name))
+    	    continue;
+
+	  tree a1 = copy_node (t1);
+	  TREE_CHAIN (a1) = filtered;
+	  filtered = a1;
+	}
+
+	return filtered;
+}
+#endif
+
 /* Return a type like TTYPE except that its TYPE_ATTRIBUTE
    is ATTRIBUTE and its qualifiers are QUALS.
 
@@ -4811,7 +4834,11 @@ build_decl_attribute_variant (tree ddecl
 tree
 build_type_attribute_qual_variant (tree ttype, tree attribute, int quals)
 {
-  if (! attribute_list_equal (TYPE_ATTRIBUTES (ttype), attribute))
+  if (! attribute_list_equal (TYPE_ATTRIBUTES (ttype), attribute)
+#if defined(TARGET_AMIGAOS)
+  && ! attribute_list_equal (TYPE_ATTRIBUTES (ttype), strip_amiga_storage_attrs(attribute))
+#endif
+  )
     {
       inchash::hash hstate;
       tree ntype;

--- gcc/tree-chkp.c	2017-09-15 15:11:57.000000000 +0100
+++ gcc/tree-chkp.c	2025-11-02 00:56:39.786306177 +0000
@@ -22,6 +22,7 @@ along with GCC; see the file COPYING3.
 #include "system.h"
 #include "coretypes.h"
 #include "backend.h"
+#include "tm_p.h"
 #include "target.h"
 #include "rtl.h"
 #include "tree.h"

--- gcc/tree-outof-ssa.c	2016-03-07 11:48:57.000000000 +0000
+++ gcc/tree-outof-ssa.c	2025-11-02 00:56:39.794306298 +0000
@@ -89,7 +89,8 @@ ssa_is_replaceable_p (gimple *stmt)
 
   /* An assignment with a register variable on the RHS is not
      replaceable.  */
-  if (gimple_assign_rhs_code (stmt) == VAR_DECL
+  tree_code rcode = gimple_assign_rhs_code (stmt);
+  if (rcode == VAR_DECL
       && DECL_HARD_REGISTER (gimple_assign_rhs1 (stmt)))
     return false;
 
@@ -101,6 +102,12 @@ ssa_is_replaceable_p (gimple *stmt)
   if (gimple_has_volatile_ops (stmt))
     return false;
 
+#if defined(TARGET_M68K)
+  /* keep TARGET_MEM_REF for better scheduling. */
+  if (rcode == TARGET_MEM_REF)
+    return false;
+#endif
+
   return true;
 }
 

--- gcc/tree-pass.h	2016-02-16 20:46:17.000000000 +0000
+++ gcc/tree-pass.h	2025-11-02 00:56:39.798306359 +0000
@@ -590,6 +590,8 @@ extern rtl_opt_pass *make_pass_branch_ta
 							     *ctxt);
 extern rtl_opt_pass *make_pass_leaf_regs (gcc::context *ctxt);
 extern rtl_opt_pass *make_pass_split_before_sched2 (gcc::context *ctxt);
+extern rtl_opt_pass *make_pass_bbb_optimizations (gcc::context *ctxt);
+extern rtl_opt_pass *make_pass_bbb_baserel (gcc::context *ctxt);
 extern rtl_opt_pass *make_pass_compare_elim_after_reload (gcc::context *ctxt);
 extern rtl_opt_pass *make_pass_sched2 (gcc::context *ctxt);
 extern rtl_opt_pass *make_pass_stack_regs (gcc::context *ctxt);

--- gcc/tree-ssa-loop-ch.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/tree-ssa-loop-ch.c	2025-11-02 00:56:39.806306480 +0000
@@ -59,8 +59,40 @@ should_duplicate_loop_header_p (basic_bl
      be true, since quite often it is possible to verify that the condition is
      satisfied in the first iteration and therefore to eliminate it.  Jump
      threading handles these cases now.  */
-  if (optimize_loop_for_size_p (loop))
-    return false;
+  if (flag_loop_size_optimize == 1 || (flag_loop_size_optimize && optimize_loop_for_size_p (loop)))
+    {
+#if defined(TARGET_M68K)
+      /**
+       * The idea of copying small headers is to support dbcc.
+       *
+       * x = x - 1
+       * x == 0
+       * jcc
+       *
+       * which yields one dbcc insn.
+       *
+       * Only check this initial - when *limit is 20
+       */
+      if (*limit != 20 || !flow_bb_inside_loop_p (loop, EDGE_SUCC (header, 0)->dest))
+	return false;
+
+      /**
+       * we expect a compare before the jmp
+       */
+      basic_block next = EDGE_SUCC (header, 0)->dest;
+      for (bsi = gsi_start_bb (next); !gsi_end_p (bsi); gsi_next (&bsi))
+	{
+	  last = gsi_stmt (bsi);
+	  enum gimple_code code = gimple_code (last);
+	  if (code != GIMPLE_ASSIGN && bsi.ptr->next)
+	    return false;
+	}
+
+      *limit = 2;
+#else
+      return false;
+#endif
+    }
 
   gcc_assert (EDGE_COUNT (header->succs) > 0);
   if (single_succ_p (header))
@@ -93,6 +125,19 @@ should_duplicate_loop_header_p (basic_bl
       if (is_gimple_call (last))
 	return false;
 
+#if defined(TARGET_M68K)
+      enum gimple_code code = gimple_code (last);
+      if (code == GIMPLE_ASSIGN)
+	{
+	  tree rhs = gimple_assign_rhs1 (last);
+	  if (rhs->base.code == MEM_REF)
+	    return false;
+	  tree lhs = gimple_assign_lhs (last);
+	  if (lhs->base.code == MEM_REF)
+	    return false;
+	}
+#endif
+
       *limit -= estimate_num_insns (last, &eni_size_weights);
       if (*limit < 0)
 	return false;

--- gcc/tree-ssa-loop-ivopts.c	2017-05-09 13:27:24.000000000 +0100
+++ gcc/tree-ssa-loop-ivopts.c	2025-11-02 00:56:39.810306541 +0000
@@ -4138,7 +4138,6 @@ get_address_cost (bool symbol_present, b
 
 	  acost = seq_cost (seq, speed);
 	  acost += address_cost (addr, mem_mode, as, speed);
-
 	  if (!acost)
 	    acost = 1;
 	  data->costs[sym_p][var_p][off_p][rat_p] = acost;
@@ -4306,13 +4305,14 @@ get_shiftadd_cost (tree expr, machine_mo
   return true;
 }
 
-/* Estimates cost of forcing expression EXPR into a variable.  */
+static unsigned ivopts_integer_cost [2];
+static unsigned small_integer_cost [2];
 
+/* Estimates cost of forcing expression EXPR into a variable.  */
 static comp_cost
 force_expr_to_var_cost (tree expr, bool speed)
 {
   static bool costs_initialized = false;
-  static unsigned integer_cost [2];
   static unsigned symbol_cost [2];
   static unsigned address_cost [2];
   tree op0, op1;
@@ -4336,8 +4336,20 @@ force_expr_to_var_cost (tree expr, bool
 
       for (i = 0; i < 2; i++)
 	{
-	  integer_cost[i] = computation_cost (build_int_cst (integer_type_node,
+	  small_integer_cost[i] = computation_cost (build_int_cst (integer_type_node,
+							     2), i);
+	  if (!small_integer_cost[i])
+	    small_integer_cost[i] = 1;
+
+	  ivopts_integer_cost[i] = computation_cost (build_int_cst (integer_type_node,
 							     2000), i);
+	  if (!ivopts_integer_cost[i])
+	    ivopts_integer_cost[i] = 1;
+
+#if defined(TARGET_M68K)
+	  if (ivopts_integer_cost[i] == small_integer_cost[i])
+	    small_integer_cost[i] = ivopts_integer_cost[i] >> 1;
+#endif
 
 	  symbol_cost[i] = computation_cost (addr, i) + 1;
 
@@ -4346,7 +4358,8 @@ force_expr_to_var_cost (tree expr, bool
 	  if (dump_file && (dump_flags & TDF_DETAILS))
 	    {
 	      fprintf (dump_file, "force_expr_to_var_cost %s costs:\n", i ? "speed" : "size");
-	      fprintf (dump_file, "  integer %d\n", (int) integer_cost[i]);
+	      fprintf (dump_file, "  small integer %d\n", (int) small_integer_cost[i]);
+	      fprintf (dump_file, "  integer %d\n", (int) ivopts_integer_cost[i]);
 	      fprintf (dump_file, "  symbol %d\n", (int) symbol_cost[i]);
 	      fprintf (dump_file, "  address %d\n", (int) address_cost[i]);
 	      fprintf (dump_file, "  other %d\n", (int) target_spill_cost[i]);
@@ -4365,7 +4378,12 @@ force_expr_to_var_cost (tree expr, bool
   if (is_gimple_min_invariant (expr))
     {
       if (TREE_CODE (expr) == INTEGER_CST)
-	return new_cost (integer_cost [speed], 0);
+	{
+	  if (-0x80 <= expr->int_cst.val[0]
+	      && expr->int_cst.val[0] <= 0x7f)
+	    return new_cost (small_integer_cost [speed], 0);
+	  return new_cost (ivopts_integer_cost [speed], 0);
+	}
 
       if (TREE_CODE (expr) == ADDR_EXPR)
 	{
@@ -5076,6 +5094,10 @@ determine_use_iv_cost_generic (struct iv
   cost = get_computation_cost (data, use, cand, false, &depends_on,
                                NULL, &inv_expr_id);
 
+  // not simple...
+  if (!infinite_cost_p (cost))
+    ++cost.cost;
+
   set_use_iv_cost (data, use, cand, cost, depends_on, NULL_TREE, ERROR_MARK,
                    inv_expr_id);
 
@@ -5528,7 +5550,7 @@ may_eliminate_iv (struct ivopts_data *da
     return false;
 
   /* Sometimes, it is possible to handle the situation that the number of
-     iterations may be zero unless additional assumtions by using <
+     iterations may be zero unless additional assumptions by using <
      instead of != in the exit condition.
 
      TODO: we could also calculate the value MAY_BE_ZERO ? 0 : NITER and
@@ -5588,7 +5610,7 @@ determine_use_iv_cost_condition (struct
       if (elim_cost.cost == 0)
         elim_cost.cost = parm_decl_cost (data, bound);
       else if (TREE_CODE (bound) == INTEGER_CST)
-        elim_cost.cost = 0;
+	elim_cost.cost = 0;
       /* If we replace a loop condition 'i < n' with 'p < base + n',
 	 depends_on_elim will have 'base' and 'n' set, which implies
 	 that both 'base' and 'n' will be live during the loop.	 More likely,
@@ -5603,6 +5625,10 @@ determine_use_iv_cost_condition (struct
       /* The bound is a loop invariant, so it will be only computed
 	 once.  */
       elim_cost.cost = adjust_setup_cost (data, elim_cost.cost);
+#if 0 // DISABLED defined(TARGET_M68K)
+      if (elim_cost.cost && !infinite_cost_p (elim_cost) && TREE_CODE (bound) != INTEGER_CST)
+	elim_cost.cost += 4*ivopts_integer_cost[0];
+#endif	
     }
   else
     elim_cost = infinite_cost;
@@ -5623,11 +5649,30 @@ determine_use_iv_cost_condition (struct
       && integer_zerop (*bound_cst)
       && (operand_equal_p (*control_var, cand->var_after, 0)
 	  || operand_equal_p (*control_var, cand->var_before, 0)))
-    elim_cost.cost -= 1;
+    elim_cost.cost -= 3*ivopts_integer_cost[0];
+
 
   express_cost = get_computation_cost (data, use, cand, false,
 				       &depends_on_express, NULL,
                                        &express_inv_expr_id);
+
+#if defined(TARGET_M68K)
+  /* SBF: force use of dbra. */
+  if (!infinite_cost_p (express_cost) && express_cost.cost > small_integer_cost[0])
+    {
+      if ((integer_minus_onep(*bound_cst) || integer_zerop(*bound_cst))
+	  && integer_minus_onep(cand->iv->step))
+	{
+	  if (TYPE_PRECISION (TREE_TYPE (cand->iv->step)) == 16)
+	    express_cost.cost = small_integer_cost[0];
+	  else
+	   express_cost.cost = ivopts_integer_cost[0];
+	}
+      else if (integer_minus_onep(cand->iv->step) || integer_onep(cand->iv->step))
+	express_cost.cost = ivopts_integer_cost[0];
+    }
+#endif
+
   fd_ivopts_data = data;
   walk_tree (&cmp_iv->base, find_depends, &depends_on_express, NULL);
 
@@ -7391,6 +7436,45 @@ rewrite_use_compare (struct ivopts_data
       gimple_cond_set_lhs (cond_stmt, var);
       gimple_cond_set_code (cond_stmt, compare);
       gimple_cond_set_rhs (cond_stmt, op);
+
+      /* search the last assignment of the var used in this compare and move it in front of. */
+      gimple_stmt_iterator i = gsi_for_stmt(cond_stmt);
+      gimple_stmt_iterator j = i;
+      gsi_prev(&i);
+      for (;!gsi_end_p(i); gsi_prev(&i))
+	{
+	  gimple * g = i.ptr;
+	  /* only reorder simple expressions */
+	  if (g->code != GIMPLE_ASSIGN || (g->subcode != PLUS_EXPR && g->subcode != MINUS_EXPR))
+	    break;
+
+	  tree lhs = gimple_get_lhs(g);
+	  if (lhs->base.code != SSA_NAME)
+	    break;
+
+	  /* found the assignment to the compare variable. */
+	  if (lhs->ssa_name.var == var->ssa_name.var)
+	    {
+	      gsi_move_before(&i, &j);
+	      break;
+	    }
+
+	  /* ensure the variable is not used in between. */
+	  tree rhs = gimple_assign_rhs1(g);
+	  if (rhs->base.code == SSA_NAME && rhs->ssa_name.var == var->ssa_name.var)
+	    break;
+
+	  if (rhs->base.code != SSA_NAME && rhs->base.code != INTEGER_CST)
+	    break;
+
+	  rhs = gimple_assign_rhs2(g);
+	  if (rhs->base.code == SSA_NAME && rhs->ssa_name.var == var->ssa_name.var)
+	    break;
+
+	  if (rhs->base.code != SSA_NAME && rhs->base.code != INTEGER_CST)
+	    break;
+	}
+
       return;
     }
 

--- gcc/tree-ssa-loop-manip.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/tree-ssa-loop-manip.c	2025-11-02 00:56:39.810306541 +0000
@@ -79,6 +79,25 @@ create_iv (tree base, tree step, tree va
     {
       vb = make_temp_ssa_name (TREE_TYPE (base), NULL, "ivtmp");
       va = make_temp_ssa_name (TREE_TYPE (base), NULL, "ivtmp");
+#ifdef TARGET_M68K
+  /* SBF: use short int if possible. */
+  if (base && step
+	  && TREE_CODE (base) == INTEGER_CST
+	  && TREE_CODE (step) == INTEGER_CST)
+    {
+	  HOST_WIDE_INT vbase = int_cst_value (base);
+	  HOST_WIDE_INT vstep = int_cst_value (step);
+//	  printf("1 base=%lld step=%lld\n", vbase, vstep);
+	  if (IN_RANGE (vbase, 1, 0xffff) && IN_RANGE (vstep, -0x8000, -1))
+  	    {
+		  tree t = TYPE_UNSIGNED (base) ? short_unsigned_type_node : short_integer_type_node;
+		  base = build_int_cst(t, vbase);
+		  step = build_int_cst(t, vstep);
+		  TREE_TYPE(va) = t;
+		  TREE_TYPE(vb) = t;
+	    }
+    }
+#endif
     }
   if (var_before)
     *var_before = vb;

--- gcc/tree-ssa-pre.c	2016-11-23 14:31:16.000000000 +0000
+++ gcc/tree-ssa-pre.c	2025-11-02 00:56:39.814306601 +0000
@@ -4027,7 +4027,14 @@ eliminate_dom_walker::before_dom_childre
     {
       tree sprime = NULL_TREE;
       gimple *stmt = gsi_stmt (gsi);
+
+      if (stmt->code == GIMPLE_DEBUG || stmt->code == GIMPLE_LABEL)
+	continue;
+
       tree lhs = gimple_get_lhs (stmt);
+
+      tree rhs = gimple_assign_rhs1 (stmt);
+
       if (lhs && TREE_CODE (lhs) == SSA_NAME
 	  && !gimple_has_volatile_ops (stmt)
 	  /* See PR43491.  Do not replace a global register variable when
@@ -4038,9 +4045,12 @@ eliminate_dom_walker::before_dom_childre
 	     be ensured by not value-numbering them the same but treating
 	     them like volatiles?  */
 	  && !(gimple_assign_single_p (stmt)
-	       && (TREE_CODE (gimple_assign_rhs1 (stmt)) == VAR_DECL
-		   && DECL_HARD_REGISTER (gimple_assign_rhs1 (stmt))
-		   && is_global_var (gimple_assign_rhs1 (stmt)))))
+	       && (TREE_CODE (rhs) == VAR_DECL
+		   && DECL_HARD_REGISTER (rhs)
+		   && is_global_var (rhs)))
+	  // SBF: BIT_FIELD_REFs are sometimes mistreaded
+	  && !(rhs && TREE_CODE (rhs) == BIT_FIELD_REF)
+	  )
 	{
 	  sprime = eliminate_avail (lhs);
 	  if (!sprime)

--- gcc/tree-tailcall.c	2018-06-25 18:28:32.000000000 +0100
+++ gcc/tree-tailcall.c	2025-11-02 00:56:39.830306843 +0000
@@ -387,6 +387,42 @@ propagate_through_phis (tree var, edge e
   return var;
 }
 
+/**
+ * Return true if register params are used.
+ *
+ * The m68k implementation is also checking if only scratch regs d0/d1/a0/a1 are used.
+ */
+static
+bool func_is_using_regparms(const_tree func)
+{
+#if defined(TARGET_M68K)
+  tree attrs = TYPE_ATTRIBUTES(TREE_TYPE(func));
+  if (attrs)
+    {
+      tree attr;
+      if (0 != (attr = lookup_attribute ("asmregs", attrs)))
+	{
+	  // this is a string containing all register names like "d0a2d3a4"
+	  char const * p = IDENTIFIER_POINTER(TREE_VALUE(attr));
+	  while (*p)
+	    {
+	      if (*p >= '2' && *p <= '7')
+		return true;
+	      ++p;
+	    }
+	}
+      if (0 != (attr = lookup_attribute ("regparm", attrs)))
+	return TREE_INT_CST_LOW(TREE_VALUE(TREE_VALUE(attr))) > 2;
+      if (m68k_regparm > 2 && !lookup_attribute ("stkparm", attrs))
+	return true;
+    }
+  else if (m68k_regparm > 2)
+    return true;
+#endif
+  return false;
+}
+
+
 /* Finds tailcalls falling into basic block BB. The list of found tailcalls is
    added to the start of RET.  */
 
@@ -461,6 +497,10 @@ find_tail_calls (basic_block bb, struct
   /* We found the call, check whether it is suitable.  */
   tail_recursion = false;
   func = gimple_call_fndecl (call);
+
+  if (func && func_is_using_regparms(func))
+    return;
+
   if (func
       && !DECL_BUILT_IN (func)
       && recursive_call_p (current_function_decl, func))

--- gcc/tree-vect-stmts.c	2018-06-21 12:18:50.000000000 +0100
+++ gcc/tree-vect-stmts.c	2025-11-02 00:56:39.838306964 +0000
@@ -3603,7 +3603,6 @@ vect_create_vectorized_promotion_stmts (
    If VEC_STMT is also passed, vectorize the STMT: create a vectorized
    stmt to replace it, put it in VEC_STMT, and insert it at GSI.
    Return FALSE if not a vectorizable STMT, TRUE otherwise.  */
-
 static bool
 vectorizable_conversion (gimple *stmt, gimple_stmt_iterator *gsi,
 			 gimple **vec_stmt, slp_tree slp_node)
@@ -3619,7 +3618,7 @@ vectorizable_conversion (gimple *stmt, g
   tree decl1 = NULL_TREE, decl2 = NULL_TREE;
   tree new_temp;
   gimple *def_stmt;
-  enum vect_def_type dt[2] = {vect_unknown_def_type, vect_unknown_def_type};
+  enum vect_def_type dt[2]; dt[0] = vect_unknown_def_type; dt[1] = vect_unknown_def_type;
   gimple *new_stmt = NULL;
   stmt_vec_info prev_stmt_info;
   int nunits_in;
@@ -4209,7 +4208,7 @@ vectorizable_assignment (gimple *stmt, g
   loop_vec_info loop_vinfo = STMT_VINFO_LOOP_VINFO (stmt_info);
   tree new_temp;
   gimple *def_stmt;
-  enum vect_def_type dt[2] = {vect_unknown_def_type, vect_unknown_def_type};
+  enum vect_def_type dt[2]; dt[0] = vect_unknown_def_type; dt[1] = vect_unknown_def_type;
   int ncopies;
   int i, j;
   vec<tree> vec_oprnds = vNULL;
@@ -4420,7 +4419,7 @@ vectorizable_shift (gimple *stmt, gimple
   int icode;
   machine_mode optab_op2_mode;
   gimple *def_stmt;
-  enum vect_def_type dt[2] = {vect_unknown_def_type, vect_unknown_def_type};
+  enum vect_def_type dt[2]; dt[0] = vect_unknown_def_type; dt[1] = vect_unknown_def_type;
   gimple *new_stmt = NULL;
   stmt_vec_info prev_stmt_info;
   int nunits_in;
@@ -4793,8 +4792,8 @@ vectorizable_operation (gimple *stmt, gi
   optab optab;
   bool target_support_p;
   gimple *def_stmt;
-  enum vect_def_type dt[3]
-    = {vect_unknown_def_type, vect_unknown_def_type, vect_unknown_def_type};
+  enum vect_def_type dt[3];
+    dt[0] = vect_unknown_def_type; dt[1] = vect_unknown_def_type; dt[2] =  vect_unknown_def_type;
   gimple *new_stmt = NULL;
   stmt_vec_info prev_stmt_info;
   int nunits_in;
@@ -7827,7 +7826,7 @@ vectorizable_comparison (gimple *stmt, g
   tree vec_rhs1 = NULL_TREE, vec_rhs2 = NULL_TREE;
   tree new_temp;
   loop_vec_info loop_vinfo = STMT_VINFO_LOOP_VINFO (stmt_info);
-  enum vect_def_type dts[2] = {vect_unknown_def_type, vect_unknown_def_type};
+  enum vect_def_type dts[2]; dts[0] = vect_unknown_def_type; dts[1] = vect_unknown_def_type;
   unsigned nunits;
   int ncopies;
   enum tree_code code;

--- gcc/varasm.c	2018-06-20 12:09:28.000000000 +0100
+++ gcc/varasm.c	2025-11-02 00:56:39.862307328 +0000
@@ -252,7 +252,6 @@ get_unnamed_section (unsigned int flags,
   sect->unnamed.callback = callback;
   sect->unnamed.data = data;
   sect->unnamed.next = unnamed_sections;
-
   unnamed_sections = sect;
   return sect;
 }
@@ -1805,7 +1804,18 @@ assemble_start_function (tree decl, cons
 
       maybe_assemble_visibility (decl);
     }
-
+#if defined(TARGET_AMIGAOS)
+  else if (profile_flag)
+    {
+      char *p;
+      char * sfnname = concat("__static__", fnname, "__", DECL_SOURCE_FILE (decl), "__", dump_base_name, NULL);
+      for (p = sfnname; *p; ++p)
+        if (*p < '0' || (*p > '9' && *p < '@') || (*p > 'Z' && *p != '_' && *p < 'a') || *p > 'z')
+          *p = '.';
+      default_globalize_label(asm_out_file, sfnname);
+      ASM_OUTPUT_FUNCTION_LABEL (asm_out_file, sfnname, current_function_decl);
+    }
+#endif
   if (DECL_PRESERVE_P (decl))
     targetm.asm_out.mark_decl_preserved (fnname);
 
@@ -2228,11 +2238,17 @@ assemble_variable (tree decl, int top_le
   else
     {
       /* Special-case handling of vtv comdat sections.  */
-      if (sect->named.name
+      if ((sect->common.flags & SECTION_STYLE_MASK) == SECTION_NAMED && sect->named.name
 	  && (strcmp (sect->named.name, ".vtable_map_vars") == 0))
 	handle_vtv_comdat_section (sect, decl);
       else
-	switch_to_section (sect);
+	{
+#ifdef TARGET_AMIGAOS
+	  if ((sect->common.flags & SECTION_STYLE_MASK) == SECTION_NAMED)
+	    sect->named.decl = decl;
+#endif
+	  switch_to_section (sect);
+	}
       if (align > BITS_PER_UNIT)
 	ASM_OUTPUT_ALIGN (asm_out_file, floor_log2 (align / BITS_PER_UNIT));
       assemble_variable_contents (decl, name, dont_output_data);
@@ -4963,7 +4979,7 @@ output_constructor_regular_field (oc_loc
      if each element has the proper size.  */
   if (local->field != NULL_TREE || local->index != NULL_TREE)
     {
-      if (fieldpos > local->total_bytes)
+      if (fieldpos >= local->total_bytes)
 	{
 	  assemble_zeros (fieldpos - local->total_bytes);
 	  local->total_bytes = fieldpos;
@@ -6356,8 +6372,11 @@ default_select_section (tree decl, int r
     }
   else if (TREE_CODE (decl) == CONSTRUCTOR)
     {
-      if (! ((flag_pic && reloc)
-	     || !TREE_READONLY (decl)
+      if (! (
+#ifndef TARGET_AMIGAOS
+	  (flag_pic && reloc) ||
+#endif
+	     !TREE_READONLY (decl)
 	     || TREE_SIDE_EFFECTS (decl)
 	     || !TREE_CONSTANT (decl)))
 	return readonly_data_section;

--- gcc/varpool.c	2016-02-10 11:22:29.000000000 +0000
+++ gcc/varpool.c	2025-11-02 00:56:39.862307328 +0000
@@ -298,6 +298,9 @@ varpool_node::get_constructor (void)
   struct lto_in_decl_state *decl_state
 	 = lto_get_function_in_decl_state (file_data, decl);
 
+  if (!decl_state)
+    return NULL;
+
   data = lto_get_section_data (file_data, LTO_section_function_body,
 			       name, &len, decl_state->compressed);
   if (!data)

--- gcc/var-tracking.c	2016-01-26 20:57:37.000000000 +0000
+++ gcc/var-tracking.c	2025-11-02 00:56:39.858307267 +0000
@@ -92,10 +92,10 @@
 #include "target.h"
 #include "rtl.h"
 #include "tree.h"
+#include "tm_p.h"
 #include "cfghooks.h"
 #include "alloc-pool.h"
 #include "tree-pass.h"
-#include "tm_p.h"
 #include "insn-config.h"
 #include "regs.h"
 #include "emit-rtl.h"

--- gcc/web.c	2016-01-04 14:30:50.000000000 +0000
+++ gcc/web.c	2025-11-02 00:56:39.862307328 +0000
@@ -99,6 +99,7 @@ union_match_dups (rtx_insn *insn, web_en
   df_ref use_link = DF_INSN_INFO_USES (insn_info);
   df_ref def_link = DF_INSN_INFO_DEFS (insn_info);
   struct web_entry *dup_entry;
+
   int i;
 
   extract_insn (insn);
@@ -272,13 +273,31 @@ entry_register (web_entry *entry, df_ref
     newreg = reg, used[REGNO (reg)] = 1;
   else
     {
-      newreg = gen_reg_rtx (GET_MODE (reg));
-      REG_USERVAR_P (newreg) = REG_USERVAR_P (reg);
-      REG_POINTER (newreg) = REG_POINTER (reg);
-      REG_ATTRS (newreg) = REG_ATTRS (reg);
-      if (dump_file)
-	fprintf (dump_file, "Web oldreg=%i newreg=%i\n", REGNO (reg),
-		 REGNO (newreg));
+      rtx_insn * last = 0;
+      df_ref d;
+      rtx set;
+      for (d = ref; d; d = DF_REF_NEXT_REG (d))
+	if (DF_REF_FLAGS (d) & DF_REF_READ_WRITE)
+	  {
+	    last = DF_REF_INSN(d);
+	    break;
+	  }
+      if (last)
+	{
+	  newreg = reg, used[REGNO (reg)] = 1;
+	  if (dump_file)
+	    fprintf (dump_file, "Web keep reg=%i\n", REGNO (reg));
+	}
+      else
+	{
+	  newreg = gen_reg_rtx (GET_MODE (reg));
+	  REG_USERVAR_P (newreg) = REG_USERVAR_P (reg);
+	  REG_POINTER (newreg) = REG_POINTER (reg);
+	  REG_ATTRS (newreg) = REG_ATTRS (reg);
+	  if (dump_file)
+	    fprintf (dump_file, "Web oldreg=%i newreg=%i\n", REGNO (reg),
+		     REGNO (newreg));
+	}
     }
 
   root->set_reg (newreg);
@@ -293,14 +312,16 @@ replace_ref (df_ref ref, rtx reg)
   rtx oldreg = DF_REF_REAL_REG (ref);
   rtx *loc = DF_REF_REAL_LOC (ref);
   unsigned int uid = DF_REF_INSN_UID (ref);
+  rtx_insn * insn = DF_REF_INSN (ref);
 
-  if (oldreg == reg)
+  /* SBF: ignore regs marked as REG_INC. */
+  if (oldreg == reg || find_reg_note(insn, REG_INC, oldreg))
     return;
   if (dump_file)
     fprintf (dump_file, "Updating insn %i (%i->%i)\n",
 	     uid, REGNO (oldreg), REGNO (reg));
   *loc = reg;
-  df_insn_rescan (DF_REF_INSN (ref));
+  df_insn_rescan (insn);
 }
 
 
@@ -404,6 +425,7 @@ pass_web::execute (function *fun)
 	{
 	  struct df_insn_info *insn_info = DF_INSN_INFO_GET (insn);
 	  df_ref def, use;
+
 	  FOR_EACH_INSN_INFO_USE (use, insn_info)
 	    if (DF_REF_REGNO (use) >= FIRST_PSEUDO_REGISTER)
 	      replace_ref (use, entry_register (use_entry + DF_REF_ID (use),
Only in ../gcc: .git

--- .gitignore	2015-11-19 12:32:09.000000000 +0000
+++ .gitignore	2025-11-02 00:56:37.110265664 +0000
@@ -49,3 +49,5 @@ lost+found
 # ignore ./contrib/gcc_update output
 LAST_UPDATED
 REVISION
+
+/.settings

--- include/libiberty.h	2016-01-05 20:23:30.000000000 +0000
+++ include/libiberty.h	2025-11-02 00:56:39.882307631 +0000
@@ -705,7 +705,9 @@ extern void stack_limit_increase (unsign
    USE_C_ALLOCA yourself.  The canonical autoconf macro C_ALLOCA is
    also set/unset as it is often used to indicate whether code needs
    to call alloca(0).  */
+#ifndef _MSC_VER
 extern void *C_alloca (size_t) ATTRIBUTE_MALLOC;
+#endif
 #undef alloca
 #if GCC_VERSION >= 2000 && !defined USE_C_ALLOCA
 # define alloca(x) __builtin_alloca(x)
@@ -716,6 +718,8 @@ extern void *C_alloca (size_t) ATTRIBUTE
    char *const libiberty_nptr = (char *const) alloca (libiberty_len); \
    (char *) memcpy (libiberty_nptr, libiberty_optr, libiberty_len); }))
 #else
+void * C_alloca(unsigned int size);
+
 # define alloca(x) C_alloca(x)
 # undef USE_C_ALLOCA
 # define USE_C_ALLOCA 1
Only in ./gcc-6.5.0/INSTALL: binaries.html
Only in ./gcc-6.5.0/INSTALL: build.html
Only in ./gcc-6.5.0/INSTALL: configure.html
Only in ./gcc-6.5.0/INSTALL: download.html
Only in ./gcc-6.5.0/INSTALL: finalinstall.html
Only in ./gcc-6.5.0/INSTALL: gfdl.html
Only in ./gcc-6.5.0/INSTALL: index.html
Only in ./gcc-6.5.0/INSTALL: old.html
Only in ./gcc-6.5.0/INSTALL: prerequisites.html
Only in ./gcc-6.5.0/INSTALL: specific.html
Only in ./gcc-6.5.0/INSTALL: test.html

--- intl/localcharset.c	2005-05-13 09:03:47.000000000 +0100
+++ intl/localcharset.c	2025-11-02 00:56:39.890307752 +0000
@@ -265,7 +265,7 @@ get_charset_aliases ()
    name.  */
 
 #ifdef STATIC
-STATIC
+//STATIC
 #endif
 const char *
 locale_charset ()
Only in ./gcc-6.5.0: LAST_UPDATED
Only in ../gcc/libatomic/config: amigaos

--- libatomic/configure.tgt	2016-05-10 08:03:07.000000000 +0100
+++ libatomic/configure.tgt	2025-11-02 00:56:39.898307873 +0000
@@ -113,6 +113,10 @@ case "${target}" in
 	config_path="${config_path} linux/arm posix"
 	;;
 
+  *-*-amiga*)
+	config_path="${config_path} amigaos/loadstore amigaos/lock"
+  	;;
+
   *-*-linux* | *-*-gnu* | *-*-k*bsd*-gnu \
   | *-*-netbsd* | *-*-freebsd* | *-*-openbsd* | *-*-dragonfly* \
   | *-*-solaris2* | *-*-sysv4* | *-*-irix6* | *-*-osf* | *-*-hpux11* \

--- libbacktrace/configure	2018-10-26 10:56:36.000000000 +0100
+++ libbacktrace/configure	2025-11-02 00:56:39.906307994 +0000
@@ -11717,6 +11717,15 @@ else
   PIC_FLAG=
 fi
 
+case $host_os in
+    amigaos*)
+      case $host_cpu in
+      m68k)
+        PIC_FLAG=
+      ;;
+    esac
+  ;;
+esac
 
 
 # Test for __sync support.

--- libbacktrace/Makefile.in	2018-10-26 10:56:36.000000000 +0100
+++ libbacktrace/Makefile.in	2025-11-02 00:56:39.902307933 +0000
@@ -411,6 +411,9 @@ clean-noinstLTLIBRARIES:
 libbacktrace.la: $(libbacktrace_la_OBJECTS) $(libbacktrace_la_DEPENDENCIES) $(EXTRA_libbacktrace_la_DEPENDENCIES) 
 	$(LINK)  $(libbacktrace_la_OBJECTS) $(libbacktrace_la_LIBADD) $(LIBS)
 
+libbacktrace.dll: libbacktrace.la
+	$(CC) $(patsubst %.lo,%.o,$(libbacktrace_la_OBJECTS) $(libbacktrace_la_LIBADD)) -shared -o $@
+
 clean-checkPROGRAMS:
 	@list='$(check_PROGRAMS)'; test -n "$$list" || exit 0; \
 	echo " rm -f" $$list; \

--- libcpp/configure	2018-10-26 10:56:36.000000000 +0100
+++ libcpp/configure	2025-11-02 00:56:39.938308478 +0000
@@ -7353,7 +7353,15 @@ else
   PICFLAG=
 fi
 
-
+case $host_os in
+    amigaos*)
+      case $host_cpu in
+      m68k)
+        PICFLAG=
+      ;;
+    esac
+  ;;
+esac
 
 # Output.
 

--- libcpp/lex.c	2018-06-15 09:51:28.000000000 +0100
+++ libcpp/lex.c	2025-11-02 00:56:39.942308539 +0000
@@ -64,6 +64,10 @@ static tokenrun *next_tokenrun (tokenrun
 
 static _cpp_buff *new_buff (size_t);
 
+/*
+ * SBF: This flag is set if an asm statement is parsed, to support multiline strings in __asm()
+ */
+int in_assembler_directive;
 
 /* Utility routine:
 
@@ -1063,7 +1067,10 @@ _cpp_process_line_notes (cpp_reader *pfi
       else if (note->type == 0)
 	/* Already processed in lex_raw_string.  */;
       else
-	abort ();
+	{
+//	  abort ();
+	  printf("ups: note type=%d\n", note->type);
+	}
     }
 }
 
@@ -1875,6 +1882,20 @@ lex_string (cpp_reader *pfile, cpp_token
 	break;
       else if (c == '\n')
 	{
+	  /*
+	   *  SBF: allow multi-line strings
+	   *  Ignore the line end and move to next line.
+	   *  Only fail, if there is no next line
+	   */
+	  if (in_assembler_directive)
+	    {
+	      cpp_buffer *buffer = pfile->buffer;
+	      if (buffer->cur < buffer->rlimit)
+		CPP_INCREMENT_LINE (pfile, 0);
+	      buffer->need_line = true;
+	      if (_cpp_get_fresh_line (pfile))
+		  continue;
+	    }
 	  cur--;
 	  /* Unmatched quotes always yield undefined behavior, but
 	     greedy lexing means that what appears to be an unterminated

--- libcpp/Makefile.in	2018-10-26 10:56:36.000000000 +0100
+++ libcpp/Makefile.in	2025-11-02 00:56:39.934308418 +0000
@@ -101,6 +101,10 @@ libcpp.a: $(libcpp_a_OBJS)
 	$(AR) $(ARFLAGS) libcpp.a $(libcpp_a_OBJS)
 	$(RANLIB) libcpp.a
 
+libcpp.dll: $(libcpp_a_OBJS)
+	rm -rf $@
+	$(CXX) -o $@ -shared $(libcpp_a_OBJS) ../libiberty/libiberty.dll ../gcc/libcommon.dll -liconv
+
 # Rules to rebuild the configuration
 
 Makefile: $(srcdir)/Makefile.in config.status
Only in ./gcc-6.5.0/libcpp/po: be.gmo
Only in ./gcc-6.5.0/libcpp/po: ca.gmo
Only in ./gcc-6.5.0/libcpp/po: da.gmo
Only in ./gcc-6.5.0/libcpp/po: de.gmo
Only in ./gcc-6.5.0/libcpp/po: el.gmo
Only in ./gcc-6.5.0/libcpp/po: eo.gmo
Only in ./gcc-6.5.0/libcpp/po: es.gmo
Only in ./gcc-6.5.0/libcpp/po: fi.gmo
Only in ./gcc-6.5.0/libcpp/po: fr.gmo
Only in ./gcc-6.5.0/libcpp/po: id.gmo
Only in ./gcc-6.5.0/libcpp/po: ja.gmo
Only in ./gcc-6.5.0/libcpp/po: nl.gmo
Only in ./gcc-6.5.0/libcpp/po: pt_BR.gmo
Only in ./gcc-6.5.0/libcpp/po: ru.gmo
Only in ./gcc-6.5.0/libcpp/po: sr.gmo
Only in ./gcc-6.5.0/libcpp/po: sv.gmo
Only in ./gcc-6.5.0/libcpp/po: tr.gmo
Only in ./gcc-6.5.0/libcpp/po: uk.gmo
Only in ./gcc-6.5.0/libcpp/po: vi.gmo
Only in ./gcc-6.5.0/libcpp/po: zh_CN.gmo
Only in ./gcc-6.5.0/libcpp/po: zh_TW.gmo

--- libdecnumber/configure	2015-06-30 17:42:07.000000000 +0100
+++ libdecnumber/configure	2025-11-02 00:56:39.958308781 +0000
@@ -4900,7 +4900,15 @@ else
   PICFLAG=
 fi
 
-
+case $host_os in
+    amigaos*)
+      case $host_cpu in
+      m68k)
+        PICFLAG=
+      ;;
+    esac
+  ;;
+esac
 
 # Output.
 

--- libdecnumber/Makefile.in	2016-01-04 14:30:50.000000000 +0000
+++ libdecnumber/Makefile.in	2025-11-02 00:56:39.958308781 +0000
@@ -87,6 +87,10 @@ libdecnumber.a: $(libdecnumber_a_OBJS)
 	$(AR) $(ARFLAGS) $@ $(libdecnumber_a_OBJS)
 	$(RANLIB) $@
 
+libdecnumber.dll: $(libcpp_a_OBJS)
+	rm -rf $@
+	$(CXX) -o $@ -shared $(libdecnumber_a_OBJS) 
+
 # Rules to rebuild the configuration
 
 Makefile: $(srcdir)/Makefile.in config.status
Only in ./gcc-6.5.0/libffi/doc: libffi.info

--- libgcc/config/m68k/fpgnulib.c	2011-11-02 15:23:48.000000000 +0000
+++ libgcc/config/m68k/fpgnulib.c	2025-11-02 00:56:40.074310537 +0000
@@ -49,6 +49,15 @@
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
+
 /* the following deal with IEEE single-precision numbers */
 #define EXCESS		126L
 #define SIGNBIT		0x80000000L
@@ -105,6 +114,7 @@ union long_double_long
 
 #ifndef EXTFLOAT
 
+#ifdef __UNORDSF2
 int
 __unordsf2(float a, float b)
 {
@@ -118,7 +128,9 @@ __unordsf2(float a, float b)
     return 1;
   return 0;
 }
+#endif
 
+#ifdef __UNORDDF2
 int
 __unorddf2(double a, double b)
 {
@@ -134,7 +146,9 @@ __unorddf2(double a, double b)
     return 1;
   return 0;
 }
+#endif
 
+#ifdef __FLOATUNSIDF
 /* convert unsigned int to double */
 double
 __floatunsidf (unsigned long a1)
@@ -167,7 +181,9 @@ __floatunsidf (unsigned long a1)
 
   return dl.d;
 }
+#endif
 
+#ifdef __FLOATSIDF
 /* convert int to double */
 double
 __floatsidf (long a1)
@@ -213,7 +229,9 @@ __floatsidf (long a1)
 
   return dl.d;
 }
+#endif
 
+#ifdef __FLOATUNSISF
 /* convert unsigned int to float */
 float
 __floatunsisf (unsigned long l)
@@ -221,7 +239,10 @@ __floatunsisf (unsigned long l)
   double foo = __floatunsidf (l);
   return foo;
 }
+#endif
 
+
+#ifdef __FLOATSISF
 /* convert int to float */
 float
 __floatsisf (long l)
@@ -229,7 +250,10 @@ __floatsisf (long l)
   double foo = __floatsidf (l);
   return foo;
 }
+#endif
+
 
+#ifdef __EXTENDSFDF2
 /* convert float to double */
 double
 __extendsfdf2 (float a1)
@@ -268,7 +292,9 @@ __extendsfdf2 (float a1)
 	
   return dl.d;
 }
+#endif
 
+#ifdef __TRUNCDFSF2
 /* convert double to float */
 float
 __truncdfsf2 (double a1)
@@ -336,7 +362,9 @@ __truncdfsf2 (double a1)
   fl.l = PACK (SIGND (dl1), exp, mant);
   return (fl.f);
 }
+#endif
 
+#ifdef __FIXDFSI
 /* convert double to int */
 long
 __fixdfsi (double a1)
@@ -368,7 +396,9 @@ __fixdfsi (double a1)
 
   return (SIGND (dl1) ? -l : l);
 }
+#endif
 
+#ifdef __FIXSFSI
 /* convert float to int */
 long
 __fixsfsi (float a1)
@@ -376,6 +406,7 @@ __fixsfsi (float a1)
   double foo = a1;
   return __fixdfsi (foo);
 }
+#endif
 
 #else /* EXTFLOAT */
 
@@ -387,14 +418,7 @@ __fixsfsi (float a1)
 
    We assume all numbers are normalized, don't do any rounding, etc.  */
 
-/* Prototypes for the above in case we use them.  */
-double __floatunsidf (unsigned long);
-double __floatsidf (long);
-float __floatsisf (long);
-double __extendsfdf2 (float);
-float __truncdfsf2 (double);
-long __fixdfsi (double);
-long __fixsfsi (float);
+#if !defined(EXTFLOATCMP)
 
 int
 __unordxf2(long double a, long double b)
@@ -445,38 +469,6 @@ __extenddfxf2 (double d)
   return ldl.ld;
 }
 
-/* convert long double to double */
-double
-__truncxfdf2 (long double ld)
-{
-  register long exp;
-  register union double_long dl;
-  register union long_double_long ldl;
-
-  ldl.ld = ld;
-  /*printf ("xfdf in: %s\n", dumpxf (ld));*/
-
-  dl.l.upper = SIGNX (ldl);
-  if ((ldl.l.upper & ~SIGNBIT) == 0 && !ldl.l.middle && !ldl.l.lower)
-    {
-      dl.l.lower = 0;
-      return dl.d;
-    }
-
-  exp = EXPX (ldl) - EXCESSX + EXCESSD;
-  /* ??? quick and dirty: keep `exp' sane */
-  if (exp >= EXPDMASK)
-    exp = EXPDMASK - 1;
-  dl.l.upper |= exp << (32 - (EXPDBITS + 1));
-  /* +1-1: add one for sign bit, but take one off for explicit-integer-bit */
-  dl.l.upper |= (ldl.l.middle & MANTXMASK) >> (EXPDBITS + 1 - 1);
-  dl.l.lower = (ldl.l.middle & MANTXMASK) << (32 - (EXPDBITS + 1 - 1));
-  dl.l.lower |= ldl.l.lower >> (EXPDBITS + 1 - 1);
-
-  /*printf ("xfdf out: %g\n", dl.d);*/
-  return dl.d;
-}
-
 /* convert a float to a long double */
 long double
 __extendsfxf2 (float f)
@@ -549,6 +541,8 @@ __negxf2 (long double x1)
   return - (double) x1;
 }
 
+#else
+
 long
 __cmpxf2 (long double x1, long double x2)
 {
@@ -591,5 +585,38 @@ __gexf2 (long double x1, long double x2)
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
Only in ../gcc/libgcc/config/m68k: gmon.c
Only in ../gcc/libgcc/config/m68k: gmon.h
Only in ../gcc/libgcc/config/m68k: new_op.c

--- libgcc/config/m68k/t-floatlib	2011-11-02 15:23:48.000000000 +0000
+++ libgcc/config/m68k/t-floatlib	2025-11-02 00:56:40.074310537 +0000
@@ -1,11 +1,62 @@
-LIB1ASMSRC = m68k/lb1sf68.S
-LIB1ASMFUNCS = _mulsi3 _udivsi3 _divsi3 _umodsi3 _modsi3 \
-   _double _float _floatex \
-   _eqdf2 _nedf2 _gtdf2 _gedf2 _ltdf2 _ledf2 \
-   _eqsf2 _nesf2 _gtsf2 _gesf2 _ltsf2 _lesf2
-
-LIB2ADD = $(srcdir)/config/m68k/fpgnulib.c xfgnulib.c
-
-xfgnulib.c: $(srcdir)/config/m68k/fpgnulib.c
-	echo '#define EXTFLOAT' > xfgnulib.c
-	cat $< >> xfgnulib.c
+#
+#LIB1ASMSRC = m68k/lb1sf68.S
+#LIB1ASMFUNCS = _mulsi3 _udivsi3 _divsi3 _umodsi3 _modsi3 \
+#   _double _float _floatex \
+#   _eqdf2 _nedf2 _gtdf2 _gedf2 _ltdf2 _ledf2 \
+#   _eqsf2 _nesf2 _gtsf2 _gesf2 _ltsf2 _lesf2
+#
+
+LIB2ADD +=  xfpgnulib.c xfpgnulib__unordsf2.c xfpgnulib__unorddf2.c \
+	xfpgnulib__floatunsidf.c xfpgnulib__floatsidf.c xfpgnulib__floatunsisf.c \
+	xfpgnulib__floatsisf.c xfpgnulib__extendsfdf2.c xfpgnulib__truncdfsf2.c \
+	xfpgnulib__fixdfsi.c xfpgnulib__fixsfsi.c xfpgnulib__cmpxf2.c
+
+xfpgnulib__unordsf2.c: $(srcdir)/config/m68k/fpgnulib.c
+	echo '#define __UNORDSF2' > xfpgnulib__unordsf2.c
+	cat $< >> xfpgnulib__unordsf2.c
+
+xfpgnulib__unorddf2.c: $(srcdir)/config/m68k/fpgnulib.c
+	echo '#define __UNORDDF2' > xfpgnulib__unorddf2.c
+	cat $< >> xfpgnulib__unorddf2.c
+
+xfpgnulib__floatunsidf.c: $(srcdir)/config/m68k/fpgnulib.c
+	echo '#define __FLOATUNSIDF' > xfpgnulib__floatunsidf.c
+	cat $< >> xfpgnulib__floatunsidf.c
+
+xfpgnulib__floatsidf.c: $(srcdir)/config/m68k/fpgnulib.c
+	echo '#define __FLOATSIDF' > xfpgnulib__floatsidf.c
+	cat $< >> xfpgnulib__floatsidf.c
+
+xfpgnulib__floatunsisf.c: $(srcdir)/config/m68k/fpgnulib.c
+	echo '#define __FLOATUNSISF' > xfpgnulib__floatunsisf.c
+	cat $< >> xfpgnulib__floatunsisf.c
+
+xfpgnulib__floatsisf.c: $(srcdir)/config/m68k/fpgnulib.c
+	echo '#define __FLOATSISF' > xfpgnulib__floatsisf.c
+	cat $< >> xfpgnulib__floatsisf.c
+
+xfpgnulib__extendsfdf2.c: $(srcdir)/config/m68k/fpgnulib.c
+	echo '#define __EXTENDSFDF2' > xfpgnulib__extendsfdf2.c
+	cat $< >> xfpgnulib__extendsfdf2.c
+
+xfpgnulib__truncdfsf2.c: $(srcdir)/config/m68k/fpgnulib.c
+	echo '#define __TRUNCDFSF2' > xfpgnulib__truncdfsf2.c
+	cat $< >> xfpgnulib__truncdfsf2.c
+
+xfpgnulib__fixdfsi.c: $(srcdir)/config/m68k/fpgnulib.c
+	echo '#define __FIXDFSI' > xfpgnulib__fixdfsi.c
+	cat $< >> xfpgnulib__fixdfsi.c
+
+xfpgnulib__fixsfsi.c: $(srcdir)/config/m68k/fpgnulib.c
+	echo '#define __FIXSFSI' > xfpgnulib__fixsfsi.c
+	cat $< >> xfpgnulib__fixsfsi.c
+
+xfpgnulib.c: $(srcdir)/config/m68k/fpgnulib.c
+	echo '#define EXTFLOAT' > xfpgnulib.c
+	cat $< >> xfpgnulib.c
+
+xfpgnulib__cmpxf2.c: $(srcdir)/config/m68k/fpgnulib.c
+	echo '#define EXTFLOAT' > xfpgnulib__cmpxf2.c
+	echo '#define EXTFLOATCMP' >> xfpgnulib__cmpxf2.c
+	cat $< >> xfpgnulib__cmpxf2.c
+	
\ No newline at end of file
Only in ../gcc/libgcc/config/m68k: t-glue

--- libgcc/config.host	2017-11-21 09:41:34.000000000 +0000
+++ libgcc/config.host	2025-11-02 00:56:39.986309205 +0000
@@ -816,6 +816,11 @@ m32r-*-linux*)
 m32rle-*-linux*)
 	tmake_file="$tmake_file m32r/t-linux t-fdpbit"
 	;;
+m68k-*-amiga*)
+	tmake_file="$tmake_file m68k/t-glue m68k/t-floatlib soft-fp"
+	CFLAGS="-Os"
+#	tmake_file="$tmake_file m68k/t-glue soft-fp"
+	;;
 m68k-*-elf* | fido-*-elf)
 	tmake_file="$tmake_file m68k/t-floatlib"
 	;;

--- libgcc/Makefile.in	2016-02-25 12:23:52.000000000 +0000
+++ libgcc/Makefile.in	2025-11-02 00:56:39.986309205 +0000
@@ -229,8 +229,8 @@ endif
 
 # Options to use when compiling libgcc2.a.
 #
-LIBGCC2_DEBUG_CFLAGS = -g
-LIBGCC2_CFLAGS = -O2 $(LIBGCC2_INCLUDES) $(GCC_CFLAGS) $(HOST_LIBGCC2_CFLAGS) \
+LIBGCC2_DEBUG_CFLAGS = 
+LIBGCC2_CFLAGS = $(LIBGCC2_INCLUDES) $(GCC_CFLAGS) $(HOST_LIBGCC2_CFLAGS) \
 		 $(LIBGCC2_DEBUG_CFLAGS) -DIN_LIBGCC2 \
 		 -fbuilding-libgcc -fno-stack-protector \
 		 $(INHIBIT_LIBC_CFLAGS)
@@ -284,7 +284,7 @@ INTERNAL_CFLAGS = $(CFLAGS) $(LIBGCC2_CF
 		  $(INCLUDES) @set_have_cc_tls@ @set_use_emutls@
 
 # Options to use when compiling crtbegin/end.
-CRTSTUFF_CFLAGS = -O2 $(GCC_CFLAGS) $(INCLUDES) $(MULTILIB_CFLAGS) -g0 \
+CRTSTUFF_CFLAGS = $(GCC_CFLAGS) $(INCLUDES) $(MULTILIB_CFLAGS)  \
   $(NO_PIE_CFLAGS) -finhibit-size-directive -fno-inline -fno-exceptions \
   -fno-zero-initialized-in-bss -fno-toplevel-reorder -fno-tree-vectorize \
   -fbuilding-libgcc -fno-stack-protector $(FORCE_EXPLICIT_EH_REGISTRY) \

--- libgcc/unwind-dw2.c	2016-01-04 14:30:50.000000000 +0000
+++ libgcc/unwind-dw2.c	2025-11-02 00:56:40.114311143 +0000
@@ -22,6 +22,9 @@
    see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
    <http://www.gnu.org/licenses/>.  */
 
+#pragma GCC push_options
+#pragma GCC optimize ("-O2")
+
 #include "tconfig.h"
 #include "tsystem.h"
 #include "coretypes.h"
@@ -260,6 +263,9 @@ _Unwind_GetCFA (struct _Unwind_Context *
 }
 
 /* Overwrite the saved value for register INDEX in CONTEXT with VAL.  */
+#if defined(TARGET_AMIGAOS)
+static int overregs[16];
+#endif
 
 inline void
 _Unwind_SetGR (struct _Unwind_Context *context, int index, _Unwind_Word val)
@@ -271,6 +277,9 @@ _Unwind_SetGR (struct _Unwind_Context *c
   gcc_assert (index < (int) sizeof(dwarf_reg_size_table));
   size = dwarf_reg_size_table[index];
 
+#if defined(TARGET_AMIGAOS)
+  overregs[index] = val;
+#endif
   if (_Unwind_IsExtendedContext (context) && context->by_value[index])
     {
       context->reg[index] = _Unwind_Get_Unwind_Context_Reg_Val (val);
@@ -279,6 +288,9 @@ _Unwind_SetGR (struct _Unwind_Context *c
 
   ptr = (void *) (_Unwind_Internal_Ptr) context->reg[index];
 
+  if (!ptr)
+    return;
+
   if (size == sizeof(_Unwind_Ptr))
     * (_Unwind_Ptr *) ptr = val;
   else
@@ -1612,10 +1624,10 @@ _Unwind_DebugHook (void *cfa __attribute
    macro because __builtin_eh_return must be invoked in the context of
    our caller.  */
 
-#define uw_install_context(CURRENT, TARGET)				\
+#define uw_install_context(CURRENT, TARGET, INDEX)				\
   do									\
     {									\
-      long offset = uw_install_context_1 ((CURRENT), (TARGET));		\
+      long offset = uw_install_context_1 ((CURRENT), (TARGET), (INDEX));		\
       void *handler = __builtin_frob_return_addr ((TARGET)->ra);	\
       _Unwind_DebugHook ((TARGET)->cfa, handler);			\
       __builtin_eh_return (offset, handler);				\
@@ -1624,7 +1636,8 @@ _Unwind_DebugHook (void *cfa __attribute
 
 static long
 uw_install_context_1 (struct _Unwind_Context *current,
-		      struct _Unwind_Context *target)
+		      struct _Unwind_Context *target,
+		      int index ATTRIBUTE_UNUSED)
 {
   long i;
   _Unwind_SpTmp sp_slot;
@@ -1659,7 +1672,75 @@ uw_install_context_1 (struct _Unwind_Con
       else if (t && c && t != c)
 	memcpy (c, t, dwarf_reg_size_table[i]);
     }
+#if defined(TARGET_AMIGAOS)
+  /* SBF: evil hack to patch the values for d0/d1 into the stack location.
+   * search the movem insn and count the saved regs.
+   * Now patch the values into location.
+   * Always patch d0/d1 since override is always invoked for d0/d1.
+   * Then patch all other regs which the above code omitted.
+   */
+  /* uw_install_context_1 is called from 4 different locations - each uses an unique index.
+   * So initialization is only done once.
+   */
+  static unsigned short counts[4];
+  static unsigned short masks[4];
+
+  unsigned short count = 0;
+  unsigned short reg_mask = masks[index];
+  /* init each index once. */
+  if (!reg_mask)
+    {
+      /* get the return address.*/
+      unsigned short * sp = *(((unsigned short **)&current) - 1);
+      /* search the movem -x(a5),regs insn.*/
+      for (;;)
+	{
+	  unsigned short s = *sp++;
+//      printf("%04x ", s);
+	  gcc_assert(s != (unsigned short)0x4e75);// hit return? ouch!
+	  if (s == (unsigned short)0x4ced)
+	    break;
+	}
+      reg_mask = *sp;
+      /* count saved regs */
+      for (unsigned short i = 0, m = reg_mask; i < 16; ++i)
+	{
+	  if (m & 1)
+	  ++count;
+	  m >>= 1;
+	}
+      masks[index] = reg_mask;
+      counts[index] = count;
+    }
+  else
+    count = counts[index];
 
+  /* regs are saved below local vars -> start at current */
+  int * p = ((int *)current) - count;
+
+  for (unsigned short i = 0, m = reg_mask; i < 16; ++i)
+    {
+      if (m & 1)
+	{
+	  if (i <= 1 || (!current->reg[i] && (target->reg[i] || target->by_value[i])))
+	    {
+	      int old = *p;
+	      /* not set by the code above - set it here */
+	      if (i <= 1) // use the override values for d0/d1
+	        *p = overregs[i];
+	      else
+	      if (target->by_value[i])
+	        *p = (int)target->reg[i];
+	      else
+	        *p = *(int*)target->reg[i];
+//	      printf("patch reg %d from %08lx to %08lx\n", i, old, *p);
+	    }
+	  ++p;
+	}
+      m >>= 1;
+    }
+
+#endif
   /* If the current frame doesn't have a saved stack pointer, then we
      need to rely on EH_RETURN_STACKADJ_RTX to get our target stack
      pointer value reloaded.  */
@@ -1713,3 +1794,5 @@ alias (_Unwind_SetIP);
 #endif
 
 #endif /* !USING_SJLJ_EXCEPTIONS */
+
+#pragma GCC pop_options

--- libgcc/unwind-dw2-fde.c	2016-01-04 14:30:50.000000000 +0000
+++ libgcc/unwind-dw2-fde.c	2025-11-02 00:56:40.114311143 +0000
@@ -440,6 +440,9 @@ static inline void
 fde_split (struct object *ob, fde_compare_t fde_compare,
 	   struct fde_vector *linear, struct fde_vector *erratic)
 {
+#ifdef __amiga__
+__near
+#endif
   static const fde *marker;
   size_t count = linear->count;
   const fde *const *chain_end = &marker;
@@ -749,6 +752,9 @@ init_object (struct object* ob)
 	  count = classify_object_over_fdes (ob, ob->u.single);
 	  if (count == (size_t) -1)
 	    {
+#ifdef __amiga__
+__near
+#endif
 	      static const fde terminator;
 	    unhandled_fdes:
 	      ob->s.i = 0;

--- libgcc/unwind.inc	2016-01-04 14:30:50.000000000 +0000
+++ libgcc/unwind.inc	2025-11-02 00:56:40.114311143 +0000
@@ -100,12 +100,12 @@ _Unwind_RaiseException(struct _Unwind_Ex
 
       if (code == _URC_END_OF_STACK)
 	/* Hit end of stack with no handler found.  */
-	return _URC_END_OF_STACK;
+	{ code = _URC_END_OF_STACK; goto KOT; }
 
       if (code != _URC_NO_REASON)
 	/* Some error encountered.  Usually the unwinder doesn't
 	   diagnose these and merely crashes.  */
-	return _URC_FATAL_PHASE1_ERROR;
+  	{ code = _URC_FATAL_PHASE1_ERROR; goto KOT; }
 
       /* Unwind successful.  Run the personality routine, if any.  */
       if (fs.personality)
@@ -115,7 +115,7 @@ _Unwind_RaiseException(struct _Unwind_Ex
 	  if (code == _URC_HANDLER_FOUND)
 	    break;
 	  else if (code != _URC_CONTINUE_UNWIND)
-	    return _URC_FATAL_PHASE1_ERROR;
+	  	{ code = _URC_FATAL_PHASE1_ERROR; goto KOT; }
 	}
 
       /* Update cur_context to describe the same frame as fs.  */
@@ -129,10 +129,15 @@ _Unwind_RaiseException(struct _Unwind_Ex
 
   cur_context = this_context;
   code = _Unwind_RaiseException_Phase2 (exc, &cur_context);
-  if (code != _URC_INSTALL_CONTEXT)
+KOT:
+  if (code != _URC_INSTALL_CONTEXT) {
+#if defined(TARGET_AMIGAOS)
+	  asm ("move.l %0, (sp)" :: "r"(code));
+#endif
     return code;
+  }
 
-  uw_install_context (&this_context, &cur_context);
+  uw_install_context (&this_context, &cur_context, 0);
 }
 
 
@@ -208,7 +213,7 @@ _Unwind_ForcedUnwind (struct _Unwind_Exc
   if (code != _URC_INSTALL_CONTEXT)
     return code;
 
-  uw_install_context (&this_context, &cur_context);
+  uw_install_context (&this_context, &cur_context, 1);
 }
 
 
@@ -233,7 +238,7 @@ _Unwind_Resume (struct _Unwind_Exception
 
   gcc_assert (code == _URC_INSTALL_CONTEXT);
 
-  uw_install_context (&this_context, &cur_context);
+  uw_install_context (&this_context, &cur_context, 2);
 }
 
 
@@ -258,7 +263,7 @@ _Unwind_Resume_or_Rethrow (struct _Unwin
 
   gcc_assert (code == _URC_INSTALL_CONTEXT);
 
-  uw_install_context (&this_context, &cur_context);
+  uw_install_context (&this_context, &cur_context, 3);
 }
 
 

--- libgfortran/configure	2018-10-26 10:56:36.000000000 +0100
+++ libgfortran/configure	2025-11-02 00:56:40.126311325 +0000
@@ -5768,7 +5768,14 @@ fi
 
 # Add -Wall -fno-repack-arrays -fno-underscoring if we are using GCC.
 if test "x$GCC" = "xyes"; then
-  AM_FCFLAGS="-I . -Wall -Werror -fimplicit-none -fno-repack-arrays -fno-underscoring"
+  case "${host_cpu}" in
+    amiga*)
+      AM_FCFLAGS="-I . -Wall -Werror -fimplicit-none -fno-repack-arrays"
+    ;;
+    *)
+      AM_FCFLAGS="-I . -Wall -Werror -fimplicit-none -fno-repack-arrays -fno-underscoring"
+    ;;
+  esac
   ## We like to use C11 and C99 routines when available.  This makes
   ## sure that
   ## __STDC_VERSION__ is set such that libc includes make them available.

--- libgfortran/runtime/compile_options.c	2016-01-04 14:30:50.000000000 +0000
+++ libgfortran/runtime/compile_options.c	2025-11-02 00:56:40.198312415 +0000
@@ -29,7 +29,7 @@ see the files COPYING3 and COPYING.RUNTI
 /* Useful compile-time options will be stored in here.  */
 compile_options_t compile_options;
 
-#ifndef LIBGFOR_MINIMAL
+#if !defined(LIBGFOR_MINIMAL) && !defined(__AMIGA__)
 static volatile sig_atomic_t fatal_error_in_progress = 0;
 
 
@@ -167,7 +167,7 @@ set_options (int num, int options[])
   if (num >= 9)
     compile_options.fpe_summary = options[8];
 
-#ifndef LIBGFOR_MINIMAL
+#if !defined(LIBGFOR_MINIMAL) && !defined(__AMIGA__)
   /* If backtrace is required, we set signal handlers on the POSIX
      2001 signals with core action.  */
   if (compile_options.backtrace)

--- libgfortran/runtime/error.c	2016-01-04 14:30:50.000000000 +0000
+++ libgfortran/runtime/error.c	2025-11-02 00:56:40.198312415 +0000
@@ -175,7 +175,9 @@ sys_abort (void)
       || (options.backtrace == -1 && compile_options.backtrace == 1))
     {
       estr_write ("\nProgram aborted. Backtrace:\n");
+#ifndef __AMIGA__
       show_backtrace (false);
+#endif
       signal (SIGABRT, SIG_DFL);
     }
 
@@ -193,7 +195,9 @@ exit_error (int status)
       || (options.backtrace == -1 && compile_options.backtrace == 1))
     {
       estr_write ("\nError termination. Backtrace:\n");
+#ifndef __AMIGA__
       show_backtrace (false);
+#endif
     }
   exit (status);
 }
Only in ./gcc-6.5.0/libgomp: libgomp.info

--- libiberty/configure	2015-11-09 04:32:08.000000000 +0000
+++ libiberty/configure	2025-11-02 00:56:40.446316169 +0000
@@ -5090,6 +5090,9 @@ fi
 
 case "${host}" in
     # PIC is the default on some targets or must not be used.
+    *-*-amigaos*)
+    	PICFLAG=
+    	;;
     *-*-darwin*)
 	# For darwin, common symbols are not allowed in MH_DYLIB files
 	case "${CFLAGS}" in

--- libiberty/cp-demangle.c	2016-12-13 06:20:44.000000000 +0000
+++ libiberty/cp-demangle.c	2025-11-02 00:56:40.450316230 +0000
@@ -2250,6 +2250,9 @@ d_ctor_dtor_name (struct d_info *di)
                   ::= u <source-name>
 */
 
+#ifdef __amiga__
+__near
+#endif
 CP_STATIC_IF_GLIBCPP_V3
 const struct demangle_builtin_type_info
 cplus_demangle_builtin_types[D_BUILTIN_TYPE_COUNT] =

--- libiberty/cp-demangle.h	2015-07-13 06:49:54.000000000 +0100
+++ libiberty/cp-demangle.h	2025-11-02 00:56:40.450316230 +0000
@@ -180,6 +180,9 @@ extern const struct demangle_operator_in
 
 #define D_BUILTIN_TYPE_COUNT (33)
 
+#ifdef __amiga__
+__near
+#endif
 CP_STATIC_IF_GLIBCPP_V3
 const struct demangle_builtin_type_info
 cplus_demangle_builtin_types[D_BUILTIN_TYPE_COUNT];

--- libiberty/lrealpath.c	2005-05-24 21:48:25.000000000 +0100
+++ libiberty/lrealpath.c	2025-11-02 00:56:40.454316290 +0000
@@ -73,7 +73,7 @@ extern char *canonicalize_file_name (con
 #endif
 
 char *
-lrealpath (const char *filename)
+__xlrealpath (const char *filename)
 {
   /* Method 1: The system has a compile time upper bound on a filename
      path.  Use that and realpath() to canonicalize the name.  This is
@@ -155,3 +155,42 @@ lrealpath (const char *filename)
   /* This system is a lost cause, just duplicate the filename.  */
   return strdup (filename);
 }
+
+
+char *
+lrealpath (const char *filename)
+{
+#ifdef __MSYS__
+  /* MSYS does not treat paths with drives correctly.
+   * 
+   *   convert c:/foo to /c/foo
+   */
+  char * tmp;
+  if (filename[0] && filename[1] == ':')
+    {
+      char letter[3] = {'/', filename[0], 0};
+      tmp = filename = concat(letter, &filename[2], 0);
+    }
+#endif
+
+  char * r = __xlrealpath(filename);
+#if defined (_WIN32)
+  if (strncmp(r, "/cygdrive/", 10) == 0)
+    {
+      r[9] = r[10];
+      r[10] = ':';
+      r = strdup(&r[9]);
+    }
+#endif
+#ifdef __MSYS__
+  if (tmp)
+    {
+     /* 
+      *   convert it back /c/foo to c:/foo 
+      */
+      char drive[3] = { r[1], ':', 0};
+      r = concat(drive, &r[2], 0);
+    }
+#endif
+  return r;
+}

--- libiberty/Makefile.in	2015-11-12 19:18:54.000000000 +0000
+++ libiberty/Makefile.in	2025-11-02 00:56:40.446316169 +0000
@@ -148,7 +148,7 @@ CFILES = alloca.c argv.c asprintf.c atex
 	random.c regex.c rename.c rindex.c				\
 	safe-ctype.c setenv.c setproctitle.c sha1.c sigsetmask.c        \
 	 simple-object.c simple-object-coff.c simple-object-elf.c	\
-	 simple-object-mach-o.c simple-object-xcoff.c			\
+	 simple-object-mach-o.c simple-object-xcoff.c	simple-object-amigahunk.c		\
          snprintf.c sort.c						\
 	 spaces.c splay-tree.c stack-limit.c stpcpy.c stpncpy.c		\
 	 strcasecmp.c strchr.c strdup.c strerror.c strncasecmp.c	\
@@ -186,7 +186,7 @@ REQUIRED_OFILES =							\
 	./safe-ctype.$(objext)						\
 	./simple-object.$(objext) ./simple-object-coff.$(objext)	\
 	./simple-object-elf.$(objext) ./simple-object-mach-o.$(objext)	\
-	./simple-object-xcoff.$(objext)					\
+	./simple-object-xcoff.$(objext)	./simple-object-amigahunk.$(objext)				\
 	./sort.$(objext) ./spaces.$(objext)				\
 	./splay-tree.$(objext) ./stack-limit.$(objext)			\
 	./strerror.$(objext) ./strsignal.$(objext)			\
@@ -264,6 +264,10 @@ $(TARGETLIB): $(REQUIRED_OFILES) $(EXTRA
 	  cd ..; \
 	else true; fi
 
+libiberty.dll: $(REQUIRED_OFILES) $(EXTRA_OFILES) $(LIBOBJS)
+	rm -rf $@
+	$(CC) -o $@ -shared $(REQUIRED_OFILES) $(EXTRA_OFILES) $(LIBOBJS)
+
 $(TESTLIB): $(REQUIRED_OFILES) $(CONFIGURED_OFILES)
 	-rm -f $(TESTLIB)
 	$(AR) $(AR_FLAGS) $(TESTLIB) \
@@ -1278,6 +1282,17 @@ $(CONFIGURED_OFILES): stamp-picdir stamp
 	else true; fi
 	$(COMPILE.c) $(srcdir)/simple-object-xcoff.c $(OUTPUT_OPTION)
 
+./simple-object-amigahunk.$(objext): $(srcdir)/simple-object-amigahunk.c config.h \
+	$(INCDIR)/ansidecl.h $(INCDIR)/libiberty.h \
+	$(srcdir)/simple-object-common.h $(INCDIR)/simple-object.h
+	if [ x"$(PICFLAG)" != x ]; then \
+	  $(COMPILE.c) $(PICFLAG) $(srcdir)/simple-object-amigahunk.c -o pic/$@; \
+	else true; fi
+	if [ x"$(NOASANFLAG)" != x ]; then \
+	  $(COMPILE.c) $(PICFLAG) $(NOASANFLAG) $(srcdir)/simple-object-amigahunk.c -o noasan/$@; \
+	else true; fi
+	$(COMPILE.c) $(srcdir)/simple-object-amigahunk.c $(OUTPUT_OPTION)
+
 ./simple-object.$(objext): $(srcdir)/simple-object.c config.h \
 	$(INCDIR)/ansidecl.h $(INCDIR)/libiberty.h \
 	$(srcdir)/simple-object-common.h $(INCDIR)/simple-object.h

--- libiberty/make-relative-prefix.c	2012-01-26 14:26:25.000000000 +0000
+++ libiberty/make-relative-prefix.c	2025-11-02 00:56:40.454316290 +0000
@@ -61,12 +61,19 @@ relative prefix can be found, return @co
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
@@ -214,6 +221,32 @@ free_split_directories (char **dirs)
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
@@ -233,170 +266,107 @@ make_relative_prefix_1 (const char *prog
   int i, n, common;
   int needed_len;
   char *ret = NULL, *ptr, *full_progname;
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
+#ifdef __amiga__
+//printf("prog=<%s> bin_prefix=<%s> prefix=<%s>\t", progname, bin_prefix, prefix);
 #endif
-	  nstore = (char *) alloca (len);
-
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
-
-  if (resolve_links)
-    full_progname = lrealpath (progname);
-  else
-    full_progname = strdup (progname);
-  if (full_progname == NULL)
-    return NULL;
 
-  prog_dirs = split_directories (full_progname, &prog_num);
-  free (full_progname);
-  if (prog_dirs == NULL)
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
     return NULL;
+  else if (n)
+    buf[n] = 0;
 
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
     }
-  *ptr = '\0';
-
-  /* Put in directories to move over to prefix.  */
-  for (i = common; i < prefix_num; i++)
-    strcat (ret, prefix_dirs[i]);
-
- bailout:
-  free_split_directories (prog_dirs);
-  free_split_directories (bin_dirs);
-  free_split_directories (prefix_dirs);
+#endif
+  if (d) closedir(d);
+  else
+    {
+// printf("can't open dir: %s\n", p);
+      free(p);
+      strcpy(buf, prefix);
+      buf[q - prefix] = 0;
+      p = concat(buf, q, NULL);
+    }
+  
+#ifdef __amiga__
+//  printf("->\t<%s>\n", p);
+#endif
 
-  return ret;
+  return p;
 }
 
 

--- libiberty/make-temp-file.c	2014-10-15 21:20:05.000000000 +0100
+++ libiberty/make-temp-file.c	2025-11-02 00:56:40.454316290 +0000
@@ -115,6 +115,10 @@ choose_tmpdir (void)
 #ifdef VMS
       /* Try VMS standard temp logical.  */
       base = try_dir ("/sys$scratch", base);
+#elif defined(__amiga__)
+      if (!memoized_tmpdir)
+	memoized_tmpdir = xstrdup("T:");
+      return memoized_tmpdir;
 #else
       base = try_dir (getenv ("TMPDIR"), base);
       base = try_dir (getenv ("TMP"), base);

--- libiberty/pex-unix.c	2015-10-18 11:33:37.000000000 +0100
+++ libiberty/pex-unix.c	2025-11-02 00:56:40.454316290 +0000
@@ -584,6 +584,19 @@ pex_unix_exec_child (struct pex_obj *obj
   return (pid_t) -1;
 }
 
+#elif defined(__amiga__)
+static pid_t
+pex_unix_exec_child (struct pex_obj *obj, int flags, const char *executable,
+		     char * const * argv, char * const * env,
+                     int in, int out, int errdes,
+		     int toclose, const char **errmsg, int *err)
+{
+  int r = execv(executable, argv);
+//  printf("pex_unix_exec_child %s %p -> %d\n", executable, argv, r);
+  *err = r;
+  *errmsg = 0;
+  return r ? -1 : 0;
+}
 #else
 /* Implementation of pex->exec_child using standard vfork + exec.  */
 
@@ -735,6 +748,11 @@ pex_unix_wait (struct pex_obj *obj, pid_
   if (done)
     kill (pid, SIGTERM);
 
+#ifdef __amiga__
+  *status = *err = 0;
+  return 0;
+#endif
+
   if (pex_wait (obj, pid, status, time) < 0)
     {
       *err = errno;
Only in ../gcc/libiberty: simple-object-amigahunk.c

--- libiberty/simple-object.c	2014-04-01 08:45:48.000000000 +0100
+++ libiberty/simple-object.c	2025-11-02 00:56:40.458316351 +0000
@@ -52,7 +52,8 @@ static const struct simple_object_functi
   &simple_object_elf_functions,
   &simple_object_mach_o_functions,
   &simple_object_coff_functions,
-  &simple_object_xcoff_functions
+  &simple_object_xcoff_functions,
+  &simple_object_amigahunk_functions
 };
 
 /* Read data from a file using the simple_object error reporting

--- libiberty/simple-object-common.h	2013-01-02 02:04:42.000000000 +0000
+++ libiberty/simple-object-common.h	2025-11-02 00:56:40.458316351 +0000
@@ -149,6 +149,7 @@ extern const struct simple_object_functi
 extern const struct simple_object_functions simple_object_elf_functions;
 extern const struct simple_object_functions simple_object_mach_o_functions;
 extern const struct simple_object_functions simple_object_xcoff_functions;
+extern const struct simple_object_functions simple_object_amigahunk_functions;
 
 /* Read SIZE bytes from DESCRIPTOR at file offset OFFSET into BUFFER.
    Return non-zero on success.  On failure return 0 and set *ERRMSG

--- libiberty/strsignal.c	2008-06-19 16:08:53.000000000 +0100
+++ libiberty/strsignal.c	2025-11-02 00:56:40.458316351 +0000
@@ -551,7 +551,7 @@ followed by a newline.
 #ifndef HAVE_PSIGNAL
 
 void
-psignal (int signo, char *message)
+psignal (int signo, const char *message)
 {
   if (signal_names == NULL)
     {
Only in ./gcc-6.5.0/libitm: libitm.info
Only in ./gcc-6.5.0/libjava/classpath/doc: cp-tools.info
Only in ./gcc-6.5.0/libjava/classpath/doc: gappletviewer.1
Only in ./gcc-6.5.0/libjava/classpath/doc: gjar.1
Only in ./gcc-6.5.0/libjava/classpath/doc: gjarsigner.1
Only in ./gcc-6.5.0/libjava/classpath/doc: gjavah.1
Only in ./gcc-6.5.0/libjava/classpath/doc: gjdoc.1
Only in ./gcc-6.5.0/libjava/classpath/doc: gkeytool.1
Only in ./gcc-6.5.0/libjava/classpath/doc: gnative2ascii.1
Only in ./gcc-6.5.0/libjava/classpath/doc: gorbd.1
Only in ./gcc-6.5.0/libjava/classpath/doc: grmid.1
Only in ./gcc-6.5.0/libjava/classpath/doc: grmiregistry.1
Only in ./gcc-6.5.0/libjava/classpath/doc: gserialver.1
Only in ./gcc-6.5.0/libjava/classpath/doc: gtnameserv.1

--- libobjc/configure	2018-10-26 10:56:36.000000000 +0100
+++ libobjc/configure	2025-11-02 00:56:41.894338091 +0000
@@ -7637,7 +7637,8 @@ $as_echo_n "checking for $compiler optio
             # FIXME: we need at least 68020 code to build shared libraries, but
             # adding the `-m68020' flag to GCC prevents building anything better,
             # like `-m68040'.
-            lt_prog_compiler_pic='-m68020 -resident32 -malways-restore-a4'
+            #lt_prog_compiler_pic='-m68020 -resident32 -malways-restore-a4'
+            enable_shared=no
         ;;
       esac
       ;;

--- libobjc/objc/objc.h	2016-01-04 14:30:50.000000000 +0000
+++ libobjc/objc/objc.h	2025-11-02 00:56:41.894338091 +0000
@@ -52,7 +52,11 @@ extern "C" {
    Important: this could change and we could switch to 'typedef bool
    BOOL' in the future.  Do not depend on the type of BOOL.  */
 #undef BOOL
+#ifdef AMIGA
+typedef short  BOOL;
+#else
 typedef unsigned char  BOOL;
+#endif
 
 #define YES   (BOOL)1
 #define NO    (BOOL)0
Only in ./gcc-6.5.0/libquadmath: libquadmath.info

--- libstdc++-v3/config/os/newlib/ctype_configure_char.cc	2016-01-04 14:30:50.000000000 +0000
+++ libstdc++-v3/config/os/newlib/ctype_configure_char.cc	2025-11-02 00:56:41.982339423 +0000
@@ -65,6 +65,15 @@ _GLIBCXX_BEGIN_NAMESPACE_VERSION
     _M_narrow_ok = 0;
   }
 
+#ifdef __AMIGA__
+  ctype<char>::~ctype()
+  {
+    _S_destroy_c_locale(_M_c_locale_ctype);
+    if (_M_del)
+      delete[] this->table();
+  }
+#endif
+
   char
   ctype<char>::do_toupper(char __c) const
   {

--- libstdc++-v3/config/os/newlib/os_defines.h	2016-01-29 10:52:08.000000000 +0000
+++ libstdc++-v3/config/os/newlib/os_defines.h	2025-11-02 00:56:41.982339423 +0000
@@ -56,6 +56,9 @@
 // See libstdc++/69506
 #define _GLIBCXX_USE_WEAK_REF 0
 
+#elif defined (__AMIGA__)
+#define _GLIBCXX_GTHREAD_USE_WEAK 0
+#define _GLIBCXX_USE_WEAK_REF 0
 #endif
 
 #endif

--- libstdc++-v3/configure	2018-02-19 17:02:38.000000000 +0000
+++ libstdc++-v3/configure	2025-11-02 00:56:42.006339786 +0000
@@ -8638,6 +8638,7 @@ $as_echo_n "checking for $compiler optio
             # adding the `-m68020' flag to GCC prevents building anything better,
             # like `-m68040'.
             lt_prog_compiler_pic='-m68020 -resident32 -malways-restore-a4'
+            enable_shared=no
         ;;
       esac
       ;;
@@ -28885,6 +28886,10 @@ else
 
 # Base decisions on target environment.
 case "${host}" in
+  m68k-*-*)
+  	# Nothing to do here.
+  	;;
+
   arm*-*-symbianelf*)
     # This is a freestanding configuration; there is nothing to do here.
     ;;

--- libstdc++-v3/configure.host	2016-09-27 07:11:15.000000000 +0100
+++ libstdc++-v3/configure.host	2025-11-02 00:56:42.006339786 +0000
@@ -226,6 +226,11 @@ case "${host_os}" in
     os_include_dir="os/generic"
     atomicity_dir="cpu/generic"
     ;;
+  amiga*)
+        os_include_dir="os/newlib"
+        CFLAGS="${TARGET_C_FLAGS}"
+        CXXFLAGS="${TARGET_C_FLAGS}"
+        ;;
   bsd*)
     # Plain BSD attempts to share FreeBSD files.
     os_include_dir="os/bsd/freebsd"

--- libstdc++-v3/include/bits/basic_string.h	2018-10-25 17:42:01.000000000 +0100
+++ libstdc++-v3/include/bits/basic_string.h	2025-11-02 00:56:42.082340937 +0000
@@ -5469,9 +5469,11 @@ _GLIBCXX_BEGIN_NAMESPACE_CXX11
   stod(const string& __str, size_t* __idx = 0)
   { return __gnu_cxx::__stoa(&std::strtod, "stod", __str.c_str(), __idx); }
 
+#ifdef _HAVE_LONG_DOUBLE
   inline long double
   stold(const string& __str, size_t* __idx = 0)
   { return __gnu_cxx::__stoa(&std::strtold, "stold", __str.c_str(), __idx); }
+#endif
 #endif // _GLIBCXX_USE_C99_STDLIB
 
 #if _GLIBCXX_USE_C99_STDIO
@@ -5575,9 +5577,11 @@ _GLIBCXX_BEGIN_NAMESPACE_CXX11
   stod(const wstring& __str, size_t* __idx = 0)
   { return __gnu_cxx::__stoa(&std::wcstod, "stod", __str.c_str(), __idx); }
 
+#ifdef _HAVE_LONG_DOUBLE
   inline long double
   stold(const wstring& __str, size_t* __idx = 0)
   { return __gnu_cxx::__stoa(&std::wcstold, "stold", __str.c_str(), __idx); }
+#endif
 
 #ifndef _GLIBCXX_HAVE_BROKEN_VSWPRINTF
   // DR 1261.

--- libstdc++-v3/include/bits/codecvt.h	2016-01-04 14:30:50.000000000 +0000
+++ libstdc++-v3/include/bits/codecvt.h	2025-11-02 00:56:42.082340937 +0000
@@ -298,7 +298,7 @@ _GLIBCXX_BEGIN_NAMESPACE_VERSION
 
     protected:
       virtual
-      ~codecvt() { }
+      ~codecvt();
 
       virtual result
       do_out(state_type& __state, const intern_type* __from,
@@ -478,8 +478,7 @@ _GLIBCXX_BEGIN_NAMESPACE_VERSION
       static locale::id			id;
 
       explicit
-      codecvt(size_t __refs = 0)
-      : __codecvt_abstract_base<char16_t, char, mbstate_t>(__refs) { }
+      codecvt(size_t __refs = 0);
 
     protected:
       virtual
@@ -535,8 +534,7 @@ _GLIBCXX_BEGIN_NAMESPACE_VERSION
       static locale::id			id;
 
       explicit
-      codecvt(size_t __refs = 0)
-      : __codecvt_abstract_base<char32_t, char, mbstate_t>(__refs) { }
+      codecvt(size_t __refs = 0);
 
     protected:
       virtual

--- libstdc++-v3/include/c_global/cstdlib	2016-01-20 12:34:25.000000000 +0000
+++ libstdc++-v3/include/c_global/cstdlib	2025-11-02 00:56:42.106341300 +0000
@@ -249,8 +249,9 @@ _GLIBCXX_BEGIN_NAMESPACE_VERSION
   using ::strtoull;
 #endif
   using ::strtof;
+#ifdef _HAVE_LONG_DOUBLE
   using ::strtold;
-
+#endif
 _GLIBCXX_END_NAMESPACE_VERSION
 } // namespace __gnu_cxx
 
@@ -269,7 +270,9 @@ namespace std
   using ::__gnu_cxx::strtof;
   using ::__gnu_cxx::strtoll;
   using ::__gnu_cxx::strtoull;
+#ifdef _HAVE_LONG_DOUBLE
   using ::__gnu_cxx::strtold;
+#endif
 } // namespace std
 
 #endif // _GLIBCXX_USE_C99_STDLIB

--- libstdc++-v3/include/Makefile.in	2018-02-19 17:02:38.000000000 +0000
+++ libstdc++-v3/include/Makefile.in	2025-11-02 00:56:42.078340876 +0000
@@ -13,6 +13,8 @@
 # even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 # PARTICULAR PURPOSE.
 
+UNAME := $(shell uname)
+
 @SET_MAKE@
 VPATH = @srcdir@
 am__make_dryrun = \
@@ -1230,6 +1232,7 @@ thread_host_headers = \
 	${host_builddir}/gthr-posix.h \
 	${host_builddir}/gthr-default.h
 
+ifeq (,$(findstring MSYS,$(UNAME)))
 pch1_source = ${glibcxx_srcdir}/include/precompiled/stdc++.h
 pch1_output_builddir = ${host_builddir}/stdc++.h.gch
 pch1_output_anchor = ${host_builddir}/stdc++.h
@@ -1257,6 +1260,7 @@ pch_output_anchors = \
 PCHFLAGS = -x c++-header -nostdinc++ $(CXXFLAGS) $(VTV_PCH_CXXFLAGS)
 @GLIBCXX_BUILD_PCH_FALSE@pch_build = 
 @GLIBCXX_BUILD_PCH_TRUE@pch_build = ${pch_output}
+endif
 
 # List of all timestamp files.  By keeping only one copy of this list, both
 # CLEANFILES and all-local are kept up-to-date.
@@ -1713,6 +1717,7 @@ ${host_builddir}/gthr-default.h: ${tople
 	    -e 's,^#include "\(.*\)",#include <bits/\1>,g' \
 	    < $< > $@
 
+ifeq (,$(findstring MSYS,$(UNAME)))
 # Build two precompiled C++ includes, stdc++.h.gch/*.gch
 ${pch1a_output}: ${allstamped} ${host_builddir}/c++config.h ${pch1_source}
 	-mkdir -p ${pch1_output_builddir}
@@ -1732,6 +1737,7 @@ ${pch2_output}: ${pch2_source} ${pch1_ou
 ${pch3_output}: ${pch3_source} ${pch2_output}
 	-mkdir -p ${pch3_output_builddir}
 	$(CXX) $(PCHFLAGS) $(AM_CPPFLAGS) -O2 -g ${pch3_source} -o $@
+endif
 
 # For robustness sake (in light of junk files or in-source
 # configuration), copy from the build or source tree to the install
@@ -1859,9 +1865,14 @@ install-headers:
 	for file in ${ext_host_headers}; do \
 	  $(INSTALL_DATA) $${file} $(DESTDIR)${host_installdir}/../ext; done
 
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

--- libstdc++-v3/include/std/memory	2017-03-02 18:58:07.000000000 +0000
+++ libstdc++-v3/include/std/memory	2025-11-02 00:56:42.154342027 +0000
@@ -115,14 +115,14 @@ inline void*
 align(size_t __align, size_t __size, void*& __ptr, size_t& __space) noexcept
 {
   const auto __intptr = reinterpret_cast<uintptr_t>(__ptr);
-  const auto __aligned = (__intptr - 1u + __align) & -__align;
-  const auto __diff = __aligned - __intptr;
+  const auto __alignedx = (__intptr - 1u + __align) & -__align;
+  const auto __diff = __alignedx - __intptr;
   if ((__size + __diff) > __space)
     return nullptr;
   else
     {
       __space -= __diff;
-      return __ptr = reinterpret_cast<void*>(__aligned);
+      return __ptr = reinterpret_cast<void*>(__alignedx);
     }
 }
 

--- libstdc++-v3/include/tr1/cstdint	2016-01-04 14:30:50.000000000 +0000
+++ libstdc++-v3/include/tr1/cstdint	2025-11-02 00:56:42.158342088 +0000
@@ -30,7 +30,9 @@
 #define _GLIBCXX_TR1_CSTDINT 1
 
 #pragma GCC system_header
-
+#ifdef AMIGA
+#include <stdint.h>
+#endif
 #include <bits/c++config.h>
 
 // For 8.22.1/1 (see C99, Notes 219, 220, 222)

--- libstdc++-v3/src/c++11/codecvt.cc	2018-08-08 16:40:17.000000000 +0100
+++ libstdc++-v3/src/c++11/codecvt.cc	2025-11-02 00:56:42.174342330 +0000
@@ -733,6 +733,10 @@ namespace
 
 locale::id codecvt<char16_t, char, mbstate_t>::id;
 
+codecvt<char16_t, char, mbstate_t>::codecvt(size_t __refs)
+      : __codecvt_abstract_base<char16_t, char, mbstate_t>(__refs)
+{}
+
 codecvt<char16_t, char, mbstate_t>::~codecvt() { }
 
 codecvt_base::result
@@ -810,6 +814,10 @@ codecvt<char16_t, char, mbstate_t>::do_m
 
 locale::id codecvt<char32_t, char, mbstate_t>::id;
 
+codecvt<char32_t, char, mbstate_t>::codecvt(size_t __refs)
+: __codecvt_abstract_base<char32_t, char, mbstate_t>(__refs) { }
+
+
 codecvt<char32_t, char, mbstate_t>::~codecvt() { }
 
 codecvt_base::result

--- libstdc++-v3/src/c++11/ctype.cc	2016-01-04 14:30:50.000000000 +0000
+++ libstdc++-v3/src/c++11/ctype.cc	2025-11-02 00:56:42.174342330 +0000
@@ -51,12 +51,15 @@ _GLIBCXX_BEGIN_NAMESPACE_VERSION
 
   const size_t ctype<char>::table_size;
 
+#ifndef __AMIGA__
+/* moved to ctype_configure_char */
   ctype<char>::~ctype()
   { 
     _S_destroy_c_locale(_M_c_locale_ctype);
     if (_M_del) 
       delete[] this->table(); 
   }
+#endif
 
   // Fill in the narrowing cache and flag whether all values are
   // valid or not.  _M_narrow_ok is set to 2 if memcpy can't

--- libstdc++-v3/src/c++98/math_stubs_float.cc	2016-01-04 14:30:50.000000000 +0000
+++ libstdc++-v3/src/c++98/math_stubs_float.cc	2025-11-02 00:56:42.182342451 +0000
@@ -27,7 +27,7 @@
 // For targets which do not have support for float versions,
 // we use the following crude approximations. We keep saying that we'll do
 // better later, but never do.
-
+#ifndef __AMIGA__
 extern "C" 
 {
 #ifndef _GLIBCXX_HAVE_FABSF
@@ -222,3 +222,4 @@ extern "C"
   }
 #endif
 } // extern "C"
+#endif

--- libstdc++-v3/src/c++98/math_stubs_long_double.cc	2016-01-04 14:30:50.000000000 +0000
+++ libstdc++-v3/src/c++98/math_stubs_long_double.cc	2025-11-02 00:56:42.182342451 +0000
@@ -27,7 +27,7 @@
 // For targets which do not have support for long double versions,
 // we use the following crude approximations. We keep saying that we'll do
 // better later, but never do.
-
+#ifndef __AMIGA__
 extern "C" 
 {
 #ifndef _GLIBCXX_HAVE_FABSL
@@ -222,3 +222,4 @@ extern "C"
   }
 #endif
 } // extern "C"
+#endif
Only in ./gcc-6.5.0: MD5SUMS
Only in ./gcc-6.5.0: NEWS
Only in ../gcc: .project

--- zlib/configure	2015-05-13 12:31:32.000000000 +0100
+++ zlib/configure	2025-11-02 00:56:42.646349476 +0000
@@ -7445,7 +7445,7 @@ $as_echo_n "checking for $compiler optio
             # FIXME: we need at least 68020 code to build shared libraries, but
             # adding the `-m68020' flag to GCC prevents building anything better,
             # like `-m68040'.
-            lt_prog_compiler_pic='-m68020 -resident32 -malways-restore-a4'
+            lt_prog_compiler_pic='--invalid'
         ;;
       esac
       ;;
@@ -11215,7 +11215,15 @@ else
   PICFLAG=
 fi
 
-
+case $host_os in
+    amigaos*)
+      case $host_cpu in
+      m68k)
+        PICFLAG=
+      ;;
+    esac
+  ;;
+esac
 
 ac_config_files="$ac_config_files Makefile"
 
