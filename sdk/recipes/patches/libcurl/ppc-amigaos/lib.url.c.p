--- lib/url.c	2014-05-26 18:40:19.896019262 +0100
+++ lib/url.c	2014-05-26 18:40:01.224019316 +0100
@@ -4004,7 +4004,7 @@ static CURLcode parseurlandfillconn(stru
       }
       else {
         /* Zone identifier is not numeric */
-#if defined(HAVE_NET_IF_H) && defined(IFNAMSIZ)
+#if defined(HAVE_NET_IF_H) && defined(IFNAMSIZ) && !defined(__amigaos4__)
         char ifname[IFNAMSIZ + 2];
         char *square_bracket;
         unsigned int scopeidx = 0;
