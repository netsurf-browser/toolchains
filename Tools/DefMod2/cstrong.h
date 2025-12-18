#ifndef cstrong_H
#define cstrong_H

/*cstrong.h - output an cstrong file given a module defn*/

#include <stdio.h>

#ifndef os_H
   #include "oslib/os.h"
#endif

/* from support */
#ifndef lookup_H
   #include "lookup.h"
#endif

extern os_error *cstrong_output (char *, char *title, char *author,
      lookup_t, lookup_t, lookup_t, lookup_t, lookup_t);

#endif
