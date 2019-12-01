#include <reg51.h> 	       
#include "UART.H"

sfr  IAP_CONTR   = 0xC7;     

void UART_Init(void)
{
    AUXR |= 0x04;   
    AUXR |= 0x01;   
    AUXR |= 0x10;   
    BRT   = 0xFD;  
    PCON &= 0x7F;   
 
    SCON = 0x50;    
    EA   = 1;	    
    ES   = 1;	    
}
 
void UART(void) interrupt 4
{
  unsigned char com_buff;
  if(RI == 1)   
  {
    RI = 0;
	com_buff = SBUF;
    if(com_buff==0x8A)
    {
      IAP_CONTR=0x60; 
    }
  }
}
