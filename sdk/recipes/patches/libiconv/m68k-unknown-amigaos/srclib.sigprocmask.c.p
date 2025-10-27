--- srclib/sigprocmask.c.orig	2011-08-07 14:42:06.000000000 +0100
+++ srclib/sigprocmask.c	2014-01-15 00:59:33.130968152 +0000
@@ -110,77 +110,6 @@
 # define signal ext_signal
 #endif
 
-int
-sigismember (const sigset_t *set, int sig)
-{
-  if (sig >= 0 && sig < NSIG)
-    {
-      #ifdef SIGABRT_COMPAT
-      if (sig == SIGABRT_COMPAT)
-        sig = SIGABRT;
-      #endif
-
-      return (*set >> sig) & 1;
-    }
-  else
-    return 0;
-}
-
-int
-sigemptyset (sigset_t *set)
-{
-  *set = 0;
-  return 0;
-}
-
-int
-sigaddset (sigset_t *set, int sig)
-{
-  if (sig >= 0 && sig < NSIG)
-    {
-      #ifdef SIGABRT_COMPAT
-      if (sig == SIGABRT_COMPAT)
-        sig = SIGABRT;
-      #endif
-
-      *set |= 1U << sig;
-      return 0;
-    }
-  else
-    {
-      errno = EINVAL;
-      return -1;
-    }
-}
-
-int
-sigdelset (sigset_t *set, int sig)
-{
-  if (sig >= 0 && sig < NSIG)
-    {
-      #ifdef SIGABRT_COMPAT
-      if (sig == SIGABRT_COMPAT)
-        sig = SIGABRT;
-      #endif
-
-      *set &= ~(1U << sig);
-      return 0;
-    }
-  else
-    {
-      errno = EINVAL;
-      return -1;
-    }
-}
-
-
-int
-sigfillset (sigset_t *set)
-{
-  *set = ((2U << (NSIG - 1)) - 1) & ~ SIGABRT_COMPAT_MASK;
-  return 0;
-}
-
 /* Set of currently blocked signals.  */
 static volatile sigset_t blocked_set /* = 0 */;
 
