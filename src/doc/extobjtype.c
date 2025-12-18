/* $Id: extobjtype.c,v 1.1 2005/11/17 23:06:20 bercegeay Exp $ */

/**@file*/

/** @page Extobjtype Extended OBJECT types of windom

Windom defines some extra OBJECTs, based on mydial objects. The table hereafter summaries
all the extended objects of windom.

<table>
<tr><td>name		
    <td>ob_type
	<td>Xtype
	<td>ob_flag & ob_state
	<td>Shortcut
<tr><td>Long boxed text (internal windom constant: XTEDINFO)
    <td>G_BOXTEXT, G_TEXT
	<td>11
	<td>
	<td>No
<tr><td>Long boxed text (internal windom constant: XBOXLONGTEXT)
    <td>G_BOXTEXT, G_TEXT
	<td>12
	<td>
	<td>No
<tr><td>Color icons (internal windom constant: XCICON)
    <td>G_CICON
	<td>13
	<td>optional ob_state: [STATE8]
	<td>No
<tr><td>Editable field (internal windom constant: XEDIT)
    <td>G_FTEXT, G_TEXT
	<td>14
	<td>mandatory ob_flag : EDITABLE \n
	    optional ob_state : [DRAW3D]
	<td>No
<tr><td>Menu item (internal windom constant: MENUTITLE)
    <td>G_STRING
	<td>15
	<td>
	<td>No
<tr><td>Menu title (internal windom constant: MENUTITLE)
    <td>G_TITLE
	<td>15
	<td>
	<td>No
<tr><td>Thumb index button (internal windom constant: ONGLET)
    <td>G_BUTTON, G_STRING
	<td>16
	<td>mandatory ob_flag: RBUTTON \n
	    optional ob_state: [DRAW3D][STATE8][STATE15]
	<td>Yes
<tr><td>Thumb index background (internal windom constant: ONGLET)
    <td>G_BUTTON, G_STRING
	<td>16
	<td>forbidden ob_flag: RBUTTON \n
	    optional ob_state: [DRAW3D]
	<td>Yes
<tr><td>Background form (internal windom constant: DIALMOVER)
    <td>G_BOX
	<td>17
	<td>optional ob_state: [DRAW3D][OUTLINE]
	<td>No
<tr><td>Radio button (internal windom constant: DCRBUTTON)
    <td>G_BUTTON, G_STRING
	<td>18
	<td>mandatory ob_flag: RBUTTON \n
	    optinal ob_state: [DRAW3D] [STATE8]
	<td>Yes
<tr><td>Checked button (internal windom constant: DCRBUTTON)
    <td>G_BUTTON, G_STRING
	<td>18
	<td>forbidden ob_flag: RBUTTON, EXIT \n
	    optinal ob_state: [DRAW3D][STATE8]
	<td>Yes
<tr><td>Exit button (internal windom constant: DCRBUTTON)
    <td>G_BUTTON, G_STRING
	<td>18
	<td>mandatory ob_flag: EXIT \n
	    forbidden ob_flag:  RBUTTON \n
	    optional ob_flag: [DEFAULT] \n
		optional ob_state: [DRAW3D]
	<td>Yes
<tr><td>Cycle button (internal windom constant: CIRCLEBUT)
    <td>G_BUTTON, G_STRING
	<td>22
	<td>optional ob_state: [DRAW3D]
	<td>No
<tr><td>Underlined texte (internal windom constant: UNDERLINE)
    <td>G_BUTTON, G_STRING
	<td>19
	<td>optional ob_state: [DRAW3D]
	<td>Yes
<tr><td>Frame with title (internal windom constant: TITLEBOX)
    <td>G_BUTTON
	<td>20
	<td>optional ob_state: [DRAW3D]
	<td>No
<tr><td>Help button (internal windom constant: HELPBUT)
    <td>G_BUTTON
	<td>21
	<td>
	<td>No
<tr><td>Text with keyboard shortcut (internal windom constant: KPOPUPSTRG)
    <td>G_BUTTON, G_STRING
	<td>24
	<td>optional ob_state: [DISABLED]
	<td>Yes
<tr><td>Box Char (internal windom constant: SLIDEPART)
    <td>G_BOXCHAR
	<td>25
	<td>optional ob_state: [DRAW3D]
	<td>No
<tr><td>Undo button (internal windom constant: UNDOHELP)
    <td>G_BUTTON
	<td>31
	<td>
	<td>No 
<tr><td>Undo object
    <td>any
	<td>
	<td>FLAG11
	<td>No
<tr><td>Relief button 
    <td>-
	<td>-
	<td>optional ob_state: [DRAW3D]
	<td>No
<tr><td>Userdraw object 
    <td>-
	<td>255
	<td>
	<td>No
</table>

@par Legend:
- STATEn	means the n bit of the (!I)ob_state(!i) field (!nl)
- DRAW3D is an alias of STATE7 (used by Interface) (!nl)

If you use such extended objects in your application, your should call RsrcXtype() to transform
these "standard" objects with an extended type to a G_USERDEF object with the nice rendering
awaiting. 

@note
Please note that such "extended" objects may be transformed to a G_USERDEF object, and so
it's forbidden to diectly access to the ob_spec data. You should use generic ObjcString()
function instead to change or get the object text for example.


TODO: SLIDEPART is deprecated ? Undo object deprecated (reserved for highligh flag ?)

@note
SLIDEPART is an object used to create boxchar (specially for scrolling
objects) having the same apparence under TOS, Naes or MagiC.


Windom specific objects
- XTEDINFO is used to display G_TEXT and G_BOXTEXT object with the
	  same aspect (in DRAW3D mode) on TOS, MagiC or any AES replacement.
- XBOXLONGTEXT is a BOXTEXT object displaying the text on several
	lines if needed. ObjcEdit() is used as interface to the object.
- XCICON is used to display color icons. Using this king of object
		allows to supported color icon on any TOS version. RsrcXload()
		converts automatically G_CICON icon into XCICON icon using
		RsrcFixCicon() to fix icons to current resolution.
		XCICON admit additional states : STATE8 enable a transparent
		text, STATE9, STATE10 and STATE11 controls text effect
		(as describe below).
- XEDIT is an editable objet with an unlimited (except the memory)
	text length. It is very recommanded to use it rather than classic
	EDITABLE object. ObjcEdit() is used as interface to the object.
- MENUTITLE is used as menu title and menu item object. The text
	can be display using a specific font (see windom.menu.font). Keyboard
	shortcut are correctly aligned on the right even with a 
	proportional font.
- ONGLET is a special object for multiple formular thumb indexed.
	As all extended object, it is sensible to the DRAW3D state.
- USERDRAW is a special type reserved by RsrcUserDraw. Please, never
	  use this value for your own extended objects.



Usual extended states

Follow, the signification of the \a ob_state bits unused by AES but 
used by WinDom.

- DRAW3D draws the object with a relief effect,
- STATE8 draws the object with an alternative look :
    - radio button: (!nl)
		STATE8: button have an MagiC look, (!nl)
		!~STATE8: button have a xv look,
    - check button: (!nl)
		STATE8: a square button with a white foreground color and
				a cross inside, (!nl)
		!~STATE8: a square button with a ckeck symbol (OpenLook style),
    - thumb index: (!nl)
		STATE8: thumb index are displayed with rounded corners, (!nl)
		!~STATE8: thumb index are displayed with squarred corners,
    - underlined text: (!nl)
		STATE8: the object is underlined, (!nl)
		!~STATE8: only the text inside the object is underlined,
- All extended object including text support the following text
	  attributs :
    !table_caption Attributs de texte
    !begin_table [|c|c|] !hline
    ob_state bit!! Attribut
    !hline
    STATE9  !!  (!B)Bold(!b)
    STATE10 !!  (!U)Underline(!u)
    STATE11 !!  (!I)Italic(!i)
	STATE12 !!  Outline
    STATE13 !!  Shadow
    STATE14 !!  Light
    !hline
    !end_table
    Note: the light attribut is used when an object has the state DISABLED.


Keyboard shortcuts

Some objects can display a keybord shortcut (see table ...). A keyboard
shortcut is a underlined letter inside the text. It means that the object
can be selected by typing the key combinaison [Alternate + letter].
To make appear this shortcut, just add a '[' character behing the
letter. For example: (!nl)

[Cancel  -> (!U)C(!u)ancel -> alt+c.


Special text objects

WinDom defines two special text objects : XBOXLONGTEXT and XBOXLONGTEXT.

The first one allows you to display a long text in a BOXTEXT objet. The
text can be display on sereval lines if needed. Just set the extended type
in your ressource editor. Long text is read or written using the function
ObjcString().

The second one is an editable object without limitation of text size. This
object is very easy to handle and usefull and replaces effiencly the
standard EDITABLE object. However, standard EDITABLE object can be used
in a case of formated fields (such as date input for example) because
XEDIT objects don't use the template string (as G_FTEXT objets).



The UserDraw objects

These objects are not really extended object. The extended type (255) should
never be set directly by the user from the ressource editor. This value 
is not used by RsrcXtype() but by RsrcUserDraw().

The goal of these objects is to provide an easy way to draw something
in a formular or a toolbar inside a window. Drawing inside classical GEM
formular is not possible. 

To attribute a drawing function to an object, you have to use the
RsrcUserDraw() function. This function transforms the object in a 
special extended type (255) format. After this call, the AES will call
the function given to RsrcUserDraw() to draw the object. The function
given to RsrcUserDraw() - that we call the UserDraw function - and the 
Userdef function are differents. Actually, the Userdef function calls 
the drawing function to draw the object. The main raison of this system 
is that the drawing function is similar to a standard drawing function 
of a window (i.e. the function called by (!B)WM_REDRAW(!b)), but there 
are some differences:

	- the UserDraw function have not the same argument. There is
 		  an additionnal parameter, a (!B)PARMBLK(!b) structure poviding
		  all informations related to the object.
	- we have the same limitations for the UserDraw function than
		  the userdef function.

The only one difference with a Userdef function is that we should never
clip the redraw area of the object. This action is performed by the
userdef function calling your UserDraw function.




*/

