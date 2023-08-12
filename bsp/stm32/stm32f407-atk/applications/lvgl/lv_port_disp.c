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
#include "lcd.h"

#define MY_DISP_HOR_RES (800)   /* 屏幕宽度 */
#define MY_DISP_VER_RES (480)   /* 屏幕高度 */


static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);


void lcd_draw_fast_rgb_color(int16_t sx, int16_t sy,int16_t ex, int16_t ey, uint16_t *color)
{
    uint16_t w = ex-sx+1;
    uint16_t h = ey-sy+1;

    lcd_set_window(sx, sy, w, h);
    uint32_t draw_size = w * h;
    lcd_write_ram_prepare();

    for(uint32_t i = 0; i < draw_size; i++)
    {
        lcd_wr_data(color[i]);
    }
}



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

    /* 设置显示设备的分辨率
     * 这里为了适配正点原子的多款屏幕，采用了动态获取的方式，
     * 在实际项目中，通常所使用的屏幕大小是固定的，因此可以直接设置为屏幕的大小 */
    disp_drv.hor_res = lcddev.width;
    disp_drv.ver_res = lcddev.height;

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
    /*You code here*/
    lcd_init();         /* 初始化LCD */
    lcd_display_dir(1); /* 设置横屏 */
}



static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /* LVGL 官方给出的一个打点刷新屏幕的例子，但这个效率是最低效的 */

//    int32_t x;
	
//    int32_t y;
//    for(y = area->y1; y <= area->y2; y++) {
//        for(x = area->x1; x <= area->x2; x++) {
//            /*Put a pixel to the display. For example:*/
//            /*put_px(x, y, *color_p)*/
//            color_p++;
//        }
//    }

//    /* 在指定区域内填充指定颜色块 */
//    lcd_color_fill(area->x1, area->y1, area->x2, area->y2, (uint16_t *)color_p);
    lcd_draw_fast_rgb_color(area->x1,area->y1,area->x2,area->y2,(uint16_t*)color_p);

    /* 重要!!!
     * 通知图形库，已经刷新完毕了 */
    lv_disp_flush_ready(disp_drv);
}






//// #define MY_DISP_HOR_RES LCD_W
//// #define DISP_BUFFER_LINES 70

//void lcd_draw_fast_rgb_color(int16_t sx, int16_t sy,int16_t ex, int16_t ey, uint16_t *color)
//{
//    uint16_t w = ex-sx+1;
//    uint16_t h = ey-sy+1;

//    lcd_set_window(sx, sy, w, h);
//    uint32_t draw_size = w * h;
//    lcd_write_ram_prepare();

//    for(uint32_t i = 0; i < draw_size; i++)
//    {
//        lcd_wr_data(color[i]);
//    }
//}


///*A static or global variable to store the buffers*/
//static lv_disp_draw_buf_t disp_buf;

///*Descriptor of a display driver*/
//static lv_disp_drv_t disp_drv;

///*Static or global buffer(s). The second buffer is optional*/
//#if defined ( __ICCARM__ ) /*!< IAR Compiler */
//#pragma location=0x68000000
//lv_color_t buf_1[LCD_H * LCD_W];
//#elif defined ( __CC_ARM )  /* MDK ARM Compiler */
//__attribute__((at(0x68000000))) lv_color_t buf_1[LCD_H * LCD_W];
//#elif defined ( __clang__ ) /* MDK ARM Compiler v6 */
//__attribute__((section(".ARM.__at_0x68000000"))) lv_color_t buf_1[LCD_H * LCD_W];
//#elif defined ( __GNUC__ ) /* GNU Compiler */
//lv_color_t buf_1[LCD_H * LCD_W] __attribute__((section(".MCUlcdgrambysram")));
//#ifdef RT_USING_MEMHEAP_AS_HEAP
//#error "You should modify this logic, such as use 'rt_malloc' to create lvgl buf"
//#endif
//#endif

///*Flush the content of the internal buffer the specific area on the display
// *You can use DMA or any hardware acceleration to do this operation in the background but
// *'lv_disp_flush_ready()' has to be called when finished.*/
//static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
//{
//    /* color_p is a buffer pointer; the buffer is provided by LVGL */
//    lcd_draw_fast_rgb_color(area->x1,area->y1,area->x2,area->y2,(uint16_t*)color_p);
//    /*IMPORTANT!!!
//     *Inform the graphics library that you are ready with the flushing*/
//    lv_disp_flush_ready(disp_drv);
//}

//void lv_port_disp_init(void)
//{
//	lcd_init();         /* 初始化LCD */
//    lcd_display_dir(1); /* 设置横屏 */
//    /*Initialize `disp_buf` with the buffer(s). With only one buffer use NULL instead buf_2 */
//    lv_disp_draw_buf_init(&disp_buf, buf_1, NULL, LCD_H * LCD_W);

//    lv_disp_drv_init(&disp_drv); /*Basic initialization*/

//    /*Set the resolution of the display*/
//    disp_drv.hor_res = LCD_W;
//    disp_drv.ver_res = LCD_H;

//    /*Set a display buffer*/
//    disp_drv.draw_buf = &disp_buf;

//    /*Used to copy the buffer's content to the display*/
//    disp_drv.flush_cb = disp_flush;

//    /*Finally register the driver*/
//    lv_disp_drv_register(&disp_drv);
//}
