/*Example of how to use OS_ReadArgs in C.*/

#include <stdio.h>

#include "oslib/macros.h"
#include "oslib/os.h"

int main (void)

{  struct {char *programme, *from, *to, *since; osbool help; os_gi *size;
         os_gs *name; char argd [1024];} argl;

   os_read_args ("programme/a,from/a,to/a,since/k,help/s,size/e,name/g",
         os_get_env (NULL, NULL), (char *) &argl, sizeof argl);
   /*Each structure element must correspond to the keyword in that place:

            keyword qualifier       component type
            ------- ---------       --------- ----
            switch (/s)             osbool
            expression (/e)         os_gi
            string (/g)             os_gs
            everything else         char *
   */

   printf
   (  "programme: %s\n"
      "from: %s\n"
      "to: %s\n"
      "since: %s\n"
      "help: %s\n"
      /*"size: %d\n"	-- TV 20040707 */
      "size: %hd\n"
      "name: %.*s\n",
      argl.programme,
      argl.from,
      argl.to,
      argl.since? argl.since: "(not given)",
      WHETHER (argl.help),
      argl.size? WORD (argl.size->i): -1,
      argl.name? /*SHORT*/ (argl.name->size): 80,
      argl.name? argl.name->s: "(not given)"
   );
   	/*Note use of WORD and SHORT, since these are not necessarily word-
          aligned pointers.*/
	/* The logic of this statement escapes me;
	   moreover, SHORT has to be commented out, else it gives a compiler warning.
	   -- TV 20040707
	*/
   return 0;
}
