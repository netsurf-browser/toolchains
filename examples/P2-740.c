#include "oslib/hourglass.h"
#include "oslib/os.h"

void example (void)

{  hourglass_on ();
      hourglass_on ();
      hourglass_percentage (10);    /*sets to 10*/
      hourglass_percentage (20);    /*sets to 20*/
         hourglass_on ();
         hourglass_percentage (50); /*DOESN'T set to 50*/
         hourglass_off ();
      hourglass_percentage (30);    /*sets to 30*/
      hourglass_off ();             /*turns off percentages*/
   hourglass_off ();                /*turns off hourglass*/
}
