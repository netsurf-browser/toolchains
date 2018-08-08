Index: gcc4/Makefile
===================================================================
--- gcc4/Makefile	(revision 7156)
+++ gcc4/Makefile	(working copy)
@@ -45,7 +45,7 @@
 MPFR_VERSION=3.0.1
 MPC_VERSION=0.8.2
 GCC_USE_PPL_CLOOG=yes
-PPL_VERSION=0.11.2
+PPL_VERSION=1.2
 CLOOG_VERSION=0.15.11
 GCC_USE_LTO=yes
 
@@ -182,8 +182,8 @@
 RONATIVE_MPFR_CONFIG_ARGS := --disable-shared --with-gmp=$(PREFIX_RONATIVEGCC_LIBS) --host=$(TARGET) --prefix=$(PREFIX_RONATIVEGCC_LIBS)
 
 # Configure arguments PPL:
-CROSS_PPL_CONFIG_ARGS := --disable-shared --disable-watchdog --with-gnu-ld --with-gmp-prefix=$(PREFIX_CROSSGCC_LIBS) --prefix=$(PREFIX_CROSSGCC_LIBS)
-RONATIVE_PPL_CONFIG_ARGS := --disable-shared --disable-watchdog --with-gnu-ld --with-gmp-prefix=$(PREFIX_RONATIVEGCC_LIBS) --host=$(TARGET) --prefix=$(PREFIX_RONATIVEGCC_LIBS)
+CROSS_PPL_CONFIG_ARGS := --disable-shared --disable-watchdog --with-gnu-ld --with-gmp=$(PREFIX_CROSSGCC_LIBS) --prefix=$(PREFIX_CROSSGCC_LIBS)
+RONATIVE_PPL_CONFIG_ARGS := --disable-shared --disable-watchdog --with-gnu-ld --with-gmp=$(PREFIX_RONATIVEGCC_LIBS) --host=$(TARGET) --prefix=$(PREFIX_RONATIVEGCC_LIBS)
 
 # Configure arguments CLooG:
 CROSS_CLOOG_CONFIG_ARGS := --disable-shared --with-gmp=$(PREFIX_CROSSGCC_LIBS) --with-bits=gmp --with-ppl=$(PREFIX_CROSSGCC_LIBS) --with-host-libstdcxx='-lstdc++' --prefix=$(PREFIX_CROSSGCC_LIBS)
Index: gcc4/recipe/patches/cloog/configure.in.p
===================================================================
--- gcc4/recipe/patches/cloog/configure.in.p	(revision 7156)
+++ gcc4/recipe/patches/cloog/configure.in.p	(working copy)
@@ -1,5 +1,5 @@
---- configure.in.orig	2013-05-01 16:15:33.235945936 +0200
-+++ configure.in	2013-05-01 16:15:15.484169106 +0200
+--- configure.in.orig	2017-04-23 11:55:00.956204411 +0100
++++ configure.in	2017-04-23 11:50:39.385519538 +0100
 @@ -41,7 +41,7 @@ dnl ************************************
  AC_PREREQ(2.13)
  AC_INIT(source/cloog.c)
@@ -9,7 +9,13 @@
  dnl Every other copy of the package version number gets its value from here.
  AM_INIT_AUTOMAKE(cloog, 0.15)
  AC_CONFIG_HEADER(include/cloog/cloog-config.h)
-@@ -343,7 +343,7 @@ if test "x$with_ppl" != "x" -a "x$with_p
+@@ -338,12 +338,12 @@ if test "x$with_ppl" != "x" -a "x$with_p
+ 
+ 	AC_MSG_CHECKING([for version 0.10 (or later revision) of PPL])
+ 	AC_TRY_COMPILE([#include "ppl_c.h"],[
+-	#if PPL_VERSION_MAJOR != 0 || PPL_VERSION_MINOR < 10
++	#if PPL_VERSION_MAJOR == 0 && PPL_VERSION_MINOR < 10
+ 	choke me
  	#endif
  	], [AC_MSG_RESULT([yes])], [AC_MSG_ERROR(Can't find correct version of PPL.) ])
  
Index: gcc4/recipe/patches/gcc/configure.ac.p
===================================================================
--- gcc4/recipe/patches/gcc/configure.ac.p	(revision 7156)
+++ gcc4/recipe/patches/gcc/configure.ac.p	(working copy)
@@ -1,6 +1,6 @@
 Index: configure.ac
 ===================================================================
---- configure.ac	(revision 197339)
+--- configure.ac	(revision 247077)
 +++ configure.ac	(working copy)
 @@ -152,6 +152,7 @@
  # Note that libiberty is not a target library.
@@ -34,6 +34,15 @@
  
    if test "$enable_ppl_version_check" != no; then
      saved_CFLAGS="$CFLAGS"
+@@ -1577,7 +1586,7 @@
+     CFLAGS="$CFLAGS $pplinc $gmpinc"
+     AC_MSG_CHECKING([for version 0.11 (revision 0 or later) of PPL])
+     AC_TRY_COMPILE([#include "ppl_c.h"],[
+-    #if PPL_VERSION_MAJOR != 0 || PPL_VERSION_MINOR < 11
++    #if PPL_VERSION_MAJOR == 0 && PPL_VERSION_MINOR < 11
+     choke me
+     #endif
+     ], [AC_MSG_RESULT([yes])], [AC_MSG_RESULT([no]); ppllibs= ; pplinc= ; with_ppl=no ])
 @@ -2895,6 +2904,10 @@
     ;;
    esac
Index: gcc4/recipe/patches/ppl/src.Congruence_System.defs.hh.p
===================================================================
--- gcc4/recipe/patches/ppl/src.Congruence_System.defs.hh.p	(revision 7156)
+++ gcc4/recipe/patches/ppl/src.Congruence_System.defs.hh.p	(nonexistent)
@@ -1,19 +0,0 @@
---- src/Congruence_System.defs.hh.orig	2015-03-01 19:57:52.887328139 +0100
-+++ src/Congruence_System.defs.hh	2015-03-01 19:57:12.315328259 +0100
-@@ -33,6 +33,7 @@ site: http://www.cs.unipr.it/ppl/ . */
- #include "Grid.types.hh"
- #include "Grid_Certificate.types.hh"
- #include <iosfwd>
-+#include <cstddef>
- 
- namespace Parma_Polyhedra_Library {
- 
-@@ -235,7 +236,7 @@ public:
-   class const_iterator
-     : public std::iterator<std::forward_iterator_tag,
- 			   Congruence,
--			   ptrdiff_t,
-+			   std::ptrdiff_t,
- 			   const Congruence*,
- 			   const Congruence&> {
-   public:
Index: gcc4/recipe/patches/ppl/src.Constraint_System.defs.hh.p
===================================================================
--- gcc4/recipe/patches/ppl/src.Constraint_System.defs.hh.p	(revision 7156)
+++ gcc4/recipe/patches/ppl/src.Constraint_System.defs.hh.p	(nonexistent)
@@ -1,19 +0,0 @@
---- src/Constraint_System.defs.hh.orig	2015-03-01 19:58:03.831328106 +0100
-+++ src/Constraint_System.defs.hh	2015-03-01 19:57:12.315328259 +0100
-@@ -33,6 +33,7 @@ site: http://www.cs.unipr.it/ppl/ . */
- #include "Congruence_System.types.hh"
- #include <iterator>
- #include <iosfwd>
-+#include <cstddef>
- 
- namespace Parma_Polyhedra_Library {
- 
-@@ -204,7 +205,7 @@ public:
-   class const_iterator
-     : public std::iterator<std::forward_iterator_tag,
- 			   Constraint,
--			   ptrdiff_t,
-+			   std::ptrdiff_t,
- 			   const Constraint*,
- 			   const Constraint&> {
-   public:
Index: gcc4/recipe/patches/ppl/src.Generator_System.defs.hh.p
===================================================================
--- gcc4/recipe/patches/ppl/src.Generator_System.defs.hh.p	(revision 7156)
+++ gcc4/recipe/patches/ppl/src.Generator_System.defs.hh.p	(nonexistent)
@@ -1,19 +0,0 @@
---- src/Generator_System.defs.hh.orig	2015-03-01 19:58:13.511328077 +0100
-+++ src/Generator_System.defs.hh	2015-03-01 19:57:12.319328259 +0100
-@@ -33,6 +33,7 @@ site: http://www.cs.unipr.it/ppl/ . */
- #include "Polyhedron.types.hh"
- #include "Poly_Con_Relation.defs.hh"
- #include <iosfwd>
-+#include <cstddef>
- 
- namespace Parma_Polyhedra_Library {
- 
-@@ -250,7 +251,7 @@ public:
-   class const_iterator
-     : public std::iterator<std::forward_iterator_tag,
- 			   Generator,
--			   ptrdiff_t,
-+			   std::ptrdiff_t,
- 			   const Generator*,
- 			   const Generator&> {
-   public:
Index: gcc4/recipe/patches/ppl/src.Grid_Generator_System.defs.hh.p
===================================================================
--- gcc4/recipe/patches/ppl/src.Grid_Generator_System.defs.hh.p	(revision 7156)
+++ gcc4/recipe/patches/ppl/src.Grid_Generator_System.defs.hh.p	(nonexistent)
@@ -1,19 +0,0 @@
---- src/Grid_Generator_System.defs.hh.orig	2015-03-01 19:57:31.127328203 +0100
-+++ src/Grid_Generator_System.defs.hh	2015-03-01 19:57:12.315328259 +0100
-@@ -30,6 +30,7 @@ site: http://www.cs.unipr.it/ppl/ . */
- #include "Variables_Set.types.hh"
- #include "Grid.types.hh"
- #include <iosfwd>
-+#include <cstddef>
- 
- namespace Parma_Polyhedra_Library {
- 
-@@ -267,7 +268,7 @@ public:
-   class const_iterator
-     : public std::iterator<std::forward_iterator_tag,
- 			   Grid_Generator,
--			   ptrdiff_t,
-+			   std::ptrdiff_t,
- 			   const Grid_Generator*,
- 			   const Grid_Generator&>,
-       private Generator_System::const_iterator {
