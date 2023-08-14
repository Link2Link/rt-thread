/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-10-18     Meco Man     The first version
 */
#include <lvgl.h>
#include <stdbool.h>
#include <rtdevice.h>
#include <board.h>
#include "gt911.h"

TS_StateTypeDef gt911_state;

static void touchpad_init(void);
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static bool touchpad_is_pressed(void);
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y);


static void touchpad_init(void)
{
    gt911_init();
}

static bool touchpad_is_pressed(void)
{
    /*Your code comes here*/
    gt911_get_state(&gt911_state);
    return (bool)gt911_state.TouchDetected;
}

static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y)
{
    // /*Your code comes here*/
    (*x) = gt911_state.X;
    (*y) = gt911_state.Y;
}

lv_indev_t *touch_indev;

static lv_indev_state_t last_state = LV_INDEV_STATE_REL;
static rt_int16_t last_x = 0;
static rt_int16_t last_y = 0;

static void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;

    /* 保存按下的坐标和状态 */
    if(touchpad_is_pressed())
    {
        touchpad_get_xy(&last_x, &last_y);
        data->state = LV_INDEV_STATE_PR;
    } 
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }

    /* 设置最后按下的坐标 */
    data->point.x = last_x;
    data->point.y = last_y;
}

void lv_port_indev_input(rt_int16_t x, rt_int16_t y, lv_indev_state_t state)
{
    last_state = state;
    last_x = x;
    last_y = y;
}

void lv_port_indev_init(void)
{
    static lv_indev_drv_t indev_drv;
	touchpad_init();
	

    lv_indev_drv_init(&indev_drv); /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;

    /*Register the driver in LVGL and save the created input device object*/
    touch_indev = lv_indev_drv_register(&indev_drv);
}
