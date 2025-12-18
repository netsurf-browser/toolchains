/*hdr.h - output an assembler header file given a module defn*/
#ifndef hdr_H
#define hdr_H

#include <stdio.h>

#ifndef os_H
   #include "oslib/os.h"
#endif

/* from support */
#ifndef lookup_H
   #include "lookup.h"
#endif

extern os_error *hdr_output (FILE *, const char *, const char *, lookup_t, lookup_t,
      lookup_t, lookup_t, lookup_t);

#endif
