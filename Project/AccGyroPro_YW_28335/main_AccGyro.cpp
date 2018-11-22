#ifndef _DEBUG
    #define DEBUG 1
#endif

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include <string.h>

#include "GpioConfig.h"
#include "SCC1300-D02_SPI.h"
#include "BSP_cfg.h"

// Prototype statements for functions found within this file.
// initial system
void InitGpio_Used();
void InitGpio_FrBus(void);
void attachInterrupts(void);
void initInterrupts(void);
void initData(void);


double rateX;
void error(void);



/*@}************************************************/

/* {@*********************************************************************/
/*
 *  Èí¼þÊ¾²¨Æ÷
 #define SCOPE_SIZE 1024
 float value[SCOPE_SIZE]={0};
 void Scope(float fData)
 {
    // TODO:Scope
     static int i=0;
     if (i < SCOPE_SIZE) { i++; }else {i = 0;}
     value[i]=fData;
 }
 */
/* @}*********************************************************************/


void main(void)
{

// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the DSP2833x_SysCtrl.c file.

    InitSysCtrl();

// Step 2. Initialize GPIO:
// This example function is found in the DSP2833x_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
// InitGpio();
// Setup only the GP I/O only for SCI-A and SCI-B functionality
// This function is found in DSP2833x_Sci.c
    InitGpio_Used();

    SCC1300_Init();
    GyroPowerup();
// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
    DINT;

// Initialize PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the DSP2833x_PieCtrl.c file.
    InitPieCtrl();

// Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;

// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in DSP2833x_DefaultIsr.c.
// This function is found in DSP2833x_PieVect.c.
    InitPieVectTable();

// Interrupts that are used in this example are re-mapped to
    attachInterrupts();

// Step 4. Initialize all the Device Peripherals:
// This function is found in DSP2833x_InitPeripherals.c
// InitPeripherals(); // Not required for this example
    // init SCI

    // init SPI.***Moved into initData();

    // init CPU timers
//    InitCpuTimers();   // For this example, only initialize the Cpu Timers
//
//    ConfigCpuTimer(&CpuTimer1,  CPU_FREQ*1E-6, PERIOD_TIMER1_US);
//    ConfigCpuTimer(&CpuTimer2,  CPU_FREQ*1E-6, PERIOD_TIMER2_US);
//    StartCpuTimer2();
// Step X. Flash.
#if !DEBUG
//    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    InitFlash();
#endif
// Step 5. User specific code, enable interrupts:

// Init send data.  After each transmission this data
// will be updated for the next transmission
    initData();

// Enable interrupts required for this example
    initInterrupts();



//  Write protection will on for 10 times continuity writing so we reset write enable.

// FlexRay Config
// Enable Power Supply(12V/5VW).

// Enable global Interrupts and higher priority real-time debug events:
    EINT;
    ERTM;
// Step 6. IDLE loop. Just sit and loop forever (optional):
    for (;;)
    {
        rateX = GetGyroRateX() * 0.02;
        DELAY_US(100);
    }
}

void error(void)
{
    __asm("     ESTOP0"); // Test failed!! Stop!
    for (;;);
}

/************************************************************************/

void attachInterrupts()
{
    // ISR functions found within this file.
    EALLOW;  // This is needed to write to EALLOW protected registers
//    PieVectTable.TINT0 = &cpu_timer0_isr;
//    PieVectTable.XINT13 = &cpu_timer1_isr;
//    PieVectTable.TINT2 = &cpu_timer2_isr;
//
//    PieVectTable.SCIRXINTA = &sciaRxFifoIsr;
//     PieVectTable.SCITXINTA = &sciaTxFifoIsr;
//    PieVectTable.SCIRXINTB = &scibRxFifoIsr;
//     PieVectTable.SCITXINTB = &scibTxFifoIsr;
//     PieVectTable.SCIRXINTC = &scicRxFifoIsr;
//     PieVectTable.SCITXINTC = &scicTxFifoIsr;

    EDIS;   // This is needed to disable write to EALLOW protected registers
}

void initData(void)
{
    SCC1300_Init();
    GyroPowerup();

}

void initInterrupts(void)
{
//    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
//    PieCtrlRegs.PIEIER9.bit.INTx1=1;     // PIE Group 9, INT1. SCIRXINTA
//    PieCtrlRegs.PIEIER9.bit.INTx2=1;     // PIE Group 9, INT2. SCITXINTA
//    PieCtrlRegs.PIEIER9.bit.INTx3=1;     // PIE Group 9, INT3. SCIRXINTB
//    PieCtrlRegs.PIEIER9.bit.INTx4=1;     // PIE Group 9, INT4. SCITXINTB
//    PieCtrlRegs.PIEIER8.bit.INTx5=1;     // PIE Group 8, INT5. SCIRXINTC
//    PieCtrlRegs.PIEIER8.bit.INTx6=1;     // PIE Group 8, INT6. SCITXINTC
//    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;  // PIE Group 1, INT7. TINT0. Timer1 and Timer2 no PieGroup

//    PieCtrlRegs.PIEIER9.bit.INTx5 = 1;  //CAN0-A
//    PieCtrlRegs.PIEIER9.bit.INTx7 = 1;  //CAN0-B
//    PieCtrlRegs.PIEIER9.bit.INTx6 = 1;  //CAN1-A
//    PieCtrlRegs.PIEIER9.bit.INTx8 = 1;  //CAN1-B

//    IER |= M_INT8;   // Enable CPU INT8 which is connected to SCI-C:
//    IER |= M_INT9;   // Enable CPU INT8 which is connected to SCI-A and SCI-B

//    IER |= M_INT1;   // Enable CPU int1 which is connected to CPU-Timer 0
//    IER |= M_INT9;  // Enable CPU int9 which is connected to eCAN0/1
//    IER |= M_INT13;   // Enable CPU int13 which is connected to CPU-Timer 1. Timer1 and Timer2 no PieGroup
//    IER |= M_INT14;   // Enable CPU int14 which is connected to CPU-Timer 2. Timer1 and Timer2 no PieGroup
}


void InitGpio_Used()
{
    pinMode(26, OUTPUT);
    pinMode(27, OUTPUT);
    digitalWrite(26, LOW);
    digitalWrite(27, HIGH);
}


//===========================================================================
// No more.
//===========================================================================

