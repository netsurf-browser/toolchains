/* Simple stub kernel.h header file required by defmod builds */

#ifndef kernel_H
#define kernel_H

typedef struct {
  int errnum;
  char errmess[252];
} _kernel_oserror;

// return the last os error since
//	(a) the last call
//	(b) the program was started
//	(c) NULL if none
extern _kernel_oserror *_kernel_last_oserror(void);

# endif

