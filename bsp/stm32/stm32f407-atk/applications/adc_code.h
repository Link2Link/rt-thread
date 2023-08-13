#ifndef __ADC_CODE_H__
#define __ADC_CODE_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <lvgl.h>
#include "delay.h"
#include "stm32f4xx_hal.h"

static rt_sem_t sem_adc_done = RT_NULL;


/******************************************************************************************/
/* ADC及引脚 定义 */

#define ADC_ADCX_CH0_GPIO_PORT              GPIOB
#define ADC_ADCX_CH0_GPIO_PIN               GPIO_PIN_1
#define ADC_ADCX_CH0_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)         /* PA口时钟使能 */

#define ADC_ADCX_CH1_GPIO_PORT              GPIOA
#define ADC_ADCX_CH1_GPIO_PIN               GPIO_PIN_0
#define ADC_ADCX_CH1_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)         /* PA口时钟使能 */

#define ADC_ADCX_CH2_GPIO_PORT              GPIOB
#define ADC_ADCX_CH2_GPIO_PIN               GPIO_PIN_0
#define ADC_ADCX_CH2_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)         /* PA口时钟使能 */

#define ADC_ADCX                            ADC1 
#define ADC_ADCX_CH0                        ADC_CHANNEL_9                                       /* 通道Y,  0 <= Y <= 17 */ 
#define ADC_ADCX_CH1                        ADC_CHANNEL_0 
#define ADC_ADCX_CH2                        ADC_CHANNEL_8  
#define ADC_ADCX_CHY_CLK_ENABLE()           do{ __HAL_RCC_ADC1_CLK_ENABLE(); }while(0)          /* ADC1 时钟使能 */

#define ADC_CH_NUM                          3                                                   /* 需要转换的通道数目 */
#define ADC_IDX                             3
#define ADC_COLL                            (1<<ADC_IDX)                                                /* 单采集次数 */
#define ADC_SUM                             ADC_CH_NUM * ADC_COLL                               /* 总采集次数 */
/* ADC单通道/多通道 DMA采集 DMA及通道 定义*/

#define ADC_ADCX_DMASx                      DMA2_Stream4                                        /* DMA2数据流4 */
#define ADC_ADCX_DMASx_Chanel               DMA_CHANNEL_0                                       /* DMA通道0 */
#define ADC_ADCX_DMASx_IRQn                 DMA2_Stream4_IRQn
#define ADC_ADCX_DMASx_IRQHandler           DMA2_Stream4_IRQHandler

#define ADC_ADCX_DMASx_IS_TC()              ( __HAL_DMA_GET_FLAG(&g_dma_nch_adc_handle, DMA_FLAG_TCIF0_4) )  /* 判断DMA2 Stream4传输完成标志, 这是一个假函数形式,
                                                                                                              * 不能当函数使用, 只能用在if等语句里面 
                                                                                                              */
#define ADC_ADCX_DMASx_CLR_TC()             do{ __HAL_DMA_CLEAR_FLAG(&g_dma_nch_adc_handle, DMA_FLAG_TCIF0_4); }while(0)   /* 清除DMA2 Stream4传输完成标志 */

/******************************************************************************************/

void adc_nch_dma_init(void);                                                                    /* ADC DMA初始化 */
void adc_nch_dma_start(void);
uint32_t adc_get_result_average(uint8_t ch);                                                    /* 计算ADC平均值（滤波）*/
float get_busVoltage(uint16_t para);
float get_motorCurrent(uint16_t para, int OffsetCNT);
float get_temp(uint16_t para);



#define ADC2VBUS    (float)(3.3f * 25 / 4096)
#define ADC2CURT    (float)(3.3f / 4.096f / 0.12f)


#endif


