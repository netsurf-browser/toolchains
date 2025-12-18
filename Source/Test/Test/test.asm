; Test assembler headers
   	GET	all_oslib.Hdr		; gets all OSLib headers

	EXPORT	|_main|

	AREA	code, CODE

|main|	STMFD	sp!, {lr}		; save the return address
	ADR	R0, message		; write the message std out
	SWI	OS_Write0
	SWI	OS_NewLine
	LDMFD	sp!, {pc}

message	=	"Hello World from ObjAsm",0

	END
