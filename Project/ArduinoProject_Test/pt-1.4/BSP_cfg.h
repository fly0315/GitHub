/*
 * BSP_cfg.h
 *
 *  Created on: 2018年4月15日
 *      Author: LiuBin
 */
#include <Arduino.h>
#ifndef SOURCE_BSP_CFG_H_
#define SOURCE_BSP_CFG_H_

#define CPU_FREQ     150E6
#define LSPCLK_FREQ  ((CPU_FREQ)*0.25)
// timer
#define PERIOD_TIMER2_US 1000UL
#define PERIOD_TIMER2_MS ((unsigned int)(PERIOD_TIMER2_US * 0.001))
#define PERIOD_TIMER1_US 1000UL
#define PERIOD_TIMER1_MS ((unsigned int)(PERIOD_TIMER1_US * 0.001))
// Set AD
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))	
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))	
#endif

/***********************
** 获取log2(x)算法
***********************/
inline int fastLog2(int x)
{
	float fx;
	unsigned long ix, exp;
	fx = (float)x;
	ix = *(unsigned long*)& fx;
	exp = (ix >> 23) & 0xFF;
	return exp - 127;
}

/*************************
** 设定AD采样周期预设值，默认为128
************************/
inline void setPrescale(unsigned int value)
{
	int pw_2 = fastLog2(value);
	if ((pw_2 >> 2) & 0x01)
	{
		sbi(ADCSRA, ADPS2);
	}
	else
	{
		cbi(ADCSRA, ADPS2);
	}
	if ((pw_2 >> 1) & 0x01)
	{
		sbi(ADCSRA, ADPS1);
	}
	else
	{
		cbi(ADCSRA, ADPS1);
	}
	if ((pw_2 >> 0) & 0x01)
	{
		sbi(ADCSRA, ADPS0);
	}
	else
	{
		cbi(ADCSRA, ADPS0);
	}
}


inline void ToggleIO(int pin)
{
	static int i = 0;
	i ^= 1;
	digitalWrite(pin, i);
}

#endif /* SOURCE_BSP_CFG_H_ */

