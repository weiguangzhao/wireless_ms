//==============================================================================
#define  RF24L01_CE_0        P3OUT &=~BIT1         
#define  RF24L01_CE_1        P3OUT |= BIT1        
//=============================RF24L01_CSN端口==================================
#define  RF24L01_CSN_0       P3OUT &=~BIT3         
#define  RF24L01_CSN_1       P3OUT |= BIT3     
//=============================RF24L01_SCK======================================
#define  RF24L01_SCK_0       P3OUT &=~BIT2      
#define  RF24L01_SCK_1       P3OUT |= BIT2   
//=============================MISO端口=========================================
#define  RF24L01_MISO_0      P3OUT &=~BIT0
#define  RF24L01_MISO_1      P3OUT |= BIT0
//============================= RF24L01_MOSI端口================================
#define  RF24L01_MOSI_0      P2OUT &=~BIT6
#define  RF24L01_MOSI_1      P2OUT |= BIT6
//==========================IRQ状态============================================
#define  RF24L01_IRQ_0       P2OUT &=~BIT7       
#define  RF24L01_IRQ_1       P2OUT |= BIT7
//==========================NRF24L01============================================
#define TX_ADR_WIDTH    5   	// 5 uints TX address width
#define RX_ADR_WIDTH    5   	// 5 uints RX address width
#define TX_PLOAD_WIDTH  32  	// 32 TX payload
#define RX_PLOAD_WIDTH  32  	// 32 uints TX payload
//=========================NRF24L01寄存器指令===================================
#define READ_REG        0x00  	// 读寄存器指令
#define WRITE_REG       0x20 	// 写寄存器指令
#define RD_RX_PLOAD     0x61  	// 读取接收数据指令
#define WR_TX_PLOAD     0xA0  	// 写待发数据指令
#define FLUSH_TX        0xE1 	// 冲洗发送 FIFO指令
#define FLUSH_RX        0xE2  	// 冲洗接收 FIFO指令
#define REUSE_TX_PL     0xE3  	// 定义重复装载数据指令
#define NOP1            0xFF  	// 保留
//========================SPI(nRF24L01)寄存器地址===============================
#define CONFIG          0x00  // 配置收发状态，CRC校验模式以及收发状态响应方式
#define EN_AA           0x01  // 自动应答功能设置
#define EN_RXADDR       0x02  // 可用信道设置
#define SETUP_AW        0x03  // 收发地址宽度设置
#define SETUP_RETR      0x04  // 自动重发功能设置
#define RF_CH           0x05  // 工作频率设置
#define RF_SETUP        0x06  // 发射速率、功耗功能设置
#define STATUS          0x07  // 状态寄存器
#define OBSERVE_TX      0x08  // 发送监测功能
#define CD              0x09  // 地址检测           
#define RX_ADDR_P0      0x0A  // 频道0接收数据地址
#define RX_ADDR_P1      0x0B  // 频道1接收数据地址
#define RX_ADDR_P2      0x0C  // 频道2接收数据地址
#define RX_ADDR_P3      0x0D  // 频道3接收数据地址
#define RX_ADDR_P4      0x0E  // 频道4接收数据地址
#define RX_ADDR_P5      0x0F  // 频道5接收数据地址
#define TX_ADDR         0x10  // 发送地址寄存器
#define RX_PW_P0        0x11  // 接收频道0接收数据长度
#define RX_PW_P1        0x12  // 接收频道0接收数据长度
#define RX_PW_P2        0x13  // 接收频道0接收数据长度
#define RX_PW_P3        0x14  // 接收频道0接收数据长度
#define RX_PW_P4        0x15  // 接收频道0接收数据长度
#define RX_PW_P5        0x16  // 接收频道0接收数据长度
#define FIFO_STATUS     0x17  // FIFO栈入栈出状态寄存器设置
//=============================RF24l01状态=====================================
char  TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//本地地址
char  RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//接收地址
char  sta;
char  RxBuf[32],temp[10];
char seg[10]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};
//===========================RF24L01端口设置==========================================
void RF24L01_IO_set(void)
{
      P2DIR &= 0x7f;    P2DIR |= 0x40;  P2SEL&=0x3F;   P2IE=P2IE&0x7f;
      P3DIR &= 0xFE;    P3DIR |= 0x0E;  P3SEL&=0xF0;
}
