#include "oslib/macros.h"
#include "oslib/draw.h"

#define PATH_LEN 256

static int Path [PATH_LEN/sizeof (int)], Path_Ptr = 0;
static os_error Too_Small = {0, "Insufficient path buffer"};

void add (int value)

{  if (Path_Ptr + 1 > COUNT (Path))
      os_generate_error (&Too_Small);

   Path [Path_Ptr++] = value;
}

int main (void)

{  int xsize, ysize, xscale, yscale;
   os_trfm trfm;
   draw_path *path = (draw_path *) Path;
   draw_line_style line_style;

   xsize = 210000, ysize = 160000;

   xscale = 1280*256/xsize << 16;
   yscale = 1024*256/ysize << 16;

   trfm.entries [0] [0] = xscale;
   trfm.entries [0] [1] = 0;
   trfm.entries [1] [0] = 0;
   trfm.entries [1] [1] = yscale;
   trfm.entries [2] [0] = 0;
   trfm.entries [2] [1] = 0;

   line_style.join_style = draw_JOIN_BEVELLED;
   line_style.end_cap_style = draw_CAP_ROUND;
   line_style.start_cap_style = draw_CAP_ROUND;
   line_style.reserved = 0;
   line_style.mitre_limit = 0;
   line_style.start_cap_width = 0;
   line_style.start_cap_length = 0;
   line_style.end_cap_width = 0;
   line_style.end_cap_length = 0;

   add (draw_MOVE_TO), add (80000), add (80000);
   add (draw_LINE_TO), add (80000), add (120000);
   add (draw_LINE_TO), add (170000), add (120000);
   add (draw_LINE_TO), add (170000), add (80000);
   add (draw_MOVE_TO), add (50000), add (50000);
   add (draw_BEZIER_TO), add (80000), add (80000),
         add (85000), add (30000), add (50000), add (60000);
   add (draw_CLOSE_GAP);
   add (draw_END_PATH), add (PATH_LEN - (Path_Ptr + 1)*sizeof (int));
   
   os_writen ("\x1D\0\0\0\0", 5);
   draw_stroke (path, NONE, &trfm, xsize/640, 5000, &line_style, NULL);

   trfm.entries [2] [0] = 60 << 8;
   trfm.entries [2] [1] = -100 << 8;
   draw_stroke (path, NONE, &trfm, xsize/640, 2500, &line_style, NULL);

   return 0;
}
