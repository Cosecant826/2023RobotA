#include "define.h"

#ifndef _STEPMOTOR_H_
#define _STEPMOTOR_H_

void step(u8 step);
void stepangle(u16 step, bit dir);
void steprun(char step, bit dir);
#endif