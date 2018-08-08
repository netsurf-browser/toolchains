--- srclib/signal.in.h	2017-01-01 23:02:22.000000000 +0000
+++ srclib/signal.in.h	2017-03-26 22:56:18.360017409 +0100
@@ -211,69 +211,6 @@ typedef int verify_NSIG_constraint[NSIG
 # undef sigismember
 #endif
 
-/* Test whether a given signal is contained in a signal set.  */
-# if @HAVE_POSIX_SIGNALBLOCKING@
-/* This function is defined as a macro on Mac OS X.  */
-#  if defined __cplusplus && defined GNULIB_NAMESPACE
-#   undef sigismember
-#  endif
-# else
-_GL_FUNCDECL_SYS (sigismember, int, (const sigset_t *set, int sig)
-                                    _GL_ARG_NONNULL ((1)));
-# endif
-_GL_CXXALIAS_SYS (sigismember, int, (const sigset_t *set, int sig));
-_GL_CXXALIASWARN (sigismember);
-
-/* Initialize a signal set to the empty set.  */
-# if @HAVE_POSIX_SIGNALBLOCKING@
-/* This function is defined as a macro on Mac OS X.  */
-#  if defined __cplusplus && defined GNULIB_NAMESPACE
-#   undef sigemptyset
-#  endif
-# else
-_GL_FUNCDECL_SYS (sigemptyset, int, (sigset_t *set) _GL_ARG_NONNULL ((1)));
-# endif
-_GL_CXXALIAS_SYS (sigemptyset, int, (sigset_t *set));
-_GL_CXXALIASWARN (sigemptyset);
-
-/* Add a signal to a signal set.  */
-# if @HAVE_POSIX_SIGNALBLOCKING@
-/* This function is defined as a macro on Mac OS X.  */
-#  if defined __cplusplus && defined GNULIB_NAMESPACE
-#   undef sigaddset
-#  endif
-# else
-_GL_FUNCDECL_SYS (sigaddset, int, (sigset_t *set, int sig)
-                                  _GL_ARG_NONNULL ((1)));
-# endif
-_GL_CXXALIAS_SYS (sigaddset, int, (sigset_t *set, int sig));
-_GL_CXXALIASWARN (sigaddset);
-
-/* Remove a signal from a signal set.  */
-# if @HAVE_POSIX_SIGNALBLOCKING@
-/* This function is defined as a macro on Mac OS X.  */
-#  if defined __cplusplus && defined GNULIB_NAMESPACE
-#   undef sigdelset
-#  endif
-# else
-_GL_FUNCDECL_SYS (sigdelset, int, (sigset_t *set, int sig)
-                                  _GL_ARG_NONNULL ((1)));
-# endif
-_GL_CXXALIAS_SYS (sigdelset, int, (sigset_t *set, int sig));
-_GL_CXXALIASWARN (sigdelset);
-
-/* Fill a signal set with all possible signals.  */
-# if @HAVE_POSIX_SIGNALBLOCKING@
-/* This function is defined as a macro on Mac OS X.  */
-#  if defined __cplusplus && defined GNULIB_NAMESPACE
-#   undef sigfillset
-#  endif
-# else
-_GL_FUNCDECL_SYS (sigfillset, int, (sigset_t *set) _GL_ARG_NONNULL ((1)));
-# endif
-_GL_CXXALIAS_SYS (sigfillset, int, (sigset_t *set));
-_GL_CXXALIASWARN (sigfillset);
-
 /* Return the set of those blocked signals that are pending.  */
 # if !@HAVE_POSIX_SIGNALBLOCKING@
 _GL_FUNCDECL_SYS (sigpending, int, (sigset_t *set) _GL_ARG_NONNULL ((1)));
