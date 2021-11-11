#define BIT(x) 1<<x
char jh[10]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};
//==================================BELL========================================
#define  BELL_0     P6OUT &=~BIT3          //输出0
#define  BELL_1     P6OUT |= BIT3          //输出1
void INIT_anjian()
{
  P1SEL=0x00;
  P1DIR=0x00;

}
int flashflag_1=0,flashflag_2=0,flash_1=0,flash_2=0;
void setflash()
{
   int *Flash_ptra;
  Flash_ptra=(int *)0x1090;
  FCTL2=FWKEY+FSSEL0+FN0;
  FCTL1=FWKEY+ERASE;
  FCTL3=FWKEY;
  *Flash_ptra=0;
  FCTL1=FWKEY+WRT;
  *Flash_ptra=flash_1*100+flash_2;
  FCTL1=FWKEY;
  FCTL3=FWKEY+LOCK;
}
//====================================// 设置警戒线界面  //===============================
void jjjiemain()  
{
   if((P1IN&BIT(0))==BIT(0))
     {
    while((P1IN&BIT(0))==BIT(0));
    LcdWriteCommand(0x01, 1);	        //显示清屏
    delay_ms(5);
    DisoneChar(0,0,0x41);
    DisoneChar(0,1,0x6c);
    DisoneChar(0,2,0x61);
    DisoneChar(0,3,0x72);
    DisoneChar(0,4,0x6d);
    DisoneChar(0,5,0x20);
    DisoneChar(0,6,0x68);
    DisoneChar(0,7,0x69);
    DisoneChar(0,8,0x67);
    DisoneChar(0,9,0x68);
    DisoneChar(0,10,0x3a);
     ms_delay();
    DisoneChar(1,5,0x5f);
    DisoneChar(1,6,0x5f);
    DisoneChar(1,7,0x63);
    DisoneChar(1,8,0x6d);
     ms_delay();
     int m=0,n=0;
    while(1)
{
if((P1IN&BIT(3))==BIT(3))
     {
      while((P1IN&BIT(3))==BIT(3));
       m++;
       if(m==10) m=0;
       DisoneChar(1,5,jh[m]);
        ms_delay();
      }
if((P1IN&BIT(2))==BIT(2))
     {
      while((P1IN&BIT(2))==BIT(2));
       n++;
       if(n==10) n=0;
       DisoneChar(1,6,jh[n]);
        ms_delay();
      } 
 if((P1IN&BIT(0))==BIT(0))
     {
    while((P1IN&BIT(0))==BIT(0));
     flash_2=10*m+n;
     if(flashflag_1==0)
     {
     int *Flash_ptra;
     Flash_ptra=( int *)0x1090;  //读flash值
     flash_1=*Flash_ptra/100;
     }
     setflash();
     flashflag_2=1;
    break;
     }
}
 LcdWriteCommand(0x01, 1);	        //显示清屏
 delay_ms(5);
  
      }
}
//====================================// 设置高度界面  //===============================
void thjiemain()  
{
   if((P1IN&BIT(5))==BIT(5))
     {
    while((P1IN&BIT(5))==BIT(5));
    LcdWriteCommand(0x01, 1);	        //显示清屏
    delay_ms(5);
    DisoneChar(0,0,0x54);
    DisoneChar(0,1,0x6f);
    DisoneChar(0,2,0x74);
    DisoneChar(0,3,0x61);
    DisoneChar(0,4,0x6c);
    DisoneChar(0,5,0x20);
    DisoneChar(0,6,0x68);
    DisoneChar(0,7,0x69);
    DisoneChar(0,8,0x67);
    DisoneChar(0,9,0x68);
    DisoneChar(0,10,0x3a);
     ms_delay();
    DisoneChar(1,5,0x5f);
    DisoneChar(1,6,0x5f);
    DisoneChar(1,7,0x63);
    DisoneChar(1,8,0x6d);
     ms_delay();
     int m=0,n=0;
    while(1)
{
if((P1IN&BIT(3))==BIT(3))
     {
      while((P1IN&BIT(3))==BIT(3));
       m++;
       if(m==10) m=0;
       DisoneChar(1,5,jh[m]);
        ms_delay();
      }
if((P1IN&BIT(2))==BIT(2))
     {
      while((P1IN&BIT(2))==BIT(2));
       n++;
       if(n==10) n=0;
       DisoneChar(1,6,jh[n]);
        ms_delay();
      } 
 if((P1IN&BIT(5))==BIT(5))
     {
    while((P1IN&BIT(5))==BIT(5));
     flash_1=10*m+n;
     if(flashflag_2==0)
     {
     int *Flash_ptra;
     Flash_ptra=( int *)0x1090;  //读flash值
     flash_2=*Flash_ptra%100;
     }
     setflash();
     flashflag_1=1;
    break;
     }
}
 LcdWriteCommand(0x01, 1);	        //显示清屏
 delay_ms(5);
      }
}
//====================================温度 显示 ================================
void wen_xianshi()
{
     
    int *Flash_ptra;
     Flash_ptra=( int *)0x1090;  //读flash值
     char jjx[2],yy[2],d;
     d=temp[7]*10+temp[8];
     jjx[1]=(char)((*Flash_ptra%100)/10);
     jjx[0]=(char)((*Flash_ptra%100)%10);
     yy[1]=(char)((*Flash_ptra/100-d)/10);
     yy[0]=(char)((*Flash_ptra/100-d)%10);
     DisoneChar(0,0,0x54) ; // T:xx.x度
     DisoneChar(0,1,0x3a) ;
     DisoneChar(0,2,seg[temp[4]]) ;
     DisoneChar(0,3,seg[temp[5]]) ;
     DisoneChar(0,4,0x2e) ;
     DisoneChar(0,5,seg[temp[1]]) ;
     DisoneChar(0,6,0xdf) ;
     DisoneChar(0,7,0x43) ;
     ms_delay();
     DisoneChar(0,14,seg[jjx[1]]) ;
     DisoneChar(0,15,seg[jjx[0]]) ;
     ms_delay();
     DisoneChar(1,0,0x68) ;//h:xxcm
     DisoneChar(1,1,0x3a) ;
     DisoneChar(1,2,seg[yy[1]]) ;
     DisoneChar(1,3,seg[yy[0]]) ;
     DisoneChar(1,4,0x63) ;
     DisoneChar(1,5,0x6d) ;
     ms_delay();
}
//========================开机界面============================
void Lcdstart()
{
  
    DisoneChar(0,0,0x20);
    DisoneChar(0,1,0x20);
    DisoneChar(0,2,0x20);
    DisoneChar(0,3,0x20);
    DisoneChar(0,4,0x57);
    DisoneChar(0,5,0x65);
    DisoneChar(0,6,0x6c);
    DisoneChar(0,7,0x63);
    DisoneChar(0,8,0x6f);
    DisoneChar(0,9,0x6d);
    DisoneChar(0,10,0x65);
    DisoneChar(0,11,0x21);
    delay_ms(1000);
    LcdWriteCommand(0x01, 1);	        //显示清屏
     int *Flash_ptra;
     Flash_ptra=( int *)0x1090;  //读flash值
     char jjx[2];
     jjx[1]=(char)((*Flash_ptra/100)/10);
     jjx[0]=(char)((*Flash_ptra/100)%10);
    DisoneChar(0,0,0x54);   //zonggaodu
    DisoneChar(0,1,0x6f);
    DisoneChar(0,2,0x74);
    DisoneChar(0,3,0x61);
    DisoneChar(0,4,0x6c);
    DisoneChar(0,5,0x20);
    DisoneChar(0,6,0x68);
    DisoneChar(0,7,0x69);
    DisoneChar(0,8,0x67);
    DisoneChar(0,9,0x68);
    DisoneChar(0,10,0x3a);
     ms_delay();
    DisoneChar(1,5,seg[jjx[1]]);
    DisoneChar(1,6,seg[jjx[0]]);
    DisoneChar(1,7,0x63);
    DisoneChar(1,8,0x6d);
      delay_ms(1000);
     LcdWriteCommand(0x01, 1);	        //显示清屏
     jjx[1]=(char)((*Flash_ptra%100)/10);
     jjx[0]=(char)((*Flash_ptra%100)%10);
     DisoneChar(0,0,0x41);
    DisoneChar(0,1,0x6c);
    DisoneChar(0,2,0x61);
    DisoneChar(0,3,0x72);
    DisoneChar(0,4,0x6d);
    DisoneChar(0,5,0x20);
    DisoneChar(0,6,0x68);
    DisoneChar(0,7,0x69);
    DisoneChar(0,8,0x67);
    DisoneChar(0,9,0x68);
    DisoneChar(0,10,0x3a);
     ms_delay();
    DisoneChar(1,5,seg[jjx[1]]);
    DisoneChar(1,6,seg[jjx[0]]);
    DisoneChar(1,7,0x63);
    DisoneChar(1,8,0x6d);
     delay_ms(1000);
     LcdWriteCommand(0x01, 1);	        //显示清屏
    
}
void baojin()
{
     unsigned int *Flash_ptra;
     Flash_ptra=(unsigned int *)0x1090;
     char d;
     d=temp[7]*10+temp[8];
  if((char)(*Flash_ptra%100)<=(char)(*Flash_ptra/100)-d)
	{
		BELL_1;  //打开蜂明器报警
	}
	else
	{
	 	BELL_0;//关闭蜂明器报警
	}
}
