#include <Elementary.h>

static Evas_Object* pg;
static double val = 0.0;

static void
on_more(void *data, Evas_Object *obj, void *event_info)
{
   if (val < 1.0)
       val += 0.05;

   if (val > 1.0)
       val = 1.0;

   elm_progressbar_value_set(pg, val);
}

static void
on_less(void *data, Evas_Object *obj, void *event_info)
{
   if (val > 0)
       val -= 0.05;

   if (val < 0)
       val = 0;

   elm_progressbar_value_set(pg, val);
}

EAPI int
elm_main(int argc, char **argv)
{
   Evas_Object *win, *bg;

   win = elm_win_add(NULL, "hello", ELM_WIN_BASIC);

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, 1.0, 1.0);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   pg = elm_progressbar_add(win);
   elm_progressbar_label_set(pg, "Download");
   evas_object_size_hint_weight_set(pg, 0.8, 1.0);
   elm_win_resize_object_add(win, pg);

   Evas_Object *box = elm_box_add(win);
   evas_object_size_hint_weight_set(box, 1.0, 1.0);
   elm_win_resize_object_add(win, box);

   Evas_Object *box2 = elm_box_add(win);
   evas_object_size_hint_weight_set(box2, 1.0, 1.0);
   elm_win_resize_object_add(win, box2);

   Evas_Object *more = elm_button_add(win);
   elm_button_label_set(more, "Increase");
   evas_object_smart_callback_add(more, "clicked", on_more, NULL);
   
   Evas_Object *less = elm_button_add(win);
   elm_button_label_set(less, "Decrease");
   evas_object_smart_callback_add(less, "clicked", on_less, NULL);
   
   elm_box_pack_end(box, pg);
   elm_box_pack_end(box, box2);
   elm_box_pack_end(box2, more);
   elm_box_pack_end(box2, less);
   
   evas_object_show(pg);
   evas_object_show(more);
   evas_object_show(less);
   evas_object_show(box2);
   evas_object_show(box);

   evas_object_show(win);
   elm_run();
   elm_shutdown();
   return 0;
}
ELM_MAIN()
