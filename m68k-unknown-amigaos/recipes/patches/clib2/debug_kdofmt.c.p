--- debug_kdofmt.c	2021-07-01 11:32:15.376743755 +0100
+++ debug_kdofmt.c	2021-07-01 11:40:09.153038071 +0100
@@ -40,6 +40,6 @@ KDoFmt(const char * format_string,APTR d
 {
 	assert( format_string != NULL );
 
-	if(format_string != NULL)
-		RawDoFmt((STRPTR)format_string,data_stream,(VOID (*)())put_char_routine,put_char_data);
+/*	if(format_string != NULL)
+		RawDoFmt((STRPTR)format_string,data_stream,(VOID (*)())put_char_routine,put_char_data); */
 }
