#include "MQTT.h"
#include <string.h>

char qos     = 1;
char * topic = "$dp";

int PublishMqttData(char * topic, int qos, unsigned char * dat, int size)
{  
    int topic_length   = strlen(topic);    
    int message_length = size;  
    int i,index=0;    
    static unsigned int id = 0;
    if(qos){
	    Tx_Buffer[index++] = 0x32;             
        Tx_Buffer[index++] = 2 + topic_length + 2 + 3 + message_length;  
    }else{
        Tx_Buffer[index++] = MQTT_PKT_PUBLISH<<4;                      
		Tx_Buffer[index++] = 2 + topic_length + 3 + message_length;     
	}
    Tx_Buffer[index++] = (0xff00&topic_length)>>8;                     
    Tx_Buffer[index++] = 0xff&topic_length;
         
    for(i = 0; i < topic_length; i++)
    {
        Tx_Buffer[index + i] = topic[i];                              
    }
    index += topic_length;
        
    if(qos)
    {
        Tx_Buffer[index++] = (0xff00&id)>>8;
        Tx_Buffer[index++] = 0xff&id;
        id++;
		if(id==65535){
		   id=0;
		}
    }
	Tx_Buffer[index++] = 0x03;
	Tx_Buffer[index++] = (0xff00&message_length)>>8;              
    Tx_Buffer[index++] = 0xff&message_length;

    for(i = 0; i < message_length; i++)
    {
        Tx_Buffer[index + i] = dat[i];                           
    }
    index += message_length;
        
    return index;
}
int PingMqttPkg(void)
{
  int Index = 2;
  Tx_Buffer[0] = MQTT_PKT_PINGREQ<<4;
  Tx_Buffer[1] = 0x00;
  return Index;
}
int ConnectMqttPkg(unsigned char * dat, int addr)
{
	int messageLen = dat[addr];
	int i = 0;
    int Index = 2;
    int DataLen = 10+messageLen;
    Tx_Buffer[0] = MQTT_PKT_CONNECT<<4;        
    Tx_Buffer[1] = DataLen;                           
    Tx_Buffer[Index++] = 0;                           
    Tx_Buffer[Index++] = 4;                            
    Tx_Buffer[Index++] = 'M';                      
    Tx_Buffer[Index++] = 'Q';                        
    Tx_Buffer[Index++] = 'T';                      
    Tx_Buffer[Index++] = 'T';                     
    Tx_Buffer[Index++] = 4;                       
    Tx_Buffer[Index++] = 0xc0;                    
    Tx_Buffer[Index++] =    0;                     
    Tx_Buffer[Index++] =   60;                       
	for(i = 0; i < messageLen ; i++)
    {
      Tx_Buffer[Index+i] = dat[addr+1+i];    
    }
	Index = Index + messageLen; 
	return Index;
}
int MqttSend(unsigned char * dat, int datLength, unsigned char * message, int messageLength)
{
	int Index = 0;
	if( dat[datLength-1]==0x00 && dat[datLength-2]==0x00 )
	{
		dat[datLength-1]=MQTT_PKT_CONNECT;
		Index = ConnectMqttPkg(dat, InitMqttPkgAddr);
	}
    if( dat[datLength-1]==MQTT_PKT_CONNECT && dat[datLength-2]==MQTT_PKT_CONNACK )
    {
		dat[datLength-1]=MQTT_PKT_PUBLISH;
		Index = PublishMqttData(topic, qos, message, messageLength);
	}
    if( dat[datLength-1]==MQTT_PKT_PUBLISH && dat[datLength-2]==MQTT_PKT_PUBACK )
	{
		dat[datLength-1]=MQTT_PKT_PINGREQ;
		Index = PingMqttPkg();
	}
    if( dat[datLength-1]==MQTT_PKT_PINGREQ && dat[datLength-2]==MQTT_PKT_PINGRESP )
    {
	   dat[datLength-1]=MQTT_PKT_PUBLISH;
	   Index = PublishMqttData(topic, qos, message, messageLength);
	}
    return Index;
}
void Mqtt_HandleConnAck(unsigned char * dat, int datLength, unsigned short ReceiveSize)
{
	if( Rx_Buffer[1]+2==ReceiveSize )
	{
		dat[datLength-2]=MQTT_PKT_CONNACK;
	} else {
		dat[datLength-2]=0x00;
		dat[datLength-1]=0x00;
	}
}
void Mqtt_HandlePublish(unsigned char * serverBuffer, int serverBufferLength, unsigned short ReceiveSize)
{
    int i, k;
	if( Rx_Buffer[1]+2==ReceiveSize )
	{
		if(Rx_Buffer[1]==2+Rx_Buffer[2]*16+Rx_Buffer[3]+sizeof(response))
		{
		  	for(i=0; i<sizeof(response); i++)
			{
			    k = 2+2+Rx_Buffer[2]*16+Rx_Buffer[3];
				if( response[i] != Rx_Buffer[i+k] )
				{
				    i=sizeof(response)+1;
				}
			}
			if(i==sizeof(response))
			{
			    W5500_LED = 0;   
				if( EEPROMWrite( serverBuffer, sizeof(serverBufferLength) ) )
				{
				    for(k=0; k<(int)ReceiveSize; k++)
			        {
			            W5500_LED = ~W5500_LED;
			            Delay(100000);
			        }
			        IAP_CONTR=0x60;
				}
			}
		}
	}
}
void Mqtt_HandlePubAck(unsigned char * dat, int datLength, unsigned short ReceiveSize)
{
	if( Rx_Buffer[1]+2==ReceiveSize )
	{
		dat[datLength-2]=MQTT_PKT_PUBACK;
	} else {
		dat[datLength-2]=0x00;
		dat[datLength-1]=0x00;
	}
}
void Mqtt_HandlePingResp(unsigned char * dat, int datLength, unsigned short ReceiveSize)
{
	if( Rx_Buffer[1]+2==ReceiveSize )
	{
		dat[datLength-2]=MQTT_PKT_PINGRESP;
	} else {
		dat[datLength-2]=0x00;
		dat[datLength-1]=0x00;
	}
}
void MqttReceive(unsigned char * dat, int datLength, unsigned char * serverBuffer, int serverBufferLength, unsigned short ReceiveSize)
{
	unsigned int x;
	if(ReceiveSize>0)
	{
		switch( Rx_Buffer[0]>>4 )
		{
			case MQTT_PKT_PINGRESP:
			Mqtt_HandlePingResp(dat, datLength, ReceiveSize);
			break;

			case MQTT_PKT_CONNACK:
			Mqtt_HandleConnAck(dat, datLength, ReceiveSize);
			break;
			
			case MQTT_PKT_PUBLISH:
			Mqtt_HandlePublish(serverBuffer, serverBufferLength, ReceiveSize);
			break;

			case MQTT_PKT_PUBACK:
			Mqtt_HandlePubAck(dat, datLength, ReceiveSize);
			break;

			case MQTT_PKT_PUBREC:
			break;

			case MQTT_PKT_PUBREL:
			break;

			case MQTT_PKT_PUBCOMP:
			break;

			case MQTT_PKT_SUBACK:
			break;

			case MQTT_PKT_UNSUBACK:
			break;
		
			default:
			break;
			
		}

		if(ReceiveSize>5 && Rx_Buffer[0]==0x55 && Rx_Buffer[1]==0xaa && Rx_Buffer[2]==ReceiveSize && Rx_Buffer[ReceiveSize-1]==0x55 && Rx_Buffer[ReceiveSize-2]==0xaa && Rx_Buffer[ReceiveSize-3]==0x01 )	   
	    {
		  W5500_LED = 1;    
		  memcpy(Tx_Buffer, serverBuffer, serverBufferLength);			
	      Write_SOCK_Data_Buffer(0, Tx_Buffer, serverBufferLength);
	    }
	    if(ReceiveSize>5 && Rx_Buffer[0]==0x55 && Rx_Buffer[1]==0xaa && Rx_Buffer[2]==ReceiveSize && Rx_Buffer[ReceiveSize-1]==0x55 && Rx_Buffer[ReceiveSize-2]==0xaa && Rx_Buffer[ReceiveSize-3]==0x02 )	
	    {
		  for(x=0;x<(int)ReceiveSize-3;x++)
		  {
		    Tx_Buffer[x]=Rx_Buffer[x+3];
		  }
		  if( EEPROMWrite(Tx_Buffer, (int)ReceiveSize-6) )
		  {
			W5500_LED = 0;    
		    Write_SOCK_Data_Buffer(0, Tx_Buffer, ReceiveSize-6);
			for(x=0; x<(int)ReceiveSize; x++)
			{
			  W5500_LED = ~W5500_LED;
			  Delay(100000);
			}
			IAP_CONTR=0x60;
          }
	    }
	}	
}	  
void HexToMAC(unsigned char * dat, unsigned char * message)
{
  unsigned int j;
  unsigned char value;
  for(j=0;j<6;j++)
  {
    value=dat[j+1];
	value=value>>4;
	if(value>=0 && value<=9)
	{
	  value=value+48;
	} else {
	  value=value+55;
	}
	message[2*j+8]=value;
    value=dat[j+1];
	value=value<<4;
	value=value>>4;
	if(value>=0 && value<=9)
	{
	  value=value+48;
	} else {
	  value=value+55;
	}
	message[2*j+9]=value;	
  }
}
