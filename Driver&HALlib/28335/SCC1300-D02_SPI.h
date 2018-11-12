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
	typedef union
	{
		uint16_t all;
		struct
		{
			unsigned int ParOdd : 1;
			unsigned int fixed0 : 1;
			unsigned int RW : 1;
			unsigned int addr : 13;
		}bits;
	}GYRO_MOSI_OPC;
	typedef union
	{
		uint16_t all;
		struct
		{
			unsigned int Par_Odd : 1;	// 0
			unsigned int S_Ok : 1;	// 1
			unsigned int D14 : 14;	// 2~15
		}bits;
	}GYRO_MISO_DATA;
	typedef struct  
	{
		GYRO_MOSI_OPC opc;
		GYRO_MOSI_OPC data;
	}GYRO_MOSI;
	typedef struct  
	{
		GYRO_MISO_DATA status;
		GYRO_MISO_DATA data;
	}GYRO_MISO;
	unsigned int SCC1300_Init
	(
// 		PIN CSB_G, PIN SCK_G, PIN MOSI_G, PIN MISO_G,
// 		PIN CSB_A, PIN SCK_A, PIN MOSI_A, PIN MISO_A,
// 		unsigned int Baud_G, unsigned int Baud_A
	);
	void GyroPowerup();
	// Enable/Disable CSB_G
	static inline void CSBEnable_G(unsigned int IsEnable) { GpioDataRegs.GPBDAT.bit.GPIO57 = IsEnable; }
	inline void Select_Gyro() { CSBEnable_G(ENABLE); }
	inline void Deselect_Gyro() { CSBEnable_G(DISABLE); }
	int16_t GetGyroRateX();
	static inline void CSBEnable_A(unsigned int IsEnable) { GpioDataRegs.GPBDAT.bit.GPIO61 = IsEnable; }

#ifdef __cplusplus
}
#endif /* extern "C" */


#endif /* GPIOCONFIG_H_ */
