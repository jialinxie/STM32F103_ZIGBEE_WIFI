/*************	本地常量声明	**************/
#define MAIN_Fosc		22118400L	//定义主时钟
#define	RX1_Lenth		32			//串口接收缓冲长度
//#define	BaudRate1		9600UL	//选择波特率
//#define	Timer1_Reload	(65536UL -(MAIN_Fosc / 4 / BaudRate1))		//Timer 1 重装值， 对应300KHZ
//#define	Timer2_Reload	(65536UL -(MAIN_Fosc / 4 / BaudRate1))		//Timer 2 重装值， 对应300KHZ
#include"STC15Fxxxx.H"
#include<intrins.h>
#define uchar unsigned char
#define uint unsigned int
/*************	本地变量声明	**************/
u8	idata RX1_Buffer[RX1_Lenth];	//接收缓冲
u8	TX1_Cnt;	//发送计数
u8	RX1_Cnt;	//接收计数
bit	B_TX1_Busy;	//发送忙标志
/*************	端口引脚定义	**************/
sbit LED1=P1^0;//LED1
sbit LED2=P1^1;//LED2
sbit LED3=P3^7;//LED3
sbit DK1=P3^3;//继电器
sbit BEEP=P3^4;//蜂鸣器
sbit K1=P1^3;//按键1
sbit K2=P1^2;//按键2
sbit K3=P1^4;//按键3
sbit DQ=P1^6;//18B20温度传感器
char led1bl,led2bl,led3bl;
#define jump_ROM 0xCC                   //跳过ROM命令
#define start    0x44               //启动转换命令
#define read_EEROM 0xBE                 //读存储器命令
uchar TMPH,TMPL;                //温度值
uchar HL;                      //当前温度
unsigned char LYMS[13]="AT+CWMODE=2\r\n";
unsigned char SZLY[38]="AT+CWSAP='ESP8266','0123456789',11,0\r\n";
unsigned char RST[8]="AT+RST\r\n";
unsigned char SZDLJ[13]="AT+CIPMUX=1\r\n";
unsigned char KQFU[21]="AT+CIPSERVER=1,5000\r\n";
unsigned char FSSJ[13]="AT+CIPSEND=\r\n";//AT+CIPSEND= 发送数据
unsigned char WDSJ[4]={0x00,0x00,0x00,0x00};
void Delay1(unsigned long ms)
{
	unsigned char i, j,k;
	for(k=0;k<ms;k++)
	{
		_nop_();
		_nop_();
		i = 22;
		j = 128;
		do
		{
			while (--j);
		} while (--i);
	}
}
void Delay2(unsigned long cnt)
{
	long i;
 	for(i=0;i<cnt*100;i++);
}
void Delay3(unsigned int N) 
{
	int i;
	for(i=0;i<N*10;i++);
}
//--------------------
// 函数名称： Reset
// 入口参数： 无
// 返回deceive_ready
// 函数功能：复位 
//--------------------
unsigned char Reset(void) 
{
	unsigned char deceive_ready;
	Delay3(10); 
	DQ=0;                               //拉低DQ线 
	Delay3(29);                          //延时至少480us~960us
	DQ=1;                               //将DQ线设置位逻辑高    
	Delay3(3);                           //延时等待deceive_ready响应 
	deceive_ready=DQ;                   //采样deceive_ready信号
	Delay3(25);                          //等待时序结束
	return(deceive_ready);              //有deceive_ready信号时返回0，否则返回1
}


//---------------------------
// 函数名称：read_bit
// 入口参数： 无
// 返回接收的数据
// 函数功能：读一个bit子程序 
//---------------------------
unsigned char read_bit(void)
{
	unsigned char i;
	DQ=0;                                 //拉低DQ线开始时序
	DQ=1;                                 //升高DQ线                        
	for(i=0;i<3;i++);         //延时至时序开始15us
	return(DQ);                           //返回DQ值
}


//---------------------------
// 函数名称： write_bit
// 入口参数： bitval
// 函数功能：写一个bit子程序 
//---------------------------
void write_bit(unsigned char bitval)
{
	DQ=0;                             //拉低DQ线开始时序
	if(bitval==1)
		DQ=1;                             //如果写逻辑为高
	Delay3(5);                         //延时 
	DQ=1;                             //升高DQ线 
}


//----------------------------
// 函数名称： write_byte
// 入口参数： val
// 函数功能：写一个byte子程序 
//----------------------------
void write_byte(unsigned char val)
{
	unsigned char i,temp;
	for(i=0;i<8;i++)
	{
		temp=val>>i;                        //将val位右移i位赋值给比temp
		temp=temp&0x01;                     //取temp最低位
		write_bit(temp);
		Delay3(5);                           //延时至时序结束
	}
}


//----------------------------
// 函数名称： read_byte
// 返回接收的数据 value
// 函数功能：读一个byte子程序 
//----------------------------
unsigned char read_byte(void)
{
	unsigned char i,m,receive_data;
	m=1;receive_data=0;                          //初始化
	for(i=0;i<8;i++)
	{
		if(read_bit()) 
		{
			receive_data=receive_data+(m<<i);
		}                                          //每读一位数据据，左移一位 
		Delay3(6);                                  //延时至时序结束 
	} 
	return(receive_data);                        //返回value
}


//---------------------------
// 函数名称： Get_temp
// 返回接收的数据 TMPL, TMPH
// 函数功能：取出温度值
//---------------------------
uint Get_temp(void)
{
	unsigned int T;
	//EA = 0;
	Reset();
	write_byte(jump_ROM);       //发跳过ROM命令
	write_byte(start);          //发启动转换命令
	Reset();
	write_byte(jump_ROM);       //发跳过ROM命令
	write_byte(read_EEROM);     //发跳过ROM命令
	TMPL=read_byte();           //读低8位温度值
	TMPH=read_byte();           //读高8位温度值
	
	//EA = 0;
	T=TMPL+TMPH*256;     
	if(T==0xFFFF) return 0xFFFF;
	if(T>0x8000)   //温度为负
	{
		T=~T+1;
		return(0x8000+T*5/8);
	}
	else     //温度位正
	{
		return(T*5/8);
	}
}
void UARTSendByte(unsigned char byte)//?????????
{
	SBUF=byte;              //???????????
	while(TI==0);          //??????,TI?????1
	TI=0;                    //?????????
}
void DisplayTemp(unsigned int temp)
{
	uchar i=0;
	uchar TmpTable[3] = {0};
	TmpTable[ 0 ] = (temp%1000)/100;
	TmpTable[ 1 ] = (temp%100)/10;
	TmpTable[ 2 ] = (temp%10);

//	UARTSendByte(TmpTable[0] + 0x30);Delay3(9);
//	UARTSendByte(TmpTable[1] + 0x30);Delay3(9);
//	UARTSendByte('.');Delay3(9);
//	UARTSendByte(TmpTable[2] + 0x30);Delay3(9);	
//	UARTSendByte(0x0d);Delay3(9);	
//	UARTSendByte(0x0a);Delay3(9);
	WDSJ[0]=(TmpTable[0] + 0x30);
	WDSJ[1]=(TmpTable[1] + 0x30);
	WDSJ[2]=('.');
	WDSJ[3]=(TmpTable[2] + 0x30);	
}
void main(void)
{
	char i=0;
	B_TX1_Busy = 0;
	RX1_Cnt = 0;
	TX1_Cnt = 0;
	S1_8bit();				//8位数据
	S1_USE_P30P31();		//UART1 使用P30 P31口	默认
	/*AUXR &= ~(1<<4);	//Timer stop		波特率使用Timer2产生
	AUXR |= 0x01;		//S1 BRT Use Timer2;
	AUXR |=  (1<<2);	//Timer2 set as 1T mode
	TH2 = (u8)(Timer2_Reload >> 8);
	TL2 = (u8)Timer2_Reload;
	AUXR |=  (1<<4);	//Timer run enable
	REN = 1;	//允许接收
	ES  = 1;	//允许中断
	EA = 1;		//允许全局中断*/
	S2CON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xD0;		//设定定时初值
	T2H = 0xFF;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
	REN = 1;	//允许接收
	ES  = 1;	//允许中断
	EA = 1;		//允许全局中断*/
	P3M1 = 0x00;
    P3M0 = 0xFF;
	RX1_Cnt=0;
	DK1=0;
	BEEP=0;
	Delay2(2000);
	for(i=0;i<13;i++)//AT+CWMODE=2 设置成路由模式
	{
		SBUF=LYMS[i];Delay1(5);
	}
	Delay2(1000);
	for(i=0;i<38;i++)//AT+CWSAP="ESP8266","0123456789",11,0 设置路由
	{
		SBUF=SZLY[i];Delay1(5);
	}
	Delay2(3000);
	for(i=0;i<8;i++)//AT+RST 重启
	{
		SBUF=RST[i];Delay1(5);
	}
	Delay2(5000);
	for(i=0;i<13;i++)//AT+CIPMUX=1 设置成多连接
	{
		SBUF=SZDLJ[i];Delay1(5);
	}
	Delay2(2000);
	for(i=0;i<21;i++)//AT+CIPSERVER=1,5000 开启TCP服务端口
	{
		SBUF=KQFU[i];Delay1(5);
	}
	Delay2(2000);
	Get_temp();
	Delay3(50000);
	Get_temp();
	Delay3(50000);
	while (1)
	{
		DisplayTemp(Get_temp());//刷新18B20
		if(K1==0)
		{
			Delay1(5);
			if(K1==0)
			{
				LED1=!LED1;
			}
			while(K1==0);
		}
		if(K2==0)
		{
			Delay1(5);
			if(K2==0)
			{
				LED2=!LED2;
			}
			while(K2==0);
		}
		if(K3==0)
		{
			Delay1(5);
			if(K3==0)
			{
				LED3=!LED3;
			}
			while(K3==0);
		}
	}
}

/********************* UART1中断函数************************/
void UART1_int (void) interrupt UART1_VECTOR
{
	char i,a,b,c;
	if(RI)
	{
		RI = 0;
		RX1_Buffer[RX1_Cnt] = SBUF;		//保存一个字节
		if(RX1_Buffer[0]==0x45)
		{
			RX1_Cnt++;
		}
		else
		{
			RX1_Cnt=0;
		}
		if(RX1_Cnt>=10)
		{
			if(RX1_Buffer[0]==0x45&&RX1_Buffer[1]==0x53&&RX1_Buffer[2]==0x50)
			{
				if(RX1_Buffer[4]==0x4C&&RX1_Buffer[5]==0x45&&RX1_Buffer[6]==0x44)//判断LED
				{
					if(RX1_Buffer[7]==0x31)//判断LED1
					{
						if(RX1_Buffer[3]==0x4B)//判断开
						{
							LED1=0;
						}
						if(RX1_Buffer[3]==0x47)//判断关
						{
							LED1=1;
						}
					}
					if(RX1_Buffer[7]==0x32)//判断LED2
					{
						if(RX1_Buffer[3]==0x4B)//判断开
						{
							LED2=0;
						}
						if(RX1_Buffer[3]==0x47)//判断关
						{
							LED2=1;
						}
					}
					if(RX1_Buffer[7]==0x33)//判断LED3
					{
						if(RX1_Buffer[3]==0x4B)//判断开
						{
							LED3=0;
						}
						if(RX1_Buffer[3]==0x47)//判断关
						{
							LED3=1;
						}
					}
				}
				if(RX1_Buffer[4]==0x4A&&RX1_Buffer[5]==0x44&&RX1_Buffer[6]==0x51)//判断继电器
				{
					if(RX1_Buffer[7]==0x31)//判断LED1
					{
						if(RX1_Buffer[3]==0x4B)//判断开
						{
							DK1=1;
						}
						if(RX1_Buffer[3]==0x47)//判断关
						{
							DK1=0;
						}
					}
				}
				if(RX1_Buffer[3]==0x46&&RX1_Buffer[4]==0x4D&&RX1_Buffer[5]==0x51&&RX1_Buffer[6]==0x43&&RX1_Buffer[7]==0x53)//判断蜂鸣器
				{
					BEEP=1;Delay2(100);BEEP=0;Delay2(100);BEEP=1;Delay2(100);BEEP=0;Delay2(100);
				}
				if(RX1_Buffer[3]==0x43&&RX1_Buffer[4]==0x58&&RX1_Buffer[5]==0x53&&RX1_Buffer[6]==0x4A)//查询数据
				{
					if(LED1==0){a=0x4B;}else{a=0x47;}
					if(LED2==0){b=0x4B;}else{b=0x47;}
					if(LED3==0){c=0x4B;}else{c=0x47;}
					for(i=0;i<11;i++)//AT+CIPSEND= 发送数据
					{
						SBUF=FSSJ[i];Delay1(5);
					}
					SBUF=0x30;Delay1(5);
					SBUF=0x2C;Delay1(5);
					SBUF=0x32;Delay1(5);
					SBUF=0x36;Delay1(5);
					SBUF=0x0D;Delay1(5);
					SBUF=0x0A;Delay1(5);
					
					SBUF=0x45;Delay1(5);
					SBUF=0x53;Delay1(5);
					SBUF=0x50;Delay1(5);
					SBUF=0x4C;Delay1(5);
					SBUF=0x45;Delay1(5);
					SBUF=0x44;Delay1(5);
					SBUF=0x31;Delay1(5);
					SBUF=a;Delay1(5);
					SBUF=0x4C;Delay1(5);
					SBUF=0x45;Delay1(5);
					SBUF=0x44;Delay1(5);
					SBUF=0x32;Delay1(5);
					SBUF=b;Delay1(5);
					SBUF=0x4C;Delay1(5);
					SBUF=0x45;Delay1(5);
					SBUF=0x44;Delay1(5);
					SBUF=0x33;Delay1(5);
					SBUF=c;Delay1(5);
					SBUF=WDSJ[0];Delay1(5);
					SBUF=WDSJ[1];Delay1(5);
					SBUF=WDSJ[2];Delay1(5);
					SBUF=WDSJ[3];Delay1(5);
					SBUF=0x50;Delay1(5);
					SBUF=0x53;Delay1(5);
					SBUF=0x45;Delay1(5);
					SBUF=0x0A;Delay1(5);
					//SBUF=0x0A;Delay1(5);
					//45 53 50 4C 45 44 31 4B 4C 45 44 32 4B 4C 45 44 33 4B 50 53 45 
				}
				RX1_Cnt=0;
			}
			else
			{
				RX1_Cnt=0;
			}
			RX1_Cnt=0;
		}
	}
	if(TI)
	{
		TI = 0;
		B_TX1_Busy = 0;		//清除发送忙标志
	}
}