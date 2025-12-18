/*
 *  test program for ldg_callback
 *
 *  (c)2005 Arnaud BERCEGEAY
 *
 *  $Id: interface.h 37 2005-10-25 22:51:53Z bercegeay $
 */

typedef struct 
{
	/* computation of the very complex formula "a2+b2+2ab" provided
	 *  by the client
	 */
	long __CDECL (*compute_a2plusb2plus2ab)( long a, long b);
	/* print a string to the "stdout" buffer of the client
	 */
	void __CDECL (*print2stdout)( const char *str);
} APP_STUFF;

