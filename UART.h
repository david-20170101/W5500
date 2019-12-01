#ifndef	_STC12C5A_UART_H_
#define	_STC12C5A_UART_H_

sfr  AUXR          = 0x8E;
sfr  BRT           = 0x9C;

extern void UART_Init(void);
extern void Delay(unsigned int x);

#endif
