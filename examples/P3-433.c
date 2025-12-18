#include "oslib/font.h"

char *move_string (int move)

{  static char S [5];

   char *p = S;

   *p++ = font_COMMAND_YMOVE;
   *p++ = move;
   *p++ = move >> 8;
   *p++ = move >> 16;

   return S;
}
