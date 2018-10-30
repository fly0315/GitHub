/*
 * GpioConfig.h
 *
 *  Created on: 2018Äê1ÔÂ26ÈÕ
 *      Author: LiuBin
 */

#ifndef GPIOCONFIG_H_
#define GPIOCONFIG_H_

#include "DSP28x_Project.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define TOGGLE 0x02
#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1

#define GROUP_A 0
#define GROUP_B 1
#define GROUP_C 2

inline void SetBit(unsigned long* Dest, unsigned int index)
{
    *Dest |= ((unsigned long)1 << index);
}
inline void ClrBit(unsigned long* Dest, unsigned int index)
{
    *Dest &= ~((unsigned long)1 << index);
}
inline unsigned int GetBit(unsigned long Src, unsigned int index)
{
    return (Src >> index) & 0x01;
}

void pinMode(unsigned int iGpio_x, unsigned int iMode);
void digitalWrite(unsigned int iGpio_x, unsigned int value);
int digitalRead(unsigned int iGpio_x);
void InitGpio_Default(void);

#ifdef __cplusplus
}
#endif /* extern "C" */


#endif /* GPIOCONFIG_H_ */
