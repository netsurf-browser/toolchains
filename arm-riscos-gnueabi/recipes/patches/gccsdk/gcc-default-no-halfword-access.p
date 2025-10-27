Index: gcc/gcc.config.arm.arm.opt.p
===================================================================
--- gcc/gcc.config.arm.arm.opt.p	(nonexistent)
+++ gcc/gcc.config.arm.arm.opt.p	(working copy)
@@ -5,7 +5,7 @@
  that may trigger Cortex-M3 errata.
  
 +mhalfword-access
-+Target Report Var(halfword_access) Init(1)
++Target Report Var(halfword_access) Init(0)
 +Enable halfword memory accesses.
 +
  munaligned-access
