//===========================DS18B20端口========================================
#define  OUT_DS18B20_0     P6OUT &=~BIT7          //输出0
#define  OUT_DS18B20_1     P6OUT |= BIT7          //输出1
#define  DS18B20_IN        P6DIR &=~BIT7          //方向设置为输入
#define  DS18B20_OUT       P6DIR |= BIT7          //方向设置为输出
//============================DS18B20寄存器指令=================================
#define DS18B20_READ_ROM          0x33
#define DS18B20_MATCH_ROM         0X55
#define DS18B20_SKIP_ROM          0XCC
#define DS18B20_SEARCH_ROM        0XF0
#define DS18B20_ALARM_SEARCH_ROM  0XEC
#define DS18B20_WRITE_RAM         0X40
#define DS18B20_READ_RAM          0XBE
#define DS18B20_COPY_RAM          0X48 
#define DS18B20_CONVERT_TEM       0X44
#define DS18B20_EECALL_EEPROM     0XB8
#define DS18B20_READ_POWER_SUPPLY 0XB4
//==============================================================================
#define CPU_F ((double)8000000)
#define delay_us(x)  __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x)  __delay_cycles((long)(CPU_F*(double)x/1000.0))
//==============================================================================
void DS18B20_IO_set(void)
{
  P6SEL&=0x7f; 
}
//===========================DS18B20相关变量====================================
char temp_data[2]={0x00,0x00};
char dispaly[32];
long int d=99,width=99;
char distance;
//========================================长延时================================
void Delay(int s)
{
	unsigned int i,j;
	for(i=0; i<s; i++);
	for(j=0; j<s; j++);
}
//===========================DS18B20延时========================================  
void DS1820_delay(int i)
{
	while(i--);
}

//==========================DS18B20复位=========================================
void ds18b20_reset(void) 
{ 

  DS18B20_OUT; //
  OUT_DS18B20_1;
  OUT_DS18B20_0;
  delay_us(500);
  //DS1820_delay(3000);  //500us
  OUT_DS18B20_1;
  DS18B20_IN;    //设置为输入
  //delay_us(5);
  DS1820_delay(40);
  while(P6IN&BIT7);
  DS18B20_OUT;
  OUT_DS18B20_1;
  //delay_us(50);
  DS1820_delay(300);
} 
//========================DS18B20读操作函数=====================================
char ds18b20_read(void) 
{ 
  char i;
  char temp=0;
  for(i=0;i<8;i++)
  {
   DS18B20_OUT;
   OUT_DS18B20_0;
   temp>>=1;
   OUT_DS18B20_1;
   DS18B20_IN;
   if(P6IN&BIT7)
   {
     temp|=0x80;
   }
   //delay_us(1);
   DS1820_delay(4);
     DS18B20_OUT;
     //delay_us(50);
     DS1820_delay(50);
  }
return (temp);
}
//========================DS18B20写操作函数=====================================
void ds18b20_write(char Date) 
{ 
  char i; 
    DS18B20_OUT;
     for(i=0;i<8;i++)
     {
       OUT_DS18B20_0;
      // DS1820_delay(6);
       if(Date & 0x01) 
       {
         OUT_DS18B20_1;
       }
       else
       {
         OUT_DS18B20_0;
       }
       //delay_us(6.67);
       DS1820_delay(40);
       OUT_DS18B20_1;
       Date>>=1;
     }
       delay_us(1);//DS1820_delay(8);
} 

//======================DS18B20温度转换函数=====================================
void Ds18b20Convert(void) 
{ 
   ds18b20_reset(); 
  ds18b20_write(0xCC); 
  ds18b20_write(0x44);
  
  delay_ms(5);//Delay(200);
    
  ds18b20_reset(); 
  ds18b20_write(0xCC); //??????
  ds18b20_write(0xBE); //?RAM??
  
  temp_data[0]=ds18b20_read(); 
  temp_data[1]=ds18b20_read();
}
//=========================温度换算处理=========================================
void work_temp()
{
	char n=0;
if(temp_data[1]>127)
	{
	temp_data[1]=(256-temp_data[1]);			//负值
	temp_data[0]=(256-temp_data[0]);
	n=1;
	}
        distance=(d-65)%100;
        dispaly[8]=distance%10;
        dispaly[7]=(distance/10)%10;
	dispaly[6]=((temp_data[0]&0xf0)>>4)|((temp_data[1]&0x0f)<<4);
	dispaly[5]=dispaly[6]/100;				//百位
	dispaly[4]=dispaly[6]%100;				//
	dispaly[2]=dispaly[4]/10;				//十位
	dispaly[1]=dispaly[4]%10;				//个位	
	switch	(temp_data[0]&0x0f)				//小数位
	{
	case 0x0f:dispaly[0]=9;break;
	case 0x0e:dispaly[0]=9;break;
	case 0x0d:dispaly[0]=8;break;
	case 0x0c:dispaly[0]=8;break;
	case 0x0b:dispaly[0]=7;break;
	case 0x0a:dispaly[0]=6;break;
	case 0x09:dispaly[0]=6;break;
	case 0x08:dispaly[0]=5;break;
	case 0x07:dispaly[0]=4;break;
	case 0x06:dispaly[0]=4;break;
	case 0x05:dispaly[0]=3;break;
	case 0x04:dispaly[0]=3;break;
	case 0x03:dispaly[0]=2;break;
	case 0x02:dispaly[0]=1;break;
	case 0x01:dispaly[0]=1;break;
	case 0x00:dispaly[0]=1;break;
	default:break;
	}
if(n)										//负值时显示aa,正直显示dd
	{
	dispaly[3]=0x11;						//
	}
else 	dispaly[3]=0x22;
}