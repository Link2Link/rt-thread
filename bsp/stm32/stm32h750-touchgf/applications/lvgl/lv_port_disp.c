/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-10-18     Meco Man     The first version
 */
#include <lvgl.h>
#include <board.h>
#include "ltdc.h"

#define MY_DISP_HOR_RES (LCD_W)   /* 屏幕宽度 */
#define MY_DISP_VER_RES (LCD_H)   /* 屏幕高度 */


static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);


void lv_port_disp_init(void)
{
    /*-------------------------
     * 初始化显示设备
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * 创建一个绘图缓冲区
     *----------------------------*/

    /**
     * LVGL 需要一个缓冲区用来绘制小部件
     * 随后，这个缓冲区的内容会通过显示设备的 `flush_cb`(显示设备刷新函数) 复制到显示设备上
     * 这个缓冲区的大小需要大于显示设备一行的大小
     *
     * 这里有3中缓冲配置:
     * 1. 单缓冲区:
     *      LVGL 会将显示设备的内容绘制到这里，并将他写入显示设备。
     *
     * 2. 双缓冲区:
     *      LVGL 会将显示设备的内容绘制到其中一个缓冲区，并将他写入显示设备。
     *      需要使用 DMA 将要显示在显示设备的内容写入缓冲区。
     *      当数据从第一个缓冲区发送时，它将使 LVGL 能够将屏幕的下一部分绘制到另一个缓冲区。
     *      这样使得渲染和刷新可以并行执行。
     *
     * 3. 全尺寸双缓冲区
     *      设置两个屏幕大小的全尺寸缓冲区，并且设置 disp_drv.full_refresh = 1。
     *      这样，LVGL将始终以 'flush_cb' 的形式提供整个渲染屏幕，您只需更改帧缓冲区的地址。
     */

    /* 单缓冲区示例) */
    static lv_disp_draw_buf_t draw_buf_dsc_1;
#if USE_SRAM
    static lv_color_t buf_1 = mymalloc(SRAMEX, MY_DISP_HOR_RES * MY_DISP_VER_RES);              /* 设置缓冲区的大小为屏幕的全尺寸大小 */
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * MY_DISP_VER_RES);     /* 初始化显示缓冲区 */
#else
    static lv_color_t buf_1[MY_DISP_HOR_RES * MY_DISP_VER_RES/10];                                              /* 设置缓冲区的大小为 10 行屏幕的大小 */
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * MY_DISP_VER_RES/20);                  /* 初始化显示缓冲区 */
#endif

    /* 双缓冲区示例) */
//    static lv_disp_draw_buf_t draw_buf_dsc_2;
//    static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10];                                            /* 设置缓冲区的大小为 10 行屏幕的大小 */
//    static lv_color_t buf_2_2[MY_DISP_HOR_RES * 10];                                            /* 设置另一个缓冲区的大小为 10 行屏幕的大小 */
//    lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * 10);             /* 初始化显示缓冲区 */

    /* 全尺寸双缓冲区示例) 并且在下面设置 disp_drv.full_refresh = 1 */
//    static lv_disp_draw_buf_t draw_buf_dsc_3;
//    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];                               /* 设置一个全尺寸的缓冲区 */
//    static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];                               /* 设置另一个全尺寸的缓冲区 */
//    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2, MY_DISP_HOR_RES * MY_DISP_VER_RES);/* 初始化显示缓冲区 */

    /*-----------------------------------
     * 在 LVGL 中注册显示设备
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                  /* 显示设备的描述符 */
    lv_disp_drv_init(&disp_drv);                    /* 初始化为默认值 */

    /* 建立访问显示设备的函数  */
    disp_drv.hor_res = LCD_W;
    disp_drv.ver_res = LCD_H;

    /* 用来将缓冲区的内容复制到显示设备 */
    disp_drv.flush_cb = disp_flush;

    /* 设置显示缓冲区 */
    disp_drv.draw_buf = &draw_buf_dsc_1;

    /* 全尺寸双缓冲区示例)*/
    //disp_drv.full_refresh = 1

    /* 如果您有GPU，请使用颜色填充内存阵列
     * 注意，你可以在 lv_conf.h 中使能 LVGL 内置支持的 GPUs
     * 但如果你有不同的 GPU，那么可以使用这个回调函数。 */
    //disp_drv.gpu_fill_cb = gpu_fill;

    /* 注册显示设备 */
    lv_disp_drv_register(&disp_drv);
}


static void disp_init(void)
{
    ltdc_init();
}



static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /* LVGL 官方给出的一个打点刷新屏幕的例子，但这个效率是最低效的 */

   int32_t x;
   int32_t y;
   uint16_t *color;

   for(y = area->y1; y <= area->y2; y++) {
       for(x = area->x1; x <= area->x2; x++) {
           /*Put a pixel to the display. For example:*/
           /*put_px(x, y, *color_p)*/
            color = (uint16_t*)color_p;
            ltdc_draw_point(x, y, *color);
            color_p++;
       }
   }
    /* 重要!!!
     * 通知图形库，已经刷新完毕了 */
    lv_disp_flush_ready(disp_drv);
}