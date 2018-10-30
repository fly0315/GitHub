/*
 * InterruptsCZT.h
 *
 *  Created on: 2018年4月15日
 *      Author: LiuBin
 */

#ifndef CZT_INTERRUPTSCZT_H_
#define CZT_INTERRUPTSCZT_H_

#pragma once
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

__interrupt void cpu_timer1_isr(void);
__interrupt void cpu_timer2_isr(void);  // 用于完成定时调度标志置位
//__interrupt void sciaTxFifoIsr(void);
__interrupt void sciaRxFifoIsr(void);   // 用于完成SCI数据接收并放入环形缓冲区
//__interrupt void scibTxFifoIsr(void);
__interrupt void scibRxFifoIsr(void);   // 用于完成SCI数据接收并放入环形缓冲区
//__interrupt void scicTxFifoIsr(void);
//__interrupt void scicRxFifoIsr(void);


#endif /* CZT_INTERRUPTSCZT_H_ */
