/* $Id: usage.c,v 1.2 2006/02/13 23:30:53 bercegeay Exp $ */

/**@file*/

/** @page pageusage Usage of windom

WinDom requires two external libraries:
- GEMLIB 0.43.5 or newer, 
- and LDG-DEV 2.33 or newer.

In your makefile, you have to add these two libraries.

Exemple for makefile:
@code LDFLAGS = -lwindom -lgem -lldg @endcode

In your source code, you just have to include the windom header :
@code #include <windom.h> @endcode

Please note that gemlib headers are automatically included by the windom
header files, so you don't have to explicitly include gem.h or gemx.h.


Note for other compilers:

for GCC2.81, sozobon and purec, gemlib 0.43.6+ is required (a function added in
gemlib 0.43.5 is missing for other compilers than gcc -- this has been fixed in
gemlib 0.43.6).

example for pure c project with LDG and GEMLIB libraries:
@code
	;; Pure C projet file (standard)
	windom.lib
	gem.lib
	ldg.lib
	pcstdlib.lib
	pctoslib.lib
@endcode

link libc instead of ilibc (===> sozobonx) (TODO)

stack size set to 32k
- example for sozobon (TODO)
- example for purec (TODO)


*/
