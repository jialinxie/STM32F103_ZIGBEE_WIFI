

//#include "stm32f10x.h"
#include "tm1638.h"
#include "delay.h"
//#include "gpio.h"

//TM1638 STB接P9  CLK接PB4    DIO接PB8
////TM1638 STB接PA4  CLK接PA5    DIO接PA6
//#define STB PCout(13)// PC13
/*
 _________________________________________________
| 			|				|				|				|					|				|
|2BYTE	|4BYTE	| 2BYTE | 1BYTE |	32BYTE	| 1BYTE	|
|_______|_______|_______|_______|_________|_______|
| 			|				|				|				|					|				|
|  帧头	|8LED		| LED		| 点阵位|点阵数据 | 检验位|
|_______|_______|_______|_______|_________|_______|
*/
u8  rec_frame[42];

void delay_tim(unsigned char tim)
{
	unsigned char i;
	for(;tim>0;tim--)
	 for(i=0;i<250;i++)
	{;}
}
//读数据延时函数
static Delay_1us(int counter)
{
	int i,j;
	for(i=0;i<counter;i++)
	  for(j=0;j<33;j++);
}
//old
//PA6-->DIO
//PA5-->CLK
//PA4-->STB
//new
//PA8-->DIO
//PB1-->CLK
//PB2-->STB


void DIO_SETOUT() //配置TM1638DIO引脚为推挽输出
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DIO_RCC, ENABLE);	 //使能PF端口时钟
	GPIO_InitStructure.GPIO_Pin = DIO_GPIO_PIN;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(DIO_GPIO, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB
	DIO_OUT(1);
}
void  DIO_SETIN()  //配置TM1638DIO引脚为上拉输入
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DIO_RCC, ENABLE);	 //使能PF端口时钟
	GPIO_InitStructure.GPIO_Pin = DIO_GPIO_PIN;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(DIO_GPIO, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB
	
//	GPIO_SetBits(GPIOA,GPIO_Pin_4);					//初始化设置为0
	
}
void TM1638_Write(unsigned char	DATA)			//写数据函数 Q
{
   unsigned char i;
	
   DIO_SETOUT(); 		
   for(i=0;i<8;i++)
   {
   	//CLK=0
		 CLK_OUT(0);
	  if(DATA&0X01)
		{
	    //DIO=1;
			DIO_OUT(1);
		}
	  else 
		{
	    //DIO=0;
			DIO_OUT(0);
		}
	  DATA>>=1;
	  //CLK=1;
		CLK_OUT(1);
	  delay_tim(10);
   }
	 
}
unsigned char TM1638_Read(void)					//读数据函数 Q
{
	unsigned char i;
	unsigned char temp=0;
	DIO_SETIN();
	delay_tim(1);
	//DIO=1;
	for(i=0;i<8;i++)
	{
	  temp>>=1;
	 // CLK=0
		CLK_OUT(0);
		delay_tim(10);
		if(DIO_IN() == Bit_SET)
	    temp|=0x80;
	  //CLK=1
		CLK_OUT(1);
		
		delay_tim(1);
	}
	return temp;
}

void Write_COM(unsigned char cmd)		//发送命令字Q
{ 
	//STB=0;
  STB_OUT(0);
	TM1638_Write(cmd);
	//STB=1;
	STB_OUT(1);
}

unsigned char c[4];
unsigned char Read_key(void)
{
	unsigned char i,key_value=0;
	unsigned char tmp_num[4]={0,0,0,0};
//	int* key;
  // STB=0;
	STB_OUT(0);
	TM1638_Write(0x42);
	delay_tim(200);
	
	for(i=0;i<4;i++)
	{
		c[i]=TM1638_Read();
	}
	//STB=1;				//4个字节数据合成一个字节
	STB_OUT(1);
	
		for(i=0;i<4;i++)
		{
			if(c[i]!=0)
				switch(c[i])
			{
				case 0x01:tmp_num[i]=i*8+1; break;
				case 0x10:tmp_num[i]=i*8+2; break;
				case 0x02:tmp_num[i]=i*8+3; break;
				case 0x20:tmp_num[i]=i*8+4; break;
				case 0x04:tmp_num[i]=i*8+5; break;
				case 0x40:tmp_num[i]=i*8+6; break;
				case 0x08:tmp_num[i]=i*8+7; break;
				case 0x80:tmp_num[i]=i*8+8; break;
				default : tmp_num[i]=0;break;
			}
		}
		key_value=tmp_num[0]+tmp_num[1]+tmp_num[2]+tmp_num[3];
    key_value=keytab[(key_value-1)];
  return (key_value);
}
void Write_DATA(unsigned char add,unsigned char DATA) //Q
{
    Write_COM(0x44);
   // STB=0
	STB_OUT(0);
	TM1638_Write(0xc0|add);
	TM1638_Write(DATA);
	//STB=1
	STB_OUT(1);
}
void Write_oneLED(unsigned char num,unsigned char flag)	//单独控制一个LED函数，num为需要控制的led序号，flag为0时熄灭，不为0时点亮
{
	Write_DATA(num,flag);
}
void init_TM1638(void) //Q
{	
	unsigned char i;
	
	TM1638_GPIO_Configuration();   //IO口初始化

	Write_COM(0x8a);//脉冲宽度为全亮度的4/16
	Write_COM(0x40);
	//STB=0;
	STB_OUT(0);
	TM1638_Write(0xc0);
	for(i=0;i<16;i++)
	TM1638_Write(0x00);
	//STB=1;
	STB_OUT(1);

}

void TM1638_GPIO_Configuration(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DIO_RCC, ENABLE);	 //使能DIO端口时钟
	GPIO_InitStructure.GPIO_Pin = DIO_GPIO_PIN;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(DIO_GPIO, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB
	DIO_OUT(1);
	
//	GPIO_SetBits(GPIOA,GPIO_Pin_4);					//初始化设置为0
	RCC_APB2PeriphClockCmd(CLK_RCC, ENABLE);	 //使能PF端口时钟
	GPIO_InitStructure.GPIO_Pin = CLK_GPIO_PIN;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(CLK_GPIO, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB
	CLK_OUT(1);
	
	RCC_APB2PeriphClockCmd(STB_RCC, ENABLE);	 //使能PF端口时钟
	GPIO_InitStructure.GPIO_Pin = STB_GPIO_PIN;				 // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(STB_GPIO, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB
	STB_OUT(1);
}
void Tm1638_Test(void)
{
	unsigned char i,j;

	u8 tmp_fcs=0,tmp_len,tmp, keyDat ,t_kdat ,temp_keydat;
	
	for(i=0;i<16;i++)
	{
	  for(j=0;j<4;j++)
	  Write_DATA(j<<1,tab[i]); 
	  delay_ms(200);
	}

	for(i=0;i<16;i++)
	{ 
		i++;
		Write_oneLED(i,1);//i是第几个LED，j是LED的状态
	  delay_ms(200);
	}
}

void LED8_Display(unsigned char add,unsigned char DATA)
	{
		Write_DATA(add<<1,tab[DATA]);
	}
		