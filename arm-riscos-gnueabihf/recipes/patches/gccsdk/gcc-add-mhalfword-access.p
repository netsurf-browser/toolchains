Index: gcc/gcc.config.arm.arm.h.p
===================================================================
--- gcc/gcc.config.arm.arm.h.p	(nonexistent)
+++ gcc/gcc.config.arm.arm.h.p	(working copy)
@@ -0,0 +1,33 @@
+--- gcc/config/arm/arm.h.orig	2020-07-23 07:35:17.344384552 +0100
++++ gcc/config/arm/arm.h	2022-05-28 22:48:25.064918314 +0100
+@@ -1338,7 +1338,9 @@
+    : (TARGET_IWMMXT && (CLASS) == IWMMXT_REGS)			\
+    ? coproc_secondary_reload_class (MODE, X, TRUE)		\
+    : TARGET_32BIT						\
+-   ? (((MODE) == HImode && ! arm_arch4 && true_regnum (X) == -1) \
++   ? (((MODE) == HImode &&					\
++      (!arm_arch4 || !halfword_access) &&			\
++      true_regnum (X) == -1)					\
+     ? GENERAL_REGS : NO_REGS)					\
+    : THUMB_SECONDARY_OUTPUT_RELOAD_CLASS (CLASS, MODE, X))
+ 
+@@ -1353,7 +1355,7 @@
+     (((CLASS) == IWMMXT_REGS || (CLASS) == IWMMXT_GR_REGS)	\
+      && CONSTANT_P (X))						\
+     ? GENERAL_REGS :						\
+-    (((MODE) == HImode && ! arm_arch4				\
++    (((MODE) == HImode && (!arm_arch4 || !halfword_access)	\
+       && (MEM_P (X)					\
+ 	  || ((REG_P (X) || GET_CODE (X) == SUBREG)	\
+ 	      && true_regnum (X) == -1)))			\
+@@ -2016,7 +2018,9 @@
+    done, UNKNOWN if none.  */
+ #define LOAD_EXTEND_OP(MODE)						\
+   (TARGET_THUMB ? ZERO_EXTEND :						\
+-   ((arm_arch4 || (MODE) == QImode) ? ZERO_EXTEND			\
++   (((arm_arch4 && (halfword_access || (MODE) != HImode)) ||		\
++     (MODE) == QImode)							\
++    ? ZERO_EXTEND							\
+     : ((BYTES_BIG_ENDIAN && (MODE) == HImode) ? SIGN_EXTEND : UNKNOWN)))
+ 
+ /* Nonzero if access to memory by bytes is slow and undesirable.  */
Index: gcc/gcc.config.arm.arm.md.p
===================================================================
--- gcc/gcc.config.arm.arm.md.p	(revision 7698)
+++ gcc/gcc.config.arm.arm.md.p	(working copy)
@@ -8,6 +8,42 @@
     (IP_REGNUM	     12)	; Scratch register
     (SP_REGNUM	     13)	; Stack pointer
     (LR_REGNUM        14)	; Return address register
+@@ -5546,7 +5547,7 @@
+ 	(zero_extend:SI (match_operand:HI 1 "nonimmediate_operand")))]
+   "TARGET_EITHER"
+ {
+-  if (TARGET_ARM && !arm_arch4 && MEM_P (operands[1]))
++  if (TARGET_ARM && (!arm_arch4 || !halfword_access) && MEM_P (operands[1]))
+     {
+       emit_insn (gen_movhi_bytes (operands[0], operands[1]));
+       DONE;
+@@ -5574,7 +5575,7 @@
+ (define_insn "*arm_zero_extendhisi2"
+   [(set (match_operand:SI 0 "s_register_operand" "=r,r")
+ 	(zero_extend:SI (match_operand:HI 1 "nonimmediate_operand" "r,m")))]
+-  "TARGET_ARM && arm_arch4 && !arm_arch6"
++  "TARGET_ARM && arm_arch4 && halfword_access && !arm_arch6"
+   "@
+    #
+    ldrh%?\\t%0, %1"
+@@ -5735,7 +5736,7 @@
+       emit_insn (gen_thumb1_extendhisi2 (operands[0], operands[1]));
+       DONE;
+     }
+-  if (MEM_P (operands[1]) && TARGET_ARM && !arm_arch4)
++  if (MEM_P (operands[1]) && TARGET_ARM && (!arm_arch4 || !halfword_access))
+     {
+       emit_insn (gen_extendhisi2_mem (operands[0], operands[1]));
+       DONE;
+@@ -5813,7 +5814,7 @@
+ (define_insn "*arm_extendhisi2"
+   [(set (match_operand:SI 0 "s_register_operand" "=r,r")
+ 	(sign_extend:SI (match_operand:HI 1 "nonimmediate_operand" "r,Uh")))]
+-  "TARGET_ARM && arm_arch4 && !arm_arch6"
++  "TARGET_ARM && arm_arch4 && halfword_access && !arm_arch6"
+   "@
+    #
+    ldrsh%?\\t%0, %1"
 @@ -6693,7 +6694,7 @@
  	(mem:SI (plus:SI (match_operand:SI 1 "register_operand" "r")
  			 (unspec:SI [(match_operand:SI 2 "" "X")]
@@ -17,3 +53,49 @@
    "ldr%?\\t%0, [%1,%2]"
    [(set_attr "type" "load_4")]
  )
+@@ -6845,7 +6846,7 @@
+ (define_expand "storehi_single_op"
+   [(set (match_operand:HI 0 "memory_operand")
+ 	(match_operand:HI 1 "general_operand"))]
+-  "TARGET_32BIT && arm_arch4"
++  "TARGET_32BIT && arm_arch4 && halfword_access"
+   "
+   if (!s_register_operand (operands[1], HImode))
+     operands[1] = copy_to_mode_reg (HImode, operands[1]);
+@@ -6865,7 +6865,7 @@
+         {
+           if (MEM_P (operands[0]))
+ 	    {
+-	      if (arm_arch4)
++	      if (arm_arch4 && halfword_access)
+ 	        {
+ 	          emit_insn (gen_storehi_single_op (operands[0], operands[1]));
+ 	          DONE;
+@@ -6905,15 +6905,15 @@
+ 	      emit_insn (gen_movsi (reg, GEN_INT (val)));
+ 	      operands[1] = gen_lowpart (HImode, reg);
+ 	    }
+-	  else if (arm_arch4 && optimize && can_create_pseudo_p ()
+-		   && MEM_P (operands[1]))
++	  else if (arm_arch4 && halfword_access && optimize
++		   && can_create_pseudo_p () && MEM_P (operands[1]))
+ 	    {
+ 	      rtx reg = gen_reg_rtx (SImode);
+ 
+ 	      emit_insn (gen_zero_extendhisi2 (reg, operands[1]));
+ 	      operands[1] = gen_lowpart (HImode, reg);
+ 	    }
+-          else if (!arm_arch4)
++          else if (!arm_arch4 || !halfword_access)
+ 	    {
+ 	      if (MEM_P (operands[1]))
+ 	        {
+@@ -7099,7 +7100,7 @@
+   [(set (match_operand:HI 0 "nonimmediate_operand" "=r,r,r,m,r")
+ 	(match_operand:HI 1 "general_operand"      "rIk,K,n,r,mi"))]
+   "TARGET_ARM
+-   && arm_arch4 && !TARGET_HARD_FLOAT
++   && arm_arch4 && halfword_access && !TARGET_HARD_FLOAT
+    && (register_operand (operands[0], HImode)
+        || register_operand (operands[1], HImode))"
+   "@
Index: gcc/gcc.config.arm.arm.opt.p
===================================================================
--- gcc/gcc.config.arm.arm.opt.p	(nonexistent)
+++ gcc/gcc.config.arm.arm.opt.p	(working copy)
@@ -0,0 +1,13 @@
+--- gcc/config/arm/arm.opt.orig	2020-07-23 07:35:17.344384552 +0100
++++ gcc/config/arm/arm.opt	2022-05-28 22:39:28.543576700 +0100
+@@ -271,6 +271,10 @@
+ Avoid overlapping destination and address registers on LDRD instructions
+ that may trigger Cortex-M3 errata.
+ 
++mhalfword-access
++Target Report Var(halfword_access) Init(1)
++Enable halfword memory accesses.
++
+ munaligned-access
+ Target Report Var(unaligned_access) Init(2) Save
+ Enable unaligned word and halfword accesses to packed data.
