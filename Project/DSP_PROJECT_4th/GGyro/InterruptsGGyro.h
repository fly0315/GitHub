/*
 * InterruptsGGyro.h
 *
 *  Created on: 2018Äê4ÔÂ2ÈÕ
 *      Author: LiuBin
 */

#ifndef GGYRO_INTERRUPTSGGYRO_H_
#define GGYRO_INTERRUPTSGGYRO_H_

#pragma once

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

// __interrupt void cpu_timer0_isr(void);

__interrupt void cpu_timer1_isr(void);

__interrupt void cpu_timer2_isr(void);

 //__interrupt void sciaTxFifoIsr(void);

 //__interrupt void scibTxFifoIsr(void);

 //__interrupt void scicTxFifoIsr(void);

__interrupt void sciaRxFifoIsr(void);

__interrupt void scibRxFifoIsr(void);

__interrupt void scicRxFifoIsr(void);

__interrupt void ECAN1A_ISR(void);

__interrupt void ECAN1B_ISR(void);

// inline void startCpuTimer0()
// {
// 	CpuTimer0Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0
// }


#endif /* GGYRO_INTERRUPTSGGYRO_H_ */
