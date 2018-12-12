/*
 * GpioConfig.h
 *
 *  Created on: 2018Äê11ÔÂ05ÈÕ
 *      Author: LiuBin
 */
#ifndef SCC1300-D02_SPI_H_
#define SCC1300-D02_SPI_H_

#pragma once

#include "DSP28x_Project.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

	unsigned int SCC1300_Init
	(
// 		PIN CSB_G, PIN SCK_G, PIN MOSI_G, PIN MISO_G,
// 		PIN CSB_A, PIN SCK_A, PIN MOSI_A, PIN MISO_A,
// 		unsigned int Baud_G, unsigned int Baud_A
	);
	void GyroPowerup();
	int16_t GetGyroRateX();
	int AccStartup();
	void ReadAccerations(int16_t* XAcc,int16_t* YAcc, int16_t* ZAcc);
#ifdef __cplusplus
}
#endif /* extern "C" */


#endif /* SCC1300-D02_SPI_H */
