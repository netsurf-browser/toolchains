/*
 * OS Lib very limited port
 */

#include <time.h>
#include <math.h>
#include "oslib/osword.h"
#include "oslib/os.h"

static struct tm time1900 =
{ 
    0,	    /* seconds after the minute - [0,59] */
    0,      /* minutes after the hour - [0,59] */
    0,      /* hours since midnight - [0,23] */
    1,      /* day of the month - [1,31] */
    1,      /* months since January - [0,11] */
    0,      /* years since 1900 */
    0,      /* days since Sunday - [0,6] */
    0,      /* days since January 1 - [0,365] */
    0       /* daylight savings time flag */
};
static struct tm time1970 =
{ 
    0,	    /* seconds after the minute - [0,59] */
    0,      /* minutes after the hour - [0,59] */
    0,      /* hours since midnight - [0,23] */
    1,      /* day of the month - [1,31] */
    1,      /* months since January - [0,11] */
    70,     /* years since 1900 */
    0,      /* days since Sunday - [0,6] */
    0,      /* days since January 1 - [0,365] */
    0       /* daylight savings time flag */
};

/* ------------------------------------------------------------------------
 * Function:      oswordreadclock_utc()
 *
 * Description:   Reads the real-time clock in 5-byte format
 *
 * Input:         utc - value of R1 on entry
 *
 * Other notes:   Calls SWI 0x7 with R0 = 0xE.
 */

os_error *xoswordreadclock_utc (oswordreadclock_utc_block *utc)
{
	static double zt = 0;
	double d         = 0;
	time_t t         = time(NULL);
	int    i;

	/* calculate zero time correction */
	if(zt==0)
	{
		time_t t0 = mktime(&time1900);
		time_t t1 = mktime(&time1970);
		zt        = (double)t1 - (double)t0;
	}

	/* correct for zero time and convert seconds to centiseconds */
	d = ((double)t + zt)*100.0L;

	/* write the five byte value */
	for(i=0; i<5; i++)
	{
		if (utc) 	utc->utc[i] = (byte)fmod(d, 256.0L);
		d          /= 256.0L;
	}

	return NULL;
}

void oswordreadclock_utc (oswordreadclock_utc_block *utc)
{
	os_generate_error( xoswordreadclock_utc(utc) );
}
