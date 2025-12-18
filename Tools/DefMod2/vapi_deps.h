#ifndef vapi_deps_H
#define vapi_deps_H

/*vapi_deps.h - output a deps file for dependencies for the different vapi-file*/

#include <stdio.h>

#ifndef os_H
   #include "oslib/os.h"
#endif

/* from support */
#ifndef lookup_H
   #include "lookup.h"
#endif

extern os_error *vapi_deps_output (FILE *, lookup_t);

#endif

