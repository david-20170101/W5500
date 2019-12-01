#include <reg51.h>
#include "DS18B20.h"

#ifdef OSC_FREQ_8
void delay10us()  
{
    TMOD |= 0X10;
    TH1  = 0xff;
    TL1  = 0xf9;
    
    TF1 = 0; 
    TR1 = 1; 
    while(TF1 == 0);
    TR1 = 0;
}
void delay20us() 
{
    TMOD |= 0X10;
    TH1  = 0xff;
    TL1  = 0xf2;
    
    TF1 = 0; 
    TR1 = 1; 
    while(TF1 == 0);
    TR1 = 0;
}
void delay30us()
{
    TMOD |= 0X10;
    TH1  = 0xff;
    TL1  = 0xec;
    
    TF1 = 0; 
    TR1 = 1; 
    while(TF1 == 0);
    TR1 = 0; 
}
void delay70us() 
{
    TMOD |= 0X10;
    TH1  = 0xff;
    TL1 = 0xd1;
    
    TF1 = 0; 
    TR1 = 1; 
    while(TF1 == 0);
    TR1 = 0; 
}
void delay500us() 
{
    TMOD |= 0X10;
    TH1  = 0xfe;
    TL1  = 0xb2;
    
    TF1 = 0; 
    TR1 = 1; 
    while(TF1 == 0);
    TR1 = 0; 
}
#endif

#ifdef OSC_FREQ_11059200
void delay10us() 
{
    TMOD |= 0X10;
    TH1  = 0xff;
    TL1  = 0xf6;
    
    TF1 = 0; 
    TR1 = 1; 
    while(TF1 == 0);
    TR1 = 0;
}
void delay20us() 
{
    TMOD |= 0X10;
    TH1  = 0xff;
    TL1  = 0xed;
    
    TF1 = 0; 
    TR1 = 1; 
    while(TF1 == 0);
    TR1 = 0; 
}
void delay30us() 
{
    TMOD |= 0X10;
    TH1  = 0xff;
    TL1  = 0xe4;
    
    TF1 = 0; 
    TR1 = 1; 
    while(TF1 == 0);
    TR1 = 0; 
}
void delay70us() 
{
    TMOD |= 0X10;
    TH1  = 0xff;
    TL1 = 0xbf;
    
    TF1 = 0; 
    TR1 = 1; 
    while(TF1 == 0);
    TR1 = 0;
}
void delay500us()
{
    TMOD |= 0X10;
    TH1  = 0xfe;
    TL1  = 0x33;
    
    TF1 = 0; 
    TR1 = 1; 
    while(TF1 == 0);
    TR1 = 0; 
}
#endif
/*****************DS18B20******************/ 
unsigned char Init_Ds18b20(void)       
{
  unsigned char x=0;
  W5500_DS18B20 = 1;              
  delay10us();                      
  W5500_DS18B20 = 0;                 
  delay500us();             
  W5500_DS18B20 = 1;          
  delay70us();               
  x = W5500_DS18B20;
  delay500us();           
  return x;
}

unsigned char Read_One_Byte()      
{                                  
  unsigned char i   = 0;
  unsigned char dat = 0;
  for(i=8;i>0;i--)
  {
    W5500_DS18B20 = 0;          
    _nop_();               
    dat >>= 1;              
    W5500_DS18B20 = 1;      
    delay10us();    
    if(W5500_DS18B20)    
    {
      dat |= 0x80;       
    }        
    delay20us();   
  }
  return (dat);
}

void Write_One_Byte(unsigned char dat)
{
  unsigned char i = 0;
  for(i=8;i>0;i--)
  {
    W5500_DS18B20 = 0;   
    _nop_();        
    W5500_DS18B20 = dat&0x01;    
    delay30us();            
    W5500_DS18B20 = 1;    
    dat >>= 1;
    delay10us();
  }
}
unsigned int Get_DS18B20_Tmp()    
{
  float tt;
  unsigned int temp;
  unsigned char TH,TL;
  if(Init_Ds18b20())
  {
	temp = 0;
  } else {
    Write_One_Byte(0xcc);     
    Write_One_Byte(0x44);     
    Init_Ds18b20();       
    Write_One_Byte(0xcc);     
    Write_One_Byte(0xbe);     
    TH  = Read_One_Byte();       
    TL  = Read_One_Byte();         
    temp = TL;
    temp <<=8;
    temp = temp | TH;
    tt = temp*0.0625;            
    temp = tt*10+0.5;            
  }
  return temp;
}
void Show_Ds18B20(unsigned char * dat)
{
  unsigned int tmp;
  tmp = Get_DS18B20_Tmp();
  dat[47] = 48+tmp/100;     
  dat[48] = 48+tmp%100/10;   
  dat[50] = 48+tmp%10;     
}
