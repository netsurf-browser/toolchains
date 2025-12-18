#include "oslib/buffer.h"
#include "oslib/osbyte.h"

os_error *insert_character (byte *buffer)

{  return xos_byte (osbyte_BUFFER_INSERT, buffer [4], buffer_KEYBOARD,
         NULL, NULL);
}
