#include <stddef.h>
#include <string.h>

#include "oslib/os.h"
#include "oslib/osmodule.h"
#include "oslib/messagetrans.h"

extern void *x_alloc (int);

typedef struct desc desc;
struct desc {messagetrans_control_block cb; char file_name [UNKNOWN];};
#define DESC(N) struct {messagetrans_control_block cb; char file_name [N];}
#define SIZEOF_DESC(N) (offsetof (desc, file_name) + (N))

void ex (char *file_name)

{  bits flags;
   int size;
   char *buffer;
   desc *file_desc;

   messagetrans_file_info (file_name, &flags, &size);
   if ((flags & messagetrans_DIRECT_ACCESS) != NONE)
      buffer = NULL;
   else
      buffer = x_alloc (size);

   file_desc = osmodule_alloc (SIZEOF_DESC (strlen (file_name) + 1));
   strcpy (file_desc->file_name, file_name);
   
   messagetrans_open_file (&file_desc->cb, file_desc->file_name, buffer);   
}
