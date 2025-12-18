;Index: OSLib/!OsLib/Source/Core/oslib/OSChangeDynamicAreaNoFail.asm	20031228
; os_change_dynamic_area_no_fail - Extend or shrink dynamic area without throwing error if
;	the change was only partially performed
 
; Initial Release 	08-Nov-01	Tony van der Hoff
;	Bug fix		21-Dec-01	TV

;OSLib---efficient, type-safe, transparent, extensible,
;   register-safe API coverage of RISC OS
;
;   OSLib is free software; you can redistribute it and/or modify
;it under the terms of the GNU General Public Licence as published by
;the Free Software Foundation; either version 1, or (at your option)
;any later version.
;
;   OSLib is distributed in the hope that it will be useful,
;but WITHOUT ANY WARRANTY; without even the implied warranty of
;MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;GNU General Public License for more details.
;
;   You should have received a copy of the GNU General Public Licence
;along with this programme; if not, write to the Free Software
;Foundation, Inc, 675 Mass Ave, Cambridge, MA 02139, U S A.

	GET oslib/OS.Hdr

	EXPORT	os_change_dynamic_area_no_fail

	AREA	|SWI$$Code|, CODE, READONLY, PIC

;	os_change_dynamic_area_no_fail
; 	APCS Compliant
;Entry  A1 =  area type
;	A2 =  required size change in bytes (signed integer)
;Exit  	R  =  actual size change in bytes (unsigned integer)
;
;  C Prototype:
;extern int os_change_dynamic_area_no_fail( os_dynamic_area_no area,
;				   	    os_dynamic_area_no size_increase
;			                  );
;
; This code works around the problem that OS_ChangeDynamicArea throws an error if the move is only
; partially performed. OSLib is then unable to return the amount by which the area has changed.
;
; The object code is patched into the library at build time by the MakeFile.


os_change_dynamic_area_no_fail ROUT
        SWI     XOS_ChangeDynamicArea	; XOS_ChangeDynamicArea	(PRM 1-377)
        BVC 	%99			; no error - normal return

        ;Test for Error_ChDynamNotAllMoved -  report any other error
	LDR 	R3, =Error_ChDynamNotAllMoved
	LDR	R2, [R0]		; get error number in R2 [TV 011221]
	CMP 	R2, R3
	SWINE	&2B			; OS_GenerateError - no return

	; ignore Error_ChDynamNotAllMoved

	; exit
99	MOV 	R, R1			; APCS return value

	[ {CONFIG} = 32
	MOV	PC, LR
	|
        MOVS    PC, LR
	]

	LTORG

        END
