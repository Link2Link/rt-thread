#include "rtthread.h"
#include "adc_code.h"
#include "math.h"
#define TEST_PIN2 GET_PIN(I, 10)

DMA_HandleTypeDef g_dma_nch_adc_handle = {0};                   /* 定义要搬运ADC多通道数据的DMA句柄 */
ADC_HandleTypeDef g_adc_nch_dma_handle = {0};                   /* 定义ADC（多通道DMA读取）句柄 */
uint8_t g_adc_dma_sta = 0;                                                  /* DMA传输状态标志, 0,未完成; 1, 已完成 */
uint16_t g_adc_value[ADC_SUM] = {0};                            /* 存储ADC原始值 */




void adc_nch_dma_init(void)
{
    rt_pin_mode(TEST_PIN2, PIN_MODE_OUTPUT);
    rt_pin_write(TEST_PIN2, PIN_LOW);

    GPIO_InitTypeDef gpio_init_struct;
    ADC_ChannelConfTypeDef sConfig = {0};

    ADC_ADCX_CHY_CLK_ENABLE();                                              /* 使能ADCx时钟 */
    ADC_ADCX_CH0_GPIO_CLK_ENABLE();
    ADC_ADCX_CH1_GPIO_CLK_ENABLE();
    ADC_ADCX_CH2_GPIO_CLK_ENABLE();

    if ((uint32_t)ADC_ADCX_DMASx > (uint32_t)DMA2)                          /* 大于DMA2的基地址, 则为DMA2的数据流通道了 */
    {
        __HAL_RCC_DMA2_CLK_ENABLE();                                        /* DMA2时钟使能 */
    }
    else 
    {
        __HAL_RCC_DMA1_CLK_ENABLE();                                        /* DMA1时钟使能 */
    }

    /* 设置AD采集通道对应IO引脚工作模式 */
    gpio_init_struct.Pin = ADC_ADCX_CH0_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;
    gpio_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(ADC_ADCX_CH0_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = ADC_ADCX_CH1_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;
    gpio_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(ADC_ADCX_CH1_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = ADC_ADCX_CH2_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;
    gpio_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(ADC_ADCX_CH2_GPIO_PORT, &gpio_init_struct);
    
    /* 初始化DMA */
    g_dma_nch_adc_handle.Instance = ADC_ADCX_DMASx;                             /* 设置DMA数据流 */
    g_dma_nch_adc_handle.Init.Channel = DMA_CHANNEL_0;                          /* 设置DMA通道 */
    g_dma_nch_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;                 /* 从外设到存储器模式 */
    g_dma_nch_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;                     /* 外设非增量模式 */
    g_dma_nch_adc_handle.Init.MemInc = DMA_MINC_ENABLE;                         /* 存储器增量模式 */
    g_dma_nch_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;    /* 外设数据长度:16位 */
    g_dma_nch_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;       /* 存储器数据长度:16位 */
    g_dma_nch_adc_handle.Init.Mode = DMA_NORMAL;                                /* 外设流控模式 */
    g_dma_nch_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                   /* 中等优先级 */
    HAL_DMA_Init(&g_dma_nch_adc_handle);
    
    g_adc_nch_dma_handle.Instance = ADC_ADCX;
    g_adc_nch_dma_handle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;    /* 4分频，21Mhz */
    g_adc_nch_dma_handle.Init.Resolution = ADC_RESOLUTION_12B;                  /* 12位模式 */
    g_adc_nch_dma_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                  /* 右对齐 */
    g_adc_nch_dma_handle.Init.ScanConvMode = ENABLE;                           /* 非扫描模式 */
    g_adc_nch_dma_handle.Init.ContinuousConvMode = ENABLE;                      /* 开启连续转换 */
    g_adc_nch_dma_handle.Init.NbrOfConversion = ADC_CH_NUM;                     /* 本实验用到1个规则通道序列 */
    g_adc_nch_dma_handle.Init.DiscontinuousConvMode = DISABLE;                  /* 禁止不连续采样模式 */
    g_adc_nch_dma_handle.Init.NbrOfDiscConversion = 0;                          /* 不连续采样通道数为0 */
    g_adc_nch_dma_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;            /* 软件触发 */
    g_adc_nch_dma_handle.Init.DMAContinuousRequests = ENABLE;                   /* 开启DMA请求 */
    HAL_ADC_Init(&g_adc_nch_dma_handle);                                        /* 初始化ADC */
    
    __HAL_LINKDMA(&g_adc_nch_dma_handle, DMA_Handle, g_dma_nch_adc_handle);         /* 把ADC和DMA连接起来 */
    
    /* 配置ADC通道 */
    sConfig.Channel = ADC_ADCX_CH0;                                     /* 通道 */
    sConfig.Rank = 1;                                                   /* 序列 */
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;                    /* 采样时间 */
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &sConfig);                 /* 通道配置 */

    sConfig.Channel = ADC_ADCX_CH1;                                     /* 通道 */
    sConfig.Rank = 2;                                                   /* 序列 */
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;                    /* 采样时间 */
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &sConfig);                 /* 通道配置 */

    sConfig.Channel = ADC_ADCX_CH2;                                     /* 通道 */
    sConfig.Rank = 3;                                                   /* 序列 */
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;                    /* 采样时间 */
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &sConfig);                 /* 通道配置 */

    /* 配置DMA数据流请求中断优先级 */
    HAL_NVIC_SetPriority(ADC_ADCX_DMASx_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(ADC_ADCX_DMASx_IRQn);
    HAL_ADC_Stop_DMA(&g_adc_nch_dma_handle); 
    
}


void ADC_ADCX_DMASx_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_dma_nch_adc_handle);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    HAL_ADC_Stop_DMA(hadc);                             /* 关闭DMA传输 */
    rt_sem_release(sem_adc_done);
    rt_pin_write(TEST_PIN2, PIN_LOW);
}

HAL_StatusTypeDef HAL_ADC_Start_DMA_Mine(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length);
void adc_nch_dma_start(void)
{
    rt_pin_write(TEST_PIN2, PIN_HIGH);
    HAL_ADC_Start_DMA_Mine(&g_adc_nch_dma_handle, (uint32_t*)&g_adc_value, ADC_SUM);

}

uint32_t adc_get_result_average(uint8_t ch)
{
    uint32_t temp_val = 0;
    uint16_t t;

    for (t = ch; t < ADC_SUM; t += ADC_CH_NUM )         /* 获取ADC_SUM次数据 */
    {
        temp_val += g_adc_value[t];
    }

    return temp_val >> ADC_IDX;                         /* 返回平均值 */
}




float get_temp(uint16_t para)
{

    const float Rp = 10000.0f;          /* 10K */
    const float T2 = (273.15f + 25.0f); /* T2 */
    const float Bx = 3380.0f;           /* B */
    const float Ka = 273.15f;

    float Rt;
    float temp;
    
    /* 
        第一步：
        Rt = 3.3 * 4700 / VTEMP - 4700 ,其中VTEMP就是温度检测通道采集回来的电压值,VTEMP = ADC值* 3.3/4096
        由此我们可以计算出当前Rt的值：Rt = 3.3f * 4700.0f / (para * 3.3f / 4096.0f ) - 4700.0f; 
    */
    
    Rt = 3.3f * 4700.0f / (para * 3.3f / 4096.0f ) - 4700.0f;       /* 根据当前ADC值计算出Rt的值 */

    /* 
        第二步：
        根据当前Rt的值来计算对应温度值：Rt = Rp *exp(B*(1/T1-1/T2)) 
    */
    
    temp = Rt / Rp;                 /* 解出exp(B*(1/T1-1/T2)) ，即temp = exp(B*(1/T1-1/T2)) */
    temp = log(temp);               /* 解出B*(1/T1-1/T2) ，即temp = B*(1/T1-1/T2) */
    temp /= Bx;                     /* 解出1/T1-1/T2 ，即temp = 1/T1-1/T2 */
    temp += (1.0f / T2);            /* 解出1/T1 ，即temp = 1/T1 */
    temp = 1.0f / (temp);           /* 解出T1 ，即temp = T1 */
    temp -= Ka;                     /* 计算T1对应的摄氏度 */
    return temp;                    /* 返回温度值 */
}


float get_busVoltage(uint16_t para)
{
    return ((float)para)*ADC2VBUS;
}


float get_motorCurrent(uint16_t para, int OffsetCNT)
{
    int x = para - OffsetCNT;
    return ((float)x)*ADC2CURT;  //单位mA
}


/** HAL_ADC_Start_DMA **/


static void ADC_DMAConvCplt_Mine(DMA_HandleTypeDef *hdma)   
{
  /* Retrieve ADC handle corresponding to current DMA handle */
  ADC_HandleTypeDef* hadc = ( ADC_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  
  /* Update state machine on conversion status if not in error state */
  if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL | HAL_ADC_STATE_ERROR_DMA))
  {
    /* Update ADC state machine */
    SET_BIT(hadc->State, HAL_ADC_STATE_REG_EOC);
    
    /* Determine whether any further conversion upcoming on group regular   */
    /* by external trigger, continuous mode or scan sequence on going.      */
    /* Note: On STM32F4, there is no independent flag of end of sequence.   */
    /*       The test of scan sequence on going is done either with scan    */
    /*       sequence disabled or with end of conversion flag set to        */
    /*       of end of sequence.                                            */
    if(ADC_IS_SOFTWARE_START_REGULAR(hadc)                   &&
       (hadc->Init.ContinuousConvMode == DISABLE)            &&
       (HAL_IS_BIT_CLR(hadc->Instance->SQR1, ADC_SQR1_L) || 
        HAL_IS_BIT_CLR(hadc->Instance->CR2, ADC_CR2_EOCS)  )   )
    {
      /* Disable ADC end of single conversion interrupt on group regular */
      /* Note: Overrun interrupt was enabled with EOC interrupt in          */
      /* HAL_ADC_Start_IT(), but is not disabled here because can be used   */
      /* by overrun IRQ process below.                                      */
      __HAL_ADC_DISABLE_IT(hadc, ADC_IT_EOC);
      
      /* Set ADC state */
      CLEAR_BIT(hadc->State, HAL_ADC_STATE_REG_BUSY);   
      
      if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_INJ_BUSY))
      {
        SET_BIT(hadc->State, HAL_ADC_STATE_READY);
      }
    }
    
    /* Conversion complete callback */
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
    hadc->ConvCpltCallback(hadc);
#else
    HAL_ADC_ConvCpltCallback(hadc);
#endif /* USE_HAL_ADC_REGISTER_CALLBACKS */
  }
  else /* DMA and-or internal error occurred */
  {
    if ((hadc->State & HAL_ADC_STATE_ERROR_INTERNAL) != 0UL)
    {
      /* Call HAL ADC Error Callback function */
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
      hadc->ErrorCallback(hadc);
#else
      HAL_ADC_ErrorCallback(hadc);
#endif /* USE_HAL_ADC_REGISTER_CALLBACKS */
    }
	else
	{
      /* Call DMA error callback */
      hadc->DMA_Handle->XferErrorCallback(hdma);
    }
  }
}


extern void ADC_DMAConvCplt(DMA_HandleTypeDef *hdma);
extern void ADC_DMAHalfConvCplt(DMA_HandleTypeDef *hdma);
extern void ADC_DMAError(DMA_HandleTypeDef *hdma);

HAL_StatusTypeDef HAL_ADC_Start_DMA_Mine(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length)
{
  __IO uint32_t counter = 0U;
  ADC_Common_TypeDef *tmpADC_Common;
  
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(hadc->Init.ContinuousConvMode));
  assert_param(IS_ADC_EXT_TRIG_EDGE(hadc->Init.ExternalTrigConvEdge)); 
  
  /* Process locked */
  __HAL_LOCK(hadc);
  
  /* Enable the ADC peripheral */
  /* Check if ADC peripheral is disabled in order to enable it and wait during 
  Tstab time the ADC's stabilization */
  if((hadc->Instance->CR2 & ADC_CR2_ADON) != ADC_CR2_ADON)
  {  
    /* Enable the Peripheral */
    __HAL_ADC_ENABLE(hadc);
    
    /* Delay for ADC stabilization time */
    /* Compute number of CPU cycles to wait for */
    // counter = (ADC_STAB_DELAY_US * (SystemCoreClock / 1000000U));
    // while(counter != 0U)
    // {
    //   counter--;
    // }
    udelay(ADC_STAB_DELAY_US);
  }
  
  /* Check ADC DMA Mode                                                     */
  /* - disable the DMA Mode if it is already enabled                        */
  if((hadc->Instance->CR2 & ADC_CR2_DMA) == ADC_CR2_DMA)
  {
    CLEAR_BIT(hadc->Instance->CR2, ADC_CR2_DMA);
  }
  
  /* Start conversion if ADC is effectively enabled */
  if(HAL_IS_BIT_SET(hadc->Instance->CR2, ADC_CR2_ADON))
  {
    /* Set ADC state                                                          */
    /* - Clear state bitfield related to regular group conversion results     */
    /* - Set state bitfield related to regular group operation                */
    ADC_STATE_CLR_SET(hadc->State,
                      HAL_ADC_STATE_READY | HAL_ADC_STATE_REG_EOC | HAL_ADC_STATE_REG_OVR,
                      HAL_ADC_STATE_REG_BUSY);
    
    /* If conversions on group regular are also triggering group injected,    */
    /* update ADC state.                                                      */
    if (READ_BIT(hadc->Instance->CR1, ADC_CR1_JAUTO) != RESET)
    {
      ADC_STATE_CLR_SET(hadc->State, HAL_ADC_STATE_INJ_EOC, HAL_ADC_STATE_INJ_BUSY);  
    }
    
    /* State machine update: Check if an injected conversion is ongoing */
    if (HAL_IS_BIT_SET(hadc->State, HAL_ADC_STATE_INJ_BUSY))
    {
      /* Reset ADC error code fields related to conversions on group regular */
      CLEAR_BIT(hadc->ErrorCode, (HAL_ADC_ERROR_OVR | HAL_ADC_ERROR_DMA));         
    }
    else
    {
      /* Reset ADC all error code fields */
      ADC_CLEAR_ERRORCODE(hadc);
    }

    /* Process unlocked */
    /* Unlock before starting ADC conversions: in case of potential           */
    /* interruption, to let the process to ADC IRQ Handler.                   */
    __HAL_UNLOCK(hadc);   

    /* Pointer to the common control register to which is belonging hadc    */
    /* (Depending on STM32F4 product, there may be up to 3 ADCs and 1 common */
    /* control register)                                                    */
    tmpADC_Common = ADC_COMMON_REGISTER(hadc);

    /* Set the DMA transfer complete callback */
    hadc->DMA_Handle->XferCpltCallback = ADC_DMAConvCplt_Mine;

    // /* Set the DMA half transfer complete callback */
    // hadc->DMA_Handle->XferHalfCpltCallback = ADC_DMAHalfConvCplt;
    
    // /* Set the DMA error callback */
    // hadc->DMA_Handle->XferErrorCallback = ADC_DMAError;

    
    /* Manage ADC and DMA start: ADC overrun interruption, DMA start, ADC     */
    /* start (in case of SW start):                                           */
    
    /* Clear regular group conversion flag and overrun flag */
    /* (To ensure of no unknown state from potential previous ADC operations) */
    __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_EOC | ADC_FLAG_OVR);

    /* Enable ADC overrun interrupt */
    __HAL_ADC_ENABLE_IT(hadc, ADC_IT_OVR);
    
    /* Enable ADC DMA mode */
    hadc->Instance->CR2 |= ADC_CR2_DMA;
    
    /* Start the DMA channel */
    HAL_DMA_Start_IT(hadc->DMA_Handle, (uint32_t)&hadc->Instance->DR, (uint32_t)pData, Length);
    
    /* Check if Multimode enabled */
    if(HAL_IS_BIT_CLR(tmpADC_Common->CCR, ADC_CCR_MULTI))
    {
#if defined(ADC2) && defined(ADC3)
      if((hadc->Instance == ADC1) || ((hadc->Instance == ADC2) && ((ADC->CCR & ADC_CCR_MULTI_Msk) < ADC_CCR_MULTI_0)) \
                                  || ((hadc->Instance == ADC3) && ((ADC->CCR & ADC_CCR_MULTI_Msk) < ADC_CCR_MULTI_4)))
      {
#endif /* ADC2 || ADC3 */
        /* if no external trigger present enable software conversion of regular channels */
        if((hadc->Instance->CR2 & ADC_CR2_EXTEN) == RESET) 
        {
          /* Enable the selected ADC software conversion for regular group */
          hadc->Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
        }
#if defined(ADC2) && defined(ADC3)
      }
#endif /* ADC2 || ADC3 */
    }
    else
    {
      /* if instance of handle correspond to ADC1 and  no external trigger present enable software conversion of regular channels */
      if((hadc->Instance == ADC1) && ((hadc->Instance->CR2 & ADC_CR2_EXTEN) == RESET))
      {
        /* Enable the selected ADC software conversion for regular group */
          hadc->Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
      }
    }
  }
  else
  {
    /* Update ADC state machine to error */
    SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL);

    /* Set ADC error code to ADC IP internal error */
    SET_BIT(hadc->ErrorCode, HAL_ADC_ERROR_INTERNAL);
  }
  
  /* Return function status */
  return HAL_OK;
}