#include <STC12C5A60S2.H>
#include <INTRINS.H>


//引脚定义
sbit OneWire_DQ = P3 ^ 0;

void Delay500us(void)	//@11.0592MHz
{
    unsigned char data i, j;
    i = 6;
    j = 93;

    do
    {
        while (--j);
    } while (--i);
}

void Delay70us(void)	//@11.0592MHz
{
    unsigned char data i;
    _nop_();
    _nop_();
    i = 190;

    while (--i);
}

void Delay10us(void)	//@11.0592MHz
{
    unsigned char data i;
    _nop_();
    _nop_();
    _nop_();
    i = 24;

    while (--i);
}

void Delay50us(void)	//@11.0592MHz
{
    unsigned char data i;
    _nop_();
    i = 135;

    while (--i);
}

void Delay5us(void)	//@11.0592MHz
{
    unsigned char data i;
    _nop_();
    _nop_();
    _nop_();
    i = 10;

    while (--i);
}

/**
  * @brief  单总线初始化
  * @param  无
  * @retval 从机响应位，0为响应，1为未响应
  */
unsigned char OneWire_Init(void)
{
    unsigned char AckBit;
    OneWire_DQ = 1;
    OneWire_DQ = 0;
    Delay500us();		//Delay 500us
    OneWire_DQ = 1;
    Delay70us();			//Delay 70us
    AckBit = OneWire_DQ;
    Delay500us();		//Delay 500us
    return AckBit;
}

/**
  * @brief  单总线发送一位
  * @param  Bit 要发送的位
  * @retval 无
  */
void OneWire_SendBit(unsigned char Bit)
{
    OneWire_DQ = 0;
    Delay10us();			//Delay 10us
    OneWire_DQ = Bit;
    Delay50us();			//Delay 50us
    OneWire_DQ = 1;
}

/**
  * @brief  单总线接收一位
  * @param  无
  * @retval 读取的位
  */
unsigned char OneWire_ReceiveBit(void)
{
    unsigned char Bit;
    OneWire_DQ = 0;
    Delay5us();			//Delay 5us
    OneWire_DQ = 1;
    Delay5us();			//Delay 5us
    Bit = OneWire_DQ;
    Delay50us();			//Delay 50us
    return Bit;
}

/**
  * @brief  单总线发送一个字节
  * @param  Byte 要发送的字节
  * @retval 无
  */
void OneWire_SendByte(unsigned char Byte)
{
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        OneWire_SendBit(Byte & (0x01 << i));
    }
}

/**
  * @brief  单总线接收一个字节
  * @param  无
  * @retval 接收的一个字节
  */
unsigned char OneWire_ReceiveByte(void)
{
    unsigned char i;
    unsigned char Byte = 0x00;

    for (i = 0; i < 8; i++)
    {
        if (OneWire_ReceiveBit())
        {
            Byte |= (0x01 << i);
        }
    }

    return Byte;
}
