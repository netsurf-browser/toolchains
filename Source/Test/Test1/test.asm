; Test each assembler header individually

	GET	include.Hdr	; gets all OSLib headers

	EXPORT	|entry|

	AREA	code, CODE

|entry|	MOV 	pc, lr

	END
