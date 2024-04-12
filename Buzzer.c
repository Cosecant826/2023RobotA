#include <STC12C5A60S2.H>
#include "Delay.h"

//蜂鸣器端口：
sbit Buzzer = P2 ^ 5;


/**
  * @brief  蜂鸣器发声
  * @param  ms 发声的时长，范围：0~32767
  * @retval 无
  */
void Buzzerrun(unsigned int ms, hz)
{
    unsigned int i;
    ms *= (100 / hz);

    for (i = 0; i < ms; i++)
    {
        Buzzer = !Buzzer;
        Delay10us(hz);
    }
}
