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
char H=100;//水高
void LCD_IO_set()
  {
    CtrlDir |= 0x70;                 //控制线端口设为输出状态 
    DataDir  = 0xFF;                 //数据端口设为输出状态
    P4SEL=0x00;                    //设置P4为一般数字IO口   
    P6SEL&=0x8f;                  //设置P6为一般数字IO口  
  }
//========================等待1602液晶完成内部操作==============================
void WaitForEnable(void) 
{
    P4DIR &= 0x00;  //将P4口切换为输入状态
    CLR_RS;
    SET_RW;
    _NOP();
    SET_EN; 
    _NOP();
    _NOP();
    while((P4IN & Busy)!=0);  //检测忙标志
    CLR_EN;
    P4DIR |= 0xFF;  //将P4口切换为输出状态
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
//========================================长延时================================
void Delay(int s)
{
	unsigned int i,j;
	for(i=0; i<s; i++);
	for(j=0; j<s; j++);
}                  
//==========================向液晶模块写入命令==================================
//=================cmd--命令，chk--是否判忙的标志，1：判忙，0：不判=============
void LcdWriteCommand(char cmd,char chk) 
{

    if (chk) WaitForEnable();   // 检测忙信号?
    
    CLR_RS;	
    CLR_RW; 
    _NOP();

    DataPort = cmd;             //将命令字写入数据端口 
    _NOP();					
    
    SET_EN;                     //产生使能脉冲信号
    _NOP();
    _NOP();
    CLR_EN;			
}
//=====================向液晶显示的当前地址写入显示数据=========================
void LcdWriteData(char data ) 
{
    WaitForEnable();        //等待液晶不忙
    SET_RS;
    CLR_RW; 
    _NOP();
    DataPort = data;        //将显示数据写入数据端口
    _NOP();
    SET_EN;                 //产生使能脉冲信号
    _NOP(); 
    _NOP(); 
    CLR_EN;		
}
//==================向液晶输入显示字符位置的坐标信息============================
void LocateXY(char x,char y) 
{
   char temp;
    x&= 0x01;
    temp = y&0x0f;
    if(x)   temp |= 0x40;  //如果在第2行
    temp |= 0x80;
    LcdWriteCommand(temp,0);
}
//==========================在某个位置显示一个字符==============================
//============x--位置的列坐标   y--位置的行坐标data--显示的字符数据=============
void DisoneChar(char x,char y,char data) 
{
    LocateXY( x, y );			
    LcdWriteData( data );		
}
//==============================让液晶从某个位置起连续显示一个字符==============
//==============================y--位置的列坐标  x--位置的行坐标================
void DispStr(char x,char y,char *ptr) 
{
   char *temp;
   char i,n = 0;
    
    temp = ptr;
    while(*ptr++ != '\0')   n++;    //计算字符串有效字符的个数
    
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
//让液晶从某个位置起连续显示N个字符
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
//========================对1602液晶模块进行复位操作============================
void LcdReset(void) 
{
    LcdWriteCommand(0x38, 0);	    //规定的复位操作
    ms_delay();
    LcdWriteCommand(0x38, 0);		
    ms_delay();
    LcdWriteCommand(0x38, 0);
    ms_delay();
    LcdWriteCommand(0x38, 1);		//显示模式设置
    LcdWriteCommand(0x08, 1);		//显示关闭
    LcdWriteCommand(0x01, 1);	        //显示清屏
    LcdWriteCommand(0x06, 1);		//写字符时整体不移动
    LcdWriteCommand(0x0c, 1);		//显示开，不开游标，不闪烁
   
}
