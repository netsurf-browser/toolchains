#!/bin/bash

set -x
COMPILER=x86_64-linux-kos32-4.8.5.7z
SDK=sdk-28-10-16.7z

SVN_URL=svn://kolibrios.org
SDK_URL=http://ftp.kolibrios.org/users/Serge/new/Toolchain/$SDK
COMPILER_URL=http://ftp.kolibrios.org/users/Serge/new/Toolchain/$COMPILER

INSTALL_PATH=/opt/netsurf/kos32-gcc
CROSS_PATH=$INSTALL_PATH/cross/autobuild/tools/
ENV_PATH=$INSTALL_PATH/env
OBJ_PATH=$ENV_PATH/obj

SVN_CHECKOUT=$ENV_PATH/kolibrios

SHA512_SDK=e37226c1421104f9025c644277080a0adad1cb5db560fe04bbcc6c41797fe87e57d337da314bde4abc4decfbedf3087ffb16b1d71d2587e317933204255ead1a
SHA512_COMPILER=28555b2247848c5d85be6b18e10dc903b9a161179230b65d35f7f3bf40dff3b135972e84f6c25db23fee2a90c7a96b5a755dba9a2eaa9dbc4735e717436db1ce

# clean
# rm -rf $INSTALL_PATH

# set up directories
for dir in $INSTALL_PATH $CROSS_PATH $ENV_PATH $OBJ_PATH; do
	mkdir -p $dir
done

# download sdk and friends
svn checkout ${SVN_URL} ${SVN_CHECKOUT}
wget -v ${COMPILER_URL} --directory-prefix=${CROSS_PATH}
wget -v ${SDK_URL} --directory-prefix=${CROSS_PATH}

# verify checksums
for file in COMPILER SDK; do
    VARNAME=SHA512_${file}
	[[ $(sha512sum $CROSS_PATH/${!file} | awk '{print $1}') != ${!VARNAME} ]] && exit 1
done

# set up toolchain
pushd $PWD

cd $CROSS_PATH
7z x $COMPILER

mv $SDK win32/

cd win32
7z x $SDK

popd

# fix up environment
PATH=${PATH}:/opt/netsurf/kos32-gcc/cross/autobuild/tools/win32/bin
GCC="kos32-gcc -c -fno-ident -fomit-frame-pointer -U__WIN32__ -I${SVN_CHECKOUT}/contrib/sdk/sources/newlib/libc/include -U_Win32 -U_WIN32 -U__MINGW32__ -UWIN32 -U__BSD_VISIBLE"
ASM=fasm

# build stuff
$GCC ${SVN_CHECKOUT}/programs/fs/unzip60/kolibri/dirent.c -o ${OBJ_PATH}/dirent.o
$GCC ${SVN_CHECKOUT}/programs/develop/libraries/iconv/iconv.c -o ${OBJ_PATH}/iconv.o
$ASM ${SVN_CHECKOUT}/contrib/C_Layer/ASM/loadhttp.asm ${OBJ_PATH}/loadhttp.obj



