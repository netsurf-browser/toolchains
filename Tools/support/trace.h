#ifndef trace_H
#define trace_H

/*tracef.h*/

/*OSLib---efficient, type-safe, transparent, extensible,\n"
   register-safe API coverage of RISC OS*/
/*Copyright (c) 1994 Jonathan Coxhead*/

/*
      OSLib is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

      OSLib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
   along with this programme; if not, write to the Free Software
   Foundation, Inc, 675 Mass Ave, Cambridge, MA 02139, U S A.
*/

#ifndef TRACE
#define TRACE 0
#endif

#ifndef os_H
   #include "oslib/os.h"
#endif

#if TRACE
   #define tracef(args)    trace_f (__FILE__, __LINE__, args)
   #define tracevdu(s, n)  trace_vdu (s, n)
   #define tracewait(t)    trace_wait (__FILE__, __LINE__, t)
   #define trace_FINISH \
      {  trace_file = __FILE__; \
         trace_line = __LINE__; \
         goto finish; \
      }
   #define trace_ASSIGN(e, f) \
      {  if (((e) = (f)) != NULL) \
         {  trace_file = __FILE__; \
            trace_line = __LINE__; \
      }  }
   #define trace_RETURN(e) \
      {  if ((e) != NULL) \
            trace_f (trace_file, trace_line, "<%s>\n", \
                  (e)->errmess); \
         return e; \
      }
   #define trace_ERROR(e) \
      {  if ((e) != NULL) \
            trace_f (trace_file, trace_line, "<%s>\n", \
                  (e)->errmess); \
      }

   extern os_error *trace_initialise (char *var);
   extern os_error *trace_terminate (void);
   extern void trace_f (char *file, int line, char *, ...);
   extern void trace_vdu (char *, int);
   extern void trace_wait (char *file, int line, int t);

   extern char *trace_file;
   extern int  trace_line;
#else
   #define tracef(args)
   #define tracevdu(args)
   #define tracewait(args)

   #define trace_initialise(var)
   #define trace_terminate()
   #define trace_f(args)
   #define trace_vdu(s, n)
   #define trace_wait(t)        
   #define trace_FINISH          {goto finish;}
   #define trace_ASSIGN(e,f)     {(e) = (f);}
   #define trace_RETURN(e)       {return e;}
   #define trace_ERROR(e)        ;

#endif
#endif
