/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-28     unknow       copy by STemwin
 */
#ifndef __DRV_MYLCD_H
#define __DRV_MYLCD_H
#include <rtthread.h>
#include "rtdevice.h"
#include <drv_common.h>

typedef struct 
{
    uint16_t x;
    uint16_t y; 
    uint16_t width; 
    uint16_t height; 
    uint8_t size;
    char *p;
    uint16_t color;

}ST_LCD_Write;

void rt_lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color);
void lcd_clean();
void lcd_test();
void lcd_fill_array(rt_uint16_t x_start, rt_uint16_t y_start, rt_uint16_t x_end, rt_uint16_t y_end, void *pcolor);




#endif
