#include "oslib/os.h"
#include "oslib/osbyte.h"

extern void My_Byte_Veneer (void);

void ex (void)

{  os_claim (ByteV, (void *) &My_Byte_Veneer, NULL);
}
