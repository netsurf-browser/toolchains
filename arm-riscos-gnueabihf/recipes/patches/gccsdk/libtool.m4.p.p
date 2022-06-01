Index: gcc/libtool.m4.p
===================================================================
--- gcc/libtool.m4.p	(revision 7698)
+++ gcc/libtool.m4.p	(working copy)
@@ -5,7 +5,7 @@
      ])
      ;;
 + 
-+  *riscos | *linux-gnueabihf)
++  *riscos*)
 +    lt_cv_dlopen="dlopen"
 +    lt_cv_dlopen_libs="-ldl"
 +    lt_cv_dlopen_self=yes
@@ -17,7 +17,7 @@
    shlibpath_var=LD_LIBRARY_PATH
    ;;
  
-+*riscos | linux-gnueabihf)
++*riscos*)
 +  version_type=linux
 +  library_names_spec='${libname}$versuffix.so ${libname}.so$major $libname.so'
 +  soname_spec='${libname}.so$major'
@@ -32,7 +32,7 @@
        # it will coredump.
        _LT_TAGVAR(lt_prog_compiler_pic, $1)='-fPIC -shared'
        ;;
-+    *linux-gnueabihf)
++    riscos-gnu*)
 +      _LT_TAGVAR(lt_prog_compiler_pic, $1)='-fpic'
 +      _LT_TAGVAR(lt_prog_compiler_static, $1)='-static'
 +      _LT_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
@@ -48,7 +48,7 @@
      esac
    else
      case $host_os in
-+      linux-gnueabihf)
++      riscos-gnu*)
 +	# RISC OS
 +        _LT_TAGVAR(lt_prog_compiler_pic, $1)='-fPIC'
 +        _LT_TAGVAR(lt_prog_compiler_static, $1)='-static'
@@ -61,7 +61,7 @@
  	;;
        vxworks*)
  	;;
-+      *linux-gnueabihf)
++      riscos-gnu*)
 +        _LT_TAGVAR(lt_prog_compiler_pic, $1)='-fpic'
 +        _LT_TAGVAR(lt_prog_compiler_static, $1)='-static'
 +        _LT_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
@@ -78,13 +78,13 @@
        fi
        ;;
  
-+    *linux-gnueabihf)
++    riscos-gnu*)
 +      _LT_TAGVAR(lt_prog_compiler_pic, $1)='-fpic'
 +      _LT_TAGVAR(lt_prog_compiler_static, $1)='-static'
 +      _LT_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
 +      ;;
 +
-+    *riscos | linux-gnueabihf)
++    *riscos*)
 +      _LT_TAGVAR(lt_prog_compiler_pic, $1)='-fPIC'
 +      _LT_TAGVAR(lt_prog_compiler_static, $1)='-static'
 +      _LT_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
@@ -97,7 +97,7 @@
    else
      # PORTME Check for flag to pass linker flags through the system compiler.
      case $host_os in
-+    linux-gnueabihf)
++    riscos-gnu*)
 +      # RISC OS
 +      _LT_TAGVAR(lt_prog_compiler_pic, $1)='-fPIC'
 +      _LT_TAGVAR(lt_prog_compiler_static, $1)='-static'
@@ -110,7 +110,7 @@
        _LT_TAGVAR(lt_prog_compiler_static, $1)='-Bstatic'
        ;;
  
-+    *linux-gnueabihf)
++    riscos-gnu*)
 +      _LT_TAGVAR(lt_prog_compiler_pic, $1)='-fpic'
 +      _LT_TAGVAR(lt_prog_compiler_static, $1)='-static'
 +      _LT_TAGVAR(lt_prog_compiler_wl, $1)='-Wl,'
@@ -129,7 +129,7 @@
    else
      # PORTME fill in a description of your system's linker (not GNU ld)
      case $host_os in
-+    linux-gnueabihf)
++    riscos-gnu*)
 +      # RISC OS
 +      _LT_TAGVAR(ld_shlibs, $1)=yes
 +      ;;
@@ -140,7 +140,7 @@
        _LT_TAGVAR(hardcode_shlibpath_var, $1)=no
        ;;
  
-+    *riscos | *linux-gnueabihf)
++    *riscos*)
 +      _LT_TAGVAR(ld_shlibs, $1)=yes
 +      ;;
 +
@@ -151,7 +151,7 @@
      AC_MSG_CHECKING([whether the $compiler linker ($LD) supports shared libraries])
      _LT_TAGVAR(ld_shlibs, $1)=yes
      case $host_os in
-+      linux-gnueabihf)
++      riscos-gnu*)
 +	 # RISC OS
 +         _LT_TAGVAR(ld_shlibs, $1)=yes
 +         ;;
@@ -162,7 +162,7 @@
          _LT_TAGVAR(ld_shlibs, $1)=no
          ;;
  
-+     *riscos | *linux-gnueabihf)
++     *riscos*)
 +        _LT_TAGVAR(ld_shlibs, $1)=yes
 +        ;;
 +
