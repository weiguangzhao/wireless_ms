#include <msp430x14x.h>
#include "24l01.h"
#include "1602.h"
#include "flash12.h"
//==============================================================================
#define  LED1_0              P6OUT &=~BIT1          //���0
#define  LED1_1              P6OUT |= BIT1          //���1
#define  LED2_0              P6OUT &=~BIT2          //���0
#define  LED2_1              P6OUT |= BIT2          //���1
//******************************************************************************
//ϵͳ��ʼ��
//******************************************************************************
void InitSys()
{
   unsigned int iq0;
   _DINT();
   BCSCTL1 &=~XT2OFF;
   do
   {
      IFG1 &= ~OFIFG;							// �������ʧЧ��־
      for (iq0 = 0xFF; iq0 > 0; iq0--);	// ��ʱ���ȴ�XT2����
   }
   while ((IFG1 & OFIFG) != 0);		// �ж�XT2�Ƿ�����		
   BCSCTL2 =SELM1+SELS;                // MCLK,SMCLKʱ��ΪXT2	
}
//===========================LED�˿�����==========================================
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
//������uint SPI_RW(uint uchar)
//���ܣ�NRF24L01��SPIдʱ��
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
//������uchar SPI_Read(uchar reg)
//���ܣ�NRF24L01��SPIʱ��
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
//���ܣ�NRF24L01��д�Ĵ�������
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
//������uint SPI_Read_Buf(uchar reg, uchar *pBuf, uchar uchars)
//����: ���ڶ����ݣ�reg��Ϊ�Ĵ�����ַ��pBuf��Ϊ���������ݵ�ַ��uchars���������ݵĸ���
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
//������uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars)
//����: ����д���ݣ�Ϊ�Ĵ�����ַ��pBuf��Ϊ��д�����ݵ�ַ��uchars��д�����ݵĸ���
//*********************************************************************************************************/
char SPI_Write_Buf(char reg, char *pBuf, char chars)
{
	char status1,uchar_ctr;
	RF24L01_CSN_0;             //SPIʹ��       
	status1 = SPI_RW(reg);   
	for(uchar_ctr=0; uchar_ctr<chars; uchar_ctr++) //
        {
	SPI_RW(*pBuf++);
        }
	RF24L01_CSN_1;           //�ر�SPI
	return(status1);    		  // 
}
//****************************************************************************************************/
//������void SetRX_Mode(void)
//���ܣ����ݽ������� 
//****************************************************************************************************/
void SetRX_Mode(void)
{
	RF24L01_CE_0 ;
        Delay(1);
	RF24L01_CE_1; 
	Delay(1000);//ע�ⲻ��̫С
}
//******************************************************************************************************/
//������unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
//���ܣ����ݶ�ȡ�����rx_buf���ջ�������
//******************************************************************************************************/
char nRF24L01_RxPacket(char* rx_buf)
{
    char revale=0;
	sta=SPI_Read(STATUS);	     // ��ȡ״̬�Ĵ������ж����ݽ���״��		
	if(sta&0x40)                 // �ж��Ƿ���յ�����
	{
	    RF24L01_CE_0 ; 			//SPIʹ��
	    SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer
	    revale =1;			//��ȡ������ɱ�־
	}
        else 
        {
              revale =0;	
        }
	SPI_RW_Reg(WRITE_REG+STATUS,sta);   //���յ����ݺ�RX_DR,TX_DS,MAX_PT���ø�Ϊ1��ͨ��д1������жϱ�־
	return revale;
}
//***********************************************************************************************************
//������void nRF24L01_TxPacket(char * tx_buf)
//���ܣ����� tx_buf������
//**********************************************************************************************************/
void nRF24L01_TxPacket(char * tx_buf)
{
	RF24L01_CE_0 ;			//StandBy Iģʽ	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // װ�ؽ��ն˵�ַ
	SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); 			 // װ������	
//	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);   		 // IRQ�շ�����ж���Ӧ��16λCRC��������
	RF24L01_CE_1;		 //�ø�CE���������ݷ���
	Delay(600);
}
//****************************************************************************************
//NRF24L01��ʼ��
//***************************************************************************************/
void init_NRF24L01(void)
{

 	RF24L01_CE_0 ;    // chip enable
 	RF24L01_CSN_1;   // Spi disable 
 	RF24L01_SCK_0;   // Spi clock line init high
	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // д���ص�ַ	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); // д���ն˵�ַ
	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      //  Ƶ��0�Զ�	ACKӦ������	
	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  //  ������յ�ַֻ��Ƶ��0�������Ҫ��Ƶ�����Բο�Page21  
	SPI_RW_Reg(WRITE_REG + RF_CH, 0);        //   �����ŵ�����Ϊ2.4GHZ���շ�����һ��
	SPI_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //���ý������ݳ��ȣ���������Ϊ32�ֽ�
	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   		//���÷�������Ϊ1MHZ�����书��Ϊ���ֵ0dB
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);   		// IRQ�շ�����ж���Ӧ��16λCRC	��������}
        Delay(1000);
}
//===============================���ڳ�ʼ��=====================================
void init_uart0(void)
  {
//====================���ڹ���ģʽ����========================================
    U0CTL=0x00;      // U0CTL��������0ͨ��Э�顢ͨ��ģʽ��У��λ�����ã�����UART0
    U0CTL +=CHAR;    //(CHAR=0x10)��CHAR=0ʱλ7λ���ݣ��� CHAR=1ʱΪ8λ���� 
                     //����ҪУ�飬����λΪ8λ���޷������첽UARTͨ�ţ�UART������
//====================���ڷ��Ͳ�������========================================
    U0TCTL=0x00;     //U0TCTL��������0���͹��ܲ���
    U0TCTL +=SSEL0;  //�����ʷ�����ѡ��ACLK 
                     //#define SSEL1  (0x20)       #define SSEL0  (0x10)
//====================���ڲ���������9600====================================
//===================�����ʼ��㹫ʽ��������=BRCLK/��UBR+��M7+M6+��������+M0��/8��
    UBR0_0=0x03;     //UBR0_0Ϊ����0�������������ֵ͵�ַ 
    UBR1_0=0x00;     //UBR1_0Ϊ����0�������������ָߵ�ַ 
    UMCTL_0=0x4A;    //UBR1_0Ϊ����0������С�����ָߵ�ַ 
//===================�����շ�ʹ�ܿ���=========================================
    ME1 |= UTXE0;    //����0���͹���ʹ��
    ME1 |= URXE0;    //����0���չ���ʹ��    
//===================�����ж�ʹ�ܿ���=========================================
//  IE1 |= URXIE0;   //����0�����ж�ʹ��
//   IE1 |= UTXIE0;   //����0�����ж�ʹ��
//===================�˿ڵڶ�����ʹ��=========================================
    P3SEL |=BIT4;    //����P3��4ΪUART0 ��TXD
    P3SEL |=BIT5;    //����P3��5ΪUART0 ��RXD
    P3DIR |=BIT4;    //����P3��4Ϊ���
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
      WDTCTL = WDTPW + WDTHOLD; //��ֹ���Ź�
      LED_IO_set();
      BELL_IO_set();
      LCD_IO_set();      //LCD�˿�����    
      RF24L01_IO_set();
      InitSys();
      init_uart0();
      init_NRF24L01() ;
      INIT_anjian();
      LcdReset();        //LCD��ʼ��  
      LED1_1;LED2_1;
       Lcdstart();
      while(1)
      {
      jjjiemain();
      thjiemain(); 
     SetRX_Mode();
  if(nRF24L01_RxPacket(RxBuf))   //�ж��Ƿ��յ�����
      {
        
       temp[0]=RxBuf[3];      //����λ
       temp[2]=((RxBuf[2]<<4)|RxBuf[1]);		//����λ
       temp[1]=RxBuf[0];				//С��λ
       temp[4]=RxBuf[2];				//ʮλ
       temp[5]=RxBuf[1];	//��λ
       temp[7]=RxBuf[7];	//gaoshiwei
       temp[8]=RxBuf[8];       //
       LED1_1;
   for(i=0;i<3;i++)
      {
       R_S_Byte(temp[2-i]);     //�����ϴ��¶�
       Delay(10);
       }                     
     }
        wen_xianshi();    //LCD�¶���ʾ
        baojin();
        LED1_0;
      }
}