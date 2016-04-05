#ifndef __LCD_H
#define __LCD_H		
#include "sys.h"	 
#include "stdlib.h"

/////////////////////////////////////用户配置区///////////////////////////////////	 
//以下2个宏定义，定义屏幕的显示方式及IO速度
#define USE_HORIZONTAL  0	//定义是否使用横屏 		0,不使用.1,使用.
 
//TFTLCD部分外要调用的函数		   
extern u16  POINT_COLOR;//默认红色    
extern u16  BACK_COLOR; //背景颜色.默认为白色
//定义LCD的尺寸
#if USE_HORIZONTAL==1	//使用横屏
#define LCD_W 400
#define LCD_H 240
#else
#define LCD_W 240
#define LCD_H 400
#endif
////////////////////////////////////////////////////////////////////
//-----------------LCD端口定义---------------- 
#define	LCD_LED PCout(13) //LCD背光    		 PC13  

//操作频繁，为了提高速度，建议直接寄存器操作

#define	LCD_CS_SET  GPIOA->BSRR=1<<0    //片选端口  	 Pa0
#define	LCD_RS_SET	GPIOA->BSRR=1<<1    //数据/命令    Pa1	   
#define	LCD_WR_SET	GPIOB->BSRR=1<<0    //写数据			 Pb0
#define	LCD_RD_SET	GPIOC->BSRR=1<<13    //读数据			 Pa13
								    
#define	LCD_CS_CLR  GPIOA->BRR=1<<0     //片选端口  	 Pa0
#define	LCD_RS_CLR	GPIOA->BRR=1<<1     //数据/命令    Pa1	   
#define	LCD_WR_CLR	GPIOB->BRR=1<<0     //写数据			 Pb0
#define	LCD_RD_CLR	GPIOC->BRR=1<<13     //读数据			 Pa13
							    

//PB8~15,作为数据线
//在这里，因为操作频繁，为了提高速度，建议直接用寄存器操作。
#define DATAOUT(x) GPIOB->ODR=((GPIOB->ODR&0x00ff)|x); //数据输出
#define DATAIN     GPIOB->IDR   //数据输入


//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	   0x001F  
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //棕色
#define BRRED 			     0XFC07 //棕红色
#define GRAY  			     0X8430 //灰色
//GUI颜色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
#define LIGHTGREEN     	 0X841F //浅绿色
#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY 			     0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)
	    		
					
extern u16 BACK_COLOR, POINT_COLOR ; 


void LCD_Init(void);
void LCD_Clear(u16 Color);	 
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_DrawPoint(u16 x,u16 y);//画点
u16  LCD_ReadPoint(u16 x,u16 y); //读点
void Draw_Circle(u16 x0,u16 y0,u8 r);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);		   
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);//显示一个字符
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);  //显示一个数字
void LCD_Show2Num(u16 x,u16 y,u16 num,u8 len,u8 size,u8 mode);//显示2个数字
void LCD_ShowString(u16 x,u16 y,const u8 *p);		 //显示一个字符串,16字体
									    
u16 LCD_ReadReg(u16 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
u16 LCD_ReadRAM(void);		   

 
#define LCD_WR_DATA(data){\
LCD_RS_SET;\
LCD_CS_CLR;\
DATAOUT(data&0xff00);\
LCD_WR_CLR;\
LCD_WR_SET;\
DATAOUT((0x00ff&data)<<8);\
LCD_WR_CLR;\
LCD_WR_SET;\
LCD_CS_SET;\
} 

#define LCD_INIT_DATA(data){\
LCD_RS_SET;\
LCD_CS_CLR;\
DATAOUT((0x00ff&data)<<8);\
LCD_WR_CLR;\
LCD_WR_SET;\
LCD_CS_SET;\
} 
				  		 
#endif  
	 
