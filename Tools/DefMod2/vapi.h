#ifndef vapi_H
#define vapi_H

/*vapi.h - output an vapi file given a module defn*/

#include <stdio.h>

#ifndef os_H
   #include "oslib/os.h"
#endif

/* from support */
#ifndef lookup_H
   #include "lookup.h"
#endif

extern os_error *vapi_output (FILE *, char *title, char *author,
      lookup_t, lookup_t, lookup_t, lookup_t, lookup_t);

#endif
