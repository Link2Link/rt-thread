/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2021-10-17     Meco Man      first version
 * 2022-05-10     Meco Man      improve rt-thread initialization process
 */
#include <lvgl.h>

#include "lv_conf.h"

#define RUN_LVGL_APP lv_demo_mine

void ui_init(void)
{
    /* display demo; you may replace with your LVGL application at here */
    extern void RUN_LVGL_APP(void);
    RUN_LVGL_APP();
}
