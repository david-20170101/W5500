#ifndef	_DS18B20_H_
#define	_DS18B20_H_

#include <reg51.h>
#include <intrins.h>

sbit W5500_DS18B20 = P1^6;   
 
#define OSC_FREQ_12

extern void Show_Ds18B20(unsigned char * dat);

#endif
