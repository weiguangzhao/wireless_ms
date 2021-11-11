#include <msp430x14x.h>
#include "24l01.h"
#include "1602.h"
#include "flash12.h"
//==============================================================================
#define  LED1_0              P6OUT &=~BIT1          //输出0
#define  LED1_1              P6OUT |= BIT1          //输出1
#define  LED2_0              P6OUT &=~BIT2          //输出0
#define  LED2_1              P6OUT |= BIT2          //输出1
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
//==============================================================================
void BELL_IO_set(void)
{    
        P6DIR |= 0x08;   P6SEL&=0xF7;        
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
	return(status2);   
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
        Delay(1);
	RF24L01_CE_1; 
	Delay(1000);//注意不能太小
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
        else 
        {
              revale =0;	
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
//	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);   		 // IRQ收发完成中断响应，16位CRC，主发送
	RF24L01_CE_1;		 //置高CE，激发数据发送
	Delay(600);
}
//****************************************************************************************
//NRF24L01初始化
//***************************************************************************************/
void init_NRF24L01(void)
{

 	RF24L01_CE_0 ;    // chip enable
 	RF24L01_CSN_1;   // Spi disable 
 	RF24L01_SCK_0;   // Spi clock line init high
	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // 写本地地址	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); // 写接收端地址
	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      //  频道0自动	ACK应答允许	
	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  //  允许接收地址只有频道0，如果需要多频道可以参考Page21  
	SPI_RW_Reg(WRITE_REG + RF_CH, 0);        //   设置信道工作为2.4GHZ，收发必须一致
	SPI_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //设置接收数据长度，本次设置为32字节
	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   		//设置发射速率为1MHZ，发射功率为最大值0dB
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);   		// IRQ收发完成中断响应，16位CRC	，主接收}
        Delay(1000);
}
//===============================串口初始化=====================================
void init_uart0(void)
  {
//====================串口工作模式设置========================================
    U0CTL=0x00;      // U0CTL包含串口0通信协议、通信模式、校验位等设置，允许UART0
    U0CTL +=CHAR;    //(CHAR=0x10)当CHAR=0时位7位数据，当 CHAR=1时为8位数据 
                     //不需要校验，数据位为8位，无反馈，异步UART通信，UART被允许
//====================串口发送操作设置========================================
    U0TCTL=0x00;     //U0TCTL包含串口0发送功能操作
    U0TCTL +=SSEL0;  //波特率发生器选择ACLK 
                     //#define SSEL1  (0x20)       #define SSEL0  (0x10)
//====================串口拨特率设置9600====================================
//===================拨特率计算公式：拨特率=BRCLK/（UBR+（M7+M6+。。。。+M0）/8）
    UBR0_0=0x03;     //UBR0_0为串口0波特率整数部分低地址 
    UBR1_0=0x00;     //UBR1_0为串口0波特率整数部分高地址 
    UMCTL_0=0x4A;    //UBR1_0为串口0波特率小数部分高地址 
//===================串口收发使能控制=========================================
    ME1 |= UTXE0;    //串口0发送功能使能
    ME1 |= URXE0;    //串口0接收功能使能    
//===================串口中断使能控制=========================================
//  IE1 |= URXIE0;   //串口0接收中断使能
//   IE1 |= UTXIE0;   //串口0发送中断使能
//===================端口第二功能使能=========================================
    P3SEL |=BIT4;    //设置P3。4为UART0 的TXD
    P3SEL |=BIT5;    //设置P3。5为UART0 的RXD
    P3DIR |=BIT4;    //设置P3。4为输出
  }
//==============================================================================
void   R_S_Byte(char R_Byte)
{
    while((IFG1&UTXIFG0)==0);	//
    TXBUF0=R_Byte;
}
//=============================================================================
main()
{
      char i;
      WDTCTL = WDTPW + WDTHOLD; //禁止看门狗
      LED_IO_set();
      BELL_IO_set();
      LCD_IO_set();      //LCD端口设置    
      RF24L01_IO_set();
      InitSys();
      init_uart0();
      init_NRF24L01() ;
      INIT_anjian();
      LcdReset();        //LCD初始化  
      LED1_1;LED2_1;
       Lcdstart();
      while(1)
      {
      jjjiemain();
      thjiemain(); 
     SetRX_Mode();
  if(nRF24L01_RxPacket(RxBuf))   //判断是否收到数据
      {
        
       temp[0]=RxBuf[3];      //符号位
       temp[2]=((RxBuf[2]<<4)|RxBuf[1]);		//整数位
       temp[1]=RxBuf[0];				//小数位
       temp[4]=RxBuf[2];				//十位
       temp[5]=RxBuf[1];	//个位
       temp[7]=RxBuf[7];	//gaoshiwei
       temp[8]=RxBuf[8];       //
       LED1_1;
   for(i=0;i<3;i++)
      {
       R_S_Byte(temp[2-i]);     //串口上传温度
       Delay(10);
       }                     
     }
        wen_xianshi();    //LCD温度显示
        baojin();
        LED1_0;
      }
}