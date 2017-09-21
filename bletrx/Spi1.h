#ifndef _SPI_H_
#define _SPI_H_

#include <Arduino.h>
extern void SPI_Write_Byte(byte data1) ;
extern byte SPI_Read_Byte(void) ;

extern void SPI_Write_Reg(byte reg, byte data);

extern byte SPI_Read_Reg(byte reg) ;

extern void SPI_Write_Buffer(byte reg, byte *dataBuf, byte len);

extern void SPI_Read_Buffer(byte reg, byte *dataBuf, byte len);

#endif

