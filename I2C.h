#ifndef _I2C_H_
#define _I2C_H_

#include "define.h"

void I2C_Start();
void I2C_Stop();
void I2C_SendByte(u8 Byte);
u8 I2C_ReceiveByte();
void I2C_SendAck(u8 Bit);
u8 I2C_ReceiveAck();

#endif