--- Configure.old	2010-12-29 02:43:14.000000000 +0000
+++ Configure	2010-12-29 02:45:29.000000000 +0000
@@ -440,6 +440,13 @@
 "beos-x86-r5",   "gcc:-DL_ENDIAN -DTERMIOS -O3 -fomit-frame-pointer -mcpu=pentium -Wall::-D_REENTRANT:BEOS:-lbe -lnet:BN_LLONG ${x86_gcc_des} ${x86_gcc_opts}:${x86_elf_asm}:beos:beos-shared:-fPIC -DPIC:-shared:.so",
 "beos-x86-bone", "gcc:-DL_ENDIAN -DTERMIOS -O3 -fomit-frame-pointer -mcpu=pentium -Wall::-D_REENTRANT:BEOS:-lbe -lbind -lsocket:BN_LLONG ${x86_gcc_des} ${x86_gcc_opts}:${x86_elf_asm}:beos:beos-shared:-fPIC:-shared:.so",
 
+"arm-unknown-riscos","arm-unknown-riscos-gcc:-DL_ENDIAN -DTERMIOS -DNO_SYS_UN_H -O3 -fomit-frame-pointer -Wall::-D_REENTRANT:::BN_LLONG RC4_CHAR RC4_CHUNK DES_INT DES_UNROLL BF_PTR:${armv4_asm}:::::echo",
+"m68k-atari-mint","m68k-atari-mint-gcc:-DB_ENDIAN -DTERMIOS -DNO_SYSLOG -DNO_SYS_UN_H -DOPENSSL_USE_IPV6=0 -O3 -fomit-frame-pointer -Wall::-D_REENTRANT:::BN_LLONG DES_RISC1::::::::::::::::::::m68k-atari-mint-ranlib",
+"m5475-atari-mint","m5475-atari-mint-gcc:-DB_ENDIAN -DTERMIOS -DNO_SYSLOG -DNO_SYS_UN_H -DOPENSSL_USE_IPV6=0 -O3 -fomit-frame-pointer -Wall::-D_REENTRANT:::BN_LLONG DES_RISC1::::::::::::::::::::m5475-atari-mint-ranlib",
+"m68k-unknown-amigaos","m68k-unknown-amigaos-gcc:-DB_ENDIAN -DOPENSSL_SYS_AMIGAOS3 -DNO_SYSLOG -DHAVE_FORK=0 -DNO_SYS_PARAM_H -DNO_SYS_UN_H -O3 -fomit-frame-pointer -Wall::-D_REENTRANT::-lnet -lm:BN_LLONG DES_RISC1::::::::::::::::::::m68k-unknown-amigaos-ranlib",
+"ppc-amigaos","ppc-amigaos-gcc:-DB_ENDIAN -DOPENSSL_SYS_AMIGAOS4 -DNO_SYSLOG -DHAVE_FORK=0 -DNO_SYS_PARAM_H -DNO_SYS_UN_H -O3 -fomit-frame-pointer -Wall::-D_REENTRANT:::BN_LLONG DES_RISC1::::::::::::::::::::ppc-amigaos-ranlib",
+"i686-w64-mingw32", "gcc:-DL_ENDIAN -DWIN32_LEAN_AND_MEAN -fomit-frame-pointer -O3 -march=i486 -Wall::-D_MT:MINGW32:-lws2_32 -lgdi32 -lcrypt32:BN_LLONG ${x86_gcc_des} ${x86_gcc_opts} EXPORT_VAR_AS_FN:${x86_asm}:coff:win32:cygwin-shared:-D_WINDLL -DOPENSSL_USE_APPLINK:-mno-cygwin:.dll.a",
+
 #### SCO/Caldera targets.
 #
 # Originally we had like unixware-*, unixware-*-pentium, unixware-*-p6, etc.
@@ -1123,6 +1130,7 @@
 my $IsMK1MF=scalar grep /^$target$/,@MK1MF_Builds;
 
 $exe_ext=".exe" if ($target eq "Cygwin" || $target eq "DJGPP" || $target =~ /^mingw/);
+$exe_ext=",e1f" if ($target eq "arm-unknown-riscos");
 $exe_ext=".nlm" if ($target =~ /netware/);
 $exe_ext=".pm"  if ($target =~ /vos/);
 $openssldir="/usr/local/ssl" if ($openssldir eq "" and $prefix eq "");
