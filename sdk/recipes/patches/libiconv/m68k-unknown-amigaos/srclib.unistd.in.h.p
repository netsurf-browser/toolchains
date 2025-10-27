--- srclib/unistd.in.h.orig	2017-03-27 23:12:52.149619622 +0100
+++ srclib/unistd.in.h	2017-03-27 23:13:12.582243962 +0100
@@ -1873,44 +1873,6 @@
 #endif
 
 
-#if @GNULIB_READLINK@
-/* Read the contents of the symbolic link FILE and place the first BUFSIZE
-   bytes of it into BUF.  Return the number of bytes placed into BUF if
-   successful, otherwise -1 and errno set.
-   See the POSIX:2008 specification
-   <https://pubs.opengroup.org/onlinepubs/9699919799/functions/readlink.html>.  */
-# if @REPLACE_READLINK@
-#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
-#   define readlink rpl_readlink
-#  endif
-_GL_FUNCDECL_RPL (readlink, ssize_t,
-                  (const char *restrict file,
-                   char *restrict buf, size_t bufsize)
-                  _GL_ARG_NONNULL ((1, 2)));
-_GL_CXXALIAS_RPL (readlink, ssize_t,
-                  (const char *restrict file,
-                   char *restrict buf, size_t bufsize));
-# else
-#  if !@HAVE_READLINK@
-_GL_FUNCDECL_SYS (readlink, ssize_t,
-                  (const char *restrict file,
-                   char *restrict buf, size_t bufsize)
-                  _GL_ARG_NONNULL ((1, 2)));
-#  endif
-_GL_CXXALIAS_SYS (readlink, ssize_t,
-                  (const char *restrict file,
-                   char *restrict buf, size_t bufsize));
-# endif
-_GL_CXXALIASWARN (readlink);
-#elif defined GNULIB_POSIXCHECK
-# undef readlink
-# if HAVE_RAW_DECL_READLINK
-_GL_WARN_ON_USE (readlink, "readlink is unportable - "
-                 "use gnulib module readlink for portability");
-# endif
-#endif
-
-
 #if @GNULIB_READLINKAT@
 # if @REPLACE_READLINKAT@
 #  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
