/*asmhelp.h - output an help file given a module defn*/
#ifndef asmhelp_H
#define asmhelp_H

#include <stdio.h>

#ifndef os_H
   #include "oslib/os.h"
#endif

/* from support */
#ifndef lookup_H
   #include "lookup.h"
#endif

extern os_error *asmhelp_output (FILE *, char *title, char *author,
      lookup_t, lookup_t, lookup_t, lookup_t, lookup_t);

#endif
