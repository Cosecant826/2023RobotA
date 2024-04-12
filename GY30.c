#include <STC12C5A60S2.H>
#include "I2C.h"
#include "define.h"
#include "Delay.h"

u8 BUF[5];

#define GY30_Address 0x46

void GY30_WriteByte(u8 Data)
{
    I2C_Start();
    I2C_SendByte(GY30_Address);
    I2C_ReceiveAck();
    I2C_SendByte(Data);
    I2C_ReceiveAck();
    I2C_Stop();
}

void GY30_ReadByte()
{
    I2C_Start();
    I2C_SendByte(GY30_Address + 1);
    I2C_ReceiveAck();
    BUF[0] = I2C_ReceiveByte();
    I2C_SendAck(0);
    BUF[1] = I2C_ReceiveByte();
    I2C_SendAck(0);
    BUF[2] = I2C_ReceiveByte();
    I2C_SendAck(0);
    I2C_Stop();
}

void GY30_Init()
{
    GY30_WriteByte(0x01);
    GY30_WriteByte(0x11);
    Delay(100);
}

u16 Get_GY()
{
    u16 Data;
    GY30_ReadByte();
    Data = BUF[0];
    Data = (Data << 8) + BUF[1];
    Data = (float)Data / 1.2;
    return Data;
}