//===================================LCD========================================
#define  DataDir             P4DIR              
#define  DataPort            P4OUT	
//==============================================================================
#define  Busy	             0x80
//==============================================================================
#define  CtrlDir             P6DIR
#define  CLR_RS              P6OUT&=~BIT6;         //RS = P6.6	
#define  SET_RS              P6OUT|=BIT6;	
//==============================================================================
#define  CLR_RW              P6OUT&=~BIT5;	  //RW = P6.5
#define  SET_RW              P6OUT|=BIT5;	
//==============================================================================
#define  CLR_EN              P6OUT&=~BIT4;	  //EN = P6.4
#define  SET_EN              P6OUT|=BIT4;	
//==============================================================================
//==============================================================================
#define CPU_F ((double)8000000)
#define delay_us(x)  __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x)  __delay_cycles((long)(CPU_F*(double)x/1000.0))
//==============================================================================
char H=100;//ˮ��
void LCD_IO_set()
  {
    CtrlDir |= 0x70;                 //�����߶˿���Ϊ���״̬ 
    DataDir  = 0xFF;                 //���ݶ˿���Ϊ���״̬
    P4SEL=0x00;                    //����P4Ϊһ������IO��   
    P6SEL&=0x8f;                  //����P6Ϊһ������IO��  
  }
//========================�ȴ�1602Һ������ڲ�����==============================
void WaitForEnable(void) 
{
    P4DIR &= 0x00;  //��P4���л�Ϊ����״̬
    CLR_RS;
    SET_RW;
    _NOP();
    SET_EN; 
    _NOP();
    _NOP();
    while((P4IN & Busy)!=0);  //���æ��־
    CLR_EN;
    P4DIR |= 0xFF;  //��P4���л�Ϊ���״̬
}	
//========================��ʱԼ5ms=============================================
void ms_delay(void)
{ 
   unsigned int i=40000;
    while (i != 0)
    {
        i--;
    }
}
//========================================����ʱ================================
void Delay(int s)
{
	unsigned int i,j;
	for(i=0; i<s; i++);
	for(j=0; j<s; j++);
}                  
//==========================��Һ��ģ��д������==================================
//=================cmd--���chk--�Ƿ���æ�ı�־��1����æ��0������=============
void LcdWriteCommand(char cmd,char chk) 
{

    if (chk) WaitForEnable();   // ���æ�ź�?
    
    CLR_RS;	
    CLR_RW; 
    _NOP();

    DataPort = cmd;             //��������д�����ݶ˿� 
    _NOP();					
    
    SET_EN;                     //����ʹ�������ź�
    _NOP();
    _NOP();
    CLR_EN;			
}
//=====================��Һ����ʾ�ĵ�ǰ��ַд����ʾ����=========================
void LcdWriteData(char data ) 
{
    WaitForEnable();        //�ȴ�Һ����æ
    SET_RS;
    CLR_RW; 
    _NOP();
    DataPort = data;        //����ʾ����д�����ݶ˿�
    _NOP();
    SET_EN;                 //����ʹ�������ź�
    _NOP(); 
    _NOP(); 
    CLR_EN;		
}
//==================��Һ��������ʾ�ַ�λ�õ�������Ϣ============================
void LocateXY(char x,char y) 
{
   char temp;
    x&= 0x01;
    temp = y&0x0f;
    if(x)   temp |= 0x40;  //����ڵ�2��
    temp |= 0x80;
    LcdWriteCommand(temp,0);
}
//==========================��ĳ��λ����ʾһ���ַ�==============================
//============x--λ�õ�������   y--λ�õ�������data--��ʾ���ַ�����=============
void DisoneChar(char x,char y,char data) 
{
    LocateXY( x, y );			
    LcdWriteData( data );		
}
//==============================��Һ����ĳ��λ����������ʾһ���ַ�==============
//==============================y--λ�õ�������  x--λ�õ�������================
void DispStr(char x,char y,char *ptr) 
{
   char *temp;
   char i,n = 0;
    
    temp = ptr;
    while(*ptr++ != '\0')   n++;    //�����ַ�����Ч�ַ��ĸ���
    
    for (i=0;i<n;i++)
    {
        DisoneChar(x++,y,temp[i]);
        if (x == 0x0f)
        {
           x  = 0; 
           y ^= 1;
        }
    }
}
//��Һ����ĳ��λ����������ʾN���ַ�
void DispNChar(char x,char y,char n,char *ptr) 
{
   char i;
    
    for (i=0;i<n;i++)
    {
        DisoneChar(x++,y,ptr[i]);
        if (x == 0x0f)
        {
           x = 0; 
           y ^= 1;
        }
    }
}
//========================��1602Һ��ģ����и�λ����============================
void LcdReset(void) 
{
    LcdWriteCommand(0x38, 0);	    //�涨�ĸ�λ����
    ms_delay();
    LcdWriteCommand(0x38, 0);		
    ms_delay();
    LcdWriteCommand(0x38, 0);
    ms_delay();
    LcdWriteCommand(0x38, 1);		//��ʾģʽ����
    LcdWriteCommand(0x08, 1);		//��ʾ�ر�
    LcdWriteCommand(0x01, 1);	        //��ʾ����
    LcdWriteCommand(0x06, 1);		//д�ַ�ʱ���岻�ƶ�
    LcdWriteCommand(0x0c, 1);		//��ʾ���������α꣬����˸
   
}
