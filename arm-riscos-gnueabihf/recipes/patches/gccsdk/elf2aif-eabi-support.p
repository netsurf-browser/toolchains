Index: elf2aif/src/elf2aif.c
===================================================================
--- elf2aif/src/elf2aif.c	(revision 7698)
+++ elf2aif/src/elf2aif.c	(working copy)
@@ -72,12 +72,13 @@
 } aifheader_t;
 
 static int opt_verbose = 0;
+static int opt_eabi = 0;
 
 static Elf32_External_Ehdr elf_ehdr;
 static phdr_list_t *elf_phdrlistP;
 static const char *elf_filename;
 
-static const unsigned int aifcode[] = {
+static const uint32_t aifcode[] = {
   0xE1A00000,			/* NOP (BL decompress)      */
   0xE1A00000,			/* NOP (BL self-relocate)   */
   0xEB00000C,			/* BL zero-init             */
@@ -113,6 +114,24 @@
   0xEAFFFFFB			/* B     zeroloop           */
 };
 
+static const uint32_t crt0code[] = {
+  0xE59F3010,			/* LDR   R3, =crt1_data     */
+  0xE583000C,			/* STR   R0, [R3, #12]      */
+  0xE583101C,			/* STR   R1, [R3, #28]      */
+  0xE5832020,			/* STR   R2, [R3, #32]      */
+  0xE1A00003			/* MOV   R0, R3             */
+ 				/* B     __main             */
+};
+
+static const uint32_t eabi_crt0code[] = {
+  0xE59F3010,			/* LDR   R3, =crt1_data     */
+  0xE583000C,			/* STR   R0, [R3, #12]      */
+  0xE3A00002,			/* MOV   R0, #2             */
+  0xEF059D01,			/* SWI   ARMEABISupport_AbortOp */
+  0xE1A00003			/* MOV   R0, R3             */
+  				/* B     __main             */
+};
+
 /* Read a little-endian 'short' value.  */
 static uint16_t
 RdShort (const uint8_t sh[2])
@@ -143,6 +162,7 @@
   fprintf (stderr, "Usage: elf2aif [options] <ELF file> [<AIF file>]\n"
 	   "Convert static ARM ELF binary to AIF (Acorn Image Format) binary.\n"
 	   "Options:\n"
+	   "  -e, --eabi	source binary uses EABI\n"
 	   "  -v, --verbose	prints informational messages during processing\n"
 	   "      --help	display this help and exit\n"
 	   "      --version	output version information and exit\n");
@@ -201,7 +221,8 @@
       return EXIT_FAILURE;
     }
 
-  if (elf_ehdr.e_ident[EI_OSABI] != ELFOSABI_ARM)
+  if ((!opt_eabi && elf_ehdr.e_ident[EI_OSABI] != ELFOSABI_ARM) ||
+      (opt_eabi && elf_ehdr.e_ident[EI_OSABI] != ELFOSABI_NONE))
     {
       fprintf (stderr, "ELF file '%s' is not for ARM\n", elf_filename);
       return EXIT_FAILURE;
@@ -503,6 +524,37 @@
       return EXIT_FAILURE;
     }
 
+  /* In the EABI case we need to inject the code to install
+   * the ARMEABISupport abort handler */
+  if (opt_eabi)
+    {
+      uint32_t crt0[5];
+      assert(sizeof (crt0code) == sizeof (crt0));
+      assert(sizeof (eabi_crt0code) == sizeof (crt0));
+
+      if (opt_verbose)
+	printf ("Rewriting crt0 at offset 0x%x\n", (exec_addr - load_addr));
+
+      if (fseek (elfhandle, (exec_addr - load_addr), SEEK_SET) != 0 ||
+	  fread (crt0, sizeof (crt0), 1, elfhandle) != 1)
+	{
+	  fprintf (stderr, "Failed to read crt0\n");
+	  return EXIT_FAILURE;
+	}
+      if (memcmp(crt0, crt0code, sizeof (crt0)) != 0)
+	{
+	  fprintf (stderr, "crt0 code not as expected\n");
+	  return EXIT_FAILURE;
+	}
+      if (fseek (aifhandle, (exec_addr - load_addr), SEEK_SET) != 0 ||
+	  fwrite (eabi_crt0code, sizeof (crt0), 1, aifhandle) != 1 ||
+	  fseek (aifhandle, aifend, SEEK_SET) != 0)
+	{
+	  fprintf (stderr, "Failed to write crt0\n");
+	  return EXIT_FAILURE;
+	}
+    }
+
   return EXIT_SUCCESS;
 }
 
@@ -683,6 +735,9 @@
 	    fprintf (stderr, "Warning: extra options/arguments ignored\n");
 	  return EXIT_SUCCESS;
 	}
+      else if (!strcmp (&argv[i][1], "-eabi")
+	       || !strcmp (&argv[i][1], "e"))
+	opt_eabi = 1;
       else if (!strcmp (&argv[i][1], "-verbose")
 	       || !strcmp (&argv[i][1], "v"))
 	++opt_verbose;
