#include <reg51.h>
#include "EEPROM.h"	

/*----------------------------Software delay function----------------------------*/
void DelayEEPROM(BYTE n)
{
    WORD x;
    while (n--)
    {
        x = 0;
        while (++x);
    }
}

void IapIdle()
{
    IAP_CONTR = 0;      
    IAP_CMD   = 0;      
    IAP_TRIG  = 0;       
    IAP_ADDRH = 0xFF;    
    IAP_ADDRL = 0xFF;
}

BYTE IapReadByte(WORD addr)
{
    BYTE dat;       
    IAP_CONTR = ENABLE_IAP;   
    IAP_CMD   = CMD_READ;    
    IAP_ADDRL = addr;        
    IAP_ADDRH = addr >> 8;   
    IAP_TRIG  = 0x5a;        
    IAP_TRIG  = 0xa5;       
    _nop_();               
    _nop_();                 
    _nop_();                
    dat = IAP_DATA;         
    IapIdle();               
    return dat;             
}

void IapWriteByte(WORD addr, BYTE dat)
{
    IAP_CONTR = ENABLE_IAP;   
    IAP_CMD   = CMD_WRITE;    
    IAP_ADDRL = addr;         
    IAP_ADDRH = addr >> 8;    
    IAP_DATA  = dat;          
    IAP_TRIG  = 0x5a;        
    IAP_TRIG  = 0xa5;        
    _nop_();                 
    _nop_();                
    _nop_();               
    IapIdle();              
}

void IapEraseSector(WORD addr)
{
    IAP_CONTR = ENABLE_IAP;   
    IAP_CMD   = CMD_ERASE;   
    IAP_ADDRL = addr;        
    IAP_ADDRH = addr >> 8;   
    IAP_TRIG  = 0x5a;        
    IAP_TRIG  = 0xa5;        
    _nop_();                 
    _nop_();                
    _nop_();               
    IapIdle();              
}

///////////////////////////////////////////////////////////////////	
BYTE EEPROMCompare()
{
  BYTE i,value;
  BYTE idata *iptr; 
  iptr = ID_ADDR_RAM;

  DelayEEPROM(10);
  for (i=0; i<7; i++)                       
  {
    value = *iptr++;
	if (IapReadByte(IAP_ADDRESS2+i) != value)
        goto Error;                         
  }
  DelayEEPROM(10);
  return 0x01;

  Error:
  DelayEEPROM(10);
  return 0x00;
}
void EEPROMGetMAC(BYTE * MAC, WORD size)
{
  WORD k;

  for (k=0; k<size; k++)
  {
	MAC[k] = IapReadByte(IAP_ADDRESS2+k+7);
  }
}
BYTE EEPROMWrite(BYTE * addr, WORD size)
{
  WORD i; 
  DelayEEPROM(10);
  IapEraseSector(IAP_ADDRESS1);               
  for (i=0; i<512; i++)                       
  {
    if (IapReadByte(IAP_ADDRESS1+i) != 0xff)
        goto Error;                            
  }
  DelayEEPROM(10);
  for (i=0; i<size; i++)                     
  {
    IapWriteByte(IAP_ADDRESS1+i, addr[i]);
  }
  DelayEEPROM(10);
  for (i=0; i<size; i++)                     
  {
    if (IapReadByte(IAP_ADDRESS1+i) != addr[i])
        goto Error;                          
  }
  DelayEEPROM(10);
  return 0x01;

  Error:
  DelayEEPROM(10);
  return 0x00;
}
void EEPROMRead(BYTE * addr, WORD size)
{
  WORD i; 
  DelayEEPROM(10);
  for (i=0; i<size; i++) 
  {
    addr[i] = IapReadByte(IAP_ADDRESS1+i);
  }
  DelayEEPROM(10);
}
