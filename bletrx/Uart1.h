#ifndef _UART1_H_
#define _UART1_H_

#include <Arduino.h>
extern volatile byte Rx_Buffer[] ;

//
extern volatile byte Rx_Tx_Buffer_Cnt ;


//extern void Init_Uart(void);
//extern void Uart_Send_Byte(char data);
//extern void Uart_Send_String(char *data);

extern void Int2ASCII(uint16_t data, byte*pBuf);

extern void Byte2ASCII(byte data, byte*pBuf);



#endif

