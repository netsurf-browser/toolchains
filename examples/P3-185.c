#include "oslib/os.h"

extern os_t Old_Time;

void ex (void)

{  Old_Time += 100*((os_read_monotonic_time () - Old_Time - 1)/100 + 1);
}
