/* tmpfile.h
   this file provides a subset of the UnixLib tempfile functions, for use
   with stdio for the Shared C library

   The code is borrowed from the UnixLib source code, in order
   not to have to make the OSLib distribution dependent upon UnixLib.
*/

#if !defined TMPFILE_H
#define TMPFILE_H


char *
mktemp (char *file_template);

#endif
