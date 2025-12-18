/* $Id: presentation.c,v 1.4 2006/02/13 23:30:53 bercegeay Exp $ */

/**@file*/

/*! @page pagepres Global presentation of windom


@par For the programmer...

Windom is a C library to make GEM programming very easy.
With the help of windom, you can focus on programming the real job
of your application, and let windom handle complex GEM stuff
(toolbar, forms, menu in windows...). If GEM lacks some features,
then windom will try to emulate them the best it can (for example
iconification of windows).

@par For the end-user...

Nothing has to be installed. Windom is a static C library, so you just
have to provide your application (linked against windom and all other
requiered libraries), and that's all.

Nevertheless, the user may customize the application thanks to a 
configuration file or userdef libraries (skin for forms). See
\ref Configfile for details.

@par Compatibility

Windom applications should work on any "atari"
machine and clones (falcon, hades, milan, aranym...) and with
any GEM version (all TOS Atari version, Milan TOS, MagiC,
MagicMac, MultiTOS, N.AES, MyAES, XaAES...).

@par Programming windom application

At the moment, windom is available for gcc (32 bit ints), but it might be
compiled for the following compilers too:
- Pure C
- SozobonX C (with 16 bits int size)
- GCC 2.81 (with 32 bits int size) and its binutils

Windom requiers the following libraries to be installed (all of them are
available for the same compilers) :
- gemlib 0.43.5 (or newer). See http://arnaud.bercegeay.free.fr/gemlib
- ldg-dev 2.33 (or newer). See http://ldg.sf.net

The general idea of windom is to have an API similar to standard AES
programming. As consequence, windom functions are very similar to their
GEM homolog (but with extended actions). For example, using windom, we
never call the AES function appl_init(), but its windom equivalent 
ApplInit().

@par Extensions

Windom extensions are just standard libraries, but based on windom functions.
There's nothing magical in the design of "windom extensions".

There is for example the DFRM library, to dynamically build forms at runtime,
the GSlib library to automatise the handle of GEMScript protocol, or
woutlib to manage a window that contains text (kind of stdout output in window).

Visit the windom site for a list of available extensions.

*/

