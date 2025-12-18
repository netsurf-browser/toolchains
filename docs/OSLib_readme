Introduction
------------
  OSLib is a set of functions  and C headers to provide complete coverage  of
the RISC OS application programmer's interface in C. It provides access from
C code to all RISC OS system calls ("SWI's") which is

      efficient: often, memory access is completely avoided;

      type-safe: every argument can be type-checked by the compiler;

      obvious: a SWI is called by the "obvious" syntax;

      complete: every SWI is covered;

      register-safe: hides (often idiosyncratic) register allocation;

      language-independent: although the  headers are specific to C,  the
      library is not - any APCS-32 conformant language can call it.

  It also provides names for all the data structures and reason codes used by
the API. Code that uses it is superior  to similar code using _kernel_swi()
or _swix(), both in terms of the compile-time checking that is available, and
the size and speed of the code generated.

  OSLib  provides a  very convenient  interface to  the RISC  OS programmer,
since all the facilities of the  C compiler are available to catch errors and
generate good  code. It is conceptually very small,  in that it is completely
documented  by this file. As  a bonus, code  written using it  is smaller and
faster than code written using other means.

  OSLib is copyright (c) 1994-2012 Jonathan Coxhead and OSLib maintainers.  It
is distributed in the hope that it will be useful, but  without any warranty;
without  even the  implied  warranty  of  merchantability or  fitness  for  a
particular purpose.

  OSLib is released under the GNU  public licence - for details see the  file
"COPYING" included with  this release. The  copyright holders have granted  a
relaxation of the conditions of this licence to allow its use in constructing
proprietary  software.  This means  that  it  is  free software  itself,  but
applications linked to it need not be.  It follows that any changes to  OSLib
itself (the contents of the directories Tools, Source, and OSLibSupport) fall
under the terms of the GNU Public Licence; but programmes written using OSLib
do not.

  Fault  reports  and  suggestions  for  improvement  may  be  sent   to  the
maintainers, mailto:<oslib-user@compton.nu>.

A number of mail lists have been set up to allow discussion of topics
concerned with OSLib. They are:

      oslib-user -- for comments from users, as opposed to developers of the
      library. Subscribing and posting are both open.
      mailto:<oslib-user-subscribe@compton.nu>

      oslib-team -- for discussions amongst the developer 'team'. Subscribing
      and posting are both moderated.
      mailto:<oslib-team-subscribe@compton.nu>

      oslib-announce -- announcements about OSLib. Subscribing is open;
      posting moderated.
      mailto:<oslib-news-subscribe@compton.nu>

Downloading
-----------
Make sure you have downloaded the appropriate archive for your build system,
there are different archives depending on when you're using AOF or ELF object
types, and whether you're building using soft-float, hard-float (application
use) or hard-float (module use).

The OSLib website http://ro-oslib.sourceforge.net contains more information on
this topic.

Installation
------------

The file you are presently reading is "OSLib_readme", and contains the basic
introduction to OSLib, and how to install it.

Make sure you read the "COPYING" text so that you know what you may and may
not legally do.

The "ChangeLog" text contains details on how OSLib has evolved over the
years, and will be useful if you're upgrading, and and you find its
behaviour to be subtly different.

The "OSLib_API" file describes the relationships between OSLib names, and
the associated PRM descriptions.

Finally, the OSLib directory is what it's all about; you should copy this to
a suitable place on your hard disc; probably in the same place as your other
libraries. Don't put it too low down in the directory structure; if you do
the path name will become too long for the system to resolve it. Two or three
levels down should be OK. It is possible to use OSLib within the archive, but
you will find it will significantly slow down the compilation process.

Using OSLib
-----------
At the top level in the OSLib directory you will find an Obey file named
"SetVars". When this is run, the OSLib: path is set up. If you want to change
it, that's fine, but the default is really all you need. Make sure SetVars is
executed before you attempt to build anything using the library.

Compiling
---------
In any source file from which you want to call an OSLib veneer, you will
have to specify where it is to be found with a line of the type
#include "oslib/wimp.h".  In this example, the "oslib/" prefix ensures that
"wimp.h"  is  loaded from oslib, as opposed to any  other library. When
compiling, you need to ensure that the compiler can locate OSLib, by
specifying "-IOSLib:" on the command line.

A typical Norcroft C compiler command line for a C module using OSLib and 
OSLibSupport will be similar to (remove "OSLibSupport:," if you don't want
OSLibSupport):

    cc -c -afh -apcs 3/32 -IOSLibSupport:,OSLib:,C: -o foo.o foo.c

For GCC (both AOF and ELF) the equivalent command would be (remove
"-IOSLibSupport:" if you don't want OSLibSupport):

    gcc -c -IOSLibSupport: -IOSLib: -o foo.o foo.c

For assembling with ObjAsm (remove "<OSLibSupport$Dir>," if you don't want
OSLibSupport):

    objasm -I<OSLibSupport$Dir>,<OSLib$Dir> -o foo.o -s foo.s

For assembling with AsAsm (remove "<OSLibSupport$Dir>," if you don't want
OSLibSupport):

    asasm -I<OSLibSupport$Dir>,<OSLib$Dir> -o foo.o -s foo.s

For assembling with GCCSDK GCC 3.4.x (AOF based, remove "-IOSLibSupport:" if
you don't want OSLibSupport):

    gcc -c -IOSLibSupport: -IOSLib: -xassembler-with-cpp -mapcs-32 -Wa,-objasm,-apcs32 -o foo.o foo.s

For assembling with GCCSDK GCC 4.x (ELF based, remove "-IOSLibSupport:" if you
don't want OSLibSupport):

    gcc -c -IOSLibSupport: -IOSLib: -xassembler-with-cpp -o foo.o foo.s

Please refer to your compiler manual for details on the use of the command line.

Linking
-------
The only OSLib-specific precaution required for the linking stage is to include 
OSLib (and OSLibSupport, if required) on the command line.

A typical Norcroft Link command would be (remove "OSLibSupport:OSLibSupport32.o"
if you don't want OSLibSupport):

   link foo1.o foo2.o ... OSLibSupport:OSLibSupport32.o OSLib:OSLib32.o C:stubs.o

For GCCSDK GCC 3.4.x (AOF based, remove "OSLibSupport:OSLibSupport32.o" if
you don't want OSLibSupport):

   gcc -o final foo1.o foo2.o ... OSLibSupport:OSLibSupport32.o OSLib:OSLib32.o

For GCCSDK GCC 4.x (ELF based, remove "-LOSLibSupport: -lOSLibSupport32" if you
don't want OSLibSupport):

   gcc -o final foo1.o foo2.o ... -LOSLibSupport: -lOSLibSupport32 -LOSLib: -lOSLib32

As the ELF object format records which ABI settings (like softfloat vs hardfloat)
are used to build the object files and that only compatible object files can
successfully be linked, it is important to choose the right OSLib library
version for linking and this is depending on the runtime library and/or
compiler options used.  E.g.:

  gcc -mlibscl => SharedCLibrary to build applications => hardfloat fpa
  gcc -mmodule => SharedCLibrary to build modules => hardfloat fpa module
  gcc -munixlib (default) => UnixLib => softfloat

Again, this is inherent in the tool usage, there is nothing special about 
its use with OSLib.

Help is at hand
---------------

If, having read, and re-read this documentation, and the related FAQ on the
web site, you still have problems, then subscribe to the OSLib-User mail
list, as described on the web site, and ask your question there.

The OSLib web-site is at http://ro-oslib.sourceforge.net.

--------------------
OSLib Developers.
