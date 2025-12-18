#ifndef vapi_h_H
#define vapi_h_H

/*vapi_h.h - output a c header file for use in a vapi-file*/

#include <stdio.h>

#ifndef os_H
   #include "oslib/os.h"
#endif

/* from support */
#ifndef lookup_H
   #include "lookup.h"
#endif

extern os_error *vapi_h_output (FILE *, lookup_t);

#endif

