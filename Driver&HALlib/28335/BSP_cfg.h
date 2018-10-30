/*
 * BSP_cfg.h
 *
 *  Created on: 2018Äê4ÔÂ15ÈÕ
 *      Author: LiuBin
 */

#ifndef SOURCE_BSP_CFG_H_
#define SOURCE_BSP_CFG_H_

#include "DSP28x_Project.h"

#define CPU_FREQ     150E6
#define LSPCLK_FREQ  ((CPU_FREQ)*0.25)
// timer
#define PERIOD_TIMER2_US 1000UL
#define PERIOD_TIMER2_MS ((unsigned int)(PERIOD_TIMER2_US * 0.001))
#define PERIOD_TIMER1_US 100UL

#endif /* SOURCE_BSP_CFG_H_ */
