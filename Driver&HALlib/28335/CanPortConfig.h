/*
 * CanPortConfig.h
 *
 *  Created on: 2018Äê1ÔÂ26ÈÕ
 *      Author: LiuBin
 */

#ifndef CANPORTCONFIG_H_
#define CANPORTCONFIG_H_

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DSP28x_Project.h"

typedef enum
{
	eCan_a = 0,
	eCan_b,
} ECAN_X;
void InitECanGpio_Can_A_B(void);
	//
	// change CAN-bus Baud Rate if necessary.
	// By default BR=1MHZ
	//
//	void InitCanBaudRate_A(void);
//	void InitCanBaudRate_B(void);

	void InitCan_Customed(ECAN_X eCan_x);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* CANPORTCONFIG_H_ */
