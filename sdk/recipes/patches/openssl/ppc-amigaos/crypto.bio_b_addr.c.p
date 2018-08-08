--- crypto/bio/b_addr.c.orig	2017-11-21 22:49:00.185608040 +0000
+++ crypto/bio/b_addr.c	2017-11-21 22:49:21.488219518 +0000
@@ -854,7 +854,7 @@
                 addrlistp++)
                 ;
 
-            for(addresses = addrlistp - he->h_addr_list;
+            for(addresses = addrlistp - (char **)he->h_addr_list;
                 addrlistp--, addresses-- > 0; ) {
                 if (!addrinfo_wrap(he->h_addrtype, socktype,
                                    *addrlistp, he->h_length,
