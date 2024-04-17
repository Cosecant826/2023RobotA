/*
	P20P21 GY30
	P22 LED
	P24 门锁
	P23 风扇
	P31  dht11
	P30 DS18B20
	key1 键入1     手动自动切换开关
	key5 LED开关
	key9 风扇开关
	key13-16  步进电机1 2 3 4圈
	key2  修改密码
	key3  翻页至时间页/翻回
	key11  退格
	key12  确认修改密码
	key13  退出修改密码
    P33  屏幕背光开关
*/


#include <STC12C5A60S2.H>
#include "lcd12864.h"
#include "define.h"
#include "Matrixkey.h"
#include "Delay.h"
#include "AT24C02.h"
#include "DS1302.h"
#include "DHT11.h"
#include "GY30.h"
#include "Timer.h"
#include "stepmotor.h"
#include "Buzzer.h"
#include "DS18B20.h"



u16 Light;//亮度
bit flag, lcdflag = 0, ledflag, motorflag, Hflag, Lflag, autoflag, page;
float Te;
/*
flag=0密码锁输入，输入正确密码后flag=1
lcdflag=1，显示时间温湿度等状态，通过定时器1检测
ledflag灯光标志位
motorflag，风扇标志位
hflag&lflag，自动调节时检测这两个标志位判断当前状态需不需要调整，环境改变后修改这两个标志位，下次检测时如果环境没有改变就不用调整
autoflag自动调节标志位，两秒改变一次，和温湿度检测同步

*/
u8 stepmotorflag, laststep, changemode;
/*
stepmotorflag  步进电机状态，1234为位置
laststep 用于读取当前步进电机位置，和stepflag比较来决定步进电机要执行的动作，数值存储在at24c02（5）
changemode  0手动调整，1自动调整
*/

void showmenu()				//显示状态框架
{
    Lcd12864_ShowString(0, 0, "温度");
    Lcd12864_ShowString(1, 0, "湿度    风扇:");
    Lcd12864_ShowString(2, 0, "光照    灯光: ");
    Lcd12864_ShowString(3, 0, "窗帘:");
    Lcd12864_ShowString(4, 0, "    年  月  日");
    Lcd12864_ShowString(5, 0,  "  :   :   ");
}

void autoset()			//无视hflag&lflag进行自动调整，用于手动调整切换回自动调整时hflag&lflag不准确
{
    if (Te <= 30)
    {
        motorflag = 0;
        stepmotorflag = 1;
        Hflag = 0;
    }
    else
    {
        motorflag = 1;
        stepmotorflag = 4;
        Hflag = 1;
    }

    if (Light <= 100)
    {
        ledflag = 1;
        stepmotorflag = 4;
        Lflag = 0;
    }					//窗帘0，led亮
    else if (Light > 100 && Light <= 300)
    {
        ledflag = 0;
        stepmotorflag = 3;
        Lflag = 1;
    }//窗帘1/4,led灭
    else if (Light > 300 && Light <= 700)
    {
        ledflag = 0;
        stepmotorflag = 2;
        Lflag = 2;
    }
    else
    {
        ledflag = 0;
        stepmotorflag = 1;
        Lflag = 3;
    }

    laststep = AT24C02_ReadByte(5);

    if (laststep != stepmotorflag)
    {
        EA = 0;											//为了使步进电机运行流畅，关闭中断
        Lcd12864_Init();								//关闭中断后显示不刷新，显示正在调整窗帘减少卡顿感
        Lcd12864_ShowString(0, 0, "正\xfd在调整窗帘");
        step(stepmotorflag);
        showmenu();										//恢复显示
        EA = 1;
    }

    P22 = !ledflag;
    P23 = motorflag;
    autoflag = 0;
}
void main()
{
    u8 i, k = 0, KeyNum, err, keymode, temp[10];
    u32 Password, PasswordNum;
    P23 = 0;
    P33 = 1;
    EA = 1;
    Timer0_Init();
    TR0 = 0;
    Lcd12864_Init();
    Lcd12864_ShowString(0, 0, "ZNJJXT");
    Lcd12864_ShowString(1, 0, "0826");
    DS1302_Init();
    DS1302_SetTime();
    DHT_Init();
    GY30_Init();

    for (i = 0; i < 5; i++)
    {
        Delay(600);
        Right(1);
    }

    i = 0;
    Delay(600);
    ShutdownDisplay();
    TR0 = 1;
    P33 = 0;

    while (!KeyNum)
    {
        KeyNum = Key();
    }

    Lcd12864_Init();
    P33 = 1;
    Lcd12864_ShowString(0, 0, "输入密码：");
    KeyNum = 0;
    PasswordNum = AT24C02_ReadByte(1) * 65536;
    PasswordNum += AT24C02_ReadByte(2) * 256;
    PasswordNum += AT24C02_ReadByte(3);

    while (1)
    {
        KeyNum = Key();

        if (flag == 0)
        {
            Delay(1);

            if (KeyNum <= 10 && KeyNum > 0)
            {
                if (i == 0)
                {
                    Lcd12864_WriteCMD(0x90);
                }

                Lcd12864_WriteData(0x2A);
                Password *= 10;
                Password += KeyNum % 10;
                i++;

                if (i == 6)
                {
                    TR0 = 0;

                    if (Password == PasswordNum)
                    {
                        Password = 0;
                        Lcd12864_ShowString(1, 0, "密码正\xfd确    ");
                        P24 = 0;
                        flag = 1;
                        lcdflag = 1;
                        Delay(700);
                        Lcd12864_Init();
                        showmenu();
                        stepmotorflag = AT24C02_ReadByte(5);
                        DS1302_ReadTime();
                        Timer1_Init();
                        P20 = 0;
                        Delay(100);
                        P20 = 1;
                        Delay(100);
                        P20 = 0;
                        Delay(100);
                        P20 = 1;
                        Delay(100);
                        P20 = 0;
                        Delay(100);
                        P20 = 1;
                    }
                    else
                    {
                        EA = 0;
                        err++;
                        Password = 0;
                        Lcd12864_ShowString(1, 0, "密码错误    ");
                        Lcd12864_ShowString(2, 0, "密码错误  次");
                        Lcd12864_ShowNum(2, 4, err, 2);
                        i = 0;

                        if ((err) % 3 == 0)
                        {
                            Buzzerrun(500, 100);
                            Delay(100);
                            Buzzerrun(500, 25);
                            Delay(100);
                            Buzzerrun(500, 50);
                            Delay(5000);
                        }
                        else
                        {
                            Delay(1000);
                        }

                        Lcd12864_ShowString(1, 0, "            ");
                        EA = 1;
                    }
                }

                TR0 = 1;
            }
            else if (KeyNum == 11)
            {
                if (i > 0)
                {
                    i--;
                    Password /= 10;
                    Lcd12864_WriteCMD(0x90 + i / 2);

                    if (i % 2)
                    {
                        Lcd12864_WriteData(0x2A);
                    }

                    Lcd12864_WriteData(0x20);
                    Lcd12864_WriteCMD(0x90 + i / 2);

                    if (i % 2)
                    {
                        Lcd12864_WriteData(0x2A);
                    }
                }
            }

            KeyNum = 0;
        }
        else if (flag)
        {
            if (keymode == 0)
            {
                if (KeyNum == 1)
                {
                    if (changemode == 0)
                    {
                        autoset();
                    }

                    changemode = !changemode;
                }
                else if (KeyNum == 2)
                {
                    TR1 = 0;
                    Lcd12864_Init();
                    Lcd12864_ShowString(0, 0, "修改密码");
                    keymode = 1;
                    lcdflag = 0;
                }
                else if (KeyNum == 3)
                {
                    page = !page;

                    if (page)
                    {
                        Roll(32);
                    }
                    else
                    {
                        Roll(0);
                    }
                }
                else
                {
                    DS1302_ReadTime();
                }

                if (changemode == 0)
                {
                    switch (KeyNum)
                    {
                    case 5:
                        ledflag = !ledflag;
                        P22 = !P22;
                        break;

                    case 9:
                        motorflag = !motorflag;
                        P23 = !P23;
                        break;

                    case 13:
                        stepmotorflag = 1;
                        EA = 0;
                        Lcd12864_Init();
                        Lcd12864_ShowString(0, 0, "正\xfd在调整窗帘");
                        step(1);
                        P22 = !ledflag;
                        showmenu();
                        EA = 1;
                        break;

                    case 14:
                        stepmotorflag = 2;
                        EA = 0;
                        Lcd12864_Init();
                        Lcd12864_ShowString(0, 0, "正\xfd在调整窗帘");
                        step(2);
                        P22 = !ledflag;
                        showmenu();
                        EA = 1;
                        break;

                    case 15:
                        stepmotorflag = 3;
                        EA = 0;
                        Lcd12864_Init();
                        Lcd12864_ShowString(0, 0, "正\xfd在调整窗帘");
                        step(3);
                        P22 = !ledflag;
                        showmenu();
                        EA = 1;
                        break;

                    case 16:
                        stepmotorflag = 4;
                        EA = 0;
                        Lcd12864_Init();
                        Lcd12864_ShowString(0, 0, "正\xfd在调整窗帘");
                        step(4);
                        P22 = !ledflag;
                        showmenu();
                        EA = 1;
                        break;

                    default:
                        break;
                    }
                }
                else
                {
                    if (autoflag)
                    {
                        if (Te <= 30)
                        {
                            if (Hflag)
                            {
                                motorflag = 0;
                                stepmotorflag = 1;
                                Hflag = 0;
                            }
                        }
                        else
                        {
                            if (Hflag == 0)
                            {
                                motorflag = 1;
                                stepmotorflag = 4;
                                Hflag = 1;
                            }
                        }

                        if (Light <= 100)
                        {
                            if (Lflag)
                            {
                                ledflag = 1;
                                stepmotorflag = 4;
                                Lflag = 0;
                            }
                        }					//窗帘0，led亮
                        else if (Light > 100 && Light <= 300)
                        {
                            if (Lflag != 1)
                            {
                                ledflag = 0;
                                stepmotorflag = 3;
                                Lflag = 1;
                            }
                        }//窗帘1/4,led灭
                        else if (Light > 300 && Light <= 700)
                        {
                            if (Lflag != 2)
                            {
                                ledflag = 0;
                                stepmotorflag = 2;
                                Lflag = 2;
                            }
                        }
                        else
                        {
                            if (Lflag != 3)
                            {
                                ledflag = 0;
                                stepmotorflag = 1;
                                Lflag = 3;
                            }
                        }

                        laststep = AT24C02_ReadByte(5);

                        if (laststep != stepmotorflag)
                        {
                            EA = 0;
                            Lcd12864_Init();
                            Lcd12864_ShowString(0, 0, "正\xfd在调整窗帘");
                            step(stepmotorflag);
                            showmenu();
                            EA = 1;
                        }

                        P22 = !ledflag;
                        P23 = motorflag;
                        autoflag = 0;
                    }
                }
            }
            else
            {
                if (KeyNum <= 10 && KeyNum > 0 && k < 6)
                {
                    Password *= 10;
                    Password += KeyNum % 10;
                    temp[k] = KeyNum % 10 + 0x30;
                    Lcd12864_ShowString(1, 0,  "        ");
                    Lcd12864_ShowString(1, 0, temp);
                    k++;
                }

                if (KeyNum == 11)
                {
                    if (k > 0)
                    {
                        k--;
                        Password /= 10;
                        temp[k] = 0;
                        Lcd12864_ShowString(1, 0,  "        ");
                        Lcd12864_ShowString(1, 0, temp);
                    }
                }

                if (KeyNum == 12)
                {
                    AT24C02_WriteByte(1, Password / 65536 % 256);
                    Delay(5);
                    AT24C02_WriteByte(2, Password / 256 % 256);
                    Delay(5);
                    AT24C02_WriteByte(3, Password % 256);
                    Delay(5);
                    Lcd12864_ShowString(1, 0,  "        ");
                    Lcd12864_ShowString(1, 0,  "修改成功");
                    keymode = 0;
                    Delay(1000);
                    showmenu();
                    TR1 = 1;
                    lcdflag = 1;
                }

                if (KeyNum == 13)
                {
                    Password = 0;

                    for (; k > 0; k--)
                    {
                        temp[k] = 0;
                    }

                    showmenu();
                    keymode = 0;
                    TR1 = 1;
                    lcdflag = 1;
                }
            }
        }
    }
}


void Timer0_Isr(void) interrupt 1
{
    static unsigned int T0Count, T4Count;
    TL0 = 0x66;				//设置定时初始值
    TH0 = 0xFC;				//设置定时初始值
    T0Count++;

    if (T0Count % 20 == 0)
    {
        Key_Loop();    //20ms调用一次按键驱动函数
    }

    T4Count %= 500;

    if (T4Count == 0)
    {
        DHT();
        Light = Get_GY();
        DS18B20_ConvertT();
        Te = DS18B20_ReadT();
        autoflag = 1;
    }

    T4Count++;
}



void Timer1_Isr(void) interrupt 3
{
    static unsigned int T1Count, T2Count, T3Count;
    TL1 = 0x66;				//设置定时初始值
    TH1 = 0xFC;				//设置定时初始值
    T1Count++;
    T2Count++;

    if (T3Count < 1600)
    {
        T3Count++;
    }

    if (T3Count == 1500)
    {
        P24 = 1;
    }

    if (T2Count == 20)
    {
        T2Count = 0;

        if (lcdflag == 1)
        {
            Lcd12864_ShowNum(4, 0, DS1302_Time[0] + 2000, 4);
            Lcd12864_ShowNum(4, 3, DS1302_Time[1], 2);
            Lcd12864_ShowNum(4, 5, DS1302_Time[2], 2);
            Lcd12864_ShowNum(5, 0, DS1302_Time[3], 2);
            Lcd12864_ShowNum(5, 2, DS1302_Time[4], 2);
            Lcd12864_ShowNum(5, 4, DS1302_Time[5], 2);
            Lcd12864_ShowNum(0, 2, Te, 2);
			Lcd12864_ShowFloatNum(0, 3, (u32)(Te*10000)%10000, 4);
            Lcd12864_ShowNum(1, 2, DHTWetH, 2);
            Lcd12864_ShowNum(2, 2, Light, 4);

            if (ledflag)
            {
                Lcd12864_ShowString(2, 7, "开");//灯
            }
            else
            {
                Lcd12864_ShowString(2, 7, "关");//灯
            }

            if (motorflag)
            {
                Lcd12864_ShowString(1, 7, "开");//开关风扇
            }
            else
            {
                Lcd12864_ShowString(1, 7, "关");    //开关风扇
            }

            if (stepmotorflag == 4)
            {
                Lcd12864_ShowString(3, 2, ":1  ");    //窗帘0
            }
            else if (stepmotorflag == 1)
            {
                Lcd12864_ShowString(3, 2, ":1/4");    //窗帘1/4
            }
            else if (stepmotorflag == 2)
            {
                Lcd12864_ShowString(3, 2, ":1/2");    //窗帘1/2
            }
            else if (stepmotorflag == 3)
            {
                Lcd12864_ShowString(3, 2, ":3/4");    //窗帘3/4
            }

            if (changemode)
            {
                Lcd12864_ShowString(3, 4, "自动调节");
            }
            else
            {
                Lcd12864_ShowString(3, 4, "手动调节");    //手动调节
            }
        }
    }
}
