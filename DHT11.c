#include <STC12C5A60S2.H>
#include "Delay.h"
#include "lcd12864.h"
sbit  DHT_IO  = P3 ^ 1;

unsigned char DHTCount, DHTTemp, DHTComdata, DHTHotH, DHTHotL, DHTWetH, DHTWetL, DHTCheck;
unsigned char DHTHotHtemp, DHTHotLtemp, DHTWetHtemp, DHTWetLtemp, DHTChecktemp;

void DHTADC()
{
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        while (!DHT_IO);

        Delay10u();
        Delay10u();
        Delay10u();

        if (DHT_IO)
        {
            DHTTemp = 1;
        }
        else
        {
            DHTTemp = 0;
        }

        while (DHT_IO);

        DHTComdata <<= 1;
        DHTComdata |= DHTTemp;
    }
}

void DHT(void)
{
    DHT_IO = 1;
    Delay10u();
    DHT_IO = 0;
    Delay25ms();
    DHT_IO = 1;
    Delay10u();
    Delay10u();
    Delay10u();

    while (!DHT_IO);

    while (DHT_IO);

    DHTADC();
    DHTWetHtemp = DHTComdata;
    DHTADC();
    DHTWetLtemp = DHTComdata;
    DHTADC();
    DHTHotHtemp = DHTComdata;
    DHTADC();
    DHTHotLtemp = DHTComdata;
    DHTADC();
    DHTChecktemp = DHTComdata;
    DHT_IO = 1;
    DHTTemp = (DHTHotHtemp + DHTHotLtemp + DHTWetHtemp + DHTWetLtemp);

    if (DHTTemp == DHTChecktemp)
    {
        DHTWetH = DHTWetHtemp;
        DHTWetL = DHTWetLtemp;
        DHTHotH = DHTHotHtemp;
        DHTHotL = DHTHotLtemp;
        DHTCheck = DHTChecktemp;
    }
}

void DHT_Init()
{
    DHT_IO = 1;
    Delay10u();
    DHT_IO = 0;
    Delay25ms();
    DHT_IO = 1;
    Delay10u();
    Delay10u();
    Delay10u();
}
