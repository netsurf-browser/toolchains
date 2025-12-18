;os_read_var_val_size - return length of a variable
; Initial Release 	23-Mar-99	Tony van der Hoff

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

; This code works around the RO3 infelicity which causes OS_ReadVarVal to return an error
; when R2 bit 31 is set to read the variable length. The object code is patched into the
; library at build time by the MakeFile

	GET oslib/Types.Hdr

	EXPORT	os_read_var_val_size

	AREA	|SWI$$Code|, CODE, READONLY, PIC

;	os_Read_var_val_size
; 	APCS Compliant
;Entry  R0 (a1) ->  name of a variable (may be wildcarded)
;	R1 (a2) ->  context pointer for wildcarded names
;       R2 (a3) =   variable type (os_VARTYPE_EXPANDED or 0)
;	R3 (a4) ->  integer to contain result = NOT length of variable value; 0 if not found
;	R4 (v1) ->  integer to contain variable type
;Exit  	R 	= updated context pointer
;
;  C Prototype:
;extern int os_read_var_val_size (
;	char const *var,
;      	int context,
;      	os_var_type var_type,
;      	int *used,
;      	os_var_type *var_type_out);

; C prototype: int xos_read_var_val_size( 	char const *name,
;						int *context,
;						os_var_type var_type
;					);
;
;

os_read_var_val_size ROUT
        STMFD   SP!, {A4, V1}
        MOV     R4, A3		; variable type
        MOV     R3, A2		; context in
        MOV     R2, #&80000000	; check variable
        MOV     R1, #0		; unused
        SWI     &20023		; XOS_ReadVarVal
        BVC 	%90		; no error

        ;Ignore error if RO 3 or later
	STMFD	SP!, {R0, R2}	; keep results
	MOV 	R0, #129	; Get OS version
	MOV	R1, #0
	MOV 	R2, #&FF
	SWI	&6              ; OS_Byte
	LDMFD	SP!, {R0, R2}	; restore state
	CMP	R1, #&A3        ; test for RISC OS 3
	SWILO	&2B		; RO2 - call OS_GenerateError

90      LDR     IP, [SP, #0]	; Result pointer
        TEQ     IP, #0
        STRNE   R2, [IP]	; update result
        LDR     IP, [SP, #8]    ; var-type out pointer
        TEQ     IP, #0
        STRNE   R4, [IP]	; update var type
        MOV     R, R3		; new context

99      ADD     SP, SP, #4      ; flatten stack
        LDMFD   SP!, {V1}       ; restore state
        
	[ {CONFIG} = 32
	MOV	PC, LR
	|
        MOVS    PC, LR          ; exit
	]

        END
