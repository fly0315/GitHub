/*
 * CanPortConfig.h
 *
 *  Created on: 2018Äê1ÔÂ26ÈÕ
 *      Author: LiuBin
 */

#ifndef SERIALPORTCONFIG_H_
#define SERIALPORTCONFIG_H_

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DSP28x_Project.h"
#include "BSP_cfg.h"
	/*
	static const float CPU_FREQ = 150*1E6;
	static const float LSPCLK_FREQ = CPU_FREQ * 0.25;
	static const float SCI_FREQ = 921600;
	static const unsigned int SCI_PRD = (unsigned int)((LSPCLK_FREQ / (SCI_FREQ * 8)) - 1);
	*/

	typedef enum
	{
		sci_a = 0,
		sci_b,
		sci_c
	} SCI_X;

//#define CPU_FREQ     150E6
//#define LSPCLK_FREQ  ((CPU_FREQ)*0.25)

	void InitGpio_SCI_A(unsigned int TxPin, unsigned int RxPin);
	void InitGpio_SCI_B(unsigned int TxPin, unsigned int RxPin);
	void InitGpio_SCI_C(unsigned int TxPin, unsigned int RxPin);

	void sci_fifo_init(SCI_X sci_x, unsigned long BR, char Parity, unsigned int TXFFIL , unsigned int RXFFIL);

	int tolower(int ch);
	int toupper(int ch);
	
	unsigned int getSciPrd(unsigned long BR);
	unsigned int getParity(unsigned char parity);
#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* SERIALPORTCONFIG_H_ */
