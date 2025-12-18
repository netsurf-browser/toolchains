/*chelp.h - output an chelp file given a module defn*/
#ifndef chelp_H
#define chelp_H

#include <stdio.h>

#ifndef os_H
   #include "oslib/os.h"
#endif

/* from support */
#ifndef lookup_H
   #include "lookup.h"
#endif

extern os_error *chelp_output (FILE *, char *title, char *author,
      lookup_t, lookup_t, lookup_t, lookup_t, lookup_t);

#endif
