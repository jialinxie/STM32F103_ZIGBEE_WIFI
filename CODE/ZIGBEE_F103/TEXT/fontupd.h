#ifndef __FONTUPD_H__
#define __FONTUPD_H__	 
#include "sys.h"

														   
 
#define FONTINFOADDR	0	//字体信息保存地址,占25个字节,第24个字节用于标记字库是否存在.
//按钮结构体定义
__packed typedef struct 
{
	const u32 ugbkaddr; 	//unigbk的地址
	u32 f16addr;			//gbk16地址
	u32 f12addr;			//gbk12地址	
	u32 ugbksize;			//unigbk的大小	 
}_font_info;

extern _font_info ftinfo;	//字库信息结构体


u8 update_font(void);//更新字库   
u8 font_init(void);//初始化字库
#endif





















