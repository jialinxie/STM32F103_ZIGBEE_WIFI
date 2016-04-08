#include "bmp180.h"

#define OSS 0	// Oversampling Setting (note: code is not set up to use other OSS values)

//***BMP180??
unsigned char ge,shi,bai,qian,wan,shiwan;           //????
short ac1;
short ac2; 
short ac3; 
unsigned short ac4;
unsigned short ac5;
unsigned short ac6;
short b1; 
short b2;
short mb;
short mc;
short md;

long  temperature;
long  pressure;
float tempdat=0;
unsigned char outdata[6]={0};

void delay5ms(void);
void I2C_Stop();


//************?????????****************************
void string_shift(unsigned int dat) 
{ 
  outdata[3] =dat/1000+'0';      // ??? 
  outdata[2] =(dat%1000)/100+'0'; //??? 
  outdata[1] =(dat%100)/10+'0';  // ??? 
  outdata[0] =dat%10+'0';        // ??? 
} 

  /*******************************/
void DATA_printf(u8 *s,short temp_data)
{
	if(temp_data<0){
	temp_data=-temp_data;
    *s='-';
	}
	else *s=' ';
    *++s =temp_data/100+0x30;
    temp_data=temp_data%100;     //????
    *++s =temp_data/10+0x30;
    temp_data=temp_data%10;      //????
    *++s =temp_data+0x30; 	
}

void I2C_Gpio_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	I2C_Stop();
}


void i2c_SDA_input(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
   GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void i2c_SDA_output(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	 GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	 GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  : I2C_delay
* Description    : Simulation IIC Timing series delay
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_delay(void)
{
   u16 i=5000; //????????	,??????5????
   while(i) 
   { 
     i--; 
   }  
}

void delay5ms(void)
{
   int i=50000;  
   while(i) 
   { 
     i--; 
   }  
}
//*************************************************************************************************
//I2C????
//*************************************************************************************************
void I2C_Start()
{
    SDA_H;                    //?????
    SCL_H;                    //?????
    I2C_delay();                 //??
    SDA_L;                    //?????
    I2C_delay();                 //??
    SCL_L;                    //?????
    I2C_delay();                 //??
}
//*************************************************************************************************
//I2C????
//*************************************************************************************************
void I2C_Stop()
{
	  SCL_L;                    //?????
    SDA_L;                    //?????
    I2C_delay();                 //??	
    SCL_H;                    //?????
    I2C_delay();                 //??
    SDA_H;                    //?????
    I2C_delay();                 //??
}
//**************************************************************************************************
//I2C??????
//????:ack (0:ACK 1:NAK)
//**************************************************************************************************
void I2C_SendACK(char ack)
{
	if(ack == 1)
	  SDA_H;
	else
    SDA_L;                  //?????
	SCL_H;                    //?????
	I2C_delay();                 //??
	SCL_L;                    //?????
	I2C_delay();                 //??
}
//****************************************************************************************************
//I2C??????
//****************************************************************************************************
char I2C_RecvACK()
{
	char CY;
	i2c_SDA_input();
	I2C_delay();                 //??	
	SCL_H;                    //?????
	I2C_delay();                 //??
	if(SDA_read)
    CY = 1;                   //?????
	else
		CY = 0;
	SCL_L;                    //?????
	i2c_SDA_output();
	I2C_delay();                 //??
	return CY;
}
//*****************************************************************************************************
//?I2C??????????
//*****************************************************************************************************
void I2C_SendByte(u8 dat)
{
    u8 i;
    for (i=0; i<8; i++)         //8????
    {
			  if(dat&0x80)
          SDA_H;               //????
			  else
					SDA_L;
				
        dat <<= 1;              //????????			
        SCL_H;                //?????
        I2C_delay();             //??
        SCL_L;                //?????
        I2C_delay();             //??
    }
    I2C_RecvACK();
}
//*****************************************************************************************************
//?I2C??????????
//******************************************************************************************************
u8 I2C_RecvByte(void)
{
    u8 i;
    u8 dat = 0;
		i2c_SDA_input();
    for (i=0; i<8; i++)         //8????
    {
        SCL_L;                //?????
        I2C_delay();             //?? 
        SCL_H;                //?????
        I2C_delay();             //??
  			dat <<= 1;			
			  if(SDA_read)
					dat |= 1;             //???               

    }
	  i2c_SDA_output();		
    return dat;
}



//*****************************************************************************************************
//?I2C??????????
//*****************************************************************************************************
void Single_WriteI2C(u8 SlaveAddress, u8 REG_Address,u8 REG_data)
{
    I2C_Start();                  //????
    I2C_SendByte(SlaveAddress);   //??????+???
    I2C_SendByte(REG_Address);    //???????,
    I2C_SendByte(REG_data);       //???????,
    I2C_Stop();                   //??????
}

//*******************************************************************************************************
//?I2C??????????
//*******************************************************************************************************
u8 Single_ReadI2C(u8 SlaveAddress, u8 REG_Address)
{
	u8 REG_data;
	I2C_Start();                   //????
	I2C_SendByte(SlaveAddress - 1);    //??????+???
	I2C_SendByte(REG_Address);     //????????,?0??	
	I2C_Start();                   //????
	I2C_SendByte(SlaveAddress);  //??????+???
	REG_data=I2C_RecvByte();       //???????
	I2C_SendACK(1);                //??????
	I2C_Stop();                    //????
	return REG_data;
}				   

//*********************************************************
//??BMP180????,????
//*********************************************************
short Multiple_read(u8 SlaveAddress, u8 ST_Address)
{   
	u8 msb, lsb;
	short _data;
    I2C_Start();                          //????
    I2C_SendByte(SlaveAddress);    //??????+???
    I2C_SendByte(ST_Address);             //????????
    I2C_Start();                          //????
    I2C_SendByte(SlaveAddress+1);         //??????+???

    msb = I2C_RecvByte();                 //BUF[0]??
    I2C_SendACK(0);                       //??ACK
    lsb = I2C_RecvByte();     
	I2C_SendACK(1);                       //?????????NOACK

    I2C_Stop();                           //????
    //Delay5ms();
    _data = msb << 8;
	_data |= lsb;	
	return _data;
}
//******************************************************************************************************
//???MPU6050
//******************************************************************************************************
void InitBMP180(void)
{
	ac1 = Multiple_read(BMP180_SlaveAddress, 0xAA);
	ac2 = Multiple_read(BMP180_SlaveAddress, 0xAC);
	ac3 = Multiple_read(BMP180_SlaveAddress, 0xAE);
	ac4 = Multiple_read(BMP180_SlaveAddress, 0xB0);
	ac5 = Multiple_read(BMP180_SlaveAddress, 0xB2);
	ac6 = Multiple_read(BMP180_SlaveAddress, 0xB4);
	b1 =  Multiple_read(BMP180_SlaveAddress, 0xB6);
	b2 =  Multiple_read(BMP180_SlaveAddress, 0xB8);
	mb =  Multiple_read(BMP180_SlaveAddress, 0xBA);
	mc =  Multiple_read(BMP180_SlaveAddress, 0xBC);
	md =  Multiple_read(BMP180_SlaveAddress, 0xBE);
}
//********************************************************************
long bmp180ReadTemp(void)
{

    I2C_Start();                  //????
    I2C_SendByte(BMP180_SlaveAddress);   //??????+???
    I2C_SendByte(0xF4);	          // write register address
    I2C_SendByte(0x2E);       	// write register data for temp
    I2C_Stop();                   //??????
		I2C_delay();	// max time is 4.5ms
	
	return (long) Multiple_read(BMP180_SlaveAddress, 0xF6);
}
//*************************************************************
long bmp180ReadPressure(void)
{
	long pressure = 0;

    I2C_Start();                   //????
    I2C_SendByte(BMP180_SlaveAddress);   //??????+???
    I2C_SendByte(0xF4);	          // write register address
    I2C_SendByte(0x34);       	  // write register data for pressure
    I2C_Stop();                    //??????
	I2C_delay();    	                  // max time is 4.5ms
	
	pressure = Multiple_read(BMP180_SlaveAddress, 0xF6);
	pressure &= 0x0000FFFF;
	
	return pressure;	
	//return (long) bmp180ReadShort(0xF6);
}

//***********************************************************************
void bmp180Convert()
{
	unsigned int ut = 0;
	unsigned long up = 0;
	long x1, x2, b5, b6, x3, b3, p;
	unsigned long b4, b7;

	ut = bmp180ReadTemp();	   // change by enviornment
	up = bmp180ReadPressure();  // change by enviornment
	
	x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
	x2 = ((long) mc << 11) / (x1 + md);
	b5 = x1 + x2;
	temperature = ((b5 + 8) >> 4);
	
  b6 = b5 - 4000;
  // Calculate B3
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<< OSS) + 2)>>2;
  
  // Calculate B4
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;
  
  b7 = ((unsigned long)(up - b3) * (50000 >> OSS));
  if (b7 < 0x80000000)
    p = (b7<<1)/b4;
  else
    p = (b7/b4)<<1;
    
  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  pressure = p+((x1 + x2 + 3791)>>4);
}

void bmp180_conversion(long temp_data, uint8_t *p)
{
	  *p++ = temp_data/100000+0x30;
    temp_data=temp_data%100000;   
		*p++=temp_data/10000+0x30;
    temp_data=temp_data%10000;   
		*p++=temp_data/1000+0x30;
    temp_data=temp_data%1000;    
    *p++=temp_data/100+0x30;
    temp_data=temp_data%100;     
    *p++=temp_data/10+0x30;
    temp_data=temp_data%10;     
    *p++=temp_data+0x30; 
}


void DATA_Diplay()
{ 
	//u8 tmp[15]={"temperture:00.0"}, pres[19] = {"pressure:000000 Pa"};
	u8 tmp[15]={"00.0"}, pres[19] = {"000000 Pa"};
	long temp_value;
	temp_value=temperature;

	bmp180_conversion(temp_value, outdata);	

	tmp[0]=outdata[3];
	tmp[1]=outdata[4];
	tmp[3]=outdata[5];

	LCD_ShowString(90, 70, tmp, BLACK, WHITE);
	LCD_ShowString(122, 70, " C", BLACK, WHITE);
	//printf("temperature=%c%c.%c\n",outdata[3],outdata[4],outdata[5]);
	
	temp_value = pressure;
	bmp180_conversion(temp_value, outdata);
	
	pres[0] = outdata[0];
	pres[1] = outdata[1];
	pres[2] = outdata[2];
	pres[3] = outdata[3];
	pres[4] = outdata[4];
	pres[5] = outdata[5];	
	
	LCD_ShowString(90, 90, pres, BLACK, WHITE);
	//printf("pressure=%c%c%c%c%c%c Pa\n",outdata[0],outdata[1],outdata[2],outdata[3],outdata[4],outdata[5]);
}

void BMP180_Test()
{
	I2C_Gpio_Init();
	InitBMP180();
  bmp180Convert();
  DATA_Diplay();		
}

