Index: elf2aif/src/elf2aif.c
===================================================================
--- elf2aif/src/elf2aif.c	(revision 7698)
+++ elf2aif/src/elf2aif.c	(working copy)
@@ -201,7 +201,11 @@
       return EXIT_FAILURE;
     }
 
+#ifndef __ARM_EABI__
   if (elf_ehdr.e_ident[EI_OSABI] != ELFOSABI_ARM)
+#else
+  if (elf_ehdr.e_ident[EI_OSABI] != ELFOSABI_NONE)
+#endif
     {
       fprintf (stderr, "ELF file '%s' is not for ARM\n", elf_filename);
       return EXIT_FAILURE;
