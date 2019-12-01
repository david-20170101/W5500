#include <reg51.h>	
#include "W5500.h"
#include "MQTT.h"
#include "DS18B20.h"

#define Serial_download 
#define Second_download
#define BUFFER_MAX_SIZE 90
#define OSC_FREQ_12

sbit W5500_KEY = P1^7;
bit led;
unsigned int num;

unsigned char xdata client[BUFFER_MAX_SIZE];
unsigned char xdata server[32];
unsigned char xdata message[52]={
0x7b, 0x22, 0x4d, 0x41, 0x43, 0x22, 0x3a, 0x22,
0x30, 0x43, 0x32, 0x39, 0x41, 0x42, 0x37, 0x43,
0x30, 0x30, 0x30, 0x31, 0x22, 0x2c, 0x22, 0xE5, 
0x88, 0x9D, 0xE5, 0xA7, 0x8B, 0xE5, 0x8C, 0x96, 
0x22, 0x3a, 0x22, 0x30, 0x22, 0x2c, 0x22, 0xE6,
0xB8, 0xA9, 0xE5, 0xBA, 0xA6, 0x22, 0x3a, 0x30,
0x30, 0x2e, 0x30, 0x7d};
unsigned char xdata response[30]={
0x7B, 0x22, 0x69, 0x64, 0x22, 0x3A, 0x22, 0xE5, 
0x88, 0x9D, 0xE5, 0xA7, 0x8B, 0xE5, 0x8C, 0x96, 
0x22, 0x2C, 0x22, 0x76, 0x61, 0x6C, 0x75, 0x65, 
0x22, 0x3A, 0x22, 0x31, 0x22, 0x7D};

void W5500_Initialization(void)
{
	W5500_Init();	
	Detect_Gateway();
	Socket_Init(0);	
}
void Load_Net_Parameters(unsigned char * dat)
{
	S0_Mode       =	dat[0];
	Phy_Addr[0]   = dat[1];   
	Phy_Addr[1]   = dat[2];
	Phy_Addr[2]   = dat[3];
	Phy_Addr[3]   = dat[4];
	Phy_Addr[4]   = dat[5];
	Phy_Addr[5]   = dat[6]; 
	Gateway_IP[0] = dat[7];  
	Gateway_IP[1] = dat[8];
	Gateway_IP[2] = dat[9];
	Gateway_IP[3] = dat[10];
	Sub_Mask[0]   = dat[11];  
	Sub_Mask[1]   = dat[12];
	Sub_Mask[2]   = dat[13];
	Sub_Mask[3]   = dat[14];
	IP_Addr[0]    = dat[15];   
	IP_Addr[1]    = dat[16];
	IP_Addr[2]    = dat[17];
	IP_Addr[3]    = dat[18];
	S0_Port[0]    = dat[19];  
	S0_Port[1]    = dat[20];
	S0_DIP[0]     = dat[21];  
	S0_DIP[1]     = dat[22];
	S0_DIP[2]     = dat[23];
	S0_DIP[3]     = dat[24];
	S0_DPort[0]   = dat[25];
	S0_DPort[1]   = dat[26];	
}
void W5500_Socket_Set(void)
{
	if(S0_State==0)
	{
		if(S0_Mode==TCP_SERVER)
		{
			if(Socket_Listen(0)==TRUE)
				S0_State=S_INIT;
			else
				S0_State=0;
		}
		else if(S0_Mode==TCP_CLIENT)
		{
			client[sizeof(client)-1] = 0x00; 
			client[sizeof(client)-2] = 0x00;
			if(Socket_Connect(0)==TRUE)
			{
				S0_State=S_INIT;
			} else {
				S0_State=0;
			}
		}
		else//UDP模式 
		{
			if(Socket_UDP(0)==TRUE)
				S0_State=S_INIT|S_CONN;
			else
				S0_State=0;
		}
	}
}
void Process_Socket_Data(SOCKET s)
{
	unsigned short size;
	size=Read_SOCK_Data_Buffer(s, Rx_Buffer);
	if(size>0)
	{
	  MqttReceive(client, sizeof(client), server, sizeof(server), size);
	}
}
void TR0_Init(void)
{
    TMOD = 0x01;

#ifdef OSC_FREQ_8
    TH0  = (65536-33333)/256;	
    TL0  = (65536-33333)%256;	
#endif
#ifdef OSC_FREQ_11059200
    TH0  = (65536-46080)/256;	
    TL0  = (65536-46080)%256;	
#endif
#ifdef OSC_FREQ_12
    TH0  = (65536-50000)/256;	
    TL0  = (65536-50000)%256;	
#endif

	EA   = 1;
	ET0  = 1;
	TR0  = 1;
}
void TR0_(void) interrupt 1
{
    TMOD = 0x01;

#ifdef OSC_FREQ_8
    TH0  = (65536-33333)/256;	
    TL0  = (65536-33333)%256;	
#endif
#ifdef OSC_FREQ_11059200
    TH0  = (65536-46080)/256;	 
    TL0  = (65536-46080)%256;	
#endif
#ifdef OSC_FREQ_12
    TH0  = (65536-50000)/256;	 
    TL0  = (65536-50000)%256;	
#endif

	num++;
}
void main(void)
{
    unsigned int MqttSize      =    0;
    unsigned int W5500_Counter =    0;
    unsigned int TimerInterval = 1200;
#ifdef Serial_download
    UART_Init();
#endif

#ifdef Second_download
  if(EEPROMCompare())
  {
    EEPROMRead(client, sizeof(client));
	if(client[0]==0x01)
    {
	  HexToMAC(client, message);
      Load_Net_Parameters(client);	
    } else {
	  EEPROMGetMAC(server, sizeof(server));
      Load_Net_Parameters(server);	
	}
	W5500_Hardware_Reset();		
	W5500_Initialization();		
	W5500_LED   = 0;                
	for(W5500_Counter=0; W5500_Counter<10; W5500_Counter++)
	{
	  Delay(100000);
	  W5500_LED = ~W5500_LED;
	}
	if(W5500_KEY==0)
	{
	  for(W5500_Counter=0; W5500_Counter<10; W5500_Counter++)
	  {
	    Delay(100000);
	    W5500_LED = ~W5500_LED;
	  }
	  if(W5500_KEY==0)
      {
	    W5500_LED = 1;   
		if( EEPROMWrite( server, sizeof(server) ) )
		{
		  W5500_LED = 0;   
		}
		while (1);
	  }
	}	                                                                   
	TR0_Init();
	while (1)
	{
      W5500_Socket_Set();   
      W5500_Interrupt_Process();
      if((S0_Data & S_RECEIVE) == S_RECEIVE)  
      {
        S0_Data&=~S_RECEIVE;
        Process_Socket_Data(0); 
      }
      else if(client[0]==0x01 && num>=TimerInterval)
      {
        num=0;
		if(S0_State == (S_INIT|S_CONN))
        {
          S0_Data&=~S_TRANSMITOK;
          Show_Ds18B20(message);
          W5500_LED = ~W5500_LED;
          MqttSize = MqttSend(client, sizeof(client), message, sizeof(message));
          Write_SOCK_Data_Buffer(0, Tx_Buffer, MqttSize);
        }
      } 
	}
  } else {
    W5500_LED = 0;  
    while(1)
    {
      W5500_LED = ~W5500_LED;
      Delay(100000);
    }
  }
#endif
}
void Delay(unsigned int  x)
{
	unsigned int i,j;

	for(j=0;j<5;j++)
		for(i=0;i<x;i++);
}
