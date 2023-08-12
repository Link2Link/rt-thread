/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 * 2018-11-19     flybreak     add stm32f407-atk-explorer bsp
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <lvgl.h>
#include "delay.h"
#include "stm32f4xx_hal.h"
#include "adc_code.h"



static rt_sem_t sem_adc_done = RT_NULL;
void adc_init(void);                        /* ADC初始化 */
void adc_nch_dma_init(void);                /* ADC DMA传输 初始化函数 */
double get_temp(uint16_t para);



#define HWTIMER_DEV_NAME   "timer11"     /* 定时器名称 */

#define TEST_PIN    GET_PIN(I, 11)

static rt_err_t timeout_cb_10KHZ(rt_device_t dev, rt_size_t size)
{
    
    int temp;
    rt_pin_write(TEST_PIN, PIN_HIGH);
    rt_pin_write(TEST_PIN, PIN_LOW);
    
    return 0;
}

static int hwtimer_10KHz_init()
{
    sem_adc_done = rt_sem_create("sem_adc_done", 0, RT_IPC_FLAG_PRIO);
    if (sem_adc_done == RT_NULL)
    {
        rt_kprintf("create sem_adc_done semaphore failed.\n");
        return -1;
    }

    rt_pin_mode(TEST_PIN, PIN_MODE_OUTPUT);

    rt_err_t ret = RT_EOK;
    rt_hwtimerval_t timeout_s;      /* 定时器超时值 */
    rt_device_t hw_dev = RT_NULL;   /* 定时器设备句柄 */
    rt_hwtimer_mode_t mode;         /* 定时器模式 */
    rt_uint32_t freq = 1000*1000;               /* 计数频率 */

    /* 查找定时器设备 */
    hw_dev = rt_device_find(HWTIMER_DEV_NAME);
    if (hw_dev == RT_NULL)
    {
        rt_kprintf("hwtimer sample run failed! can't find %s device!\n", HWTIMER_DEV_NAME);
        return RT_ERROR;
    }

    /* 以读写方式打开设备 */
    ret = rt_device_open(hw_dev, RT_DEVICE_OFLAG_RDWR);
    if (ret != RT_EOK)
    {
        rt_kprintf("open %s device failed!\n", HWTIMER_DEV_NAME);
        return ret;
    }

    /* 设置超时回调函数 */
    rt_device_set_rx_indicate(hw_dev, timeout_cb_10KHZ);

    /* 设置计数频率(若未设置该项，默认为1Mhz 或 支持的最小计数频率) */
    rt_device_control(hw_dev, HWTIMER_CTRL_FREQ_SET, &freq);
    /* 设置模式为周期性定时器（若未设置，默认是HWTIMER_MODE_ONESHOT）*/
    mode = HWTIMER_MODE_PERIOD;
    ret = rt_device_control(hw_dev, HWTIMER_CTRL_MODE_SET, &mode);
    if (ret != RT_EOK)
    {
        rt_kprintf("set mode failed! ret is :%d\n", ret);
        return ret;
    }

    /* 设置定时器超时值为5s并启动定时器 */
    timeout_s.sec = 0;      /* 秒 */
    timeout_s.usec = 100;     /* 微秒 */
    if (rt_device_write(hw_dev, 0, &timeout_s, sizeof(timeout_s)) != sizeof(timeout_s))
    {
        rt_kprintf("set timeout value failed\n");
        return RT_ERROR;
    }

    return ret;
}


rt_align(RT_ALIGN_SIZE)
static char thread_adc_stack[1024];
static struct rt_thread thread_adc;

/* 线程 1 的入口函数 */
static void thread_adc_entry(void *parameter)
{

    int temp;

    static rt_err_t result;

    double lvgl_show_adc = 0;
    extern lv_obj_t* showString;

    //adc_init();
    while (1)
    {
        result = rt_sem_take(sem_adc_done, RT_WAITING_FOREVER);
        if (result != RT_EOK)
        {
            rt_kprintf("thread_adc take a dynamic semaphore, failed.\n");
            rt_sem_delete(sem_adc_done);
            return;
        }
        // 同步于adc采集完成
         

    }
}


rt_align(RT_ALIGN_SIZE)
static char thread_adcshow_stack[1024];
static struct rt_thread thread_adcshow;

/* 线程 1 的入口函数 */
static void thread_adcshow_entry(void *parameter)
{

    double lvgl_show_adc = 0;
    extern lv_obj_t* showString;

    adc_nch_dma_init();
    while (1)
    {
        uint32_t adc_result;
        float voltage;
        
//         adc_result = adc_get_result_average(100);
//         voltage = ((float)adc_result)*ADC2VBUS;
// #define Pf(X) (int)X, (int)((X - (int)X)*100)
//         lv_label_set_text_fmt(showString, "adc_result = %d\n V = %d.%d", adc_result, Pf(voltage));
        
        msleep(20);


    }
}



void adc_task_setup()
{
    hwtimer_10KHz_init();
    //adc_nch_dma_init();
    rt_thread_init(&thread_adc,
                   "thread_adc",
                   thread_adc_entry,
                   RT_NULL,
                   &thread_adc_stack[0],
                   sizeof(thread_adc_stack),
                   10,      //优先级
                   5);
    rt_thread_startup(&thread_adc);

    rt_thread_init(&thread_adcshow,
                "thread_adcshow",
                thread_adcshow_entry,
                RT_NULL,
                &thread_adcshow_stack[0],
                sizeof(thread_adcshow_stack),
                20,      //优先级
                5);
    rt_thread_startup(&thread_adcshow);

    
}


void btn1_cb_app()
{

}

void get_info()
{

}


MSH_CMD_EXPORT(get_info, hwtimer sample);


