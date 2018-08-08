--- src/tool_operate.c.orig	2018-01-26 13:44:37.960724591 +0000
+++ src/tool_operate.c	2018-01-26 13:44:48.385035073 +0000
@@ -1832,15 +1832,6 @@
           }
         }
 
-#ifdef __AMIGA__
-        if(!result && outs.s_isreg && outs.filename) {
-          /* Set the url (up to 80 chars) as comment for the file */
-          if(strlen(url) > 78)
-            url[79] = '\0';
-          SetComment(outs.filename, url);
-        }
-#endif
-
 #if defined(HAVE_UTIME) || \
     (defined(WIN32) && (SIZEOF_CURL_OFF_T >= 8))
         /* File time can only be set _after_ the file has been closed */
