/****************************************************************
** Title  :   	SupportLib.Choices.h.Choices
**
** Purpose:   	Give standard interface to application choices file
**
** Copyright:   (c) 1997 Control Computer Consultants
**
** History:     970820  created
**
*****************************************************************/
/*
      OSLibSupport is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version - and this with the following clarification and
   special exception:

      Linking this library statically or dynamically with other modules
   is making a combined work based on this library. Thus, the terms
   and conditions of the GNU General Public License cover the whole
   combination.

      As a special exception, the copyright holders of this library
   give you permission to link this library with independent modules
   to produce an executable, regardless of the license terms of these
   independent modules, and to copy and distribute the resulting
   executable under terms of your choice, provided that you also meet,
   for each linked independent module, the terms and conditions of the
   license of that module. An independent module is a module which is
   not derived from or based on this library. If you modify this
   library, you may extend this exception to your version of the library,
   but you are not obliged to do so. If you do not wish to do so, delete
   this exception statement from your version.

      OSLibSupport is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
   along with this programme; if not, write to the Free Software
   Foundation, Inc, 675 Mass Ave, Cambridge, MA 02139, U S A.
*/

/****************************************************************
**  The application choices file is a file specified by system variable
**  <AppName>$Choices, where <AppName> is the name of the of the application
**  as passed to toolbox_initialize.
**
**  The format of the application choices file consists of one line per choice,
**  prefixed by a tag which is seperated from the data by a colon.
**  Multiple instances of a tag are allowed, but only the first occurance will be returned.
**
**  Any line starting with a # is regarded as a comment and will be ignored.
**
****************************************************************/
#if !defined Choices_H
#define Choices_H

#include "oslib/types.h"

/****************************************************************
** Definitions
****************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************
** Global Interface
****************************************************************/

//  Returns TRUE if zTag exists in choices file
extern osbool Choices_Exists( const char* zTag );

//  Reads the string associated with zTag from the application choices file
//  Returns:  string length
//  Outputs:  if acString != NULL: at most iBufSiz chars into acString.
//    	      else just returns string length (required buffer size - 1).
//    	      Outputs zDefault if tag not found
extern int Choices_GetString( const char* zTag,
                              char* acString,
                              int iBufSize,
                              const char* zDefault
                            );

// Reads the integer associated with zTag from the application choices file
//  Returns:  integer value
//  Returns iDefault if tag not found
extern int Choices_GetInt( const char* zTag, int iDefault );

// set the string at zTag to zString
// if zTag is not found, then it is created at end of file
// throws exception on file error
extern void Choices_SetString( const char* zTag, const char* zString );

// set the int at zTag to iInt
// if zTag is not found, then it is created at end of file
// throws exception on file error
extern void Choices_SetInt( const char* zTag, int iInt );

#ifdef __cplusplus
}
#endif

#endif
/*** END ***/
