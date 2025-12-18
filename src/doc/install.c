/* $Id: install.c,v 1.2 2006/02/13 23:30:53 bercegeay Exp $ */

/**@file*/


/** @page pageinstall Installation of windom (and how to rebuild it) 

\section firstuse Installation of windom

To get windom installed, you have to copy the headers files and the static
library :
- copy the content of
  the windom-distrib/include directory (including its windom subdirectory) with
  the headers files of your compiler (/usr/include, or
  /usr/m68k-atari-mint/include for example). 
- Then you should copy the library itself windom/lib/gcc/libwindom.a
  in the lib directory of your complier (/usr/lib for example).

If you use the Sparemint environment, then just install the RPM package.

\section Compiling Compiling

If you want to rebuild windom, it's higly recommanded to setup a complete
sparemint environment because the building process needs a bash shell,
gmake, sed, doxygen (for the documentation)... and a filesystem with long
filenames.

You can also build the gcc version of windom by using a cross compiler under
linux for example.

TODO (to be completed)

*/
