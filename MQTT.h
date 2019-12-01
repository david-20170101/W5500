#ifndef	_MQTT_H_
#define	_MQTT_H_

#include <reg51.h>
#include "EEPROM.h"
#include "W5500.h"

sbit W5500_LED = P1^5;

#define InitMqttPkgAddr 27
#define BUFFER_MAX_SIZE 90

extern unsigned char Rx_Buffer[BUFFER_MAX_SIZE];	 
extern unsigned char Tx_Buffer[BUFFER_MAX_SIZE];	 
extern unsigned char xdata response[30];

#define MQTT_PKT_CONNECT      0x01   
#define MQTT_PKT_CONNACK      0x02   
#define MQTT_PKT_PUBLISH      0x03    
#define MQTT_PKT_PUBACK       0x04   
#define MQTT_PKT_PUBREC       0x05    
#define MQTT_PKT_PUBREL       0x06   
#define MQTT_PKT_PUBCOMP      0x07   
#define MQTT_PKT_SUBSCRIBE    0x08   
#define MQTT_PKT_SUBACK       0x09   
#define MQTT_PKT_UNSUBSCRIBE  0x0a   
#define MQTT_PKT_UNSUBACK     0x0b    
#define MQTT_PKT_PINGREQ      0x0c   
#define MQTT_PKT_PINGRESP     0x0d   
#define MQTT_PKT_DISCONNECT   0x0e   

extern void HexToMAC(unsigned char * dat, unsigned char * message);
extern int  MqttSend(unsigned char * dat, int datLength, unsigned char * message, int messageLength);
extern void MqttReceive(unsigned char * dat, int datLength, unsigned char * serverBuffer, int serverBufferLength, unsigned short ReceiveSize);

#endif
