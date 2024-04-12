#include <STC12C5A60S2.H>
#include "define.h"

sbit I2C_SCL = P2 ^ 1;
sbit I2C_SDA = P2 ^ 0;

void I2C_Start()
{
    I2C_SDA = 1;
    I2C_SCL = 1;
    I2C_SDA = 0;
    I2C_SCL = 0;
}

void I2C_Stop()
{
    I2C_SDA = 0;
    I2C_SCL = 1;
    I2C_SDA = 1;
}

void I2C_SendByte(u8 Byte)
{
    u8 i;

    for (i = 0; i < 8; i++)
    {
        I2C_SDA = Byte & (0x80 >> i);
        I2C_SCL = 1;
        I2C_SCL = 0;
    }
}

u8 I2C_ReceiveByte()
{
    u8 i, Byte;

    for (i = 0; i < 8; i++)
    {
        I2C_SCL = 1;

        if (I2C_SDA)
        {
            Byte |= (0X80 >> i);
        }

        I2C_SCL = 0;
    }

    return Byte;
}
/*
  * @brief  I2C发送应答
  * @param  Bit 0为应答，1为非应答
  * @retval
*/
void I2C_SendAck(u8 Bit)
{
    I2C_SDA = Bit;
    I2C_SCL = 1;
    I2C_SCL = 0;
}
/*
  * @brief  I2C接收应答
  * @param
  * @retval Bit 为1表示不应答，为0表示应答
*/
u8 I2C_ReceiveAck()
{
    u8 Bit;
    I2C_SDA = 1;
    I2C_SCL = 1;
    Bit = I2C_SDA;
    I2C_SCL = 0;
    return Bit;
}

