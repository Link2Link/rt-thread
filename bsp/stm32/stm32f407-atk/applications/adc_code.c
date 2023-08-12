
#include "adc_code.h"
#define TEST_PIN2 GET_PIN(I, 10)
DMA_HandleTypeDef g_dma_nch_adc_handle = {0};                   /* 定义要搬运ADC多通道数据的DMA句柄 */
ADC_HandleTypeDef g_adc_nch_dma_handle = {0};                   /* 定义ADC（多通道DMA读取）句柄 */

uint16_t g_adc_value[ADC_SUM] = {0};                            /* 存储ADC原始值 */
int16_t adc[3] = {0};                                           /* ADC平均值 */

/**
 * @brief       ADC N通道(3通道) DMA读取 初始化函数
 * @param       无
 * @retval      无
 */
void adc_nch_dma_init(void)
{
	rt_pin_mode(TEST_PIN2, PIN_MODE_OUTPUT);
	
    GPIO_InitTypeDef gpio_init_struct = {0};
    ADC_ChannelConfTypeDef sConfig = {0};
      
    ADC_ADCX_CHY_CLK_ENABLE();                                  /* 使能ADCx时钟 */
    ADC_ADCX_CH3_GPIO_CLK_ENABLE();                             /* 开启GPIO时钟 */
    ADC_ADCX_CH4_GPIO_CLK_ENABLE();
    ADC_ADCX_CH5_GPIO_CLK_ENABLE();
    
    if ((uint32_t)ADC_ADCX_DMASx > (uint32_t)DMA2)              /* 大于DMA2基地址，则为DMA2的数据流通道了 */
    {
        __HAL_RCC_DMA2_CLK_ENABLE();                            /* DMA2时钟使能 */
    }
    else 
    {
        __HAL_RCC_DMA1_CLK_ENABLE();                            /* DMA1时钟使能 */
    }
    
    /* 设置ADC1通道3~5对应的IO口模拟输入 */
    gpio_init_struct.Pin = ADC_ADCX_CH3_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;
    gpio_init_struct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADC_ADCX_CH3_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = ADC_ADCX_CH4_GPIO_PIN;
    HAL_GPIO_Init(ADC_ADCX_CH4_GPIO_PORT, &gpio_init_struct);
    
    gpio_init_struct.Pin = ADC_ADCX_CH5_GPIO_PIN;   
    HAL_GPIO_Init(ADC_ADCX_CH5_GPIO_PORT, &gpio_init_struct); 

    /* DMA配置 */
    g_dma_nch_adc_handle.Instance = ADC_ADCX_DMASx;                                 /* 设置DMAx */
    g_dma_nch_adc_handle.Init.Channel = DMA_CHANNEL_0;                              /* 设置DMA通道 */
    g_dma_nch_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;                     /* 外设到存储器模式 */
    g_dma_nch_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;                         /* 外设非增量模式 */
    g_dma_nch_adc_handle.Init.MemInc = DMA_MINC_ENABLE;                             /* 存储器增量模式 */
    g_dma_nch_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;        /* 外设数据长度:16位 */
    g_dma_nch_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;           /* 存储器数据长度:16位 */
    g_dma_nch_adc_handle.Init.Mode = DMA_NORMAL;                                    /* 外设流控模式 */
    g_dma_nch_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                       /* 中等优先级 */
    HAL_DMA_Init(&g_dma_nch_adc_handle);
    
    __HAL_LINKDMA(&g_adc_nch_dma_handle,DMA_Handle,g_dma_nch_adc_handle);           /* 将DMA与adc联系起来 */
    
    g_adc_nch_dma_handle.Instance = ADC_ADCX;
    g_adc_nch_dma_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;            /* 4分频，21Mhz */
    g_adc_nch_dma_handle.Init.Resolution = ADC_RESOLUTION_12B;                      /* 12位模式 */
    g_adc_nch_dma_handle.Init.ScanConvMode = ENABLE;                                /* 使能扫描模式 */
    g_adc_nch_dma_handle.Init.ContinuousConvMode = ENABLE;                          /* 使能连续转换 */
    g_adc_nch_dma_handle.Init.DiscontinuousConvMode = DISABLE;                      /* 禁止规则通道组间断模式 */
    g_adc_nch_dma_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; /* 使用软件触发 */
    g_adc_nch_dma_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                /* 软件触发 */
    g_adc_nch_dma_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                      /* 右对齐 */
    g_adc_nch_dma_handle.Init.NbrOfConversion = ADC_CH_NUM;                         /* 本实验用到3个规则通道序列 */
    g_adc_nch_dma_handle.Init.DMAContinuousRequests = ENABLE;                       /* 开启DMA连续转换 */
    HAL_ADC_Init(&g_adc_nch_dma_handle);
    
    sConfig.Channel = ADC_ADCX_CH3;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &sConfig);                         /* 配置ADC通道3 */

    sConfig.Channel = ADC_ADCX_CH4;
    sConfig.Rank = 2;
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &sConfig);                         /* 配置ADC通道4 */

    sConfig.Channel = ADC_ADCX_CH5;
    sConfig.Rank = 3;
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &sConfig);                         /* 配置ADC通道5 */

    /* 配置中断 */
    HAL_NVIC_SetPriority(ADC_ADCX_DMASx_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(ADC_ADCX_DMASx_IRQn);

    /* 开启ADC，使用DMA读取 */
    HAL_ADC_Start_DMA(&g_adc_nch_dma_handle,(uint32_t *)g_adc_value,ADC_SUM);
}

/**
 * @brief       ADC DMA采集中断服务函数
 * @param       无 
 * @retval      无
 */
void ADC_ADCX_DMASx_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_dma_nch_adc_handle);
}

/**
 * @brief       ADC采集中断回调函数
 * @param       * hadc：ADC句柄
 * @retval      无
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    HAL_ADC_Stop_DMA(hadc);                             /* 关闭DMA传输 */
    rt_pin_write(TEST_PIN2 , PIN_HIGH);
    // adc[0] = adc_get_result_average(0);                 /* 计算ADC平均值 */
    // adc[1] = adc_get_result_average(1);
    // adc[2] = adc_get_result_average(2);
    rt_pin_write(TEST_PIN2 , PIN_LOW);

    HAL_ADC_Start_DMA(&g_adc_nch_dma_handle, (uint32_t*)&g_adc_value, ADC_SUM);
}

/**
 * @brief       计算ADC平均值（滤波）
 * @param       ch : 通道
 * @retval      ADC平均值
 */
uint32_t adc_get_result_average(uint8_t ch)
{
    uint32_t temp_val = 0;
    uint16_t t;

    for (t = ch; t < ADC_SUM; t += ADC_CH_NUM )         /* 获取ADC_SUM次数据 */
    {
        temp_val += g_adc_value[t];
    }

    return temp_val / ADC_COLL;                         /* 返回平均值 */
}

