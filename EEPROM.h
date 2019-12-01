#ifndef	_EEPROM_H_
#define	_EEPROM_H_

#include <reg51.h>
#include <intrins.h>

typedef unsigned char BYTE;
typedef unsigned int WORD;

sfr  IAP_DATA   = 0xC2;      
sfr  IAP_ADDRH  = 0xC3;      
sfr  IAP_ADDRL  = 0xC4;       
sfr  IAP_CMD    = 0xC5;      
sfr  IAP_TRIG   = 0xC6;      
sfr  IAP_CONTR  = 0xC7;      
 
#define   CMD_IDLE      0     
#define   CMD_READ      1    
#define   CMD_WRITE     2     
#define   CMD_ERASE     3    
 
//#define   ENABLE_IAP   0x80    
//#define   ENABLE_IAP   0x81    
#define   ENABLE_IAP   0x82    
//#define   ENABLE_IAP   0x83     
//#define   ENABLE_IAP   0x84     
//#define   ENABLE_IAP   0x85    
//#define   ENABLE_IAP   0x86    
//#define   ENABLE_IAP   0x87   

#define    IAP_ADDRESS1   0x0000   
#define    IAP_ADDRESS2   0x0200  
#define    ID_ADDR_RAM 0xf1        

//extern void DelayEEPROM(BYTE n);
//extern void IapIdle();                              
//extern BYTE IapReadByte(WORD addr);
//extern void IapWriteByte(WORD addr, BYTE dat);
//extern void IapEraseSector(WORD addr);             
extern void EEPROMcopyUID();
extern BYTE EEPROMCompare();
extern void EEPROMRead(BYTE * addr, WORD size);
extern BYTE EEPROMWrite(BYTE * addr, WORD size);
extern void EEPROMGetMAC(BYTE * MAC, WORD size);

#endif
