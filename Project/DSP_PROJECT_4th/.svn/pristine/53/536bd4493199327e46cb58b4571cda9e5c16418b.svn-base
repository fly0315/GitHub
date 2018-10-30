/*
 * InterruptsGGyro.cpp
 *
 *  Created on: 2018年4月2日
 *      Author: LiuBin
 */
#include "InterruptsGGyro.h"
#include "UnlockFifoBuffer.h"
#include "PtThreadEx.h"
#include "ThreadDataGGyro.h"

extern UnlockFifoBuffer FifoBuffer_Sci_A;
extern UnlockFifoBuffer FifoBuffer_Sci_B;
extern UnlockFifoBuffer FifoBuffer_Sci_C;

extern PtThreadEx ThreadQ[];

// __interrupt void cpu_timer0_isr(void)
// {
// 
// 	CpuTimer0.InterruptCount++;
// 	
// 	// Acknowledge this interrupt to receive more interrupts from group 1
// 	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
// }
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
	// 	ThreadQ[TKIN_SCIB].TimerRemarks();
	EDIS;
}

__interrupt void sciaRxFifoIsr(void)
{
	// get data from ram buffer
	unsigned int rdata[16] = { 0 };
	unsigned int dataInFifoLen = SciaRegs.SCIFFRX.bit.RXFFST;
	for (int i = 0; i < dataInFifoLen; i++)
	{
		rdata[i] = SciaRegs.SCIRXBUF.all;  // Read data
	}
	// Push data in circle buffer
	FifoBuffer_Sci_A.Put((const unsigned int*)rdata, dataInFifoLen);
	//////////////////////////////////////////////////////////////////////////
    SciaRegs.SCIFFRX.bit.RXFFOVRCLR=1;   // Clear Overflow flag
    SciaRegs.SCIFFRX.bit.RXFFINTCLR=1;   // Clear Interrupt flag
    PieCtrlRegs.PIEACK.all|=0x100;       // Issue PIE ack
}

__interrupt void scibRxFifoIsr(void)
{
	// do something here.
	// get data from ram buffer
	unsigned int rdata[16] = { 0 };
	unsigned int dataInFifoLen = ScibRegs.SCIFFRX.bit.RXFFST;
	for (int i = 0; i < dataInFifoLen; i++)
	{
		rdata[i] = ScibRegs.SCIRXBUF.all;  // Read data
	}
	// put in circle buffer
	FifoBuffer_Sci_B.Put(rdata, dataInFifoLen);
	//////////////////////////////////////////////////////////////////////////
	ScibRegs.SCIFFRX.bit.RXFFOVRCLR = 1;  // Clear Overflow flag
	ScibRegs.SCIFFRX.bit.RXFFINTCLR = 1;  // Clear Interrupt flag
	PieCtrlRegs.PIEACK.all |= 0x100;      // Issue PIE ack
}

__interrupt void scicRxFifoIsr(void)
{
	// do something here.
	// get data from ram buffer
	unsigned int rdata[16] = { 0 };
	unsigned int dataInFifoLen = ScicRegs.SCIFFRX.bit.RXFFST;
	for (int i = 0; i < dataInFifoLen; i++)
	{
		rdata[i] = ScicRegs.SCIRXBUF.all;  // Read data
	}
	// put in circle buffer
	FifoBuffer_Sci_C.Put(rdata, dataInFifoLen);
	//////////////////////////////////////////////////////////////////////////
	ScicRegs.SCIFFRX.bit.RXFFOVRCLR = 1;   // Clear Overflow flag
	ScicRegs.SCIFFRX.bit.RXFFINTCLR = 1;   // Clear Interrupt flag
	PieCtrlRegs.PIEACK.all |= 0x0080;       // Issue PIE ack
}

// INT9.5
__interrupt void ECAN1A_ISR(void)  // eCAN-A
{
// 	if (ECanaMboxes.MBOX31.MSGID.bit.STDMSGID == 0x1a8)
// 	{
// 		CheGyroData_.GetDataFromFrame(&ECanaMboxes.MBOX31);
// 	}
	ThreadQ[TKIN_CANA].SetState(READY);

	// 开中断
//      struct ECAN_REGS ECanaShadow;
//      ECanaShadow.CANRMP.all = ECanaRegs.CANRMP.all;
//      ECanaShadow.CANRMP.bit.RMP0 = 1;
//      ECanaRegs.CANRMP.all = ECanaShadow.CANRMP.all;

	ECanaRegs.CANRMP.all = 0xFFFFFFFF;

//      ECanaShadow.CANGIF1.all = ECanaRegs.CANGIF1.all;
//      ECanaShadow.CANGIF1.bit.GMIF1 = 1;
//      ECanaRegs.CANGIF1.all = ECanaShadow.CANGIF1.all;

	ECanaRegs.CANGIF1.all = 0xFFFFFFFF;

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
	EINT;

}
// INT9.7
__interrupt void ECAN1B_ISR(void)  // eCAN-B
{

//	ThreadQ[TKIN_CANB].SetState(READY);

	// 开中断
	//  struct ECAN_REGS ECanbShadow;
	//  ECanbShadow.CANRMP.all = ECanbRegs.CANRMP.all;
	//  ECanbShadow.CANRMP.bit.RMP0 = 1;
	//  ECanbRegs.CANRMP.all = ECanbShadow.CANRMP.all;

	ECanbRegs.CANRMP.all = 0xFFFFFFFF;

	//  ECanbShadow.CANGIF1.all = ECanbRegs.CANGIF1.all;
	//  ECanbShadow.CANGIF1.bit.GMIF1 = 1;
	//  ECanbRegs.CANGIF1.all = ECanbShadow.CANGIF1.all;

	ECanbRegs.CANGIF1.all = 0xFFFFFFFF;

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;

	EINT;

}

/************************************************************************/
/*



__interrupt void sciaTxFifoIsr(void)
{
SciaRegs.SCIFFTX.bit.TXFFINTCLR = 1;  // Clear Interrupt flag
PieCtrlRegs.PIEACK.all |= 0x100;      // Issue PIE ACK
}



__interrupt void scibTxFifoIsr(void)
{


ScibRegs.SCIFFTX.bit.TXFFINTCLR=1;  // Clear Interrupt flag
PieCtrlRegs.PIEACK.all|=0x100;      // Issue PIE ACK
}



__interrupt void scicTxFifoIsr(void)
{
ScicRegs.SCIFFTX.bit.TXFFINTCLR = 1;  // Clear Interrupt flag
PieCtrlRegs.PIEACK.all |= 0x0080;      // Issue PIE ACK
}


*/
/************************************************************************/
