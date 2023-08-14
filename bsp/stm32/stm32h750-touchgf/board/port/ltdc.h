#ifndef _LTDC_MINE_H_
#define _LTDC_MINE_H_

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "stm32h7xx_hal.h"



//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色
//#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

#define LCD_FRAME_BUF_ADDR 0X24000000
#define LCD_W 480
#define LCD_H 272

typedef struct  
{							 
	uint32_t pwidth;			//LCD面板的宽度,固定参数,不随显示方向改变,如果为0,说明没有任何RGB屏接入
	uint32_t pheight;		//LCD面板的高度,固定参数,不随显示方向改变
	uint16_t hsw;			//水平同步宽度
	uint16_t vsw;			//垂直同步宽度
	uint16_t hbp;			//水平后廊
	uint16_t vbp;			//垂直后廊
	uint16_t hfp;			//水平前廊
	uint16_t vfp;			//垂直前廊 
	uint8_t activelayer;		//当前层编号:0/1	
	uint8_t dir;				//0,竖屏;1,横屏;
	uint16_t width;			//LCD宽度
	uint16_t height;			//LCD高度
	uint32_t pixsize;		//每个像素所占字节数
}_ltdc_dev; 


void ltdc_init(void);
void ltdc_draw_point(uint16_t x, uint16_t y, uint32_t color);
void ltdc_draw_point2(uint16_t x, uint16_t y, uint16_t * color);
uint32_t ltdc_read_point(uint16_t x, uint16_t y);
void ltdc_clear(uint32_t color);

#endif

