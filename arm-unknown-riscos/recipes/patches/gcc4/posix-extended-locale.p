Index: gcc4/recipe/files/gcc/libunixlib/Makefile.am
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/Makefile.am	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/Makefile.am	(working copy)
@@ -583,11 +583,11 @@
 else
 locale_src = \
 	locale/localeconv.c \
+	locale/newlocale.c \
 	locale/nl_langinfo.c \
 	locale/setlocale.c \
 	locale/strcoll.c \
-	locale/strxfrm.c \
-	locale/territory.c
+	locale/strxfrm.c
 endif
 locale_src += \
 	locale/iconv.c
@@ -1312,6 +1312,7 @@
 	bits/in.h \
 	bits/libc-lock.h \
 	bits/locale.h \
+	bits/locale_t.h \
 	bits/mathcalls.h \
 	bits/mathdef.h \
 	bits/mathinline.h \
Index: gcc4/recipe/files/gcc/libunixlib/ctype/ctypetable.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/ctype/ctypetable.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/ctype/ctypetable.c	(working copy)
@@ -10,36 +10,35 @@
 #include <internal/os.h>
 #include <internal/unix.h>
 
-/* Note the offset 1 and depth 257 instead of 0 and 256: ISO C says we must
-   support EOF.  */
-static unsigned char ctype[257];
-const unsigned char * const __ctype = &ctype[1];
+/* Global containing current locale settings. */
+struct _locale __locale_global;
+/* Offset 1 as the first entry is reserved for EOF.  */
+const unsigned char * const __ctype = &__locale_global.ctype[1];
+const short * const __ctype_upper = &__locale_global.ctype_upper[1];
+const short * const __ctype_lower = &__locale_global.ctype_lower[1];
 
-static short ctype_upper[257];
-const short * const __ctype_upper = &ctype_upper[1];
-
-static short ctype_lower[257];
-const short * const __ctype_lower = &ctype_lower[1];
-
 void
-__build_ctype_tables (int territory)
+__build_ctype_tables (locale_t locobj, int territory)
 {
   PTHREAD_UNSAFE
 
   if (territory == -2)
     {
-      /* Initialise the array. This is only done by __unixinit().  */
+      /* Initialise the array. This is only done by __unixinit()/newlocale().  */
       territory = -1;
-      for (int x = 0; x <= LC_ALL; x++)
-        __locale_territory[x] = -1;
+      for (int x = 0; x < LC_ALL; x++)
+        locobj->locale_territory[x] = -1;
+      __localeconv_lconv_init(&locobj->lc);
+      locobj->lc_needs_refresh = 1;
     }
 
   /* Initialise ctype_upper/ctype_lower tables.  */
   for (int x = 0; x < 257; x++)
     {
-      /* In the C/POSIX locate, tolower(top bit set char)
+      /* In the C/POSIX locale, tolower(top bit set char)
 	 should return the character unchanged.  */
-      ctype_lower[x] = ctype_upper[x] = x - 1;
+      locobj->ctype_lower[x] = locobj->ctype_upper[x] = x - 1;
+      locobj->ctype[x] = 0;
     }
 
   int regs[10];
@@ -61,7 +60,7 @@
 	  for (int offset = 1; bits; bits = bits >> 1, offset += 1)
 	    {
 	      if (bits & 1)
-		ctype[pos + offset] |= 1 << code;
+		locobj->ctype[pos + offset] |= 1 << code;
 	    }
 	  pos += 32;
 	}
@@ -80,7 +79,7 @@
   int y = (territory == -1) ? 128 : 256;
   for (int x = 1; x <= y; x++)
     {
-      ctype_lower[x] = (short)*p++;
-      ctype_upper[x] = (short)*q++;
+      locobj->ctype_lower[x] = (short)*p++;
+      locobj->ctype_upper[x] = (short)*q++;
     }
 }
Index: gcc4/recipe/files/gcc/libunixlib/ctype/isalnum.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/ctype/isalnum.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/ctype/isalnum.c	(working copy)
@@ -3,6 +3,7 @@
  */
 
 #include <ctype.h>
+#include <locale.h>
 
 int
 (isalnum) (int c)
@@ -10,3 +11,8 @@
   return isalnum (c);
 }
 
+int
+isalnum_l (int c, locale_t locobj)
+{
+  return ((&locobj->ctype[1])[c] & (___ctype_alpha | ___ctype_digit));
+}
Index: gcc4/recipe/files/gcc/libunixlib/ctype/isalpha.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/ctype/isalpha.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/ctype/isalpha.c	(working copy)
@@ -3,6 +3,7 @@
  */
 
 #include <ctype.h>
+#include <locale.h>
 
 int
 (isalpha) (int c)
@@ -10,3 +11,8 @@
   return isalpha (c);
 }
 
+int
+isalpha_l (int c, locale_t locobj)
+{
+  return ((&locobj->ctype[1])[c] & ___ctype_alpha);
+}
Index: gcc4/recipe/files/gcc/libunixlib/ctype/isascii.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/ctype/isascii.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/ctype/isascii.c	(working copy)
@@ -10,3 +10,9 @@
   return isascii (c);
 }
 
+int
+isascii_l (int c, locale_t locobj)
+{
+  (void) locobj;
+  return isascii (c);
+}
Index: gcc4/recipe/files/gcc/libunixlib/ctype/isblank.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/ctype/isblank.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/ctype/isblank.c	(working copy)
@@ -9,3 +9,10 @@
 {
   return isblank (c);
 }
+
+int
+isblank_l (int c, locale_t locobj)
+{
+  (void) locobj;
+  return isblank (c);
+}
Index: gcc4/recipe/files/gcc/libunixlib/ctype/iscntrl.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/ctype/iscntrl.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/ctype/iscntrl.c	(working copy)
@@ -3,6 +3,7 @@
  */
 
 #include <ctype.h>
+#include <locale.h>
 
 int
 (iscntrl) (int c)
@@ -10,3 +11,8 @@
   return iscntrl (c);
 }
 
+int
+iscntrl_l (int c, locale_t locobj)
+{
+  return ((&locobj->ctype[1])[c] & ___ctype_ctrl);
+}
Index: gcc4/recipe/files/gcc/libunixlib/ctype/isdigit.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/ctype/isdigit.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/ctype/isdigit.c	(working copy)
@@ -3,6 +3,7 @@
  */
 
 #include <ctype.h>
+#include <locale.h>
 
 int
 (isdigit) (int c)
@@ -10,3 +11,8 @@
   return isdigit (c);
 }
 
+int
+isdigit_l (int c, locale_t locobj)
+{
+  return ((&locobj->ctype[1])[c] & ___ctype_digit);
+}
Index: gcc4/recipe/files/gcc/libunixlib/ctype/isgraph.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/ctype/isgraph.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/ctype/isgraph.c	(working copy)
@@ -3,6 +3,7 @@
  */
 
 #include <ctype.h>
+#include <locale.h>
 
 int
 (isgraph) (int c)
@@ -10,3 +11,8 @@
   return isgraph (c);
 }
 
+int
+isgraph_l (int c, locale_t locobj)
+{
+  return ((&locobj->ctype[1])[c] & ~(___ctype_white | ___ctype_ctrl));
+}
Index: gcc4/recipe/files/gcc/libunixlib/ctype/islower.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/ctype/islower.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/ctype/islower.c	(working copy)
@@ -3,6 +3,7 @@
  */
 
 #include <ctype.h>
+#include <locale.h>
 
 int
 (islower) (int c)
@@ -10,3 +11,8 @@
   return islower (c);
 }
 
+int
+islower_l (int c, locale_t locobj)
+{
+  return ((&locobj->ctype[1])[c] & ___ctype_lower);
+}
Index: gcc4/recipe/files/gcc/libunixlib/ctype/isprint.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/ctype/isprint.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/ctype/isprint.c	(working copy)
@@ -3,6 +3,7 @@
  */
 
 #include <ctype.h>
+#include <locale.h>
 
 int
 (isprint) (int c)
@@ -10,3 +11,10 @@
   return isprint (c);
 }
 
+int
+isprint_l (int c, locale_t locobj)
+{
+  return ((&locobj->ctype[1])[c] &
+          (___ctype_upper | ___ctype_lower | ___ctype_digit | ___ctype_punc |
+           ___ctype_white)) && !((&locobj->ctype[1])[c] & ___ctype_ctrl);
+}
Index: gcc4/recipe/files/gcc/libunixlib/ctype/ispunct.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/ctype/ispunct.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/ctype/ispunct.c	(working copy)
@@ -3,6 +3,7 @@
  */
 
 #include <ctype.h>
+#include <locale.h>
 
 int
 (ispunct) (int c)
@@ -10,3 +11,8 @@
   return ispunct (c);
 }
 
+int
+ispunct_l (int c, locale_t locobj)
+{
+  return ((&locobj->ctype[1])[c] & ___ctype_punc);
+}
Index: gcc4/recipe/files/gcc/libunixlib/ctype/isspace.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/ctype/isspace.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/ctype/isspace.c	(working copy)
@@ -3,6 +3,7 @@
  */
 
 #include <ctype.h>
+#include <locale.h>
 
 int
 (isspace) (int c)
@@ -10,3 +11,8 @@
   return isspace (c);
 }
 
+int
+isspace_l (int c, locale_t locobj)
+{
+  return ((&locobj->ctype[1])[c] & ___ctype_white);
+}
Index: gcc4/recipe/files/gcc/libunixlib/ctype/isupper.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/ctype/isupper.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/ctype/isupper.c	(working copy)
@@ -3,6 +3,7 @@
  */
 
 #include <ctype.h>
+#include <locale.h>
 
 int
 (isupper) (int c)
@@ -10,3 +11,8 @@
   return isupper (c);
 }
 
+int
+isupper_l (int c, locale_t locobj)
+{
+  return ((&locobj->ctype[1])[c] & ___ctype_upper);
+}
Index: gcc4/recipe/files/gcc/libunixlib/ctype/isxdigit.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/ctype/isxdigit.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/ctype/isxdigit.c	(working copy)
@@ -3,6 +3,7 @@
  */
 
 #include <ctype.h>
+#include <locale.h>
 
 int
 (isxdigit) (int c)
@@ -10,3 +11,8 @@
   return isxdigit (c);
 }
 
+int
+isxdigit_l (int c, locale_t locobj)
+{
+  return ((&locobj->ctype[1])[c] & ___ctype_xdigit);
+}
Index: gcc4/recipe/files/gcc/libunixlib/ctype/toascii.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/ctype/toascii.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/ctype/toascii.c	(working copy)
@@ -10,3 +10,9 @@
   return toascii (c);
 }
 
+int
+toascii_l (int c, locale_t locobj)
+{
+  (void) locobj;
+  return toascii (c);
+}
Index: gcc4/recipe/files/gcc/libunixlib/ctype/tolower.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/ctype/tolower.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/ctype/tolower.c	(working copy)
@@ -3,6 +3,7 @@
  */
 
 #include <ctype.h>
+#include <locale.h>
 
 int
 (tolower) (int c)
@@ -10,3 +11,8 @@
   return tolower (c);
 }
 
+int
+tolower_l (int c, locale_t locobj)
+{
+  return (&locobj->ctype_lower[1])[c];
+}
Index: gcc4/recipe/files/gcc/libunixlib/ctype/toupper.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/ctype/toupper.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/ctype/toupper.c	(working copy)
@@ -3,6 +3,7 @@
  */
 
 #include <ctype.h>
+#include <locale.h>
 
 int
 (toupper) (int c)
@@ -10,3 +11,8 @@
   return toupper (c);
 }
 
+int
+toupper_l (int c, locale_t locobj)
+{
+  return (&locobj->ctype_upper[1])[c];
+}
Index: gcc4/recipe/files/gcc/libunixlib/incl-local/locale.h
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/incl-local/locale.h	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/incl-local/locale.h	(working copy)
@@ -13,15 +13,36 @@
 
 #ifndef __TARGET_SCL__
 
-/* Territory number for each locale.  C locale is -1.  */
-extern int __locale_territory[LC_ALL + 1];
+struct _locale {
+  /* Map from locale category to territory number. C locale is -1. */
+  int locale_territory[LC_ALL];
 
-/* Set to 1 is setlocale has been called since the last call to
-   localeconv. localeconv uses this flag to cache the lconv structure.  */
-extern int __setlocale_called;
+  /* Character type LUTs: EOF + one entry per character
+   *
+   * EOF is defined as -1, so character entries start at offset 1.
+   */
+  short ctype_upper[257];
+  short ctype_lower[257];
+  unsigned char ctype[257];
 
-extern void __build_ctype_tables (int __territory) __THROW;
+  /* Set to 1 if the locale information has changed since the last time
+   * the lconv structure was updated. */
+  int lc_needs_refresh;
+  /* Numeric formatting information for locale */
+  struct lconv lc;
+};
 
+extern struct _locale __locale_global;
+
+extern void __build_ctype_tables (locale_t locobj, int __territory) __THROW;
+
+extern char *__setlocale_l (locale_t locobj, int __category,
+                            const char *__locale) __THROW;
+
+void __localeconv_lconv_init (struct lconv *lc);
+void __localeconv_lconv_fini (struct lconv *lc);
+void __localeconv_l (locale_t locobj);
+
 #endif
 
 __END_DECLS
Index: gcc4/recipe/files/gcc/libunixlib/include/bits/locale_t.h
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/include/bits/locale_t.h	(nonexistent)
+++ gcc4/recipe/files/gcc/libunixlib/include/bits/locale_t.h	(working copy)
@@ -0,0 +1,10 @@
+/*
+ * Copyright (c) 2022 UnixLib Developers
+ */
+
+#ifndef _BITS_LOCALE_T_H_
+#define _BITS_LOCALE_T_H_
+
+typedef struct _locale *locale_t;
+
+#endif
Index: gcc4/recipe/files/gcc/libunixlib/include/ctype.h
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/include/ctype.h	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/include/ctype.h	(working copy)
@@ -51,6 +51,48 @@
 extern int isblank (int __c) __THROW;
 
 #ifndef __TARGET_SCL__
+#  ifdef __USE_XOPEN2K8
+#    include <bits/locale_t.h>
+
+/* c is alphabetic or numeric.  */
+extern int isalnum_l (int __c, locale_t locobj) __THROW;
+
+/* c is alphabetic.  */
+extern int isalpha_l (int __c, locale_t locobj) __THROW;
+
+/* c is a control character.  */
+extern int iscntrl_l (int __c, locale_t locobj) __THROW;
+
+/* c is a decimal digit.  */
+extern int isdigit_l (int __c, locale_t locobj) __THROW;
+
+/* c is any printable character other than a space. */
+extern int isgraph_l (int __c, locale_t locobj) __THROW;
+
+/* c is a lower-case letter.  */
+extern int islower_l (int __c, locale_t locobj) __THROW;
+
+/* c is an upper-case letter.  */
+extern int isupper_l (int __c, locale_t locobj) __THROW;
+
+/* c is a printable character.  */
+extern int isprint_l (int __c, locale_t locobj) __THROW;
+
+/* c is a printable character other than a space or a
+   alphanumeric character.  */
+extern int ispunct_l (int __c, locale_t locobj) __THROW;
+
+/* c is a white space character e.g. space, newline, tab, linefeed,
+   return, vertical tab.  */
+extern int isspace_l (int __c, locale_t locobj) __THROW;
+
+/* c is a hex digit.  */
+extern int isxdigit_l (int __c, locale_t locobj) __THROW;
+
+/* c is tab or space.  */
+extern int isblank_l (int __c, locale_t locobj) __THROW;
+#  endif
+
 /* Characteristics. */
 extern const unsigned char * const __ctype;
 /* Lower case table.  */
@@ -108,6 +150,10 @@
 extern int toupper (int __c) __THROW;
 #ifndef __TARGET_SCL__
 #  define toupper(c) ((int) __ctype_upper[(int) (c)])
+
+#  ifdef __USE_XOPEN2K8
+extern int toupper_l (int __c, locale_t locobj) __THROW;
+#  endif
 #endif
 
 /* Convert c to lower case.  */
@@ -114,6 +160,10 @@
 extern int tolower (int __c) __THROW;
 #ifndef __TARGET_SCL__
 #  define tolower(c) ((int) __ctype_lower[(int) (c)])
+
+#  ifdef __USE_XOPEN2K8
+extern int tolower_l (int __c, locale_t locobj) __THROW;
+#  endif
 #endif
 
 #ifndef __TARGET_SCL__
@@ -135,6 +185,11 @@
 /* Is c an ASCII character.  */
 extern int isascii (int __c) __THROW;
 #    define isascii(c) ((unsigned)(c) <= 0x7f)
+
+#    ifdef __USE_XOPEN2K8
+extern int toascii_l (int __c, locale_t locobj) __THROW;
+extern int isascii_l (int __c, locale_t locobj) __THROW;
+#    endif
 #  endif
 #endif
 
Index: gcc4/recipe/files/gcc/libunixlib/include/langinfo.h
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/include/langinfo.h	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/include/langinfo.h	(working copy)
@@ -580,18 +580,12 @@
 extern char *nl_langinfo (nl_item __item) __THROW;
 
 
-#if 0
-#ifdef	__USE_GNU
-/* This interface is for the extended locale model.  See <locale.h> for
-   more information.  */
+#ifdef	__USE_XOPEN2K8
+# include <bits/locale_t.h>
 
-/* Get locale datatype definition.  */
-# include <xlocale.h>
-
 /* Just like nl_langinfo but get the information from the locale object L.  */
-extern char *nl_langinfo_l (nl_item __item, __locale_t l);
+extern char *nl_langinfo_l (nl_item __item, locale_t __l);
 #endif
-#endif
 
 __END_DECLS
 
Index: gcc4/recipe/files/gcc/libunixlib/include/locale.h
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/include/locale.h	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/include/locale.h	(working copy)
@@ -33,16 +33,16 @@
 /* Entire locale.  */
 #  define LC_ALL 6
 
-#define LC_COLLATE_MASK (1L << 1)
-#define LC_CTYPE_MASK (1L << 2)
-#define LC_MESSAGES_MASK (1L << 3)
-#define LC_MONETARY_MASK (1L << 4)
-#define LC_NUMERIC_MASK (1L << 5)
-#define LC_TIME_MASK (1L << 6)
-#define LC_ALL_MASK (LC_COLLATE_MASK | LC_CTYPE_MASK | LC_MESSAGES_MASK | LC_MONETARY_MASK | LC_NUMERIC_MASK | LC_TIME_MASK)
+#  ifdef __USE_XOPEN2K8
+#    define LC_COLLATE_MASK (1L << 0)
+#    define LC_CTYPE_MASK (1L << 1)
+#    define LC_MESSAGES_MASK (1L << 2)
+#    define LC_MONETARY_MASK (1L << 3)
+#    define LC_NUMERIC_MASK (1L << 4)
+#    define LC_TIME_MASK (1L << 5)
+#    define LC_ALL_MASK (LC_COLLATE_MASK | LC_CTYPE_MASK | LC_MESSAGES_MASK | LC_MONETARY_MASK | LC_NUMERIC_MASK | LC_TIME_MASK)
+#  endif
 
-typedef struct _locale *locale_t;
-
 #else
 /* String collation (functions 'strcoll' and 'strxfrm').  */
 #  define LC_COLLATE 1
@@ -121,6 +121,9 @@
 extern struct lconv *localeconv (void) __THROW;
 
 #ifndef __TARGET_SCL__
+#  ifdef __USE_XOPEN2K8
+#    include <bits/locale_t.h>
+
 extern locale_t uselocale(locale_t newloc);
 
 extern void freelocale(locale_t locobj);
@@ -127,7 +130,11 @@
 
 extern locale_t newlocale(int category_mask, const char *locale,
                           locale_t base);
-# define LC_GLOBAL_LOCALE        ((locale_t) -1L)
+
+extern locale_t duplocale(locale_t locobj);
+
+#    define LC_GLOBAL_LOCALE        ((locale_t) -1L)
+#  endif
 #endif
 
 __END_NAMESPACE_STD
Index: gcc4/recipe/files/gcc/libunixlib/include/string.h
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/include/string.h	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/include/string.h	(working copy)
@@ -93,6 +93,16 @@
 		       const char *__restrict __src, size_t __n)
      __THROW __nonnull ((2));
 
+#ifdef __USE_XOPEN2K8
+#include <bits/locale_t.h>
+
+extern int strcoll_l (const char *__s1, const char *__s2, locale_t __l)
+     __THROW __attribute_pure__ __nonnull ((1, 2, 3));
+
+extern size_t strxfrm_l (char *__dest, const char *__src, size_t __n,
+			 locale_t __l) __THROW __nonnull ((2, 4));
+#endif
+
 /* Find the first occurrence of c in s. */
 extern char *strchr (const char *__s, int __c)
      __THROW __attribute_pure__ __nonnull ((1)) __wur;
@@ -281,6 +291,17 @@
 /* Compare no more than N chars of S1 and S2, ignoring case.  */
 extern int strncasecmp (const char *__s1, const char *__s2, size_t __n)
      __THROW __attribute_pure__ __nonnull ((1, 2)) __wur;
+
+#  ifdef __USE_XOPEN2K8
+#    include <bits/locale_t.h>
+
+extern int strcasecmp_l (const char *__s1, const char *__s2, locale_t __loc)
+     __THROW __attribute_pure__ __nonnull ((1, 2, 3));
+
+extern int strncasecmp_l (const char *__s1, const char *__s2,
+                          size_t __n, locale_t __loc)
+     __THROW __attribute_pure__ __nonnull ((1, 2, 4));
+#  endif
 #endif /* Use BSD.  */
 
 #if defined __USE_XOPEN2K || defined __USE_MISC
@@ -289,6 +310,11 @@
      __THROW __nonnull ((2));
 #endif
 
+#ifdef __USE_XOPEN2K8
+/* Translate error number to string according to the locale L.  */
+extern char *strerror_l (int __errnum, locale_t __l) __THROW;
+#endif
+
 #ifndef __TARGET_SCL__
 # ifndef basename
 /* Return the file name within directory of FILENAME.  We don't
Index: gcc4/recipe/files/gcc/libunixlib/include/strings.h
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/include/strings.h	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/include/strings.h	(working copy)
@@ -37,7 +37,17 @@
 /* Compare n chars of S1 and S2, ignoring case.  */
 extern int strncasecmp (const char *__s1, const char *__s2, size_t __n);
 
+#  ifdef __USE_XOPEN2K8
+#    include <bits/locale_t.h>
 
+extern int strcasecmp_l (const char *__s1, const char *__s2, locale_t __loc)
+     __THROW __attribute_pure__ __nonnull ((1, 2, 3));
+
+extern int strncasecmp_l (const char *__s1, const char *__s2,
+                          size_t __n, locale_t __loc)
+     __THROW __attribute_pure__ __nonnull ((1, 2, 4));
+#  endif
+
 /* Return the position of the first bit set in I, or 0 if none are set.
    The least-significant bit is position 1, the most-significant 32.  */
 extern int ffs (int __i);
Index: gcc4/recipe/files/gcc/libunixlib/include/wchar.h
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/include/wchar.h	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/include/wchar.h	(working copy)
@@ -1,7 +1,7 @@
 /*
  * File taken from glibc 2.11.
  * Following changes were made:
- *   - Disabled non-standard reentrant locale prototypes.
+ *   - None
  */
 
 /* Copyright (C) 1995-2008, 2009 Free Software Foundation, Inc.
@@ -172,7 +172,6 @@
 __END_NAMESPACE_STD
 #endif
 
-#if 0
 #ifdef __USE_XOPEN2K8
 /* Compare S1 and S2, ignoring case.  */
 extern int wcscasecmp (__const wchar_t *__s1, __const wchar_t *__s2) __THROW;
@@ -183,15 +182,14 @@
 
 /* Similar to the two functions above but take the information from
    the provided locale and not the global locale.  */
-# include <xlocale.h>
+# include <bits/locale_t.h>
 
 extern int wcscasecmp_l (__const wchar_t *__s1, __const wchar_t *__s2,
-			 __locale_t __loc) __THROW;
+			 locale_t __loc) __THROW;
 
 extern int wcsncasecmp_l (__const wchar_t *__s1, __const wchar_t *__s2,
-			  size_t __n, __locale_t __loc) __THROW;
+			  size_t __n, locale_t __loc) __THROW;
 #endif
-#endif
 
 __BEGIN_NAMESPACE_STD
 /* Compare S1 and S2, both interpreted as appropriate to the
@@ -205,7 +203,6 @@
 __END_NAMESPACE_STD
 
 #ifdef __USE_XOPEN2K8
-#if 0
 /* Similar to the two functions above but take the information from
    the provided locale and not the global locale.  */
 
@@ -212,14 +209,13 @@
 /* Compare S1 and S2, both interpreted as appropriate to the
    LC_COLLATE category of the given locale.  */
 extern int wcscoll_l (__const wchar_t *__s1, __const wchar_t *__s2,
-		      __locale_t __loc) __THROW;
+		      locale_t __loc) __THROW;
 
 /* Transform S2 into array pointed to by S1 such that if wcscmp is
    applied to two transformed strings the result is the as applying
    `wcscoll' to the original strings.  */
 extern size_t wcsxfrm_l (wchar_t *__s1, __const wchar_t *__s2,
-			 size_t __n, __locale_t __loc) __THROW;
-#endif
+			 size_t __n, locale_t __loc) __THROW;
 
 #ifndef __TARGET_SCL__
 /* Duplicate S, returning an identical malloc'd string.  */
Index: gcc4/recipe/files/gcc/libunixlib/include/wctype.h
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/include/wctype.h	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/include/wctype.h	(working copy)
@@ -3,7 +3,6 @@
  * Following changes were made:
  *  - Replaced "#include <bits/types.h>" into "#include <unixlib/types.h>"
  *  - Add wint_t typedef when it hasn't been defined in stddef.h.
- *  - Disabled non-standard reentrant locale prototypes.
  */
 
 /* Copyright (C) 1996-2002,2005,2007,2008,2009 Free Software Foundation, Inc.
@@ -191,7 +190,6 @@
 /* Determine whether the wide-character WC has the property described by
    DESC.  */
 extern int iswctype (wint_t __wc, wctype_t __desc) __THROW;
-#define iswctype_l(c, d, l) iswctype(c, d)
 
 __END_NAMESPACE_C99
 
@@ -241,73 +239,71 @@
 extern wint_t towctrans (wint_t __wc, wctrans_t __desc) __THROW;
 __END_NAMESPACE_C99
 
-#if 0
 # ifdef __USE_XOPEN2K8
-/* Declare the interface to extended locale model.  */
-#  include <xlocale.h>
+#  include <bits/locale_t.h>
 
 /* Test for any wide character for which `iswalpha' or `iswdigit' is
    true.  */
-extern int iswalnum_l (wint_t __wc, __locale_t __locale) __THROW;
+extern int iswalnum_l (wint_t __wc, locale_t __locale) __THROW;
 
 /* Test for any wide character for which `iswupper' or 'iswlower' is
    true, or any wide character that is one of a locale-specific set of
    wide-characters for which none of `iswcntrl', `iswdigit',
    `iswpunct', or `iswspace' is true.  */
-extern int iswalpha_l (wint_t __wc, __locale_t __locale) __THROW;
+extern int iswalpha_l (wint_t __wc, locale_t __locale) __THROW;
 
 /* Test for any control wide character.  */
-extern int iswcntrl_l (wint_t __wc, __locale_t __locale) __THROW;
+extern int iswcntrl_l (wint_t __wc, locale_t __locale) __THROW;
 
 /* Test for any wide character that corresponds to a decimal-digit
    character.  */
-extern int iswdigit_l (wint_t __wc, __locale_t __locale) __THROW;
+extern int iswdigit_l (wint_t __wc, locale_t __locale) __THROW;
 
 /* Test for any wide character for which `iswprint' is true and
    `iswspace' is false.  */
-extern int iswgraph_l (wint_t __wc, __locale_t __locale) __THROW;
+extern int iswgraph_l (wint_t __wc, locale_t __locale) __THROW;
 
 /* Test for any wide character that corresponds to a lowercase letter
    or is one of a locale-specific set of wide characters for which
    none of `iswcntrl', `iswdigit', `iswpunct', or `iswspace' is true.  */
-extern int iswlower_l (wint_t __wc, __locale_t __locale) __THROW;
+extern int iswlower_l (wint_t __wc, locale_t __locale) __THROW;
 
 /* Test for any printing wide character.  */
-extern int iswprint_l (wint_t __wc, __locale_t __locale) __THROW;
+extern int iswprint_l (wint_t __wc, locale_t __locale) __THROW;
 
 /* Test for any printing wide character that is one of a
    locale-specific et of wide characters for which neither `iswspace'
    nor `iswalnum' is true.  */
-extern int iswpunct_l (wint_t __wc, __locale_t __locale) __THROW;
+extern int iswpunct_l (wint_t __wc, locale_t __locale) __THROW;
 
 /* Test for any wide character that corresponds to a locale-specific
    set of wide characters for which none of `iswalnum', `iswgraph', or
    `iswpunct' is true.  */
-extern int iswspace_l (wint_t __wc, __locale_t __locale) __THROW;
+extern int iswspace_l (wint_t __wc, locale_t __locale) __THROW;
 
 /* Test for any wide character that corresponds to an uppercase letter
    or is one of a locale-specific set of wide character for which none
    of `iswcntrl', `iswdigit', `iswpunct', or `iswspace' is true.  */
-extern int iswupper_l (wint_t __wc, __locale_t __locale) __THROW;
+extern int iswupper_l (wint_t __wc, locale_t __locale) __THROW;
 
 /* Test for any wide character that corresponds to a hexadecimal-digit
    character equivalent to that performed be the functions described
    in the previous subclause.  */
-extern int iswxdigit_l (wint_t __wc, __locale_t __locale) __THROW;
+extern int iswxdigit_l (wint_t __wc, locale_t __locale) __THROW;
 
 /* Test for any wide character that corresponds to a standard blank
    wide character or a locale-specific set of wide characters for
    which `iswalnum' is false.  */
-extern int iswblank_l (wint_t __wc, __locale_t __locale) __THROW;
+extern int iswblank_l (wint_t __wc, locale_t __locale) __THROW;
 
 /* Construct value that describes a class of wide characters identified
    by the string argument PROPERTY.  */
-extern wctype_t wctype_l (__const char *__property, __locale_t __locale)
+extern wctype_t wctype_l (__const char *__property, locale_t __locale)
      __THROW;
 
 /* Determine whether the wide-character WC has the property described by
    DESC.  */
-extern int iswctype_l (wint_t __wc, wctype_t __desc, __locale_t __locale)
+extern int iswctype_l (wint_t __wc, wctype_t __desc, locale_t __locale)
      __THROW;
 
 
@@ -316,22 +312,21 @@
  */
 
 /* Converts an uppercase letter to the corresponding lowercase letter.  */
-extern wint_t towlower_l (wint_t __wc, __locale_t __locale) __THROW;
+extern wint_t towlower_l (wint_t __wc, locale_t __locale) __THROW;
 
 /* Converts an lowercase letter to the corresponding uppercase letter.  */
-extern wint_t towupper_l (wint_t __wc, __locale_t __locale) __THROW;
+extern wint_t towupper_l (wint_t __wc, locale_t __locale) __THROW;
 
 /* Construct value that describes a mapping between wide characters
    identified by the string argument PROPERTY.  */
-extern wctrans_t wctrans_l (__const char *__property, __locale_t __locale)
+extern wctrans_t wctrans_l (__const char *__property, locale_t __locale)
      __THROW;
 
 /* Map the wide character WC using the mapping described by DESC.  */
 extern wint_t towctrans_l (wint_t __wc, wctrans_t __desc,
-			   __locale_t __locale) __THROW;
+			   locale_t __locale) __THROW;
 
 # endif /* Use POSIX 2008.  */
-#endif
 
 __END_DECLS
 
Index: gcc4/recipe/files/gcc/libunixlib/locale/localeconv.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/locale/localeconv.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/locale/localeconv.c	(working copy)
@@ -14,8 +14,6 @@
 
 /* #define DEBUG */
 
-int __setlocale_called = 1;
-
 static int
 read_symbol (int reason_code, int territory)
 {
@@ -71,26 +69,44 @@
   *grouping = new_grouping;
 }
 
-static struct lconv lc = { NULL, NULL, NULL, NULL, NULL,
-			   NULL, NULL, NULL, NULL, NULL,
-			   CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX,
-			   CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX };
+void
+__localeconv_lconv_init (struct lconv *lc)
+{
+  memset(lc, 0, sizeof(*lc));
+  lc->int_frac_digits = lc->frac_digits = lc->p_cs_precedes =
+  lc->p_sep_by_space = lc->n_cs_precedes = lc->n_sep_by_space =
+  lc->p_sign_posn = lc->n_sign_posn = CHAR_MAX;
+}
 
-/* Defined by POSIX as not threadsafe */
-struct lconv *
-localeconv (void)
+void
+__localeconv_lconv_fini (struct lconv *lc)
 {
+  free(lc->decimal_point);
+  free(lc->thousands_sep);
+  free(lc->grouping);
+  free(lc->int_curr_symbol);
+  free(lc->currency_symbol);
+  free(lc->mon_decimal_point);
+  free(lc->mon_thousands_sep);
+  free(lc->mon_grouping);
+  free(lc->positive_sign);
+  free(lc->negative_sign);
+}
+
+void
+__localeconv_l (locale_t locobj)
+{
   int numeric, monetary;
 
   /* If setlocale has not been called since the last call to
      localeconv, then the lconv structure will be the same.  */
-  if (!__setlocale_called)
-    return &lc;
+  if (!locobj->lc_needs_refresh)
+    return;
 
-  __setlocale_called = 0;
+  locobj->lc_needs_refresh = 0;
 
-  numeric = __locale_territory[LC_NUMERIC];
-  monetary = __locale_territory[LC_MONETARY];
+  numeric = locobj->locale_territory[LC_NUMERIC];
+  monetary = locobj->locale_territory[LC_MONETARY];
 
   /* See the PRMs regarding SWI Territory_ReadSymbols for the
      meanings of the following numbers.  */
@@ -97,71 +113,80 @@
   if (numeric == -1)
     {
       /* We're using the 'C' locale.  */
-      free (lc.decimal_point);
-      lc.decimal_point = strdup (".");
-      free (lc.thousands_sep);
-      lc.thousands_sep = strdup ("");
-      free (lc.grouping);
-      lc.grouping = strdup ("");
+      free (locobj->lc.decimal_point);
+      locobj->lc.decimal_point = strdup (".");
+      free (locobj->lc.thousands_sep);
+      locobj->lc.thousands_sep = strdup ("");
+      free (locobj->lc.grouping);
+      locobj->lc.grouping = strdup ("");
     }
   else
     {
-      free (lc.decimal_point);
-      lc.decimal_point = strdup ((char *) read_symbol (0, numeric));
-      free (lc.thousands_sep);
-      lc.thousands_sep = strdup ((char *) read_symbol (1, numeric));
-      read_byte_list (2, &lc.grouping, numeric);
+      free (locobj->lc.decimal_point);
+      locobj->lc.decimal_point = strdup ((char *) read_symbol (0, numeric));
+      free (locobj->lc.thousands_sep);
+      locobj->lc.thousands_sep = strdup ((char *) read_symbol (1, numeric));
+      read_byte_list (2, &locobj->lc.grouping, numeric);
     }
   if (monetary == -1)
     {
       /* We using the 'C' locale.  Empty strings and CHAR_MAX means
 	 that these fields are unspecified.  */
-      free (lc.mon_decimal_point);
-      lc.mon_decimal_point = strdup ("");
-      free (lc.mon_thousands_sep);
-      lc.mon_thousands_sep = strdup ("");
-      free (lc.mon_grouping);
-      lc.mon_grouping = strdup ("");
-      lc.int_frac_digits = CHAR_MAX;
-      lc.frac_digits = CHAR_MAX;
-      free (lc.currency_symbol);
-      lc.currency_symbol = strdup ("");
-      free (lc.int_curr_symbol);
-      lc.int_curr_symbol = strdup ("");
-      lc.p_cs_precedes = CHAR_MAX;
-      lc.n_cs_precedes = CHAR_MAX;
-      lc.p_sep_by_space = CHAR_MAX;
-      lc.n_sep_by_space = CHAR_MAX;
-      free (lc.positive_sign);
-      lc.positive_sign = strdup ("");
-      free (lc.negative_sign);
-      lc.negative_sign = strdup ("");
-      lc.p_sign_posn = CHAR_MAX;
-      lc.n_sign_posn = CHAR_MAX;
+      free (locobj->lc.mon_decimal_point);
+      locobj->lc.mon_decimal_point = strdup ("");
+      free (locobj->lc.mon_thousands_sep);
+      locobj->lc.mon_thousands_sep = strdup ("");
+      free (locobj->lc.mon_grouping);
+      locobj->lc.mon_grouping = strdup ("");
+      locobj->lc.int_frac_digits = CHAR_MAX;
+      locobj->lc.frac_digits = CHAR_MAX;
+      free (locobj->lc.currency_symbol);
+      locobj->lc.currency_symbol = strdup ("");
+      free (locobj->lc.int_curr_symbol);
+      locobj->lc.int_curr_symbol = strdup ("");
+      locobj->lc.p_cs_precedes = CHAR_MAX;
+      locobj->lc.n_cs_precedes = CHAR_MAX;
+      locobj->lc.p_sep_by_space = CHAR_MAX;
+      locobj->lc.n_sep_by_space = CHAR_MAX;
+      free (locobj->lc.positive_sign);
+      locobj->lc.positive_sign = strdup ("");
+      free (locobj->lc.negative_sign);
+      locobj->lc.negative_sign = strdup ("");
+      locobj->lc.p_sign_posn = CHAR_MAX;
+      locobj->lc.n_sign_posn = CHAR_MAX;
     }
   else
     {
-      free (lc.int_curr_symbol);
-      lc.int_curr_symbol = strdup ((char *)read_symbol (3, monetary));
-      free (lc.currency_symbol);
-      lc.currency_symbol = strdup ((char *)read_symbol (4, monetary));
-      free (lc.mon_decimal_point);
-      lc.mon_decimal_point = strdup ((char *)read_symbol (5, monetary));
-      free (lc.mon_thousands_sep);
-      lc.mon_thousands_sep = strdup ((char *)read_symbol (6, monetary));
-      read_byte_list (7, &lc.mon_grouping, monetary);
-      free (lc.positive_sign);
-      lc.positive_sign = strdup ((char *)read_symbol (8, monetary));
-      free (lc.negative_sign);
-      lc.negative_sign = strdup ((char *)read_symbol (9, monetary));
-      lc.int_frac_digits = (char)read_symbol (10, monetary);
-      lc.frac_digits = (char)read_symbol (11, monetary);
-      lc.p_cs_precedes = (char)read_symbol (12, monetary);
-      lc.p_sep_by_space = (char)read_symbol (13, monetary);
-      lc.n_cs_precedes = (char)read_symbol (14, monetary);
-      lc.n_sep_by_space = (char)read_symbol (15, monetary);
-      lc.p_sign_posn = (char)read_symbol (16, monetary);
-      lc.n_sign_posn = (char)read_symbol (17, monetary);
+      free (locobj->lc.int_curr_symbol);
+      locobj->lc.int_curr_symbol = strdup ((char *)read_symbol (3, monetary));
+      free (locobj->lc.currency_symbol);
+      locobj->lc.currency_symbol = strdup ((char *)read_symbol (4, monetary));
+      free (locobj->lc.mon_decimal_point);
+      locobj->lc.mon_decimal_point = strdup ((char *)read_symbol (5, monetary));
+      free (locobj->lc.mon_thousands_sep);
+      locobj->lc.mon_thousands_sep = strdup ((char *)read_symbol (6, monetary));
+      read_byte_list (7, &locobj->lc.mon_grouping, monetary);
+      free (locobj->lc.positive_sign);
+      locobj->lc.positive_sign = strdup ((char *)read_symbol (8, monetary));
+      free (locobj->lc.negative_sign);
+      locobj->lc.negative_sign = strdup ((char *)read_symbol (9, monetary));
+      locobj->lc.int_frac_digits = (char)read_symbol (10, monetary);
+      locobj->lc.frac_digits = (char)read_symbol (11, monetary);
+      locobj->lc.p_cs_precedes = (char)read_symbol (12, monetary);
+      locobj->lc.p_sep_by_space = (char)read_symbol (13, monetary);
+      locobj->lc.n_cs_precedes = (char)read_symbol (14, monetary);
+      locobj->lc.n_sep_by_space = (char)read_symbol (15, monetary);
+      locobj->lc.p_sign_posn = (char)read_symbol (16, monetary);
+      locobj->lc.n_sign_posn = (char)read_symbol (17, monetary);
     }
-  return &lc;
+  return;
 }
+
+/* Defined by POSIX as not threadsafe */
+struct lconv *
+localeconv (void)
+{
+  __localeconv_l (&__locale_global);
+
+  return &__locale_global.lc;
+}
Index: gcc4/recipe/files/gcc/libunixlib/locale/newlocale.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/locale/newlocale.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/locale/newlocale.c	(working copy)
@@ -1,17 +1,17 @@
 /* Copyright (c) 2019 UnixLib Developers
  */
 
+#include <ctype.h>
+#include <errno.h>
+#include <locale.h>
 #include <stdlib.h>
-#include <locale.h>
 #include <stdio.h>
-#include <errno.h>
+#include <string.h>
 
-struct _locale {
-  struct lconv lc;
-};
+#include <internal/unix.h>
 
 /* This is supposed to be per-thread.  */
-static locale_t current_locale;
+static locale_t current_locale = LC_GLOBAL_LOCALE;
 
 locale_t uselocale(locale_t newloc)
 {
@@ -27,20 +27,94 @@
 
 void freelocale(locale_t locobj)
 {
-  if (locobj)
+  if (locobj) {
+    __localeconv_lconv_fini(&locobj->lc);
     free(locobj);
+  }
 }
 
 locale_t newlocale(int category_mask, const char *locale,
                           locale_t base)
 {
-  locale_t loc = (locale_t)malloc(sizeof(*loc));
-  if (!loc) {
+  struct _locale tmp;
+
+  PTHREAD_UNSAFE
+
+  if ((category_mask & ~LC_ALL_MASK) || locale == NULL) {
+    __set_errno (EINVAL);
+    return 0;
+  }
+
+  /* The locale provided will be verified by __setlocale_l() */
+
+  /* Prepare the temporary locale we will modify */
+  if (base != 0 && base != LC_GLOBAL_LOCALE) {
+    memcpy(&tmp, base, sizeof(tmp));
+  } else {
+    if (base == LC_GLOBAL_LOCALE) {
+      /* Undefined: be helpful to client */
+      memcpy(&tmp, &__locale_global, sizeof(tmp));
+    } else {
+      /* Initialise to C locale */
+      __build_ctype_tables(&tmp, -2);
+    }
+  }
+
+  /* Now, apply the requested locale to each of the categories in the mask */
+  if (category_mask == LC_ALL_MASK) {
+    /* Special-case LC_ALL to handle the encoded locale string */
+    if (__setlocale_l(&tmp, LC_ALL, locale) == NULL) {
+      return 0;
+    }
+  } else {
+    int category = 0, mask = category_mask;
+    while (mask != 0) {
+      if (mask & 1) {
+        if (__setlocale_l(&tmp, category, locale) == NULL) {
+          return 0;
+        }
+      }
+      category++;
+      mask >>= 1;
+    }
+  }
+
+  /* Allocate the result, if necessary */
+  if (base == 0 || base == LC_GLOBAL_LOCALE) {
+    base = (locale_t)malloc(sizeof(*base));
+    if (!base) {
+      __set_errno (ENOMEM);
+      return 0;
+    }
+  }
+  /* Fill it in */
+  memcpy(base, &tmp, sizeof(tmp));
+
+  /* Mark the lconv data stale */
+  base->lc_needs_refresh = 1;
+
+  return base;
+}
+
+locale_t duplocale(locale_t locobj)
+{
+  locale_t loc;
+
+  loc = (locale_t) malloc(sizeof(*loc));
+  if (loc == NULL) {
     __set_errno (ENOMEM);
     return 0;
   }
 
-  loc->lc = *localeconv();
+  if (locobj == LC_GLOBAL_LOCALE) {
+    memcpy(loc, &__locale_global, sizeof(*loc));
+  } else {
+    memcpy(loc, locobj, sizeof(*loc));
+  }
 
+  /* Invalidate lconv in the copy */
+  __localeconv_lconv_init(&loc->lc);
+  loc->lc_needs_refresh = 1;
+
   return loc;
 }
Index: gcc4/recipe/files/gcc/libunixlib/locale/nl_langinfo.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/locale/nl_langinfo.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/locale/nl_langinfo.c	(working copy)
@@ -24,3 +24,10 @@
 
   return (char *)value;
 }
+
+char *
+nl_langinfo_l (nl_item item, locale_t l)
+{
+  (void) l;
+  return nl_langinfo(item);
+}
Index: gcc4/recipe/files/gcc/libunixlib/locale/setlocale.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/locale/setlocale.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/locale/setlocale.c	(working copy)
@@ -195,7 +195,7 @@
 }
 
 static void
-do_lc_all (char *buffer, int size)
+do_lc_all (locale_t locobj, char *buffer, int size)
 {
   char temp[64];
   int category, same = 1;
@@ -204,13 +204,13 @@
      setlocale to set all locales.  If all locales are
      the same, then we can produce a very short string.  */
   for (category = 1; category < LC_ALL; ++category)
-    if (__locale_territory[0] != __locale_territory[category])
+    if (locobj->locale_territory[0] != locobj->locale_territory[category])
       same = 0;
 
   if (same)
     {
       /* All locales are set to the same territory.  */
-      territory_name (__locale_territory[0], buffer, size);
+      territory_name (locobj->locale_territory[0], buffer, size);
     }
   else
     {
@@ -218,7 +218,7 @@
 	   LC_CATEGORY=country;LC_CATEGORY=country; ...  */
       for (category = 0; category < LC_ALL; ++category)
 	{
-	  territory_name (__locale_territory[category], temp, sizeof (temp));
+	  territory_name (locobj->locale_territory[category], temp, sizeof (temp));
 	  buffer = stpcpy (buffer, locale_names[category]);
 	  *buffer++ = '=';
 	  buffer = stpcpy (buffer, temp);
@@ -230,15 +230,13 @@
 }
 
 char *
-setlocale (int category, const char *locale)
+__setlocale_l (locale_t locobj, int category, const char *locale)
 {
   int new_territory, changed;
   static char old_locale[256];
 
-  PTHREAD_UNSAFE
-
   /* This tells localeconv to re-read data for the lconv structure.  */
-  __setlocale_called = 1;
+  locobj->lc_needs_refresh = 1;
 
   if (locale == NULL)
     {
@@ -247,11 +245,11 @@
 	{
 	  /* The locale string is specially encoded for LC_ALL so we
 	     could restore all locales at any time.  */
-	  do_lc_all (old_locale, sizeof (old_locale));
+	  do_lc_all (locobj, old_locale, sizeof (old_locale));
 	  return old_locale;
 	}
 
-      territory_name (__locale_territory[category], old_locale, sizeof (old_locale));
+      territory_name (locobj->locale_territory[category], old_locale, sizeof (old_locale));
       return old_locale;
     }
 
@@ -270,7 +268,7 @@
       /* Encode the locale string, as we will be returning this
 	 later.  Remember, setlocale returns the locale settings
 	 that are about to be changed.  */
-      do_lc_all (old_locale, sizeof (old_locale));
+      do_lc_all (locobj, old_locale, sizeof (old_locale));
 
       /* Check for an encoded (composite) name.  Simply looking for
 	 a semi-colon will verify this.  */
@@ -332,11 +330,11 @@
 
 	  /* We now know all locales exist, so set them.  */
 	  for (category = 0; category < LC_ALL; ++category)
-	    __locale_territory[category] = territory_number (newnames[category]);
+	    locobj->locale_territory[category] = territory_number (newnames[category]);
 
 	  /* Re-build the character type tables according to the new
 	     locale settings.  */
-	  __build_ctype_tables (__locale_territory[LC_CTYPE]);
+	  __build_ctype_tables (locobj, locobj->locale_territory[LC_CTYPE]);
 	  return old_locale;
 	}
     }
@@ -359,9 +357,9 @@
       /* Change the locale for all categories. old_locale was created
 	 when we previously checked for a composite string.  */
       for (category = 0; category < LC_ALL; ++category)
-	if (__locale_territory[category] != new_territory)
+	if (locobj->locale_territory[category] != new_territory)
 	  {
-	    __locale_territory[category] = new_territory;
+	    locobj->locale_territory[category] = new_territory;
 	    changed = 1;
 	  }
     }
@@ -368,12 +366,12 @@
   else
     {
       /* Change the locale for just one category.  */
-      territory_name (__locale_territory[category],
+      territory_name (locobj->locale_territory[category],
 		      old_locale, sizeof (old_locale));
 
-      if (__locale_territory[category] != new_territory)
+      if (locobj->locale_territory[category] != new_territory)
 	{
-	  __locale_territory[category] = new_territory;
+	  locobj->locale_territory[category] = new_territory;
 	  changed = 1;
 	}
     }
@@ -384,7 +382,16 @@
      is changing.  The GNU Java compiler is known to repeatedly call
      setlocale.  */
   if (changed && (category == LC_ALL || category == LC_CTYPE))
-    __build_ctype_tables (new_territory);
+    __build_ctype_tables (locobj, new_territory);
 
   return old_locale;
 }
+
+char *
+setlocale (int category, const char *locale)
+{
+
+  PTHREAD_UNSAFE
+
+  return __setlocale_l(&__locale_global, category, locale);
+}
Index: gcc4/recipe/files/gcc/libunixlib/locale/strcoll.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/locale/strcoll.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/locale/strcoll.c	(working copy)
@@ -11,9 +11,15 @@
 int
 strcoll (const char *s1, const char *s2)
 {
+  return strcoll_l (s1, s2, &__locale_global);
+}
+
+int
+strcoll_l (const char *s1, const char *s2, locale_t l)
+{
   int regs[10];
 
-  regs[0] = __locale_territory[LC_COLLATE];
+  regs[0] = l->locale_territory[LC_COLLATE];
   regs[1] = (int)s1;
   regs[2] = (int)s2;
   regs[3] = 0;
@@ -21,3 +27,4 @@
   __os_swi (Territory_Collate, regs);
   return regs[0];
 }
+
Index: gcc4/recipe/files/gcc/libunixlib/locale/strxfrm.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/locale/strxfrm.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/locale/strxfrm.c	(working copy)
@@ -11,9 +11,15 @@
 size_t
 strxfrm (char *to, const char *from, size_t size)
 {
+  return strxfrm_l (to, from, size, &__locale_global);
+}
+
+size_t
+strxfrm_l (char *to, const char *from, size_t size, locale_t l)
+{
   int regs[10];
 
-  regs[0] = __locale_territory[LC_COLLATE];
+  regs[0] = l->locale_territory[LC_COLLATE];
   regs[1] = (int)to;
   regs[2] = (int)from;
   regs[3] = size;
Index: gcc4/recipe/files/gcc/libunixlib/locale/territory.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/locale/territory.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/locale/territory.c	(nonexistent)
@@ -1,10 +0,0 @@
-/* __locale_territory
- * Copyright (c) 2000-2006 UnixLib Developers
- */
-
-#include <locale.h>
-
-/* Global used for all calls to the Territory module. These variable
-   contain the territory number as set by setlocale. A value of
-   -1 means use the C locale.  */
-int __locale_territory[LC_ALL + 1];
Index: gcc4/recipe/files/gcc/libunixlib/string/stricmp.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/string/stricmp.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/string/stricmp.c	(working copy)
@@ -4,6 +4,7 @@
 #include <string.h>
 #include <strings.h>
 #include <ctype.h>
+#include <locale.h>
 
 int
 stricmp (const char *s1, const char *s2)
@@ -26,3 +27,24 @@
   return result;
 }
 strong_alias (stricmp, strcasecmp)
+
+int
+strcasecmp_l (const char *s1, const char *s2, locale_t locobj)
+{
+  const unsigned char *p1 = (const unsigned char *) s1;
+  const unsigned char *p2 = (const unsigned char *) s2;
+  int result = 0;
+
+  if (p1 == p2)
+    return result;
+
+  while (! result)
+    {
+      result = tolower_l (*p1, locobj) - tolower_l (*p2, locobj);
+      if (*p1++ == '\0')
+        break;
+      p2 ++;
+    }
+
+  return result;
+}
Index: gcc4/recipe/files/gcc/libunixlib/string/strnicmp.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/string/strnicmp.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/string/strnicmp.c	(working copy)
@@ -5,6 +5,7 @@
 #include <string.h>
 #include <strings.h>
 #include <ctype.h>
+#include <locale.h>
 
 int
 strnicmp (const char *s1, const char *s2, size_t n)
@@ -26,3 +27,22 @@
 }
 strong_alias (strnicmp, strncasecmp)
 
+int
+strncasecmp_l (const char *s1, const char *s2, size_t n, locale_t locobj)
+{
+  int i, j;
+
+  if (!n)
+    return 0;
+
+  do
+    {
+      i = *s1++, j = *s2++;
+      i = tolower_l (i, locobj);
+      j = tolower_l (j, locobj);
+    }
+  while (i && i == j && --n);
+
+  return i - j;
+}
+
Index: gcc4/recipe/files/gcc/libunixlib/sys/errlist.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/sys/errlist.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/sys/errlist.c	(working copy)
@@ -211,3 +211,10 @@
 
   return 0;
 }
+
+char *
+strerror_l (int errnum, locale_t l)
+{
+  (void) l;
+  return strerror (errnum);
+}
Index: gcc4/recipe/files/gcc/libunixlib/time/broken.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/time/broken.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/time/broken.c	(working copy)
@@ -49,7 +49,7 @@
 #ifdef __TARGET_SCL__
 					     -1,
 #else
-					     __locale_territory[LC_TIME],
+					     __locale_global.locale_territory[LC_TIME],
 #endif
 					     riscos_time, ordinals);
 }
Index: gcc4/recipe/files/gcc/libunixlib/time/gmtime_r.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/time/gmtime_r.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/time/gmtime_r.c	(working copy)
@@ -46,7 +46,7 @@
 #ifdef __TARGET_SCL__
 						     -1,
 #else
-						     __locale_territory[LC_TIME],
+						     __locale_global.locale_territory[LC_TIME],
 #endif
 						     riscos_time, ordinals)) != NULL)
     {
Index: gcc4/recipe/files/gcc/libunixlib/time/localtime_r.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/time/localtime_r.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/time/localtime_r.c	(working copy)
@@ -28,7 +28,7 @@
 #ifdef __TARGET_SCL__
 						  -1,
 #else
-						  __locale_territory[LC_TIME],
+						  __locale_global.locale_territory[LC_TIME],
 #endif
 						  riscos_time, ordinals)) != NULL)
     {
Index: gcc4/recipe/files/gcc/libunixlib/time/mktime.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/time/mktime.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/time/mktime.c	(working copy)
@@ -23,7 +23,7 @@
 #ifdef __TARGET_SCL__
 						     -1,
 #else
-						     __locale_territory[LC_TIME],
+						     __locale_global.locale_territory[LC_TIME],
 #endif
 						     riscos_time,
 						     ordinals)) != NULL)
Index: gcc4/recipe/files/gcc/libunixlib/time/stdtime.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/time/stdtime.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/time/stdtime.c	(working copy)
@@ -48,7 +48,7 @@
 #ifdef __TARGET_SCL__
 					       -1,
 #else
-					       __locale_territory[LC_TIME],
+					       __locale_global.locale_territory[LC_TIME],
 #endif
 					       riscos_time,
 					       result,
Index: gcc4/recipe/files/gcc/libunixlib/time/strftime.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/time/strftime.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/time/strftime.c	(working copy)
@@ -353,7 +353,7 @@
   int regs[10];
   char buffer[64];
 
-  regs[0] = __locale_territory[LC_TIME];
+  regs[0] = __locale_global.locale_territory[LC_TIME];
   regs[1] = (int)timep;
   regs[2] = (int)buffer;
   regs[3] = sizeof (buffer) - 1;
@@ -368,7 +368,7 @@
   int regs[10];
   char buffer[64];
 
-  regs[0] = __locale_territory[LC_TIME];
+  regs[0] = __locale_global.locale_territory[LC_TIME];
   regs[1] = (int)timep;
   regs[2] = (int)buffer;
   regs[3] = sizeof (buffer) - 1;
Index: gcc4/recipe/files/gcc/libunixlib/time/tzset.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/time/tzset.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/time/tzset.c	(working copy)
@@ -36,7 +36,7 @@
   /* Get timezone information for current territory.  */
   _kernel_swi_regs regs;
 #ifndef __TARGET_SCL__
-  regs.r[0] = __locale_territory[LC_TIME];
+  regs.r[0] = __locale_global.locale_territory[LC_TIME];
 #else
   regs.r[0] = -1; /* Current territory.  */
 #endif
Index: gcc4/recipe/files/gcc/libunixlib/unix/unix.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/unix/unix.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/unix/unix.c	(working copy)
@@ -191,7 +191,7 @@
   __pthread_prog_init ();
   __unixlib_signal_initialise (__u);
   /* Initialise ctype tables to the C locale.  */
-  __build_ctype_tables (-2);
+  __build_ctype_tables (&__locale_global, -2);
   /* Define and initialise the Unix I/O.  */
   initialise_unix_io ();
   __stdioinit ();
Index: gcc4/recipe/files/gcc/libunixlib/vscript
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/vscript	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/vscript	(working copy)
@@ -58,6 +58,9 @@
      __init_des_r;
      __init_des;
      __invalidate;
+     __locale_global;
+     __localeconv_lconv_init;
+     __localeconv_lconv_fini;
      malloc_trim;
      malloc_trim_unlocked;
      malloc_unlocked;
@@ -80,6 +83,7 @@
      __res_vinit;
      __runtime_features;
      __setup_signalhandler_stack;
+     __setlocale_l;
      __sdirinit;
      __sfixinit;
      __sfixfind;
Index: gcc4/recipe/files/gcc/libunixlib/wchar/wctype.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/wchar/wctype.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/wchar/wctype.c	(working copy)
@@ -4,6 +4,7 @@
  */
 
 #include <ctype.h>
+#include <locale.h>
 #include <wctype.h>
 
 int
@@ -71,3 +72,69 @@
 {
   return isxdigit (wc);
 }
+
+int
+iswalnum_l (wint_t wc, locale_t locale)
+{
+  return isalnum_l (wc, locale);
+}
+
+int
+iswalpha_l (wint_t wc, locale_t locale)
+{
+  return isalpha_l (wc, locale);
+}
+
+int
+iswcntrl_l (wint_t wc, locale_t locale)
+{
+  return iscntrl_l (wc, locale);
+}
+
+int
+iswdigit_l (wint_t wc, locale_t locale)
+{
+  return isdigit_l (wc, locale);
+}
+
+int
+iswgraph_l (wint_t wc, locale_t locale)
+{
+  return isgraph_l (wc, locale);
+}
+
+int
+iswprint_l (wint_t wc, locale_t locale)
+{
+  return isprint_l (wc, locale);
+}
+
+int
+iswpunct_l (wint_t wc, locale_t locale)
+{
+  return ispunct_l (wc, locale);
+}
+
+int
+iswspace_l (wint_t wc, locale_t locale)
+{
+  return isspace_l (wc, locale);
+}
+
+int
+iswxdigit_l (wint_t wc, locale_t locale)
+{
+  return isxdigit_l (wc, locale);
+}
+
+wint_t
+towlower_l (wint_t wc, locale_t locale)
+{
+  return tolower_l (wc, locale);
+}
+
+wint_t
+towupper_l (wint_t wc, locale_t locale)
+{
+  return toupper_l (wc, locale);
+}
Index: gcc4/recipe/files/gcc/libunixlib/wchar/wmissing.c
===================================================================
--- gcc4/recipe/files/gcc/libunixlib/wchar/wmissing.c	(revision 7698)
+++ gcc4/recipe/files/gcc/libunixlib/wchar/wmissing.c	(working copy)
@@ -1,7 +1,7 @@
+#include <locale.h>
+#include <stdio.h>
 #include <stdlib.h>
 #include <wctype.h>
-#include <wctype.h>
-#include <stdio.h>
 
 int iswupper (wint_t __wc)
 {
@@ -9,13 +9,26 @@
   abort();
 }
 
-unsigned long int wcstoul (__const wchar_t *__restrict __nptr,
-				  wchar_t **__restrict __endptr, int __base)
+int
+iswupper_l (wint_t wc, locale_t locale)
 {
   printf("%s: Not implemented\n", __func__);
   abort();
 }
 
+int iswlower (wint_t __wc)
+{
+  printf("%s: Not implemented\n", __func__);
+  abort();
+}
+
+int
+iswlower_l (wint_t wc, locale_t locale)
+{
+  printf("%s: Not implemented\n", __func__);
+  abort();
+}
+
 int wcscoll (__const wchar_t *__s1, __const wchar_t *__s2)
 {
   printf("%s: Not implemented\n", __func__);
@@ -22,12 +35,47 @@
   abort();
 }
 
-int iswlower (wint_t __wc)
+int wcscoll_l (__const wchar_t *__s1, __const wchar_t *__s2,
+		locale_t loc)
 {
   printf("%s: Not implemented\n", __func__);
   abort();
 }
 
+int wcscasecmp (__const wchar_t *__s1, __const wchar_t *__s2)
+{
+  printf("%s: Not implemented\n", __func__);
+  abort();
+}
+
+int wcsncasecmp (__const wchar_t *__s1, __const wchar_t *__s2,
+		size_t __n)
+{
+  printf("%s: Not implemented\n", __func__);
+  abort();
+}
+
+int wcscasecmp_l (__const wchar_t *__s1, __const wchar_t *__s2,
+		 locale_t __loc)
+{
+  printf("%s: Not implemented\n", __func__);
+  abort();
+}
+
+int wcsncasecmp_l (__const wchar_t *__s1, __const wchar_t *__s2,
+		size_t __n, locale_t __loc)
+{
+  printf("%s: Not implemented\n", __func__);
+  abort();
+}
+
+unsigned long int wcstoul (__const wchar_t *__restrict __nptr,
+				  wchar_t **__restrict __endptr, int __base)
+{
+  printf("%s: Not implemented\n", __func__);
+  abort();
+}
+
 long long int wcstoll (__const wchar_t *__restrict __nptr,
 			      wchar_t **__restrict __endptr, int __base)
 {
@@ -63,6 +111,13 @@
   abort();
 }
 
+size_t wcsxfrm_l (wchar_t *__s1, __const wchar_t *__s2,
+		size_t __n, locale_t __loc)
+{
+  printf("%s: Not implemented\n", __func__);
+  abort();
+}
+
 float wcstof (__const wchar_t *__restrict __nptr,
 		     wchar_t **__restrict __endptr)
 {
@@ -83,6 +138,13 @@
   abort();
 }
 
+int
+iswblank_l (wint_t __wc, locale_t __locale)
+{
+  printf("%s: Not implemented\n", __func__);
+  abort();
+}
+
 int iswctype (wint_t __wc, wctype_t __desc)
 {
   printf("%s: Not implemented\n", __func__);
@@ -89,6 +151,12 @@
   abort();
 }
 
+int iswctype_l (wint_t __wc, wctype_t __desc, locale_t __locale)
+{
+  printf("%s: Not implemented\n", __func__);
+  abort();
+}
+
 unsigned long long int wcstoull (__const wchar_t *__restrict __nptr,
 					wchar_t **__restrict __endptr,
 					int __base)
@@ -116,6 +184,12 @@
   abort();
 }
 
+wctype_t wctype_l (__const char *__property, locale_t __locale)
+{
+  printf("%s: Not implemented\n", __func__);
+  abort();
+}
+
 wint_t ungetwc(wint_t wc, FILE *stream)
 {
   printf("%s: Not implemented\n", __func__);
