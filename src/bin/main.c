#include <Elementary.h>
   
/* callback used for sorting files in pure ascii ordering */
static int
sort_callback(char *f1, char *f2)
{
   return strcmp(f1, f2);
}

/* clear the list of previous entries, then load a dir and fill the list */
static void
load_files(Evas_Object *li, const char *dir)
{
   Eina_List *files;
   const Eina_List *items,*cur;

   /* if no dir is given (NULL) make it "." */
   if (!dir) dir = ".";
   /* if the dir is empty ("") then we must be in "/" (/
   else if (dir[0] == 0) dir = "/";
   /* while there are items in the list - delete them */
   while ((items = elm_list_items_get(li)))
     {
        /* also free the path string attached to the data value of the item */
        char *path = (char *)elm_list_item_data_get(items->data);
        if (path) free(path);
        /* delete the item */
        elm_list_item_del(items->data);
     }
   /* list files in the dir */
   files = ecore_file_ls(dir);
   if (files)
     {
        char buf[PATH_MAX];
        char *path, *file;
        Evas_Object *ic;

        /* if we are in root - set the dir name to "" to avoid double // */
        if (!strcmp(dir, "/")) dir = "";
        else
          {
             /* if not in root - we can go back a dir, so add a ".." */
             file = "..";
             snprintf(buf, sizeof(buf), "%s/%s", dir, file);
             path = strdup(buf);
             /* add an icon widget */
             ic = elm_icon_add(li);
             /* use the standard arrow_left icon */
             elm_icon_standard_set(ic, "arrow_left");
             /* allow it to scale down, but not up */
             elm_icon_scale_set(ic, 0, 1);
             /* append item with icon and path we duplicated as data */
             elm_list_item_append(li, file, ic, NULL,  NULL, path);
             /* show the icon */
             evas_object_show(ic);
          }
        /* sort the  file list we got - alphabetically (ascii-wise) */
        files = eina_list_sort(files, 0, EINA_COMPARE_CB(sort_callback));
        /* walk the file list for the dir */
        EINA_LIST_FOREACH(files, cur, file)
          {
             /* if it's a "dot file" - skip it */
             if (file[0] == '.')
               {
                  continue;
               }
             /* generate a full path */
             snprintf(buf, sizeof(buf), "%s/%s", dir, file);
             /* FIXME: This is never free'd */
             path = strdup(buf);
             /* if its a direcotry */
             if (ecore_file_is_dir(buf))
               {
                  /* put in item with an "arrow_right" icon */
                  ic = elm_icon_add(li);
                  elm_icon_standard_set(ic, "arrow_right");
                  elm_icon_scale_set(ic, 0, 1);
                  elm_list_item_append(li, file, ic, NULL,  NULL, path);
                  evas_object_show(ic);
               }
             else
               {
                  /* put in icon with "apps" icon */
                  ic = elm_icon_add(li);
                  elm_icon_standard_set(ic, "apps");
                  elm_icon_scale_set(ic, 0, 1);
                  elm_list_item_append(li, file, ic, NULL,  NULL, path);
                  evas_object_show(ic);
               }
          }
        /* free the file list */
        EINA_LIST_FOREACH(files, cur, file) free(file);
        eina_list_free(files);
     }
   /* tell the list widget we are done messing with it - and go go go! */
   elm_list_go(li);
}

/* if someone presses the close button on our window - exit nicely */
static void
win_del(void *data, Evas_Object *obj, void *event_info)
{
   elm_exit();
}
/* when ok is pressed this runs */
static void
on_ok(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *li = data;
   /* get the selected item */
   Elm_List_Item *it = (Elm_List_Item *)elm_list_selected_item_get(li);
   if (it)
     {
        /* if there is a selected item - get the diata, i'ts the string
         * we added to the item, print it */
        char *path = (char *)elm_list_item_data_get(it);
        printf("%s\n", path);
     }
   /* cleanly exit */
   elm_exit();
}

/* when cancel is pressed this runs */
static void
on_cancel(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *li = data;
   /* cleanly exit */
   elm_exit();
}

/* when the user double-clicks on the list, this runs */
static void
on_double_click(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *li = data;
   Elm_List_Item *it = (Elm_List_Item *)elm_list_selected_item_get(li);
   if (it)
     {
        char *path = (char *)elm_list_item_data_get(it);
        if (path)
          {
             if (ecore_file_is_dir(path))
               {
                  char *p, *newpath;

                  p = strrchr(path, '/');
                  if ((p) && (!strcmp(p, "/..")))
                    {
                       *p = 0;
                       p = strrchr(path, '/');
                       if (p) *p = 0;
                    }
                  newpath = strdup(path);
                  load_files(li, newpath);
                  free(newpath);
               }
             else
               {
                  elm_exit();
               }
          }
     }
}

/* main elm app core - build the ui core here. something like the below:
 *
 * Inside a window we put:
 * +---------------------------+
 * |Box + Bg under it here     |
 * |    +-----------------+    |
 * |    |Frame + outdent  |    |
 * |    |+---------------+|    |
 * |    ||Frame + pad    ||    |
 * |    ||+-------------+||    |
 * |    |||Label        |||    |
 * |    ||+-------------+||    |
 * |    |+---------------+|    |
 * |    +-----------------+    |
 * |+-------------------------+|
 * ||List here                ||
 * ||                         ||
 * |+-------------------------+|
 * |+-------------------------+|
 * ||Frame + outdent          ||
 * ||+-----------------------+||
 * |||Frame + padding        |||
 * |||+---------------------+|||
 * ||||Box (horiz)          ||||
 * ||||+---------++--------+||||
 * |||||    OK   || Cancel |||||
 * ||||+---------++--------+||||
 * |||+---------------------+|||
 * ||+-----------------------+||
 * |+-------------------------||
 * +---------------------------+
 */
EAPI int
elm_main(int argc, char **argv)
{
   Evas_Object *win, *bg, *bx, *lb, *li, *bt, *bx2, *fr, *fr0;

   /* new window - do the usual and give it a name, title and delete handler */
   win = elm_win_add(NULL, "selector", ELM_WIN_BASIC);
   elm_win_title_set(win, "Selector");
   evas_object_smart_callback_add(win, "delete,request", win_del, NULL);

   /* add a standard bg */
   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, 1.0, 1.0);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   /* add a box object - default is vertical. a box holds children in a row,
    * either horizontally or vertically. nothing more. */
   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, 1.0, 1.0);
   elm_win_resize_object_add(win, bx);
   evas_object_show(bx);
   
   /* now let's add a styled frame for the top - outdent_top style */
   fr0 = elm_frame_add(win);
   /* weight here means "dont expand either horizontally OR vertically */
   evas_object_size_hint_weight_set(fr0, 0.0, 0.0);
   /* align here means center horizontally and fill vertically */
   evas_object_size_hint_align_set(fr0, 0.5, -1.0);
   /* add the frame to the end of the box */
   elm_box_pack_end(bx, fr0);
   evas_object_show(fr0);

   /* add a padding frame - empty space. medium padding. themes can vary
    * the amount of padding, as can scaling, so don't depend on your pixel
    * cound here. add pad frame to styled frame above */
   fr = elm_frame_add(win);
   elm_frame_content_set(fr0, fr);
   evas_object_show(fr);

   /* add a label widget, set the text and put it in the pad frame */
   lb = elm_label_add(win);
   elm_label_label_set(lb, "Please select a file.");
   elm_frame_content_set(fr, lb);
   evas_object_show(lb);

   /* add the list widget - it comes in a scroller so pretty easy */
   li = elm_list_add(win);
   /* set the weight to say "expand this entry horizontally AND vertically */
   evas_object_size_hint_weight_set(li, 1.0, 1.0);
   /* set the align to mean "fill the region this widget gets" */
   evas_object_size_hint_align_set(li, -1.0, -1.0);
   elm_box_pack_end(bx, li);
   evas_object_show(li);
   /* if you double-click on the list - on_double_click gets called and
    * pass li as data param */
   evas_object_smart_callback_add(li, "clicked", on_double_click, li);
                  
   /* add another styled frame - outdent_bottom now */
   fr0 = elm_frame_add(win);
   /* expand horizontallt but not vertically */
   evas_object_size_hint_weight_set(fr0, 1.0, 0.0);
   /* fill allocated region */
   evas_object_size_hint_align_set(fr0, -1.0, -1.0);
   elm_box_pack_end(bx, fr0);
   evas_object_show(fr0);

   /* add medium pad frame inside outdent frame */
   fr = elm_frame_add(win);
   elm_frame_content_set(fr0, fr);
   evas_object_show(fr);

   /* add horizontal box inside pad frame */
   bx2 = elm_box_add(win);
   elm_box_horizontal_set(bx2, 1);
   elm_box_homogenous_set(bx2, 1);
   elm_frame_content_set(fr, bx2);
   evas_object_show(bx2);
 
   /* add ok button to box - fill and expand */
   bt = elm_button_add(win);
   elm_button_label_set(bt, "OK");
   evas_object_size_hint_weight_set(bt, 1.0, 1.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);
   /* and when clicked - run "on_ok" - pass list as data param */
   evas_object_smart_callback_add(bt, "clicked", on_ok, li);
 
   /* add cancel button to box - fill and expand */
   bt = elm_button_add(win);
   elm_button_label_set(bt, "Cancel");
   evas_object_size_hint_weight_set(bt, 1.0, 1.0);
   evas_object_size_hint_align_set(bt, -1.0, -1.0);
   elm_box_pack_end(bx2, bt);
   evas_object_show(bt);
   /* and when clicked - run "on_cancel" - pass list as data param */
   evas_object_smart_callback_add(bt, "clicked", on_cancel, li);
 
   /* if we have more than 1 argument, then we have parameters as arg 0 is
    * the command itself, do this first arg is the dir */
   if (argc > 1) load_files(li, argv[1]);
   /* no args - use "." as the dir */
   else load_files(li, ".");

   /* force the window size to 300x300 otherwise list is at min size - that
    * is tiny */
   evas_object_resize(win, 480, 272);

   /* show the window */
   evas_object_show(win);

   /* get going and draw/respond to the user */
   elm_run();
   /* standard shutdown */
   elm_shutdown();
   return 0;
}
ELM_MAIN()
