

#include "ltdc.h"


LTDC_HandleTypeDef hltdc;
_ltdc_dev lcdltdc;
uint16_t framebuf[LCD_W][LCD_H] __attribute__((at(LCD_FRAME_BUF_ADDR)));

void ltdc_layer1_config(void);
void ltdc_init(void)
{
    lcdltdc.pwidth = LCD_W;
    lcdltdc.pheight = LCD_H;
    lcdltdc.hsw = 1;			//水平同步宽度
    lcdltdc.vsw = 3;			//垂直同步宽度
    lcdltdc.hbp = 40;			//水平后廊
    lcdltdc.vbp = 8;			//垂直后廊
    lcdltdc.hfp = 5;	   		//水平前廊
    lcdltdc.vfp = 8;			//垂直前廊 
    lcdltdc.dir = 1;    //0,竖屏;1,横屏;
    lcdltdc.width = LCD_W;
    lcdltdc.height = LCD_H;
    lcdltdc.pixsize = 2;

    hltdc.Instance = LTDC;
    hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL; 
    hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    hltdc.Init.HorizontalSync = lcdltdc.hsw-1;
    hltdc.Init.VerticalSync = lcdltdc.vsw-1;
    hltdc.Init.AccumulatedHBP = lcdltdc.hsw + lcdltdc.hbp - 1;
    hltdc.Init.AccumulatedVBP = lcdltdc.vsw + lcdltdc.vbp - 1;
    hltdc.Init.AccumulatedActiveW = lcdltdc.hsw + lcdltdc.hbp + lcdltdc.pwidth - 1;
    hltdc.Init.AccumulatedActiveH = lcdltdc.vsw + lcdltdc.vbp + lcdltdc.pheight - 1;
    hltdc.Init.TotalWidth = lcdltdc.hsw + lcdltdc.hbp + lcdltdc.pwidth + lcdltdc.hfp - 1;
    hltdc.Init.TotalHeigh = lcdltdc.vsw + lcdltdc.vbp + lcdltdc.pheight + lcdltdc.vfp - 1;
    hltdc.Init.Backcolor.Blue = 0xFF;
    hltdc.Init.Backcolor.Green = 0xFF;
    hltdc.Init.Backcolor.Red = 0xFF;
    if (HAL_LTDC_Init(&hltdc) != HAL_OK)
    {
      Error_Handler();
    }


    ltdc_clear(WHITE);
    ltdc_layer1_config();

}

void ltdc_layer1_config(void)
{
    LTDC_LayerCfgTypeDef pLayerCfg = {0};

    //区域
    pLayerCfg.WindowX0 = 0;
    pLayerCfg.WindowX1 = lcdltdc.pwidth;
    pLayerCfg.WindowY0 = 0;
    pLayerCfg.WindowY1 = lcdltdc.pheight;
    
    //像素格式
    pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
    
    //层混合因数
    pLayerCfg.Alpha = 255;
    pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
    pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
    
    //层缓冲设置
    pLayerCfg.FBStartAdress = (uint32_t)framebuf;
    pLayerCfg.ImageWidth = lcdltdc.pwidth;
    pLayerCfg.ImageHeight = lcdltdc.pheight;

    //层背景
    pLayerCfg.Alpha0 = 0;
    pLayerCfg.Backcolor.Blue = 0;
    pLayerCfg.Backcolor.Green = 0;
    pLayerCfg.Backcolor.Red = 0;
    if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
    {
      Error_Handler();
    }

    // LTDC_LayerCfgTypeDef pLayerCfg1 = {0};
    // pLayerCfg1.WindowX0 = 0;
    // pLayerCfg1.WindowX1 = 0;
    // pLayerCfg1.WindowY0 = 0;
    // pLayerCfg1.WindowY1 = 0;
    // pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
    // pLayerCfg1.Alpha = 0;
    // pLayerCfg1.Alpha0 = 0;
    // pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
    // pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
    // pLayerCfg1.FBStartAdress = 0;
    // pLayerCfg1.ImageWidth = 0;
    // pLayerCfg1.ImageHeight = 0;
    // pLayerCfg1.Backcolor.Blue = 0;
    // pLayerCfg1.Backcolor.Green = 0;
    // pLayerCfg1.Backcolor.Red = 0;
    // if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 1) != HAL_OK)
    // {
    //   Error_Handler();
    // }

}

void ltdc_draw_point(uint16_t x, uint16_t y, uint32_t color)
{
    *(uint16_t *)((uint32_t)framebuf + lcdltdc.pixsize * (lcdltdc.pwidth * y + x)) = color;
}

void ltdc_draw_point2(uint16_t x, uint16_t y, uint16_t * color)
{
    *(uint16_t *)((uint32_t)framebuf + lcdltdc.pixsize * (lcdltdc.pwidth * y + x)) = *color;
}



uint32_t ltdc_read_point(uint16_t x, uint16_t y)
{
      return *(uint16_t *)((uint32_t)framebuf[x] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x));
}

void ltdc_clear(uint32_t color)
{
    for (int i = 0; i < LCD_W; i++)
    {
      for (int j = 0; j < LCD_H; j++)
      {
        framebuf[i][j] = color;
      }
    }
}