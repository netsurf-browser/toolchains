--- test/drbgtest.c.orig	2017-10-10 15:19:45.611896396 +0100
+++ test/drbgtest.c	2017-10-17 15:42:25.304921197 +0100
@@ -277,7 +277,7 @@
 }
 
 
-#if defined(OPENSSL_SYS_UNIX) && !defined(OPENSSL_RAND_SEED_EGD)
+#if defined(OPENSSL_SYS_UNIX) && !defined(OPENSSL_RAND_SEED_EGD) && !defined(OPENSSL_SYS_AMIGAOS3) && !defined(OPENSSL_SYS_AMIGAOS4)
 /* number of children to fork */
 #define DRBG_FORK_COUNT 9
 /* two results per child, two for the parent */
@@ -895,7 +895,7 @@
 int setup_tests(void)
 {
     ADD_TEST(test_rand_reseed);
-#if defined(OPENSSL_SYS_UNIX) && !defined(OPENSSL_RAND_SEED_EGD)
+#if defined(OPENSSL_SYS_UNIX) && !defined(OPENSSL_RAND_SEED_EGD) && !defined(OPENSSL_SYS_AMIGAOS3) && !defined(OPENSSL_SYS_AMIGAOS4)
     ADD_ALL_TESTS(test_rand_fork_safety, RANDOM_SIZE);
 #endif
     ADD_TEST(test_rand_prediction_resistance);
