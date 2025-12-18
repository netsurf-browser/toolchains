#include <string.h>

#include "oslib/os.h"

extern char Old_Font_Path [];
extern void Cache_List_Of_Fonts (void);

void ex (void)

{  char buffer [0x200];
   int used;

   os_gs_trans ("<Font$Prefix> and <Font$Path>", buffer, sizeof buffer - 1,
         &used);
   buffer [used] = '\0'; /*ensure there is a terminator (0 for C)*/

   if (strcmp (buffer, Old_Font_Path) != 0)
   {  strcpy (Old_Font_Path, buffer);
      Cache_List_Of_Fonts ();
   }
}
