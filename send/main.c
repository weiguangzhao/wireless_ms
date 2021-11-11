#include <msp430x14x.h>
#include "24l01.h"
#include "18b20.h"
//==============================================================================
#define  LED1_0              P6OUT &=~BIT1          //输出0
#define  LED1_1              P6OUT |= BIT1          //输出1
#define  LED2_0              P6OUT &=~BIT2          //输出0
#define  LED2_1              P6OUT |= BIT2          //输出1
//==============================================================================
#define BIT(x) 1<<x
#define uint unsigned int
//****************************************************
void INIT_set()
{
   P4SEL|=BIT0;                          //P4.0作为捕获模块功能的输入端输入方波 
   TBCCTL0&=~(CCIS1+CCIS0);          // 捕获源为P4.0，即CCI0A(也是CCI0B) 
   TBCCTL0|=CM_1+SCS+CAP;           //上沿捕获，同步捕获，工作在捕获模式 
   TBCCTL0|=CCIE;                     //允许捕获比较模块提出中断请求 
   TBCTL|=ID_3;                         //8分频
   TBCTL|=TBSSEL_2;                   //选择时钟MCLK 
   //定时器开始计数（连续计数模式0～0xFFFF） 
   TBCTL|=MC_2; 
  P4SEL&=~BIT1;
  P4DIR|=BIT(1);
  _EINT();//打开全局总中断(_EINT)
}
void trig()
{
   _EINT();
P4OUT|=BIT(1);
delay_us(20);
P4OUT&=~BIT1;
delay_us(20);
}
//******************************************************************************
//系统初始化
//******************************************************************************
void InitSys()
{
   unsigned int iq0;
   _DINT();
   BCSCTL1 &=~XT2OFF;
   do
   {
      IFG1 &= ~OFIFG;							// 清除振荡器失效标志
      for (iq0 = 0xFF; iq0 > 0; iq0--);	// 延时，等待XT2起振
   }
   while ((IFG1 & OFIFG) != 0);		// 判断XT2是否起振		
   BCSCTL2 =SELM1+SELS;                // MCLK,SMCLK时钟为XT2		
}
//===========================LED端口设置==========================================
void LED_IO_set(void)
{    
      P6DIR |= 0x06;   P6SEL&=0xf9;        
}
//========================延时约5ms=============================================
void ms_delay(void)
{ 
   unsigned int i=40000;
    while (i != 0)
    {
        i--;
    }
}
//******************************************************************************************
//延时函数
//******************************************************************************************
void inerDelay_us(char n)
{
	for(;n>0;n--);
}
//==============================================================================
//函数：uint SPI_RW(uint uchar)
//功能：NRF24L01的SPI写时序
//******************************************************************************
char SPI_RW(char data)
{
	char i,temp=0;
   	for(i=0;i<8;i++) // output 8-bit
   	{
	if((data & 0x80)==0x80)
	{
		RF24L01_MOSI_1;         // output 'uchar', MSB to MOSI
	}
	else
	{
	 	RF24L01_MOSI_0; 
	}	
//==============================================================================
		data = (data << 1);            // shift next bit into MSB..
		temp<<=1;
		RF24L01_SCK_1;                // Set SCK high..
		if((P3IN&0x01)==0x01)temp++;         // capture current MISO bit
		RF24L01_SCK_0;              // ..then set SCK low again
   	}
    return(temp);           		  // return read uchar
}
//****************************************************************************************************
//函数：uchar SPI_Read(uchar reg)
//功能：NRF24L01的SPI时序
//****************************************************************************************************
char SPI_Read(char reg)
{
	char reg_val;
	RF24L01_CSN_0;           // CSN low, initialize SPI communication...
	SPI_RW(reg);            // Select register to read from..
	reg_val = SPI_RW(0);    // ..then read registervalue
	RF24L01_CSN_1;         // CSN high, terminate SPI communication
	return(reg_val);       // return register value
}
//****************************************************************************************************/
//功能：NRF24L01读写寄存器函数
//****************************************************************************************************/
char SPI_RW_Reg(char reg, char value)
{
	char status1;
	RF24L01_CSN_0;                   // CSN low, init SPI transaction
	status1 = SPI_RW(reg);      // select register
	SPI_RW(value);             // ..and write value to it..
	RF24L01_CSN_1;                   // CSN high again
	return(status1);            // return nRF24L01 status uchar
}
//****************************************************************************************************/
//函数：uint SPI_Read_Buf(uchar reg, uchar *pBuf, uchar uchars)
//功能: 用于读数据，reg：为寄存器地址，pBuf：为待读出数据地址，uchars：读出数据的个数
//****************************************************************************************************/
char SPI_Read_Buf(char reg, char *pBuf, char chars)
{
	char status2,uchar_ctr;
	RF24L01_CSN_0;                    		// Set CSN low, init SPI tranaction
	status2 = SPI_RW(reg);       		// Select register to write to and read status uchar
	for(uchar_ctr=0;uchar_ctr<chars;uchar_ctr++)
        {
	pBuf[uchar_ctr] = SPI_RW(0);    // 
        }
	RF24L01_CSN_1;                           
	return(status2);                    // return nRF24L01 status uchar
}
//*********************************************************************************************************
//函数：uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars)
//功能: 用于写数据：为寄存器地址，pBuf：为待写入数据地址，uchars：写入数据的个数
//*********************************************************************************************************/
char SPI_Write_Buf(char reg, char *pBuf, char chars)
{
	char status1,uchar_ctr;
	RF24L01_CSN_0;             //SPI使能       
	status1 = SPI_RW(reg);   
	for(uchar_ctr=0; uchar_ctr<chars; uchar_ctr++) //
        {
	SPI_RW(*pBuf++);
        }
	RF24L01_CSN_1;           //关闭SPI
	return(status1);    		  // 
}
//****************************************************************************************************/
//函数：void SetRX_Mode(void)
//功能：数据接收配置 
//****************************************************************************************************/
void SetRX_Mode(void)
{
	RF24L01_CE_0 ;
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);   		// IRQ收发完成中断响应，16位CRC	，主接收
	RF24L01_CE_1; 
	inerDelay_us(130);//注意不能太小
}
//******************************************************************************************************/
//函数：unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
//功能：数据读取后放如rx_buf接收缓冲区中
//******************************************************************************************************/
char nRF24L01_RxPacket(char* rx_buf)
{
    char revale=0;
	sta=SPI_Read(STATUS);	     // 读取状态寄存其来判断数据接收状况		
	if(sta&0x40)                 // 判断是否接收到数据
	{
	    RF24L01_CE_0 ; 			//SPI使能
	    SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer
	    revale =1;			//读取数据完成标志
	}
	SPI_RW_Reg(WRITE_REG+STATUS,sta);   //接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清楚中断标志
	return revale;
}
//***********************************************************************************************************
//函数：void nRF24L01_TxPacket(char * tx_buf)
//功能：发送 tx_buf中数据
//**********************************************************************************************************/
void nRF24L01_TxPacket(char * tx_buf)
{
	RF24L01_CE_0 ;			//StandBy I模式	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // 装载接收端地址
	SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); 			 // 装载数据	
   //     SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);   		 // IRQ收发完成中断响应，16位CRC，主发送
	RF24L01_CE_1;		 //置高CE，激发数据发送
	inerDelay_us(10);
}
//****************************************************************************************
//NRF24L01初始化
//***************************************************************************************/
void init_NRF24L01(void)
{
        inerDelay_us(100);
 	RF24L01_CE_0 ;    // chip enable
 	RF24L01_CSN_1;   // Spi disable 
 	RF24L01_SCK_0;   // Spi clock line init high
	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // 写本地地址	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); // 写接收端地址
	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      //  频道0自动	ACK应答允许	
	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  //  允许接收地址只有频道0，如果需要多频道可以参考Page21  
	SPI_RW_Reg(WRITE_REG + RF_CH, 0);        //   设置信道工作为2.4GHZ，收发必须一致
	SPI_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //设置接收数据长度，本次设置为32字节
	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   //设置发射速率为1MHZ，发射功率为最大值0dB
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0E);   // IRQ收发完成中断响应，16位CRC	，主接收}
} 
//=============================================================================
main()
{     
      WDTCTL = WDTPW + WDTHOLD; //禁止看门狗
      LED_IO_set();
      RF24L01_IO_set();
      DS18B20_IO_set(); 
      INIT_set();
      InitSys();
      init_NRF24L01() ;
      Ds18b20Convert();
      LED1_1;LED2_1;
      while(1)
      {
       Ds18b20Convert();          //采集温度
       LED1_0;
       work_temp();	
       nRF24L01_TxPacket(dispaly);  //把采集好的温度数据发送出去	
       SPI_RW_Reg(WRITE_REG+STATUS,0XFF);   //清状态寄存器
       delay_ms(5);//      ms_delay();          
       delay_ms(5);//      ms_delay();  
       trig();
       delay_ms(5);//      ms_delay();          
       delay_ms(5);//      ms_delay();    
       LED1_1;
       delay_ms(5);//      ms_delay();           
       delay_ms(5);//      ms_delay();     
      }
}
//―――――定时器TB的CCR0的中断：用于检测脉冲上升与下降沿―――― 
 #pragma vector=TIMERB0_VECTOR        
 __interrupt void TimerB0(void) 
 { 
   _NOP();//关闭中断  
   TBCTL|=TBCLR;                      //定时器清零， 
      while((P4IN&BIT0)==BIT0);
      width=TBCCR0;
       d=(width*17)/1000-15; 
       _EINT();//允许产生中断
 } 
//捕获高电平时间