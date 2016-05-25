#include "fontupd.h"
#include "ff.h"	  
#include "flash.h"   
#include "lcd.h"  
#include "delay.h"


//字库信息结构体. 
_font_info ftinfo=
{
	FONTINFOADDR+100000,  					
	0,
	0,
	0,
};

//在sd卡中的路径
const u8 *GBK16_SDPATH="0:/SYSTEM/FONT/GBK16.FON";		//GBK16的存放位置
const u8 *UNIGBK_SDPATH="0:/SYSTEM/FONT/UNI2GBK.SYS";	//UNIGBK.BIN的存放位置

FIL gbkFile;
u8 temp[1024];
//显示当前字体更新进度
u32 fupd_prog(u32 fsize,u32 pos)
{
	float prog;
	u8 t=0XFF;
	prog=(float)pos/fsize;
	prog*=100;
	if(t!=prog)
	{
		LCD_ShowString(108,110,"%");		
		t=prog;
		if(t>100)t=100;
		LCD_ShowNum(84,110,t,3,16);//显示数值
	}
	return 0;					    
}

//更新某一个
//fxpath:路径
//fx:更新的内容 0,ungbk;1 ,gbk16;
//返回值:0,成功;其他,失败.
u8 updata_fontx(u8 *fxpath,u8 fx)
{
	u32 flashaddr=0;
 	u32 tempsys[2];  //临时记录文件起始位置和文件大小
	FIL * fftemp;
 	u8 res;	
	u16 bread;
	u32 offx=0;
	fftemp=&gbkFile;
	if(fftemp==NULL)return 1;			//内存分配出错.
 	res=f_open(fftemp,(const TCHAR*)fxpath,FA_READ); 
 	if(res)//打开文件失败
	{
		f_close(fftemp);
		return 2;//文件丢失
	}
	if(fftemp==NULL)return 2;

	if(fx==0)//更新UNI2GBK.SYS
	{
 		tempsys[0]=ftinfo.ugbkaddr;							//UNIGBK存放地址
		tempsys[1]=fftemp->fsize;	 	  					//UNIGBK 大小
		SPI_Flash_Write((u8*)tempsys,FONTINFOADDR,8);		//记录在地址FONTINFOADDR+1~8处
		flashaddr=ftinfo.ugbkaddr;
	}else//GBK16
	{
		SPI_Flash_Read((u8*)tempsys,FONTINFOADDR,8);    //读出8个字节   
		flashaddr=tempsys[0]+tempsys[1];
		tempsys[0]=flashaddr;								//GBK16的起始地址
		tempsys[1]=fftemp->fsize;	 	  					//GBK16 大小
		SPI_Flash_Write((u8*)tempsys,FONTINFOADDR+8,8);	//记录在地址FONTINFOADDR+1~8+16处
	}	   
	while(res==FR_OK)//死循环执行
	{
 	 	res=f_read(fftemp,temp,1024,(UINT *)&bread);	
		if(res!=FR_OK)break;				//执行错误
		SPI_Flash_Write(temp,offx+flashaddr,1024);//从0开始写入1024个数据  
  		offx+=bread;	  
		fupd_prog(fftemp->fsize,offx);			//进度显示
		if(bread!=1024)break;					//读完了.
 	} 
	return res;
}

//更新字体文件,UNIGBK,GBK16一起更新								  
//返回值:0,更新成功;
//		 其他,错误代码.	  
u8 update_font()
{	
	u8 *gbk16_path;
	u8 *unigbk_path;
	u8 res;
	unigbk_path=(u8*)UNIGBK_SDPATH;
	gbk16_path=(u8*)GBK16_SDPATH;

	POINT_COLOR=RED;	 
	LCD_ShowString(60,90,"Updating UNI2GBK.SYS");		
	res=updata_fontx(unigbk_path,0);//更新UNIGBK.BIN
	if(res)return 1;
	LCD_ShowString(60,90,"Updating GBK16.FON  ");
	res=updata_fontx(gbk16_path,1);//更新GBK16.FON
	if(res)return 2;	   
	//全部更新好了
	res=0XAA;
   	SPI_Flash_Write(&res,FONTINFOADDR+24,1);//写入字库存在标志	0XAA
	return 0;//无错误.		 
}


//初始化字体
//返回值:0,字库完好.
//其他,字库丢失
u8 font_init(void)
{			  
	u32 tempsys[2];//临时记录文件起始位置和文件大小

	u8 fontok=0;

 	SPI_Flash_Read(&fontok,FONTINFOADDR+24,1);//读出字库标志位,看是否存在字库

	if(fontok!=0XAA)return 1;//字库错误.FLASH不存在字库
 	SPI_Flash_Read((u8*)tempsys,FONTINFOADDR,8);//读出8个字节   

	ftinfo.ugbksize=tempsys[1];	//UNIGBK的大小

	SPI_Flash_Read((u8*)tempsys,FONTINFOADDR+8,8);//读出8个字节   

	ftinfo.f16addr=tempsys[0];	//FONT16的起始地址

	return 0;		    
}





























