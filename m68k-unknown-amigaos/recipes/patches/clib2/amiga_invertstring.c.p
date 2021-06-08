--- ./builddir/clib2/amiga_invertstring.c	2017-05-01 17:06:23.000000000 +0100
+++ ./amiga_invertstring.c	2021-06-08 14:18:03.132632551 +0100
@@ -138,7 +138,7 @@ extern VOID FreeIEvents(struct InputEven
 /****************************************************************************/
 
 struct InputEvent *
-InvertString(CONST_STRPTR str, CONST struct KeyMap *km)
+InvertString(STRPTR str, CONST struct KeyMap *km)
 {
 	/* bugs:
 	   can't escape '>'
