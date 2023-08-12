/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-28     unknow       copy by STemwin
 * 2021-12-29     xiangxistu   port for lvgl <lcd_fill_array>
 * 2022-6-26      solar        Improve the api required for resistive touch screen calibration
 */

#include <board.h>
#include "drv_mylcd.h"
#include "string.h"
#include "lcd.h"


/**RTT 设备驱动 **/


#define LCD_DEVICE(dev) (struct drv_lcd_device *)(dev)


struct drv_lcd_device
{
    struct rt_device parent;

    struct rt_device_graphic_info lcd_info;

    struct rt_semaphore lcd_lock;

    rt_uint8_t cur_buf;
    rt_uint8_t *front_buf;
    rt_uint8_t *back_buf;
    
};

static struct drv_lcd_device _lcd;

static rt_err_t drv_lcd_init(struct rt_device *device)
{
    struct drv_lcd_device *lcd = LCD_DEVICE(device);

    
    lcd_init();
    lcd_display_dir(1);
    lcd_clear(WHITE);
    lcd_show_string(10, 40, 240, 32, 32, "STM32", RED);

    return RT_EOK;
}

static rt_err_t drv_lcd_write(rt_device_t device, rt_off_t pos, const void *buffer, rt_size_t size)
{
    struct drv_lcd_device *lcd = LCD_DEVICE(device);

        ST_LCD_Write * ptr;
        ptr = (ST_LCD_Write * )buffer;
        lcd_show_string(ptr->x, ptr->y, ptr->width, ptr->height, ptr->size, ptr->p, ptr->color);
    return RT_EOK;
}

static rt_err_t drv_lcd_control(struct rt_device *device, int cmd, void *args)
{
    struct drv_lcd_device *lcd = LCD_DEVICE(device);
    switch (cmd)
    {
    case RTGRAPHIC_CTRL_GET_INFO:
    {
        struct rt_device_graphic_info *info = (struct rt_device_graphic_info *)args;

        RT_ASSERT(info != RT_NULL);

        //this needs to be replaced by the customer
        info->pixel_format  = lcd->lcd_info.pixel_format;
        info->bits_per_pixel = lcd->lcd_info.bits_per_pixel;
        info->width = lcddev.width;
        info->height = lcddev.height;
    }
    break;
    case RTGRAPHIC_CTRL_RECT_UPDATE:
    {
        struct rt_device_graphic_info *info = (struct rt_device_graphic_info *)args;
        lcd_clear(WHITE);
    }
    break;
    }

    return RT_EOK;

}
static rt_err_t  drv_lcd_open  (rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t  drv_lcd_close  (rt_device_t dev)
{
    return RT_EOK;
}

const static struct rt_device_ops lcd_ops =
    {
        drv_lcd_init,
        drv_lcd_open,
        drv_lcd_close,
        RT_NULL,
        drv_lcd_write,
        drv_lcd_control};


static void LCD_Fast_DrawPoint(const char *pixel, int x, int y)
{

}

void LCD_ReadPoint(char *pixel, int x, int y)
{

}

void LCD_HLine(const char *pixel, int x1, int x2, int y)
{

}
    
void LCD_VLine(const char *pixel, int x, int y1, int y2)
{

}

void LCD_BlitLine(const char *pixel, int x, int y, rt_size_t size)
{

}


struct rt_device_graphic_ops fsmc_lcd_ops =
    {
        LCD_Fast_DrawPoint,
        LCD_ReadPoint,
        LCD_HLine,
        LCD_VLine,
        LCD_BlitLine,
};


int drv_lcd_hw_init(void)
{
    rt_err_t result = RT_EOK;
    struct rt_device *device = &_lcd.parent;
    memset(&_lcd, 0x00, sizeof(_lcd));

    _lcd.lcd_info.bits_per_pixel = 16;
    _lcd.lcd_info.pixel_format = RTGRAPHIC_PIXEL_FORMAT_RGB565;
    
    device->type = RT_Device_Class_Graphic;
    #ifdef RT_USING_DEVICE_OPS
    device->ops = &lcd_ops;
    #else
    device->init = drv_lcd_init;
    device->control = drv_lcd_control;
    #endif
    device->user_data = &fsmc_lcd_ops;
    rt_device_register(device, "mylcd", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);
    return result;

}

INIT_DEVICE_EXPORT(drv_lcd_hw_init);

void rt_lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color)
{
    rt_device_t lcd; 
    struct drv_lcd_device *lcd_d;
    lcd = rt_device_find("mylcd");
    // lcd_d = (struct drv_lcd_device * )lcd;
    

    ST_LCD_Write data;
    data.x = x;
    data.y = y;
    data.width = width;
    data.height = height;
    data.size = size;
    data.p = p;
    data.color = color;

    rt_device_init(lcd);
    rt_device_open(lcd, RT_DEVICE_OFLAG_RDWR);
    rt_device_write(lcd, 0, &data, sizeof(data));
    rt_device_close(lcd);
}


void lcd_test()
{
    rt_lcd_show_string(10, 40, 240, 32, 32, "STM32", RED);
    rt_lcd_show_string(10, 80, 240, 24, 24, "TFTLCD TEST", RED);
    rt_lcd_show_string(10, 110, 240, 16, 16, "ATOM@ALIENTEK", RED);
	rt_kprintf("lcddev.width=%d\n", lcddev.width);
	rt_kprintf("lcddev.height=%d\n", lcddev.height);
}
MSH_CMD_EXPORT(lcd_test, lcd test);

void lcd_clean()
{
    rt_device_t lcd; 
    struct drv_lcd_device *lcd_d;
    lcd = rt_device_find("mylcd");
    lcd_d = (struct drv_lcd_device * )lcd;
    rt_device_control(lcd, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
}
MSH_CMD_EXPORT(lcd_clean, lcd clean);
