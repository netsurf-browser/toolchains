/****************************************************************
** Title  :   	SupportLib.message.h
**
** Purpose:   	interface to MessageTrans
**
** Copyright:   (c) 1997 Control Computer Consultants
**
** History:     971018  created
**              981227  various calls changed to remove dependance on toolbox
**
*****************************************************************/
#if !defined Message_H
  #define Message_H

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
#include "oslib/types.h"
#include "oslib/os.h"
#include "oslib/messagetrans.h"

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************
** look up the message associated with zToken in the Messages file.
**  return pointer to message if found;
**  else return pointer to zDefault
**  981227  extra argument added: messagetrans_control_block* pxBlock,
****************************************************************/
const char* Message_GetMessage( messagetrans_control_block* pxBlock, const char* zToken, const char* zDefault );


/****************************************************************
**  Return a pointer to an error block which contains the error message text
**  associated with the token passed in pxError
**  returns pxError if unable to find messages file
**  981227  extra argument added: messagetrans_control_block* pxBlock,
****************************************************************/
const os_error* Message_GetError(  messagetrans_control_block* pxBlock, const os_error* pxError );


/****************************************************************
**  Return a pointer to an error block which contains the error message text
**  associated with the token passed in zToken; or from zDefault if token not found
****************************************************************/
const os_error* Message_MakeError(  int iErrNum,
                                    messagetrans_control_block* pxBlock,
                                    const char* zToken,
                                    const char* zDefault, ... );


/****************************************************************
**  Display a message in a wimp box with OK button
**  and info sprite (RO3.5+)
**  Does not return until OK is clicked
**  981227  extra arguments added: messagetrans_control_block* pxBlock,
**                               const char* zAppName
****************************************************************/
void Message_ShowMessage( messagetrans_control_block* pxBlock,
      	      	      	  const char* zAppName,
      	      	      	  const char* zToken,
      	      	      	  const char* zDefault, ... );


/****************************************************************
**  Display an error message in a wimp box with OK button
**  and Caution sprite (RO3.5+)
**  Does not return until OK is clicked
**  981227  extra arguments added: messagetrans_control_block* pxBlock,
**                               const char* zAppName
****************************************************************/
void Message_ShowError(   messagetrans_control_block* pxBlock,
      	      	      	  const char* zAppName,
      	      	      	  const char* zToken,
      	      	      	  const char* zDefault, ... );


/****************************************************************
**  Display an error message in a wimp box with OK button
**  and Stop sprite (RO3.5+)
**  Does not return
**  981227  extra arguments added: messagetrans_control_block* pxBlock,
**                               const char* zAppName
****************************************************************/
void Message_ShowErrorFatal(  messagetrans_control_block* pxBlock,
      	      	      	      const char* zAppName,
      	      	      	      const char* zToken,
      	      	      	      const char* zDefault, ... );



/****************************************************************
**  Display a question in a wimp box with OK and cancel buttons
**  and question sprite (RO3.5+)
**  returns TRUE if OK is clicked
**  981227  extra arguments added:  messagetrans_control_block* pxBlock,
**                                  const char* zAppName
****************************************************************/
osbool Message_ShowQuery(       messagetrans_control_block* pxBlock,
      	      	      	      const char* zAppName,
      	      	      	      const char* zToken,
      	      	      	      const char* zDefault, ... );


#ifdef __cplusplus
}
#endif

#endif
/*** END ***/
