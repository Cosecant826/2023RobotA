#include <STC12C5A60S2.H>
#include "lcd12864.h"
#include "define.h"
#include "Delay.h"
#include "AT24C02.h"
u8 stepmotor[] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09};

u16 i;

void steprun(char step, bit dir)
{
    if (dir == 0)
    {
        for (i = 0; i < step * 4096; i++)
        {
            P1 = stepmotor[i % 8];
            Delay10us(32);
        }
    }

    if (dir == 1)
    {
        for (i = step * 4096; i > 0; i--)
        {
            P1 = stepmotor[i % 8];
            Delay10us(32);
        }
    }

    P1 = 0xFF;
}

void stepangle(u16 step, bit dir)
{
    if (dir == 0)
    {
        for (i = 0; i < step * 8 * 512 / 360; i++)
        {
            P1 = stepmotor[i % 8];
            Delay10us(32);
        }
    }

    if (dir == 1)
    {
        for (i = step * 512 * 8 / 360; i > 0; i--)
        {
            P1 = stepmotor[i % 8];
            Delay10us(32);
        }
    }

    P1 = 0xFF;
}

void step(u8 step)
{
    u8 laststep, dir;
    char runstep;
    laststep = AT24C02_ReadByte(5);
    runstep = step - laststep;

    if (runstep >= 0)
    {
        dir = 0;
    }
    else
    {
        runstep = 0 - runstep;
        dir = 1;
    }

    steprun(runstep, dir);
    AT24C02_WriteByte(5, step);
    Delay(5);
}