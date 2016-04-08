#include "sys.h"
#include "usart.h"	 


 //qinyx write
 uint8_t BufferA[20];
 uint8_t BufferB[20];
 uint8_t g_flag1=0,g_flag2=0;
 uint8_t A_count=0,B_count=0;
 uint8_t DBufferFlag1=0,DBufferFlag2=0;
 
 uint8_t message[42] = {0};
 bool OK_flag = false;
 bool CONNECT_flag = false;
 bool NodeAddress = false;
 bool NodeStat = false;

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
void uart_init1(u32 bound)
{
    //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC 配置

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART1, ENABLE);                    //使能串口 
		NVIC_EnableIRQ(USART1_IRQn);
		USART_ClearFlag(USART1, USART_FLAG_TC);

}
//mp3
void uart_init2(u32 bound)
{
//**************************************************
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 第1步：打开GPIO和USART部件的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* 第2步：将USART Tx的GPIO配置为推挽复用模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* 第3步：将USART Rx的GPIO配置为浮空输入模式
		由于CPU复位后，GPIO缺省都是浮空输入模式，因此下面这个步骤不是必须的
		但是，我还是建议加上便于阅读，并且防止其它地方修改了这个口线的设置参数
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/*  第3步已经做了，因此这步可以不做
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);
//////////////////////////////////////////////////
	   //Usart2 NVIC 配置

  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
/*	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3/
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3/*/
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能/
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器/  

	/* 第4步：配置USART参数
	    - BaudRate = 115200 baud
	    - Word Length = 8 Bits
	    - One Stop Bit
	    - No parity
	    - Hardware flow control disabled (RTS and CTS signals)
	    - Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = 9600;//115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	/* 第5步：使能 USART， 配置完毕 */
	USART_Cmd(USART2, ENABLE);

	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART2, USART_FLAG_TC);     /* 清发送外城标志，Transmission Complete flag */
	///////////////////////////////////////////////////////////
	  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
		NVIC_EnableIRQ(USART2_IRQn);	
}

void USART1_Send(uint8_t *str,uint8_t num)
{
	while(num!=0)
	{
    //USART1->DR = (*str++ & (uint16_t)0x01FF);
		//while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		//num--;
		USART_SendData(USART1,*str++ & (uint16_t)0x01FF);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		num--;
	}
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	uint8_t tmp1;
	uint8_t i = 0, j = 0;
	uint16_t tim_dly;
	uint8_t sum = 0;

	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	message[0] = USART1->DR & (uint16_t)0x01FF;
	
		if(message[0] == 0xFE)//等待帧头
		{
				for(i = 1; i < 8; i++)		//receive front 8 bytes
				{  
					tim_dly=0xffff;
					while(!USART_GetITStatus(USART1,USART_IT_RXNE))
					{
						if(tim_dly==0)
						{
							g_flag1=0;
							return ;
						}
						else tim_dly--;			
					}//等待下一个字符的到来
					USART_ClearITPendingBit(USART1,USART_IT_RXNE);
					message[i]=USART1->DR & (uint16_t)0x01FF;
				}
				if(message[7] == 0x1F){	//查询地址的响应，返回节点地址
					for(i = 8; i < 42; i++)		//receive remain 42-8 bytes
					{  
						tim_dly=0xffff;
						while(!USART_GetITStatus(USART1,USART_IT_RXNE))
						{
							if(tim_dly==0)
							{
								g_flag1=0;
								return ;
							}
							else tim_dly--;			
						}//等待下一个字符的到来
						USART_ClearITPendingBit(USART1,USART_IT_RXNE);
						message[i]=USART1->DR & (uint16_t)0x01FF;		
					}
					NodeAddress = true;
					
				}else
				if(message[7] == 0x0B){		//查询状态的响应，返回节点状态
						for(i = 8; i < 22; i++)		//receive remain 42-8 bytes
						{  
							tim_dly=0xffff;
							while(!USART_GetITStatus(USART1,USART_IT_RXNE))
							{
								if(tim_dly==0)
								{
									g_flag1=0;
									return ;
								}
								else tim_dly--;			
							}//等待下一个字符的到来
							USART_ClearITPendingBit(USART1,USART_IT_RXNE);
							message[i]=USART1->DR & (uint16_t)0x01FF;		
						}	
						NodeStat = true;	
				}
		} 
}

void USART2_Send(uint8_t *str,uint8_t num)
{
	while(num!=0)
	{
    //USART1->DR = (*str++ & (uint16_t)0x01FF);
		//while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		//num--;
		USART_SendData(USART2,*str++ & (uint16_t)0x01FF);
		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
		num--;
	}
}


void USART2_IRQHandler(void)                	//串口1中断服务程序
{
	g_flag2=1;
	USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	BufferB[B_count]=USART2->DR & (uint16_t)0x01FF;
	/*if(B_count<20)
	{
		B_count=B_count++;
		DBufferFlag2=0;
}
else if((B_count>20)||(B_count==20))
	{
		B_count=0;
		DBufferFlag2=1;
}*/
	USART_SendData(USART1,BufferB[B_count]);
} 
#endif	

