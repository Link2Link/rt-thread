#include "lv_conf.h"
#include <lvgl.h>
#include <board.h>
#include <drv_lcd.h>
#include "rtthread.h"

lv_obj_t* showString;
lv_obj_t* showString2;
lv_obj_t* showString3;
lv_obj_t* showString4;
extern void btn1_cb_app();


static void btn1_cb(lv_event_t* event)
{
    rt_kprintf("Clicked 1\n");
    btn1_cb_app();
}

static void btn2_cb(lv_event_t* event)
{
    rt_kprintf("Clicked 2\n");
}

static void btn3_cb(lv_event_t* event)
{
    rt_kprintf("Clicked 3\n");
}

static void btn4_cb(lv_event_t* event)
{
    rt_kprintf("Clicked 4\n");
}

static void btn5_cb(lv_event_t* event)
{
    rt_kprintf("Clicked 5\n");
}

void lv_demo_mine(void)
{

    lv_obj_t* btn1 = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn1, 200, 80);
    lv_obj_align(btn1, LV_ALIGN_RIGHT_MID, 0, -200);
    lv_obj_add_event_cb(btn1, btn1_cb, LV_EVENT_CLICKED, NULL);   /*Assign an event callback*/
    lv_obj_t* label1 = lv_label_create(btn1);
    lv_label_set_text_fmt(label1, "btn1");
    //lv_obj_set_style_text_font(label1, &lv_font_montserrat_30, LV_STATE_DEFAULT);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);


    lv_obj_t* btn2 = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn2, 200, 80);
    lv_obj_align(btn2, LV_ALIGN_RIGHT_MID, 0, -100);
    lv_obj_add_event_cb(btn2, btn2_cb, LV_EVENT_CLICKED, NULL);   /*Assign an event callback*/
    lv_obj_t* label2 = lv_label_create(btn2);
    lv_label_set_text_fmt(label2, "btn2");
    lv_obj_set_align(label2, LV_ALIGN_CENTER);


    lv_obj_t* btn3 = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn3, 200, 80);
    lv_obj_align(btn3, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_event_cb(btn3, btn3_cb, LV_EVENT_CLICKED, NULL);   /*Assign an event callback*/
    lv_obj_t* label3 = lv_label_create(btn3);
    lv_label_set_text_fmt(label3, "btn3");
    lv_obj_set_align(label3, LV_ALIGN_CENTER);


    lv_obj_t* btn4 = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn4, 200, 80);
    lv_obj_align(btn4, LV_ALIGN_RIGHT_MID, 0, 100);
    lv_obj_add_event_cb(btn4, btn4_cb, LV_EVENT_CLICKED, NULL);   /*Assign an event callback*/
    lv_obj_t* label4 = lv_label_create(btn4);
    lv_label_set_text_fmt(label4, "btn4");
    lv_obj_set_align(label4, LV_ALIGN_CENTER);

    lv_obj_t* btn5 = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn5, 200, 80);
    lv_obj_align(btn5, LV_ALIGN_RIGHT_MID, 0, 200);
    lv_obj_add_event_cb(btn5, btn5_cb, LV_EVENT_CLICKED, NULL);   /*Assign an event callback*/
    lv_obj_t* label5 = lv_label_create(btn5);
    lv_label_set_text_fmt(label5, "btn5");
    lv_obj_set_align(label5, LV_ALIGN_CENTER);

    // 左侧工作区域
    lv_obj_t* area = lv_obj_create(lv_scr_act());
    lv_obj_set_size(area, 600, 480);

    showString = lv_label_create(area);
    lv_label_set_text_fmt(showString, "LVGL");

    showString2 = lv_label_create(area);
    lv_label_set_text_fmt(showString2, "LVGL");
    lv_obj_align_to(showString2, showString, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    showString3 = lv_label_create(area);
    lv_label_set_text_fmt(showString3, "LVGL");
    lv_obj_align_to(showString3, showString2, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    showString4 = lv_label_create(area);
    lv_label_set_text_fmt(showString4, "LVGL");
    lv_obj_align_to(showString4, showString3, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

}


