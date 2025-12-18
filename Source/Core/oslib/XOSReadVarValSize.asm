;xos_read_var_val_size - return length of a variable
; Initial Release 	23-mar-99	Tony van der Hoff
; History
; 001230	TV	Update for 32-bit compatibility

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

	EXPORT 	xos_read_var_val_size

	AREA	|SWI$$Code|, CODE, READONLY, PIC

;	xos_Read_var_val_size
; 	APCS Compliant
;Entry  R0 (a1) ->  name of a variable (may be wildcarded)
;	R1 (a2) ->  context pointer for wildcarded names
;       R2 (a3) =   expand variable flag (os_VARTYPE_EXPANDED or 0)
;	R3 (a4) ->  integer to contain result = NOT length of variable value; 0 if not found
;	R4 (v1) ->  integer to contain variable type
;Exit  	R 	= 0, or pointer to error block
;
; C Prototype:
;extern os_error *xos_read_var_val_size (
;	char const *var,
;      	int context,
;      	os_var_type var_type,
;      	int *used,
;      	int *context_out,
;      	os_var_type *var_type_out );

xos_read_var_val_size ROUT
        MOV     IP, LR
        STMFD   SP!, {A4, V1}
        MOV     R4, A3		; expand variable flag
        MOV     R3, A2		; context pointer
        MOV     R2, #&80000000	; check variable
        MOV     R1, #0		; unused
        SWI     &20023		; XOS_ReadVarVal
        BVC     %90

        ;Ignore error if RO 3 or later
	STMFD	SP!, {R0, R2}	; keep results
	MOV 	R0, #129	; Get OS version
	MOV	R1, #0
	MOV 	R2, #&FF
	SWI	&20006          ; XOS_Byte
	ADDVS	SP, SP, #8	; flatten stack
	BVS	%99		; error exit
	LDMFD	SP!, {R, R2}	; restore state
	CMP	R1, #&A3        ; test for RISC OS 3
	BLO	%99		; RO2 - Can't ignore error

	; no real error
90      LDR     LR, [SP, #0]	; a4 = pointer to result
        TEQ     LR, #0
        STRNE   R2, [LR]	; save result
        LDR     LR, [SP, #8]	; v1 = pointer to context out
        TEQ     LR, #0
        STRNE   R3, [LR]        ; update context
        LDR     LR, [SP, #12]	; a3 = pointer to var type
        TEQ     LR, #0
        STRNE   R4, [LR]  	; variable type out
        MOV     R, #0		; clear error pointer

99      ADD     SP, SP, #4	; flatten stack
        LDMFD   SP!, {V1}	; restore arguments
	[ {CONFIG} = 32
	MOV	PC, IP
	|
        MOVS    PC, IP          ; exit
	]


	END

