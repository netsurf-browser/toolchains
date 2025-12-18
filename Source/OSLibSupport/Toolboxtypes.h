/****************************************************************
** Title  :   SupportLib.h.ToolboxTypes
**
** Purpose:   Provides translation for ToolboxLib names to OSLib names
**
** Copyright:   (c) 1997 Control Computer Consultants
**
** History:     970709  created
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

#if !defined toolboxtypes_H
  #define toolboxtypes_H

#if !defined( toolbox_H )
  #include "oslib/toolbox.h"
#endif

  typedef toolbox_block     	    IdBlock;
    #define self_id                this_obj
    #define self_component         this_cmp
    #define parent_id              parent_obj
    #define parent_component       parent_cmp
    #define ancestor_id            ancestor_obj
    #define ancestor_component     ancestor_cmp

  typedef toolbox_o 	      	    ObjectId;
  typedef toolbox_c 	      	    ComponentId;

  typedef toolbox_action          ToolboxEvent;


#endif



/*** END ***/
