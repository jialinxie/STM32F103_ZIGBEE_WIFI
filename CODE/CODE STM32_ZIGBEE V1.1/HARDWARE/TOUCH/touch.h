#ifndef __TOUCH_H__
#define __TOUCH_H__
#include "sys.h"


//按键状态	 
#define Key_Down 0x01
#define Key_Up   0x00 
//笔杆结构体
typedef struct 
{
	u16 X0;//原始坐标
	u16 Y0;
	u16 X; //最终/暂存坐标
	u16 Y;						   	    
	u8  Key_Sta;//笔的状态			  
//触摸屏校准参数
	float xfac;
	float yfac;
	short xoff;
	short yoff;
//新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
//touchtype=0的时候,适合左右为X坐标,上下为Y坐标的TP.
//touchtype=1的时候,适合左右为Y坐标,上下为X坐标的TP.
	u8 touchtype;
}Pen_Holder;	   

extern Pen_Holder Pen_Point;

//与触摸屏芯片连接引脚	   
#define PEN  PBin(7)   //PB7  INT
#define DOUT PBin(6)   //PB6  MISO
#define TDIN PBout(5)  //PB5  MOSI
#define TCLK PBout(3)  //PB3  SCLK
#define TCS  PBout(4)  //PB4 CS    

//ADS7843/7846/UH7843/7846/XPT2046/TSC2046 指令集
//#define CMD_RDX   0X90  //0B10010000即用差分方式读X坐标
//#define CMD_RDY	0XD0  //0B11010000即用差分方式读Y坐标
extern u8 CMD_RDX;
extern u8 CMD_RDY;
   											 
#define TEMP_RD	0XF0  //0B11110000即用差分方式读Y坐标    
//使用保存
//#define ADJ_SAVE_ENABLE	    
			  
void Touch_Init(void);		 //初始化
u8 Read_ADS(u16 *x,u16 *y);	 //带舍弃的双方向读取
u8 Read_ADS2(u16 *x,u16 *y); //带加强滤波的双方向坐标读取
u16 ADS_Read_XY(u8 xy);		 //带滤波的坐标读取(单方向)
u16 ADS_Read_AD(u8 CMD);	 //读取AD转换值
void ADS_Write_Byte(u8 num); //向控制芯片写入一个数据
void Drow_Touch_Point(u8 x,u16 y);//画一个坐标叫准点
void Draw_Big_Point(u8 x,u16 y);  //画一个大点
void Touch_Adjust(void);          //触摸屏校准
void Save_Adjdata(void);		  //保存校准参数
u8 Get_Adjdata(void); 			  //读取校准参数
void Pen_Int_Set(u8 en); 		  //PEN中断使能/关闭
void Convert_Pos(void);           //结果转换函数	 
#endif

















