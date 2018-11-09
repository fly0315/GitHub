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
#define PIN (volatile unsigned int*) 
#define ENABLE 0
#define DISABLE 1


#ifdef __cplusplus
extern "C" {
#endif

	unsigned int SCC1300_Init
	(
		PIN CSB_G, PIN SCK_G, PIN MOSI_G, PIN MISO_G,
		PIN CSB_A, PIN SCK_A, PIN MOSI_A, PIN MISO_A,
		unsigned int Baud_G, unsigned int Baud_A
	);
	void InitGpio_Spi_A();
	void spi_init();
	void spi_fifo_init();
	void GyroPowerup();
	// Enable/Disable CSB_G
	static inline void CSBEnable_G(unsigned int IsEnable) { GpioDataRegs.GPBDAT.bit.GPIO57 = IsEnable; }
	inline void Enable_Gyro() { CSBEnable_G(ENABLE); }
	inline void Disable_Gyro() { CSBEnable_G(DISABLE); }
	unsigned int GYRO_ReadRegister(uint16_t Address, uint16_t *Data);

	static inline void CSBEnable_A(unsigned int IsEnable) { GpioDataRegs.GPBDAT.bit.GPIO61 = IsEnable; }

#ifdef __cplusplus
}
#endif /* extern "C" */


#endif /* GPIOCONFIG_H_ */
