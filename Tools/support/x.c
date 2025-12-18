/****************************************************************
** Title  :   	Tools.support.c.x
**
** Purpose:   	RISC OS Exception Handling
**
** Copyright:   (c) OSLib
**
** History:     970913	TV  Port from OSLib x with StrongArm amendments
**              981221  TV  fixed bug giving wrong offsets in International_Error
**
*****************************************************************/
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

/*x.c*/

/*From CLib*/
#include <kernel.h>
#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

/*From OSLib*/
#include "oslib/macros.h"
#include "oslib/messagetrans.h"
#include "oslib/os.h"

/*From Support*/
#include "x.h"
#include "trace.h"

/* For bootstrap builds */
#ifndef error_ESCAPE
#  define error_ESCAPE 0x11u
#endif
#ifndef error_STK_OFLO
#  define error_STK_OFLO 0x163u
#endif

 typedef struct
 {
   os_error* dflt;
   os_error* token;
 }
 International_Error;

/*A macro to declare them*/
#define LOCAL_INTERNATIONAL_ERROR(id, token, num, mess) \
  static struct \
  { \
    bits defaultnum; \
    char errmess [sizeof mess + 1]; \
  } id ## _dflt_ = { num, mess }; \
\
  static struct \
  { \
    bits errnum; \
    char errtoken [sizeof token + 1]; \
  } id ## _token_ = {num, token}; \
\
  static International_Error id ## _ = { ( os_error *) &id ## _dflt_ , (os_error*) &id ## _token_ }; \
  static International_Error *id = &id ## _

#ifdef SIGSTAK
/* international error blocks (for use with Copy_Error())*/
LOCAL_INTERNATIONAL_ERROR(  Error_Stack,
      	      	      	    "C64",
      	      	      	    error_STK_OFLO,
      	      	      	    "Not enough memory, allocate failed" );
#endif

LOCAL_INTERNATIONAL_ERROR(  Error_Unknown,
      	      	      	    "C35",
      	      	      	    1,
      	      	      	    "No error (errno = 0)" );

/* synchronisation error */
x_LOCAL_ERROR( Error_Sync, 1 + (1U<<30), "Try/Catch Synchronisation Error" );

/* exception stack overflow */
x_LOCAL_ERROR( Error_XStack, 2 + (1U<<30), "Too many nested x_TRY blocks" );


/* error blocks with tokens in (for use with MessageTrans).*/
x_LOCAL_ERROR( Error_Escape, error_ESCAPE, "Escape" );
x_LOCAL_ERROR( Error_No_Memory, os_GLOBAL_NO_MEM, "NoMem" );


/* we assume SIGINT is always defined. */
static const int Signals [] = {
#ifdef SIGSTAK
  SIGSTAK,
#endif
#ifdef SIGOSERROR
  SIGOSERROR,
#endif
  SIGINT
  };


/*------------------------------------------------------------------------*/
static os_error *Copy_Error (International_Error *ie)
{
#ifndef EXECUTE_ON_UNIX
  messagetrans_control_block cb;
#endif

  os_error *e;

   /*This code is what the SharedCLibrary does, except that this happens
      every time a message is to be looked up, and the module only opens
      the file at most once. We don't really care about this.*/

#ifndef EXECUTE_ON_UNIX
  if( xmessagetrans_open_file( &cb, "SharedCLibrary:Messages", NULL ) == NULL )
  {
    e = xmessagetrans_error_lookup( ie -> token,
      	      	      	      	    &cb,
      	      	      	            NULL,
      	      	      	            0,
      	      	      	            "SharedCLibrary",
      	      	      	            NULL, NULL, NULL );
      xmessagetrans_close_file( &cb );
   }
   else
#endif
      e =  ie -> dflt;

   return e;
}


// start off with a stack chunk of adequate size to hold initial exception blocks
static x_exception*   aX[ x_MAXDEPTH ];  // stack
x_exception**  pX = aX;	    // stack pointer

/*------------------------------------------------------------------------*/

// signal handler
static void SigHandler( int Signal )
{
  os_error *Error = NULL /**/;

  tracef("Signal %d\n" _ Signal);

  /*Save this condition in the buffer.*/
  switch( Signal )
  {
    case SIGINT:
#ifdef EXECUTE_ON_UNIX
      Error = Error_Escape;
#else
      Error = xmessagetrans_error_lookup( Error_Escape, NULL, NULL, 0, \
      						SKIP, SKIP, SKIP, SKIP );
#endif
      break;

#ifdef SIGSTAK
    case SIGSTAK:
      Error = Copy_Error( Error_Stack );
      break;
#endif

#ifdef SIGOSERROR
    case SIGOSERROR:
      Error = x__last_error();
      break;
#endif
   }

#if  !defined (EXECUTE_ON_UNIX)
   /*Longjump back to the x_TRY() call.*/
   longjmp( (*(pX - 1)) -> buf, (int)( (*(pX - 1)) -> error = Error));
#else
   /*Longjump back to the x_TRY() call.*/
   siglongjmp( (*(pX - 1)) -> buf, (int)( (*(pX - 1)) -> error = Error));
#endif
}


/*------------------------------------------------------------------------*/
void x__try( x_exception* X )
{
  int sig;

  // enter on exception stack
  if( pX < aX + x_MAXDEPTH )
    *pX++ = X;
  else
    x_THROW( Error_XStack ); // throw an error on exception stack overflow


  /* Register handlers for each of the signal types which we're prepared to handle
  ** We maintain a record of the previous handlers so that we can restore them.
  ** It is more efficient to place the pointers on the caller's stack than to
  ** allocate our own memory */
  for( sig = 0; sig < COUNT (Signals); sig++ )
  {
    X -> previous[sig] = signal( Signals[sig], SigHandler );

    if( X -> previous[sig] == SIG_ERR )
      X -> previous[sig] = NULL;
  }

   /*error pointer*/
   X -> error = NULL;
}


/*------------------------------------------------------------------------*/
void x__catch( x_exception* X )
{
  int sig;
  void (*prev)(int);

  /*Restore the caller's handlers.*/
  for( sig = 0; sig < COUNT (Signals); sig++ )
  {
    //if( X -> previous[sig] != NULL )
      prev = signal( Signals[sig], X -> previous[sig] );

    tracef( "x__catch (%d): restoring signal %d: from %p to %p\n" \
    		_ pX - aX \
    		_ Signals[sig] \
		_ prev \
		_ X -> previous[sig]
		);
  }

  /* remove the x_exception entry from the exception stack */
  if( X == *(--pX) )
    *pX = NULL;
  else
    x_THROW( Error_Sync );  // try/catch blocks out of sync
}

/*------------------------------------------------------------------------*/
os_error *x__last_error (void)
{
  os_error *last_error;
  static os_error *Last_Error;

   /*Messing about here because _kernel_last_oserror() returns NULL the
      second time it's called*/

  if ((last_error = (os_error *) _kernel_last_oserror() ) != NULL )
    Last_Error = last_error;

  if (Last_Error == NULL)
    Last_Error = Copy_Error (Error_Unknown);

#if 0	  // unsafe to call *printf from sig handler
  tracef( "Last Error: 0x%x; \"%s\"\n" _ Last_Error -> errnum _ Last_Error -> errmess );
#endif

  return Last_Error; /*never NULL*/
}


/*------------------------------------------------------------------------*/
void *x__alloc (int size)
{
  void *ptr;

   if ((ptr = malloc (size)) == NULL && size != 0)
   {
#ifndef EXECUTE_ON_UNIX
      messagetrans_error_lookup (Error_No_Memory, NULL, NULL, 0, SKIP,
            SKIP, SKIP, SKIP);
#else
	os_generate_error( Error_No_Memory );
#endif
   }
   return ptr;
}


/*------------------------------------------------------------------------*/
void *x__calloc (int count, int size)
{
  void *ptr;

  if ((ptr = calloc (count, size)) == NULL && size != 0 && count != 0)
  {
#ifndef EXECUTE_ON_UNIX
      messagetrans_error_lookup (Error_No_Memory, NULL, NULL, 0, SKIP,
            SKIP, SKIP, SKIP);
#else
	os_generate_error( Error_No_Memory );
#endif
  }

   return ptr;
}


/*------------------------------------------------------------------------*/
void x__free (void *ptr, int size) {NOT_USED (size) free (ptr);}


/*------------------------------------------------------------------------*/
void *x__realloc (void *ptr, int old_size, int size)
{
//  NOT_USED (old_size)
//  NOT_USED (size)

  if ((ptr = realloc (ptr, size)) == NULL && size != 0)
  {
#ifndef EXECUTE_ON_UNIX
      messagetrans_error_lookup (Error_No_Memory, NULL, NULL, 0, SKIP,
            SKIP, SKIP, SKIP);
#else
	os_generate_error( Error_No_Memory );
#endif
  }

   return ptr;
}
