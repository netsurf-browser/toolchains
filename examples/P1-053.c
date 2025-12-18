#include "oslib/buffer.h"
#include "oslib/os.h"
#include "oslib/osbyte.h"

void ex (void)

{  os_byte (osbyte_BUFFER_INSERT, buffer_KEYBOARD, 'A', NULL, NULL);
}
