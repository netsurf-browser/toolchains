/*
 * OSLib limited port
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "oslib/territory.h"
#include "oslib/os.h"

static double time1970 = ((60.0*60.0*24.0)*(70.0*365.0+17.0));

 /* ------------------------------------------------------------------------
 * Function:      territory_collate()
 *
 * Description:   Compares two strings in the given territory's alphabet
 *
 * Input:         territory - value of R0 on entry
 *                s1 - value of R1 on entry
 *                s2 - value of R2 on entry
 *                flags - value of R3 on entry
 *
 * Output:        collate - value of R0 on exit (X version only)
 *
 * Returns:       R0 (non-X version only)
 *
 * Other notes:   Calls SWI 0x4305D.
 */

os_error *xterritory_collate (territory_t territory,
      char const *s1,
      char const *s2,
      territory_collate_flags flags,
      int *collate)
{
    if(flags & territory_IGNORE_CASE)
    {
#ifdef WIN32
        if (collate) *collate = _stricoll(s1, s2);
#else
        if (collate) *collate = strcasecmp(s1, s2);
#endif
        }
    else
    {
        if (collate) *collate = strcoll(s1, s2);
    }
    return NULL;
}

int territory_collate (territory_t territory,
      char const *s1,
      char const *s2,
      territory_collate_flags flags)
{
    int collate;
    os_generate_error( xterritory_collate (territory, s1, s2, flags, &collate) );
    return collate;
}


/* ------------------------------------------------------------------------
 * Function:      territory_convert_date_and_time()
 *
 * Description:   Converts a 5-byte UTC time into a string giving the date
 *                and time using a supplied format string
 *
 * Input:         territory - value of R0 on entry
 *                date_and_time - value of R1 on entry
 *                buffer - value of R2 on entry
 *                size - value of R3 on entry
 *                format - value of R4 on entry
 *
 * Output:        end - value of R1 on exit (X version only)
 *
 * Returns:       R1 (non-X version only)
 *
 * Other notes:   Calls SWI 0x4304B.
 */

extern os_error *xterritory_convert_date_and_time (territory_t territory,
      os_date_and_time const *date_and_time,
      char *buffer,
      int size,
      char const *format,
      char **end)
{
    static double zt = 0;
    struct tm    *tm;
    time_t        dtime;
    char          format2[256];
    double        d  = 0;
    int           cs = 0;
    int           i,j;

    /* read the five byte value */
    for(i=4; i>=0; i--)
    {
        d *= 256.0L;
        d += ((byte*)date_and_time)[i];
    }

    // get centiseconds
    cs = (int)fmod(d, 100);

    /* convert centiseconds to seconds and correct for zero time */
    d  = (d/100.0L) - time1970;

    /* convert to time structure */
    dtime = (time_t)d;
    if((tm = localtime(&dtime))==NULL)
    {
        if (end) *end = buffer + sprintf(buffer, "<invalid time>");
        return NULL;
    }

    /* convert format strings */
    for(i=0, j=0; i<(int)strlen(format); i++)
    {
        format2[j++] = format[i];

        if(format[i]=='%')
        {
            switch(format[++i])
            {
                case '%':
                    format2[j++] = '%';
                    break;

                case '0':
                    format2[j-1] = 0;
                    break;

                case '1': /* 12 */
                    format2[j++] = 'I';
                    i++;
                    break;

                case '2': /* 24 */
                    format2[j++] = 'H';
                    i++;
                    break;

                case 'A': /* AM */
                case 'P': /* PM */
                    format2[j++] = 'p';
                    i++;
                    break;

                case 'C':
                    switch(format[++i])
                    {
                        case 'S': /* CS - not ANSI, so make inline */
                            j += sprintf(format2+j-1, "%02d", cs)-1;
                            break;

                        case 'E': /* CE - not ANSI, check for %YR following */
                            if(format[i+1]=='%' &&
                               format[i+2]=='Y' &&
                               format[i+3]=='R')
                            {
                                format2[j++] = 'Y';
                                i+=3;
                            }
                            else
                            {
                                j += sprintf(format2+j, "%d", (tm->tm_year+1900)/100);
                            }
                            break;

                        default:
                            /* remove special interpretation from unknown field*/
                            format2[j++] = '%';
                            format2[j++] = format[i-1];
                            format2[j++] = format[i];
                            break;
                    }
                    break;

                case 'D':
                    switch(format[++i])
                    {
                        case 'N': /* DN */
                            format2[j++] = 'j';
                            break;

                        case 'Y': /* DY */
                            format2[j++] = 'd';
                            break;

                        default:
                            /* remove special interpretation from unknown field*/
                            format2[j++] = '%';
                            format2[j++] = format[i-1];
                            format2[j++] = format[i];
                            break;
                    }
                    break;

                case 'M':
                    switch(format[++i])
                    {
                        case '3': /* M3 */
                            format2[j++] = 'b';
                            break;

                        case 'I': /* MI */
                            format2[j++] = 'M';
                            break;

                        case 'N': /* MN */
                            format2[j++] = 'm';
                            break;

                        case 'O': /* MO */
                            format2[j++] = 'B';
                            break;

                        default:
                            /* remove special interpretation from unknown field*/
                            format2[j++] = '%';
                            format2[j++] = format[i-1];
                            format2[j++] = format[i];
                            break;
                    }
                    break;

                case 'S':
                    switch(format[++i])
                    {
                        case 'E': /* SE */
                            format2[j++] = 'S';
                            break;

                        case 'T': /* ST - non ANSI */
                            strncpy(format2+j, (char*)("thstndrdthththththth"+((tm->tm_mday%10)*2)), 2);
                            j+=2;
                            break;

                        default:
                            /* remove special interpretation from unknown field*/
                            format2[j++] = '%';
                            format2[j++] = format[i-1];
                            format2[j++] = format[i];
                            break;
                    }
                    break;

                case 'T': /* TZ - non ANSI */
                    strcpy(format2+j, tm->tm_isdst ? "BST":"GMT");
                    j+=2;
                    i++;
                    break;

                case 'W':
                    switch(format[++i])
                    {
                        case '3': /* W3 */
                            format2[j++] = 'a';
                            break;

                         case 'E': /* WE */
                            format2[j++] = 'A';
                            break;

                        case 'K': /* WK */
                            format2[j++] = 'U';
                            break;

                        case 'N': /* WN */
                            format2[j++] = 'w';
                            break;

                        default:
                            /* remove special interpretation from unknown field*/
                            format2[j++] = '%';
                            format2[j++] = format[i-1];
                            format2[j++] = format[i];
                            break;
                    }
                    break;

                case 'Y': /* YR */
                    format2[j++] = 'y';
                    i++;
                    break;

                default:
                    /* remove special interpretation from unknown field*/
                    format2[j++] = '%';
                    format2[j++] = format[i];
                    break;
            }
        }
    }
    format2[j] = 0;

    if (end) *end = buffer + strftime(buffer, size, format2, tm);
    return(NULL);
}

extern char *territory_convert_date_and_time (territory_t territory,
      os_date_and_time const *date_and_time,
      char *buffer,
      int size,
      char const *format)
{
    char *end;
    os_generate_error( xterritory_convert_date_and_time(territory,
    							date_and_time,
							buffer,
							size,
							format,
							&end
							));
    return end;
}
