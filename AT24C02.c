#include <STC12C5A60S2.H>
#include "I2C.h"
#include "define.h"
#include "Delay.h"

#define AT24C02_Address 0xA0

void AT24C02_WriteByte(u8 WordAddress, Data)
{
    I2C_Start();
    I2C_SendByte(AT24C02_Address);
    I2C_ReceiveAck();
    I2C_SendByte(WordAddress);
    I2C_ReceiveAck();
    I2C_SendByte(Data);
    I2C_ReceiveAck();
    I2C_Stop();
}

u8 AT24C02_ReadByte(u8 WordAddress)
{
    u8 Data;
    I2C_Start();
    I2C_SendByte(AT24C02_Address);
    I2C_ReceiveAck();
    I2C_SendByte(WordAddress);
    I2C_ReceiveAck();
    I2C_Start();
    I2C_SendByte(AT24C02_Address + 1);
    I2C_ReceiveAck();
    Data = I2C_ReceiveByte();
    I2C_SendAck(1);
    I2C_Stop();
    return Data;
}

