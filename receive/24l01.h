//==============================================================================
#define  RF24L01_CE_0        P3OUT &=~BIT1         
#define  RF24L01_CE_1        P3OUT |= BIT1        
//=============================RF24L01_CSN�˿�==================================
#define  RF24L01_CSN_0       P3OUT &=~BIT3         
#define  RF24L01_CSN_1       P3OUT |= BIT3     
//=============================RF24L01_SCK======================================
#define  RF24L01_SCK_0       P3OUT &=~BIT2      
#define  RF24L01_SCK_1       P3OUT |= BIT2   
//=============================MISO�˿�=========================================
#define  RF24L01_MISO_0      P3OUT &=~BIT0
#define  RF24L01_MISO_1      P3OUT |= BIT0
//============================= RF24L01_MOSI�˿�================================
#define  RF24L01_MOSI_0      P2OUT &=~BIT6
#define  RF24L01_MOSI_1      P2OUT |= BIT6
//==========================IRQ״̬============================================
#define  RF24L01_IRQ_0       P2OUT &=~BIT7       
#define  RF24L01_IRQ_1       P2OUT |= BIT7
//==========================NRF24L01============================================
#define TX_ADR_WIDTH    5   	// 5 uints TX address width
#define RX_ADR_WIDTH    5   	// 5 uints RX address width
#define TX_PLOAD_WIDTH  32  	// 32 TX payload
#define RX_PLOAD_WIDTH  32  	// 32 uints TX payload
//=========================NRF24L01�Ĵ���ָ��===================================
#define READ_REG        0x00  	// ���Ĵ���ָ��
#define WRITE_REG       0x20 	// д�Ĵ���ָ��
#define RD_RX_PLOAD     0x61  	// ��ȡ��������ָ��
#define WR_TX_PLOAD     0xA0  	// д��������ָ��
#define FLUSH_TX        0xE1 	// ��ϴ���� FIFOָ��
#define FLUSH_RX        0xE2  	// ��ϴ���� FIFOָ��
#define REUSE_TX_PL     0xE3  	// �����ظ�װ������ָ��
#define NOP1            0xFF  	// ����
//========================SPI(nRF24L01)�Ĵ�����ַ===============================
#define CONFIG          0x00  // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define EN_AA           0x01  // �Զ�Ӧ��������
#define EN_RXADDR       0x02  // �����ŵ�����
#define SETUP_AW        0x03  // �շ���ַ�������
#define SETUP_RETR      0x04  // �Զ��ط���������
#define RF_CH           0x05  // ����Ƶ������
#define RF_SETUP        0x06  // �������ʡ����Ĺ�������
#define STATUS          0x07  // ״̬�Ĵ���
#define OBSERVE_TX      0x08  // ���ͼ�⹦��
#define CD              0x09  // ��ַ���           
#define RX_ADDR_P0      0x0A  // Ƶ��0�������ݵ�ַ
#define RX_ADDR_P1      0x0B  // Ƶ��1�������ݵ�ַ
#define RX_ADDR_P2      0x0C  // Ƶ��2�������ݵ�ַ
#define RX_ADDR_P3      0x0D  // Ƶ��3�������ݵ�ַ
#define RX_ADDR_P4      0x0E  // Ƶ��4�������ݵ�ַ
#define RX_ADDR_P5      0x0F  // Ƶ��5�������ݵ�ַ
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        0x11  // ����Ƶ��0�������ݳ���
#define RX_PW_P1        0x12  // ����Ƶ��0�������ݳ���
#define RX_PW_P2        0x13  // ����Ƶ��0�������ݳ���
#define RX_PW_P3        0x14  // ����Ƶ��0�������ݳ���
#define RX_PW_P4        0x15  // ����Ƶ��0�������ݳ���
#define RX_PW_P5        0x16  // ����Ƶ��0�������ݳ���
#define FIFO_STATUS     0x17  // FIFOջ��ջ��״̬�Ĵ�������
//=============================RF24l01״̬=====================================
char  TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//���ص�ַ
char  RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//���յ�ַ
char  sta;
char  RxBuf[32],temp[10];
char seg[10]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};
//===========================RF24L01�˿�����==========================================
void RF24L01_IO_set(void)
{
      P2DIR &= 0x7f;    P2DIR |= 0x40;  P2SEL&=0x3F;   P2IE=P2IE&0x7f;
      P3DIR &= 0xFE;    P3DIR |= 0x0E;  P3SEL&=0xF0;
}
