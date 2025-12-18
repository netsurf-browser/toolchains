/* Pull in the sources from Tools/support we need here.  */
#include "kernel.c"
#include "lookup.c"
#include "x.c"
#ifdef EXECUTE_ON_UNIX
#  include "oslib/unix/os.c"
#  include "oslib/unix/osfile.c"
#  include "oslib/unix/osfind.c"
#  include "unix.c"
#endif
