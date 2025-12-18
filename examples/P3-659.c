#include "oslib/os.h"
#include "oslib/osfind.h"
#include "oslib/pdriver.h"

extern os_error *Get_Document_Size (os_box *);
extern os_error *Fit_Two_Pages (int x0, int y0, int x1, int y1,
      os_box *, os_hom_trfm *, os_coord *, os_coord *);
extern os_error *Draw_Page (int page, os_box *);

os_error *print_out (int first_page, int last_page, char *title,
      char *file_name, font_f *font_ptr)

{  os_error *error;
   os_f job, old_job;
   osbool done_openout = FALSE, done_select_job = FALSE, more;
   bits features;
   os_box box, printable, box2;
   os_hom_trfm trfm;
   os_coord o1, o2;
   int page, page_to_print;

   if ((error = xosfind_openout (osfind_NO_PATH | osfind_ERROR_IF_DIR |
         osfind_ERROR_IF_ABSENT, "printer:", NULL, &job)) != NULL)
      goto finish;
   done_openout = TRUE;

   if ((error = xpdriver_select_job (job, title, &old_job)) != NULL)
      goto finish;
   done_select_job = TRUE;

   /*This code is not explicitly in the BASIC, but PROCdeclarefonts must do
      something like this:*/
   if ((error = xpdriver_info (NULL, NULL, NULL, &features, NULL, NULL,
         NULL, NULL)) != NULL)
      goto finish;

   if ((features & pdriver_FEATURE_DECLARE_FONT) != 0)
      do
         if ((error = xpdriver_declare_font (*font_ptr, NULL, 0)) != NULL)
            goto finish;
      while (*font_ptr++ != 0);

   if ((error = Get_Document_Size (&box)) != NULL)
      goto finish;

   if ((error = xpdriver_page_size (NULL, NULL, &printable.x0,
         &printable.y0, &printable.x1, &printable.y1)) != NULL)
      goto finish;

   if ((error = Fit_Two_Pages (printable.x0, printable.y0, printable.x1,
         printable.y1, &box, &trfm, &o1, &o2)) != NULL)
      goto finish;

   for (page = first_page; page <= last_page; page += 2)
   {  if ((error = xpdriver_give_rectangle (page, &box, &trfm, &o1,
            os_COLOUR_WHITE)) != NULL)
         goto finish;

      if (page < last_page)
         if ((error = xpdriver_give_rectangle (page + 1, &box, &trfm, &o2,
               os_COLOUR_WHITE)) != NULL)
            goto finish;

      if ((error = xpdriver_draw_page (1, &box2, 0, NULL, &more,
            &page_to_print)) != NULL)
         goto finish;

      while (more)
      {  if ((error = Draw_Page (page_to_print, &box2)) != NULL)
            goto finish;

         if ((error = xpdriver_get_rectangle (&box2, &more, &page_to_print))
               != NULL)
            goto finish;
      }
   }

finish:
   if (done_select_job)
   {  os_error *error1;

      error1 = error == NULL? xpdriver_end_job (job):
            xpdriver_abort_job (job);
      if (error == NULL) error = error1;

      error1 = xpdriver_select_job (old_job, NULL, NULL);
      if (error == NULL) error = error1;
   }

   if (done_openout)
   {  os_error *error1;

      error1 = xosfind_close (job);
      if (error == NULL) error = error1;
   }

   return error;
}
