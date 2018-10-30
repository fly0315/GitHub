/*
 * InterruptsCZT.cpp
 *
 *  Created on: 2018Äê4ÔÂ15ÈÕ
 *      Author: LiuBin
 */

#include "InterruptsCZT.h"
#include "UnlockFifoBuffer.h"
#include "PtThreadEx.h"
#include "ThreadDataCZT.h"

 /************************************************************************/
extern UnlockFifoBuffer FifoBuffer_Sci_A;
extern UnlockFifoBuffer FifoBuffer_Sci_B;

extern PtThreadEx ThreadQ[];
__interrupt void cpu_timer1_isr(void)
{
    EALLOW;
    CpuTimer1.InterruptCount++;
    for (int i = 0; i < SCHED_MAX; i++)
    {
        ThreadQ[i].SleeperRemarks_US();
    }
    // The CPU acknowledges the interrupt.
    EDIS;
}
__interrupt void cpu_timer2_isr(void)
{
	EALLOW;
	CpuTimer2.InterruptCount++;

	// The CPU acknowledges the interrupt.
	for (int i = 0; i < SCHED_MAX; i++)
	{
		ThreadQ[i].TimerRemarks();
		ThreadQ[i].SleeperRemarks_MS();
	}
	EDIS;
}

//__interrupt void sciaTxFifoIsr(void)
//{
//    SciaRegs.SCIFFTX.bit.TXFFINTCLR = 1;  // Clear Interrupt flag
//    PieCtrlRegs.PIEACK.all |= 0x100;      // Issue PIE ACK
//}

__interrupt void sciaRxFifoIsr(void)
{
	// get data from ram buffer
	unsigned int rdata[16] = { 0 };
	unsigned int dataInFifoLen = SciaRegs.SCIFFRX.bit.RXFFST;
	for (int i = 0; i < dataInFifoLen; i++)
	{
		rdata[i] = SciaRegs.SCIRXBUF.all & 0x00FF;  // Read data

	}
	// Push data in circle buffer
	FifoBuffer_Sci_A.Put((const unsigned int*)rdata, dataInFifoLen);
	/**************************************************/
	SciaRegs.SCIFFRX.bit.RXFFOVRCLR = 1;   // Clear Overflow flag
	SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;   // Clear Interrupt flag
	PieCtrlRegs.PIEACK.all |= 0x100;       // Issue PIE ack
}
/*
__interrupt void scibTxFifoIsr(void)
{


ScibRegs.SCIFFTX.bit.TXFFINTCLR=1;  // Clear Interrupt flag
PieCtrlRegs.PIEACK.all|=0x100;      // Issue PIE ACK
}
*/
__interrupt void scibRxFifoIsr(void)
{
	// get data from ram buffer
	unsigned int rdata[16] = { 0 };
	unsigned int dataInFifoLen = ScibRegs.SCIFFRX.bit.RXFFST;
	for (int i = 0; i < dataInFifoLen; i++)
	{
		rdata[i] = ScibRegs.SCIRXBUF.all & 0x00FF;  // Read data
	}
	// put in circle buffer
	FifoBuffer_Sci_B.Put(rdata, dataInFifoLen);
	/*************************************************/
	ScibRegs.SCIFFRX.bit.RXFFOVRCLR = 1;  // Clear Overflow flag
	ScibRegs.SCIFFRX.bit.RXFFINTCLR = 1;  // Clear Interrupt flag
	PieCtrlRegs.PIEACK.all |= 0x100;      // Issue PIE ack
}
/*
__interrupt void scicTxFifoIsr(void)
{
ScicRegs.SCIFFTX.bit.TXFFINTCLR = 1;  // Clear Interrupt flag
PieCtrlRegs.PIEACK.all |= 0x0080;      // Issue PIE ACK
}

__interrupt void scicRxFifoIsr(void)
{

ScicRegs.SCIFFRX.bit.RXFFOVRCLR=1;   // Clear Overflow flag
ScicRegs.SCIFFRX.bit.RXFFINTCLR=1;   // Clear Interrupt flag

PieCtrlRegs.PIEACK.all|=0x0080;       // Issue PIE ack
}
*/
/************************************************************************/
