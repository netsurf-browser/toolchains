
--- bfd/aoutx.h	2022-07-08 10:46:47.000000000 +0100
+++ bfd/aoutx.h	2025-11-07 11:25:06.389422572 +0000
@@ -1551,7 +1551,8 @@ translate_from_native_sym_flags (bfd *ab
 
 /* Set the fields of SYM_POINTER according to CACHE_PTR.  */
 
-static bool
+//static
+bool
 translate_to_native_sym_flags (bfd *abfd,
 			       asymbol *cache_ptr,
 			       struct external_nlist *sym_pointer)
@@ -2820,7 +2821,7 @@ NAME (aout, find_nearest_line) (bfd *abf
   else
     funclen = strlen (bfd_asymbol_name (func));
 
-  free (adata (abfd).line_buf);
+//    free (adata (abfd).line_buf);	// NOOOOOO! dis is used elsewhere!! e.g. objdump print_files
 
   if (filelen + funclen == 0)
     adata (abfd).line_buf = buf = NULL;

--- bfd/archures.c	2022-07-08 10:46:47.000000000 +0100
+++ bfd/archures.c	2025-11-07 11:25:06.389422572 +0000
@@ -1239,6 +1239,10 @@ bfd_default_scan (const bfd_arch_info_ty
       arch = bfd_arch_m68k;
       number = bfd_mach_m68060;
       break;
+    case 68080:
+      arch = bfd_arch_m68k;
+      number = bfd_mach_m68080;
+      break;
     case 68332:
       arch = bfd_arch_m68k;
       number = bfd_mach_cpu32;

--- bfd/bfd-in2.h	2022-08-01 08:12:32.000000000 +0100
+++ bfd/bfd-in2.h	2025-11-07 11:25:06.393422632 +0000
@@ -1404,30 +1404,31 @@ enum bfd_architecture
 #define bfd_mach_m68030                5
 #define bfd_mach_m68040                6
 #define bfd_mach_m68060                7
-#define bfd_mach_cpu32                 8
-#define bfd_mach_fido                  9
-#define bfd_mach_mcf_isa_a_nodiv       10
-#define bfd_mach_mcf_isa_a             11
-#define bfd_mach_mcf_isa_a_mac         12
-#define bfd_mach_mcf_isa_a_emac        13
-#define bfd_mach_mcf_isa_aplus         14
-#define bfd_mach_mcf_isa_aplus_mac     15
-#define bfd_mach_mcf_isa_aplus_emac    16
-#define bfd_mach_mcf_isa_b_nousp       17
-#define bfd_mach_mcf_isa_b_nousp_mac   18
-#define bfd_mach_mcf_isa_b_nousp_emac  19
-#define bfd_mach_mcf_isa_b             20
-#define bfd_mach_mcf_isa_b_mac         21
-#define bfd_mach_mcf_isa_b_emac        22
-#define bfd_mach_mcf_isa_b_float       23
-#define bfd_mach_mcf_isa_b_float_mac   24
-#define bfd_mach_mcf_isa_b_float_emac  25
-#define bfd_mach_mcf_isa_c             26
-#define bfd_mach_mcf_isa_c_mac         27
-#define bfd_mach_mcf_isa_c_emac        28
-#define bfd_mach_mcf_isa_c_nodiv       29
-#define bfd_mach_mcf_isa_c_nodiv_mac   30
-#define bfd_mach_mcf_isa_c_nodiv_emac  31
+#define bfd_mach_m68080                8
+#define bfd_mach_cpu32                 9
+#define bfd_mach_fido                  10
+#define bfd_mach_mcf_isa_a_nodiv       11
+#define bfd_mach_mcf_isa_a             12
+#define bfd_mach_mcf_isa_a_mac         13
+#define bfd_mach_mcf_isa_a_emac        14
+#define bfd_mach_mcf_isa_aplus         15
+#define bfd_mach_mcf_isa_aplus_mac     16
+#define bfd_mach_mcf_isa_aplus_emac    17
+#define bfd_mach_mcf_isa_b_nousp       18
+#define bfd_mach_mcf_isa_b_nousp_mac   19
+#define bfd_mach_mcf_isa_b_nousp_emac  20
+#define bfd_mach_mcf_isa_b             21
+#define bfd_mach_mcf_isa_b_mac         22
+#define bfd_mach_mcf_isa_b_emac        23
+#define bfd_mach_mcf_isa_b_float       24
+#define bfd_mach_mcf_isa_b_float_mac   25
+#define bfd_mach_mcf_isa_b_float_emac  26
+#define bfd_mach_mcf_isa_c             27
+#define bfd_mach_mcf_isa_c_mac         28
+#define bfd_mach_mcf_isa_c_emac        29
+#define bfd_mach_mcf_isa_c_nodiv       30
+#define bfd_mach_mcf_isa_c_nodiv_mac   31
+#define bfd_mach_mcf_isa_c_nodiv_emac  32
   bfd_arch_vax,       /* DEC Vax.  */
 
   bfd_arch_or1k,      /* OpenRISC 1000.  */
@@ -6405,6 +6406,13 @@ typedef struct bfd_symbol
   /* This section symbol should be included in the symbol table.  */
 #define BSF_SECTION_SYM_USED    (1 << 24)
 
+  /* this labels refers to code */
+#define BSF_CODE (1<<29)
+  /* this labels refers to data */
+#define BSF_DATA (1<<30)
+  /* this label was processed */
+#define BSF_VISITED (1<<31)
+
   flagword flags;
 
   /* A pointer to the section to which this symbol is
@@ -6791,6 +6799,8 @@ struct bfd
       struct bfd_pef_data_struct *pef_data;
       struct bfd_pef_xlib_data_struct *pef_xlib_data;
       struct bfd_sym_data_struct *sym_data;
+      struct amiga_data_struct *amiga_data;
+      struct amiga_ardata_struct *amiga_ar_data;
       void *any;
     }
   tdata;
@@ -7300,6 +7310,7 @@ enum bfd_flavour
 {
   /* N.B. Update bfd_flavour_name if you change this.  */
   bfd_target_unknown_flavour,
+  bfd_target_amiga_flavour,
   bfd_target_aout_flavour,
   bfd_target_coff_flavour,
   bfd_target_ecoff_flavour,

--- bfd/config.bfd	2022-07-08 10:46:47.000000000 +0100
+++ bfd/config.bfd	2025-11-07 11:25:06.409422872 +0000
@@ -826,6 +826,11 @@ case "${targ}" in
     targ_selvecs="m68hc11_elf32_vec m68hc12_elf32_vec"
     ;;
 
+  m68*-*-amigaos*)
+    targ_defvec=amiga_vec
+    targ_selvecs="aout_amiga_vec amiga_vec"
+    targ_underscore=yes
+    ;;   
   m68*-*-*)
     targ_defvec=m68k_elf32_vec
     ;;

--- bfd/configure	2022-08-05 10:54:00.000000000 +0100
+++ bfd/configure	2025-11-07 11:25:06.417422992 +0000
@@ -1,6 +1,6 @@
 #! /bin/sh
 # Guess values for system-dependent variables and create Makefiles.
-# Generated by GNU Autoconf 2.69 for bfd 2.39.
+# Generated by GNU Autoconf 2.69 for bfd 2.39.0.
 #
 #
 # Copyright (C) 1992-1996, 1998-2012 Free Software Foundation, Inc.
@@ -587,8 +587,8 @@ MAKEFLAGS=
 # Identity of this package.
 PACKAGE_NAME='bfd'
 PACKAGE_TARNAME='bfd'
-PACKAGE_VERSION='2.39'
-PACKAGE_STRING='bfd 2.39'
+PACKAGE_VERSION='2.39.0'
+PACKAGE_STRING='bfd 2.39.0'
 PACKAGE_BUGREPORT=''
 PACKAGE_URL=''
 
@@ -1390,7 +1390,7 @@ if test "$ac_init_help" = "long"; then
   # Omit some internal or obsolete options to make the list less imposing.
   # This message is too long to be a string in the A/UX 3.1 sh.
   cat <<_ACEOF
-\`configure' configures bfd 2.39 to adapt to many kinds of systems.
+\`configure' configures bfd 2.39.0 to adapt to many kinds of systems.
 
 Usage: $0 [OPTION]... [VAR=VALUE]...
 
@@ -1461,7 +1461,7 @@ fi
 
 if test -n "$ac_init_help"; then
   case $ac_init_help in
-     short | recursive ) echo "Configuration of bfd 2.39:";;
+     short | recursive ) echo "Configuration of bfd 2.39.0:";;
    esac
   cat <<\_ACEOF
 
@@ -1589,7 +1589,7 @@ fi
 test -n "$ac_init_help" && exit $ac_status
 if $ac_init_version; then
   cat <<\_ACEOF
-bfd configure 2.39
+bfd configure 2.39.0
 generated by GNU Autoconf 2.69
 
 Copyright (C) 2012 Free Software Foundation, Inc.
@@ -2183,7 +2183,7 @@ cat >config.log <<_ACEOF
 This file contains any messages produced by compilers while
 running configure, to aid debugging if configure makes a mistake.
 
-It was created by bfd $as_me 2.39, which was
+It was created by bfd $as_me 2.39.0, which was
 generated by GNU Autoconf 2.69.  Invocation command line was
 
   $ $0 $@
@@ -3165,7 +3165,7 @@ fi
 
 # Define the identity of the package.
  PACKAGE='bfd'
- VERSION='2.39'
+ VERSION='2.39.0'
 
 
 cat >>confdefs.h <<_ACEOF
@@ -13456,6 +13456,8 @@ do
     m32r_elf32_linux_le_vec)	 tb="$tb elf32-m32r.lo elf32.lo $elf" ;;
     m68hc11_elf32_vec)		 tb="$tb elf32-m68hc11.lo elf32-m68hc1x.lo elf32.lo $elf" ;;
     m68hc12_elf32_vec)		 tb="$tb elf32-m68hc12.lo elf32-m68hc1x.lo elf32.lo $elf" ;;
+    amiga_vec)			tb="$tb amigaos.lo amigaoslink.lo" ;;
+    aout_amiga_vec)		tb="$tb aout-amiga.lo aout32.lo";;
     m68k_elf32_vec)		 tb="$tb elf32-m68k.lo elf32.lo $elf" ;;
     s12z_elf32_vec)		 tb="$tb elf32-s12z.lo elf32.lo $elf" ;;
     mach_o_be_vec)		 tb="$tb mach-o.lo dwarf2.lo" ;;
@@ -15646,7 +15648,7 @@ cat >>$CONFIG_STATUS <<\_ACEOF || ac_wri
 # report actual input values of CONFIG_FILES etc. instead of their
 # values after options handling.
 ac_log="
-This file was extended by bfd $as_me 2.39, which was
+This file was extended by bfd $as_me 2.39.0, which was
 generated by GNU Autoconf 2.69.  Invocation command line was
 
   CONFIG_FILES    = $CONFIG_FILES
@@ -15712,7 +15714,7 @@ _ACEOF
 cat >>$CONFIG_STATUS <<_ACEOF || ac_write_fail=1
 ac_cs_config="`$as_echo "$ac_configure_args" | sed 's/^ //; s/[\\""\`\$]/\\\\&/g'`"
 ac_cs_version="\\
-bfd config.status 2.39
+bfd config.status 2.39.0
 configured by $0, generated by GNU Autoconf 2.69,
   with options \\"\$ac_cs_config\\"
 

--- bfd/configure.ac	2022-08-01 08:12:32.000000000 +0100
+++ bfd/configure.ac	2025-11-07 11:25:06.417422992 +0000
@@ -513,6 +513,8 @@ do
     m32r_elf32_linux_le_vec)	 tb="$tb elf32-m32r.lo elf32.lo $elf" ;;
     m68hc11_elf32_vec)		 tb="$tb elf32-m68hc11.lo elf32-m68hc1x.lo elf32.lo $elf" ;;
     m68hc12_elf32_vec)		 tb="$tb elf32-m68hc12.lo elf32-m68hc1x.lo elf32.lo $elf" ;;
+    amiga_vec)			tb="$tb amigaos.lo amigaoslink.lo" ;;
+    aout_amiga_vec)		tb="$tb aout-amiga.lo aout32.lo";;
     m68k_elf32_vec)		 tb="$tb elf32-m68k.lo elf32.lo $elf" ;;
     s12z_elf32_vec)		 tb="$tb elf32-s12z.lo elf32.lo $elf" ;;
     mach_o_be_vec)		 tb="$tb mach-o.lo dwarf2.lo" ;;

--- bfd/cpu-m68k.c	2022-07-08 10:46:47.000000000 +0100
+++ bfd/cpu-m68k.c	2025-11-07 11:25:06.421423052 +0000
@@ -31,7 +31,7 @@ bfd_m68k_compatible (const bfd_arch_info
 
 #define N(name, print,d,next)  \
 {  32, 32, 8, bfd_arch_m68k, name, "m68k",print,2,d,bfd_m68k_compatible, \
-   bfd_default_scan, bfd_arch_default_fill, next, 0 }
+   bfd_default_scan, bfd_arch_default_fill, next, 8 }
 
 static const bfd_arch_info_type arch_info_struct[] =
   {
@@ -103,7 +103,8 @@ static const bfd_arch_info_type arch_inf
       false, &arch_info_struct[39]),
     N(bfd_mach_mcf_isa_b_float_emac, "m68k:548x",
       false, &arch_info_struct[40]),
-    N(bfd_mach_mcf_isa_b_float_emac, "m68k:cfv4e", false, 0),
+    N(bfd_mach_mcf_isa_b_float_emac, "m68k:cfv4e", false, &arch_info_struct[41]),
+    N(bfd_mach_m68080,  "m68k:68080", false, 0),    
   };
 
 const bfd_arch_info_type bfd_m68k_arch =
@@ -121,6 +122,7 @@ static const unsigned m68k_arch_features
   m68030|m68881|m68851,
   m68040|m68881|m68851,
   m68060|m68881|m68851,
+  m68080|m68881,
   cpu32|m68881,
   fido_a|m68881,
   mcfisa_a,

--- bfd/development.sh	2022-08-05 10:50:00.000000000 +0100
+++ bfd/development.sh	2025-11-07 11:25:06.425423112 +0000
@@ -16,7 +16,7 @@
 # along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 # Controls whether to enable development-mode features by default.
-development=false
+development=true
 
 # Indicate whether this is a release branch.
 experimental=false

























--- bfd/elf32-i386.c	2022-07-26 08:13:10.000000000 +0100
+++ bfd/elf32-i386.c	2025-11-07 11:25:06.465423712 +0000
@@ -1788,6 +1788,23 @@ elf_i386_scan_relocs (bfd *abfd,
 		    h->pointer_equality_needed = 1;
 		}
 
+	      if (h->pointer_equality_needed
+		  && h->type == STT_FUNC
+		  && eh->def_protected
+		  && elf_has_indirect_extern_access (h->root.u.def.section->owner))
+		{
+		  /* Disallow non-canonical reference to canonical
+		     protected function.  */
+		  _bfd_error_handler
+		    /* xgettext:c-format */
+		    (_("%pB: non-canonical reference to canonical "
+		       "protected function `%s' in %pB"),
+		     abfd, h->root.root.string,
+		     h->root.u.def.section->owner);
+		  bfd_set_error (bfd_error_bad_value);
+		  goto error_return;
+		}
+
 	      if (!func_pointer_ref)
 		{
 		  /* If this reloc is in a read-only section, we might

--- bfd/elf64-x86-64.c	2022-07-26 08:13:10.000000000 +0100
+++ bfd/elf64-x86-64.c	2025-11-07 11:25:06.525424611 +0000
@@ -2231,6 +2231,23 @@ elf_x86_64_scan_relocs (bfd *abfd, struc
 		    h->pointer_equality_needed = 1;
 		}
 
+	      if (h->pointer_equality_needed
+		  && h->type == STT_FUNC
+		  && eh->def_protected
+		  && elf_has_indirect_extern_access (h->root.u.def.section->owner))
+		{
+		  /* Disallow non-canonical reference to canonical
+		     protected function.  */
+		  _bfd_error_handler
+		    /* xgettext:c-format */
+		    (_("%pB: non-canonical reference to canonical "
+		       "protected function `%s' in %pB"),
+		     abfd, h->root.root.string,
+		     h->root.u.def.section->owner);
+		  bfd_set_error (bfd_error_bad_value);
+		  goto error_return;
+		}
+
 	      if (!func_pointer_ref)
 		{
 		  /* If this reloc is in a read-only section, we might

--- bfd/elf.c	2022-07-08 10:46:47.000000000 +0100
+++ bfd/elf.c	2025-11-07 11:25:06.445423412 +0000
@@ -5438,8 +5438,6 @@ assign_file_positions_for_load_sections
   Elf_Internal_Phdr *p;
   file_ptr off;  /* Octets.  */
   bfd_size_type maxpagesize;
-  bfd_size_type p_align;
-  bool p_align_p = false;
   unsigned int alloc, actual;
   unsigned int i, j;
   struct elf_segment_map **sorted_seg_map;
@@ -5524,7 +5522,6 @@ assign_file_positions_for_load_sections
     qsort (sorted_seg_map, alloc, sizeof (*sorted_seg_map),
 	   elf_sort_segments);
 
-  p_align = bed->p_align;
   maxpagesize = 1;
   if ((abfd->flags & D_PAGED) != 0)
     {
@@ -5559,6 +5556,8 @@ assign_file_positions_for_load_sections
       asection **secpp;
       bfd_vma off_adjust;  /* Octets.  */
       bool no_contents;
+      bfd_size_type p_align;
+      bool p_align_p;
 
       /* An ELF segment (described by Elf_Internal_Phdr) may contain a
 	 number of sections with contents contributing to both p_filesz
@@ -5569,6 +5568,8 @@ assign_file_positions_for_load_sections
       p = phdrs + m->idx;
       p->p_type = m->p_type;
       p->p_flags = m->p_flags;
+      p_align = bed->p_align;
+      p_align_p = false;
 
       if (m->count == 0)
 	p->p_vaddr = m->p_vaddr_offset * opb;

--- bfd/elfnn-loongarch.c	2022-07-08 10:46:47.000000000 +0100
+++ bfd/elfnn-loongarch.c	2025-11-07 11:25:06.537424791 +0000
@@ -2341,9 +2341,10 @@ loongarch_elf_relocate_section (bfd *out
 	case R_LARCH_SOP_PUSH_PLT_PCREL:
 	  unresolved_reloc = false;
 
-	  if (resolved_to_const)
+	  if (!is_undefweak && resolved_to_const)
 	    {
 	      relocation += rel->r_addend;
+	      relocation -= pc;
 	      break;
 	    }
 	  else if (is_undefweak)


--- bfd/libbfd.h	2022-07-08 10:46:47.000000000 +0100
+++ bfd/libbfd.h	2025-11-07 11:25:06.557425091 +0000
@@ -825,7 +825,7 @@ extern FILE *_bfd_real_fopen
 /* List of supported target vectors, and the default vector (if
    bfd_default_vector[0] is NULL, there is no default).  */
 extern const bfd_target *const *const bfd_target_vector ATTRIBUTE_HIDDEN;
-extern const bfd_target *bfd_default_vector[] ATTRIBUTE_HIDDEN;
+extern bfd_target *bfd_default_vector[] ATTRIBUTE_HIDDEN;
 
 /* List of associated target vectors.  */
 extern const bfd_target *const *const bfd_associated_vector ATTRIBUTE_HIDDEN;
@@ -1005,7 +1005,7 @@ struct bfd_iovec
                   int prot, int flags, file_ptr offset,
                   void **map_addr, bfd_size_type *map_len);
 };
-extern const struct bfd_iovec _bfd_memory_iovec;
+extern BFDDECL const struct bfd_iovec _bfd_memory_iovec;
 /* Extracted from bfdwin.c.  */
 struct _bfd_window_internal {
   struct _bfd_window_internal *next;
@@ -3499,7 +3499,7 @@ bool _bfd_unrecognized_reloc
     unsigned int r_type);
 
 /* Extracted from archures.c.  */
-extern const bfd_arch_info_type bfd_default_arch_struct;
+extern BFDDECL const bfd_arch_info_type bfd_default_arch_struct;
 const bfd_arch_info_type *bfd_default_compatible
    (const bfd_arch_info_type *a, const bfd_arch_info_type *b);
 

--- bfd/libbfd-in.h	2022-07-08 10:46:47.000000000 +0100
+++ bfd/libbfd-in.h	2025-11-07 11:25:06.553425031 +0000
@@ -820,7 +820,7 @@ extern FILE *_bfd_real_fopen
 /* List of supported target vectors, and the default vector (if
    bfd_default_vector[0] is NULL, there is no default).  */
 extern const bfd_target *const *const bfd_target_vector ATTRIBUTE_HIDDEN;
-extern const bfd_target *bfd_default_vector[] ATTRIBUTE_HIDDEN;
+extern bfd_target * bfd_default_vector[] ATTRIBUTE_HIDDEN;
 
 /* List of associated target vectors.  */
 extern const bfd_target *const *const bfd_associated_vector ATTRIBUTE_HIDDEN;

--- bfd/linker.c	2022-07-08 10:46:47.000000000 +0100
+++ bfd/linker.c	2025-11-07 11:25:06.557425091 +0000
@@ -413,7 +413,7 @@ static bool generic_add_output_symbol
   (bfd *, size_t *psymalloc, asymbol *);
 static bool default_data_link_order
   (bfd *, struct bfd_link_info *, asection *, struct bfd_link_order *);
-static bool default_indirect_link_order
+bool default_indirect_link_order
   (bfd *, struct bfd_link_info *, asection *, struct bfd_link_order *,
    bool);
 
@@ -467,6 +467,7 @@ _bfd_link_hash_table_init
   bool ret;
 
   BFD_ASSERT (!abfd->is_linker_output && !abfd->link.hash);
+  table->creator = abfd->xvec;
   table->undefs = NULL;
   table->undefs_tail = NULL;
   table->type = bfd_link_generic_hash_table;
@@ -1114,6 +1115,13 @@ generic_link_check_archive_element (bfd
 	  h->u.c.size = size;
 
 	  power = bfd_log2 (size);
+	  /* For the amiga, we don't want an alignment bigger than 2**2.
+	     Doing this here is horrible kludgy, but IMHO the maximal
+	     power alignment really should be target-dependant so that
+	     we wouldn't have to do this -- daniel */
+	  if (info->hash->creator->flavour == bfd_target_amiga_flavour
+	      && power > 2)
+	    power = 2;
 	  if (power > 4)
 	    power = 4;
 	  h->u.c.p->alignment_power = power;
@@ -1586,6 +1594,13 @@ _bfd_generic_link_add_one_symbol (struct
 	    unsigned int power;
 
 	    power = bfd_log2 (value);
+	    /* For the amiga, we don't want an alignment bigger than 2**2.
+	       Doing this here is horrible kludgy, but IMHO the maximal
+	       power alignment really should be target-dependant so that
+	       we wouldn't have to do this -- daniel */
+	    if (info->hash->creator->flavour == bfd_target_amiga_flavour
+		&& power > 2)
+	      power = 2;
 	    if (power > 4)
 	      power = 4;
 	    h->u.c.p->alignment_power = power;
@@ -1640,6 +1655,13 @@ _bfd_generic_link_add_one_symbol (struct
 	      /* Select a default alignment based on the size.  This may
 		 be overridden by the caller.  */
 	      power = bfd_log2 (value);
+	      /* For the amiga, we don't want an alignment bigger than 2**2.
+		 Doing this here is horrible kludgy, but IMHO the maximal
+		 power alignment really should be target-dependant so that
+		 we wouldn't have to do this -- daniel */
+	      if (info->hash->creator->flavour == bfd_target_amiga_flavour
+		  && power > 2)
+		power = 2;
 	      if (power > 4)
 		power = 4;
 	      h->u.c.p->alignment_power = power;
@@ -2542,7 +2564,7 @@ default_data_link_order (bfd *abfd,
 
 /* Default routine to handle a bfd_indirect_link_order.  */
 
-static bool
+bool
 default_indirect_link_order (bfd *output_bfd,
 			     struct bfd_link_info *info,
 			     asection *output_section,
@@ -2932,6 +2954,8 @@ _bfd_generic_section_already_linked (bfd
   struct bfd_section_already_linked *l;
   struct bfd_section_already_linked_hash_entry *already_linked_list;
 
+  static bfd * last_abfd;
+
   if ((sec->flags & SEC_LINK_ONCE) == 0)
     return false;
 
@@ -2954,6 +2978,24 @@ _bfd_generic_section_already_linked (bfd
 
   name = bfd_section_name (sec);
 
+  // amigaos uses this outside of linking
+  // ensure it get freed if objdumping archives!
+  if (last_abfd)
+    {
+      last_abfd = NULL;
+      if (_bfd_section_already_linked_table.size)
+	{
+	  bfd_section_already_linked_table_free();
+	  _bfd_section_already_linked_table.size = 0;
+	}
+    }
+  // open only once
+  if (!_bfd_section_already_linked_table.size)
+    {
+      bfd_section_already_linked_table_init();
+      last_abfd = abfd;
+    }
+
   already_linked_list = bfd_section_already_linked_table_lookup (name);
 
   l = already_linked_list->entry;

--- bfd/Makefile.am	2022-07-08 10:46:47.000000000 +0100
+++ bfd/Makefile.am	2025-11-07 11:25:06.381422452 +0000
@@ -263,6 +263,9 @@ ALL_MACHINES_CFILES = \
 # The .o files needed by all of the 32 bit vectors that are configured into
 # target_vector in targets.c if configured with --enable-targets=all.
 BFD32_BACKENDS = \
+	amigaos.lo \
+	amigaoslink.lo \
+	aout-amiga.lo \
 	aout-cris.lo \
 	aout-ns32k.lo \
 	aout32.lo \
@@ -395,6 +398,9 @@ BFD32_BACKENDS = \
 	xtensa-modules.lo
 
 BFD32_BACKENDS_CFILES = \
+	amigaos.c \
+	amigaoslink.c \
+	aout-amiga.c \
 	aout-cris.c \
 	aout-ns32k.c \
 	aout32.c \

--- bfd/Makefile.in	2022-08-05 10:53:59.000000000 +0100
+++ bfd/Makefile.in	2025-11-07 11:25:06.385422512 +0000
@@ -726,6 +726,9 @@ ALL_MACHINES_CFILES = \
 # The .o files needed by all of the 32 bit vectors that are configured into
 # target_vector in targets.c if configured with --enable-targets=all.
 BFD32_BACKENDS = \
+	amigaos.lo \
+	amigaoslink.lo \
+	aout-amiga.lo \
 	aout-cris.lo \
 	aout-ns32k.lo \
 	aout32.lo \
@@ -858,6 +861,9 @@ BFD32_BACKENDS = \
 	xtensa-modules.lo
 
 BFD32_BACKENDS_CFILES = \
+	amigaos.c \
+	amigaoslink.c \
+	aout-amiga.c \
 	aout-cris.c \
 	aout-ns32k.c \
 	aout32.c \
@@ -1436,6 +1442,9 @@ distclean-compile:
 
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/aix386-core.Plo@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/aix5ppc-core.Plo@am__quote@
+@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/amigaos.Plo@am__quote@
+@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/amigaoslink.Plo@am__quote@
+@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/aout-amiga.Plo@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/aout-cris.Plo@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/aout-ns32k.Plo@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/aout32.Plo@am__quote@

--- bfd/opncls.c	2022-07-08 10:46:47.000000000 +0100
+++ bfd/opncls.c	2025-11-07 11:25:06.565425210 +0000
@@ -131,7 +131,9 @@ _bfd_delete_bfd (bfd *abfd)
   else
     free ((char *) bfd_get_filename (abfd));
 
-  free (abfd->arelt_data);
+// CHECK
+//    free ((char *) abfd->filename);
+//  free (abfd->arelt_data);
   free (abfd);
 }
 

--- bfd/po/bfd.pot	2022-08-05 11:01:55.000000000 +0100
+++ bfd/po/bfd.pot	2025-11-07 11:25:06.573425330 +0000
@@ -8,7 +8,7 @@ msgid ""
 msgstr ""
 "Project-Id-Version: PACKAGE VERSION\n"
 "Report-Msgid-Bugs-To: https://sourceware.org/bugzilla/\n"
-"POT-Creation-Date: 2022-07-08 10:56+0100\n"
+"POT-Creation-Date: 2022-08-05 13:59+0100\n"
 "PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\n"
 "Last-Translator: FULL NAME <EMAIL@ADDRESS>\n"
 "Language-Team: LANGUAGE <LL@li.org>\n"
@@ -642,108 +642,108 @@ msgstr ""
 msgid "DWARF error: info pointer extends beyond end of attributes"
 msgstr ""
 
-#: dwarf2.c:1643
+#: dwarf2.c:1647
 #, c-format
 msgid "DWARF error: invalid or unhandled FORM value: %#x"
 msgstr ""
 
-#: dwarf2.c:1959
+#: dwarf2.c:1963
 msgid "DWARF error: mangled line number section (bad file number)"
 msgstr ""
 
-#: dwarf2.c:2497
+#: dwarf2.c:2501
 msgid "DWARF error: zero format count"
 msgstr ""
 
-#: dwarf2.c:2507
+#: dwarf2.c:2511
 #, c-format
 msgid "DWARF error: data count (%<PRIx64>) larger than buffer size"
 msgstr ""
 
-#: dwarf2.c:2546
+#: dwarf2.c:2550
 #, c-format
 msgid "DWARF error: unknown format content type %<PRIu64>"
 msgstr ""
 
-#: dwarf2.c:2621
+#: dwarf2.c:2625
 #, c-format
 msgid "DWARF error: line info section is too small (%<PRId64>)"
 msgstr ""
 
-#: dwarf2.c:2648
+#: dwarf2.c:2652
 #, c-format
 msgid ""
 "DWARF error: line info data is bigger (%#<PRIx64>) than the space remaining "
 "in the section (%#lx)"
 msgstr ""
 
-#: dwarf2.c:2661
+#: dwarf2.c:2665
 #, c-format
 msgid "DWARF error: unhandled .debug_line version %d"
 msgstr ""
 
-#: dwarf2.c:2670
+#: dwarf2.c:2674
 msgid "DWARF error: ran out of room reading prologue"
 msgstr ""
 
-#: dwarf2.c:2686
+#: dwarf2.c:2690
 #, c-format
 msgid "DWARF error: line info unsupported segment selector size %u"
 msgstr ""
 
-#: dwarf2.c:2708
+#: dwarf2.c:2712
 msgid "DWARF error: invalid maximum operations per instruction"
 msgstr ""
 
-#: dwarf2.c:2720
+#: dwarf2.c:2724
 msgid "DWARF error: ran out of room reading opcodes"
 msgstr ""
 
-#: dwarf2.c:2888
+#: dwarf2.c:2892
 msgid "DWARF error: mangled line number section"
 msgstr ""
 
-#: dwarf2.c:3375
+#: dwarf2.c:3379
 msgid "DWARF error: abstract instance recursion detected"
 msgstr ""
 
-#: dwarf2.c:3409 dwarf2.c:3503
+#: dwarf2.c:3413 dwarf2.c:3507
 msgid "DWARF error: invalid abstract instance DIE ref"
 msgstr ""
 
-#: dwarf2.c:3425
+#: dwarf2.c:3429
 #, c-format
 msgid "DWARF error: unable to read alt ref %<PRIu64>"
 msgstr ""
 
-#: dwarf2.c:3481
+#: dwarf2.c:3485
 #, c-format
 msgid "DWARF error: unable to locate abstract instance DIE ref %<PRIu64>"
 msgstr ""
 
-#: dwarf2.c:3518 dwarf2.c:3832 dwarf2.c:4394
+#: dwarf2.c:3522 dwarf2.c:3836 dwarf2.c:4398
 #, c-format
 msgid "DWARF error: could not find abbrev number %u"
 msgstr ""
 
-#: dwarf2.c:4098
+#: dwarf2.c:4102
 #, c-format
 msgid "DWARF error: could not find variable specification at offset 0x%lx"
 msgstr ""
 
-#: dwarf2.c:4250 dwarf2.c:4494
+#: dwarf2.c:4254 dwarf2.c:4498
 msgid ""
 "DWARF error: DW_AT_comp_dir attribute encountered with a non-string form"
 msgstr ""
 
-#: dwarf2.c:4321
+#: dwarf2.c:4325
 #, c-format
 msgid ""
 "DWARF error: found dwarf version '%u', this reader only handles version 2, "
 "3, 4 and 5 information"
 msgstr ""
 
-#: dwarf2.c:4358
+#: dwarf2.c:4362
 #, c-format
 msgid ""
 "DWARF error: found address size '%u', this reader can not handle sizes "
@@ -959,9 +959,9 @@ msgstr ""
 msgid "%pB: %s' accessed both as normal and thread local symbol"
 msgstr ""
 
-#: elf-m10300.c:2093 elf32-arm.c:13389 elf32-i386.c:3452 elf32-m32r.c:2331
+#: elf-m10300.c:2093 elf32-arm.c:13389 elf32-i386.c:3458 elf32-m32r.c:2331
 #: elf32-m68k.c:3929 elf32-s390.c:3080 elf32-sh.c:3671 elf32-tilepro.c:3275
-#: elf32-xtensa.c:3034 elf64-s390.c:3042 elf64-x86-64.c:4109 elfxx-sparc.c:2917
+#: elf32-xtensa.c:3034 elf64-s390.c:3042 elf64-x86-64.c:4107 elfxx-sparc.c:2917
 #: elfxx-sparc.c:3814 elfxx-tilegx.c:3665 elfnn-aarch64.c:5515
 #: elfnn-aarch64.c:7118
 #, c-format
@@ -1053,11 +1053,11 @@ msgstr ""
 msgid "Removed property %W to merge %pB (not found) and %pB (0x%v)\n"
 msgstr ""
 
-#: elf-properties.c:668 elfxx-aarch64.c:758 elfxx-x86.c:3738
+#: elf-properties.c:668 elfxx-aarch64.c:758 elfxx-x86.c:3755
 msgid "%F%P: failed to create GNU property section\n"
 msgstr ""
 
-#: elf-properties.c:672 elfxx-aarch64.c:762 elfxx-x86.c:3743
+#: elf-properties.c:672 elfxx-aarch64.c:762 elfxx-x86.c:3760
 #, c-format
 msgid "%F%pA: failed to align section\n"
 msgstr ""
@@ -2118,7 +2118,7 @@ msgstr ""
 
 #: elf32-arm.c:15353 elf32-arm.c:15487 elf32-i386.c:1511 elf32-s390.c:923
 #: elf32-tic6x.c:2724 elf32-tilepro.c:1435 elf32-xtensa.c:1102 elf64-s390.c:845
-#: elf64-x86-64.c:1882 elfxx-sparc.c:1386 elfxx-tilegx.c:1662 elfxx-x86.c:971
+#: elf64-x86-64.c:1882 elfxx-sparc.c:1386 elfxx-tilegx.c:1662 elfxx-x86.c:970
 #: elfnn-aarch64.c:7622 elfnn-loongarch.c:618 elfnn-riscv.c:660
 #, c-format
 msgid "%pB: bad symbol index: %d"
@@ -2258,8 +2258,8 @@ msgstr ""
 msgid "%pB(%pA+%#<PRIx64>): unresolvable relocation against symbol `%s'"
 msgstr ""
 
-#: elf32-bfin.c:1621 elf32-i386.c:3492 elf32-m68k.c:4006 elf32-s390.c:3138
-#: elf64-s390.c:3100 elf64-x86-64.c:4164
+#: elf32-bfin.c:1621 elf32-i386.c:3498 elf32-m68k.c:4006 elf32-s390.c:3138
+#: elf64-s390.c:3100 elf64-x86-64.c:4162
 #, c-format
 msgid "%pB(%pA+%#<PRIx64>): reloc against `%s': error %d"
 msgstr ""
@@ -2724,62 +2724,62 @@ msgstr ""
 msgid "%pB: unsupported non-PIC call to IFUNC `%s'"
 msgstr ""
 
-#: elf32-i386.c:1820 elf64-x86-64.c:2263
+#: elf32-i386.c:1821 elf64-x86-64.c:2264
 #, c-format
 msgid ""
 "%pB: non-canonical reference to canonical protected function `%s' in %pB"
 msgstr ""
 
-#: elf32-i386.c:2383 elf64-x86-64.c:2726 elfnn-riscv.c:2300
+#: elf32-i386.c:2384 elf64-x86-64.c:2727 elfnn-riscv.c:2300
 #, c-format
 msgid "%pB: relocation %s against STT_GNU_IFUNC symbol `%s' isn't supported"
 msgstr ""
 
-#: elf32-i386.c:2416 elf32-i386.c:3703 elf32-i386.c:3851 elf64-x86-64.c:2783
-#: elf64-x86-64.c:4337 elf64-x86-64.c:4500 elfnn-riscv.c:2173
+#: elf32-i386.c:2417 elf32-i386.c:3709 elf32-i386.c:3857 elf64-x86-64.c:2784
+#: elf64-x86-64.c:4335 elf64-x86-64.c:4498 elfnn-riscv.c:2173
 #: elfnn-riscv.c:2972 elfnn-riscv.c:3046
 #, c-format
 msgid "Local IFUNC function `%s' in %pB\n"
 msgstr ""
 
-#: elf32-i386.c:2589
+#: elf32-i386.c:2595
 #, c-format
 msgid ""
 "%pB: direct GOT relocation %s against `%s' without base register can not be "
 "used when making a shared object"
 msgstr ""
 
-#: elf32-i386.c:2624 elf64-x86-64.c:2999
+#: elf32-i386.c:2630 elf64-x86-64.c:3000
 msgid "hidden symbol"
 msgstr ""
 
-#: elf32-i386.c:2627 elf64-x86-64.c:3002
+#: elf32-i386.c:2633 elf64-x86-64.c:3003
 msgid "internal symbol"
 msgstr ""
 
-#: elf32-i386.c:2630 elf64-x86-64.c:3005
+#: elf32-i386.c:2636 elf64-x86-64.c:3006
 msgid "protected symbol"
 msgstr ""
 
-#: elf32-i386.c:2633 elf64-x86-64.c:3008
+#: elf32-i386.c:2639 elf64-x86-64.c:3009
 msgid "symbol"
 msgstr ""
 
-#: elf32-i386.c:2639
+#: elf32-i386.c:2645
 #, c-format
 msgid ""
 "%pB: relocation R_386_GOTOFF against undefined %s `%s' can not be used when "
 "making a shared object"
 msgstr ""
 
-#: elf32-i386.c:2652
+#: elf32-i386.c:2658
 #, c-format
 msgid ""
 "%pB: relocation R_386_GOTOFF against protected %s `%s' can not be used when "
 "making a shared object"
 msgstr ""
 
-#: elf32-i386.c:4054 elf64-x86-64.c:4711
+#: elf32-i386.c:4060 elf64-x86-64.c:4709
 msgid "%F%P: discarded output section: `%pA'\n"
 msgstr ""
 
@@ -3992,7 +3992,7 @@ msgstr ""
 msgid "warning: %pB and %pB differ in whether code is compiled for DSBT"
 msgstr ""
 
-#: elf32-tilepro.c:3627 elfxx-tilegx.c:4017 elfxx-x86.c:2431
+#: elf32-tilepro.c:3627 elfxx-tilegx.c:4017 elfxx-x86.c:2430
 #: elfnn-aarch64.c:9741 elfnn-loongarch.c:3148 elfnn-riscv.c:3269
 #, c-format
 msgid "discarded output section: `%pA'"
@@ -4923,21 +4923,21 @@ msgstr ""
 msgid "%pB: '%s' accessed both as normal and thread local symbol"
 msgstr ""
 
-#: elf64-x86-64.c:2752 elfnn-aarch64.c:5556 elfnn-riscv.c:2140
+#: elf64-x86-64.c:2753 elfnn-aarch64.c:5556 elfnn-riscv.c:2140
 #, c-format
 msgid ""
 "%pB: relocation %s against STT_GNU_IFUNC symbol `%s' has non-zero addend: "
 "%<PRId64>"
 msgstr ""
 
-#: elf64-x86-64.c:3014
+#: elf64-x86-64.c:3015
 #, c-format
 msgid ""
 "%pB: relocation R_X86_64_GOTOFF64 against undefined %s `%s' can not be used "
 "when making a shared object"
 msgstr ""
 
-#: elf64-x86-64.c:3028
+#: elf64-x86-64.c:3029
 #, c-format
 msgid ""
 "%pB: relocation R_X86_64_GOTOFF64 against protected %s `%s' can not be used "
@@ -4955,23 +4955,23 @@ msgstr ""
 msgid "%F%P: corrupt input: %pB\n"
 msgstr ""
 
-#: elf64-x86-64.c:4150
+#: elf64-x86-64.c:4148
 #, c-format
 msgid ""
 " failed to convert GOTPCREL relocation against '%s'; relink with --no-relax\n"
 msgstr ""
 
-#: elf64-x86-64.c:4311
+#: elf64-x86-64.c:4309
 #, c-format
 msgid "%F%pB: PC-relative offset overflow in PLT entry for `%s'\n"
 msgstr ""
 
-#: elf64-x86-64.c:4379
+#: elf64-x86-64.c:4377
 #, c-format
 msgid "%F%pB: branch displacement overflow in PLT entry for `%s'\n"
 msgstr ""
 
-#: elf64-x86-64.c:4432
+#: elf64-x86-64.c:4430
 #, c-format
 msgid "%F%pB: PC-relative offset overflow in GOT PLT entry for `%s'\n"
 msgstr ""
@@ -5900,132 +5900,132 @@ msgstr ""
 msgid "%pB: cannot link together %s and %s objects"
 msgstr ""
 
-#: elfxx-x86.c:537
+#: elfxx-x86.c:536 elfxx-x86.c:3053
 #, c-format
 msgid ""
 "%F%P: %pB: copy relocation against non-copyable protected symbol `%s' in "
 "%pB\n"
 msgstr ""
 
-#: elfxx-x86.c:1044
+#: elfxx-x86.c:1043
 #, c-format
 msgid "%F%P: %pB: failed to allocate relative reloc record\n"
 msgstr ""
 
-#: elfxx-x86.c:1386
+#: elfxx-x86.c:1385
 #, c-format
 msgid "%F%P: %pB: failed to allocate 64-bit DT_RELR bitmap\n"
 msgstr ""
 
-#: elfxx-x86.c:1422
+#: elfxx-x86.c:1421
 #, c-format
 msgid "%F%P: %pB: failed to allocate 32-bit DT_RELR bitmap\n"
 msgstr ""
 
-#: elfxx-x86.c:1727
+#: elfxx-x86.c:1726
 #, c-format
 msgid ""
 "%F%P: %pB: size of compact relative reloc section is changed: new (%lu) != "
 "old (%lu)\n"
 msgstr ""
 
-#: elfxx-x86.c:1749
+#: elfxx-x86.c:1748
 #, c-format
 msgid "%F%P: %pB: failed to allocate compact relative reloc section\n"
 msgstr ""
 
-#: elfxx-x86.c:2009
+#: elfxx-x86.c:2008
 #, c-format
 msgid ""
 "%F%P: %pB: relocation %s against absolute symbol `%s' in section `%pA' is "
 "disallowed\n"
 msgstr ""
 
-#: elfxx-x86.c:2091
+#: elfxx-x86.c:2090
 msgid "%P: %pB: warning: relocation in read-only section `%pA'\n"
 msgstr ""
 
-#: elfxx-x86.c:2807
+#: elfxx-x86.c:2806
 msgid ""
 "%pB: %s (offset: 0x%v, info: 0x%v, addend: 0x%v) against '%s' for section "
 "'%pA' in %pB\n"
 msgstr ""
 
-#: elfxx-x86.c:2813
+#: elfxx-x86.c:2812
 msgid ""
 "%pB: %s (offset: 0x%v, info: 0x%v) against '%s' for section '%pA' in %pB\n"
 msgstr ""
 
-#: elfxx-x86.c:3410
+#: elfxx-x86.c:3427
 #, c-format
 msgid "error: %pB: <corrupt x86 property (0x%x) size: 0x%x>"
 msgstr ""
 
-#: elfxx-x86.c:3757
+#: elfxx-x86.c:3774
 msgid "%P: %pB: warning: missing %s\n"
 msgstr ""
 
-#: elfxx-x86.c:3758
+#: elfxx-x86.c:3775
 msgid "%X%P: %pB: error: missing %s\n"
 msgstr ""
 
-#: elfxx-x86.c:3821
+#: elfxx-x86.c:3838
 msgid "IBT and SHSTK properties"
 msgstr ""
 
-#: elfxx-x86.c:3823
+#: elfxx-x86.c:3840
 msgid "IBT property"
 msgstr ""
 
-#: elfxx-x86.c:3825
+#: elfxx-x86.c:3842
 msgid "SHSTK property"
 msgstr ""
 
-#: elfxx-x86.c:3830
+#: elfxx-x86.c:3847
 msgid "LAM_U48 property"
 msgstr ""
 
-#: elfxx-x86.c:3835
+#: elfxx-x86.c:3852
 msgid "LAM_U57 property"
 msgstr ""
 
-#: elfxx-x86.c:3979
+#: elfxx-x86.c:3996
 msgid "%F%P: failed to create VxWorks dynamic sections\n"
 msgstr ""
 
-#: elfxx-x86.c:3988
+#: elfxx-x86.c:4005
 msgid "%F%P: failed to create GOT sections\n"
 msgstr ""
 
-#: elfxx-x86.c:4006
+#: elfxx-x86.c:4023
 msgid "%F%P: failed to create ifunc sections\n"
 msgstr ""
 
-#: elfxx-x86.c:4043
+#: elfxx-x86.c:4060
 msgid "%F%P: failed to create GOT PLT section\n"
 msgstr ""
 
-#: elfxx-x86.c:4062
+#: elfxx-x86.c:4079
 msgid "%F%P: failed to create IBT-enabled PLT section\n"
 msgstr ""
 
-#: elfxx-x86.c:4076
+#: elfxx-x86.c:4093
 msgid "%F%P: failed to create BND PLT section\n"
 msgstr ""
 
-#: elfxx-x86.c:4096
+#: elfxx-x86.c:4113
 msgid "%F%P: failed to create PLT .eh_frame section\n"
 msgstr ""
 
-#: elfxx-x86.c:4109
+#: elfxx-x86.c:4126
 msgid "%F%P: failed to create GOT PLT .eh_frame section\n"
 msgstr ""
 
-#: elfxx-x86.c:4123
+#: elfxx-x86.c:4140
 msgid "%F%P: failed to create the second PLT .eh_frame section\n"
 msgstr ""
 
-#: elfxx-x86.c:4165
+#: elfxx-x86.c:4182
 msgid "%X%P: attempted static link of dynamic object `%pB'\n"
 msgstr ""
 



















--- bfd/section.c	2022-07-08 10:46:47.000000000 +0100
+++ bfd/section.c	2025-11-07 11:25:06.613425930 +0000
@@ -746,7 +746,7 @@ CODE_FRAGMENT
 /* These symbols are global, not specific to any BFD.  Therefore, anything
    that tries to change them is broken, and should be repaired.  */
 
-static const asymbol global_syms[] =
+static asymbol global_syms[] =
 {
   GLOBAL_SYM_INIT (BFD_COM_SECTION_NAME, bfd_com_section_ptr),
   GLOBAL_SYM_INIT (BFD_UND_SECTION_NAME, bfd_und_section_ptr),
@@ -757,7 +757,7 @@ static const asymbol global_syms[] =
 #define STD_SECTION(NAME, IDX, FLAGS) \
   BFD_FAKE_SECTION(_bfd_std_section[IDX], &global_syms[IDX], NAME, IDX, FLAGS)
 
-asection _bfd_std_section[] = {
+BFDDECL asection _bfd_std_section[] = {
   STD_SECTION (BFD_COM_SECTION_NAME, 0, SEC_IS_COMMON),
   STD_SECTION (BFD_UND_SECTION_NAME, 1, 0),
   STD_SECTION (BFD_ABS_SECTION_NAME, 2, 0),
@@ -826,6 +826,21 @@ bfd_section_init (bfd *abfd, asection *n
   newsect->index = abfd->section_count;
   newsect->owner = abfd;
 
+  /* Create a symbol whose only job is to point to this section.  This
+     is useful for things like relocs which are relative to the base
+     of a section.  */
+  newsect->symbol = bfd_make_empty_symbol (abfd);
+  if (newsect->symbol == NULL)
+    return NULL;
+
+  newsect->symbol->name = newsect->name;
+  newsect->symbol->value = 0;
+  newsect->symbol->section = newsect;
+  newsect->symbol->flags = BSF_SECTION_SYM;
+
+  newsect->symbol_ptr_ptr = &newsect->symbol;
+
+
   if (! BFD_SEND (abfd, _new_section_hook, (abfd, newsect)))
     return NULL;
 
@@ -1438,6 +1453,7 @@ bfd_set_section_size (asection *sec, bfd
     }
 
   sec->size = val;
+  sec->rawsize = val;  
   return true;
 }
 

--- bfd/srec.c	2022-07-08 10:46:47.000000000 +0100
+++ bfd/srec.c	2025-11-07 11:25:06.613425930 +0000
@@ -129,12 +129,12 @@ static const char digs[] = "0123456789AB
 /* The number of data bytes we actually fit onto a line on output.
    This variable can be modified by objcopy's --srec-len parameter.
    For a 0x75 byte record you should set --srec-len=0x70.  */
-unsigned int _bfd_srec_len = DEFAULT_CHUNK;
+BFDDECL unsigned int _bfd_srec_len = DEFAULT_CHUNK;
 
 /* The type of srec output (free or forced to S3).
    This variable can be modified by objcopy's --srec-forceS3
    parameter.  */
-bool _bfd_srec_forceS3 = false;
+BFDDECL bool _bfd_srec_forceS3 = false;
 
 /* When writing an S-record file, the S-records can not be output as
    they are seen.  This structure is used to hold them in memory.  */

--- bfd/sysdep.h	2022-07-08 10:46:47.000000000 +0100
+++ bfd/sysdep.h	2025-11-07 11:25:06.617425990 +0000
@@ -22,6 +22,10 @@
 #ifndef BFD_SYSDEP_H
 #define BFD_SYSDEP_H
 
+#ifndef BFDDECL
+#define BFDDECL
+#endif
+
 #ifdef PACKAGE
 #error sysdep.h must be included in lieu of config.h
 #endif

--- bfd/targets.c	2022-07-08 10:46:47.000000000 +0100
+++ bfd/targets.c	2025-11-07 11:25:06.617425990 +0000
@@ -151,6 +151,7 @@ DESCRIPTION
 .{
 .  {* N.B. Update bfd_flavour_name if you change this.  *}
 .  bfd_target_unknown_flavour,
+.  bfd_target_amiga_flavour,
 .  bfd_target_aout_flavour,
 .  bfd_target_coff_flavour,
 .  bfd_target_ecoff_flavour,
@@ -687,6 +688,8 @@ extern const bfd_target alpha_vms_vec;
 extern const bfd_target alpha_vms_lib_txt_vec;
 extern const bfd_target am33_elf32_linux_vec;
 extern const bfd_target amdgcn_elf64_le_vec;
+extern const bfd_target amiga_vec;
+extern const bfd_target aout_amiga_vec;
 extern const bfd_target aout_vec;
 extern const bfd_target arc_elf32_be_vec;
 extern const bfd_target arc_elf32_le_vec;
@@ -1007,6 +1010,8 @@ static const bfd_target * const _bfd_tar
 
 	&am33_elf32_linux_vec,
 
+	&amiga_vec,
+	&aout_amiga_vec,
 #if 0
 	/* Since a.out files lack decent magic numbers, no way to recognize
 	   which kind of a.out file it is.  */
@@ -1422,7 +1427,7 @@ const bfd_target *const *const bfd_targe
 /* bfd_default_vector[0] contains either the address of the default vector,
    if there is one, or zero if there isn't.  */
 
-const bfd_target *bfd_default_vector[] = {
+bfd_target * bfd_default_vector[] = {
 #ifdef DEFAULT_VECTOR
 	&DEFAULT_VECTOR,
 #endif
@@ -1780,6 +1785,7 @@ bfd_flavour_name (enum bfd_flavour flavo
   switch (flavour)
     {
     case bfd_target_unknown_flavour: return "unknown file format";
+    case bfd_target_amiga_flavour: return "amiga-hunk";
     case bfd_target_aout_flavour: return "a.out";
     case bfd_target_coff_flavour: return "COFF";
     case bfd_target_ecoff_flavour: return "ECOFF";

--- bfd/version.h	2022-08-05 08:38:29.000000000 +0100
+++ bfd/version.h	2025-11-07 11:25:06.617425990 +0000
@@ -1,22 +1,4 @@
-/* The date below is automatically updated every day by a bot.  During
-   development, we include the date in the tools' version strings
-   (visible in 'ld -v' etc.) because people build binutils from a
-   variety of sources - git, tarballs, distro sources - and we want
-   something that can easily identify the source they used when they
-   report bugs.  The bfd version plus date is usually good enough for
-   that purpose.
-
-   During development, this date ends up in libbfd and libopcodes
-   sonames because people naturally expect shared libraries with the
-   same soname to have compatible ABIs.  We could bump the bfd version
-   on every ABI change, but that's just another thing contributors and
-   maintainers would need to remember.  Instead, it's much easier for
-   all if the soname contains the date.  This is not perfect but is
-   good enough.
-
-   In releases, the date is not included in either version strings or
-   sonames.  */
-#define BFD_VERSION_DATE 20220805
+#define BFD_VERSION_DATE 241223-114644
 #define BFD_VERSION @bfd_version@
-#define BFD_VERSION_STRING  @bfd_version_package@ @bfd_version_string@
+#define BFD_VERSION_STRING @bfd_version_package@ @bfd_version_string@
 #define REPORT_BUGS_TO @report_bugs_to@

--- bfd/version.m4	2022-08-05 10:49:43.000000000 +0100
+++ bfd/version.m4	2025-11-07 11:25:06.617425990 +0000
@@ -1 +1 @@
-m4_define([BFD_VERSION], [2.39])
+m4_define([BFD_VERSION], [2.39.0])

--- binutils/ar.c	2022-07-08 10:46:47.000000000 +0100
+++ binutils/ar.c	2025-11-07 11:25:06.641426350 +0000
@@ -714,10 +714,12 @@ ranlib_main (int argc, char **argv)
   xexit (status);
 }
 
-int main (int, char **);
+int is_ranlib;
+
+int armain (int, char **,int);
 
 int
-main (int argc, char **argv)
+armain (int argc, char **argv, int isran)
 {
   int arg_index;
   char **files;
@@ -725,6 +727,8 @@ main (int argc, char **argv)
   char *inarch_filename;
   int i;
 
+  is_ranlib = isran;
+  
 #ifdef HAVE_LC_MESSAGES
   setlocale (LC_MESSAGES, "");
 #endif




--- binutils/bucomm.c	2022-07-08 10:46:47.000000000 +0100
+++ binutils/bucomm.c	2025-11-07 11:25:06.641426350 +0000
@@ -31,7 +31,7 @@
 
 /* Error reporting.  */
 
-char *program_name;
+BINUTILSDECL char *program_name;
 
 void
 bfd_nonfatal (const char *string)

--- binutils/bucomm.h	2022-07-08 10:46:47.000000000 +0100
+++ binutils/bucomm.h	2025-11-07 11:25:06.641426350 +0000
@@ -62,7 +62,7 @@ off_t get_file_size (const char *);
 
 bool is_valid_archive_path (char const *);
 
-extern char *program_name;
+extern BINUTILSDECL char *program_name;
 
 /* In filemode.c.  */
 void mode_string (unsigned long, char *);

--- binutils/config.in	2022-08-05 10:56:24.000000000 +0100
+++ binutils/config.in	2025-11-07 11:25:06.641426350 +0000
@@ -204,6 +204,9 @@
 /* Configured target name. */
 #undef TARGET
 
+/* Is Amiga target. */
+#undef TARGET_AMIGA
+
 /* Define to 1 if user symbol names have a leading underscore, 0 if not. */
 #undef TARGET_PREPENDS_UNDERSCORE
 

--- binutils/configure	2022-08-05 10:56:21.000000000 +0100
+++ binutils/configure	2025-11-07 11:25:06.649426470 +0000
@@ -1,6 +1,6 @@
 #! /bin/sh
 # Guess values for system-dependent variables and create Makefiles.
-# Generated by GNU Autoconf 2.69 for binutils 2.39.
+# Generated by GNU Autoconf 2.69 for binutils 2.39.0.
 #
 #
 # Copyright (C) 1992-1996, 1998-2012 Free Software Foundation, Inc.
@@ -587,8 +587,8 @@ MAKEFLAGS=
 # Identity of this package.
 PACKAGE_NAME='binutils'
 PACKAGE_TARNAME='binutils'
-PACKAGE_VERSION='2.39'
-PACKAGE_STRING='binutils 2.39'
+PACKAGE_VERSION='2.39.0'
+PACKAGE_STRING='binutils 2.39.0'
 PACKAGE_BUGREPORT=''
 PACKAGE_URL=''
 
@@ -1395,7 +1395,7 @@ if test "$ac_init_help" = "long"; then
   # Omit some internal or obsolete options to make the list less imposing.
   # This message is too long to be a string in the A/UX 3.1 sh.
   cat <<_ACEOF
-\`configure' configures binutils 2.39 to adapt to many kinds of systems.
+\`configure' configures binutils 2.39.0 to adapt to many kinds of systems.
 
 Usage: $0 [OPTION]... [VAR=VALUE]...
 
@@ -1466,7 +1466,7 @@ fi
 
 if test -n "$ac_init_help"; then
   case $ac_init_help in
-     short | recursive ) echo "Configuration of binutils 2.39:";;
+     short | recursive ) echo "Configuration of binutils 2.39.0:";;
    esac
   cat <<\_ACEOF
 
@@ -1618,7 +1618,7 @@ fi
 test -n "$ac_init_help" && exit $ac_status
 if $ac_init_version; then
   cat <<\_ACEOF
-binutils configure 2.39
+binutils configure 2.39.0
 generated by GNU Autoconf 2.69
 
 Copyright (C) 2012 Free Software Foundation, Inc.
@@ -2269,7 +2269,7 @@ cat >config.log <<_ACEOF
 This file contains any messages produced by compilers while
 running configure, to aid debugging if configure makes a mistake.
 
-It was created by binutils $as_me 2.39, which was
+It was created by binutils $as_me 2.39.0, which was
 generated by GNU Autoconf 2.69.  Invocation command line was
 
   $ $0 $@
@@ -3251,7 +3251,7 @@ fi
 
 # Define the identity of the package.
  PACKAGE='binutils'
- VERSION='2.39'
+ VERSION='2.39.0'
 
 
 cat >>confdefs.h <<_ACEOF
@@ -14846,6 +14846,11 @@ cat >>confdefs.h <<_ACEOF
 #define TARGET "${target}"
 _ACEOF
 
+if test "x${target}" = "xm68k-unknown-amigaos"; then
+cat >>confdefs.h <<_ACEOF
+#define TARGET_AMIGA 1
+_ACEOF
+fi
 
 targ=$target
 . $srcdir/../bfd/config.bfd
@@ -15419,7 +15424,7 @@ cat >>$CONFIG_STATUS <<\_ACEOF || ac_wri
 # report actual input values of CONFIG_FILES etc. instead of their
 # values after options handling.
 ac_log="
-This file was extended by binutils $as_me 2.39, which was
+This file was extended by binutils $as_me 2.39.0, which was
 generated by GNU Autoconf 2.69.  Invocation command line was
 
   CONFIG_FILES    = $CONFIG_FILES
@@ -15485,7 +15490,7 @@ _ACEOF
 cat >>$CONFIG_STATUS <<_ACEOF || ac_write_fail=1
 ac_cs_config="`$as_echo "$ac_configure_args" | sed 's/^ //; s/[\\""\`\$]/\\\\&/g'`"
 ac_cs_version="\\
-binutils config.status 2.39
+binutils config.status 2.39.0
 configured by $0, generated by GNU Autoconf 2.69,
   with options \\"\$ac_cs_config\\"
 

--- binutils/configure.ac	2022-07-08 10:46:47.000000000 +0100
+++ binutils/configure.ac	2025-11-07 11:25:06.649426470 +0000
@@ -527,6 +527,10 @@ AC_SUBST(OBJDUMP_PRIVATE_OFILES)
 
 AC_DEFINE_UNQUOTED(TARGET, "${target}", [Configured target name.])
 
+if test "x${target}" = "xm68k-unknown-amigaos"; then
+AC_DEFINE_UNQUOTED(TARGET_AMIGA, 1, [Configured target is Amiga.])
+fi
+
 targ=$target
 . $srcdir/../bfd/config.bfd
 if test "x$targ_underscore" = "xyes"; then

--- binutils/cxxfilt.c	2022-07-08 10:46:47.000000000 +0100
+++ binutils/cxxfilt.c	2025-11-07 11:25:06.649426470 +0000
@@ -141,7 +141,7 @@ int
 main (int argc, char **argv)
 {
   int c;
-  const char *valid_symbols;
+  const char *valid_symbols = "";
   enum demangling_styles style = auto_demangling;
 
   program_name = argv[0];





















--- binutils/elfcomm.c	2022-07-08 10:46:47.000000000 +0100
+++ binutils/elfcomm.c	2025-11-07 11:25:06.657426589 +0000
@@ -95,7 +95,7 @@ byte_put_big_endian (unsigned char * fie
     }
 }
 
-elf_vma (*byte_get) (const unsigned char *, unsigned int);
+BINUTILSDECL elf_vma (*byte_get) (const unsigned char *, unsigned int);
 
 elf_vma
 byte_get_little_endian (const unsigned char *field, unsigned int size)

--- binutils/elfcomm.h	2022-07-08 10:46:47.000000000 +0100
+++ binutils/elfcomm.h	2025-11-07 11:25:06.657426589 +0000
@@ -35,7 +35,7 @@ extern void (*byte_put) (unsigned char *
 extern void byte_put_little_endian (unsigned char *, elf_vma, unsigned int);
 extern void byte_put_big_endian (unsigned char *, elf_vma, unsigned int);
 
-extern elf_vma (*byte_get) (const unsigned char *, unsigned int);
+extern BINUTILSDECL elf_vma (*byte_get) (const unsigned char *, unsigned int);
 extern elf_vma byte_get_signed (const unsigned char *, unsigned int);
 extern elf_vma byte_get_little_endian (const unsigned char *, unsigned int);
 extern elf_vma byte_get_big_endian (const unsigned char *, unsigned int);

--- binutils/is-ranlib.c	2022-07-08 10:46:47.000000000 +0100
+++ binutils/is-ranlib.c	2025-11-07 11:25:06.657426589 +0000
@@ -19,4 +19,8 @@
 
 /* Linked with ar.o to flag that this program is 'ranlib' (not 'ar').  */
 
-int is_ranlib = 1;
+extern int armain(int argc, char ** argv, int isran);
+int main(int argc, char ** argv);
+int main(int argc, char ** argv) {
+	armain(argc, argv, 1);
+}
\ No newline at end of file


--- binutils/is-strip.c	2022-07-08 10:46:47.000000000 +0100
+++ binutils/is-strip.c	2025-11-07 11:25:06.657426589 +0000
@@ -20,4 +20,9 @@
 /* Linked with objcopy.o to flag that this program is 'strip' (not
    'objcopy').  */
 
-int is_strip = 1;
+extern int obmain(int argc, char ** argv, int);
+
+int main(int argc, char ** argv)
+{
+	return obmain(argc, argv, 1);
+}

--- binutils/Makefile.am	2022-07-08 10:46:47.000000000 +0100
+++ binutils/Makefile.am	2025-11-07 11:25:06.637426290 +0000
@@ -137,7 +137,7 @@ CFILES = \
 	coffdump.c coffgrok.c cxxfilt.c \
 	dwarf.c debug.c demanguse.c dlltool.c dllwrap.c \
 	elfcomm.c emul_aix.c emul_vanilla.c filemode.c \
-	is-ranlib.c is-strip.c maybe-ranlib.c maybe-strip.c \
+	is-ranlib.c is-readelf.c is-strip.c maybe-ranlib.c maybe-strip.c \
 	nm.c not-ranlib.c not-strip.c \
 	objcopy.c objdump.c prdbg.c \
 	od-elf32_avr.c od-macho.c od-xcoff.c \
@@ -255,8 +255,8 @@ objcopy_SOURCES = objcopy.c not-strip.c
 
 strings_SOURCES = strings.c $(BULIBS)
 
-readelf_SOURCES = readelf.c version.c unwind-ia64.c dwarf.c demanguse.c $(ELFLIBS)
-readelf_LDADD   = $(LIBCTF_NOBFD) $(LIBINTL) $(LIBIBERTY) $(ZLIB) $(DEBUGINFOD_LIBS) $(MSGPACK_LIBS)
+readelf_SOURCES = is-readelf.c readelf.c version.c unwind-ia64.c dwarf.c demanguse.c $(ELFLIBS)
+readelf_LDADD   = $(LIBCTF_NOBFD) $(LIBINTL) $(LIBIBERTY) $(ZLIB) $(DEBUGINFOD_LIBS)
 
 elfedit_SOURCES = elfedit.c version.c $(ELFLIBS)
 elfedit_LDADD = $(LIBINTL) $(LIBIBERTY)

--- binutils/Makefile.in	2022-08-05 10:56:21.000000000 +0100
+++ binutils/Makefile.in	2025-11-07 11:25:06.641426350 +0000
@@ -250,7 +250,7 @@ am_ranlib_OBJECTS = ar.$(OBJEXT) is-ranl
 	arlex.$(OBJEXT) arsup.$(OBJEXT) rename.$(OBJEXT) \
 	binemul.$(OBJEXT) emul_$(EMULATION).$(OBJEXT) $(am__objects_1)
 ranlib_OBJECTS = $(am_ranlib_OBJECTS)
-am_readelf_OBJECTS = readelf.$(OBJEXT) version.$(OBJEXT) \
+am_readelf_OBJECTS = is-readelf.$(OBJEXT) readelf.$(OBJEXT) version.$(OBJEXT) \
 	unwind-ia64.$(OBJEXT) dwarf.$(OBJEXT) demanguse.$(OBJEXT) \
 	$(am__objects_2)
 readelf_OBJECTS = $(am_readelf_OBJECTS)
@@ -709,7 +709,7 @@ CFILES = \
 	coffdump.c coffgrok.c cxxfilt.c \
 	dwarf.c debug.c demanguse.c dlltool.c dllwrap.c \
 	elfcomm.c emul_aix.c emul_vanilla.c filemode.c \
-	is-ranlib.c is-strip.c maybe-ranlib.c maybe-strip.c \
+	is-ranlib.c is-readelf.c is-strip.c maybe-ranlib.c maybe-strip.c \
 	nm.c not-ranlib.c not-strip.c \
 	objcopy.c objdump.c prdbg.c \
 	od-elf32_avr.c od-macho.c od-xcoff.c \
@@ -786,8 +786,8 @@ LDADD = $(BFDLIB) $(LIBIBERTY) $(LIBINTL
 size_SOURCES = size.c $(BULIBS)
 objcopy_SOURCES = objcopy.c not-strip.c rename.c $(WRITE_DEBUG_SRCS) $(BULIBS)
 strings_SOURCES = strings.c $(BULIBS)
-readelf_SOURCES = readelf.c version.c unwind-ia64.c dwarf.c demanguse.c $(ELFLIBS)
-readelf_LDADD = $(LIBCTF_NOBFD) $(LIBINTL) $(LIBIBERTY) $(ZLIB) $(DEBUGINFOD_LIBS) $(MSGPACK_LIBS)
+readelf_SOURCES = is-readelf.c readelf.c version.c unwind-ia64.c dwarf.c demanguse.c $(ELFLIBS)
+readelf_LDADD = $(LIBCTF_NOBFD) $(LIBINTL) $(LIBIBERTY) $(ZLIB) $(DEBUGINFOD_LIBS)
 elfedit_SOURCES = elfedit.c version.c $(ELFLIBS)
 elfedit_LDADD = $(LIBINTL) $(LIBIBERTY)
 strip_new_SOURCES = objcopy.c is-strip.c rename.c $(WRITE_DEBUG_SRCS) $(BULIBS)
@@ -1165,6 +1165,7 @@ distclean-compile:
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/emul_vanilla.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/filemode.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/is-ranlib.Po@am__quote@
+@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/is-readelf.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/is-strip.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/maybe-ranlib.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/maybe-strip.Po@am__quote@



--- binutils/nm.c	2022-07-08 10:46:47.000000000 +0100
+++ binutils/nm.c	2025-11-07 11:25:06.661426649 +0000
@@ -972,6 +972,12 @@ size_forward1 (const void *P_x, const vo
   if (bfd_is_und_section (ys))
     abort ();
 
+#ifdef TARGET_AMIGA
+  xf = strcmp(xs->name, ys->name);
+  if (xf)
+    return xf;
+#endif
+
   if (valueof (x) != valueof (y))
     return valueof (x) < valueof (y) ? -1 : 1;
 
@@ -1081,6 +1087,8 @@ sort_symbols_by_size (bfd *abfd, bool is
       bfd_vma sz;
       asymbol *temp;
 
+//      printf("%s -> %08x\n", sym->name, valueof (sym));
+
       if (from + size < fromend)
 	{
 	  next = bfd_minisymbol_to_symbol (abfd,

--- binutils/not-ranlib.c	2022-07-08 10:46:47.000000000 +0100
+++ binutils/not-ranlib.c	2025-11-07 11:25:06.661426649 +0000
@@ -19,4 +19,9 @@
 
 /* Linked with ar.o to flag that this program is 'ar' (not 'ranlib').  */
 
-int is_ranlib = 0;
+extern int armain(int argc, char ** argv, int);
+
+int main(int argc, char ** argv)
+{
+	return armain(argc, argv, 0);
+}
\ No newline at end of file

--- binutils/not-strip.c	2022-07-08 10:46:47.000000000 +0100
+++ binutils/not-strip.c	2025-11-07 11:25:06.661426649 +0000
@@ -20,4 +20,9 @@
 /* Linked with objcopy.o to flag that this program is 'objcopy' (not
    'strip').  */
 
-int is_strip = 0;
+extern int obmain(int argc, char ** argv, int);
+
+int main(int argc, char ** argv)
+{
+	return obmain(argc, argv, 0);
+}

--- binutils/objcopy.c	2022-07-08 10:46:47.000000000 +0100
+++ binutils/objcopy.c	2025-11-07 11:25:06.661426649 +0000
@@ -37,6 +37,9 @@
 #include "coff/i386.h"
 #include "coff/pe.h"
 
+int
+obmain(int argc, char *argv[], int _iss);
+
 static bfd_vma pe_file_alignment = (bfd_vma) -1;
 static bfd_vma pe_heap_commit = (bfd_vma) -1;
 static bfd_vma pe_heap_reserve = (bfd_vma) -1;
@@ -529,16 +532,16 @@ extern char *program_name;
 /* This flag distinguishes between strip and objcopy:
    1 means this is 'strip'; 0 means this is 'objcopy'.
    -1 means if we should use argv[0] to decide.  */
-extern int is_strip;
+int is_strip;
 
 /* The maximum length of an S record.  This variable is defined in srec.c
    and can be modified by the --srec-len parameter.  */
-extern unsigned int _bfd_srec_len;
+extern BFDDECL unsigned int _bfd_srec_len;
 
 /* Restrict the generation of Srecords to type S3 only.
    This variable is defined in bfd/srec.c and can be toggled
    on by the --srec-forceS3 command line switch.  */
-extern bool _bfd_srec_forceS3;
+extern BFDDECL bool _bfd_srec_forceS3;
 
 /* Width of data in bytes for verilog output.
    This variable is declared in bfd/verilog.c and can be modified by
@@ -4359,8 +4362,12 @@ copy_relocations_in_section (bfd *ibfd,
 	    if (relpp[i]->sym_ptr_ptr
 		/* PR 20096 */
 		&& *relpp[i]->sym_ptr_ptr
-		&& is_specified_symbol (bfd_asymbol_name (*relpp[i]->sym_ptr_ptr),
-					keep_specific_htab))
+		&& (is_specified_symbol (bfd_asymbol_name (*relpp[i]->sym_ptr_ptr),
+					keep_specific_htab)
+#ifdef TARGET_AMIGA
+			|| 1
+#endif					
+					))
 	      *w_relpp++ = relpp[i];
 	  relcount = w_relpp - relpp;
 	  *w_relpp = 0;
@@ -6010,8 +6017,9 @@ copy_main (int argc, char *argv[])
 }
 
 int
-main (int argc, char *argv[])
+obmain (int argc, char *argv[], int _iss)
 {
+	is_strip = _iss;
 #ifdef HAVE_LC_MESSAGES
   setlocale (LC_MESSAGES, "");
 #endif

--- binutils/objdump.c	2022-07-08 10:46:47.000000000 +0100
+++ binutils/objdump.c	2025-11-07 11:25:06.661426649 +0000
@@ -72,6 +72,10 @@
 #include <sys/mman.h>
 #endif
 
+enum label_type {
+  LT_UNDEFINED, LT_CODE, LT_DATA
+};
+
 /* Internal headers for the ELF .stab-dump code - sorry.  */
 #define	BYTES_IN_WORD	32
 #include "aout/aout64.h"
@@ -133,6 +137,9 @@ static int process_links = false;
 static bool disassembler_color = false; /* --disassembler-color=color.  */
 static bool disassembler_extended_color = false; /* --disassembler-color=extended-color.  */
 
+static bool omit_offsets;	/* -N */
+static bool create_labels;       /* -Q */
+
 static int dump_any_debugging;
 static int demangle_flags = DMGL_ANSI | DMGL_PARAMS;
 
@@ -163,14 +170,19 @@ static int include_path_count;
 struct objdump_disasm_info
 {
   bfd *abfd;
+  asection *         sec;  
   bool require_sec;
   disassembler_ftype disassemble_fn;
-  arelent *reloc;
-  const char *symbol;
+  const char *       symbol;
+  arelent *          reloc;  
+  arelent ***        relppp; // pointer to relocations
+  bfd_vma            vma; // code position
+  unsigned char *    buffer; // vma + *pp - buffer determines relppp usage
+  unsigned char **   pp; // current position in buffer
 };
 
 /* Architecture to disassemble for, or default if NULL.  */
-static char *machine = NULL;
+static char *machine = "68040";
 
 /* Target specific options to the disassembler.  */
 static char *disassembler_options = NULL;
@@ -190,6 +202,9 @@ static asymbol **sorted_syms;
 /* Number of symbols in `sorted_syms'.  */
 static long sorted_symcount = 0;
 
+/* Lookup map for insn starts. */
+static bfd_byte * lookup;
+
 /* The dynamic symbol table.  */
 static asymbol **dynsyms;
 
@@ -337,6 +352,10 @@ usage (FILE *stream, int status)
       fprintf (stream, _("\
   -M, --disassembler-options=OPT Pass text OPT on to the disassembler\n"));
       fprintf (stream, _("\
+  -N                             Omit the offsets\n"));
+      fprintf (stream, _("\
+  -Q                             Create local labels - may take loooong\n"));
+      fprintf (stream, _("\
   -EB --endian=big               Assume big endian format when disassembling\n"));
       fprintf (stream, _("\
   -EL --endian=little            Assume little endian format when disassembling\n"));
@@ -712,7 +731,15 @@ sanitize_string (const char * in)
 }
 
 
-/* Returns TRUE if the specified section should be dumped.  */
+
+
+static void dummy_error_func (int status, bfd_vma memaddr, struct disassemble_info *dinfo) {
+}
+static int dummy_fprintf(void * v ATTRIBUTE_UNUSED, char const *f ATTRIBUTE_UNUSED, ...) {
+  return 0;
+}
+
+/* Returns true if the specified section should be dumped.  */
 
 static bool
 process_section_p (asection * section)
@@ -960,6 +987,21 @@ slurp_symtab (bfd *abfd)
   asymbol **sy = NULL;
   long storage;
 
+  // reset all stuff
+  syms = NULL;
+  symcount = 0;
+  sorted_syms = NULL;
+  sorted_symcount = 0;
+  lookup = NULL;
+  dynsyms = NULL;
+  synthsyms = NULL;
+  synthcount = 0;
+  dynsymcount = 0;
+  stabs = NULL;
+  stab_size = 0;
+  strtab = NULL;
+  stabstr_size = 0;
+
   if (!(bfd_get_file_flags (abfd) & HAS_SYMS))
     {
       symcount = 0;
@@ -1240,7 +1282,7 @@ compare_relocs (const void *ap, const vo
 }
 
 /* Print an address (VMA) to the output stream in INFO.
-   If SKIP_ZEROES is TRUE, omit leading zeroes.  */
+   If SKIP_ZEROES is true, omit leading zeroes.  */
 
 static void
 objdump_print_value (bfd_vma vma, struct disassemble_info *inf,
@@ -1344,7 +1386,7 @@ sym_ok (bool want_section,
 }
 
 /* Locate a symbol given a bfd and a section (from INFO->application_data),
-   and a VMA.  If INFO->application_data->require_sec is TRUE, then always
+   and a VMA.  If INFO->application_data->require_sec is true, then always
    require the symbol to be in the section.  Returns NULL if there is no
    suitable symbol.  If PLACE is not NULL, then *PLACE is set to the index
    of the symbol in sorted_syms.  */
@@ -1377,6 +1419,9 @@ find_symbol_for_address (bfd_vma vma,
   abfd = aux->abfd;
   sec = inf->section;
   opb = inf->octets_per_byte;
+  aux->relppp = NULL;
+  aux->buffer = NULL;
+  aux->pp = &aux->buffer;
 
   /* Perform a binary search looking for the closest symbol to the
      required value.  We are searching the range (min, max_count].  */
@@ -1551,6 +1596,161 @@ find_symbol_for_address (bfd_vma vma,
   return sorted_syms[thisplace];
 }
 
+
+/**
+ * search the sorted_syms plus the closest symbol of the same section.
+ */
+static int
+find_closest_symbol_index(bfd_vma vma, asection * section)
+{
+  long min = 0;
+  long max_count = sorted_symcount;
+  long thisplace = -1;
+
+  while (min + 1 < max_count)
+    {
+      asymbol *sym;
+
+      thisplace = (max_count + min) / 2;
+      sym = sorted_syms[thisplace];
+
+      if (bfd_asymbol_value (sym) > vma)
+	max_count = thisplace;
+      else if (bfd_asymbol_value (sym) < vma)
+	min = thisplace;
+      else
+	{
+	  min = thisplace;
+	  break;
+	}
+    }
+
+
+  // find the index
+  int index = thisplace;
+  while (index + 1 < sorted_symcount
+      && (sorted_syms[index]->value < vma || sorted_syms[index]->section != section))
+    ++index;
+  while (index > 0
+      && (sorted_syms[index]->value > vma || sorted_syms[index]->section != section))
+    --index;
+
+  return index;
+}
+
+/**
+ * Create the label and insert it.
+ *
+ *
+ */
+static void
+create_label(bfd_vma vma, struct disassemble_info *inf, enum label_type lt_type)
+{
+  int index = find_closest_symbol_index(vma, inf->section);
+  asymbol * sym = sorted_syms[index];
+
+  // there is an exact label
+  if (sym->value == vma)
+    {
+      if (lt_type == LT_CODE)
+	sym->flags |= BSF_CODE;
+      return;
+    }
+
+  // if it's a code label, and vma inside of the visited range,
+  // it must match the start of an insn.
+  if ((sym->flags & BSF_CODE) && sym->udata.i && vma < sym->udata.i)
+    {
+      int bit = 1 << vma % 15 / 2;
+      if (!(lookup[(vma + 15) / 16] & bit))
+	return;
+
+      // force a code label, since it's inside code
+      lt_type = LT_CODE;
+    }
+
+  // insert correctly regardless of section
+  while (index + 1 < sorted_symcount && sorted_syms[index + 1]->value < vma)
+    ++index;
+
+  ++index;
+
+  int tomove = inf->symtab_size - index;
+  // make room for the new symbol
+  ++inf->symtab_size;
+
+  static int total_size;
+  if (sorted_symcount > total_size)
+    {
+      total_size = sorted_symcount * 2;
+      sorted_syms = (asymbol **) xrealloc (sorted_syms, total_size * sizeof (asymbol *));
+    }
+  if (tomove > 0)
+    memmove(&sorted_syms[index + 1], &sorted_syms[index], tomove * sizeof(asymbol *));
+
+  asymbol * nsym = (asymbol *)xmalloc(sizeof(asymbol));
+  sorted_syms[index] = nsym;
+
+  // copy common and mark the flags
+  nsym->value = vma;
+  nsym->flags = sym->flags & ~(BSF_VISITED|BSF_CODE|BSF_DATA);
+  if (lt_type == LT_DATA)
+    nsym->flags |= BSF_DATA;
+  else
+    if (lt_type == LT_CODE)
+      nsym->flags |= BSF_CODE;
+  nsym->section = sym->section;
+  nsym->the_bfd = sym->the_bfd;
+  nsym->udata.i = 0;
+
+  // set vma
+  nsym->name = 0;
+#if 0
+  char lab[16];
+  static unsigned n;
+  static unsigned m;
+  if (nsym->flags & SBF_DATA)
+    snprintf(lab, 32, "_D%d", ++m);
+  else
+    snprintf(lab, 32, "_L%d", ++n);
+  char * name = (char *)xmalloc(16);
+  strcpy(name, lab);
+  nsym->name = name;
+#endif
+  ++sorted_symcount;
+}
+
+/**
+ * Check for a symbol for the label in text section, if none exists
+ */
+
+static void
+create_label_address_func (bfd_vma vma, struct disassemble_info *inf)
+{
+  struct objdump_disasm_info *aux;
+
+  if (vma >= inf->buffer_length)
+      return;
+
+  aux = (struct objdump_disasm_info *) inf->application_data;
+
+  // only branch insns
+  if ((aux->buffer[0] == 0x48 && aux->buffer[1] == 0x7a) // pea ...(pc)
+	||((aux->buffer[0] & 0xf1) == 0x41 && aux->buffer[1] == 0xfa) // lea ...(pc)
+	)
+	create_label(vma, inf, LT_UNDEFINED);
+  else if (!(vma & 1)
+      && (
+         (*aux->buffer >= 0x60 && *aux->buffer <= 0x6f)
+      || (aux->buffer[0] == 0x51 && aux->buffer[1] == 0xc9)
+      || (aux->buffer[0] == 0x4e && aux->buffer[1] == 0xf9) // jmp
+      || (aux->buffer[0] == 0x4e && aux->buffer[1] == 0xb9) // jsr
+      || (aux->buffer[0] == 0x4e && aux->buffer[1] == 0xba) // jsr
+      ))
+    create_label(vma, inf, LT_CODE);
+}
+
+
 /* Print an address and the offset to the nearest symbol.  */
 
 static void
@@ -1558,6 +1758,40 @@ objdump_print_addr_with_sym (bfd *abfd,
 			     bfd_vma vma, struct disassemble_info *inf,
 			     bool skip_zeroes)
 {
+//  objdump_print_value (vma, inf, skip_zeroes);
+
+  struct objdump_disasm_info *aux =
+      (struct objdump_disasm_info *) inf->application_data;
+
+  if (aux->relppp && *aux->relppp && **aux->relppp)
+    {
+      arelent * r = **aux->relppp;
+      bfd_vma offset = aux->vma + *aux->pp - aux->buffer;
+      if (r->address <= offset)
+	{
+	  sym = *r->sym_ptr_ptr;
+	  if (sym && sym->section != bfd_und_section_ptr)
+	    {
+	      arelent *** trelppp;
+	      // search the correct section
+	      sec = aux->sec;
+	      aux->sec = sym->section;
+	      trelppp = aux->relppp;
+
+	      asymbol * sym2 = find_symbol_for_address(vma, inf, NULL);
+
+	      aux->sec = sec;
+	      aux->relppp = trelppp;
+
+	      // update vma and section and move relppp
+	      if (sym2)
+		sym = sym2;
+	      sec = sym->section;
+	    }
+	  ++*aux->relppp;
+	}
+    }
+
   if (!no_addresses)
     {
       objdump_print_value (vma, inf, skip_zeroes);
@@ -1568,9 +1802,9 @@ objdump_print_addr_with_sym (bfd *abfd,
     {
       bfd_vma secaddr;
 
-      (*inf->fprintf_styled_func) (inf->stream, dis_style_text,"<");
+      (*inf->fprintf_styled_func) (inf->stream, dis_style_text, do_demangle ?" <" : " ");
       (*inf->fprintf_styled_func) (inf->stream, dis_style_symbol, "%s",
-				   sanitize_string (bfd_section_name (sec)));
+			    sanitize_string (bfd_section_name (sec)));
       secaddr = bfd_section_vma (sec);
       if (vma < secaddr)
 	{
@@ -1583,11 +1817,16 @@ objdump_print_addr_with_sym (bfd *abfd,
 	  (*inf->fprintf_styled_func) (inf->stream, dis_style_immediate, "+0x");
 	  objdump_print_value (vma - secaddr, inf, true);
 	}
-      (*inf->fprintf_styled_func) (inf->stream, dis_style_text, ">");
+
+      if (do_demangle)
+	(*inf->fprintf_styled_func) (inf->stream, dis_style_text, ">");
     }
   else
     {
-      (*inf->fprintf_styled_func) (inf->stream, dis_style_text, "<");
+      if (!omit_offsets)
+        objdump_print_value (vma, inf, skip_zeroes);
+
+      (*inf->fprintf_styled_func) (inf->stream, dis_style_text, do_demangle ? " <" : " ");
 
       objdump_print_symname (abfd, inf, sym);
 
@@ -1613,7 +1852,8 @@ objdump_print_addr_with_sym (bfd *abfd,
 	  objdump_print_value (vma - bfd_asymbol_value (sym), inf, true);
 	}
 
-      (*inf->fprintf_styled_func) (inf->stream, dis_style_text, ">");
+      if (do_demangle)
+	(*inf->fprintf_styled_func) (inf->stream, dis_style_text, ">");
     }
 
   if (display_file_offsets)
@@ -1623,7 +1863,7 @@ objdump_print_addr_with_sym (bfd *abfd,
 }
 
 /* Print an address (VMA), symbolically if possible.
-   If SKIP_ZEROES is TRUE, don't output leading zeroes.  */
+   If SKIP_ZEROES is true, don't output leading zeroes.  */
 
 static void
 objdump_print_addr (bfd_vma vma,
@@ -1633,6 +1873,7 @@ objdump_print_addr (bfd_vma vma,
   struct objdump_disasm_info *aux;
   asymbol *sym = NULL;
   bool skip_find = false;
+  asection *tmp;
 
   aux = (struct objdump_disasm_info *) inf->application_data;
 
@@ -1652,11 +1893,13 @@ objdump_print_addr (bfd_vma vma,
       return;
     }
 
+  tmp = inf->section;
   if (aux->reloc != NULL
       && aux->reloc->sym_ptr_ptr != NULL
       && * aux->reloc->sym_ptr_ptr != NULL)
     {
       sym = * aux->reloc->sym_ptr_ptr;
+      inf->section = sym->section;
 
       /* Adjust the vma to the reloc.  */
       vma += bfd_asymbol_value (sym);
@@ -1668,6 +1911,11 @@ objdump_print_addr (bfd_vma vma,
   if (!skip_find)
     sym = find_symbol_for_address (vma, inf, NULL);
 
+  inf->section = tmp;
+
+  if (create_labels && sym && sym->section == aux->sec && (sym->value != vma || sym->value == 0))
+    sym = NULL;
+
   objdump_print_addr_with_sym (aux->abfd, inf->section, sym, vma, inf,
 			       skip_zeroes);
 }
@@ -2958,6 +3206,7 @@ disassemble_bytes (struct disassemble_in
 		   arelent ***relppp,
 		   arelent **relppend)
 {
+  unsigned z;
   struct objdump_disasm_info *aux;
   asection *section;
   unsigned int octets_per_line;
@@ -2968,6 +3217,8 @@ disassemble_bytes (struct disassemble_in
   unsigned int skip_zeroes_at_end = inf->skip_zeroes_at_end;
   size_t octets;
   SFILE sfile;
+  bool is_text = 0 == strcmp(".text", inf->section->name);
+  bool insn_end = !insns;
 
   aux = (struct objdump_disasm_info *) inf->application_data;
   section = inf->section;
@@ -2980,6 +3231,8 @@ disassemble_bytes (struct disassemble_in
     octets_per_line = insn_width;
   else if (insns)
     octets_per_line = 4;
+  else if (create_labels)
+    octets_per_line = 32;
   else
     octets_per_line = 16;
 
@@ -3044,11 +3297,10 @@ disassemble_bytes (struct disassemble_in
 
       /* If we see more than SKIP_ZEROES octets of zeroes, we just
 	 print `...'.  */
-      if (! disassemble_zeroes)
-	for (; addr_offset * opb + octets < stop_offset * opb; octets++)
-	  if (data[addr_offset * opb + octets] != 0)
+      for (z = addr_offset * opb; z < stop_offset * opb; z++)
+	if (data[z] != 0)
 	    break;
-      if (! disassemble_zeroes
+      if (! disassemble_zeroes && !create_labels
 	  && (inf->insn_info_valid == 0
 	      || inf->branch_delay_insns == 0)
 	  && (octets >= skip_zeroes
@@ -3077,7 +3329,7 @@ disassemble_bytes (struct disassemble_in
 	}
       else
 	{
-	  char buf[50];
+	  char buf[256];
 	  unsigned int bpc = 0;
 	  unsigned int pb = 0;
 
@@ -3090,6 +3342,10 @@ disassemble_bytes (struct disassemble_in
 	    {
 	      char *s;
 
+	      if (omit_offsets)
+		printf("\t");
+	      else
+		{
 	      bfd_sprintf_vma (aux->abfd, buf, section->vma + addr_offset);
 	      for (s = buf + skip_addr_chars; *s == '0'; s++)
 		*s = ' ';
@@ -3097,6 +3353,7 @@ disassemble_bytes (struct disassemble_in
 		*--s = '0';
 	      printf ("%s:\t", buf + skip_addr_chars);
 	    }
+	    }
 	  else
 	    {
 	      aux->require_sec = true;
@@ -3111,7 +3368,7 @@ disassemble_bytes (struct disassemble_in
 
 	  if (insns)
 	    {
-	      int insn_size;
+	      int insn_size = insn_width;
 
 	      sfile.pos = 0;
 	      disassemble_set_printf
@@ -3190,16 +3447,33 @@ disassemble_bytes (struct disassemble_in
 		   disassembling code of course, and when -D is in effect.  */
 		inf->stop_vma = section->vma + stop_offset;
 
+	      aux->relppp = *relppp < relppend ? relppp : 0;
+	      aux->vma = section->vma + addr_offset;
 	      inf->stop_offset = stop_offset;
-	      disassembler_in_comment = false;
-	      insn_size = (*disassemble_fn) (section->vma + addr_offset, inf);
-	      octets = insn_size;
+	      octets = (*disassemble_fn) (section->vma + addr_offset, inf);
+	      aux->relppp = 0;
+
+	      // bra
+	      if (create_labels && is_text)
+		{
+		  bfd_vma pos = section->vma + addr_offset;
+		  if (data[pos] == 0x60)
+		    insn_end = true;
+		  else
+		  if (data[pos] == 0x4e &&
+		      ((data[pos + 1] & 0xc0) == 0xc0 // jmp
+		    || data[pos + 1] == 0x73 // rte
+		    || data[pos + 1] == 0x75 // rts
+		    || data[pos + 1] == 0x77 // rtr
+		    ))
+		    insn_end = true;
+	      }
 
 	      inf->stop_vma = 0;
 	      disassemble_set_printf (inf, stdout, (fprintf_ftype) fprintf,
 				      (fprintf_styled_ftype) fprintf_styled);
 	      if (insn_width == 0 && inf->bytes_per_line != 0)
-		octets_per_line = inf->bytes_per_line;
+		insn_size = octets_per_line = inf->bytes_per_line;
 	      if (insn_size < (int) opb)
 		{
 		  if (sfile.pos)
@@ -3215,23 +3489,113 @@ disassemble_bytes (struct disassemble_in
 	    }
 	  else
 	    {
-	      bfd_vma j;
+	      bfd_vma j, k;
 
 	      octets = octets_per_line;
 	      if (addr_offset + octets / opb > stop_offset)
 		octets = (stop_offset - addr_offset) * opb;
 
-	      for (j = addr_offset * opb; j < addr_offset * opb + octets; ++j)
+	      k = addr_offset * opb;
+	      if (create_labels)
+		{
+		  int i;
+		  if (ISPRINT(data[k]) || data[k] == '\r' || data[k] == '\n' || data[k] == '\t' || data[k] == ' ')
+		    {
+		      strcpy(buf, ".ascii \"");
+		      i = 8;
+		      for (j = k; j < k + octets; ++j)
+			{
+			  if (ISPRINT (data[j]))
+			    buf[i++] = data[j];
+			  else
+			    {
+			      switch (data[j]) {
+				case 0:
+				      buf[i++] = '\\';
+				      buf[i++] = '0';
+				      break;
+				case 8:
+				      buf[i++] = '\\';
+				      buf[i++] = 't';
+				      break;
+				case 0xa:
+				      buf[i++] = '\\';
+				      buf[i++] = 'n';
+				      break;
+				case 0xd:
+				      buf[i++] = '\\';
+				      buf[i++] = 'r';
+				      break;
+				default:
+				      buf[i++] = '\\';
+				      buf[i++] = 'x';
+				      sprintf(buf + i, "%02x", data[j]);
+				      i += 2;
+			      }
+			    }
+			}
+		      buf[i++] = '"';
+		      buf[i] = 0;
+		    }
+		  else
+		    {
+		      i = 6;
+		      if (octets & 1)
+			{
+			  strcpy(buf, ".byte ");
+			  for (j = k; j < k + octets; ++j)
+			    {
+			      if (j != k)
+				buf[i++] = ',';
+			      buf[i++] = '0';
+			      buf[i++] = 'x';
+			      sprintf(buf + i, "%02x", data[j]);
+			      i += 2;
+			    }
+			}
+		      else if (octets&3)
+			{
+			  strcpy(buf, ".short ");
+			  for (j = k; j < k + octets; j += 2)
+			    {
+			      if (j != k)
+				buf[i++] = ',';
+			      buf[i++] = '0';
+			      buf[i++] = 'x';
+			      sprintf(buf + i, "%04x", (data[j]<<8)  | data[j+1]);
+			      i += 4;
+			    }
+			}
+		      else
+			{
+			  strcpy(buf, ".long ");
+			  for (j = k; j < k + octets; j += 4)
+			    {
+			      if (j != k)
+				buf[i++] = ',';
+			      buf[i++] = '0';
+			      buf[i++] = 'x';
+			      sprintf(buf + i, "%08x", (data[j]<<24) | (data[j+1]<<16) | (data[j+2]<<8)| data[j+3]);
+			      i += 8;
+			    }
+			}
+		  }
+		}
+	      else
+		{
+		  for (j = k; j < k + octets; ++j)
 		{
 		  if (ISPRINT (data[j]))
-		    buf[j - addr_offset * opb] = data[j];
+			buf[j - k] = data[j];
 		  else
-		    buf[j - addr_offset * opb] = '.';
+			buf[j - k] = '.';
+		    }
+
+		  buf[j - k] = '\0';
 		}
-	      buf[j - addr_offset * opb] = '\0';
 	    }
 
-	  if (prefix_addresses
+	  if (prefix_addresses || create_labels
 	      ? show_raw_insn > 0
 	      : show_raw_insn >= 0)
 	    {
@@ -3288,9 +3652,44 @@ disassemble_bytes (struct disassemble_in
 	  if (! insns)
 	    printf ("%s", buf);
 	  else if (sfile.pos)
-	    printf ("%s", sfile.buffer);
+	    {
+	      struct bfd_symbol * asym = NULL;
+	      int off;
+	      /* check if relocation starts at start of insn, then it's a label */
+	      if ((*relppp) < relppend && (**relppp)->address == addr_offset)
+		{
+		  arelent *q = **relppp;
+		  const char *sym_name = NULL;
+		  asym = *q->sym_ptr_ptr;
+		  if (asym != NULL)
+		    {
+		      off = q->howto->bitsize == 32
+			  ? bfd_getb_signed_32(data + addr_offset)
+			  :  bfd_getb_signed_16(data + addr_offset);
+		      long index = find_closest_symbol_index(off, asym->section);
+		      asym = index >= 0 ? sorted_syms[index] : NULL;
+		    }
+		}
+	      if (asym == NULL)
+		{
+		  /* handle the case that this insn starts with 0000 and the end of insn
+		   * matches a reloc. Then treat the current insn as long too
+		   */
+		  if ((*relppp) < relppend && (**relppp)->address == addr_offset + octets / opb
+		      && bfd_getb_signed_16(data + addr_offset) == 0)
+		    printf (".long 0x%08lx", bfd_getb_signed_32(data + addr_offset));
+		  else
+		    printf ("%s", sfile.buffer);
+		}
+	      else
+		{
+		  printf (".long %s", bfd_asymbol_name (asym));
+		  if (asym->value != off)
+		    printf("+0x%lx", off - asym->value);
+		}
+	    }
 
-	  if (prefix_addresses
+	  if (prefix_addresses || create_labels
 	      ? show_raw_insn > 0
 	      : show_raw_insn >= 0)
 	    {
@@ -3422,6 +3821,15 @@ disassemble_bytes (struct disassemble_in
 	printf ("\n");
 
       addr_offset += octets / opb;
+
+      if (insn_end)
+	{
+	  insns = false;
+	  if (create_labels)
+	    octets_per_line = 32;
+	  else
+	    octets_per_line = 16;
+	}
     }
 
   free (sfile.buffer);
@@ -3494,43 +3902,35 @@ disassemble_section (bfd *abfd, asection
 
   /* Decide which set of relocs to use.  Load them if necessary.  */
   paux = (struct objdump_disasm_info *) pinfo->application_data;
-  if (pinfo->dynrelbuf && dump_dynamic_reloc_info)
-    {
-      rel_pp = pinfo->dynrelbuf;
-      rel_count = pinfo->dynrelcount;
-      /* Dynamic reloc addresses are absolute, non-dynamic are section
-	 relative.  REL_OFFSET specifies the reloc address corresponding
-	 to the start of this section.  */
-      rel_offset = section->vma;
-    }
-  else
+  paux->sec = section;
+
+  rel_count = 0;
+  rel_pp = NULL;
+  rel_offset = 0;
+
+  if ((section->flags & SEC_RELOC) != 0
+//	  && (dump_reloc_info || pinfo->disassembler_needs_relocs)
+      )
     {
-      rel_count = 0;
-      rel_pp = NULL;
-      rel_offset = 0;
+      long relsize;
 
-      if ((section->flags & SEC_RELOC) != 0
-	  && (dump_reloc_info || pinfo->disassembler_needs_relocs))
-	{
-	  long relsize;
+      relsize = bfd_get_reloc_upper_bound (abfd, section);
+      if (relsize < 0)
+	bfd_fatal (bfd_get_filename (abfd));
 
-	  relsize = bfd_get_reloc_upper_bound (abfd, section);
-	  if (relsize < 0)
+      if (relsize > 0)
+	{
+	  rel_ppstart = rel_pp = (arelent **) xmalloc (relsize);
+	  rel_count = bfd_canonicalize_reloc (abfd, section, rel_pp, syms);
+	  if (rel_count < 0)
 	    bfd_fatal (bfd_get_filename (abfd));
 
-	  if (relsize > 0)
-	    {
-	      rel_ppstart = rel_pp = (arelent **) xmalloc (relsize);
-	      rel_count = bfd_canonicalize_reloc (abfd, section, rel_pp, syms);
-	      if (rel_count < 0)
-		bfd_fatal (bfd_get_filename (abfd));
-
-	      /* Sort the relocs by address.  */
-	      qsort (rel_pp, rel_count, sizeof (arelent *), compare_relocs);
-	    }
+	  /* Sort the relocs by address.  */
+	  qsort (rel_pp, rel_count, sizeof (arelent *), compare_relocs);
 	}
     }
-  rel_ppend = PTR_ADD (rel_pp, rel_count);
+
+  rel_ppend = rel_pp + rel_count;
 
   if (!bfd_malloc_and_get_section (abfd, section, &data))
     {
@@ -3557,6 +3957,264 @@ disassemble_section (bfd *abfd, asection
 
   printf (_("\nDisassembly of section %s:\n"), sanitize_string (section->name));
 
+#ifdef TARGET_AMIGA
+  if (create_labels &&  0 == strncmp(".text", section->name, 5))
+    {
+  /**
+   * Run over the data and create labels/symbols.
+   */
+  struct disassemble_info * dinf = (struct disassemble_info *)inf;
+  void (*tmp_print_address_func)
+    (bfd_vma addr, struct disassemble_info *dinfo) = dinf->print_address_func;
+  fprintf_ftype tmp_fprintf = dinf->fprintf_func;
+  arelent ** tmp_rel_pp = rel_pp;
+
+  dinf->print_address_func = create_label_address_func;
+  dinf->fprintf_func = dummy_fprintf;
+
+  // create a lookup map where insns start.
+  // do not create labels in a section having such a lookup map
+  //   if the label is not at insn start
+  lookup = (bfd_byte *)xmalloc(16 + (section->size + 15) / 16);
+
+  if (sorted_symcount == 0)
+    {
+      sorted_syms = (asymbol **)xmalloc(sizeof(asymbol*) * abfd->section_count);
+      asection * asec = abfd->sections;
+      while (sorted_symcount < abfd->section_count)
+	{
+	  // insert at least one symbol
+	  asymbol * nsym = (asymbol *)xmalloc(sizeof(asymbol) );
+
+	  // copy common and mark the flags
+	  nsym->value = 0;
+	  nsym->flags = 0;
+	  nsym->section = asec;
+	  nsym->the_bfd = abfd;
+	    nsym->udata.i = 0;
+
+	  // set vma
+	  nsym->name = asec->name;
+	  sorted_syms[sorted_symcount] = nsym;
+
+	  dinf->symtab_size = ++sorted_symcount;
+	  asec = asec->next;
+	}
+    }
+
+  int index;
+  asymbol * asym;
+  int seen, oldcount;
+  int pass = 0;
+  int guess = 0;
+  do
+    {
+      fprintf(stderr, "pass %d:", ++pass);
+      seen = 0;
+      oldcount = sorted_symcount;
+      for (index = 0; index < sorted_symcount; ++index)
+	{
+	  asym = sorted_syms[index];
+
+	  // do not search labels know to be data
+	  if (asym->udata.i || (asym->flags & (BSF_DATA|BSF_VISITED)))
+	      continue;
+	  // only search inside code
+	  if (strncmp(asym->section->name, ".text", 5))
+	    {
+	      asym->flags |= (BSF_DATA|BSF_VISITED);
+	      continue;
+	    }
+
+	  // skip lto stuff
+	  if (asym->name && strstr(asym->name, "lto_pri"))
+	    continue;
+
+//	  if ((asym->flags & BSF_CODE) == 0)
+	    {
+	      if (guess)
+		{
+		  bfd_vma pos = asym->value;
+		  unsigned short w = ((data[pos] & 0xff) << 8) | (data[pos + 1] & 0xff);
+		  switch (w)
+		  {
+		    case 0x23c8:
+		    case 0x2608:
+		    case 0x2f02:// move.l d2,-(sp)
+		    case 0x2f03:// move.l d3,-(sp)
+		    case 0x2f0a:// move.l a2,-(sp)
+		    case 0x2f0b:// move.l a3,-(sp)
+		    case 0x2f0e:// move.l a6,-(sp)
+		    case 0x2f2f:// move.l 4(sp),-(sp)
+		    case 0x45ec:
+		    case 0x45f9:
+		    case 0x48e7:// movem.l ...
+		    case 0x4e55:// link.w a5
+		    case 0x4e75:
+		    case 0x4eba:// jsr
+		    case 0x4fef:// lea x(sp),sp
+		    case 0x598f:// subq.l #4,a7
+		    case 0x70ff:
+		    case 0xc800:
+		      asym->flags |= BSF_CODE;
+		      guess = 0;
+		      break;
+		    default:
+		      asym->flags |= BSF_DATA;
+		      break;
+		  }
+		}
+		if ((asym->flags & BSF_CODE) == 0)
+		  continue;
+	    }
+
+          asym->flags |= BSF_VISITED;
+
+	  if (!(++seen & 127))
+	    fprintf(stderr, " %d/%ld", index, sorted_symcount);
+
+//	  fprintf(stderr, "at %0lx\n", asym->value);
+
+	  bfd_vma pos;
+	  for (pos = asym->value; pos < section->size;)
+	    {
+	      paux->vma = pos;
+
+	      // set the insn bit.
+	      int bit = 1 << (pos%15/2);
+	      lookup[(pos + 15) / 16] |= bit;
+
+	      bfd_vma add = (*paux->disassemble_fn) (pos, inf);
+
+	      if (add == 0)
+		break;
+
+
+	      // bra
+	      if (data[pos] == 0x60)
+		break;
+
+	      if (data[pos] == 0x4e &&
+		  ((data[pos + 1] & 0xc0) == 0xc0 // jmp
+		|| data[pos + 1] == 0x73 // rte
+		|| data[pos + 1] == 0x75 // rts
+		|| data[pos + 1] == 0x77 // rtr
+		))
+		{
+
+		  if (data[pos + 1] == 0xfb )
+		    {
+		      // add labels based on jump table
+		      bfd_vma base = pos + add;
+		      create_label(base, pinfo, LT_DATA);
+
+		      // search the limit
+		      unsigned limit = 0;
+		      int k;
+		      for (k = -4; k > -22; k -= 2)
+			{
+			  if ((data[pos + k] & 0xf0) == 0x70 && (data[pos + k] & 0x1) == 0)
+			    {
+			      //moveq
+			      limit = data[pos + k + 1];
+			      break;
+			    }
+
+			  if (data[pos + k] == 0x0c && (data[pos + k + 1] & 0xf0) == 0)
+			    {
+			      // cmp
+			      limit = (data[pos + k + 2] <<8) | data[pos + k + 3];
+			    }
+			}
+
+		      if (limit)
+			{
+			  limit += base + limit;
+
+			  bfd_vma entry = base;
+			  while(entry <= limit)
+			    {
+			      int jndex = find_closest_symbol_index(entry, section);
+			      asymbol * ssym = sorted_syms[jndex];
+			      if (entry > base && ssym->value == entry) // stop if there is a label
+				break;
+
+			      bfd_vma offset = ((char)data[entry] << 8) | data[entry + 1];
+			      if (offset&1)
+				break;
+//			      fprintf(stderr, "jump to %08lx\n", base + offset);
+			      bfd_vma at = base + offset;
+			      create_label(at, pinfo, LT_CODE);
+			      entry += 2;
+			    }
+			}
+		    } else
+		      {
+			if (data[pos + 2] == 0x4e && data[pos + 3] == 0x55)
+			  create_label(pos + 2, pinfo, LT_CODE);
+		      }
+		  pos += add;
+		  break;
+		}
+	      pos += add;
+	    }
+	  asym->udata.i = pos; // store end for this symbol
+	}
+
+      // create the symbol names and clear BSF_PENDING
+      bfd_vma end = 0;
+      for (index = 0; index < sorted_symcount; ++index)
+        {
+          asym = sorted_syms[index];
+          if (asym->value > end)
+            end = asym->value;
+          if (asym->udata.i > end)
+            end = asym->udata.i;
+          if (end > asym->value && end < asym->udata.i)
+            asym->udata.i = end;
+        }
+
+      // nothing changed start guessing one unknown label
+      if (!guess && seen == 0 && oldcount == sorted_symcount)
+	{
+	  guess = 1;
+	  continue;
+	}
+      guess = 0;
+      fprintf(stderr, " %d/%ld\n", seen, sorted_symcount);
+    }
+  while (guess || seen || oldcount != sorted_symcount);
+
+  for (index = 0; index < sorted_symcount; ++index)
+    {
+      static int n;
+      static int m;
+
+      asym = sorted_syms[index];
+      if (asym->name == 0)
+	{
+	  char lab[16];
+	  if (asym->flags & BSF_DATA)
+	    snprintf(lab, 32, ".D%d", ++m);
+	  else
+	    snprintf(lab, 32, ".L%d", ++n);
+	  char * name = (char *)xmalloc(16);
+	  strcpy(name, lab);
+	  asym->name = name;
+	} else if (asym->section == section)
+	  n = 0;
+    }
+
+
+  // restore the real print functions
+  dinf->print_address_func = tmp_print_address_func;
+  dinf->fprintf_func = tmp_fprintf;
+  rel_pp = tmp_rel_pp;
+
+  dinf->memory_error_func = dummy_error_func;
+    }
+#endif
+
   /* Find the nearest symbol forwards from our current position.  */
   paux->require_sec = true;
   sym = (asymbol *) find_symbol_for_address (section->vma + addr_offset,
@@ -3612,7 +4270,7 @@ disassemble_section (bfd *abfd, asection
 
       /* If we are only disassembling from a specific symbol,
 	 check to see if we should start or stop displaying.  */
-      if (sym && paux->symbol)
+      if (sym && (paux->symbol || create_labels))
 	{
 	  if (do_print)
 	    {
@@ -3747,6 +4405,12 @@ disassemble_section (bfd *abfd, asection
       else
 	insns = false;
 
+      if (strncmp(".text", pinfo->section->name, 5))
+	insns = true;
+
+      if (sym && (sym->flags & BSF_DATA))
+	insns = false;
+
       if (do_print)
 	{
 	  /* Resolve symbol name.  */
@@ -3833,10 +4497,12 @@ disassemble_data (bfd *abfd)
 			 (fprintf_styled_ftype) fprintf_styled);
   disasm_info.application_data = (void *) &aux;
   aux.abfd = abfd;
+  aux.sec = NULL;
   aux.require_sec = false;
   disasm_info.dynrelbuf = NULL;
   disasm_info.dynrelcount = 0;
   aux.reloc = NULL;
+  aux.relppp = NULL;
   aux.symbol = disasm_sym;
 
   disasm_info.print_address_func = objdump_print_address;
@@ -5569,7 +6235,7 @@ main (int argc, char **argv)
   set_default_bfd_target ();
 
   while ((c = getopt_long (argc, argv,
-			   "CDE:FGHI:LM:P:RSTU:VW::ab:defghij:lm:prstvwxz",
+			   "CDE:FGHI:NQLM:P:RSTU:VW::ab:defghij:lm:prstvwxz",
 			   long_options, (int *) 0))
 	 != EOF)
     {
@@ -5580,6 +6246,9 @@ main (int argc, char **argv)
 	case 'm':
 	  machine = optarg;
 	  break;
+	case 'Q':
+	  create_labels = true;
+	  break;
 	case 'M':
 	  {
 	    char *options;
@@ -5592,6 +6261,9 @@ main (int argc, char **argv)
 	    disassembler_options = remove_whitespace_and_extra_commas (options);
 	  }
 	  break;
+	case 'N':
+	  omit_offsets = true;
+	  break;
 	case 'j':
 	  add_only (optarg);
 	  break;


























--- binutils/rddbg.c	2022-07-08 10:46:47.000000000 +0100
+++ binutils/rddbg.c	2025-11-07 11:25:06.733427729 +0000
@@ -63,7 +63,7 @@ read_debugging_info (bfd *abfd, asymbol
 					      &found))
 	goto err_exit;
     }
-
+#ifndef _MSC_VER
   /* Try reading the COFF symbols if we didn't find any stabs in COFF
      sections.  */
   if (! found
@@ -74,7 +74,7 @@ read_debugging_info (bfd *abfd, asymbol
 	goto err_exit;
       found = true;
     }
-
+#endif
   if (! found)
     {
       if (! no_messages)

--- binutils/readelf.c	2022-07-08 10:46:47.000000000 +0100
+++ binutils/readelf.c	2025-11-07 11:25:06.741427849 +0000
@@ -79,6 +79,9 @@
 #include "elf/h8.h"
 #undef _ELF_H8_H
 
+int
+readelfmain (int argc, char ** argv);
+
 /* Undo the effects of #including reloc-macros.h.  */
 
 #undef START_RELOC_NUMBERS
@@ -214,7 +217,7 @@ static struct dump_data cmdline;
 
 static struct dump_list_entry * dump_sects_byname;
 
-char * program_name = "readelf";
+BINUTILSDECL char * program_name = "readelf";
 
 static bool show_name = false;
 static bool do_dynamic = false;
@@ -15202,7 +15205,7 @@ uncompress_section_contents (unsigned ch
   strm.next_in = (Bytef *) compressed_buffer;
   strm.avail_out = uncompressed_size;
   uncompressed_buffer = (unsigned char *) xmalloc (uncompressed_size);
-
+#ifndef _MSC_VER
   rc = inflateInit (& strm);
   while (strm.avail_in > 0)
     {
@@ -15226,6 +15229,7 @@ uncompress_section_contents (unsigned ch
 
  fail:
   free (uncompressed_buffer);
+#endif
   /* Indicate decompression failure.  */
   *buffer = NULL;
   return false;
@@ -22815,7 +22819,7 @@ db_task_printsym (unsigned int addr)
 #endif
 
 int
-main (int argc, char ** argv)
+readelfmain (int argc, char ** argv)
 {
   int err;
 

--- binutils/rename.c	2022-07-08 10:46:47.000000000 +0100
+++ binutils/rename.c	2025-11-07 11:25:06.741427849 +0000
@@ -182,7 +182,7 @@ set_times (const char *destination, cons
   tv[1].tv_sec = statbuf->st_mtime;
   tv[1].tv_usec = get_stat_mtime_ns (statbuf) / 1000;
   result = utimes (destination, tv);
-#elif defined HAVE_GOOD_UTIME_H
+#elif defined(HAVE_GOOD_UTIME_H) || defined(_MSC_VER)
   struct utimbuf tb;
 
   tb.actime = statbuf->st_atime;

--- binutils/stabs.c	2022-07-08 10:46:47.000000000 +0100
+++ binutils/stabs.c	2025-11-07 11:25:06.745427909 +0000
@@ -4198,7 +4198,7 @@ stab_demangle_qualified (struct stab_dem
 	++*pp;
       if (**pp == 't')
 	{
-	  char *name;
+	  char *name = "";
 
 	  if (! stab_demangle_template (minfo, pp,
 					ptype != NULL ? &name : NULL))
@@ -4648,7 +4648,7 @@ stab_demangle_arg (struct stab_demangle_
 		   unsigned int *palloc)
 {
   const char *start;
-  debug_type type;
+  debug_type type = DEBUG_TYPE_NULL;
 
   start = *pp;
   if (! stab_demangle_type (minfo, pp,
@@ -4772,7 +4772,7 @@ stab_demangle_type (struct stab_demangle
     case 'F':
       /* A function.  */
       {
-	debug_type *args;
+	debug_type *args = 0;
 	bool varargs;
 
 	++*pp;
@@ -5156,7 +5156,7 @@ stab_demangle_fund_type (struct stab_dem
 
     case 't':
       {
-	char *name;
+	char *name = "";
 
 	if (! stab_demangle_template (minfo, pp,
 				      ptype != NULL ? &name : NULL))

--- binutils/sysdep.h	2022-07-08 10:46:47.000000000 +0100
+++ binutils/sysdep.h	2025-11-07 11:25:06.745427909 +0000
@@ -20,6 +20,13 @@
 #ifndef _BIN_SYSDEP_H
 #define _BIN_SYSDEP_H
 
+#ifndef BINUTILSDECL
+#define BINUTILSDECL
+#endif
+#ifndef BFDDECL
+#define BFDDECL
+#endif
+
 #include "config.h"
 #include <stdio.h>
 #ifdef HAVE_SYS_TYPES_H






--- configure	2022-07-08 10:55:40.000000000 +0100
+++ configure	2025-11-07 11:25:06.797428688 +0000
@@ -756,6 +756,7 @@ infodir
 docdir
 oldincludedir
 includedir
+runstatedir
 localstatedir
 sharedstatedir
 sysconfdir
@@ -921,6 +922,7 @@ datadir='${datarootdir}'
 sysconfdir='${prefix}/etc'
 sharedstatedir='${prefix}/com'
 localstatedir='${prefix}/var'
+runstatedir='${localstatedir}/run'
 includedir='${prefix}/include'
 oldincludedir='/usr/include'
 docdir='${datarootdir}/doc/${PACKAGE}'
@@ -1173,6 +1175,15 @@ do
   | -silent | --silent | --silen | --sile | --sil)
     silent=yes ;;
 
+  -runstatedir | --runstatedir | --runstatedi | --runstated \
+  | --runstate | --runstat | --runsta | --runst | --runs \
+  | --run | --ru | --r)
+    ac_prev=runstatedir ;;
+  -runstatedir=* | --runstatedir=* | --runstatedi=* | --runstated=* \
+  | --runstate=* | --runstat=* | --runsta=* | --runst=* | --runs=* \
+  | --run=* | --ru=* | --r=*)
+    runstatedir=$ac_optarg ;;
+
   -sbindir | --sbindir | --sbindi | --sbind | --sbin | --sbi | --sb)
     ac_prev=sbindir ;;
   -sbindir=* | --sbindir=* | --sbindi=* | --sbind=* | --sbin=* \
@@ -1310,7 +1321,7 @@ fi
 for ac_var in	exec_prefix prefix bindir sbindir libexecdir datarootdir \
 		datadir sysconfdir sharedstatedir localstatedir includedir \
 		oldincludedir docdir infodir htmldir dvidir pdfdir psdir \
-		libdir localedir mandir
+		libdir localedir mandir runstatedir
 do
   eval ac_val=\$$ac_var
   # Remove trailing slashes.
@@ -1470,6 +1481,7 @@ Fine tuning of the installation director
   --sysconfdir=DIR        read-only single-machine data [PREFIX/etc]
   --sharedstatedir=DIR    modifiable architecture-independent data [PREFIX/com]
   --localstatedir=DIR     modifiable single-machine data [PREFIX/var]
+  --runstatedir=DIR       modifiable per-process data [LOCALSTATEDIR/run]
   --libdir=DIR            object code libraries [EPREFIX/lib]
   --includedir=DIR        C header files [PREFIX/include]
   --oldincludedir=DIR     C header files for non-gcc [/usr/include]
@@ -3879,6 +3891,9 @@ case "${target}" in
   rs6000-*-*)
     noconfigdirs="$noconfigdirs gprof"
     ;;
+  m68k-*-amiga*)
+    noconfigdirs="$noconfigdirs gprof"
+    ;;
   m68k-apollo-*)
     noconfigdirs="$noconfigdirs ld binutils gprof target-libgloss"
     ;;

--- configure.ac	2022-08-05 12:02:04.000000000 +0100
+++ configure.ac	2025-11-07 11:25:06.801428748 +0000
@@ -140,7 +140,7 @@ host_libs="intl libiberty opcodes bfd re
 # binutils, gas and ld appear in that order because it makes sense to run
 # "make check" in that particular order.
 # If --enable-gold is used, "gold" may replace "ld".
-host_tools="flex bison binutils gas ld fixincludes gcc cgen sid sim gdb gdbserver gprof etc expect dejagnu m4 utils guile fastjar gnattools libcc1 gotools"
+host_tools="texinfo flex bison binutils gas ld fixincludes gcc cgen sid sim gdb gdbserver gprof etc expect dejagnu m4 utils guile fastjar gnattools libcc1 gotools"
 
 # these libraries are built for the target environment, and are built after
 # the host libraries and the host tools (which may be a cross compiler)
@@ -1157,6 +1157,9 @@ case "${target}" in
   rs6000-*-*)
     noconfigdirs="$noconfigdirs gprof"
     ;;
+  m68k-*-amiga*)
+    noconfigdirs="$noconfigdirs gprof"
+    ;;
   m68k-apollo-*)
     noconfigdirs="$noconfigdirs ld binutils gprof target-libgloss"
     ;;














--- gas/app.c	2022-07-08 10:46:47.000000000 +0100
+++ gas/app.c	2025-11-07 11:25:06.857429587 +0000
@@ -1537,3 +1537,16 @@ do_scrub_chars (size_t (*get) (char *, s
     last_char = to[-1];
   return to - tostart;
 }
+
+/* Return amount of pending input.  */
+
+size_t
+do_scrub_pending (void)
+{
+  size_t len = 0;
+  if (saved_input)
+    len += saved_input_len;
+  if (state == -1)
+    len += strlen (out_string);
+  return len;
+}

--- gas/as.c	2022-07-08 10:46:47.000000000 +0100
+++ gas/as.c	2025-11-07 11:25:06.857429587 +0000
@@ -120,6 +120,16 @@ segT text_section;
 segT data_section;
 segT bss_section;
 
+#ifdef OBJ_AMIGAHUNK
+segT data_chip_section;
+segT data_fast_section;
+segT data_far_section;
+segT bss_chip_section;
+segT bss_fast_section;
+segT bss_far_section;
+#endif
+
+
 /* Name of listing file.  */
 static char *listing_filename = NULL;
 
@@ -1210,6 +1220,16 @@ perform_an_assembly_pass (int argc, char
   text_section = subseg_new (TEXT_SECTION_NAME, 0);
   data_section = subseg_new (DATA_SECTION_NAME, 0);
   bss_section = subseg_new (BSS_SECTION_NAME, 0);
+
+#ifdef OBJ_AMIGAHUNK
+  data_chip_section = subseg_new (".datachip", 0);
+  data_fast_section = subseg_new (".datafast", 0);
+  data_far_section = subseg_new (".datafar", 0);
+  bss_chip_section = subseg_new (".bsschip", 0);
+  bss_fast_section = subseg_new (".bssfast", 0);
+  bss_far_section = subseg_new (".bssfar", 0);
+#endif
+
   /* @@ FIXME -- we're setting the RELOC flag so that sections are assumed
      to have relocs, otherwise we don't find out in time.  */
   applicable = bfd_applicable_section_flags (stdoutput);
@@ -1220,6 +1240,19 @@ perform_an_assembly_pass (int argc, char
 			 applicable & (SEC_ALLOC | SEC_LOAD | SEC_RELOC
 				       | SEC_DATA));
   bfd_set_section_flags (bss_section, applicable & SEC_ALLOC);
+
+#ifdef OBJ_AMIGAHUNK
+  bfd_set_section_flags (data_chip_section,
+			 applicable & (SEC_ALLOC | SEC_LOAD | SEC_RELOC));
+  bfd_set_section_flags (data_fast_section,
+			 applicable & (SEC_ALLOC | SEC_LOAD | SEC_RELOC));
+  bfd_set_section_flags (data_far_section,
+			 applicable & (SEC_ALLOC | SEC_LOAD | SEC_RELOC));
+  bfd_set_section_flags (bss_chip_section, applicable & SEC_ALLOC);
+  bfd_set_section_flags (bss_fast_section, applicable & SEC_ALLOC);
+  bfd_set_section_flags (bss_far_section, applicable & SEC_ALLOC);
+#endif
+  
   seg_info (bss_section)->bss = 1;
 #endif
   subseg_new (BFD_ABS_SECTION_NAME, 0);

--- gas/as.h	2022-07-08 10:46:47.000000000 +0100
+++ gas/as.h	2025-11-07 11:25:06.857429587 +0000
@@ -460,6 +460,7 @@ void   input_scrub_insert_file (char *);
 char * input_scrub_new_file (const char *);
 char * input_scrub_next_buffer (char **bufp);
 size_t do_scrub_chars (size_t (*get) (char *, size_t), char *, size_t);
+size_t do_scrub_pending (void);
 bool   scan_for_multibyte_characters (const unsigned char *, const unsigned char *, bool);
 int    gen_to_words (LITTLENUM_TYPE *, int, long);
 int    had_err (void);








--- gas/config/m68k-parse.h	2022-07-08 10:46:47.000000000 +0100
+++ gas/config/m68k-parse.h	2025-11-07 11:25:06.865429707 +0000
@@ -61,6 +61,40 @@ enum m68k_register
   ADDR6,
   ADDR7,
 
+  DATA8,	/* additional data regs of the 68080. */
+  DATA9,
+  DATA10,
+  DATA11,
+  DATA12,
+  DATA13,
+  DATA14,
+  DATA15,
+  DATA16,
+  DATA17,
+  DATA18,
+  DATA19,
+  DATA20,
+  DATA21,
+  DATA22,
+  DATA23,
+  DATA24,
+  DATA25,
+  DATA26,
+  DATA27,
+  DATA28,
+  DATA29,
+  DATA30,
+  DATA31,
+
+  ADDR8,	/* additional address regs of the 68080. */
+  ADDR9,
+  ADDR10,
+  ADDR11,
+  ADDR12,
+  ADDR13,
+  ADDR14,
+  ADDR15,
+
   FP0,				/* Eight FP registers */
   FP1,
   FP2,
@@ -295,6 +329,9 @@ struct m68k_exp
 
   /* The expression itself.  */
   expressionS exp;
+
+  /* base-relative? */
+  short baserel;
 };
 
 /* The operand modes.  */
@@ -319,6 +356,10 @@ enum m68k_operand_type
   REGLST
 };
 
+enum m68080_bank {
+  BANK0, BANK1, BANK2, BANK3
+};
+
 /* The structure used to hold a parsed operand.  */
 
 struct m68k_op
@@ -329,6 +370,8 @@ struct m68k_op
   /* The main register.  */
   enum m68k_register reg;
 
+  enum m68080_bank bank;
+
   /* The register mask for mode REGLST.  */
   unsigned long mask;
 

--- gas/config/m68k-parse.y	2022-07-08 10:46:47.000000000 +0100
+++ gas/config/m68k-parse.y	2025-11-07 11:25:06.865429707 +0000
@@ -422,7 +422,7 @@ mit_operand:
 	  optzapc '@'
 		{
 		  /* We use optzapc to avoid a shift/reduce conflict.  */
-		  if ($1 < ADDR0 || $1 > ADDR7)
+		  if (($1 < ADDR0 || $1 > ADDR7) && ($1 < ADDR8 || $1 > ADDR15))
 		    yyerror (_("syntax error"));
 		  op->mode = AINDR;
 		  op->reg = $1;
@@ -430,7 +430,7 @@ mit_operand:
 	| optzapc '@' '+'
 		{
 		  /* We use optzapc to avoid a shift/reduce conflict.  */
-		  if ($1 < ADDR0 || $1 > ADDR7)
+		  if (($1 < ADDR0 || $1 > ADDR7) && ($1 < ADDR8 || $1 > ADDR15))
 		    yyerror (_("syntax error"));
 		  op->mode = AINC;
 		  op->reg = $1;
@@ -438,7 +438,7 @@ mit_operand:
 	| optzapc '@' '-'
 		{
 		  /* We use optzapc to avoid a shift/reduce conflict.  */
-		  if ($1 < ADDR0 || $1 > ADDR7)
+		  if (($1 < ADDR0 || $1 > ADDR7) && ($1 < ADDR8 || $1 > ADDR15))
 		    yyerror (_("syntax error"));
 		  op->mode = ADEC;
 		  op->reg = $1;
@@ -650,11 +650,11 @@ reglistpair:
 reglistreg:
 	  DR
 		{
-		  $$ = $1 - DATA0;
+		  $$ = $1 < DATA8 ? $1 - DATA0 : $1 - DATA8 + 256;
 		}
 	| AR
 		{
-		  $$ = $1 - ADDR0 + 8;
+		  $$ = $1 < ADDR8 ? $1 - ADDR0 + 8 : $1 - ADDR8 + 128 + 8;
 		}
 	| FPR
 		{
@@ -850,8 +850,12 @@ yylex (void)
 
       if (reg >= DATA0 && reg <= DATA7)
 	ret = DR;
+	  else if (reg >= DATA8 && reg <= DATA23)
+	ret = DR;	
       else if (reg >= ADDR0 && reg <= ADDR7)
 	ret = AR;
+      else if (reg >= ADDR8 && reg <= ADDR15)
+	ret = AR;
       else if (reg >= FP0 && reg <= FP7)
 	return FPR;
       else if (reg == FPI
@@ -862,7 +866,7 @@ yylex (void)
 	return LPC;
       else if (reg >= ZDATA0 && reg <= ZDATA7)
 	ret = ZDR;
-      else if (reg >= ZADDR0 && reg <= ZADDR7)
+      else if (reg >= ZADDR0 && reg <= ZADDR7) 
 	ret = ZAR;
       else if (reg == ZPC)
 	return LZPC;
@@ -971,6 +975,7 @@ yylex (void)
     }
 
   yylval.exp.size = SIZE_UNSPEC;
+  yylval.exp.baserel = 0;
   if (s <= str + 2
       || (s[-2] != '.' && s[-2] != ':'))
     tail = 0;
@@ -978,18 +983,21 @@ yylex (void)
     {
       switch (s[-1])
 	{
+	case 'B':
+          yylval.exp.baserel = 1;
 	case 's':
 	case 'S':
 	case 'b':
-	case 'B':
 	  yylval.exp.size = SIZE_BYTE;
 	  break;
-	case 'w':
 	case 'W':
+          yylval.exp.baserel = 1;
+	case 'w':
 	  yylval.exp.size = SIZE_WORD;
 	  break;
-	case 'l':
 	case 'L':
+          yylval.exp.baserel = 1;
+	case 'l':
 	  yylval.exp.size = SIZE_LONG;
 	  break;
 	default:
@@ -1089,6 +1097,7 @@ yylex (void)
 int
 m68k_ip_op (char *s, struct m68k_op *oparg)
 {
+  int r;
   memset (oparg, 0, sizeof *oparg);
   oparg->error = NULL;
   oparg->index.reg = ZDATA0;
@@ -1099,7 +1108,34 @@ m68k_ip_op (char *s, struct m68k_op *opa
   str = strorig = s;
   op = oparg;
 
-  return yyparse ();
+  r = yyparse ();
+  if (oparg->reg >= DATA8 && oparg->reg <= DATA31)
+    {
+      oparg->mode = DREG;
+      oparg->bank = (oparg->reg - DATA8 + 8) >> 3;
+      oparg->reg = ((oparg->reg - DATA8) & 7) + DATA0; 
+    }
+  else if (oparg->reg >= ADDR8 && oparg->reg <= ADDR15)
+    {
+      oparg->bank = BANK1;
+      oparg->reg = oparg->reg - ADDR8 + ADDR0;
+    }
+  if (oparg->index.reg >= DATA16 && oparg->index.reg <= DATA23)
+    {
+      oparg->error = "invalid index register, e8-e15 are not supported";
+      r = 1;
+    }
+  else if (oparg->index.reg >= DATA8 && oparg->index.reg <= DATA15)
+    {
+      oparg->bank |= BANK2;
+      oparg->index.reg += DATA0 - DATA8;
+    }
+  else if (oparg->index.reg >= ADDR8 && oparg->index.reg <= ADDR15)
+    {
+      oparg->bank |= BANK2;
+      oparg->index.reg += ADDR0 - ADDR8;
+    }
+  return r;
 }
 
 /* The error handler.  */



--- gas/config/obj-aout.c	2022-07-08 10:46:47.000000000 +0100
+++ gas/config/obj-aout.c	2025-11-07 11:25:06.865429707 +0000
@@ -24,6 +24,65 @@
 #undef NO_RELOC
 #include "aout/aout64.h"
 
+static void obj_section PARAMS((int));
+static char * obj_section_name PARAMS ((void));
+
+#ifndef BFD_ASSEMBLER
+/* Get name of section.  */
+static char *
+obj_section_name ()
+{
+  char *name;
+
+  SKIP_WHITESPACE ();
+  if (*input_line_pointer == '"')
+    {
+      int dummy;
+
+      name = demand_copy_C_string (&dummy);
+      if (name == NULL)
+	{
+	  ignore_rest_of_line ();
+	  return NULL;
+	}
+    }
+  else
+    {
+      char *end = input_line_pointer;
+
+      while (0 == strchr ("\n\t,; ", *end))
+	end++;
+      if (end == input_line_pointer)
+	{
+	  as_warn (_("missing name"));
+	  ignore_rest_of_line ();
+	  return NULL;
+	}
+
+      name = xmalloc (end - input_line_pointer + 1);
+      memcpy (name, input_line_pointer, end - input_line_pointer);
+      name[end - input_line_pointer] = '\0';
+      input_line_pointer = end;
+    }
+  SKIP_WHITESPACE ();
+  return name;
+}
+
+
+static void obj_section(int push) {
+	char const * name = obj_section_name();
+	if (name == NULL)
+		return;
+//printf("section: %s\r\n", name);
+	if (0 == strcmp(".rodata", name)
+//|| 0 == strncmp(".text", name, 5)
+	)
+	  s_text(push);
+	else
+	  s_data(push);
+}
+#endif
+
 void
 obj_aout_frob_symbol (symbolS *sym, int *punt ATTRIBUTE_UNUSED)
 {
@@ -350,6 +409,16 @@ const pseudo_typeS aout_pseudo_table[] =
   {"version", s_ignore, 0},
 
   {"optim", s_ignore, 0},	/* For sun386i cc (?).  */
+  {"2byte", cons, 2},
+  {"4byte", cons, 4},
+  {"8byte", cons, 8},
+  {"hidden", s_ignore, 0},
+  {"local", s_ignore, 0},
+  {"section", obj_section, 0},
+  {"section.s", obj_section, 0},
+  {"sect", obj_section, 0},
+  {"sect.s", obj_section, 0},
+  {"swbeg", s_ignore, 0},
 
   /* other stuff */
   {"ABORT", s_abort, 0},

--- gas/config/obj-elf.c	2022-07-08 10:46:47.000000000 +0100
+++ gas/config/obj-elf.c	2025-11-07 11:25:06.865429707 +0000
@@ -1865,7 +1865,7 @@ obj_elf_get_vtable_inherit (void)
   gas_assert (symbol_get_value_expression (csym)->X_op == O_constant);
   return fix_new (symbol_get_frag (csym),
 		  symbol_get_value_expression (csym)->X_add_number,
-		  0, psym, 0, 0, BFD_RELOC_VTABLE_INHERIT);
+		  0, psym, 0, 0, BFD_RELOC_VTABLE_INHERIT, 0);
 }
 
 /* This is a version of obj_elf_get_vtable_inherit() that is
@@ -1907,7 +1907,7 @@ obj_elf_get_vtable_entry (void)
   demand_empty_rest_of_line ();
 
   return fix_new (frag_now, frag_now_fix (), 0, sym, offset, 0,
-		  BFD_RELOC_VTABLE_ENTRY);
+		  BFD_RELOC_VTABLE_ENTRY, 0);
 }
 
 /* This is a version of obj_elf_get_vtable_entry() that is





--- gas/config/tc-arm.c	2022-07-08 10:46:47.000000000 +0100
+++ gas/config/tc-arm.c	2025-11-07 11:25:06.885430007 +0000
@@ -742,6 +742,7 @@ const char * const reg_expected_msgs[] =
 #define REG_SP	13
 #define REG_LR	14
 #define REG_PC	15
+#define REG_RA_AUTH_CODE 143
 
 /* ARM instructions take 4bytes in the object file, Thumb instructions
    take 2:  */
@@ -1943,21 +1944,6 @@ parse_reg_list (char ** strp, enum reg_l
 
 	      reg = arm_reg_parse (&str, rt);
 
-	      /* Skip over allowed registers of alternative types in mixed-type
-	         register lists.  */
-	      if (reg == FAIL && rt == REG_TYPE_PSEUDO
-		  && ((reg = arm_reg_parse (&str, REG_TYPE_RN)) != FAIL))
-		{
-		  cur_reg = reg;
-		  continue;
-		}
-	      else if (reg == FAIL && rt == REG_TYPE_RN
-		       && ((reg = arm_reg_parse (&str, REG_TYPE_PSEUDO)) != FAIL))
-		{
-		  cur_reg = reg;
-		  continue;
-		}
-
 	      if (etype == REGLIST_CLRM)
 		{
 		  if (reg == REG_SP || reg == REG_PC)
@@ -4139,7 +4125,6 @@ s_arm_unwind_fnstart (int ignored ATTRIB
   unwind.sp_restored = 0;
 }
 
-
 /* Parse a handlerdata directive.  Creates the exception handling table entry
    for the function.  */
 
@@ -4297,15 +4282,19 @@ s_arm_unwind_personality (int ignored AT
 /* Parse a directive saving pseudo registers.  */
 
 static void
-s_arm_unwind_save_pseudo (long range)
+s_arm_unwind_save_pseudo (int regno)
 {
   valueT op;
 
-  if (range & (1 << 12))
+  switch (regno)
     {
+    case REG_RA_AUTH_CODE:
       /* Opcode for restoring RA_AUTH_CODE.  */
       op = 0xb4;
       add_unwind_opcode (op, 1);
+      break;
+    default:
+      as_bad (_("Unknown register %d encountered\n"), regno);
     }
 }
 
@@ -4375,6 +4364,80 @@ s_arm_unwind_save_core (long range)
     }
 }
 
+/* Implement correct handling of .save lists enabling the split into
+sublists where necessary, while preserving correct sublist ordering.  */
+
+static void
+parse_dot_save (char **str_p, int prev_reg)
+{
+  long core_regs = 0;
+  int reg;
+  int in_range = 0;
+
+  if (**str_p == ',')
+    *str_p += 1;
+  if (**str_p == '}')
+    {
+      *str_p += 1;
+      return;
+    }
+
+  while ((reg = arm_reg_parse (str_p, REG_TYPE_RN)) != FAIL)
+    {
+      if (!in_range)
+	{
+	  if (core_regs & (1 << reg))
+	    as_tsktsk (_("Warning: duplicated register (r%d) in register list"),
+		       reg);
+	  else if (reg <= prev_reg)
+	    as_tsktsk (_("Warning: register list not in ascending order"));
+
+	  core_regs |= (1 << reg);
+	  prev_reg = reg;
+	  if (skip_past_char(str_p, '-') != FAIL)
+	    in_range = 1;
+	  else if (skip_past_comma(str_p) == FAIL)
+	    first_error (_("bad register list"));
+	}
+      else
+	{
+	  int i;
+	  if (reg <= prev_reg)
+	    first_error (_("bad range in register list"));
+	  for (i = prev_reg + 1; i <= reg; i++)
+	    {
+	      if (core_regs & (1 << i))
+		as_tsktsk (_("Warning: duplicated register (r%d) in register list"),
+			   i);
+	      else
+		core_regs |= 1 << i;
+	    }
+	  in_range = 0;
+	}
+    }
+  if (core_regs)
+    {
+      /* Higher register numbers go in higher memory addresses.  When splitting a list,
+	 right-most sublist should therefore be .saved first.  Use recursion for this.  */
+      parse_dot_save (str_p, reg);
+      /* We're back from recursion, so emit .save insn for sublist.  */
+      s_arm_unwind_save_core (core_regs);
+      return;
+    }
+  /* Handle pseudo-regs, under assumption these are emitted singly.  */
+  else if ((reg = arm_reg_parse (str_p, REG_TYPE_PSEUDO)) != FAIL)
+    {
+      /* Recurse for remainder of input.  Note: No assumption is made regarding which
+	 register in core register set holds pseudo-register.  It's not considered in
+	 ordering check beyond ensuring it's not sandwiched between 2 consecutive
+	 registers.  */
+      parse_dot_save (str_p, prev_reg + 1);
+      s_arm_unwind_save_pseudo (reg);
+      return;
+    }
+  else
+    as_bad (BAD_SYNTAX);
+}
 
 /* Parse a directive saving FPA registers.  */
 
@@ -4716,39 +4779,13 @@ s_arm_unwind_save_mmxwcg (void)
   ignore_rest_of_line ();
 }
 
-/* Convert range and mask_range into a sequence of s_arm_unwind_core
-   and s_arm_unwind_pseudo operations.  We assume that mask_range will
-   not have consecutive bits set, or that one operation per bit is
-   acceptable.  */
-
-static void
-s_arm_unwind_save_mixed (long range, long mask_range)
-{
-  while (mask_range)
-    {
-      long mask_bit = mask_range & -mask_range;
-      long subrange = range & (mask_bit - 1);
-
-      if (subrange)
-	s_arm_unwind_save_core (subrange);
-
-      s_arm_unwind_save_pseudo (mask_bit);
-      range &= ~subrange;
-      mask_range &= ~mask_bit;
-    }
-
-  if (range)
-    s_arm_unwind_save_core (range);
-}
-
 /* Parse an unwind_save directive.
    If the argument is non-zero, this is a .vsave directive.  */
 
 static void
 s_arm_unwind_save (int arch_v6)
 {
-  char *peek, *mask_peek;
-  long range, mask_range;
+  char *peek;
   struct reg_entry *reg;
   bool had_brace = false;
 
@@ -4756,7 +4793,7 @@ s_arm_unwind_save (int arch_v6)
     as_bad (MISSING_FNSTART);
 
   /* Figure out what sort of save we have.  */
-  peek = mask_peek = input_line_pointer;
+  peek = input_line_pointer;
 
   if (*peek == '{')
     {
@@ -4788,20 +4825,13 @@ s_arm_unwind_save (int arch_v6)
 
     case REG_TYPE_PSEUDO:
     case REG_TYPE_RN:
-      mask_range = parse_reg_list (&mask_peek, REGLIST_PSEUDO);
-      range = parse_reg_list (&input_line_pointer, REGLIST_RN);
-
-      if (range == FAIL || mask_range == FAIL)
-	{
-	  as_bad (_("expected register list"));
-	  ignore_rest_of_line ();
-	  return;
-	}
-
-      demand_empty_rest_of_line ();
-
-      s_arm_unwind_save_mixed (range, mask_range);
-      return;
+      {
+	if (had_brace)
+	  input_line_pointer++;
+	parse_dot_save (&input_line_pointer, -1);
+	demand_empty_rest_of_line ();
+	return;
+      }
 
     case REG_TYPE_VFD:
       if (arch_v6)
@@ -23993,12 +24023,8 @@ static const struct reg_entry reg_names[
   /* XScale accumulator registers.  */
   REGNUM(acc,0,XSCALE), REGNUM(ACC,0,XSCALE),
 
-  /* DWARF ABI defines RA_AUTH_CODE to 143. It also reserves 134-142 for future
-     expansion.  RA_AUTH_CODE here is given the value 143 % 134 to make it easy
-     for tc_arm_regname_to_dw2regnum to translate to DWARF reg number using
-     134 + reg_number should the range 134 to 142 be used for more pseudo regs
-     in the future.  This also helps fit RA_AUTH_CODE into a bitmask.  */
-  REGDEF(ra_auth_code,12,PSEUDO),
+  /* AADWARF32 defines RA_AUTH_CODE to 143.  */
+  REGDEF(ra_auth_code,143,PSEUDO),
 };
 #undef REGDEF
 #undef REGNUM
@@ -27905,7 +27931,6 @@ create_unwind_entry (int have_data)
   return 0;
 }
 
-
 /* Initialize the DWARF-2 unwind information for this procedure.  */
 
 void

--- gas/config/tc-i386.c	2022-07-08 10:46:47.000000000 +0100
+++ gas/config/tc-i386.c	2025-11-07 11:25:06.901430247 +0000
@@ -5925,7 +5925,7 @@ optimize_disp (void)
 		 || i.reloc[op] == BFD_RELOC_X86_64_TLSDESC_CALL)
 	  {
 	    fix_new_exp (frag_now, frag_more (0) - frag_now->fr_literal, 0,
-			 i.op[op].disps, 0, i.reloc[op]);
+			 i.op[op].disps, 0, i.reloc[op], 0);
 	    i.types[op] = operand_type_and_not (i.types[op], anydisp);
 	  }
  	else
@@ -8927,7 +8927,7 @@ output_jump (void)
   jump_reloc = reloc (size, 1, 1, jump_reloc);
 
   fixP = fix_new_exp (frag_now, p - frag_now->fr_literal, size,
-		      i.op[0].disps, 1, jump_reloc);
+		      i.op[0].disps, 1, jump_reloc, 0);
 
   /* All jumps handled here are signed, but don't unconditionally use a
      signed limit check for 32 and 16 bit jumps as we want to allow wrap
@@ -9011,14 +9011,14 @@ output_interseg_jump (void)
     }
   else
     fix_new_exp (frag_now, p - frag_now->fr_literal, size,
-		 i.op[1].imms, 0, reloc (size, 0, 0, i.reloc[1]));
+		 i.op[1].imms, 0, reloc (size, 0, 0, i.reloc[1]), 0);
 
   p += size;
   if (i.op[0].imms->X_op == O_constant)
     md_number_to_chars (p, (valueT) i.op[0].imms->X_add_number, 2);
   else
     fix_new_exp (frag_now, p - frag_now->fr_literal, 2,
-		 i.op[0].imms, 0, reloc (2, 0, 0, i.reloc[0]));
+		 i.op[0].imms, 0, reloc (2, 0, 0, i.reloc[0]), 0);
 }
 
 #if defined (OBJ_ELF) || defined (OBJ_MAYBE_ELF)
@@ -10048,7 +10048,7 @@ output_disp (fragS *insn_start_frag, off
 		}
 	      fixP = fix_new_exp (frag_now, p - frag_now->fr_literal,
 				  size, i.op[n].disps, pcrel,
-				  reloc_type);
+				  reloc_type, 0);
 
 	      if (flag_code == CODE_64BIT && size == 4 && pcrel
 		  && !i.prefix[ADDR_PREFIX])
@@ -10196,7 +10196,7 @@ output_imm (fragS *insn_start_frag, offs
 		    encoding_length (insn_start_frag, insn_start_off, p);
 		}
 	      fix_new_exp (frag_now, p - frag_now->fr_literal, size,
-			   i.op[n].imms, 0, reloc_type);
+			   i.op[n].imms, 0, reloc_type, 0);
 	    }
 	}
     }
@@ -10222,7 +10222,7 @@ x86_cons_fix_new (fragS *frag, unsigned
     r = BFD_RELOC_16_SECIDX;
 #endif
 
-  fix_new_exp (frag, off, len, exp, 0, r);
+  fix_new_exp (frag, off, len, exp, 0, r, 0);
 }
 
 /* Export the ABI address size for use by TC_ADDRESS_BYTES for the
@@ -12299,7 +12299,7 @@ md_estimate_size_before_relax (fragS *fr
 	  fixP = fix_new (fragP, old_fr_fix, size,
 			  fragP->fr_symbol,
 			  fragP->fr_offset, 1,
-			  reloc_type);
+			  reloc_type, 0);
 	  break;
 
 	case COND_JUMP86:
@@ -12318,7 +12318,7 @@ md_estimate_size_before_relax (fragS *fr
 	      fix_new (fragP, old_fr_fix + 2, 2,
 		       fragP->fr_symbol,
 		       fragP->fr_offset, 1,
-		       reloc_type);
+		       reloc_type, 0);
 	      break;
 	    }
 	  /* Fall through.  */
@@ -12330,7 +12330,7 @@ md_estimate_size_before_relax (fragS *fr
 	      fixP = fix_new (fragP, old_fr_fix, 1,
 			      fragP->fr_symbol,
 			      fragP->fr_offset, 1,
-			      BFD_RELOC_8_PCREL);
+			      BFD_RELOC_8_PCREL, 0);
 	      fixP->fx_signed = 1;
 	      break;
 	    }
@@ -12344,7 +12344,7 @@ md_estimate_size_before_relax (fragS *fr
 	  fixP = fix_new (fragP, old_fr_fix + 1, size,
 			  fragP->fr_symbol,
 			  fragP->fr_offset, 1,
-			  reloc_type);
+			  reloc_type, 0);
 	  break;
 
 	default:

--- gas/config/tc-m68k.c	2022-07-08 10:46:47.000000000 +0100
+++ gas/config/tc-m68k.c	2025-11-07 11:25:06.909430367 +0000
@@ -27,11 +27,22 @@
 
 #include "opcode/m68k.h"
 #include "m68k-parse.h"
+
+#if defined (OBJ_ELF)
 #include "elf/m68k.h"
 
 static void m68k_elf_cons (int);
 static void m68k_elf_gnu_attribute (int);
 
+#endif
+
+#ifdef TE_AMIGA
+extern const bfd_target amiga_vec;
+#endif
+long
+md_pcrel_from_m68k (fixS *fixP, segT current_section);
+
+
 /* This string holds the chars that always start a comment.  If the
    pre-processor is disabled, these aren't very useful.  The macro
    tc_comment_chars points to this.  We use this, rather than the
@@ -74,6 +85,9 @@ int flag_want_pic;
 static int flag_short_refs;	/* -l option.  */
 static int flag_long_jumps;	/* -S option.  */
 static int flag_keep_pcrel;	/* --pcrel option.  */
+static int flag_small_code;	/* -sc option */
+
+#define FLAG_SMALL_CODE (0 && flag_small_code)
 
 #ifdef REGISTER_PREFIX_OPTIONAL
 int flag_reg_prefix_optional = REGISTER_PREFIX_OPTIONAL;
@@ -330,6 +344,8 @@ struct m68k_it
   int numo;			/* Number of shorts in opcode.  */
   short opcode[11];
 
+  short bank;
+
   struct m68k_op operands[6];
 
   int nexp;			/* Number of exprs in use.  */
@@ -360,9 +376,12 @@ struct m68k_it
 	 significance of some values (in the branch instruction, for
 	 example).  */
       int pcrel_fix;
+#ifdef OBJ_ELF
       /* Whether this expression needs special pic relocation, and if
 	 so, which.  */
       enum pic_relocation pic_reloc;
+#endif
+      char baserel;
     }
   reloc[5];			/* Five is enough???  */
 };
@@ -375,13 +394,13 @@ struct m68k_it
 
 /* Macros for determining if cpu supports a specific addressing mode.  */
 #define HAVE_LONG_DISP(x)	\
-	((x) & (m68020|m68030|m68040|m68060|cpu32|fido_a|mcfisa_b|mcfisa_c))
+	((x) & (m68020|m68030|m68040|m68060|m68080|cpu32|fido_a|mcfisa_b|mcfisa_c))
 #define HAVE_LONG_CALL(x)	\
-	((x) & (m68020|m68030|m68040|m68060|cpu32|fido_a|mcfisa_b|mcfisa_c))
+	((x) & (m68020|m68030|m68040|m68060|m68080|cpu32|fido_a|mcfisa_b|mcfisa_c))
 #define HAVE_LONG_COND(x)	\
-	((x) & (m68020|m68030|m68040|m68060|cpu32|fido_a|mcfisa_b|mcfisa_c))
+	((x) & (m68020|m68030|m68040|m68060|m68080|cpu32|fido_a|mcfisa_b|mcfisa_c))
 #define HAVE_LONG_BRANCH(x)	\
-	((x) & (m68020|m68030|m68040|m68060|cpu32|fido_a|mcfisa_b))
+	((x) & (m68020|m68030|m68040|m68060|m68080|cpu32|fido_a|mcfisa_b))
 #define LONG_BRANCH_VIA_COND(x) (HAVE_LONG_COND(x) && !HAVE_LONG_BRANCH(x))
 
 static struct m68k_it the_ins;	/* The instruction being assembled.  */
@@ -413,7 +432,7 @@ insop (int w, const struct m68k_incant *
 /* The numo+1 kludge is so we can hit the low order byte of the prev word.
    Blecch.  */
 static void
-add_fix (int width, struct m68k_exp *exp, int pc_rel, int pc_fix)
+add_fix (int width, struct m68k_exp *exp, int pc_rel, int pc_fix, int base_rel)
 {
   the_ins.reloc[the_ins.nrel].n = (width == 'B' || width == '3'
 				   ? the_ins.numo * 2 - 1
@@ -423,8 +442,11 @@ add_fix (int width, struct m68k_exp *exp
   the_ins.reloc[the_ins.nrel].exp = exp->exp;
   the_ins.reloc[the_ins.nrel].wid = width;
   the_ins.reloc[the_ins.nrel].pcrel_fix = pc_fix;
+#ifdef OBJ_ELF
   the_ins.reloc[the_ins.nrel].pic_reloc = exp->pic_reloc;
-  the_ins.reloc[the_ins.nrel++].pcrel = pc_rel;
+#endif  
+  the_ins.reloc[the_ins.nrel].pcrel = pc_rel;
+  the_ins.reloc[the_ins.nrel++].baserel = base_rel;
 }
 
 /* Cause an extra frag to be generated here, inserting up to 10 bytes
@@ -509,6 +531,7 @@ static const struct m68k_cpu m68k_archs[
   {m68030|m68881|m68851,			m68020_ctrl, "68030", 0},
   {m68040,					m68040_ctrl, "68040", 0},
   {m68060,					m68060_ctrl, "68060", 0},
+  {m68080,					m68060_ctrl, "68080", 0},
   {cpu32|m68881,				cpu32_ctrl, "cpu32", 0},
   {fido_a,					fido_ctrl, "fidoa", 0},
   {mcfisa_a|mcfhwdiv,				NULL, "isaa", 0},
@@ -564,6 +587,7 @@ static const struct m68k_cpu m68k_cpus[]
   {m68040,					m68040_ctrl, "68ec040", 1},
   {m68060,					m68060_ctrl, "68060", 0},
   {m68060,					m68060_ctrl, "68ec060", 1},
+  {m68080,					m68060_ctrl, "68080", 0},
 
   {cpu32|m68881,				cpu32_ctrl, "cpu32",  0},
   {cpu32|m68881,				cpu32_ctrl, "68330", 1},
@@ -775,9 +799,21 @@ static void m68k_init_arch (void);
 #define PCINDEX		8	/* PC + displacement + index. */
 #define ABSTOPCREL	9	/* Absolute relax down to 16-bit PC-relative.  */
 
+/* ABSREL (nice name;-)) is used in small-code, it might be implemented
+ * base-relative (a4), pc-relative, or base-relative with an extra add
+ * instruction to add the base-register.
+ *
+ * IMMREL is the analogous mode for immediate addressing of variables. This
+ * one can lead into situations, where a replacement is not possible:
+ *   addl #foo,a0
+ * can't be made pc-relative, if foo is in the text segment.  */
+
+#define ABSREL		10
+#define IMMREL		11
+
 /* This relaxation is required for branches where there is no long
    branch and we are in pcrel mode.  We generate a bne/beq pair.  */
-#define BRANCHBWPL	10      /* Branch byte, word or pair of longs
+#define BRANCHBWPL	12      /* Branch byte, word or pair of longs
 				   */
 
 /* Note that calls to frag_var need to specify the maximum expansion
@@ -847,6 +883,16 @@ relax_typeS md_relax_table[] =
   { 32767, -32768,  2, TAB (BRANCHBWPL, LONG) },
   {     0,	0,  10, 0 },
   {     1,	1,  0, 0 },
+  
+  {   127,   -128,  0, 0 },
+  { 32767, -32768,  2, TAB (ABSREL, LONG) },
+  {     0,      0,  6, 0 },
+  {     1,      1,  0, 0 },
+
+  {   127,   -128,  0, 0 },
+  { 32767, -32768,  2, TAB (IMMREL, LONG) },
+  {     0,      0,  6, 0 },
+  {     1,      1,  0, 0 },
 };
 
 /* These are the machine dependent pseudo-ops.  These are included so
@@ -866,15 +912,17 @@ const pseudo_typeS md_pseudo_table[] =
   {"even", s_even, 0},
   {"skip", s_space, 0},
   {"proc", s_proc, 0},
+#ifdef OBJ_ELF
   {"align", s_align_bytes, 0},
   {"swbeg", s_ignore, 0},
   {"long", m68k_elf_cons, 4},
+  {"gnu_attribute", m68k_elf_gnu_attribute, 0},
+#endif
   {"extend", float_cons, 'x'},
   {"ldouble", float_cons, 'x'},
 
   {"arch", s_m68k_arch, 0},
   {"cpu", s_m68k_cpu, 0},
-  {"gnu_attribute", m68k_elf_gnu_attribute, 0},
 
   /* The following pseudo-ops are supported for MRI compatibility.  */
   {"chip", s_chip, 0},
@@ -936,7 +984,11 @@ const pseudo_typeS mote_pseudo_table[] =
   {"dsb", s_space, 1},
 
   {"xdef", s_globl, 0},
+#ifdef OBJ_ELF
   {"align", s_align_bytes, 0},
+#else
+  {"align", s_align_ptwo, 0},
+#endif
   {0, 0, 0}
 };
 
@@ -961,6 +1013,7 @@ static char alt_notend_table[256];
       || (*s == ':'						\
 	  && alt_notend_table[(unsigned char) s[1]])))
 
+#ifdef OBJ_ELF
 
 /* Return zero if the reference to SYMBOL from within the same segment may
    be relaxed.  */
@@ -1186,6 +1239,16 @@ tc_m68k_fix_adjustable (fixS *fixP)
     }
 }
 
+#else /* !OBJ_ELF */
+
+#define get_reloc_code(SIZE,PCREL,OTHER) NO_RELOC
+
+/* PR gas/3041 Weak symbols are not relaxable
+   because they must be treated as extern.  */
+#define relaxable_symbol(symbol)   (!(S_IS_WEAK (symbol)))
+
+#endif /* OBJ_ELF */
+
 arelent *
 tc_gen_reloc (asection *section ATTRIBUTE_UNUSED, fixS *fixp)
 {
@@ -1265,6 +1328,17 @@ tc_gen_reloc (asection *section ATTRIBUT
 	    }
 	}
     }
+  else if (fixp->tc_fix_data)
+    {
+      switch (fixp->fx_size)
+	{
+	case 1: code = BFD_RELOC_8_BASEREL; break;
+	case 2: code = BFD_RELOC_16_BASEREL; break;
+	case 4: code = BFD_RELOC_32_BASEREL; break;
+	default:
+	  abort ();
+	}
+    }
   else
     {
 #define F(SZ,PCREL)		(((SZ) << 1) + (PCREL))
@@ -1288,13 +1362,59 @@ tc_gen_reloc (asection *section ATTRIBUT
   reloc->sym_ptr_ptr = XNEW (asymbol *);
   *reloc->sym_ptr_ptr = symbol_get_bfdsym (fixp->fx_addsy);
   reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
+#ifndef OBJ_ELF
+  if (OUTPUT_FLAVOR == bfd_target_aout_flavour
+      && fixp->fx_addsy
+      && S_IS_WEAK (fixp->fx_addsy)
+      && ! bfd_is_und_section (S_GET_SEGMENT (fixp->fx_addsy)))
+    {
+      /* PR gas/3041 References to weak symbols must be treated as extern
+	 in order to be overridable by the linker, even if they are defined
+	 in the same object file. So the original addend must be written
+	 "as is" into the output section without further processing.
+	 The addend value must be hacked here in order to force
+	 bfd_install_relocation() to write the original value into the
+	 output section.
+	 1) MD_APPLY_SYM_VALUE() is set to 1 for m68k/a.out, so the symbol
+	 value has already been added to the addend in fixup_segment(). We
+	 have to remove it.
+	 2) bfd_install_relocation() will incorrectly treat this symbol as
+	 resolved, so it will write the symbol value plus its addend and
+	 section VMA. As a workaround we can tweak the addend value here in
+	 order to get the original value in the section after the call to
+	 bfd_install_relocation().  */
+      reloc->addend = fixp->fx_addnumber
+		      /* Fix because of MD_APPLY_SYM_VALUE() */
+		      - S_GET_VALUE (fixp->fx_addsy)
+		      /* Fix for bfd_install_relocation() */
+		      - (S_GET_VALUE (fixp->fx_addsy)
+			 + S_GET_SEGMENT (fixp->fx_addsy)->vma);
+    }
+  else if (OUTPUT_FLAVOR == bfd_target_amiga_flavour
+      && fixp->fx_addsy
+      && S_IS_WEAK (fixp->fx_addsy)
+      && ! bfd_is_und_section (S_GET_SEGMENT (fixp->fx_addsy)))
+    {
+      // similar fix for amigavec
+      reloc->addend = fixp->fx_addnumber - S_GET_VALUE (fixp->fx_addsy);
+      if (fixp->fx_pcrel)
+	reloc->addend += reloc->address;
+    }
+  else if (fixp->fx_pcrel)
+    reloc->addend = fixp->fx_addnumber;
+  else
+    reloc->addend = 0;
+#else
   if (!fixp->fx_pcrel)
     reloc->addend = fixp->fx_addnumber;
   else
     reloc->addend = (section->vma
-		     + fixp->fx_pcrel_adjust
+		     /* Explicit sign extension in case char is
+			unsigned.  */
+		     + ((fixp->fx_pcrel_adjust & 0xff) ^ 0x80) - 0x80
 		     + fixp->fx_addnumber
-		     + md_pcrel_from (fixp));
+		     + md_pcrel_from_m68k (fixp, section));
+#endif
 
   reloc->howto = bfd_reloc_type_lookup (stdoutput, code);
   gas_assert (reloc->howto != 0);
@@ -1815,8 +1935,8 @@ m68k_ip (char *instring)
 		case '4':
 		  if (opP->mode != AINDR && opP->mode != AINC && opP->mode != ADEC
 		      && (opP->mode != DISP
-			   || opP->reg < ADDR0
-			   || opP->reg > ADDR7))
+			   || (!(opP->reg >= ADDR0 && opP->reg <= ADDR7) &&
+			       !(opP->reg >= ADDR8 && opP->reg <= ADDR15))))
 		    ++losing;
 		  break;
 
@@ -1852,8 +1972,8 @@ m68k_ip (char *instring)
 
 		case 'd':
 		  if (opP->mode != DISP
-		      || opP->reg < ADDR0
-		      || opP->reg > ADDR7)
+			   || (!(opP->reg >= ADDR0 && opP->reg <= ADDR7) &&
+			       !(opP->reg >= ADDR8 && opP->reg <= ADDR15)))
 		    losing++;
 		  break;
 
@@ -2380,6 +2500,22 @@ m68k_ip (char *instring)
   the_ins.opcode[0] = getone (opcode);
   the_ins.opcode[1] = gettwo (opcode);
 
+  {
+    // 68080 support
+    int k;
+    int bank = 0xf7100;
+    for (k = 0, opP = &the_ins.operands[0]; k < opsfound; ++k, ++opP) {
+	bank |= (int)opP->bank << (2 - k*2);
+    }
+    if (bank != 0xf7100)
+      {
+	/* For a single operand instruction, both AA and BB should be the same. */
+	if (opsfound == 1)
+	  bank |= (bank >> 2) & 3;
+	the_ins.bank = bank;
+      } else
+	the_ins.bank = 0;
+
   for (s = the_ins.args, opP = &the_ins.operands[0]; *s; s += 2, opP++)
     {
       int have_disp = 0;
@@ -2425,13 +2561,24 @@ m68k_ip (char *instring)
 	      else
 		nextword = get_num (&opP->disp, 0);
 	      if (isvar (&opP->disp))
-		add_fix (s[1], &opP->disp, 0, 0);
+		{
+/* This doesn't work when the symbol is N_UNDF! We ignore this for now. */
+		  if (FLAG_SMALL_CODE)
+		    {
+		      add_frag (adds (&opP->disp),
+				offs (&opP->disp),
+				TAB (IMMREL, SZ_UNDEF));
+		       break;
+		    }
+		  else
+		    add_fix(s[1], &opP->disp, 0, 0, opP->disp.baserel);
+	        }
 	      switch (s[1])
 		{
 		case 'b':
 		  if (!isbyte (nextword))
 		    opP->error = _("operand out of range");
-		  addword (nextword);
+		  addword (nextword & 0xff); /* G.Nikl: clear upper byte */
 		  baseo = 0;
 		  break;
 		case 'w':
@@ -2461,7 +2608,7 @@ m68k_ip (char *instring)
 		  outro = 11;
 		  break;
 		case 'x':
-		  baseo = 6;
+		  baseo = 5;
 		  outro = 15;
 		  break;
 		case 'p':
@@ -2586,12 +2733,15 @@ m68k_ip (char *instring)
 		      if (opP->reg == PC)
 			{
 			  if (opP->disp.size == SIZE_LONG
+#ifdef OBJ_ELF
 			      /* If the displacement needs pic
 				 relocation it cannot be relaxed.  */
-			      || opP->disp.pic_reloc != pic_none)
+			      || opP->disp.pic_reloc != pic_none
+#endif
+			      )
 			    {
 			      addword (0x0170);
-			      add_fix ('l', &opP->disp, 1, 2);
+			      add_fix ('l', &opP->disp, 1, 2, opP->disp.baserel);
 			    }
 			  else
 			    {
@@ -2604,7 +2754,7 @@ m68k_ip (char *instring)
 		      else
 			{
 			  addword (0x0170);
-			  add_fix ('l', &opP->disp, 0, 0);
+			  add_fix ('l', &opP->disp, 0, 0, opP->disp.baserel);
 			}
 		    }
 		  else
@@ -2622,10 +2772,10 @@ m68k_ip (char *instring)
 		    {
 		      if (opP->reg == PC)
 			{
-			  add_fix ('w', &opP->disp, 1, 0);
+			  add_fix ('w', &opP->disp, 1, 0, opP->disp.baserel);
 			}
 		      else
-			add_fix ('w', &opP->disp, 0, 0);
+			add_fix ('w', &opP->disp, 0, 0, opP->disp.baserel);
 		    }
 		}
 	      addword (nextword);
@@ -2735,9 +2885,9 @@ m68k_ip (char *instring)
 				 fit (possible on m68000) let the
 				 fixup processing complain later.  */
 			      if (opP->reg == PC)
-				add_fix ('B', &opP->disp, 1, 1);
+				add_fix ('B', &opP->disp, 1, 1, opP->disp.baserel);
 			      else
-				add_fix ('B', &opP->disp, 0, 0);
+				add_fix ('B', &opP->disp, 0, 0, opP->disp.baserel);
 			    }
 			  else if (siz1 != SIZE_BYTE)
 			    {
@@ -2753,9 +2903,12 @@ m68k_ip (char *instring)
 			       && opP->reg == PC
 			       && isvar (&opP->disp)
 			       && subs (&opP->disp) == NULL
+#ifdef OBJ_ELF
 			       /* If the displacement needs pic
 				  relocation it cannot be relaxed.  */
-			       && opP->disp.pic_reloc == pic_none)
+			       && opP->disp.pic_reloc == pic_none
+#endif
+			       )
 			{
 			  /* The code in md_convert_frag_1 needs to be
                              able to adjust nextword.  Call frag_grow
@@ -2865,9 +3018,9 @@ m68k_ip (char *instring)
 	      if (siz1 != SIZE_UNSPEC && isvar (&opP->disp))
 		{
 		  if (opP->reg == PC || opP->reg == ZPC)
-		    add_fix (siz1 == SIZE_LONG ? 'l' : 'w', &opP->disp, 1, 2);
+		    add_fix (siz1 == SIZE_LONG ? 'l' : 'w', &opP->disp, 1, 2, opP->disp.baserel);
 		  else
-		    add_fix (siz1 == SIZE_LONG ? 'l' : 'w', &opP->disp, 0, 0);
+		    add_fix (siz1 == SIZE_LONG ? 'l' : 'w', &opP->disp, 0, 0, opP->disp.baserel);
 		}
 	      if (siz1 == SIZE_LONG)
 		addword (baseo >> 16);
@@ -2875,7 +3028,7 @@ m68k_ip (char *instring)
 		addword (baseo);
 
 	      if (siz2 != SIZE_UNSPEC && isvar (&opP->odisp))
-		add_fix (siz2 == SIZE_LONG ? 'l' : 'w', &opP->odisp, 0, 0);
+		add_fix (siz2 == SIZE_LONG ? 'l' : 'w', &opP->odisp, 0, 0, opP->odisp.baserel);
 	      if (siz2 == SIZE_LONG)
 		addword (outro >> 16);
 	      if (siz2 != SIZE_UNSPEC)
@@ -2899,9 +3052,11 @@ m68k_ip (char *instring)
 		  if (isvar (&opP->disp)
 		      && !subs (&opP->disp)
 		      && adds (&opP->disp)
+#ifdef OBJ_ELF
 		      /* If the displacement needs pic relocation it
 			 cannot be relaxed.  */
 		      && opP->disp.pic_reloc == pic_none
+#endif
 		      && !flag_long_jumps
 		      && !strchr ("~%&$?", s[0]))
 		    {
@@ -2913,8 +3068,17 @@ m68k_ip (char *instring)
 		    }
 		  /* Fall through.  */
 		case SIZE_LONG:
+/* This doesn't work when the symbol is N_UNDF! We ignore this for now. */
+		  if (FLAG_SMALL_CODE)
+		    {
+		      tmpreg=0x3A; /* 7.2 */
+		      add_frag (adds (&opP->disp),
+			        offs (&opP->disp),
+				TAB (ABSREL, SZ_UNDEF));
+		      break;
+		    }
 		  if (isvar (&opP->disp))
-		    add_fix ('l', &opP->disp, 0, 0);
+		    add_fix ('l', &opP->disp, 0, 0, opP->disp.baserel);
 
 		  tmpreg = 0x39;/* 7.1 mode */
 		  addword (nextword >> 16);
@@ -2927,7 +3091,7 @@ m68k_ip (char *instring)
 
 		case SIZE_WORD:
 		  if (isvar (&opP->disp))
-		    add_fix ('w', &opP->disp, 0, 0);
+		    add_fix ('w', &opP->disp, 0, 0, opP->disp.baserel);
 
 		  tmpreg = 0x38;/* 7.0 mode */
 		  addword (nextword);
@@ -2973,7 +3137,7 @@ m68k_ip (char *instring)
 	    }
 	  tmpreg = get_num (&opP->disp, tmpreg);
 	  if (isvar (&opP->disp))
-	    add_fix (s[1], &opP->disp, 0, 0);
+	    add_fix (s[1], &opP->disp, 0, 0, opP->disp.baserel);
 	  switch (s[1])
 	    {
 	    case 'b':		/* Danger:  These do no check for
@@ -2981,7 +3145,7 @@ m68k_ip (char *instring)
 				   user beware! */
 	      if (!isbyte (tmpreg))
 		opP->error = _("out of range");
-	      insop (tmpreg, opcode);
+	      insop (tmpreg & 0xff, opcode); /* G.Nikl: clear upper byte */
 	      if (isvar (&opP->disp))
 		the_ins.reloc[the_ins.nrel - 1].n =
 		  (opcode->m_codenum) * 2 + 1;
@@ -3041,16 +3205,16 @@ m68k_ip (char *instring)
 	  switch (s[1])
 	    {
 	    case 'B':
-	      add_fix ('B', &opP->disp, 1, -1);
+	      add_fix ('B', &opP->disp, 1, -1, opP->disp.baserel);
 	      break;
 	    case 'W':
-	      add_fix ('w', &opP->disp, 1, 0);
+	      add_fix ('w', &opP->disp, 1, 0, opP->disp.baserel);
 	      addword (0);
 	      break;
 	    case 'L':
 	    long_branch:
 	      the_ins.opcode[0] |= 0xff;
-	      add_fix ('l', &opP->disp, 1, 0);
+	      add_fix ('l', &opP->disp, 1, 0, opP->disp.baserel);
 	      addword (0);
 	      addword (0);
 	      break;
@@ -3068,9 +3232,12 @@ m68k_ip (char *instring)
 
 	      var_branch:
 	      if (subs (&opP->disp)	/* We can't relax it.  */
+#ifdef OBJ_ELF
 		  /* If the displacement needs pic relocation it cannot be
 		     relaxed.  */
-		  || opP->disp.pic_reloc != pic_none)
+		  || opP->disp.pic_reloc != pic_none
+#endif
+		  || 0)
 		{
 		  if (!have_disp)
 		    as_warn (_("Can't use long branches on this architecture"));
@@ -3093,7 +3260,7 @@ m68k_ip (char *instring)
 		      the_ins.opcode[0] |= 0x0006;
 		      addword (0x4EF9);
 		    }
-		  add_fix ('l', &opP->disp, 0, 0);
+		  add_fix ('l', &opP->disp, 0, 0, opP->disp.baserel);
 		  addword (0);
 		  addword (0);
 		  break;
@@ -3144,12 +3311,19 @@ m68k_ip (char *instring)
 				  TAB (DBCCABSJ, SZ_UNDEF));
 		      break;
 		    }
-		  add_fix ('w', &opP->disp, 1, 0);
+		  add_fix ('w', &opP->disp, 1, 0, opP->disp.baserel);
 		}
 	      addword (0);
 	      break;
+
+      case '~':                                               // APOLLO 68080 DBRAL
+        opP->disp.exp.X_add_number |=1;                       // APOLLO 68080 DBRAL
+        add_fix ('w', &opP->disp, 1, 0, opP->disp.baserel);   // APOLLO 68080 DBRAL
+        addword (0);                                          // APOLLO 68080 DBRAL
+        break;                                                // APOLLO 68080 DBRAL
+
 	    case 'C':		/* Fixed size LONG coproc branches.  */
-	      add_fix ('l', &opP->disp, 1, 0);
+	      add_fix ('l', &opP->disp, 1, 0, opP->disp.baserel);
 	      addword (0);
 	      addword (0);
 	      break;
@@ -3157,7 +3331,7 @@ m68k_ip (char *instring)
 	      if (subs (&opP->disp) || (adds (&opP->disp) == 0))
 		{
 		  the_ins.opcode[the_ins.numo - 1] |= 0x40;
-		  add_fix ('l', &opP->disp, 1, 0);
+		  add_fix ('l', &opP->disp, 1, 0, opP->disp.baserel);
 		  addword (0);
 		  addword (0);
 		}
@@ -3611,7 +3785,7 @@ m68k_ip (char *instring)
 	  break;
 	case '_':	/* used only for move16 absolute 32-bit address.  */
 	  if (isvar (&opP->disp))
-	    add_fix ('l', &opP->disp, 0, 0);
+	    add_fix ('l', &opP->disp, 0, 0, opP->disp.baserel);
 	  tmpreg = get_num (&opP->disp, 90);
 	  addword (tmpreg >> 16);
 	  addword (tmpreg & 0xFFFF);
@@ -3639,7 +3813,7 @@ m68k_ip (char *instring)
 	  abort ();
 	}
     }
-
+  }
   /* By the time when get here (FINALLY) the_ins contains the complete
      instruction, ready to be emitted. . .  */
 }
@@ -3960,6 +4134,12 @@ struct init_entry
     int number;
   };
 
+#if TE_AMIGA
+  #define FRAME ADDR5
+#else
+  #define FRAME ADDR6
+#endif
+
 static const struct init_entry init_table[] =
 {
   { "d0", DATA0 },
@@ -3977,7 +4157,7 @@ static const struct init_entry init_tabl
   { "a4", ADDR4 },
   { "a5", ADDR5 },
   { "a6", ADDR6 },
-  { "fp", ADDR6 },
+  { "fp", FRAME },
   { "a7", ADDR7 },
   { "sp", ADDR7 },
   { "ssp", ADDR7 },
@@ -4000,6 +4180,39 @@ static const struct init_entry init_tabl
   { "status", FPS },
   { "iaddr", FPI },
 
+  { "b0", ADDR8 },
+  { "b1", ADDR9 },
+  { "b2", ADDR10 },
+  { "b3", ADDR11 },
+  { "b4", ADDR12 },
+  { "b5", ADDR13 },
+  { "b6", ADDR14 },
+  { "b7", ADDR15 },
+  { "e0", DATA8 },
+  { "e1", DATA9 },
+  { "e2", DATA10 },
+  { "e3", DATA11 },
+  { "e4", DATA12 },
+  { "e5", DATA13 },
+  { "e6", DATA14 },
+  { "e7", DATA15 },
+  { "e8", DATA16 },
+  { "e9", DATA17 },
+  { "e10", DATA19 },
+  { "e11", DATA19 },
+  { "e12", DATA20 },
+  { "e13", DATA21 },
+  { "e14", DATA22 },
+  { "e15", DATA23 },
+  { "e16", DATA24 },
+  { "e17", DATA25 },
+  { "e18", DATA26 },
+  { "e19", DATA27 },
+  { "e20", DATA28 },
+  { "e21", DATA29 },
+  { "e22", DATA30 },
+  { "e23", DATA31 },
+
   { "cop0", COP0 },
   { "cop1", COP1 },
   { "cop2", COP2 },
@@ -4299,13 +4512,28 @@ md_assemble (char *str)
       current_label = NULL;
     }
 
+#ifdef OBJ_ELF
   /* Tie dwarf2 debug info to the address at the start of the insn.  */
   dwarf2_emit_insn (0);
+#endif
 
   if (the_ins.nfrag == 0)
     {
       /* No frag hacking involved; just put it out.  */
-      toP = frag_more (2 * the_ins.numo);
+      int numc = 2 * the_ins.numo;
+
+      if (the_ins.bank)
+        numc += 2;
+
+      toP = frag_more (numc);
+
+      if (the_ins.bank)
+	{
+  	  the_ins.bank |= (numc / 2 - 2) << 6;
+	  md_number_to_chars (toP, (long) the_ins.bank, 2);
+	  toP += 2;
+	}
+
       fromP = &the_ins.opcode[0];
       for (m = the_ins.numo; m; --m)
 	{
@@ -4346,7 +4574,8 @@ md_assemble (char *str)
 			      &the_ins.reloc[m].exp,
 			      the_ins.reloc[m].pcrel,
 			      get_reloc_code (n, the_ins.reloc[m].pcrel,
-					      the_ins.reloc[m].pic_reloc));
+					      the_ins.reloc[m].pic_reloc),
+			      the_ins.reloc[m].baserel);
 	  fixP->fx_pcrel_adjust = the_ins.reloc[m].pcrel_fix;
 	  if (the_ins.reloc[m].wid == 'B')
 	    fixP->fx_signed = 1;
@@ -4364,6 +4593,10 @@ md_assemble (char *str)
       wid += 2 * (the_ins.numo - the_ins.fragb[n - 1].fragoff);
     /* frag_var part.  */
     wid += FRAG_VAR_SIZE;
+
+    if (the_ins.bank)
+      wid += 2;
+
     /* Make sure the whole insn fits in one chunk, in particular that
        the var part is attached, as we access one byte before the
        variable frag for byte branches.  */
@@ -4378,7 +4611,19 @@ md_assemble (char *str)
 	wid = 2 * the_ins.fragb[n].fragoff;
       else
 	wid = 2 * (the_ins.numo - the_ins.fragb[n - 1].fragoff);
+
+      if (the_ins.bank)
+        wid += 2;
+
       toP = frag_more (wid);
+
+      if (the_ins.bank)
+  	{
+  	  the_ins.bank |= (wid / 2 - 2) << 6;
+  	  md_number_to_chars (toP, (long) the_ins.bank, 2);
+  	  toP += 2;
+  	}
+
       to_beg_P = toP;
       shorts_this_frag = 0;
       for (m = wid / 2; m; --m)
@@ -4408,7 +4653,8 @@ md_assemble (char *str)
 			      &the_ins.reloc[m].exp,
 			      the_ins.reloc[m].pcrel,
 			      get_reloc_code (wid, the_ins.reloc[m].pcrel,
-					      the_ins.reloc[m].pic_reloc));
+					      the_ins.reloc[m].pic_reloc),
+			      the_ins.reloc[m].baserel);
 	  fixP->fx_pcrel_adjust = the_ins.reloc[m].pcrel_fix;
 	}
       (void) frag_var (rs_machine_dependent, FRAG_VAR_SIZE, 0,
@@ -4446,7 +4692,8 @@ md_assemble (char *str)
 			  &the_ins.reloc[m].exp,
 			  the_ins.reloc[m].pcrel,
 			  get_reloc_code (wid, the_ins.reloc[m].pcrel,
-					  the_ins.reloc[m].pic_reloc));
+					  the_ins.reloc[m].pic_reloc),
+			  the_ins.reloc[m].baserel);
       fixP->fx_pcrel_adjust = the_ins.reloc[m].pcrel_fix;
     }
 }
@@ -4670,9 +4917,11 @@ md_begin (void)
 
   init_regtable ();
 
+#ifdef OBJ_ELF
   record_alignment (text_section, 2);
   record_alignment (data_section, 2);
   record_alignment (bss_section, 2);
+#endif
 }
 
 
@@ -4688,7 +4937,9 @@ m68k_frob_label (symbolS *sym)
   n->text = 0;
   current_label = n;
 
+#ifdef OBJ_ELF
   dwarf2_emit_label (sym);
+#endif
 }
 
 /* This is called when a value that is not an instruction is emitted.  */
@@ -4792,6 +5043,7 @@ md_apply_fix (fixS *fixP, valueT *valP,
   if (fixP->fx_addsy == NULL && fixP->fx_pcrel == 0)
     fixP->fx_done = 1;
 
+#ifdef OBJ_ELF
   if (fixP->fx_addsy)
     {
       memset (buf, 0, fixP->fx_size);
@@ -4828,6 +5080,15 @@ md_apply_fix (fixS *fixP, valueT *valP,
 
       return;
     }
+#elif defined(OBJ_AOUT)
+  /* PR gas/3041 Do not fix frags referencing a weak symbol.  */
+  if (fixP->fx_addsy && S_IS_WEAK (fixP->fx_addsy))
+    {
+      memset (buf, 0, fixP->fx_size);
+      fixP->fx_addnumber = val;	/* Remember value for emit_reloc.  */
+      return;
+    }
+#endif
 
   if (fixP->fx_r_type == BFD_RELOC_VTABLE_INHERIT
       || fixP->fx_r_type == BFD_RELOC_VTABLE_ENTRY)
@@ -4886,7 +5147,8 @@ md_apply_fix (fixS *fixP, valueT *valP,
      in write.c may have clobbered fx_pcrel, so we need to examine the
      reloc type.  */
   if ((fixP->fx_pcrel
-       || fixP->fx_r_type == BFD_RELOC_8_PCREL)
+       || fixP->fx_r_type == BFD_RELOC_8_PCREL
+       || fixP->fx_r_type == BFD_RELOC_NONE)
       && fixP->fx_size == 1
       && (fixP->fx_addsy == NULL
 	  || S_IS_DEFINED (fixP->fx_addsy))
@@ -4931,7 +5193,7 @@ md_convert_frag_1 (fragS *fragP)
 	as_bad_where (fragP->fr_file, fragP->fr_line,
 		      _("short branch with zero offset: use :w"));
       fixP = fix_new (fragP, fragP->fr_fix - 1, 1, fragP->fr_symbol,
-		      fragP->fr_offset, 1, RELAX_RELOC_PC8);
+		      fragP->fr_offset, 1, RELAX_RELOC_PC8, 0);
       fixP->fx_pcrel_adjust = -1;
       break;
     case TAB (BRANCHBWL, SHORT):
@@ -4941,13 +5203,13 @@ md_convert_frag_1 (fragS *fragP)
     case TAB (BRANCHBWPL, SHORT):
       fragP->fr_opcode[1] = 0x00;
       fixP = fix_new (fragP, fragP->fr_fix, 2, fragP->fr_symbol,
-		      fragP->fr_offset, 1, RELAX_RELOC_PC16);
+		      fragP->fr_offset, 1, RELAX_RELOC_PC16, 0);
       fragP->fr_fix += 2;
       break;
     case TAB (BRANCHBWL, LONG):
       fragP->fr_opcode[1] = (char) 0xFF;
       fixP = fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol,
-		      fragP->fr_offset, 1, RELAX_RELOC_PC32);
+		      fragP->fr_offset, 1, RELAX_RELOC_PC32, 0);
       fragP->fr_fix += 4;
       break;
     case TAB (BRANCHBWPL, LONG):
@@ -4956,7 +5218,7 @@ md_convert_frag_1 (fragS *fragP)
       fragP->fr_opcode[0] = 0x66; /* bne */
       fragP->fr_opcode[1] = 0xFF;
       fixP = fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol,
-		      fragP->fr_offset, 1, RELAX_RELOC_PC32);
+		      fragP->fr_offset, 1, RELAX_RELOC_PC32, 0);
       fixP->fx_file = fragP->fr_file;
       fixP->fx_line = fragP->fr_line;
       fragP->fr_fix += 4;  /* Skip first offset */
@@ -4965,10 +5227,14 @@ md_convert_frag_1 (fragS *fragP)
       *buffer_address++ = 0xff;
       fragP->fr_fix += 2;  /* Skip second branch opcode */
       fixP = fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol,
-		      fragP->fr_offset, 1, RELAX_RELOC_PC32);
+		      fragP->fr_offset, 1, RELAX_RELOC_PC32, 0);
       fragP->fr_fix += 4;
       break;
     case TAB (BRABSJUNC, LONG):
+      if (flag_small_code)
+	{
+	  as_bad (_("Long branch in small code model, not supported."));
+	}
       if (fragP->fr_opcode[0] == 0x61)		/* jbsr */
 	{
 	  if (flag_keep_pcrel)
@@ -4977,7 +5243,7 @@ md_convert_frag_1 (fragS *fragP)
 	  fragP->fr_opcode[0] = 0x4E;
 	  fragP->fr_opcode[1] = (char) 0xB9; /* JSR with ABSL LONG operand.  */
 	  fixP = fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol,
-			  fragP->fr_offset, 0, RELAX_RELOC_ABS32);
+			  fragP->fr_offset, 0, RELAX_RELOC_ABS32, 0);
 	  fragP->fr_fix += 4;
 	}
       else if (fragP->fr_opcode[0] == 0x60)	/* jbra */
@@ -4988,7 +5254,7 @@ md_convert_frag_1 (fragS *fragP)
 	  fragP->fr_opcode[0] = 0x4E;
 	  fragP->fr_opcode[1] = (char) 0xF9; /* JMP with ABSL LONG operand.  */
 	  fixP = fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol,
-			  fragP->fr_offset, 0, RELAX_RELOC_ABS32);
+			  fragP->fr_offset, 0, RELAX_RELOC_ABS32, 0);
 	  fragP->fr_fix += 4;
 	}
       else
@@ -5015,25 +5281,25 @@ md_convert_frag_1 (fragS *fragP)
       *buffer_address++ = (char) 0xf9;
       fragP->fr_fix += 2;	/* Account for jmp instruction.  */
       fixP = fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol,
-		      fragP->fr_offset, 0, RELAX_RELOC_ABS32);
+		      fragP->fr_offset, 0, RELAX_RELOC_ABS32, 0);
       fragP->fr_fix += 4;
       break;
     case TAB (FBRANCH, SHORT):
       know ((fragP->fr_opcode[1] & 0x40) == 0);
       fixP = fix_new (fragP, fragP->fr_fix, 2, fragP->fr_symbol,
-		      fragP->fr_offset, 1, RELAX_RELOC_PC16);
+		      fragP->fr_offset, 1, RELAX_RELOC_PC16, 0);
       fragP->fr_fix += 2;
       break;
     case TAB (FBRANCH, LONG):
       fragP->fr_opcode[1] |= 0x40;	/* Turn on LONG bit.  */
       fixP = fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol,
-		      fragP->fr_offset, 1, RELAX_RELOC_PC32);
+		      fragP->fr_offset, 1, RELAX_RELOC_PC32, 0);
       fragP->fr_fix += 4;
       break;
     case TAB (DBCCLBR, SHORT):
     case TAB (DBCCABSJ, SHORT):
       fixP = fix_new (fragP, fragP->fr_fix, 2, fragP->fr_symbol,
-		      fragP->fr_offset, 1, RELAX_RELOC_PC16);
+		      fragP->fr_offset, 1, RELAX_RELOC_PC16, 0);
       fragP->fr_fix += 2;
       break;
     case TAB (DBCCLBR, LONG):
@@ -5049,7 +5315,7 @@ md_convert_frag_1 (fragS *fragP)
 
       fragP->fr_fix += 6;	/* Account for bra/jmp instructions.  */
       fixP = fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol,
-		      fragP->fr_offset, 1, RELAX_RELOC_PC32);
+		      fragP->fr_offset, 1, RELAX_RELOC_PC32, 0);
       fragP->fr_fix += 4;
       break;
     case TAB (DBCCABSJ, LONG):
@@ -5069,14 +5335,14 @@ md_convert_frag_1 (fragS *fragP)
 
       fragP->fr_fix += 6;		/* Account for bra/jmp instructions.  */
       fixP = fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol,
-		      fragP->fr_offset, 0, RELAX_RELOC_ABS32);
+		      fragP->fr_offset, 0, RELAX_RELOC_ABS32, 0);
       fragP->fr_fix += 4;
       break;
     case TAB (PCREL1632, SHORT):
       fragP->fr_opcode[1] &= ~0x3F;
       fragP->fr_opcode[1] |= 0x3A; /* 072 - mode 7.2 */
       fixP = fix_new (fragP, (int) (fragP->fr_fix), 2, fragP->fr_symbol,
-		      fragP->fr_offset, 1, RELAX_RELOC_PC16);
+		      fragP->fr_offset, 1, RELAX_RELOC_PC16, 0);
       fragP->fr_fix += 2;
       break;
     case TAB (PCREL1632, LONG):
@@ -5086,7 +5352,7 @@ md_convert_frag_1 (fragS *fragP)
       *buffer_address++ = 0x70;
       fragP->fr_fix += 2;
       fixP = fix_new (fragP, (int) (fragP->fr_fix), 4, fragP->fr_symbol,
-		      fragP->fr_offset, 1, RELAX_RELOC_PC32);
+		      fragP->fr_offset, 1, RELAX_RELOC_PC32, 0);
       fixP->fx_pcrel_adjust = 2;
       fragP->fr_fix += 4;
       break;
@@ -5094,7 +5360,7 @@ md_convert_frag_1 (fragS *fragP)
       gas_assert (fragP->fr_fix >= 2);
       buffer_address[-2] &= ~1;
       fixP = fix_new (fragP, fragP->fr_fix - 1, 1, fragP->fr_symbol,
-		      fragP->fr_offset, 1, RELAX_RELOC_PC8);
+		      fragP->fr_offset, 1, RELAX_RELOC_PC8, 0);
       fixP->fx_pcrel_adjust = 1;
       break;
     case TAB (PCINDEX, SHORT):
@@ -5102,7 +5368,7 @@ md_convert_frag_1 (fragS *fragP)
       buffer_address[-2] |= 0x1;
       buffer_address[-1] = 0x20;
       fixP = fix_new (fragP, (int) (fragP->fr_fix), 2, fragP->fr_symbol,
-		      fragP->fr_offset, 1, RELAX_RELOC_PC16);
+		      fragP->fr_offset, 1, RELAX_RELOC_PC16, 0);
       fixP->fx_pcrel_adjust = 2;
       fragP->fr_fix += 2;
       break;
@@ -5111,13 +5377,13 @@ md_convert_frag_1 (fragS *fragP)
       buffer_address[-2] |= 0x1;
       buffer_address[-1] = 0x30;
       fixP = fix_new (fragP, (int) (fragP->fr_fix), 4, fragP->fr_symbol,
-		      fragP->fr_offset, 1, RELAX_RELOC_PC32);
+		      fragP->fr_offset, 1, RELAX_RELOC_PC32, 0);
       fixP->fx_pcrel_adjust = 2;
       fragP->fr_fix += 4;
       break;
     case TAB (ABSTOPCREL, SHORT):
       fixP = fix_new (fragP, fragP->fr_fix, 2, fragP->fr_symbol,
-		      fragP->fr_offset, 1, RELAX_RELOC_PC16);
+		      fragP->fr_offset, 1, RELAX_RELOC_PC16, 0);
       fragP->fr_fix += 2;
       break;
     case TAB (ABSTOPCREL, LONG):
@@ -5131,10 +5397,82 @@ md_convert_frag_1 (fragS *fragP)
       fragP->fr_opcode[1] &= ~0x3F;
       fragP->fr_opcode[1] |= 0x39;	/* Mode 7.1 */
       fixP = fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol,
-		      fragP->fr_offset, 0, RELAX_RELOC_ABS32);
+		      fragP->fr_offset, 0, RELAX_RELOC_ABS32, 0);
+      fragP->fr_fix += 4;
+      break;
+    case TAB (ABSREL, BYTE):
+      as_bad (_("ABSREL_BYTE: how the ** does this look??"));
+      break;
+    case TAB (ABSREL, SHORT):
+      fragP->fr_opcode[1] &= ~0x3f;
+      fragP->fr_fix += 2;
+      if (S_GET_SEGMENT (fragP->fr_symbol) == text_section)
+	{
+	  /* so this is really a pc-relative address */
+	  fragP->fr_opcode[1] |= 0x3a;
+	  fix_new (fragP, fragP->fr_fix, 2, fragP->fr_symbol, fragP->fr_offset, 1, NO_RELOC, 0);
+  	  break;
+  	}
+      /* in that case we have to generate base-relative code
+       * (note: if we're in N_UNDF, this could as well be pc-relative, but the linker
+       *        will have to do the final patch in that case) */
+      fragP->fr_opcode[1] |= 0x2c;  /* (a4) */
+      fix_new (fragP, fragP->fr_fix, 2, fragP->fr_symbol, fragP->fr_offset, 0, NO_RELOC, 1);
+      break;
+    case TAB (ABSREL, LONG):
+      as_bad (_("ABSREL_LONG: sorry, not supported."));
+      break;
+    case TAB (IMMREL, BYTE):
+      as_bad (_("IMMREL_BYTE: how the ** does this look??"));
+      break;
+    case TAB (IMMREL, SHORT):
+      if (S_GET_SEGMENT (fragP->fr_symbol) == text_section)
+	{
+	/* we can only fix operations on data registers, not on <ea> */
+	if ((fragP->fr_opcode[1] & 0x38) != 0)
+	  {
+	    /* use the normal reloc32, sigh... */
+	    fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol, fragP->fr_offset, 0, NO_RELOC, 0);
       fragP->fr_fix += 4;
       break;
     }
+
+	  /* so this is really a pc-relative address
+	   * What we have to do now is a VERY UGLY AND BIG KLUDGE. Basically do the
+	   * following thing:
+	   *   turn
+	   *     addl #foo,d0      (foo is N_TEXT)
+	   *   into
+	   *     pea  foo(pc)
+	   *     addl (sp)+,d0
+	   */
+	  *buffer_address++ = fragP->fr_opcode[0]; /* save the original command */
+	  *buffer_address++ = fragP->fr_opcode[1];
+	  fragP->fr_opcode[0] = 0x48; 	/* PEA */
+	  fragP->fr_opcode[1] = 0x7a;
+	  fix_new (fragP, fragP->fr_fix, 2, fragP->fr_symbol, fragP->fr_offset, 1, NO_RELOC, 0);
+
+	  *buffer_address++ = 0x9f;	/* sp@+ */
+	  fragP->fr_fix += 4;	/* two byte fix, two byte code extension */
+	  break;
+	}
+      /* in that case we have to generate base-relative code
+       * (note: if we're in N_UNDF, this could as well be pc-relative, but the linker
+       *        will have to do the final patch in that case) */
+
+      /* analogous (more or less;-)) to above, the following conversion is done
+       *   turn
+       *     addl #bar,d0		(bar is N_DATA)
+       *   into
+       *	   addl #<bar>,d0	where <bar> is a baserel-reloc
+       *     addl a4,d0
+       */
+
+      fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol, fragP->fr_offset, 0, NO_RELOC, 1);
+      *buffer_address++ = 0xd0;
+      *buffer_address++ = 0x8c;
+      break;
+    }
   if (fixP)
     {
       fixP->fx_file = fragP->fr_file;
@@ -5169,7 +5507,7 @@ md_estimate_size_before_relax (fragS *fr
 	  {
 	    fragP->fr_subtype = TAB (TABTYPE (fragP->fr_subtype), BYTE);
 	  }
-	else if (flag_short_refs)
+	else if (flag_short_refs || flag_small_code)
 	  {
 	    /* Symbol is undefined and we want short ref.  */
 	    fragP->fr_subtype = TAB (TABTYPE (fragP->fr_subtype), SHORT);
@@ -5241,6 +5579,21 @@ md_estimate_size_before_relax (fragS *fr
 	break;
       }
 
+    case TAB (ABSREL, SZ_UNDEF):
+      {
+	if ((S_GET_SEGMENT (fragP->fr_symbol) == segment
+	     && relaxable_symbol (fragP->fr_symbol))
+	    || flag_short_refs || FLAG_SMALL_CODE)
+	  {
+	    fragP->fr_subtype = TAB (ABSREL, SHORT);
+	  }
+	else
+	  {
+	    fragP->fr_subtype = TAB (ABSREL, LONG);
+	  }
+	break;
+      }
+
     default:
       break;
     }
@@ -5311,7 +5664,7 @@ md_create_long_jump (char *ptr, addressT
       md_number_to_chars (ptr, (valueT) 0x4EF9, 2);
       md_number_to_chars (ptr + 2, (valueT) offset, 4);
       fix_new (frag, (ptr + 2) - frag->fr_literal, 4, to_symbol, (offsetT) 0,
-	       0, NO_RELOC);
+	       0, NO_RELOC, 0);
     }
   else
     {
@@ -7199,12 +7552,15 @@ s_m68k_arch (int ignored ATTRIBUTE_UNUSE
   char saved_char;
   char *name;
 
-  if (initialized)
-    {
-      as_bad (_("already assembled instructions"));
-      ignore_rest_of_line ();
-      return;
-    }
+  current_architecture = 0;
+  not_current_architecture = 0;
+
+//  if (initialized)
+//    {
+//      as_bad (_("already assembled instructions"));
+//      ignore_rest_of_line ();
+//      return;
+//    }
 
   name = input_line_pointer;
   while (*input_line_pointer && *input_line_pointer != ','
@@ -7233,6 +7589,9 @@ s_m68k_arch (int ignored ATTRIBUTE_UNUSE
 
   *input_line_pointer = saved_char;
   demand_empty_rest_of_line ();
+
+  m68k_init_arch();
+
   return;
 }
 
@@ -7271,6 +7630,8 @@ m68k_lookup_cpu (const char *arg, const
 	    arg += 2;
 	}
     }
+  else if (arg[0] == 'c' && arg[1] == 'p' && arg[2] == 'u' && arg[3] == '=')
+    arg += 4;
   else if (arg[0] == 'c' && arg[1] == '6')
     arg += 1;
 
@@ -7353,7 +7714,11 @@ m68k_set_extension (char const *name, in
    Invocation line includes a switch not recognized by the base assembler.
  */
 
+#ifdef OBJ_ELF
 const char *md_shortopts = "lSA:m:kQ:V";
+#else
+const char *md_shortopts = "lSA:m:s:k";
+#endif
 
 struct option md_longopts[] = {
 #define OPTION_PIC (OPTION_MD_BASE)
@@ -7397,6 +7762,13 @@ md_parse_option (int c, const char *arg)
       flag_keep_pcrel = 1;
       break;
 
+    case 's':
+      if (!strcmp(arg, "c") || !strcmp(arg, "mallcode"))
+	flag_small_code = 1;
+      else
+	return 0;
+      break;
+
     case OPTION_PIC:
     case 'k':
       flag_want_pic = 1;
@@ -7469,6 +7841,8 @@ md_parse_option (int c, const char *arg)
 	;
       else
 	return 0;
+
+      m68k_init_arch();
       break;
 
     default:
@@ -7532,8 +7906,8 @@ m68k_init_arch (void)
      with a coprocessor could be doing emulation.  */
   if (current_architecture & m68851)
     {
-      if (current_architecture & m68040)
-	as_warn (_("68040 and 68851 specified; mmu instructions may assemble incorrectly"));
+      if (current_architecture & (m68040|m68060|m68080))
+	as_warn (_("68040/68060/68080 and 68851 specified; mmu instructions may assemble incorrectly"));
     }
   /* What other incompatibilities could we check for?  */
 
@@ -7579,7 +7953,8 @@ md_show_usage (FILE *stream)
   fprintf (stream, _("\
 -l			use 1 word for refs to undefined symbols [default 2]\n\
 -pic, -k		generate position independent code\n\
--S			turn jbsr into jsr\n\
+-S			turn jbsr into jsr and keeps jra for non local labels.\n\
+-smallcode, -sc		small code model - does nothing atm\n\
 --pcrel                 never turn PC-relative branches into absolute jumps\n\
 --register-prefix-optional\n\
 			recognize register names without prefix character\n\
@@ -7725,16 +8100,40 @@ md_section_align (segT segment ATTRIBUTE
    word.  The difference between the addresses of the offset and the
    first extension word is stored in fx_pcrel_adjust.  */
 long
-md_pcrel_from (fixS *fixP)
+md_pcrel_from_m68k (fixS *fixP, segT current_section)
 {
   int adjust;
 
   adjust = fixP->fx_pcrel_adjust;
   if (adjust == 64)
     adjust = -1;
+
+#ifdef TE_AMIGA
+  /* Amiga Hunk adjusts to current address. */
+  if (stdoutput->xvec == &amiga_vec)
+    {
+      if (!fixP->fx_addsy)
+	return -adjust;
+
+      asymbol * sym = symbol_get_bfdsym (fixP->fx_addsy);
+      if (adjust != -1 && sym->section != current_section)
+	{
+	  if (strcmp(sym->section->name, "*UND*") && strcmp(sym->section->name, current_section->name))
+	    {
+	      long addsy_fix = fixP->fx_addsy ? symbol_get_frag(fixP->fx_addsy)->fr_fix : 0;
+	      return -adjust + fixP->fx_frag->fr_address + addsy_fix;
+	    }
+	  return -adjust;
+	}
+
+      // fall through
+    }
+#endif
+
   return fixP->fx_where + fixP->fx_frag->fr_address - adjust;
 }
 
+#ifdef OBJ_ELF
 void
 m68k_elf_final_processing (void)
 {
@@ -7836,7 +8235,7 @@ m68k_elf_suffix (char **str_p, expressio
   *str2 = '\0';
   len = str2 - ident;
 
-  if (startswith (ident, "TLSLDO")
+  if (strncmp (ident, "TLSLDO", 6) == 0
       && len == 6)
     {
       /* Now check for identifier@suffix+constant.  */
@@ -7910,7 +8309,7 @@ m68k_elf_cons (int nbytes /* 4=.long */)
 	      if (target_big_endian)
 		offset = nbytes - size;
 	      fix_new_exp (frag_now, p - frag_now->fr_literal + offset, size,
-			   &exp, 0, reloc);
+			   &exp, 0, reloc, 0);
 	    }
 	}
       else
@@ -7922,7 +8321,9 @@ m68k_elf_cons (int nbytes /* 4=.long */)
   input_line_pointer--;
   demand_empty_rest_of_line ();
 }
+#endif
 
+#if defined (OBJ_ELF)
 /* Parse a .gnu_attribute directive.  */
 static void
 m68k_elf_gnu_attribute (int ignored ATTRIBUTE_UNUSED)
@@ -7940,6 +8341,7 @@ m68k_elf_gnu_attribute (int ignored ATTR
 	as_warn (_("unknown .gnu_attribute value"));
     }
 }
+#endif
 
 int
 tc_m68k_regname_to_dw2regnum (const char *regname)

--- gas/config/tc-m68k.h	2022-07-08 10:46:47.000000000 +0100
+++ gas/config/tc-m68k.h	2025-11-07 11:25:06.909430367 +0000
@@ -27,7 +27,13 @@ struct fix;
 
 #define TARGET_BYTES_BIG_ENDIAN 1
 
+#ifdef OBJ_AMIGAHUNK
+#undef TARGET_FORMAT
+#define TARGET_FORMAT "amiga"
+#else
 #define TARGET_FORMAT "elf32-m68k"
+#endif
+
 #define TARGET_ARCH bfd_arch_m68k
 
 #define tc_comment_chars m68k_comment_chars
@@ -43,9 +49,14 @@ extern const char *m68k_comment_chars;
 #define REGISTER_PREFIX '%'
 #endif
 
+#ifdef OBJ_AMIGAHUNK
+#undef REGISTER_PREFIX_OPTIONAL 
+#define REGISTER_PREFIX_OPTIONAL 1
+#else
 #ifndef REGISTER_PREFIX_OPTIONAL
 #define REGISTER_PREFIX_OPTIONAL 0
 #endif
+#endif
 
 extern void m68k_mri_mode_change (int);
 #define MRI_MODE_CHANGE(i) m68k_mri_mode_change (i)
@@ -78,6 +89,7 @@ while (0)
 #define RELAX_RELOC_PC16  BFD_RELOC_16_PCREL
 #define RELAX_RELOC_PC32  BFD_RELOC_32_PCREL
 
+#ifdef OBJ_ELF
 #define tc_fix_adjustable(X) tc_m68k_fix_adjustable(X)
 extern int tc_m68k_fix_adjustable (struct fix *);
 
@@ -92,6 +104,7 @@ extern int tc_m68k_fix_adjustable (struc
 
 #define elf_tc_final_processing m68k_elf_final_processing
 extern void m68k_elf_final_processing (void);
+#endif
 
 #define DIFF_EXPR_OK
 
@@ -100,9 +113,15 @@ extern int m68k_parse_long_option (char
 
 #define md_operand(x)
 
+#define TARGET_WORD_SIZE 32
+#define TARGET_ARCH bfd_arch_m68k
+
 extern struct relax_type md_relax_table[];
 #define TC_GENERIC_RELAX_TABLE md_relax_table
 
+#define TC_FIX_TYPE char
+#define TC_INIT_FIX_DATA(p)
+
 /* We can't do a byte jump to the next instruction, so in that case
    force word mode by faking AIM.  */
 #define md_prepare_relax_scan(fragP, address, aim, this_state, this_type) \

--- gas/config/tc-ppc.c	2022-07-25 08:04:15.000000000 +0100
+++ gas/config/tc-ppc.c	2025-11-07 11:25:06.929430667 +0000
@@ -6666,8 +6666,6 @@ ppc_force_relocation (fixS *fix)
 int
 ppc_force_relocation (fixS *fix)
 {
-  /* Branch prediction relocations must force a relocation, as must
-     the vtable description relocs.  */
   switch (fix->fx_r_type)
     {
     case BFD_RELOC_PPC_B16_BRTAKEN:
@@ -6676,6 +6674,60 @@ ppc_force_relocation (fixS *fix)
     case BFD_RELOC_PPC_BA16_BRNTAKEN:
     case BFD_RELOC_24_PLT_PCREL:
     case BFD_RELOC_PPC64_TOC:
+    case BFD_RELOC_16_GOTOFF:
+    case BFD_RELOC_LO16_GOTOFF:
+    case BFD_RELOC_HI16_GOTOFF:
+    case BFD_RELOC_HI16_S_GOTOFF:
+    case BFD_RELOC_LO16_PLTOFF:
+    case BFD_RELOC_HI16_PLTOFF:
+    case BFD_RELOC_HI16_S_PLTOFF:
+    case BFD_RELOC_GPREL16:
+    case BFD_RELOC_16_BASEREL:
+    case BFD_RELOC_LO16_BASEREL:
+    case BFD_RELOC_HI16_BASEREL:
+    case BFD_RELOC_HI16_S_BASEREL:
+    case BFD_RELOC_PPC_TOC16:
+    case BFD_RELOC_PPC64_TOC16_LO:
+    case BFD_RELOC_PPC64_TOC16_HI:
+    case BFD_RELOC_PPC64_TOC16_HA:
+    case BFD_RELOC_PPC64_PLTGOT16:
+    case BFD_RELOC_PPC64_PLTGOT16_LO:
+    case BFD_RELOC_PPC64_PLTGOT16_HI:
+    case BFD_RELOC_PPC64_PLTGOT16_HA:
+    case BFD_RELOC_PPC64_GOT16_DS:
+    case BFD_RELOC_PPC64_GOT16_LO_DS:
+    case BFD_RELOC_PPC64_PLT16_LO_DS:
+    case BFD_RELOC_PPC64_SECTOFF_DS:
+    case BFD_RELOC_PPC64_SECTOFF_LO_DS:
+    case BFD_RELOC_PPC64_TOC16_DS:
+    case BFD_RELOC_PPC64_TOC16_LO_DS:
+    case BFD_RELOC_PPC64_PLTGOT16_DS:
+    case BFD_RELOC_PPC64_PLTGOT16_LO_DS:
+    case BFD_RELOC_PPC_EMB_NADDR16:
+    case BFD_RELOC_PPC_EMB_NADDR16_LO:
+    case BFD_RELOC_PPC_EMB_NADDR16_HI:
+    case BFD_RELOC_PPC_EMB_NADDR16_HA:
+    case BFD_RELOC_PPC_EMB_SDAI16:
+    case BFD_RELOC_PPC_EMB_SDA2I16:
+    case BFD_RELOC_PPC_EMB_SDA2REL:
+    case BFD_RELOC_PPC_EMB_SDA21:
+    case BFD_RELOC_PPC_EMB_MRKREF:
+    case BFD_RELOC_PPC_EMB_RELSEC16:
+    case BFD_RELOC_PPC_EMB_RELST_LO:
+    case BFD_RELOC_PPC_EMB_RELST_HI:
+    case BFD_RELOC_PPC_EMB_RELST_HA:
+    case BFD_RELOC_PPC_EMB_BIT_FLD:
+    case BFD_RELOC_PPC_EMB_RELSDA:
+    case BFD_RELOC_PPC_VLE_SDA21:
+    case BFD_RELOC_PPC_VLE_SDA21_LO:
+    case BFD_RELOC_PPC_VLE_SDAREL_LO16A:
+    case BFD_RELOC_PPC_VLE_SDAREL_LO16D:
+    case BFD_RELOC_PPC_VLE_SDAREL_HI16A:
+    case BFD_RELOC_PPC_VLE_SDAREL_HI16D:
+    case BFD_RELOC_PPC_VLE_SDAREL_HA16A:
+    case BFD_RELOC_PPC_VLE_SDAREL_HA16D:
+    case BFD_RELOC_PPC64_PLT_PCREL34:
+    case BFD_RELOC_PPC64_GOT_PCREL34:
       return 1;
     case BFD_RELOC_PPC_B26:
     case BFD_RELOC_PPC_BA26:


--- gas/configure	2022-08-05 10:55:17.000000000 +0100
+++ gas/configure	2025-11-07 11:25:06.961431146 +0000
@@ -1,6 +1,6 @@
 #! /bin/sh
 # Guess values for system-dependent variables and create Makefiles.
-# Generated by GNU Autoconf 2.69 for gas 2.39.
+# Generated by GNU Autoconf 2.69 for gas 2.39.0.
 #
 #
 # Copyright (C) 1992-1996, 1998-2012 Free Software Foundation, Inc.
@@ -587,8 +587,8 @@ MAKEFLAGS=
 # Identity of this package.
 PACKAGE_NAME='gas'
 PACKAGE_TARNAME='gas'
-PACKAGE_VERSION='2.39'
-PACKAGE_STRING='gas 2.39'
+PACKAGE_VERSION='2.39.0'
+PACKAGE_STRING='gas 2.39.0'
 PACKAGE_BUGREPORT=''
 PACKAGE_URL=''
 
@@ -1369,7 +1369,7 @@ if test "$ac_init_help" = "long"; then
   # Omit some internal or obsolete options to make the list less imposing.
   # This message is too long to be a string in the A/UX 3.1 sh.
   cat <<_ACEOF
-\`configure' configures gas 2.39 to adapt to many kinds of systems.
+\`configure' configures gas 2.39.0 to adapt to many kinds of systems.
 
 Usage: $0 [OPTION]... [VAR=VALUE]...
 
@@ -1440,7 +1440,7 @@ fi
 
 if test -n "$ac_init_help"; then
   case $ac_init_help in
-     short | recursive ) echo "Configuration of gas 2.39:";;
+     short | recursive ) echo "Configuration of gas 2.39.0:";;
    esac
   cat <<\_ACEOF
 
@@ -1576,7 +1576,7 @@ fi
 test -n "$ac_init_help" && exit $ac_status
 if $ac_init_version; then
   cat <<\_ACEOF
-gas configure 2.39
+gas configure 2.39.0
 generated by GNU Autoconf 2.69
 
 Copyright (C) 2012 Free Software Foundation, Inc.
@@ -1987,7 +1987,7 @@ cat >config.log <<_ACEOF
 This file contains any messages produced by compilers while
 running configure, to aid debugging if configure makes a mistake.
 
-It was created by gas $as_me 2.39, which was
+It was created by gas $as_me 2.39.0, which was
 generated by GNU Autoconf 2.69.  Invocation command line was
 
   $ $0 $@
@@ -2966,7 +2966,7 @@ fi
 
 # Define the identity of the package.
  PACKAGE='gas'
- VERSION='2.39'
+ VERSION='2.39.0'
 
 
 cat >>confdefs.h <<_ACEOF
@@ -14611,7 +14611,7 @@ cat >>$CONFIG_STATUS <<\_ACEOF || ac_wri
 # report actual input values of CONFIG_FILES etc. instead of their
 # values after options handling.
 ac_log="
-This file was extended by gas $as_me 2.39, which was
+This file was extended by gas $as_me 2.39.0, which was
 generated by GNU Autoconf 2.69.  Invocation command line was
 
   CONFIG_FILES    = $CONFIG_FILES
@@ -14677,7 +14677,7 @@ _ACEOF
 cat >>$CONFIG_STATUS <<_ACEOF || ac_write_fail=1
 ac_cs_config="`$as_echo "$ac_configure_args" | sed 's/^ //; s/[\\""\`\$]/\\\\&/g'`"
 ac_cs_version="\\
-gas config.status 2.39
+gas config.status 2.39.0
 configured by $0, generated by GNU Autoconf 2.69,
   with options \\"\$ac_cs_config\\"
 

--- gas/configure.tgt	2022-07-08 10:46:47.000000000 +0100
+++ gas/configure.tgt	2025-11-07 11:25:06.961431146 +0000
@@ -289,6 +289,8 @@ case ${generic_target} in
   m68hc11-*-* | m6811-*-*)		fmt=elf ;;
   m68hc12-*-* | m6812-*-*)		fmt=elf ;;
 
+  m68*-*-amigaos*)   		fmt=amigahunk em=amiga bfd_gas=yes ;;
+  m68k-*-aout)				fmt=aout bfd_gas=yes ;;
   m68k-*-elf*)				fmt=elf ;;
   m68k-*-sysv4*)			fmt=elf em=svr4 ;;
   m68k-*-linux-*)			fmt=elf em=linux ;;




--- gas/dw2gencfi.c	2022-07-08 10:46:47.000000000 +0100
+++ gas/dw2gencfi.c	2025-11-07 11:25:06.977431386 +0000
@@ -24,6 +24,9 @@
 #include "subsegs.h"
 #include "dwarf2dbg.h"
 
+int
+ffs(register int valu);
+
 #ifdef TARGET_USE_CFIPOP
 
 /* By default, use difference expressions if DIFF_EXPR_OK is defined.  */
@@ -200,7 +203,7 @@ emit_expr_encoded (expressionS *exp, int
       gas_assert (size == (unsigned) howto->bitsize / 8);
       md_number_to_chars (p, 0, size);
       fix_new (frag_now, p - frag_now->fr_literal, size, exp->X_add_symbol,
-	       exp->X_add_number, howto->pc_relative, code);
+	       exp->X_add_number, howto->pc_relative, code, 0);
     }
   else if ((encoding & 0x70) == DW_EH_PE_pcrel)
     {
@@ -1993,7 +1996,7 @@ output_fde (struct fde_entry *fde, struc
 	  gas_assert (addr_size == (unsigned) howto->bitsize / 8);
 	  md_number_to_chars (p, 0, addr_size);
 	  fix_new (frag_now, p - frag_now->fr_literal, addr_size,
-		   fde->start_address, 0, howto->pc_relative, code);
+		   fde->start_address, 0, howto->pc_relative, code,  0);
 	}
       else
 	{

--- gas/dwarf2dbg.c	2022-07-08 10:46:47.000000000 +0100
+++ gas/dwarf2dbg.c	2025-11-07 11:25:06.977431386 +0000
@@ -169,7 +169,12 @@ struct line_entry
 
 /* Don't change the offset of next in line_entry.  set_or_check_view
    calls in dwarf2_gen_line_info_1 depend on it.  */
+#ifdef _MSC_VER
+#define unused __gunused
+static char unused[1];
+#else
 static char unused[offsetof(struct line_entry, next) ? -1 : 1]
+#endif
 ATTRIBUTE_UNUSED;
 
 struct line_subseg
@@ -2882,6 +2887,7 @@ out_debug_info (segT info_seg, segT abbr
 	{
 	  const char *name;
 	  size_t len;
+	  expressionS size = { .X_op = O_constant };
 
 	  /* Skip warning constructs (see above).  */
 	  if (symbol_get_bfdsym (symp)->flags & BSF_WARNING)
@@ -2895,6 +2901,18 @@ out_debug_info (segT info_seg, segT abbr
 	  if (!S_IS_DEFINED (symp) || !S_IS_FUNCTION (symp))
 	    continue;
 
+#if defined (OBJ_ELF) /* || defined (OBJ_MAYBE_ELF) */
+	  size.X_add_number = S_GET_SIZE (symp);
+	  if (size.X_add_number == 0 && IS_ELF
+	      && symbol_get_obj (symp)->size != NULL)
+	    {
+	      size.X_op = O_add;
+	      size.X_op_symbol = make_expr_symbol (symbol_get_obj (symp)->size);
+	    }
+#endif
+	  if (size.X_op == O_constant && size.X_add_number == 0)
+	    continue;
+
 	  subseg_set (str_seg, 0);
 	  name_sym = symbol_temp_new_now_octets ();
 	  name = S_GET_NAME (symp);
@@ -2920,29 +2938,17 @@ out_debug_info (segT info_seg, segT abbr
 	  emit_expr (&exp, sizeof_address);
 
 	  /* DW_AT_high_pc */
-	  exp.X_op = O_constant;
-#if defined (OBJ_ELF) /* || defined (OBJ_MAYBE_ELF) */
-	  exp.X_add_number = S_GET_SIZE (symp);
-	  if (exp.X_add_number == 0 && IS_ELF
-	      && symbol_get_obj (symp)->size != NULL)
-	    {
-	      exp.X_op = O_add;
-	      exp.X_op_symbol = make_expr_symbol (symbol_get_obj (symp)->size);
-	    }
-#else
-	  exp.X_add_number = 0;
-#endif
 	  if (DWARF2_VERSION < 4)
 	    {
-	      if (exp.X_op == O_constant)
-		exp.X_op = O_symbol;
-	      exp.X_add_symbol = symp;
-	      emit_expr (&exp, sizeof_address);
+	      if (size.X_op == O_constant)
+		size.X_op = O_symbol;
+	      size.X_add_symbol = symp;
+	      emit_expr (&size, sizeof_address);
 	    }
-	  else if (exp.X_op == O_constant)
-	    out_uleb128 (exp.X_add_number);
+	  else if (size.X_op == O_constant)
+	    out_uleb128 (size.X_add_number);
 	  else
-	    emit_leb128_expr (symbol_get_value_expression (exp.X_op_symbol), 0);
+	    emit_leb128_expr (symbol_get_value_expression (size.X_op_symbol), 0);
 	}
 
       /* End of children.  */

--- gas/expr.c	2022-07-08 10:46:47.000000000 +0100
+++ gas/expr.c	2025-11-07 11:25:06.981431446 +0000
@@ -23,11 +23,13 @@
    (It also gives smaller files to re-compile.)
    Here, "operand"s are of expressions, not instructions.  */
 
-#define min(a, b)       ((a) < (b) ? (a) : (b))
-
 #include "as.h"
 #include "safe-ctype.h"
 
+#ifndef min
+#define min(a, b)       ((a) < (b) ? (a) : (b))
+#endif
+
 #include <limits.h>
 #ifndef CHAR_BIT
 #define CHAR_BIT 8
@@ -2114,7 +2116,7 @@ resolve_expression (expressionS *express
   symbolS *op_symbol = expressionP->X_op_symbol;
   operatorT op = expressionP->X_op;
   valueT left, right;
-  segT seg_left, seg_right;
+  segT seg_left = 0, seg_right;
   fragS *frag_left, *frag_right;
   offsetT frag_off;
 

--- gas/input-scrub.c	2022-07-08 10:46:47.000000000 +0100
+++ gas/input-scrub.c	2025-11-07 11:25:06.981431446 +0000
@@ -278,9 +278,11 @@ input_scrub_include_sb (sb *from, char *
 
   next_saved_file = input_scrub_push (position);
 
-  /* Allocate sufficient space: from->len + optional newline.  */
+  /* Allocate sufficient space: from->len plus optional newline
+     plus two ".linefile " directives, plus a little more for other
+     expansion.  */
   newline = from->len >= 1 && from->ptr[0] != '\n';
-  sb_build (&from_sb, from->len + newline);
+  sb_build (&from_sb, from->len + newline + 2 * sizeof (".linefile") + 30);
   if (expansion == expanding_repeat && from_sb_expansion >= expanding_macro)
     expansion = expanding_nested;
   from_sb_expansion = expansion;




--- gas/macro.c	2022-07-08 10:46:47.000000000 +0100
+++ gas/macro.c	2025-11-07 11:25:06.981431446 +0000
@@ -1056,6 +1056,8 @@ macro_expand_body (sb *in, sb *out, form
       loclist = f;
     }
 
+  if (!err && (out->len == 0 || out->ptr[out->len - 1] != '\n'))
+    sb_add_char (out, '\n');
   return err;
 }
 

--- gas/Makefile.in	2022-08-05 10:55:17.000000000 +0100
+++ gas/Makefile.in	2025-11-07 11:25:06.857429587 +0000
@@ -764,6 +764,7 @@ TARGET_EXTRA_FILES = \
 
 # OBJ files in config
 OBJ_FORMAT_CFILES = \
+	config/obj-amigahunk.c \
 	config/obj-aout.c \
 	config/obj-coff.c \
 	config/obj-ecoff.c \
@@ -775,6 +776,7 @@ OBJ_FORMAT_CFILES = \
 	config/obj-som.c
 
 OBJ_FORMAT_HFILES = \
+	config/obj-amigahunk.h \
 	config/obj-aout.h \
 	config/obj-coff.h \
 	config/obj-ecoff.h \
@@ -1305,6 +1307,7 @@ distclean-compile:
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/literal.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/macro.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/messages.Po@am__quote@
+@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/obj-amigahunk.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/output-file.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/read.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/remap.Po@am__quote@













--- gas/read.c	2022-07-08 10:46:47.000000000 +0100
+++ gas/read.c	2025-11-07 11:25:07.053432525 +0000
@@ -252,6 +252,9 @@ static size_t get_non_macro_line_sb (sb
 static void generate_file_debug (void);
 static char *_find_end_of_line (char *, int, int, int);
 
+void ignore_rest_of_line (void);
+
+
 void
 read_begin (void)
 {
@@ -345,7 +348,7 @@ static struct htab *po_hash;
 
 static const pseudo_typeS potable[] = {
   {"abort", s_abort, 0},
-  {"align", s_align_ptwo, 0},
+  {"align", s_align_bytes, 0},
   {"altmacro", s_altmacro, 1},
   {"ascii", stringer, 8+0},
   {"asciz", stringer, 8+1},
@@ -421,6 +424,7 @@ static const pseudo_typeS potable[] = {
   {"global", s_globl, 0},
   {"globl", s_globl, 0},
   {"hword", cons, 2},
+  {"ident", s_ignore, 0},
   {"if", s_if, (int) O_ne},
   {"ifb", s_ifb, 1},
   {"ifc", s_ifc, 0},
@@ -451,6 +455,7 @@ static const pseudo_typeS potable[] = {
   {"list", listing_list, 1},	/* Turn listing on.  */
   {"llen", listing_psize, 1},
   {"long", cons, 4},
+  {"loc", s_ignore, 0},
   {"lsym", s_lsym, 0},
   {"macro", s_macro, 0},
   {"mexit", s_mexit, 0},
@@ -486,6 +491,7 @@ static const pseudo_typeS potable[] = {
   {"short", cons, 2},
   {"single", float_cons, 'f'},
 /* size  */
+  {"size", s_ignore, 0},
   {"space", s_space, 0},
   {"skip", s_space, 0},
   {"sleb128", s_leb128, 1},
@@ -1513,7 +1519,10 @@ s_align (signed int arg, int bytes_p)
   if (align > align_limit)
     {
       align = align_limit;
+      // Do not warn for to large alignment on Amiga
+#ifndef OBJ_AMIGAHUNK
       as_warn (_("alignment too large: %u assumed"), align_limit);
+#endif
     }
 
   if (*input_line_pointer != ',')
@@ -1779,8 +1788,11 @@ s_comm_internal (int param,
       S_SET_EXTERNAL (symbolP);
       S_SET_SEGMENT (symbolP, bfd_com_section_ptr);
     }
-
+#ifdef OBJ_AMIGAHUNK
+  ignore_rest_of_line ();
+#else
   demand_empty_rest_of_line ();
+#endif
  out:
   if (flag_mri)
     mri_comment_end (stop, stopc);
@@ -1857,6 +1869,7 @@ s_mri_common (int small ATTRIBUTE_UNUSED
   if (S_IS_DEFINED (sym) && !S_IS_COMMON (sym))
     {
       as_bad (_("symbol `%s' is already defined"), S_GET_NAME (sym));
+      ignore_rest_of_line ();
       mri_comment_end (stop, stopc);
       return;
     }
@@ -3865,9 +3878,11 @@ demand_empty_rest_of_line (void)
     input_line_pointer++;
   else
     {
-      if (ISPRINT (*input_line_pointer))
+      if (ISPRINT (*input_line_pointer)) {
+	if (*input_line_pointer != '|')
 	as_bad (_("junk at end of line, first unrecognized character is `%c'"),
 		 *input_line_pointer);
+      }
       else
 	as_bad (_("junk at end of line, first unrecognized character valued 0x%x"),
 		 *input_line_pointer);
@@ -3921,6 +3936,12 @@ pseudo_set (symbolS *symbolP)
   else
     (void) deferred_expression (&exp);
 
+#if defined(OBJ_AMIGAHUNK)
+// needed to get .stabs working
+  if (exp.X_op == O_absent && symbol_get_bfdsym (symbolP)->section == undefined_section)
+    exp.X_op = O_constant;
+#endif
+
   if (exp.X_op == O_illegal)
     as_bad (_("illegal expression"));
   else if (exp.X_op == O_absent)
@@ -4694,7 +4715,7 @@ emit_expr_fix (expressionS *exp, unsigne
 	return;
       }
   fix_new_exp (frag, p - frag->fr_literal + offset, size,
-	       exp, 0, r);
+	       exp, 0, r, 0);
 #endif
 }
 
@@ -5695,6 +5716,14 @@ demand_copy_string (int *lenP)
 
   len = 0;
   SKIP_WHITESPACE ();
+
+  if (ISDIGIT(*input_line_pointer))
+    {
+      while (ISDIGIT(*input_line_pointer))
+	++input_line_pointer;
+      SKIP_WHITESPACE ();
+    }
+
   if (*input_line_pointer == '\"')
     {
       input_line_pointer++;	/* Skip opening quote.  */

--- gas/read.h	2022-07-08 10:46:47.000000000 +0100
+++ gas/read.h	2025-11-07 11:25:07.053432525 +0000
@@ -171,6 +171,9 @@ extern void s_bundle_lock (int);
 extern void s_bundle_unlock (int);
 extern void s_comm (int);
 extern void s_data (int);
+#ifdef OBJ_AMIGAHUNK
+extern void s_data_amiga (int);
+#endif
 extern void s_desc (int);
 extern void s_else (int arg);
 extern void s_elseif (int arg);

--- gas/sb.c	2022-07-08 10:46:47.000000000 +0100
+++ gas/sb.c	2025-11-07 11:25:07.053432525 +0000
@@ -119,11 +119,12 @@ sb_scrub_and_add_sb (sb *ptr, sb *s)
      So we loop until the input S is consumed.  */
   while (1)
     {
-      size_t copy = s->len - (scrub_position - s->ptr);
+      size_t copy = s->len - (scrub_position - s->ptr) + do_scrub_pending ();
       if (copy == 0)
 	break;
       sb_check (ptr, copy);
-      ptr->len += do_scrub_chars (scrub_from_sb, ptr->ptr + ptr->len, copy);
+      ptr->len += do_scrub_chars (scrub_from_sb, ptr->ptr + ptr->len,
+				  ptr->max - ptr->len);
     }
 
   sb_to_scrub = 0;

--- gas/testsuite/config/default.exp	2022-07-08 10:46:47.000000000 +0100
+++ gas/testsuite/config/default.exp	2025-11-07 11:25:07.061432645 +0000
@@ -52,6 +52,14 @@ if ![info exists NMFLAGS] then {
     set NMFLAGS {}
 }
 
+if ![info exists SIZE] then {
+    set SIZE [findfile $base_dir/../../binutils/size]
+}
+
+if ![info exists SIZEFLAGS] then {
+    set SIZEFLAGS ""
+}
+
 if ![info exists OBJCOPY] then {
     set OBJCOPY [findfile $base_dir/../../binutils/objcopy]
 }

--- gas/testsuite/gas/arm/unwind-pacbti-m.d	2022-07-08 10:46:47.000000000 +0100
+++ gas/testsuite/gas/arm/unwind-pacbti-m.d	2025-11-07 11:25:07.309436362 +0000
@@ -8,4 +8,4 @@
 .*:     file format.*
 
 Contents of section .ARM.extab:
- 0000 (00840281 b40084b4 b0a8b4a3|81028400 b48400b4 a3b4a8b0) 00000000  .*
+ 0000 (84b40281 84b4a300 b0a8b400|8102b484 00a3b484 00b4a8b0) 00000000  .*

--- gas/testsuite/gas/arm/unwind-pacbti-m-readelf.d	2022-07-08 10:46:47.000000000 +0100
+++ gas/testsuite/gas/arm/unwind-pacbti-m-readelf.d	2025-11-07 11:25:07.309436362 +0000
@@ -10,11 +10,11 @@ Unwind section '.ARM.exidx' at offset 0x
 
 0x0 <foo>: @0x0
   Compact model index: 1
-  0x84 0x00 pop {r14}
   0xb4      pop {ra_auth_code}
   0x84 0x00 pop {r14}
-  0xb4      pop {ra_auth_code}
   0xa3      pop {r4, r5, r6, r7}
   0xb4      pop {ra_auth_code}
+  0x84 0x00 pop {r14}
+  0xb4      pop {ra_auth_code}
   0xa8      pop {r4, r14}
   0xb0      finish



--- gas/testsuite/gas/macros/macros.exp	2022-07-08 10:46:47.000000000 +0100
+++ gas/testsuite/gas/macros/macros.exp	2025-11-07 11:25:07.945445896 +0000
@@ -70,6 +70,7 @@ run_dump_test app2
 run_dump_test app3
 remote_download host "$srcdir/$subdir/app4b.s"
 run_dump_test app4
+run_dump_test app5
 
 run_list_test badarg ""
 

--- gas/write.c	2022-07-08 10:46:47.000000000 +0100
+++ gas/write.c	2025-11-07 11:25:08.681456928 +0000
@@ -84,8 +84,10 @@
 #define TC_FINALIZE_SYMS_BEFORE_SIZE_SEG 1
 #endif
 
+extern long
+md_pcrel_from_m68k (fixS *fixP, segT current_section);
 #ifndef	MD_PCREL_FROM_SECTION
-#define MD_PCREL_FROM_SECTION(FIX, SEC) md_pcrel_from (FIX)
+#define MD_PCREL_FROM_SECTION(FIX, SEC) md_pcrel_from_m68k (FIX, SEC)
 #endif
 
 #ifndef TC_FAKE_LABEL
@@ -141,6 +143,7 @@ fix_new_internal (fragS *frag,		/* Which
 		  offsetT offset,	/* X_add_number.  */
 		  int pcrel,		/* TRUE if PC-relative relocation.  */
 		  RELOC_ENUM r_type	/* Relocation type.  */,
+		  int baserel ATTRIBUTE_UNUSED,
 		  int at_beginning)	/* Add to the start of the list?  */
 {
   fixS *fixP;
@@ -179,6 +182,7 @@ fix_new_internal (fragS *frag,		/* Which
 #endif
 
 #ifdef TC_FIX_TYPE
+  fixP->tc_fix_data = baserel;
   TC_INIT_FIX_DATA (fixP);
 #endif
 
@@ -223,10 +227,11 @@ fix_new (fragS *frag,			/* Which frag?
 	 symbolS *add_symbol,		/* X_add_symbol.  */
 	 offsetT offset,		/* X_add_number.  */
 	 int pcrel,			/* TRUE if PC-relative relocation.  */
-	 RELOC_ENUM r_type		/* Relocation type.  */)
+	 RELOC_ENUM r_type,		/* Relocation type.  */
+	 int baserel)
 {
   return fix_new_internal (frag, where, size, add_symbol,
-			   (symbolS *) NULL, offset, pcrel, r_type, false);
+			   (symbolS *) NULL, offset, pcrel, r_type, baserel, false);
 }
 
 /* Create a fixup for an expression.  Currently we only support fixups
@@ -239,7 +244,8 @@ fix_new_exp (fragS *frag,		/* Which frag
 	     unsigned long size,	/* 1, 2, or 4 usually.  */
 	     expressionS *exp,		/* Expression.  */
 	     int pcrel,			/* TRUE if PC-relative relocation.  */
-	     RELOC_ENUM r_type		/* Relocation type.  */)
+	     RELOC_ENUM r_type,		/* Relocation type.  */
+	     int baserel)
 {
   symbolS *add = NULL;
   symbolS *sub = NULL;
@@ -265,7 +271,7 @@ fix_new_exp (fragS *frag,		/* Which frag
 	exp->X_add_symbol = stmp;
 	exp->X_add_number = 0;
 
-	return fix_new_exp (frag, where, size, exp, pcrel, r_type);
+	return fix_new_exp (frag, where, size, exp, pcrel, r_type, baserel);
       }
 
     case O_symbol_rva:
@@ -295,7 +301,7 @@ fix_new_exp (fragS *frag,		/* Which frag
     }
 
   return fix_new_internal (frag, where, size, add, sub, off, pcrel,
-			   r_type, false);
+			   r_type, baserel, false);
 }
 
 /* Create a fixup at the beginning of FRAG.  The arguments are the same
@@ -306,7 +312,7 @@ fix_at_start (fragS *frag, unsigned long
 	      offsetT offset, int pcrel, RELOC_ENUM r_type)
 {
   return fix_new_internal (frag, 0, size, add_symbol,
-			   (symbolS *) NULL, offset, pcrel, r_type, true);
+			   (symbolS *) NULL, offset, pcrel, r_type, 0, true);
 }
 
 /* Generic function to determine whether a fixup requires a relocation.  */
@@ -579,6 +585,12 @@ size_seg (bfd *abfd ATTRIBUTE_UNUSED, as
   if (size > 0 && ! seginfo->bss)
     flags |= SEC_HAS_CONTENTS;
 
+#ifdef OBJ_AMIGAHUNK
+  if (size == 0 && 0 == strncmp( sec->name, ".text", 5))
+      flags |= SEC_HAS_CONTENTS | SEC_CODE;
+//  fprintf(stderr, "%s: %d\n", sec->name, size);
+#endif
+
   flags &= ~SEC_RELOC;
   x = bfd_set_section_flags (sec, flags);
   gas_assert (x);
@@ -967,7 +979,8 @@ fixup_segment (fixS *fixP, segT this_seg
       add_number = fixP->fx_offset;
 
       if (fixP->fx_addsy != NULL)
-	add_symbol_segment = S_GET_SEGMENT (fixP->fx_addsy);
+//	add_symbol_segment = S_GET_SEGMENT (fixP->fx_addsy);
+	add_symbol_segment = S_IS_WEAK (fixP->fx_addsy) ? undefined_section : S_GET_SEGMENT (fixP->fx_addsy);
 
       if (fixP->fx_subsy != NULL)
 	{
@@ -1529,7 +1542,8 @@ compress_debug (bfd *abfd, asection *sec
       int out_size;
 
       gas_assert (f->fr_type == rs_fill);
-      if (f->fr_fix)
+      if (f->fr_fix
+	  || f->fr_var)
 	{
 	  out_size = compress_frag (strm, f->fr_literal, f->fr_fix,
 				    &last_newf, ob);
@@ -1652,7 +1666,11 @@ write_contents (bfd *abfd ATTRIBUTE_UNUS
       offsetT count;
 
       gas_assert (f->fr_type == rs_fill || f->fr_type == rs_fill_nop);
-      if (f->fr_fix)
+      if (f->fr_fix
+#ifdef OBJ_AMIGAHUNK
+	  || f->fr_var
+#endif
+	  )
 	{
 	  x = bfd_set_section_contents (stdoutput, sec,
 					f->fr_literal, (file_ptr) offset,
@@ -2244,11 +2262,11 @@ write_object_file (void)
 #ifdef TC_CONS_FIX_NEW
 	  TC_CONS_FIX_NEW (lie->frag,
 			   lie->word_goes_here - lie->frag->fr_literal,
-			   2, &exp, TC_PARSE_CONS_RETURN_NONE);
+			   2, &exp, TC_PARSE_CONS_RETURN_NONE, 0);
 #else
 	  fix_new_exp (lie->frag,
 		       lie->word_goes_here - lie->frag->fr_literal,
-		       2, &exp, 0, BFD_RELOC_16);
+		       2, &exp, 0, BFD_RELOC_16, 0);
 #endif
 	  *prevP = lie->next_broken_word;
 	}

--- gas/write.h	2022-07-08 10:46:47.000000000 +0100
+++ gas/write.h	2025-11-07 11:25:08.681456928 +0000
@@ -181,12 +181,15 @@ extern long relax_frag (segT, fragS *, l
 extern int relax_segment (struct frag *, segT, int);
 extern void number_to_chars_littleendian (char *, valueT, int);
 extern void number_to_chars_bigendian (char *, valueT, int);
-extern fixS *fix_new (fragS *, unsigned long, unsigned long, symbolS *,
-		      offsetT, int, bfd_reloc_code_real_type);
-extern fixS *fix_at_start (fragS *, unsigned long, symbolS *,
-			   offsetT, int, bfd_reloc_code_real_type);
-extern fixS *fix_new_exp (fragS *, unsigned long, unsigned long,
-			  expressionS *, int, bfd_reloc_code_real_type);
+extern fixS *fix_new
+  (fragS * frag, unsigned long where, unsigned long size, symbolS * add_symbol,
+   offsetT offset, int pcrel, bfd_reloc_code_real_type r_type, int baserel);
+extern fixS *fix_at_start
+  (fragS * frag, unsigned long size, symbolS * add_symbol,
+   offsetT offset, int pcrel, bfd_reloc_code_real_type r_type);
+extern fixS *fix_new_exp
+  (fragS * frag, unsigned long where, unsigned long size, expressionS *exp, int pcrel,
+   bfd_reloc_code_real_type r_type, int baserel);
 extern void write_print_statistics (FILE *);
 extern void as_bad_subtract (fixS *);
 








--- gold/configure	2022-07-08 10:58:08.000000000 +0100
+++ gold/configure	2025-11-07 11:25:10.053477493 +0000
@@ -813,6 +813,7 @@ infodir
 docdir
 oldincludedir
 includedir
+runstatedir
 localstatedir
 sharedstatedir
 sysconfdir
@@ -906,6 +907,7 @@ datadir='${datarootdir}'
 sysconfdir='${prefix}/etc'
 sharedstatedir='${prefix}/com'
 localstatedir='${prefix}/var'
+runstatedir='${localstatedir}/run'
 includedir='${prefix}/include'
 oldincludedir='/usr/include'
 docdir='${datarootdir}/doc/${PACKAGE_TARNAME}'
@@ -1158,6 +1160,15 @@ do
   | -silent | --silent | --silen | --sile | --sil)
     silent=yes ;;
 
+  -runstatedir | --runstatedir | --runstatedi | --runstated \
+  | --runstate | --runstat | --runsta | --runst | --runs \
+  | --run | --ru | --r)
+    ac_prev=runstatedir ;;
+  -runstatedir=* | --runstatedir=* | --runstatedi=* | --runstated=* \
+  | --runstate=* | --runstat=* | --runsta=* | --runst=* | --runs=* \
+  | --run=* | --ru=* | --r=*)
+    runstatedir=$ac_optarg ;;
+
   -sbindir | --sbindir | --sbindi | --sbind | --sbin | --sbi | --sb)
     ac_prev=sbindir ;;
   -sbindir=* | --sbindir=* | --sbindi=* | --sbind=* | --sbin=* \
@@ -1295,7 +1306,7 @@ fi
 for ac_var in	exec_prefix prefix bindir sbindir libexecdir datarootdir \
 		datadir sysconfdir sharedstatedir localstatedir includedir \
 		oldincludedir docdir infodir htmldir dvidir pdfdir psdir \
-		libdir localedir mandir
+		libdir localedir mandir runstatedir
 do
   eval ac_val=\$$ac_var
   # Remove trailing slashes.
@@ -1448,6 +1459,7 @@ Fine tuning of the installation director
   --sysconfdir=DIR        read-only single-machine data [PREFIX/etc]
   --sharedstatedir=DIR    modifiable architecture-independent data [PREFIX/com]
   --localstatedir=DIR     modifiable single-machine data [PREFIX/var]
+  --runstatedir=DIR       modifiable per-process data [LOCALSTATEDIR/run]
   --libdir=DIR            object code libraries [EPREFIX/lib]
   --includedir=DIR        C header files [PREFIX/include]
   --oldincludedir=DIR     C header files for non-gcc [/usr/include]

--- gold/Makefile.in	2022-07-08 10:58:08.000000000 +0100
+++ gold/Makefile.in	2025-11-07 11:25:10.045477373 +0000
@@ -641,6 +641,7 @@ pdfdir = @pdfdir@
 prefix = @prefix@
 program_transform_name = @program_transform_name@
 psdir = @psdir@
+runstatedir = @runstatedir@
 sbindir = @sbindir@
 sharedstatedir = @sharedstatedir@
 srcdir = @srcdir@












--- gold/powerpc.cc	2022-07-08 10:46:48.000000000 +0100
+++ gold/powerpc.cc	2025-11-07 11:25:10.081477913 +0000
@@ -4338,7 +4338,8 @@ template<int size, bool big_endian>
 void
 Output_data_plt_powerpc<size, big_endian>::do_write(Output_file* of)
 {
-  if (!this->sym_ents_.empty())
+  if (!this->sym_ents_.empty()
+      && !parameters->options().output_is_position_independent())
     {
       const section_size_type offset = this->offset();
       const section_size_type oview_size
@@ -7675,22 +7676,18 @@ Target_powerpc<size, big_endian>::Scan::
 
   switch (r_type)
     {
+    case elfcpp::R_PPC64_TOC:
+      if (size != 64)
+	break;
+      // Fall through.
     case elfcpp::R_POWERPC_NONE:
     case elfcpp::R_POWERPC_GNU_VTINHERIT:
     case elfcpp::R_POWERPC_GNU_VTENTRY:
-    case elfcpp::R_PPC64_TOC:
       // No symbol reference.
       break;
 
     case elfcpp::R_PPC64_ADDR64:
     case elfcpp::R_PPC64_UADDR64:
-    case elfcpp::R_POWERPC_ADDR32:
-    case elfcpp::R_POWERPC_UADDR32:
-    case elfcpp::R_POWERPC_ADDR16:
-    case elfcpp::R_POWERPC_UADDR16:
-    case elfcpp::R_POWERPC_ADDR16_LO:
-    case elfcpp::R_POWERPC_ADDR16_HI:
-    case elfcpp::R_POWERPC_ADDR16_HA:
     case elfcpp::R_PPC64_ADDR16_HIGHER34:
     case elfcpp::R_PPC64_ADDR16_HIGHERA34:
     case elfcpp::R_PPC64_ADDR16_HIGHEST34:
@@ -7700,6 +7697,16 @@ Target_powerpc<size, big_endian>::Scan::
     case elfcpp::R_PPC64_D34_HI30:
     case elfcpp::R_PPC64_D34_HA30:
     case elfcpp::R_PPC64_D28:
+      if (size != 64)
+	break;
+      // Fall through.
+    case elfcpp::R_POWERPC_ADDR32:
+    case elfcpp::R_POWERPC_UADDR32:
+    case elfcpp::R_POWERPC_ADDR16:
+    case elfcpp::R_POWERPC_UADDR16:
+    case elfcpp::R_POWERPC_ADDR16_LO:
+    case elfcpp::R_POWERPC_ADDR16_HI:
+    case elfcpp::R_POWERPC_ADDR16_HA:
       ref = Symbol::ABSOLUTE_REF;
       break;
 
@@ -7710,13 +7717,14 @@ Target_powerpc<size, big_endian>::Scan::
       ref = Symbol::FUNCTION_CALL | Symbol::ABSOLUTE_REF;
       break;
 
-    case elfcpp::R_PPC64_REL64:
-    case elfcpp::R_POWERPC_REL32:
     case elfcpp::R_PPC_LOCAL24PC:
-    case elfcpp::R_POWERPC_REL16:
-    case elfcpp::R_POWERPC_REL16_LO:
-    case elfcpp::R_POWERPC_REL16_HI:
-    case elfcpp::R_POWERPC_REL16_HA:
+      if (size != 32)
+	break;
+      // Fall through.
+      ref = Symbol::RELATIVE_REF;
+      break;
+
+    case elfcpp::R_PPC64_REL64:
     case elfcpp::R_PPC64_REL16_HIGH:
     case elfcpp::R_PPC64_REL16_HIGHA:
     case elfcpp::R_PPC64_REL16_HIGHER:
@@ -7729,36 +7737,45 @@ Target_powerpc<size, big_endian>::Scan::
     case elfcpp::R_PPC64_REL16_HIGHEST34:
     case elfcpp::R_PPC64_REL16_HIGHESTA34:
     case elfcpp::R_PPC64_PCREL28:
+      if (size != 64)
+	break;
+      // Fall through.
+    case elfcpp::R_POWERPC_REL32:
+    case elfcpp::R_POWERPC_REL16:
+    case elfcpp::R_POWERPC_REL16_LO:
+    case elfcpp::R_POWERPC_REL16_HI:
+    case elfcpp::R_POWERPC_REL16_HA:
       ref = Symbol::RELATIVE_REF;
       break;
 
+    case elfcpp::R_PPC_PLTREL24:
+      if (size != 32)
+	break;
+      ref = Symbol::FUNCTION_CALL | Symbol::RELATIVE_REF;
+      break;
+
     case elfcpp::R_PPC64_REL24_NOTOC:
-      if (size == 32)
+    case elfcpp::R_PPC64_REL24_P9NOTOC:
+    case elfcpp::R_PPC64_PLT16_LO_DS:
+    case elfcpp::R_PPC64_PLTSEQ_NOTOC:
+    case elfcpp::R_PPC64_PLTCALL_NOTOC:
+    case elfcpp::R_PPC64_PLT_PCREL34:
+    case elfcpp::R_PPC64_PLT_PCREL34_NOTOC:
+      if (size != 64)
 	break;
       // Fall through.
-    case elfcpp::R_PPC64_REL24_P9NOTOC:
     case elfcpp::R_POWERPC_REL24:
-    case elfcpp::R_PPC_PLTREL24:
     case elfcpp::R_POWERPC_REL14:
     case elfcpp::R_POWERPC_REL14_BRTAKEN:
     case elfcpp::R_POWERPC_REL14_BRNTAKEN:
     case elfcpp::R_POWERPC_PLT16_LO:
     case elfcpp::R_POWERPC_PLT16_HI:
     case elfcpp::R_POWERPC_PLT16_HA:
-    case elfcpp::R_PPC64_PLT16_LO_DS:
     case elfcpp::R_POWERPC_PLTSEQ:
-    case elfcpp::R_PPC64_PLTSEQ_NOTOC:
     case elfcpp::R_POWERPC_PLTCALL:
-    case elfcpp::R_PPC64_PLTCALL_NOTOC:
-    case elfcpp::R_PPC64_PLT_PCREL34:
-    case elfcpp::R_PPC64_PLT_PCREL34_NOTOC:
       ref = Symbol::FUNCTION_CALL | Symbol::RELATIVE_REF;
       break;
 
-    case elfcpp::R_POWERPC_GOT16:
-    case elfcpp::R_POWERPC_GOT16_LO:
-    case elfcpp::R_POWERPC_GOT16_HI:
-    case elfcpp::R_POWERPC_GOT16_HA:
     case elfcpp::R_PPC64_GOT16_DS:
     case elfcpp::R_PPC64_GOT16_LO_DS:
     case elfcpp::R_PPC64_GOT_PCREL34:
@@ -7768,11 +7785,16 @@ Target_powerpc<size, big_endian>::Scan::
     case elfcpp::R_PPC64_TOC16_HA:
     case elfcpp::R_PPC64_TOC16_DS:
     case elfcpp::R_PPC64_TOC16_LO_DS:
+      if (size != 64)
+	break;
+      // Fall through.
+    case elfcpp::R_POWERPC_GOT16:
+    case elfcpp::R_POWERPC_GOT16_LO:
+    case elfcpp::R_POWERPC_GOT16_HI:
+    case elfcpp::R_POWERPC_GOT16_HA:
       ref = Symbol::RELATIVE_REF;
       break;
 
-    case elfcpp::R_POWERPC_GOT_TPREL16:
-    case elfcpp::R_POWERPC_TLS:
     case elfcpp::R_PPC64_TLSGD:
     case elfcpp::R_PPC64_TLSLD:
     case elfcpp::R_PPC64_TPREL34:
@@ -7781,6 +7803,11 @@ Target_powerpc<size, big_endian>::Scan::
     case elfcpp::R_PPC64_GOT_TLSLD_PCREL34:
     case elfcpp::R_PPC64_GOT_TPREL_PCREL34:
     case elfcpp::R_PPC64_GOT_DTPREL_PCREL34:
+      if (size != 64)
+	break;
+      // Fall through.
+    case elfcpp::R_POWERPC_GOT_TPREL16:
+    case elfcpp::R_POWERPC_TLS:
       ref = Symbol::TLS_REF;
       break;
 
@@ -10671,10 +10698,8 @@ Target_powerpc<size, big_endian>::Reloca
   bool has_stub_value = false;
   bool localentry0 = false;
   unsigned int r_sym = elfcpp::elf_r_sym<size>(rela.get_r_info());
-  bool use_plt_offset
-    = (gsym != NULL
-       ? gsym->use_plt_offset(Scan::get_reference_flags(r_type, target))
-       : object->local_has_plt_offset(r_sym));
+  bool pltcall_to_direct = false;
+
   if (is_plt16_reloc<size>(r_type)
       || r_type == elfcpp::R_PPC64_PLT_PCREL34
       || r_type == elfcpp::R_PPC64_PLT_PCREL34_NOTOC
@@ -10688,21 +10713,18 @@ Target_powerpc<size, big_endian>::Reloca
       // that the decision depends on the PLTCALL reloc, and we don't
       // know the address of that instruction when processing others
       // in the sequence.  So the decision needs to be made in
-      // do_relax().  For now, don't optimise inline plt calls.
-      if (gsym)
-	use_plt_offset = gsym->has_plt_offset();
-    }
-  if (use_plt_offset
-      && !is_got_reloc(r_type)
-      && !is_plt16_reloc<size>(r_type)
-      && r_type != elfcpp::R_PPC64_PLT_PCREL34
-      && r_type != elfcpp::R_PPC64_PLT_PCREL34_NOTOC
-      && r_type != elfcpp::R_POWERPC_PLTSEQ
-      && r_type != elfcpp::R_POWERPC_PLTCALL
-      && r_type != elfcpp::R_PPC64_PLTSEQ_NOTOC
-      && r_type != elfcpp::R_PPC64_PLTCALL_NOTOC
-      && (!psymval->is_ifunc_symbol()
-	  || Scan::reloc_needs_plt_for_ifunc(target, object, r_type, false)))
+      // do_relax().
+      pltcall_to_direct = !(gsym != NULL
+			    ? gsym->has_plt_offset()
+			    : object->local_has_plt_offset(r_sym));
+    }
+  else if ((gsym != NULL
+	    ? gsym->use_plt_offset(Scan::get_reference_flags(r_type, target))
+	    : psymval->is_ifunc_symbol() && object->local_has_plt_offset(r_sym))
+	   && !is_got_reloc(r_type)
+	   && (!psymval->is_ifunc_symbol()
+	       || Scan::reloc_needs_plt_for_ifunc(target, object, r_type,
+						  false)))
     {
       if (size == 64
 	  && gsym != NULL
@@ -10796,9 +10818,9 @@ Target_powerpc<size, big_endian>::Reloca
       gold_assert(has_stub_value || !(os->flags() & elfcpp::SHF_ALLOC));
     }
 
-  if (use_plt_offset && (is_plt16_reloc<size>(r_type)
-			 || r_type == elfcpp::R_PPC64_PLT_PCREL34
-			 || r_type == elfcpp::R_PPC64_PLT_PCREL34_NOTOC))
+  if (!pltcall_to_direct && (is_plt16_reloc<size>(r_type)
+			     || r_type == elfcpp::R_PPC64_PLT_PCREL34
+			     || r_type == elfcpp::R_PPC64_PLT_PCREL34_NOTOC))
     {
       const Output_data_plt_powerpc<size, big_endian>* plt;
       if (gsym)
@@ -10826,7 +10848,7 @@ Target_powerpc<size, big_endian>::Reloca
 	    value -= target->toc_pointer();
 	}
     }
-  else if (!use_plt_offset
+  else if (pltcall_to_direct
 	   && (is_plt16_reloc<size>(r_type)
 	       || r_type == elfcpp::R_POWERPC_PLTSEQ
 	       || r_type == elfcpp::R_PPC64_PLTSEQ_NOTOC))
@@ -10835,7 +10857,7 @@ Target_powerpc<size, big_endian>::Reloca
       elfcpp::Swap<32, big_endian>::writeval(iview, nop);
       r_type = elfcpp::R_POWERPC_NONE;
     }
-  else if (!use_plt_offset
+  else if (pltcall_to_direct
 	   && (r_type == elfcpp::R_PPC64_PLT_PCREL34
 	       || r_type == elfcpp::R_PPC64_PLT_PCREL34_NOTOC))
     {
@@ -11316,8 +11338,8 @@ Target_powerpc<size, big_endian>::Reloca
     }
   else if (!has_stub_value)
     {
-      if (!use_plt_offset && (r_type == elfcpp::R_POWERPC_PLTCALL
-			      || r_type == elfcpp::R_PPC64_PLTCALL_NOTOC))
+      if (pltcall_to_direct && (r_type == elfcpp::R_POWERPC_PLTCALL
+				|| r_type == elfcpp::R_PPC64_PLTCALL_NOTOC))
 	{
 	  // PLTCALL without plt entry => convert to direct call
 	  Insn* iview = reinterpret_cast<Insn*>(view);

--- gold/testsuite/Makefile.in	2022-07-08 10:58:08.000000000 +0100
+++ gold/testsuite/Makefile.in	2025-11-07 11:25:10.093478093 +0000
@@ -2727,6 +2727,7 @@ pdfdir = @pdfdir@
 prefix = @prefix@
 program_transform_name = @program_transform_name@
 psdir = @psdir@
+runstatedir = @runstatedir@
 sbindir = @sbindir@
 sharedstatedir = @sharedstatedir@
 srcdir = @srcdir@





--- gprof/configure	2022-08-05 10:59:11.000000000 +0100
+++ gprof/configure	2025-11-07 11:25:10.145478872 +0000
@@ -1,6 +1,6 @@
 #! /bin/sh
 # Guess values for system-dependent variables and create Makefiles.
-# Generated by GNU Autoconf 2.69 for gprof 2.39.
+# Generated by GNU Autoconf 2.69 for gprof 2.39.0.
 #
 #
 # Copyright (C) 1992-1996, 1998-2012 Free Software Foundation, Inc.
@@ -587,8 +587,8 @@ MAKEFLAGS=
 # Identity of this package.
 PACKAGE_NAME='gprof'
 PACKAGE_TARNAME='gprof'
-PACKAGE_VERSION='2.39'
-PACKAGE_STRING='gprof 2.39'
+PACKAGE_VERSION='2.39.0'
+PACKAGE_STRING='gprof 2.39.0'
 PACKAGE_BUGREPORT=''
 PACKAGE_URL=''
 
@@ -1338,7 +1338,7 @@ if test "$ac_init_help" = "long"; then
   # Omit some internal or obsolete options to make the list less imposing.
   # This message is too long to be a string in the A/UX 3.1 sh.
   cat <<_ACEOF
-\`configure' configures gprof 2.39 to adapt to many kinds of systems.
+\`configure' configures gprof 2.39.0 to adapt to many kinds of systems.
 
 Usage: $0 [OPTION]... [VAR=VALUE]...
 
@@ -1409,7 +1409,7 @@ fi
 
 if test -n "$ac_init_help"; then
   case $ac_init_help in
-     short | recursive ) echo "Configuration of gprof 2.39:";;
+     short | recursive ) echo "Configuration of gprof 2.39.0:";;
    esac
   cat <<\_ACEOF
 
@@ -1520,7 +1520,7 @@ fi
 test -n "$ac_init_help" && exit $ac_status
 if $ac_init_version; then
   cat <<\_ACEOF
-gprof configure 2.39
+gprof configure 2.39.0
 generated by GNU Autoconf 2.69
 
 Copyright (C) 2012 Free Software Foundation, Inc.
@@ -1885,7 +1885,7 @@ cat >config.log <<_ACEOF
 This file contains any messages produced by compilers while
 running configure, to aid debugging if configure makes a mistake.
 
-It was created by gprof $as_me 2.39, which was
+It was created by gprof $as_me 2.39.0, which was
 generated by GNU Autoconf 2.69.  Invocation command line was
 
   $ $0 $@
@@ -2864,7 +2864,7 @@ fi
 
 # Define the identity of the package.
  PACKAGE='gprof'
- VERSION='2.39'
+ VERSION='2.39.0'
 
 
 cat >>confdefs.h <<_ACEOF
@@ -7932,7 +7932,7 @@ $as_echo_n "checking for $compiler optio
 
 case $host_os in
   # For platforms which do not support PIC, -DPIC is meaningless:
-  *djgpp*)
+  amiga* | *djgpp*)
     lt_prog_compiler_pic=
     ;;
   *)
@@ -12106,7 +12106,14 @@ test "x$prefix" = xNONE && prefix=$ac_de
 # Let make expand exec_prefix.
 test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
 
+case $target_os in
+  amiga*)
+    DEFS="-DHAVE_CONFIG_H -DTARGET_AMIGA"
+  ;;
+  *)
 DEFS=-DHAVE_CONFIG_H
+  ;;
+esac
 
 ac_libobjs=
 ac_ltlibobjs=
@@ -12554,7 +12561,7 @@ cat >>$CONFIG_STATUS <<\_ACEOF || ac_wri
 # report actual input values of CONFIG_FILES etc. instead of their
 # values after options handling.
 ac_log="
-This file was extended by gprof $as_me 2.39, which was
+This file was extended by gprof $as_me 2.39.0, which was
 generated by GNU Autoconf 2.69.  Invocation command line was
 
   CONFIG_FILES    = $CONFIG_FILES
@@ -12620,7 +12627,7 @@ _ACEOF
 cat >>$CONFIG_STATUS <<_ACEOF || ac_write_fail=1
 ac_cs_config="`$as_echo "$ac_configure_args" | sed 's/^ //; s/[\\""\`\$]/\\\\&/g'`"
 ac_cs_version="\\
-gprof config.status 2.39
+gprof config.status 2.39.0
 configured by $0, generated by GNU Autoconf 2.69,
   with options \\"\$ac_cs_config\\"
 



--- gprof/gmon_io.c	2022-07-08 10:46:48.000000000 +0100
+++ gprof/gmon_io.c	2025-11-07 11:25:10.145478872 +0000
@@ -476,8 +476,12 @@ gmon_out_read (const char *filename)
 	  histograms->lowpc = tmp.low_pc;
 	  histograms->highpc = tmp.high_pc;
 	  histograms->num_bins = hist_num_bins;
+#ifdef TARGET_AMIGA
+	  hist_scale = 1.;
+#else
 	  hist_scale = (double)((tmp.high_pc - tmp.low_pc) / sizeof (UNIT))
 	    / hist_num_bins;
+#endif
 	  histograms->sample = (int *) xmalloc (hist_num_bins * sizeof (int));
 	  memset (histograms->sample, 0,
 		  hist_num_bins * sizeof (int));



--- gprof/hertz.c	2022-07-08 10:46:48.000000000 +0100
+++ gprof/hertz.c	2025-11-07 11:25:10.145478872 +0000
@@ -33,6 +33,9 @@
 int
 hertz (void)
 {
+#ifdef TARGET_AMIGA
+  return 50;
+#else
 #ifdef HERTZ
   return HERTZ;
 #else /* ! defined (HERTZ) */
@@ -44,7 +47,9 @@ hertz (void)
   tim.it_value.tv_sec = 0;
   tim.it_value.tv_usec = 0;
   setitimer (ITIMER_REAL, &tim, 0);
+#ifndef __CYGWIN__
   setitimer (ITIMER_REAL, 0, &tim);
+#endif
   if (tim.it_interval.tv_usec >= 2)
     {
       return 1000000 / tim.it_interval.tv_usec;
@@ -60,4 +65,5 @@ hertz (void)
 #endif /* ! defined (__MSDOS__) */
 #endif /* ! defined (HAVE_SYSCONF) || ! defined (_SC_CLK_TCK) */
 #endif /* ! defined (HERTZ) */
+#endif /* TARGET_AMIGA */
 }


























--- gprof/symtab.c	2022-07-08 10:46:48.000000000 +0100
+++ gprof/symtab.c	2025-11-07 11:25:10.161479112 +0000
@@ -243,8 +243,8 @@ sym_lookup (Sym_Table *sym_tab, bfd_vma
 	  else
 	    {
 	      DBG (LOOKUPDEBUG,
-		   printf ("[sym_lookup] %d probes (symtab->len=%u)\n",
-			   probes, sym_tab->len - 1));
+		   printf ("[sym_lookup] %d probes (symtab->len=%u) for addr %08x\n",
+			   probes, sym_tab->len - 1, (unsigned)address));
 	      return &sym[mid];
 	    }
 	}

--- gprofng/configure	2022-08-05 10:59:35.000000000 +0100
+++ gprofng/configure	2025-11-07 11:25:10.173479292 +0000
@@ -1,6 +1,6 @@
 #! /bin/sh
 # Guess values for system-dependent variables and create Makefiles.
-# Generated by GNU Autoconf 2.69 for gprofng 2.39.
+# Generated by GNU Autoconf 2.69 for gprofng 2.39.0.
 #
 #
 # Copyright (C) 1992-1996, 1998-2012 Free Software Foundation, Inc.
@@ -587,8 +587,8 @@ MAKEFLAGS=
 # Identity of this package.
 PACKAGE_NAME='gprofng'
 PACKAGE_TARNAME='gprofng'
-PACKAGE_VERSION='2.39'
-PACKAGE_STRING='gprofng 2.39'
+PACKAGE_VERSION='2.39.0'
+PACKAGE_STRING='gprofng 2.39.0'
 PACKAGE_BUGREPORT=''
 PACKAGE_URL=''
 
@@ -1362,7 +1362,7 @@ if test "$ac_init_help" = "long"; then
   # Omit some internal or obsolete options to make the list less imposing.
   # This message is too long to be a string in the A/UX 3.1 sh.
   cat <<_ACEOF
-\`configure' configures gprofng 2.39 to adapt to many kinds of systems.
+\`configure' configures gprofng 2.39.0 to adapt to many kinds of systems.
 
 Usage: $0 [OPTION]... [VAR=VALUE]...
 
@@ -1433,7 +1433,7 @@ fi
 
 if test -n "$ac_init_help"; then
   case $ac_init_help in
-     short | recursive ) echo "Configuration of gprofng 2.39:";;
+     short | recursive ) echo "Configuration of gprofng 2.39.0:";;
    esac
   cat <<\_ACEOF
 
@@ -1547,7 +1547,7 @@ fi
 test -n "$ac_init_help" && exit $ac_status
 if $ac_init_version; then
   cat <<\_ACEOF
-gprofng configure 2.39
+gprofng configure 2.39.0
 generated by GNU Autoconf 2.69
 
 Copyright (C) 2012 Free Software Foundation, Inc.
@@ -2079,7 +2079,7 @@ cat >config.log <<_ACEOF
 This file contains any messages produced by compilers while
 running configure, to aid debugging if configure makes a mistake.
 
-It was created by gprofng $as_me 2.39, which was
+It was created by gprofng $as_me 2.39.0, which was
 generated by GNU Autoconf 2.69.  Invocation command line was
 
   $ $0 $@
@@ -3052,7 +3052,7 @@ fi
 
 # Define the identity of the package.
  PACKAGE='gprofng'
- VERSION='2.39'
+ VERSION='2.39.0'
 
 
 cat >>confdefs.h <<_ACEOF
@@ -17440,7 +17440,7 @@ cat >>$CONFIG_STATUS <<\_ACEOF || ac_wri
 # report actual input values of CONFIG_FILES etc. instead of their
 # values after options handling.
 ac_log="
-This file was extended by gprofng $as_me 2.39, which was
+This file was extended by gprofng $as_me 2.39.0, which was
 generated by GNU Autoconf 2.69.  Invocation command line was
 
   CONFIG_FILES    = $CONFIG_FILES
@@ -17506,7 +17506,7 @@ _ACEOF
 cat >>$CONFIG_STATUS <<_ACEOF || ac_write_fail=1
 ac_cs_config="`$as_echo "$ac_configure_args" | sed 's/^ //; s/[\\""\`\$]/\\\\&/g'`"
 ac_cs_version="\\
-gprofng config.status 2.39
+gprofng config.status 2.39.0
 configured by $0, generated by GNU Autoconf 2.69,
   with options \\"\$ac_cs_config\\"
 



--- gprofng/libcollector/configure	2022-08-05 10:59:39.000000000 +0100
+++ gprofng/libcollector/configure	2025-11-07 11:25:10.185479472 +0000
@@ -1,6 +1,6 @@
 #! /bin/sh
 # Guess values for system-dependent variables and create Makefiles.
-# Generated by GNU Autoconf 2.69 for gprofng 2.39.
+# Generated by GNU Autoconf 2.69 for gprofng 2.39.0.
 #
 #
 # Copyright (C) 1992-1996, 1998-2012 Free Software Foundation, Inc.
@@ -587,8 +587,8 @@ MAKEFLAGS=
 # Identity of this package.
 PACKAGE_NAME='gprofng'
 PACKAGE_TARNAME='gprofng'
-PACKAGE_VERSION='2.39'
-PACKAGE_STRING='gprofng 2.39'
+PACKAGE_VERSION='2.39.0'
+PACKAGE_STRING='gprofng 2.39.0'
 PACKAGE_BUGREPORT=''
 PACKAGE_URL=''
 
@@ -1325,7 +1325,7 @@ if test "$ac_init_help" = "long"; then
   # Omit some internal or obsolete options to make the list less imposing.
   # This message is too long to be a string in the A/UX 3.1 sh.
   cat <<_ACEOF
-\`configure' configures gprofng 2.39 to adapt to many kinds of systems.
+\`configure' configures gprofng 2.39.0 to adapt to many kinds of systems.
 
 Usage: $0 [OPTION]... [VAR=VALUE]...
 
@@ -1396,7 +1396,7 @@ fi
 
 if test -n "$ac_init_help"; then
   case $ac_init_help in
-     short | recursive ) echo "Configuration of gprofng 2.39:";;
+     short | recursive ) echo "Configuration of gprofng 2.39.0:";;
    esac
   cat <<\_ACEOF
 
@@ -1505,7 +1505,7 @@ fi
 test -n "$ac_init_help" && exit $ac_status
 if $ac_init_version; then
   cat <<\_ACEOF
-gprofng configure 2.39
+gprofng configure 2.39.0
 generated by GNU Autoconf 2.69
 
 Copyright (C) 2012 Free Software Foundation, Inc.
@@ -1991,7 +1991,7 @@ cat >config.log <<_ACEOF
 This file contains any messages produced by compilers while
 running configure, to aid debugging if configure makes a mistake.
 
-It was created by gprofng $as_me 2.39, which was
+It was created by gprofng $as_me 2.39.0, which was
 generated by GNU Autoconf 2.69.  Invocation command line was
 
   $ $0 $@
@@ -2968,7 +2968,7 @@ fi
 
 # Define the identity of the package.
  PACKAGE='gprofng'
- VERSION='2.39'
+ VERSION='2.39.0'
 
 
 cat >>confdefs.h <<_ACEOF
@@ -16098,7 +16098,7 @@ cat >>$CONFIG_STATUS <<\_ACEOF || ac_wri
 # report actual input values of CONFIG_FILES etc. instead of their
 # values after options handling.
 ac_log="
-This file was extended by gprofng $as_me 2.39, which was
+This file was extended by gprofng $as_me 2.39.0, which was
 generated by GNU Autoconf 2.69.  Invocation command line was
 
   CONFIG_FILES    = $CONFIG_FILES
@@ -16164,7 +16164,7 @@ _ACEOF
 cat >>$CONFIG_STATUS <<_ACEOF || ac_write_fail=1
 ac_cs_config="`$as_echo "$ac_configure_args" | sed 's/^ //; s/[\\""\`\$]/\\\\&/g'`"
 ac_cs_version="\\
-gprofng config.status 2.39
+gprofng config.status 2.39.0
 configured by $0, generated by GNU Autoconf 2.69,
   with options \\"\$ac_cs_config\\"
 

--- include/bfdlink.h	2022-07-08 10:46:48.000000000 +0100
+++ include/bfdlink.h	2025-11-07 11:25:10.245480371 +0000
@@ -204,6 +204,11 @@ struct bfd_link_hash_table
 {
   /* The hash table itself.  */
   struct bfd_hash_table table;
+  /* The back end which created this hash table.  This indicates the
+     type of the entries in the hash table, which is sometimes
+     important information when linking object files of different
+     types together.  */
+  const bfd_target *creator;
   /* A linked list of undefined and common symbols, linked through the
      next field in the bfd_link_hash_entry structure.  */
   struct bfd_link_hash_entry *undefs;

--- include/demangle.h	2022-07-08 10:46:48.000000000 +0100
+++ include/demangle.h	2025-11-07 11:25:10.249480431 +0000
@@ -80,7 +80,7 @@ extern "C" {
    for operator "->", even though the first is lucid style and the second
    is ARM style. (FIXME?) */
 
-extern enum demangling_styles
+extern LIBIBERTYDECL enum demangling_styles
 {
   no_demangling = -1,
   unknown_demangling = 0,

--- include/getopt.h	2022-07-08 10:46:48.000000000 +0100
+++ include/getopt.h	2025-11-07 11:25:10.261480611 +0000
@@ -22,6 +22,8 @@
 #ifndef _GETOPT_H
 #define _GETOPT_H 1
 
+#include "libiberty.h"
+
 #ifdef	__cplusplus
 extern "C" {
 #endif
@@ -32,7 +34,7 @@ extern "C" {
    Also, when `ordering' is RETURN_IN_ORDER,
    each non-option ARGV-element is returned here.  */
 
-extern char *optarg;
+extern LIBIBERTYDECL char *optarg;
 
 /* Index in ARGV of the next element to be scanned.
    This is used for communication to and from the caller
@@ -46,12 +48,12 @@ extern char *optarg;
    Otherwise, `optind' communicates from one call to the next
    how much of ARGV has been scanned so far.  */
 
-extern int optind;
+extern LIBIBERTYDECL int optind;
 
 /* Callers store zero here to inhibit the error message `getopt' prints
    for unrecognized options.  */
 
-extern int opterr;
+extern LIBIBERTYDECL int opterr;
 
 /* Set to an option character which was unrecognized.  */
 

--- include/libiberty.h	2022-07-08 10:46:48.000000000 +0100
+++ include/libiberty.h	2025-11-07 11:25:10.265480671 +0000
@@ -35,6 +35,10 @@
 #ifndef LIBIBERTY_H
 #define LIBIBERTY_H
 
+#ifndef LIBIBERTYDECL
+#define LIBIBERTYDECL
+#endif
+
 #ifdef __cplusplus
 extern "C" {
 #endif

--- include/opcode/m68k.h	2022-07-08 10:46:48.000000000 +0100
+++ include/opcode/m68k.h	2025-11-07 11:25:10.285480971 +0000
@@ -18,6 +18,10 @@
    Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
    02110-1301, USA.  */
 
+#ifndef BINUTILSDECL
+#define BINUTILSDECL
+#endif
+
 /* These are used as bit flags for the arch field in the m68k_opcode
    structure.  */
 #define	_m68k_undef  0
@@ -27,11 +31,12 @@
 #define	m68030   0x008
 #define	m68040   0x010
 #define m68060   0x020
-#define	m68881   0x040
-#define	m68851   0x080
-#define cpu32	 0x100		/* e.g., 68332 */
-#define fido_a   0x200
-#define m68k_mask  0x3ff
+#define m68080   0x040
+#define	m68881   0x080
+#define	m68851   0x100
+#define cpu32	 0x200		/* e.g., 68332 */
+#define fido_a   0x400
+#define m68k_mask  0x7ff
 
 #define mcfmac   0x400		/* ColdFire MAC. */
 #define mcfemac  0x800		/* ColdFire EMAC. */
@@ -46,14 +51,14 @@
 #define mcf_mask 0x7e400
 
 /* Handy aliases.  */
-#define	m68040up   (m68040 | m68060)
+#define	m68040up   (m68040 | m68060 | m68080)
 #define	m68030up   (m68030 | m68040up)
 #define	m68020up   (m68020 | m68030up)
 #define	m68010up   (m68010 | cpu32 | fido_a | m68020up)
 #define	m68000up   (m68000 | m68010up)
 
-#define	mfloat  (m68881 | m68040 | m68060)
-#define	mmmu    (m68851 | m68030 | m68040 | m68060)
+#define	mfloat  (m68881 | m68040 | m68060 | m68080)
+#define	mmmu    (m68851 | m68030 | m68040 | m68060 | m68080)
 
 /* The structure used to hold information for an opcode.  */
 
@@ -369,9 +374,9 @@ struct m68k_opcode_alias
    ]  first word, bit 10
 */
 
-extern const struct m68k_opcode m68k_opcodes[];
-extern const struct m68k_opcode_alias m68k_opcode_aliases[];
+extern BINUTILSDECL const struct m68k_opcode m68k_opcodes[];
+extern BINUTILSDECL const struct m68k_opcode_alias m68k_opcode_aliases[];
 
-extern const int m68k_numopcodes, m68k_numaliases;
+extern BINUTILSDECL const int m68k_numopcodes, m68k_numaliases;
 
 /* end of m68k-opcode.h */

--- include/safe-ctype.h	2022-07-08 10:46:48.000000000 +0100
+++ include/safe-ctype.h	2025-11-07 11:25:10.301481210 +0000
@@ -35,6 +35,8 @@ Boston, MA 02110-1301, USA.  */
 #ifndef SAFE_CTYPE_H
 #define SAFE_CTYPE_H
 
+#include "libiberty.h"
+
 /* Determine host character set.  */
 #define HOST_CHARSET_UNKNOWN 0
 #define HOST_CHARSET_ASCII   1
@@ -107,8 +109,8 @@ extern const unsigned short _sch_istable
 #define IS_SPACE_OR_NUL(c)	_sch_test(c, _sch_iscppsp)
 
 /* Character transformation.  */
-extern const unsigned char  _sch_toupper[256];
-extern const unsigned char  _sch_tolower[256];
+extern LIBIBERTYDECL const unsigned char  _sch_toupper[256];
+extern LIBIBERTYDECL const unsigned char  _sch_tolower[256];
 #define TOUPPER(c) _sch_toupper[(c) & 0xff]
 #define TOLOWER(c) _sch_tolower[(c) & 0xff]
 

--- intl/ChangeLog	2022-07-08 10:46:48.000000000 +0100
+++ intl/ChangeLog	2025-11-07 11:25:10.317481450 +0000
@@ -1,3 +1,7 @@
+2022-01-22  Nick Clifton  <nickc@redhat.com>
+
+	* configure: Regenerate.
+
 2021-06-14  Michael Forney  <mforney@mforney.org>
 
 	* configure: Regenerate.

--- intl/configure	2022-07-08 10:56:24.000000000 +0100
+++ intl/configure	2025-11-07 11:25:10.321481510 +0000
@@ -696,6 +696,7 @@ infodir
 docdir
 oldincludedir
 includedir
+runstatedir
 localstatedir
 sharedstatedir
 sysconfdir
@@ -775,6 +776,7 @@ datadir='${datarootdir}'
 sysconfdir='${prefix}/etc'
 sharedstatedir='${prefix}/com'
 localstatedir='${prefix}/var'
+runstatedir='${localstatedir}/run'
 includedir='${prefix}/include'
 oldincludedir='/usr/include'
 docdir='${datarootdir}/doc/${PACKAGE}'
@@ -1027,6 +1029,15 @@ do
   | -silent | --silent | --silen | --sile | --sil)
     silent=yes ;;
 
+  -runstatedir | --runstatedir | --runstatedi | --runstated \
+  | --runstate | --runstat | --runsta | --runst | --runs \
+  | --run | --ru | --r)
+    ac_prev=runstatedir ;;
+  -runstatedir=* | --runstatedir=* | --runstatedi=* | --runstated=* \
+  | --runstate=* | --runstat=* | --runsta=* | --runst=* | --runs=* \
+  | --run=* | --ru=* | --r=*)
+    runstatedir=$ac_optarg ;;
+
   -sbindir | --sbindir | --sbindi | --sbind | --sbin | --sbi | --sb)
     ac_prev=sbindir ;;
   -sbindir=* | --sbindir=* | --sbindi=* | --sbind=* | --sbin=* \
@@ -1164,7 +1175,7 @@ fi
 for ac_var in	exec_prefix prefix bindir sbindir libexecdir datarootdir \
 		datadir sysconfdir sharedstatedir localstatedir includedir \
 		oldincludedir docdir infodir htmldir dvidir pdfdir psdir \
-		libdir localedir mandir
+		libdir localedir mandir runstatedir
 do
   eval ac_val=\$$ac_var
   # Remove trailing slashes.
@@ -1317,6 +1328,7 @@ Fine tuning of the installation director
   --sysconfdir=DIR        read-only single-machine data [PREFIX/etc]
   --sharedstatedir=DIR    modifiable architecture-independent data [PREFIX/com]
   --localstatedir=DIR     modifiable single-machine data [PREFIX/var]
+  --runstatedir=DIR       modifiable per-process data [LOCALSTATEDIR/run]
   --libdir=DIR            object code libraries [EPREFIX/lib]
   --includedir=DIR        C header files [PREFIX/include]
   --oldincludedir=DIR     C header files for non-gcc [/usr/include]


--- ld/configure	2022-08-05 10:56:54.000000000 +0100
+++ ld/configure	2025-11-07 11:25:10.349481930 +0000
@@ -1,6 +1,6 @@
 #! /bin/sh
 # Guess values for system-dependent variables and create Makefiles.
-# Generated by GNU Autoconf 2.69 for ld 2.39.
+# Generated by GNU Autoconf 2.69 for ld 2.39.0.
 #
 #
 # Copyright (C) 1992-1996, 1998-2012 Free Software Foundation, Inc.
@@ -587,8 +587,8 @@ MAKEFLAGS=
 # Identity of this package.
 PACKAGE_NAME='ld'
 PACKAGE_TARNAME='ld'
-PACKAGE_VERSION='2.39'
-PACKAGE_STRING='ld 2.39'
+PACKAGE_VERSION='2.39.0'
+PACKAGE_STRING='ld 2.39.0'
 PACKAGE_BUGREPORT=''
 PACKAGE_URL=''
 
@@ -1416,7 +1416,7 @@ if test "$ac_init_help" = "long"; then
   # Omit some internal or obsolete options to make the list less imposing.
   # This message is too long to be a string in the A/UX 3.1 sh.
   cat <<_ACEOF
-\`configure' configures ld 2.39 to adapt to many kinds of systems.
+\`configure' configures ld 2.39.0 to adapt to many kinds of systems.
 
 Usage: $0 [OPTION]... [VAR=VALUE]...
 
@@ -1487,7 +1487,7 @@ fi
 
 if test -n "$ac_init_help"; then
   case $ac_init_help in
-     short | recursive ) echo "Configuration of ld 2.39:";;
+     short | recursive ) echo "Configuration of ld 2.39.0:";;
    esac
   cat <<\_ACEOF
 
@@ -1647,7 +1647,7 @@ fi
 test -n "$ac_init_help" && exit $ac_status
 if $ac_init_version; then
   cat <<\_ACEOF
-ld configure 2.39
+ld configure 2.39.0
 generated by GNU Autoconf 2.69
 
 Copyright (C) 2012 Free Software Foundation, Inc.
@@ -2362,7 +2362,7 @@ cat >config.log <<_ACEOF
 This file contains any messages produced by compilers while
 running configure, to aid debugging if configure makes a mistake.
 
-It was created by ld $as_me 2.39, which was
+It was created by ld $as_me 2.39.0, which was
 generated by GNU Autoconf 2.69.  Invocation command line was
 
   $ $0 $@
@@ -3345,7 +3345,7 @@ fi
 
 # Define the identity of the package.
  PACKAGE='ld'
- VERSION='2.39'
+ VERSION='2.39.0'
 
 
 cat >>confdefs.h <<_ACEOF
@@ -17933,7 +17933,7 @@ cat >>$CONFIG_STATUS <<\_ACEOF || ac_wri
 # report actual input values of CONFIG_FILES etc. instead of their
 # values after options handling.
 ac_log="
-This file was extended by ld $as_me 2.39, which was
+This file was extended by ld $as_me 2.39.0, which was
 generated by GNU Autoconf 2.69.  Invocation command line was
 
   CONFIG_FILES    = $CONFIG_FILES
@@ -17999,7 +17999,7 @@ _ACEOF
 cat >>$CONFIG_STATUS <<_ACEOF || ac_write_fail=1
 ac_cs_config="`$as_echo "$ac_configure_args" | sed 's/^ //; s/[\\""\`\$]/\\\\&/g'`"
 ac_cs_version="\\
-ld config.status 2.39
+ld config.status 2.39.0
 configured by $0, generated by GNU Autoconf 2.69,
   with options \\"\$ac_cs_config\\"
 

--- ld/configure.tgt	2022-07-29 08:37:48.000000000 +0100
+++ ld/configure.tgt	2025-11-07 11:25:10.349481930 +0000
@@ -495,6 +495,8 @@ m68*-*-netbsd*)	targ_emul=m68kelfnbsd
 			;;
 m68*-*-haiku*)		targ_emul=m68kelf
 			;;
+m68*-*-amigaos*)	targ_emul=amiga ; targ_extra_emuls="amiga_bss"; targ_extra_ofiles=""
+			;;			
 m68*-*-*)		targ_emul=m68kelf
 			;;
 mcore-*-pe)		targ_emul=mcorepe ;







--- ld/ldctor.c	2022-07-08 10:46:48.000000000 +0100
+++ ld/ldctor.c	2025-11-07 11:25:10.389482530 +0000
@@ -256,8 +256,13 @@ ldctor_build_sets (void)
       /* If the symbol is defined, we may have been invoked from
 	 collect, and the sets may already have been built, so we do
 	 not do anything.  */
-      if (p->h->type == bfd_link_hash_defined
-	  || p->h->type == bfd_link_hash_defweak)
+      /* dgv -- libnix v1.1 uses absolute sets that are also explicitly
+	 defined in the library so that the sets need to be build even
+	 if the symbol is defined */
+      if ((bfd_get_flavour (link_info.output_bfd) != bfd_target_amiga_flavour) &&
+	  (p->h->type == bfd_link_hash_defined
+	  || p->h->type == bfd_link_hash_defweak))
+
 	continue;
 
       /* For each set we build:
@@ -368,7 +373,11 @@ ldctor_build_sets (void)
 	  if (!bfd_is_abs_section (e->section))
 	    e->section->flags |= SEC_KEEP;
 
-	  if (bfd_link_relocatable (&link_info))
+	  /* dgv -- on the amiga, we want the constructors to be relocateable
+	     objects. However, this should be arranged somewhere else (FIXME) */
+	  if (bfd_link_relocatable (&link_info) ||
+	      (bfd_get_flavour (link_info.output_bfd) == bfd_target_amiga_flavour &&
+	       e->section != bfd_abs_section_ptr))
 	    lang_add_reloc (p->reloc, howto, e->section, e->name,
 			    exp_intop (e->value));
 	  else

--- ld/ldfile.c	2022-07-08 10:46:48.000000000 +0100
+++ ld/ldfile.c	2025-11-07 11:25:10.389482530 +0000
@@ -55,6 +55,10 @@ static char *slash = "/";
 #endif
 #endif
 
+#ifndef PARAMS
+#define PARAMS(x) x
+#endif
+
 typedef struct search_arch
 {
   char *name;
@@ -65,6 +69,32 @@ static search_dirs_type **search_tail_pt
 static search_arch_type *search_arch_head;
 static search_arch_type **search_arch_tail_ptr = &search_arch_head;
 
+/* Flavour support.  */
+
+
+static int n_flavors, flavors_len;
+static char **flavors;
+
+void
+ldfile_sort_flavors (void)
+{
+//  if (n_flavors > 1)
+//    qsort ((void *) flavors, n_flavors, sizeof (char **), flavors_cmp);
+}
+
+void
+ldfile_add_flavor (const char * name)
+{
+  n_flavors++;
+  if (flavors)
+    flavors = (char **) xrealloc ((void *)flavors, n_flavors * sizeof (char *));
+  else
+    flavors = (char **) xmalloc (sizeof (char *));
+  flavors [n_flavors-1] = (char *) name;
+  flavors_len += strlen (name) + 2;
+}
+
+
 /* Test whether a pathname, after canonicalization, is the same or a
    sub-directory of the sysroot directory.  */
 
@@ -328,12 +358,14 @@ ldfile_try_open_bfd (const char *attempt
 /* Search for and open the file specified by ENTRY.  If it is an
    archive, use ARCH, LIB and SUFFIX to modify the file name.  */
 
+#pragma GCC diagnostic ignored "-Wstack-usage="
 bool
 ldfile_open_file_search (const char *arch,
 			 lang_input_statement_type *entry,
 			 const char *lib,
 			 const char *suffix)
 {
+  char *flavor_dir = (char *) alloca (flavors_len + n_flavors + 1);
   search_dirs_type *search;
 
   /* If this is not an archive, try to open it in the current
@@ -361,6 +393,7 @@ ldfile_open_file_search (const char *arc
   for (search = search_head; search != NULL; search = search->next)
     {
       char *string;
+      int count, i;
 
       if (entry->flags.dynamic && !bfd_link_relocatable (&link_info))
 	{
@@ -368,11 +401,20 @@ ldfile_open_file_search (const char *arc
 	    return true;
 	}
 
+      for (count=n_flavors; count>=0; count--) {
+
+	*flavor_dir = '\0';
+	for (i=0; i<count; i++) {
+	  strcat (flavor_dir, flavors[i]);
+	  strcat (flavor_dir, slash);
+	}
+
+
       if (entry->flags.maybe_archive && !entry->flags.full_name_provided)
-	string = concat (search->name, slash, lib, entry->filename,
+	string = concat (search->name, slash, flavor_dir, lib, entry->filename,
 			 arch, suffix, (const char *) NULL);
       else
-	string = concat (search->name, slash, entry->filename,
+	string = concat (search->name, slash, flavor_dir, entry->filename,
 			 (const char *) 0);
 
       if (ldfile_try_open_bfd (string, entry))
@@ -383,6 +425,7 @@ ldfile_open_file_search (const char *arc
 
       free (string);
     }
+    }
 
   return false;
 }

--- ld/ldfile.h	2022-07-08 10:46:48.000000000 +0100
+++ ld/ldfile.h	2025-11-07 11:25:10.393482590 +0000
@@ -60,4 +60,10 @@ extern bool ldfile_open_file_search
   (const char *arch, struct lang_input_statement_struct *,
    const char *lib, const char *suffix);
 
+extern void
+ldfile_sort_flavors (void);
+extern void
+ldfile_add_flavor (const char * name);
+
+
 #endif




--- ld/ldlang.c	2022-07-25 08:04:15.000000000 +0100
+++ ld/ldlang.c	2025-11-07 11:25:10.393482590 +0000
@@ -7622,6 +7622,7 @@ lang_reset_memory_regions (void)
       os->processed_lma = false;
     }
 
+  if (link_info.output_bfd)
   for (o = link_info.output_bfd->sections; o != NULL; o = o->next)
     {
       /* Save the last size for possible use by bfd_relax_section.  */
@@ -8505,6 +8506,9 @@ lang_add_reloc (bfd_reloc_code_real_type
   p->section = section;
   p->name = name;
   p->addend_exp = addend;
+// AMIGA???  
+  if (reloc == BFD_RELOC_CTOR)
+	  p->addend_exp->value.value -= section->vma;
 
   p->addend_value = 0;
   p->output_section = NULL;

--- ld/ldlang.h	2022-07-08 10:46:48.000000000 +0100
+++ ld/ldlang.h	2025-11-07 11:25:10.393482590 +0000
@@ -324,6 +324,10 @@ typedef struct lang_input_statement_stru
   const char *target;
 
   struct lang_input_statement_flags flags;
+
+  /* Added for AMIGA support of section attributes */
+  int amiga_attribute;
+
 } lang_input_statement_type;
 
 typedef struct


--- ld/Makefile.am	2022-07-08 10:46:48.000000000 +0100
+++ ld/Makefile.am	2025-11-07 11:25:10.341481810 +0000
@@ -158,6 +158,8 @@ ALL_EMULATION_SOURCES = \
 	eaixrs6.c \
 	ealpha.c \
 	ealphavms.c \
+	eamiga.c \
+	eamiga_bss.c \
 	earcelf.c \
 	earclinux.c \
 	earclinux_nps.c \
@@ -465,9 +467,9 @@ ALL_64_EMULATIONS = $(ALL_64_EMULATION_S
 
 ALL_EMUL_EXTRA_OFILES = \
 	deffilep.@OBJEXT@ \
-	pe-dll.@OBJEXT@ \
-	ldelf.@OBJEXT@ \
-	ldelfgen.@OBJEXT@
+	pe-dll.@OBJEXT@ 
+#	ldelf.@OBJEXT@ \
+#	ldelfgen.@OBJEXT@
 
 ALL_64_EMUL_EXTRA_OFILES = \
 	pep-dll.@OBJEXT@

--- ld/Makefile.in	2022-08-05 10:56:53.000000000 +0100
+++ ld/Makefile.in	2025-11-07 11:25:10.341481810 +0000
@@ -655,6 +655,8 @@ ALL_EMULATION_SOURCES = \
 	eaixrs6.c \
 	ealpha.c \
 	ealphavms.c \
+	eamiga.c \
+	eamiga_bss.c \
 	earcelf.c \
 	earclinux.c \
 	earclinux_nps.c \
@@ -960,9 +962,9 @@ ALL_64_EMULATION_SOURCES = \
 ALL_64_EMULATIONS = $(ALL_64_EMULATION_SOURCES:.c=.@OBJEXT@)
 ALL_EMUL_EXTRA_OFILES = \
 	deffilep.@OBJEXT@ \
-	pe-dll.@OBJEXT@ \
-	ldelf.@OBJEXT@ \
-	ldelfgen.@OBJEXT@
+	pe-dll.@OBJEXT@ 
+#	ldelf.@OBJEXT@ \
+#	ldelfgen.@OBJEXT@
 
 ALL_64_EMUL_EXTRA_OFILES = \
 	pep-dll.@OBJEXT@
@@ -1261,6 +1263,8 @@ distclean-compile:
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/eaixrs6.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/ealpha.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/ealphavms.Po@am__quote@
+@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/eamiga.Po@am__quote@
+@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/eamiga_bss.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earcelf.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earclinux.Po@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earclinux_nps.Po@am__quote@
@@ -2318,6 +2322,8 @@ $(ALL_EMULATION_SOURCES) $(ALL_64_EMULAT
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earcelf.Pc@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earclinux.Pc@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earclinux_nps.Pc@am__quote@
+@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/eamiga.Pc@am__quote@
+@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/eamiga_bss.Pc@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earcv2elf.Pc@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earcv2elfx.Pc@am__quote@
 @AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/earm_wince_pe.Pc@am__quote@

--- ld/NEWS	2022-07-08 10:46:48.000000000 +0100
+++ ld/NEWS	2025-11-07 11:25:10.341481810 +0000
@@ -27,10 +27,10 @@ Changes in 2.39:
   --enable-warn-rwx-segments=no
      will make --no-warn-rwx-segments enabled by default.
      
-  --enable-defaul-execstack=no
+  --enable-default-execstack=no
      will stop the creation of an executable stack simply because an input file
      is missing a .note.GNU-stack section, even on architectures where this
-     ehaviour is the default.
+     behaviour is the default.
 
 * TYPE=<type> is now supported in an output section description to set the
   section type value.

--- ld/plugin.h	2022-07-08 10:46:48.000000000 +0100
+++ ld/plugin.h	2025-11-07 11:25:10.397482649 +0000
@@ -45,8 +45,10 @@ extern void plugin_load_plugins (void);
 /* Return name of plugin which caused an error in any of the above.  */
 extern const char *plugin_error_plugin (void);
 
+#ifndef _MSC_VER
 /* Call 'claim file' hook for all plugins.  */
 extern void plugin_maybe_claim (lang_input_statement_type *);
+#endif
 
 /* Call 'all symbols read' hook for all plugins.  */
 extern int plugin_call_all_symbols_read (void);































--- libctf/Makefile.in	2022-07-08 11:02:15.000000000 +0100
+++ libctf/Makefile.in	2025-11-07 11:25:11.373497279 +0000
@@ -502,6 +502,7 @@ pdfdir = @pdfdir@
 prefix = @prefix@
 program_transform_name = @program_transform_name@
 psdir = @psdir@
+runstatedir = @runstatedir@
 sbindir = @sbindir@
 sharedstatedir = @sharedstatedir@
 srcdir = @srcdir@


--- libiberty/filename_cmp.c	2022-07-08 10:46:48.000000000 +0100
+++ libiberty/filename_cmp.c	2025-11-07 11:25:11.421497999 +0000
@@ -71,10 +71,16 @@ filename_cmp (const char *s1, const char
 
 #if defined (HAVE_DOS_BASED_FILE_SYSTEM)
       /* On DOS-based file systems, the '/' and the '\' are equivalent.  */
-      if (c1 == '/')
-        c1 = '\\';
-      if (c2 == '/')
-        c2 = '\\';
+      if (c1 == '\\') {
+        if (s1[1] == '\\')
+          ++s1;
+        c1 = '/';
+      }
+      if (c2 == '\\') {
+        if (s2[1] == '\\')
+          ++s2;
+        c2 = '/';
+      }
 #endif
 
       if (c1 != c2)

--- libiberty/getopt.c	2022-07-08 10:46:48.000000000 +0100
+++ libiberty/getopt.c	2025-11-07 11:25:11.425498058 +0000
@@ -112,7 +112,7 @@
    Also, when `ordering' is RETURN_IN_ORDER,
    each non-option ARGV-element is returned here.  */
 
-char *optarg = NULL;
+LIBIBERTYDECL char *optarg = NULL;
 
 /* Index in ARGV of the next element to be scanned.
    This is used for communication to and from the caller
@@ -127,7 +127,7 @@ char *optarg = NULL;
    how much of ARGV has been scanned so far.  */
 
 /* 1003.2 says this must be 1 before any call.  */
-int optind = 1;
+LIBIBERTYDECL int optind = 1;
 
 /* Formerly, initialization of getopt depended on optind==0, which
    causes problems with re-calling getopt as programs generally don't

--- libiberty/gettimeofday.c	2022-07-08 10:46:48.000000000 +0100
+++ libiberty/gettimeofday.c	2025-11-07 11:25:11.425498058 +0000
@@ -18,8 +18,13 @@ that @var{tz} be NULL.  Returns 0 on suc
 
 */ 
 
+#ifdef __NEWLIB_H__
+int
+gettimeofday (struct timeval *tp, struct timezone * tz)
+#else
 int
 gettimeofday (struct timeval *tp, void *tz)
+#endif
 {
   if (tz)
     abort ();

--- libiberty/strsignal.c	2022-07-08 10:46:48.000000000 +0100
+++ libiberty/strsignal.c	2025-11-07 11:25:11.433498178 +0000
@@ -404,7 +404,7 @@ call to @code{strsignal}.
 
 #ifndef HAVE_STRSIGNAL
 
-char *
+const char *
 strsignal (int signo)
 {
   char *msg;
@@ -549,7 +549,7 @@ followed by a newline.
 */
 
 #ifndef HAVE_PSIGNAL
-
+#ifndef AMIGA
 void
 psignal (int signo, char *message)
 {
@@ -568,7 +568,7 @@ psignal (int signo, char *message)
 }
 
 #endif	/* ! HAVE_PSIGNAL */
-
+#endif
 
 /* A simple little main that does nothing but print all the signal translations
    if MAIN is defined and this file is compiled and linked. */

--- libiberty/xatexit.c	2022-07-08 10:46:48.000000000 +0100
+++ libiberty/xatexit.c	2025-11-07 11:25:11.437498238 +0000
@@ -37,8 +37,10 @@ failure.  If you use @code{xatexit} to r
 #include <unixlib.h>
 #else
 /* For systems with larger pointers than ints, this must be declared.  */
+#ifndef _MSC_VER
 void *malloc (size_t);
 #endif
+#endif
 
 static void xatexit_cleanup (void);
 

--- libiberty/xmalloc.c	2022-07-08 10:46:48.000000000 +0100
+++ libiberty/xmalloc.c	2025-11-07 11:25:11.437498238 +0000
@@ -84,9 +84,14 @@ function will be called to print an erro
 #    ifdef __cplusplus
 extern "C" {
 #    endif /* __cplusplus */
+
+;
+#ifndef _MSC_VER
 void *malloc (size_t);
 void *realloc (void *, size_t);
 void *calloc (size_t, size_t);
+#endif
+
 #ifdef HAVE_SBRK
 void *sbrk (ptrdiff_t);
 #endif

--- libiberty/xstrerror.c	2022-07-08 10:46:48.000000000 +0100
+++ libiberty/xstrerror.c	2025-11-07 11:25:11.437498238 +0000
@@ -34,6 +34,7 @@ extern char *strerror (int,...);
 
 
 #ifndef DONT_DECLARE_STRERROR
+#ifndef _MSC_VER
 #  ifdef __cplusplus
 extern "C" {
 #  endif /* __cplusplus */
@@ -42,6 +43,7 @@ extern char *strerror (int);
 }
 #  endif /* __cplusplus */
 #endif
+#endif
 
 /* If strerror returns NULL, we'll format the number into a static buffer.  */
 


--- opcodes/configure	2022-08-05 10:54:59.000000000 +0100
+++ opcodes/configure	2025-11-07 11:25:11.477498838 +0000
@@ -1,6 +1,6 @@
 #! /bin/sh
 # Guess values for system-dependent variables and create Makefiles.
-# Generated by GNU Autoconf 2.69 for opcodes 2.39.
+# Generated by GNU Autoconf 2.69 for opcodes 2.39.0.
 #
 #
 # Copyright (C) 1992-1996, 1998-2012 Free Software Foundation, Inc.
@@ -587,8 +587,8 @@ MAKEFLAGS=
 # Identity of this package.
 PACKAGE_NAME='opcodes'
 PACKAGE_TARNAME='opcodes'
-PACKAGE_VERSION='2.39'
-PACKAGE_STRING='opcodes 2.39'
+PACKAGE_VERSION='2.39.0'
+PACKAGE_STRING='opcodes 2.39.0'
 PACKAGE_BUGREPORT=''
 PACKAGE_URL=''
 
@@ -1360,7 +1360,7 @@ if test "$ac_init_help" = "long"; then
   # Omit some internal or obsolete options to make the list less imposing.
   # This message is too long to be a string in the A/UX 3.1 sh.
   cat <<_ACEOF
-\`configure' configures opcodes 2.39 to adapt to many kinds of systems.
+\`configure' configures opcodes 2.39.0 to adapt to many kinds of systems.
 
 Usage: $0 [OPTION]... [VAR=VALUE]...
 
@@ -1431,7 +1431,7 @@ fi
 
 if test -n "$ac_init_help"; then
   case $ac_init_help in
-     short | recursive ) echo "Configuration of opcodes 2.39:";;
+     short | recursive ) echo "Configuration of opcodes 2.39.0:";;
    esac
   cat <<\_ACEOF
 
@@ -1545,7 +1545,7 @@ fi
 test -n "$ac_init_help" && exit $ac_status
 if $ac_init_version; then
   cat <<\_ACEOF
-opcodes configure 2.39
+opcodes configure 2.39.0
 generated by GNU Autoconf 2.69
 
 Copyright (C) 2012 Free Software Foundation, Inc.
@@ -2139,7 +2139,7 @@ cat >config.log <<_ACEOF
 This file contains any messages produced by compilers while
 running configure, to aid debugging if configure makes a mistake.
 
-It was created by opcodes $as_me 2.39, which was
+It was created by opcodes $as_me 2.39.0, which was
 generated by GNU Autoconf 2.69.  Invocation command line was
 
   $ $0 $@
@@ -3118,7 +3118,7 @@ fi
 
 # Define the identity of the package.
  PACKAGE='opcodes'
- VERSION='2.39'
+ VERSION='2.39.0'
 
 
 cat >>confdefs.h <<_ACEOF
@@ -13173,7 +13173,7 @@ cat >>$CONFIG_STATUS <<\_ACEOF || ac_wri
 # report actual input values of CONFIG_FILES etc. instead of their
 # values after options handling.
 ac_log="
-This file was extended by opcodes $as_me 2.39, which was
+This file was extended by opcodes $as_me 2.39.0, which was
 generated by GNU Autoconf 2.69.  Invocation command line was
 
   CONFIG_FILES    = $CONFIG_FILES
@@ -13239,7 +13239,7 @@ _ACEOF
 cat >>$CONFIG_STATUS <<_ACEOF || ac_write_fail=1
 ac_cs_config="`$as_echo "$ac_configure_args" | sed 's/^ //; s/[\\""\`\$]/\\\\&/g'`"
 ac_cs_version="\\
-opcodes config.status 2.39
+opcodes config.status 2.39.0
 configured by $0, generated by GNU Autoconf 2.69,
   with options \\"\$ac_cs_config\\"
 

--- opcodes/disassemble.c	2022-07-25 08:04:15.000000000 +0100
+++ opcodes/disassemble.c	2025-11-07 11:25:11.485498958 +0000
@@ -728,7 +728,10 @@ disassemble_init_for_target (struct disa
     case bfd_arch_nds32:
       disassemble_init_nds32 (info);
       break;
- #endif
+#endif
+    case bfd_arch_m68k:
+      info->disassembler_needs_relocs = true;
+      break;
     default:
       break;
     }

--- opcodes/m68k-dis.c	2022-07-08 10:46:48.000000000 +0100
+++ opcodes/m68k-dis.c	2025-11-07 11:25:11.585500457 +0000
@@ -26,10 +26,68 @@
 #include "cpu-m68k.h"
 #include "opcode/m68k.h"
 
+#define MOTOROLA 1
+#define TARGET_AMIGA 1
+
+#ifdef MOTOROLA
+/* print as signed decimal. */
+#undef sprintf_vma
+#define sprintf_vma(b,n) sprintf(b,"%d",(int)n)
+#endif
+
+#ifdef TARGET_AMIGA
+/* Extra info to pass to the disassembler address printing function.  */
+/* Extra info to pass to the section disassembler and address printing
+   function.  */
+struct objdump_disasm_info
+{
+  bfd *              abfd;
+  asection *         sec;
+  bool        require_sec;
+  disassembler_ftype disassemble_fn;
+  const char *       symbol;
+  arelent *          reloc;
+  arelent ***        relppp; // pointer to relocations
+  bfd_vma            vma; // code position
+  unsigned char *    buffer; // vma + *pp - buffer determines relppp usage
+  unsigned char **   pp; // current position in buffer
+};
+
+/* Support display of symbols in baserel offsets. */
+void print_m68k_disassembler_options (FILE * stream);
+static void
+parse_disassembler_options (const char *);
+#endif
+
+
 /* Local function prototypes.  */
+#ifdef MOTOROLA
+static int dump_baserel;
 
 const char * const fpcr_names[] =
 {
+  "", "fpiar", "fpsr", "fpiar/fpsr", "fpcr",
+  "fpiar/fpcr", "fpsr/fpcr", "fpiar/fpsr/fpcr"
+};
+
+static char *const reg_names[] =
+{
+  "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
+  "a0", "a1", "a2", "a3", "a4", "a5", "a6", "sp",
+  "ps", "pc"
+};
+
+/* Name of register halves for MAC/EMAC.
+   Seperate from reg_names since 'spu', 'fpl' look weird.  */
+static char *const reg_half_names[] =
+{
+  "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
+  "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
+  "ps", "pc"
+};
+#else
+const char * const fpcr_names[] =
+{
   "", "%fpiar", "%fpsr", "%fpiar/%fpsr", "%fpcr",
   "%fpiar/%fpcr", "%fpsr/%fpcr", "%fpiar/%fpsr/%fpcr"
 };
@@ -49,6 +107,7 @@ static char *const reg_half_names[] =
   "%a0", "%a1", "%a2", "%a3", "%a4", "%a5", "%a6", "%a7",
   "%ps", "%pc"
 };
+#endif
 
 /* Sign-extend an (unsigned char).  */
 #if __STDC__ == 1
@@ -510,22 +569,66 @@ print_base (int regno, bfd_vma disp, dis
 {
   if (regno == -1)
     {
+#ifdef MOTOROLA
+      (*info->print_address_func) (disp, info);
+      (*info->fprintf_func) (info->stream, ",pc");
+#else
       (*info->fprintf_func) (info->stream, "%%pc@(");
       (*info->print_address_func) (disp, info);
+#endif
     }
   else
     {
       char buf[50];
 
+#ifdef TARGET_AMIGA
+      /* Dump the symbol instead of the number*/
+      if (dump_baserel && regno == 12)
+	{
+	  static int offset;
+
+	  /* Swap section to .data */
+	  struct objdump_disasm_info *aux =
+	      (struct objdump_disasm_info *) info->application_data;
+	  asection * text = aux->sec;
+	  aux->sec = text->next;
+
+	  /* Try handling a4 set to 0 or set to -0x7ffe.
+	   * TODO: search the correct offset via lea statement.
+	   */
+	  if (disp & 0x80000000)
+	    offset = 0x7ffe;
+	  (*info->print_address_func) (disp + offset, info);
+
+	  /* restore section to .text */
+	  aux->sec = text;
+	}
+      else
+	{
+	  sprintf_vma (buf, disp);
+          (*info->fprintf_func) (info->stream, "%s", buf);
+	}
+#endif
+#ifdef MOTOROLA
+      if (regno == -2)
+	;
+      else if (regno == -3)
+	(*info->fprintf_func) (info->stream, ",zpc");
+      else
+	(*info->fprintf_func) (info->stream, ",%s", reg_names[regno]);
+#else
       if (regno == -2)
 	(*info->fprintf_func) (info->stream, "@(");
       else if (regno == -3)
 	(*info->fprintf_func) (info->stream, "%%zpc@(");
       else
 	(*info->fprintf_func) (info->stream, "%s@(", reg_names[regno]);
+#endif
 
+#ifndef TARGET_AMIGA
       sprintf_vma (buf, disp);
       (*info->fprintf_func) (info->stream, "%s", buf);
+#endif
     }
 }
 
@@ -541,7 +644,11 @@ print_indexed (int basereg,
 	       disassemble_info *info)
 {
   int word;
+#ifdef MOTOROLA
+  static char *const scales[] = { "", "*2", "*4", "*8" };
+#else
   static char *const scales[] = { "", ":2", ":4", ":8" };
+#endif
   bfd_vma base_disp;
   bfd_vma outer_disp;
   char buf[40];
@@ -551,7 +658,11 @@ print_indexed (int basereg,
 
   /* Generate the text for the index register.
      Where this will be output is not yet determined.  */
+#ifdef MOTOROLA
+  sprintf (buf, "%s.%c%s",
+#else
   sprintf (buf, "%s:%c%s",
+#endif
 	   reg_names[(word >> 12) & 0xf],
 	   (word & 0x800) ? 'l' : 'w',
 	   scales[(word >> 9) & 3]);
@@ -565,6 +676,9 @@ print_indexed (int basereg,
 	base_disp -= 0x100;
       if (basereg == -1)
 	base_disp += addr;
+#ifdef MOTOROLA
+      (*info->fprintf_func) (info->stream, "(");
+#endif
       print_base (basereg, base_disp, info);
       (*info->fprintf_func) (info->stream, ",%s)", buf);
       return p;
@@ -596,6 +710,9 @@ print_indexed (int basereg,
   /* Handle single-level case (not indirect).  */
   if ((word & 7) == 0)
     {
+#ifdef MOTOROLA
+      (*info->fprintf_func) (info->stream, "(");
+#endif
       print_base (basereg, base_disp, info);
       if (buf[0] != '\0')
 	(*info->fprintf_func) (info->stream, ",%s", buf);
@@ -614,6 +731,9 @@ print_indexed (int basereg,
       NEXTLONG (p, outer_disp, NULL);
     }
 
+#ifdef MOTOROLA
+  (*info->fprintf_func) (info->stream, "([");
+#endif
   print_base (basereg, base_disp, info);
   if ((word & 4) == 0 && buf[0] != '\0')
     {
@@ -621,10 +741,18 @@ print_indexed (int basereg,
       buf[0] = '\0';
     }
   sprintf_vma (vmabuf, outer_disp);
+#ifdef MOTOROLA
+  (*info->fprintf_func) (info->stream, "]");
+  if (buf[0] != '\0')
+    (*info->fprintf_func) (info->stream, ",%s", buf);
+  (*info->fprintf_func) (info->stream, ",%s", vmabuf);
+  (*info->fprintf_func) (info->stream, ")");
+#else
   (*info->fprintf_func) (info->stream, ")@(%s", vmabuf);
   if (buf[0] != '\0')
     (*info->fprintf_func) (info->stream, ",%s", buf);
   (*info->fprintf_func) (info->stream, ")");
+#endif
 
   return p;
 }
@@ -660,6 +788,11 @@ print_insn_arg (const char *d,
   bfd_signed_vma disp;
   unsigned int uval;
 
+  struct objdump_disasm_info * aux = (struct objdump_disasm_info *) info->application_data;
+  aux->buffer = buffer;
+  aux->pp = &p;
+
+
   switch (*d)
     {
     case 'c':		/* Cache identifier.  */
@@ -673,7 +806,11 @@ print_insn_arg (const char *d,
     case 'a':		/* Address register indirect only. Cf. case '+'.  */
       {
 	FETCH_ARG (3, val);
+#ifdef MOTOROLA
+	(*info->fprintf_func) (info->stream, "(%s)", reg_names[val + 8]);
+#else
 	(*info->fprintf_func) (info->stream, "%s@", reg_names[val + 8]);
+#endif
         break;
       }
 
@@ -685,27 +822,51 @@ print_insn_arg (const char *d,
       }
 
     case 'C':
+#ifdef MOTOROLA
+      (*info->fprintf_func) (info->stream, "ccr");
+#else
       (*info->fprintf_func) (info->stream, "%%ccr");
+#endif
       break;
 
     case 'S':
+#ifdef MOTOROLA
+      (*info->fprintf_func) (info->stream, "sr");
+#else
       (*info->fprintf_func) (info->stream, "%%sr");
+#endif
       break;
 
     case 'U':
+#ifdef MOTOROLA
+      (*info->fprintf_func) (info->stream, "usp");
+#else
       (*info->fprintf_func) (info->stream, "%%usp");
+#endif
       break;
 
     case 'E':
+#ifdef MOTOROLA
+      (*info->fprintf_func) (info->stream, "acc");
+#else
       (*info->fprintf_func) (info->stream, "%%acc");
+#endif
       break;
 
     case 'G':
+#ifdef MOTOROLA
+      (*info->fprintf_func) (info->stream, "macsr");
+#else
       (*info->fprintf_func) (info->stream, "%%macsr");
+#endif
       break;
 
     case 'H':
+#ifdef MOTOROLA
+      (*info->fprintf_func) (info->stream, "mask");
+#else
       (*info->fprintf_func) (info->stream, "%%mask");
+#endif
       break;
 
     case 'J':
@@ -714,6 +875,40 @@ print_insn_arg (const char *d,
 	   same address different names.  The tables below try to get it right
 	   using info->mach, but only for v4e.  */
 	struct regname { char * name; int value; };
+#ifdef MOTOROLA
+	static const struct regname names[] =
+	  {
+	    {"sfc", 0x000}, {"dfc", 0x001}, {"cacr", 0x002},
+	    {"tc",  0x003}, {"itt0",0x004}, {"itt1", 0x005},
+	    {"dtt0",0x006}, {"dtt1",0x007}, {"buscr",0x008},
+	    {"rgpiobar", 0x009}, {"acr4",0x00c},
+	    {"acr5",0x00d}, {"acr6",0x00e}, {"acr7", 0x00f},
+	    {"usp", 0x800}, {"vbr", 0x801}, {"caar", 0x802},
+	    {"msp", 0x803}, {"isp", 0x804},
+	    {"pc", 0x80f},
+	    /* Reg c04 is sometimes called flashbar or rambar.
+	       Reg c05 is also sometimes called rambar.  */
+	    {"rambar0", 0xc04}, {"rambar1", 0xc05},
+
+	    /* reg c0e is sometimes called mbar2 or secmbar.
+	       reg c0f is sometimes called mbar.  */
+	    {"mbar0", 0xc0e}, {"mbar1", 0xc0f},
+
+	    /* Should we be calling this psr like we do in case 'Y'?  */
+	    {"mmusr",0x805},
+
+	    {"urp", 0x806}, {"srp", 0x807}, {"pcr", 0x808},
+
+	    /* Fido added these.  */
+	    {"cac", 0xffe}, {"mbo", 0xfff}
+	};
+	/* Alternate names for v4e (MCF5407/5445x/MCF547x/MCF548x), at least.  */
+	static const struct regname names_v4e[] =
+	  {
+	    {"asid",0x003}, {"acr0",0x004}, {"acr1",0x005},
+	    {"acr2",0x006}, {"acr3",0x007}, {"mmubar",0x008},
+	  };
+#else
 	static const struct regname names[] =
 	  {
 	    {"%sfc", 0x000}, {"%dfc", 0x001}, {"%cacr", 0x002},
@@ -746,6 +941,7 @@ print_insn_arg (const char *d,
 	    {"%asid",0x003}, {"%acr0",0x004}, {"%acr1",0x005},
 	    {"%acr2",0x006}, {"%acr3",0x007}, {"%mmubar",0x008},
 	  };
+#endif
 	unsigned int arch_mask;
 
 	arch_mask = bfd_m68k_mach_to_features (info->mach);
@@ -837,15 +1033,23 @@ print_insn_arg (const char *d,
 
     case 'r':
       FETCH_ARG (4, regno);
+#ifdef MOTOROLA
+      (*info->fprintf_func) (info->stream, "(%s)", reg_names[regno]);
+#else
       if (regno > 7)
 	(*info->fprintf_func) (info->stream, "%s@", reg_names[regno]);
       else
 	(*info->fprintf_func) (info->stream, "@(%s)", reg_names[regno]);
+#endif
       break;
 
     case 'F':
       FETCH_ARG (3, val);
+#ifdef MOTOROLA
+      (*info->fprintf_func) (info->stream, "fp%d", val);
+#else
       (*info->fprintf_func) (info->stream, "%%fp%d", val);
+#endif
       break;
 
     case 'O':
@@ -858,12 +1062,20 @@ print_insn_arg (const char *d,
 
     case '+':
       FETCH_ARG (3, val);
+#ifdef MOTOROLA
+      (*info->fprintf_func) (info->stream, "(%s)+", reg_names[val + 8]);
+#else
       (*info->fprintf_func) (info->stream, "%s@+", reg_names[val + 8]);
+#endif
       break;
 
     case '-':
       FETCH_ARG (3, val);
+#ifdef MOTOROLA
+      (*info->fprintf_func) (info->stream, "-(%s)", reg_names[val + 8]);
+#else
       (*info->fprintf_func) (info->stream, "%s@-", reg_names[val + 8]);
+#endif
       break;
 
     case 'k':
@@ -942,7 +1154,11 @@ print_insn_arg (const char *d,
 
 	NEXTWORD (p, val, PRINT_INSN_ARG_MEMORY_ERROR);
 	FETCH_ARG (3, val1);
+#ifdef MOTOROLA
+	(*info->fprintf_func) (info->stream, "%d(%s)", val, reg_names[val1 + 8]);
+#else
 	(*info->fprintf_func) (info->stream, "%s@(%d)", reg_names[val1 + 8], val);
+#endif
 	break;
       }
 
@@ -1036,20 +1252,43 @@ print_insn_arg (const char *d,
 	  break;
 
 	case 2:
+#ifdef MOTOROLA
+	  (*info->fprintf_func) (info->stream, "(%s)", regname);
+#else
 	  (*info->fprintf_func) (info->stream, "%s@", regname);
+#endif
 	  break;
 
 	case 3:
+#ifdef MOTOROLA
+	  (*info->fprintf_func) (info->stream, "(%s)+", regname);
+#else
 	  (*info->fprintf_func) (info->stream, "%s@+", regname);
+#endif
 	  break;
 
 	case 4:
+#ifdef MOTOROLA
+	  (*info->fprintf_func) (info->stream, "-(%s)", regname);
+#else
 	  (*info->fprintf_func) (info->stream, "%s@-", regname);
+#endif
 	  break;
 
 	case 5:
 	  NEXTWORD (p, val, PRINT_INSN_ARG_MEMORY_ERROR);
+#ifdef MOTOROLA
+	  if (dump_baserel)
+	    {
+	      (*info->fprintf_func) (info->stream, "(");
+	      print_base(regno, val, info);
+	      (*info->fprintf_func) (info->stream, ")");
+	    }
+	  else
+	    (*info->fprintf_func) (info->stream, "%d(%s)", val, regname);
+#else
 	  (*info->fprintf_func) (info->stream, "%s@(%d)", regname, val);
+#endif
 	  break;
 
 	case 6:
@@ -1073,9 +1312,14 @@ print_insn_arg (const char *d,
 
 	    case 2:
 	      NEXTWORD (p, val, PRINT_INSN_ARG_MEMORY_ERROR);
+#ifdef MOTOROLA
+	      (*info->print_address_func) (addr + val, info);
+	      (*info->fprintf_func) (info->stream, "(pc)");
+#else
 	      (*info->fprintf_func) (info->stream, "%%pc@(");
 	      (*info->print_address_func) (addr + val, info);
 	      (*info->fprintf_func) (info->stream, ")");
+#endif
 	      break;
 
 	    case 3:
@@ -1216,12 +1460,20 @@ print_insn_arg (const char *d,
 		  if (doneany)
 		    (*info->fprintf_func) (info->stream, "/");
 		  doneany = 1;
+#ifdef MOTOROLA
+		  (*info->fprintf_func) (info->stream, "fp%d", regno);
+#else
 		  (*info->fprintf_func) (info->stream, "%%fp%d", regno);
+#endif
 		  first_regno = regno;
 		  while (val & (1 << (regno + 1)))
 		    ++regno;
 		  if (regno > first_regno)
+#ifdef MOTOROLA
+		    (*info->fprintf_func) (info->stream, "-fp%d", regno);
+#else
 		    (*info->fprintf_func) (info->stream, "-%%fp%d", regno);
+#endif
 		}
 	  }
 	else if (place == '8')
@@ -1250,6 +1502,29 @@ print_insn_arg (const char *d,
 	FETCH_ARG (5, val);
 	switch (val)
 	  {
+#ifdef MOTOROLA
+	  case 2: name = "tt0"; break;
+	  case 3: name = "tt1"; break;
+	  case 0x10: name = "tc"; break;
+	  case 0x11: name = "drp"; break;
+	  case 0x12: name = "srp"; break;
+	  case 0x13: name = "crp"; break;
+	  case 0x14: name = "cal"; break;
+	  case 0x15: name = "val"; break;
+	  case 0x16: name = "scc"; break;
+	  case 0x17: name = "ac"; break;
+ 	  case 0x18: name = "psr"; break;
+	  case 0x19: name = "pcsr"; break;
+	  case 0x1c:
+	  case 0x1d:
+	    {
+	      int break_reg = ((buffer[3] >> 2) & 7);
+
+	      (*info->fprintf_func)
+		(info->stream, val == 0x1c ? "bad%d" : "bac%d",
+		 break_reg);
+	    }
+#else
 	  case 2: name = "%tt0"; break;
 	  case 3: name = "%tt1"; break;
 	  case 0x10: name = "%tc"; break;
@@ -1271,6 +1546,7 @@ print_insn_arg (const char *d,
 		(info->stream, val == 0x1c ? "%%bad%d" : "%%bac%d",
 		 break_reg);
 	    }
+#endif
 	    break;
 	  default:
 	    (*info->fprintf_func) (info->stream, "<mmu register %d>", val);
@@ -1286,9 +1562,15 @@ print_insn_arg (const char *d,
 
 	FETCH_ARG (5, fc);
 	if (fc == 1)
+#ifdef MOTOROLA
+	  (*info->fprintf_func) (info->stream, "dfc");
+	else if (fc == 0)
+	  (*info->fprintf_func) (info->stream, "sfc");
+#else
 	  (*info->fprintf_func) (info->stream, "%%dfc");
 	else if (fc == 0)
 	  (*info->fprintf_func) (info->stream, "%%sfc");
+#endif
 	else
 	  /* xgettext:c-format */
 	  (*info->fprintf_func) (info->stream, _("<function code %d>"), fc);
@@ -1296,7 +1578,11 @@ print_insn_arg (const char *d,
       break;
 
     case 'V':
+#ifdef MOTOROLA
+      (*info->fprintf_func) (info->stream, "val");
+#else
       (*info->fprintf_func) (info->stream, "%%val");
+#endif
       break;
 
     case 't':
@@ -1466,6 +1752,26 @@ match_insn_m68k (bfd_vma memaddr,
 
   d = args;
 
+#ifdef MOTOROLA
+  /* add a . into movel and simila names. */
+  int bnl = strlen(best->name);
+  char c = best->name[bnl - 1];
+  if (strcmp("rts", best->name)
+      && strcmp("bfexts", best->name)
+      && strcmp("bfins", best->name)
+      && strcmp("cas", best->name)
+      && strcmp("tas", best->name)
+      && strcmp("illegal", best->name)
+      && (c == 's' || c == 'w' || c == 'b' || c == 'l'))
+    {
+      static char b[32];
+      strcpy(b, best->name);
+      b[bnl - 1] = '.';
+      b[bnl] = c;
+      b[bnl + 1] = 0;
+      info->fprintf_func (info->stream, "%s", b);
+    } else
+#endif
   info->fprintf_func (info->stream, "%s", best->name);
 
   if (*d)
@@ -1629,6 +1935,14 @@ print_insn_m68k (bfd_vma memaddr, disass
   priv.max_fetched = priv.the_buffer;
   priv.insn_start = memaddr;
 
+  if (info->disassembler_options)
+    {
+      parse_disassembler_options (info->disassembler_options);
+
+      /* To avoid repeated parsing of these options, we remove them here.  */
+      info->disassembler_options = NULL;
+    }
+
   arch_mask = bfd_m68k_mach_to_features (info->mach);
   if (!arch_mask)
     {
@@ -1643,9 +1957,37 @@ print_insn_m68k (bfd_vma memaddr, disass
       val = m68k_scan_mask (memaddr, info, arch_mask);
     }
 
-  if (val == 0)
-    /* Handle undefined instructions.  */
-    info->fprintf_func (info->stream, ".short 0x%04x", (buffer[0] << 8) + buffer[1]);
+  if (val <= 0)
+    {
+      /* Handle undefined instructions.  */
+      info->fprintf_func (info->stream, ".short 0x%04x", (buffer[0] << 8) + buffer[1]);
+      val = 2;
+    }
 
   return val ? val : 2;
 }
+
+#ifdef TARGET_AMIGA
+void print_m68k_disassembler_options (FILE * stream)
+{
+  fprintf (stream, _("\n\
+The following m68k specific disassembler options are supported for use with\n\
+the -M switch:\n"));
+  fprintf (stream, "  a4              Display labels for base relative offsets\n");
+}
+
+/*
+ * Support -M a4
+ */
+static void
+parse_disassembler_options (const char * options)
+{
+  const char *p;
+
+  p = options;
+  while (*p && *p <= 32)
+    ++p;
+
+  dump_baserel = !strncmp(p, "a4", 2);
+}
+#endif

--- opcodes/m68k-opc.c	2022-07-08 10:46:48.000000000 +0100
+++ opcodes/m68k-opc.c	2025-11-07 11:25:11.585500457 +0000
@@ -42,6 +42,7 @@ const struct m68k_opcode m68k_opcodes[]
 
 {"addib", 4,	one(0003000),	one(0177700), "#b$s", m68000up },
 {"addiw", 4,	one(0003100),	one(0177700), "#w$s", m68000up },
+{"addiwl",4,    one(0003300),   one(0177700), "#w$s", m68080 }, // 68080 APOLLO
 {"addil", 6,	one(0003200),	one(0177700), "#l$s", m68000up },
 {"addil", 6,	one(0003200),	one(0177700), "#lDs", mcfisa_a },
 
@@ -257,6 +258,7 @@ const struct m68k_opcode m68k_opcodes[]
 {"clrb", 2,	one(0041000),	one(0177700), "$s", m68000up | mcfisa_a },
 {"clrw", 2,	one(0041100),	one(0177700), "$s", m68000up | mcfisa_a },
 {"clrl", 2,	one(0041200),	one(0177700), "$s", m68000up | mcfisa_a },
+{"clrq", 2,     one(0127000),   one(0177700), "$s", m68080 }, // APOLLO 68080
 
 {"cmp2b", 4,	two(0000300,0), two(0177700,07777), "!sR1", m68020up | cpu32 | fido_a },
 {"cmp2w", 4,	two(0001300,0),	two(0177700,07777), "!sR1", m68020up | cpu32 | fido_a },
@@ -271,6 +273,7 @@ const struct m68k_opcode m68k_opcodes[]
 {"cmpiw", 4,	one(0006100),	one(0177700), "#w$s", m68000 | m68010 },
 {"cmpiw", 4,	one(0006100),	one(0177700), "#w@s", m68020up | cpu32 | fido_a },
 {"cmpiw", 4,	one(0006100),	one(0177700), "#wDs", mcfisa_b | mcfisa_c },
+{"cmpiwl",4,    one(0047000),   one(0177700), "#w@s", m68080 }, // 68080 APOLLO
 {"cmpil", 6,	one(0006200),	one(0177700), "#l$s", m68000 | m68010 },
 {"cmpil", 6,	one(0006200),	one(0177700), "#l@s", m68020up | cpu32 | fido_a },
 {"cmpil", 6,	one(0006200),	one(0177700), "#lDs", mcfisa_a },
@@ -340,6 +343,8 @@ const struct m68k_opcode m68k_opcodes[]
 {"dbvc", 2,	one(0054310),	one(0177770), "DsBw", m68000up },
 {"dbvs", 2,	one(0054710),	one(0177770), "DsBw", m68000up },
 
+{"dbral", 2, one(0050710), one(0177770), "DsB~", m68080 },   // APOLLO 68080 DBRAL
+
 {"divsw", 2,	one(0100700),	one(0170700), ";wDd", m68000up | mcfhwdiv },
 
 {"divsl", 4, 	two(0046100,0006000),two(0177700,0107770),";lD3D1", m68020up | cpu32 | fido_a },
@@ -1671,6 +1676,15 @@ const struct m68k_opcode m68k_opcodes[]
 {"mvzb", 2,	one(0070600),	one(0170700), "*bDd", mcfisa_b | mcfisa_c },
 {"mvzw", 2,	one(0070700),	one(0170700), "*wDd", mcfisa_b | mcfisa_c },
 
+{"moviwl",4,    one(0121000),   one(0177700), "#w$s", m68080 }, // Apollo 68080
+{"mov3ql", 2,   one(0120100),   one(0170700), "xd%s", m68080 }, // Apollo 68080
+{"mvsb", 2,     one(0120400),   one(0170700), "*bDd", m68080 }, // Apollo 68080
+{"mvsw", 2,     one(0120500),   one(0170700), "*wDd", m68080 }, // Apollo 68080
+{"mvzb", 2,     one(0120600),   one(0170700), "*bDd", m68080 }, // Apollo 68080
+{"mvzw", 2,     one(0120700),   one(0170700), "*wDd", m68080 }, // Apollo 68080
+{"move2l", 4,   two(0007200, 000021), two(0177700, 006077), "$sR1R2", m68080 }, // APOLLO 68080 
+{"move2l", 4,   two(0007200, 004021), two(0177700, 006077), "R1R2$s", m68080 }, // APOLLO 68080
+
 {"movesb", 4,	two(0007000, 0),     two(0177700, 07777), "~sR1", m68010up },
 {"movesb", 4,	two(0007000, 04000), two(0177700, 07777), "R1~s", m68010up },
 {"movesw", 4,	two(0007100, 0),     two(0177700, 07777), "~sR1", m68010up },
@@ -2146,9 +2160,9 @@ const struct m68k_opcode m68k_opcodes[]
 {"subil", 6,	one(0002200),	one(0177700), "#l$s", m68000up },
 {"subil", 6,	one(0002200),	one(0177700), "#lDs", mcfisa_a },
 
-{"subqb", 2,	one(0050400),	one(0170700), "Qd%s", m68000up },
-{"subqw", 2,	one(0050500),	one(0170700), "Qd%s", m68000up },
-{"subql", 2,	one(0050600),	one(0170700), "Qd%s", m68000up | mcfisa_a },
+{"subqb", 2,	one(0050400),	one(0170700), "Qd$b", m68000up },
+{"subqw", 2,	one(0050500),	one(0170700), "Qd%w", m68000up },
+{"subql", 2,	one(0050600),	one(0170700), "Qd%l", m68000up | mcfisa_a },
 
 /* The sub opcode can generate the suba, subi, and subq instructions.  */
 {"subb", 2,	one(0050400),	one(0170700), "Qd%s", m68000up },











--- opcodes/po/opcodes.pot	2022-08-05 11:02:12.000000000 +0100
+++ opcodes/po/opcodes.pot	2025-11-07 11:25:11.605500757 +0000
@@ -8,7 +8,7 @@ msgid ""
 msgstr ""
 "Project-Id-Version: PACKAGE VERSION\n"
 "Report-Msgid-Bugs-To: https://sourceware.org/bugzilla/\n"
-"POT-Creation-Date: 2022-07-08 10:57+0100\n"
+"POT-Creation-Date: 2022-08-05 14:00+0100\n"
 "PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\n"
 "Last-Translator: FULL NAME <EMAIL@ADDRESS>\n"
 "Language-Team: LANGUAGE <LL@li.org>\n"
@@ -927,12 +927,12 @@ msgstr ""
 msgid "Address 0x%s is out of bounds.\n"
 msgstr ""
 
-#: disassemble.c:862
+#: disassemble.c:863
 #, c-format
 msgid "assertion fail %s:%d"
 msgstr ""
 
-#: disassemble.c:863
+#: disassemble.c:864
 msgid "Please report this bug"
 msgstr ""
 
@@ -2232,7 +2232,7 @@ msgstr ""
 msgid "unknown S/390 disassembler option: %s"
 msgstr ""
 
-#: s390-dis.c:416
+#: s390-dis.c:471
 #, c-format
 msgid ""
 "\n"









--- opcodes/sysdep.h	2022-07-08 10:46:48.000000000 +0100
+++ opcodes/sysdep.h	2025-11-07 11:25:11.621500996 +0000
@@ -32,6 +32,11 @@
 #endif
 
 #include "config.h"
+
+#ifndef LIBIBERTYDECL
+#define LIBIBERTYDECL
+#endif
+
 #include <stdlib.h>
 #include <string.h>
 #include "ansidecl.h"






--- src-release.sh	2022-07-08 10:46:49.000000000 +0100
+++ src-release.sh	2025-11-07 11:25:12.165509151 +0000
@@ -38,7 +38,7 @@ MAKEINFOFLAGS=--split-size=5000000
 # Support for building net releases
 
 # Files in root used in any net release.
-DEVO_SUPPORT="ar-lib ChangeLog compile config config-ml.in config.guess \
+DEVO_SUPPORT="ar-lib ChangeLog ChangeLog.git.2.37-2.38 compile config config-ml.in config.guess \
 	config.rpath config.sub configure configure.ac COPYING COPYING.LIB \
 	COPYING3 COPYING3.LIB depcomp install-sh libtool.m4 ltgcc.m4 \
 	ltmain.sh ltoptions.m4 ltsugar.m4 ltversion.m4 lt~obsolete.m4 \

--- zlib/configure	2022-07-08 10:56:29.000000000 +0100
+++ zlib/configure	2025-11-07 11:25:12.181509390 +0000
@@ -11551,7 +11551,15 @@ else
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
 
