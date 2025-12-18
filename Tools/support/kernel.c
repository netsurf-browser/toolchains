/* kernel.c - emulation of some useful kernel functions*/

/*OSLib---efficient, type-safe, transparent, extensible,\n"
   register-safe API coverage of RISC OS*/
/*Copyright (c) 1994 Jonathan Coxhead*/

/*
      OSLib is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

      OSLib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
   along with this programme; if not, write to the Free Software
   Foundation, Inc, 675 Mass Ave, Cambridge, MA 02139, U S A.
*/

#include <errno.h>
#include <string.h>

#include "kernel.h"
#include "support/trace.h"
#include "support/riscos.h"

_kernel_oserror *_kernel_last_error;

/* ---------------------------------------------------------------------- */
// return the last os error since
//	(a) the last call
//	(b) the program was started
//	(c) NULL if none
/* we have to mess about, because RISC OS returns NULL when calling
** _kernel_last_oserror the second time for the same error.
*/
_kernel_oserror *_kernel_last_oserror(void)
{
	_kernel_oserror *err = _kernel_last_error;
	_kernel_last_error = 0;

	return err;
}

/* ---------------------------------------------------------------------- */
