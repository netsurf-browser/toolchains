Index: Tools/DefMod2/assembler.c
===================================================================
--- Tools/DefMod2/assembler.c	(revision 477)
+++ Tools/DefMod2/assembler.c	(working copy)
@@ -1242,6 +1242,21 @@
 		       goto finish;
                      break;
 		   }
+                 case AsmType_eGCCELFEABI:
+		   {
+                     const char *floatABI = (FloatABI == FloatABI_eSoft) ? "soft" : "hard";
+                     if (!apcs32)
+                       {
+                         fprintf(stderr, "EABI does not support APCS-26.");
+                         goto finish;
+                       }
+                     if ((rc = sprintf (cmd,
+				"arm-riscos-gnueabi-gcc -c -xassembler -mfloat-abi=%s -o %s %s",
+				floatABI,
+				name2, name1)) < 0)
+		       goto finish;
+                     break;
+		   }
                  default:
                    fprintf (stderr, "Unknown assembler type\n");
                    goto finish;
Index: Tools/DefMod2/assembler.h
===================================================================
--- Tools/DefMod2/assembler.h	(revision 477)
+++ Tools/DefMod2/assembler.h	(working copy)
@@ -22,8 +22,9 @@
   AsmType_eGCCAOF,	/* Assembler used in GCCSDK 3.4 (AOF) release, called
     'as' (predecessor of AsAsm).  */
   AsmType_eARMASM,	/* ARM's assembler.  */
-  AsmType_eGCCELF	/* Assembler used in GCCSDK 4 (ELF) release, called
+  AsmType_eGCCELF,	/* Assembler used in GCCSDK 4 (ELF) release, called
     'gas' (GNU binutils assembler).  */
+  AsmType_eGCCELFEABI	/* Assembler used in EABI-capable GCCSDK */
 } AsmType_e;
 
 typedef enum
@@ -37,7 +38,7 @@
 extern AsmType_e AsmType;
 extern FloatABI_e FloatABI;
   
-#define GASSyntax (AsmType == AsmType_eGCCELF)
+#define GASSyntax (AsmType == AsmType_eGCCELF || AsmType == AsmType_eGCCELFEABI)
 
 /*if |separate|, output is to the directory |output|; otherwise, to the file |file|.*/
 extern os_error *assembler_output (FILE *file, const char *title, const char *author, lookup_t types, lookup_t swis, osbool separate, osbool apcs32, char *output);
Index: Tools/DefMod2/defmod.y
===================================================================
--- Tools/DefMod2/defmod.y	(revision 477)
+++ Tools/DefMod2/defmod.y	(working copy)
@@ -1179,7 +1179,7 @@
                      "[-o <output-dir> | > <output-file>] "
                      "[-byte_wide <byte-wide-file>] "
                      "[-26bit | -32bit] "
-                     "[-asmtype [objasm | asasmaof | asasmelf | gccaof | armasm | gccelf]] "
+                     "[-asmtype [objasm | asasmaof | asasmelf | gccaof | armasm | gccelf | gccelfeabi]] "
 		     "[-float-abi [soft | hard]] "
                      "< <module-interface-file>\r"
                "Purpose: generate output from a module interface file\r"
@@ -1203,6 +1203,7 @@
                "\tgccaof\t\t"       "...use GCC AOF toolkit (GCCSDK 3.4 only)\r"
                "\tarmasm\t\t"       "...use ARM ASM\r"
                "\tgccelf\t\t"       "...use GCC ELF toolkit (GCCSDK 4 only)\r"
+               "\tgccelfeabi\t\t"   "...use GCC EABI ELF toolkit (GCCSDK 8+ only)\r"
 	       "-float-abi\t\t"     "marks object files which float-abi is in use\r"
 	       "\tsoft\t\t"         "...soft-float, no FP instructions used\r"
 	       "\thard\t\t"         "...hard-float, FP instructions can be used\r"
@@ -1345,9 +1346,11 @@
             AsmType = AsmType_eARMASM;
          else if (strcmp (argv[i], "gccelf") == 0)
             AsmType = AsmType_eGCCELF;
+         else if (strcmp (argv[i], "gccelfeabi") == 0)
+            AsmType = AsmType_eGCCELFEABI;
          else
          {
-            fprintf (stderr, "Unknown option value %s for -asmtype (only \"objasm\", \"asasmaof\", \"asasmelf\", \"armarm\", \"gccaof\" and \"gccelf\" are known)\n", argv[i]);
+            fprintf (stderr, "Unknown option value %s for -asmtype (only \"objasm\", \"asasmaof\", \"asasmelf\", \"armarm\", \"gccaof\", \"gccelf\" and \"gccelfeabi\" are known)\n", argv[i]);
             Parse_Error = TRUE;
             goto finish;
          }
