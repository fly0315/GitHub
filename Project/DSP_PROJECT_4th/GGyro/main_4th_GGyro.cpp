#ifndef _DEBUG
	#define DEBUG 0
#endif
//###########################################################################
// Description:
//!
//! is required. Timer0 and CanA/B interrupts and the SCIA/B/C FIFOs are used.
//!
//! SCI-A receive GD data,SCI-B for Acc and SCI-C for Gyro.
//!
//###########################################################################
// 
// $Release Date: 20180131 $
// 
//###########################################################################

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include <string.h>
#include "GpioConfig.h"
#include "Fr_UNIFIED.h"
#include "FlexRayInterface.h"
#include "CanPortConfig.h"
#include "SerialPortConfig.h"
#include "CSerialPortDataAnalyzer.h"

#include "PtThreadEx.h"
#include "DataInterfaceGGyro.h"
#include "ThreadDataGGyro.h"
#include "InterruptsGGyro.h"

#include "TestData.h"

typedef enum {ChongQing_TJ = 0, JiangSu_CF} _ACC_TYPE;
_ACC_TYPE accType = JiangSu_CF;

static const unsigned int TXFFIL_SCIA = 0;
static const unsigned int RXFFIL_SCIA = 14;
static const unsigned int TXFFIL_SCIB = 0;
static const unsigned int RXFFIL_SCIB = 6;
static const unsigned int TXFFIL_SCIC = 0;
static const unsigned int RXFFIL_SCIC = 8;

// Prototype statements for functions found within this file.
void InitGpio_Used();
void InitGpio_FrBus(void);
void Init_Xintf_FrBus(void);
void initGpio_PwrSupply();
void attachInterrupts(void);
void initInterrupts(void);
inline void EnablePwrSupply(unsigned int isEnable)
{
	GpioDataRegs.GPADAT.bit.GPIO3 = (isEnable > 0) ? 1 : 0;
}


void error(void);
void initData(void);
void PackageFrFrame();
// Global variables
// serial port
CSerialPortDataAnalyzer GetPkg_Sci_A(   14,   0x55,   13, 11, 2,  0,  0xAA,   0       );
CSerialPortDataAnalyzer GetPkg_Sci_B(   6,    0x55,   5,  4,  1,  1,  0,      0       );
CSerialPortDataAnalyzer GetPkg_Sci_C(   8,    0xA5,   6,  4,  2,  0,  0xA5,   0x0D    );

UnlockFifoBuffer FifoBuffer_Sci_A(64);
UnlockFifoBuffer FifoBuffer_Sci_B(32);
UnlockFifoBuffer FifoBuffer_Sci_C(32);

extern CanCheGyro CheGyroData_;
extern SciGyroSensorData GyroSensorData_;
extern SciAccelerometerSensorData AcceSensorData_;
extern SciNaviData NaviData_;

// flexray port
extern volatile uint16 * FR_REG_FAR Fr_CC_reg_ptr;
void reset_FlexRay();
extern "C"
int vfnFlexRay_Init();
extern "C"
Fr_POC_state_type Fr_get_POC_state();
int FR_RET = 0;
extern FlexRayTxData FrTxData_GG_Slot02;
extern FlexRayTxData FrTxData_GG_Slot12;
//Thread Data
extern  PtThreadEx ThreadQ[];
extern Semaphore SMPHR[];
unsigned long PERIOD_MS_Q[] =
{
	0,		// 	TKIN_TX_FR_FRAME = 0,事件触发任务, GGyro发送周期0.5ms。
	100,	//  TKIN_SCIC,
	100,	// 	TKIN_SCIA,
	100,	// 	TKIN_CANA,
	100,	// 	TKIN_SCIB,
//	100,	// 	TKIN_CANB,
	1000	// 	TKIN_FR_GUARANTEE,
};
static int Task_FR_TX(struct pt *pt);
static int Task_SCIA(struct pt *pt);
static int Task_SCIB(struct pt *pt);
static int Task_SCIC(struct pt *pt);
static int Task_CANA(struct pt *pt);
//static int Task_CANB(struct pt *pt);
static int Task_FR_Guarantee(struct pt *pt);
static TASK TaskArray[] = {
	Task_FR_TX,
	Task_SCIA,
	Task_SCIB,
	Task_SCIC,
	Task_CANA,
//	Task_CANB,
	Task_FR_Guarantee,
};
#if !DEBUG
    extern Uint16 econst_loadstart;
    extern Uint16 econst_runstart;
    extern Uint16 econst_size;

    extern Uint16 cinit_loadstart;
    extern Uint16 cinit_runstart;
    extern Uint16 cinit_size;

    extern Uint16 const_loadstart;
    extern Uint16 const_runstart;
    extern Uint16 const_size;

    extern Uint16 pinit_loadstart;
    extern Uint16 pinit_runstart;
    extern Uint16 pinit_size;

    extern Uint16 switch_loadstart;
    extern Uint16 switch_runstart;
    extern Uint16 switch_size;

    extern Uint16 text_loadstart;
    extern Uint16 text_runstart;
    extern Uint16 text_size;
#endif
// Test data
//#define SCOPE_SIZE 1024
//int value[SCOPE_SIZE]={0};
//void Scope(void)
//{
//    static int i=0;
//    if (i < SCOPE_SIZE) { i++; }else {i = 0;}
//    value[i]=(int)(GyroSensorData_.m_f32Speed * 500 * -1);
//}
// Just for communication test.
TestDataCnt TestSci_A, TestSci_B, TestSci_C, TestCan_A;
void main(void)
{

// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the DSP2833x_SysCtrl.c file.

    InitSysCtrl();
    Init_Xintf_FrBus();

// Step 2. Initialize GPIO:
// This example function is found in the DSP2833x_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
// InitGpio();
// Setup only the GP I/O only for SCI-A and SCI-B functionality
// This function is found in DSP2833x_Sci.c
	InitGpio_Used();

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
	sci_fifo_init(sci_a, 921600, 'E', TXFFIL_SCIA, RXFFIL_SCIA);  // Init SCI-A
    sci_fifo_init(sci_b, 921600, 'N', TXFFIL_SCIB, RXFFIL_SCIB);  // Init SCI-B
    sci_fifo_init(sci_c, 921600, 'N', TXFFIL_SCIC, RXFFIL_SCIC);  // Init SCI-C

    //
    // Initialize eCAN module.Default Baud Rate 1M Hz
    //
    InitECan();

	InitCan_Customed(eCan_a);
	InitCan_Customed(eCan_b);

    // init CPU timers
    InitCpuTimers();   // For this example, only initialize the Cpu Timers

    // Configure CPU-Timer 2
	ConfigCpuTimer(&CpuTimer1, CPU_FREQ*1E-6, PERIOD_TIMER1_US);
	ConfigCpuTimer(&CpuTimer2, CPU_FREQ*1E-6, PERIOD_TIMER2_US);

    StartCpuTimer2();
	// Step X. Flash.
    // This step is not necessary when all sections have been copied from Flash to RAM.
#if !DEBUG
//	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
//	memcpy(&econst_runstart, &econst_loadstart, (Uint32)(&econst_size));
//	memcpy(&text_runstart, &text_loadstart, (Uint32)(&text_size));
//	memcpy(&const_runstart, &const_loadstart, (Uint32)(&const_size));
//	memcpy(&cinit_runstart, &cinit_loadstart, (Uint32)(&cinit_size));
//	memcpy(&pinit_runstart, &pinit_loadstart, (Uint32)(&pinit_size));
//	memcpy(&switch_runstart, &switch_loadstart, (Uint32)(&switch_size));
	InitFlash();
#endif
// Step 5. User specific code, enable interrupts:

// Init send data.  After each transmission this data
// will be updated for the next transmission
    initData();

// Enable interrupts required for this example
    initInterrupts();

// FlexRay Config

	EnablePwrSupply(1);
 	DELAY_US(300000L);
	reset_FlexRay();
	DELAY_US(300000L);
	FR_RET = vfnFlexRay_Init();


// Enable global Interrupts and higher priority real-time debug events:
	EINT;   // Enable Global interrupt INTM
	ERTM;   // Enable Global realtime interrupt DBGM
// Step 6. IDLE loop. Just sit and loop forever (optional):
	for (;;)
	{
		ThreadSchedule();
	}
}

void error(void)
{
    __asm("     ESTOP0"); // Test failed!! Stop!
    for (;;);
}

void PackageFrFrame()
{
    static float resault = 0;
	unsigned int STCode = 0;
	unsigned int* pFrTxArry02 = (unsigned int*)FrTxData_GG_Slot02.m_u16TxDataArry;
	unsigned int* pFrTxArry12 = (unsigned int*)FrTxData_GG_Slot12.m_u16TxDataArry;

	/** 故障位，高8位 **/
    STCode += NaviData_.m_i16BGST_X
        + (NaviData_.m_i16BGST_Y << 1)
        + (NaviData_.m_i16BGST_Y << 2)
        + (NaviData_.m_i16CommST_Navi << 3)
        + (CheGyroData_.m_BGCode << 4)
        + (GyroSensorData_.m_i16BGCode << 6);

    STCode = (STCode << 8);
	/** 工作状态位，低8位 **/
	STCode += NaviData_.m_i16WKST_X
		+ (NaviData_.m_i16WKST_Y << 1)
		+ (NaviData_.m_i16WKST_Z << 2)
		+ (AcceSensorData_.m_i16WKST << 3)
		+ (CheGyroData_.m_WkST << 4)
		+ (GyroSensorData_.m_i16WKST << 5)				// 火炮陀螺内部光纤陀螺传感器状态，张鹏口头修改协议20180420。
		+ (GyroSensorData_.m_i16WKST << 6);             // 火炮陀螺部件状态。如果故障了，也无法发送数据了，
														// 所以从逻辑上分析该状态等同于无效。
	pFrTxArry12[1] = pFrTxArry02[1] = STCode;
	if (SMPHR[TKIN_SCIC].Get() > 0)
	{
		pFrTxArry12[2] = pFrTxArry02[2] = (int)(GyroSensorData_.m_f32Speed * 500 * -1);    // 系数0.002
	}
	if (SMPHR[TKIN_CANA].Get() > 0)
	{
		pFrTxArry12[3] = pFrTxArry02[3] = (int)(CheGyroData_.m_f32Gyro * 100);             // 系数0.01
	}
	if (SMPHR[TKIN_SCIB].Get() > 0)
	{
		pFrTxArry12[4] = pFrTxArry02[4] = (int)(AcceSensorData_.m_f32Acc * 100 * -1);      // 系数0.01。m/s^2 极性上正下负，需要核对安装方向
	}
	/** 惯导坐标与火炮坐标三轴极性一致。**/
	if (SMPHR[TKIN_SCIA].Get() > 0)
	{
		pFrTxArry12[5] = pFrTxArry02[5] = (int)(NaviData_.m_f32Gyro_X * 500);        // 系数0.002。x轴指向右，右手定则为正
		pFrTxArry12[6] = pFrTxArry02[6] = (int)(NaviData_.m_f32Gyro_Y * 500);        // 系数0.002。y轴指向前，右手定则为正
		pFrTxArry12[7] = pFrTxArry02[7] = (int)(NaviData_.m_f32Gyro_Z * 500);        // 系数0.002。z轴指向上，右手定则为反
	}
//	Scope();
}

void attachInterrupts()
{
    // ISR functions found within this file.
    EALLOW;  // This is needed to write to EALLOW protected registers
// 	PieVectTable.TINT0 = &cpu_timer0_isr;
	PieVectTable.XINT13 = &cpu_timer1_isr;
	PieVectTable.TINT2 = &cpu_timer2_isr;

    PieVectTable.SCIRXINTA = &sciaRxFifoIsr;
// //     PieVectTable.SCITXINTA = &sciaTxFifoIsr;
    PieVectTable.SCIRXINTB = &scibRxFifoIsr;
// //     PieVectTable.SCITXINTB = &scibTxFifoIsr;
    PieVectTable.SCIRXINTC = &scicRxFifoIsr;
//     PieVectTable.SCITXINTC = &scicTxFifoIsr;

	PieVectTable.ECAN1INTA = &ECAN1A_ISR;
//	PieVectTable.ECAN1INTB = &ECAN1B_ISR;

    EDIS;   // This is needed to disable write to EALLOW protected registers
}

void initData(void)
{
    FifoBuffer_Sci_A.Initialize();
    FifoBuffer_Sci_B.Initialize();
    FifoBuffer_Sci_C.Initialize();

	// Create threads
	for (int i = 0; i < SCHED_MAX; i++)
	{
		SMPHR[i].Init(0);
		ThreadQ[i].CreateTask(TaskArray[i], PERIOD_MS_Q[i], WAIT, NULL);
	}
}

void initInterrupts(void)
{
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
    PieCtrlRegs.PIEIER9.bit.INTx1=1;     // PIE Group 9, INT1. SCIRXINTA
//     PieCtrlRegs.PIEIER9.bit.INTx2=1;     // PIE Group 9, INT2. SCITXINTA
    PieCtrlRegs.PIEIER9.bit.INTx3=1;     // PIE Group 9, INT3. SCIRXINTB
//     PieCtrlRegs.PIEIER9.bit.INTx4=1;     // PIE Group 9, INT4. SCITXINTB
    PieCtrlRegs.PIEIER8.bit.INTx5=1;     // PIE Group 8, INT5. SCIRXINTC
//     PieCtrlRegs.PIEIER8.bit.INTx6=1;     // PIE Group 8, INT6. SCITXINTC
//     PieCtrlRegs.PIEIER1.bit.INTx7 = 1;  // PIE Group 1, INT7. TINT0. (Timer1 and Timer2 no PieGroup)

//  	PieCtrlRegs.PIEIER9.bit.INTx5 = 1;	//CAN0-A
//  	PieCtrlRegs.PIEIER9.bit.INTx7 = 1;	//CAN0-B
	PieCtrlRegs.PIEIER9.bit.INTx6 = 1;	//CAN1-A
//	PieCtrlRegs.PIEIER9.bit.INTx8 = 1;	//CAN1-B

    IER |= M_INT8;   // Enable CPU INT8 which is connected to SCI-C:
    IER |= M_INT9;   // Enable CPU INT8 which is connected to SCI-A and SCI-B

// 	IER |= M_INT1;   // Enable CPU int1 which is connected to CPU-Timer 0
 	IER |= M_INT9;	// Enable CPU int9 which is connected to eCAN0/1
    IER |= M_INT13;   // Enable CPU int13 which is connected to CPU-Timer 1. Timer1 and Timer2 no PieGroup
    IER |= M_INT14;   // Enable CPU int14 which is connected to CPU-Timer 2. Timer1 and Timer2 no PieGroup
}

void reset_FlexRay()
{
	int i, j;//20170918

	//	GpioDataRegs.GPASET.bit.GPIO12 = 1;		//输出高电平
	GpioDataRegs.GPACLEAR.bit.GPIO12 = 1;	//输出低电平
	for (j = 0; j < 20; j++)
	{
		for (i = 0; i < 30000; i++) ;//延时  20170918
	}
	GpioDataRegs.GPASET.bit.GPIO12 = 1;		//输出高电平
	for (i = 0; i < 30000; i++); //20170918

}

void InitGpio_FrBus()
{
	EALLOW;

	GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 3;  // XD15
	GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 3;  // XD14
	GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 3;  // XD13
	GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 3;  // XD12
	GpioCtrlRegs.GPCMUX1.bit.GPIO68 = 3;  // XD11
	GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 3;  // XD10
	GpioCtrlRegs.GPCMUX1.bit.GPIO70 = 3;  // XD19
	GpioCtrlRegs.GPCMUX1.bit.GPIO71 = 3;  // XD8
	GpioCtrlRegs.GPCMUX1.bit.GPIO72 = 3;  // XD7
	GpioCtrlRegs.GPCMUX1.bit.GPIO73 = 3;  // XD6
	GpioCtrlRegs.GPCMUX1.bit.GPIO74 = 3;  // XD5
	GpioCtrlRegs.GPCMUX1.bit.GPIO75 = 3;  // XD4
	GpioCtrlRegs.GPCMUX1.bit.GPIO76 = 3;  // XD3
	GpioCtrlRegs.GPCMUX1.bit.GPIO77 = 3;  // XD2
	GpioCtrlRegs.GPCMUX1.bit.GPIO78 = 3;  // XD1
	GpioCtrlRegs.GPCMUX1.bit.GPIO79 = 3;  // XD0

	GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 3;  // XA0/XWE1n
	GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 3;  // XA1
	GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 3;  // XA2
	GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 3;  // XA3
	GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 3;  // XA4
	GpioCtrlRegs.GPBMUX1.bit.GPIO45 = 3;  // XA5
	GpioCtrlRegs.GPBMUX1.bit.GPIO46 = 3;  // XA6
	GpioCtrlRegs.GPBMUX1.bit.GPIO47 = 3;  // XA7

	GpioCtrlRegs.GPCMUX2.bit.GPIO80 = 3;  // XA8
	GpioCtrlRegs.GPCMUX2.bit.GPIO81 = 3;  // XA9
	GpioCtrlRegs.GPCMUX2.bit.GPIO82 = 3;  // XA10
	GpioCtrlRegs.GPCMUX2.bit.GPIO83 = 3;  // XA11
	GpioCtrlRegs.GPCMUX2.bit.GPIO84 = 3;  // XA12
	GpioCtrlRegs.GPCMUX2.bit.GPIO85 = 3;  // XA13
	GpioCtrlRegs.GPCMUX2.bit.GPIO86 = 3;  // XA14
	GpioCtrlRegs.GPCMUX2.bit.GPIO87 = 3;  // XA15
	GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 3;  // XA16
	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 3;  // XA17
	GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 3;  // XA18
	GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 3;  // XA19

	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 3;  // XREADY
    GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 3;  // XR/W
    GpioCtrlRegs.GPBMUX1.bit.GPIO38 = 3;  // XWE0

    GpioCtrlRegs.GPBMUX1.bit.GPIO37 = 3;  // XZCS7
    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 3;  // XZCS6

	EDIS;
}

void Init_Xintf_FrBus(void)
{
    // This shows how to write to the XINTF registers.  The
    // values used here are the default state after reset.
    // Different hardware will require a different configuration.

    // For an example of an XINTF configuration used with the
    // F28335 eZdsp, refer to the examples/run_from_xintf project.

    // Any changes to XINTF timing should only be made by code
    // running outside of the XINTF.

    // All Zones---------------------------------
    // Timing for all zones based on XTIMCLK = 1/2 SYSCLKOUT
    EALLOW;
    XintfRegs.XINTCNF2.bit.XTIMCLK = 1;
    // No write buffering
    XintfRegs.XINTCNF2.bit.WRBUFF = 0;
    // XCLKOUT is enabled
    XintfRegs.XINTCNF2.bit.CLKOFF = 0;
    // XCLKOUT = XTIMCLK/2
    XintfRegs.XINTCNF2.bit.CLKMODE = 1;

/*
    // Zone 0------------------------------------
    // When using ready, ACTIVE must be 1 or greater
    // Lead must always be 1 or greater
    // Zone write timing
    XintfRegs.XTIMING0.bit.XWRLEAD = 3;
    XintfRegs.XTIMING0.bit.XWRACTIVE = 7;
    XintfRegs.XTIMING0.bit.XWRTRAIL = 3;
    // Zone read timing
    XintfRegs.XTIMING0.bit.XRDLEAD = 3;
    XintfRegs.XTIMING0.bit.XRDACTIVE = 7;
    XintfRegs.XTIMING0.bit.XRDTRAIL = 3;

    // double all Zone read/write lead/active/trail timing
    XintfRegs.XTIMING0.bit.X2TIMING = 1;

    // Zone will sample XREADY signal
    XintfRegs.XTIMING0.bit.USEREADY = 1;
    XintfRegs.XTIMING0.bit.READYMODE = 1;  // sample asynchronous

    // Size must be either:
    // 0,1 = x32 or
    // 1,1 = x16 other values are reserved
    XintfRegs.XTIMING0.bit.XSIZE = 3;
*/
    // Zone 6------------------------------------
    // When using ready, ACTIVE must be 1 or greater
    // Lead must always be 1 or greater
    // Zone write timing
    XintfRegs.XTIMING6.bit.XWRLEAD = 2;
    XintfRegs.XTIMING6.bit.XWRACTIVE = 4;
    XintfRegs.XTIMING6.bit.XWRTRAIL = 2;
    // Zone read timing
    XintfRegs.XTIMING6.bit.XRDLEAD = 2;
    XintfRegs.XTIMING6.bit.XRDACTIVE = 4;
    XintfRegs.XTIMING6.bit.XRDTRAIL = 2;

    // double all Zone read/write lead/active/trail timing
    XintfRegs.XTIMING6.bit.X2TIMING = 1;

    // Zone will sample XREADY signal
    XintfRegs.XTIMING6.bit.USEREADY = 1;
    XintfRegs.XTIMING6.bit.READYMODE = 1;  // sample asynchronous

    // Size must be either:
    // 0,1 = x32 or
    // 1,1 = x16 other values are reserved
    XintfRegs.XTIMING6.bit.XSIZE = 3;


    // Zone 7------------------------------------
    // When using ready, ACTIVE must be 1 or greater
    // Lead must always be 1 or greater
    // Zone write timing
    XintfRegs.XTIMING7.bit.XWRLEAD = 3;
    XintfRegs.XTIMING7.bit.XWRACTIVE = 7;
    XintfRegs.XTIMING7.bit.XWRTRAIL = 3;
    // Zone read timing
    XintfRegs.XTIMING7.bit.XRDLEAD = 3;
    XintfRegs.XTIMING7.bit.XRDACTIVE = 7;
    XintfRegs.XTIMING7.bit.XRDTRAIL = 3;

    // double all Zone read/write lead/active/trail timing
    XintfRegs.XTIMING7.bit.X2TIMING = 1;

    // Zone will sample XREADY signal
    XintfRegs.XTIMING7.bit.USEREADY = 1;
    XintfRegs.XTIMING7.bit.READYMODE = 1;  // sample asynchronous

    // Size must be either:
    // 0,1 = x32 or
    // 1,1 = x16 other values are reserved
    XintfRegs.XTIMING7.bit.XSIZE = 3;

    // Bank switching
    // Assume Zone 7 is slow, so add additional BCYC cycles
    // when ever switching from Zone 7 to another Zone.
    // This will help avoid bus contention.
    XintfRegs.XBANK.bit.BANK = 7;
    XintfRegs.XBANK.bit.BCYC = 7;
    EDIS;
   //Force a pipeline flush to ensure that the write to
   //the last register configured occurs before returning.


   asm(" RPT #7 || NOP");

}

void initGpio_PwrSupply()
{
	EALLOW;
// 	GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;	// Set as output
// 	GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;	// Enable pull-up
	pinMode(PWREN, OUTPUT);
	EnablePwrSupply(0);
	EDIS;
}

void InitGpio_Used()
{
	InitGpio_Default();

	InitGpio_SCI_A(29, 36);
	InitGpio_SCI_B(9, 11);
	InitGpio_SCI_C(63, 62);

	InitECanGpio_Can_A_B();

	InitGpio_FrBus();
	pinMode(RESET4310, OUTPUT);
	pinMode(INT_CC4310, OUTPUT);
	pinMode(DOG, OUTPUT);

	initGpio_PwrSupply();
}
// 事件触发任务：只要有数据更新，就会触发总线发送任务。
int Task_FR_TX(struct pt *pt)
{
	/* A protothread function must begin with PT_BEGIN() which takes a
	pointer to a struct pt. */
	PT_BEGIN(pt);

	/* We loop forever here. */
	while (1)
	{
		/* Wait until the SCIA receive interrupt has set its flag. */
		PT_WAIT_UNTIL(pt, (ThreadQ[TKIN_TX_FR_FRAME].GetState()));
		// Prepare Flexray Bus Frame.
		PackageFrFrame();
		// Datat Transmission 
		FrTxData_GG_Slot02.txData();
		FrTxData_GG_Slot12.txData();

// 		ThreadQ[TKIN_TX_FR_FRAME].SLEEP_US(100);
//      PT_WAIT_UNTIL(pt, ThreadQ[TKIN_TX_FR_FRAME].IsAwake_US());


		ThreadQ[TKIN_TX_FR_FRAME].SetState(WAIT);
		/* And we loop. */
	}

	/* All protothread functions must end with PT_END() which takes a
	pointer to a struct pt. */
	PT_END(pt);
}

int Task_SCIA(struct pt *pt)
{
	/* A protothread function must begin with PT_BEGIN() which takes a
	pointer to a struct pt. */
	PT_BEGIN(pt);

	/* We loop forever here. */
	while (1)
	{
		/* Wait until the SCIA receive interrupt has set its flag. */
		PT_WAIT_UNTIL(pt, ((FifoBuffer_Sci_A.GetDataLen() >= GetPkg_Sci_A.GetFrameLen()) || ThreadQ[TKIN_SCIA].GetState()));
		// Time-Out for SCI receive data.
		if (TIMEOUT == ThreadQ[TKIN_SCIA].GetState())
		{
			TestSci_A.TimeOut++;
			// Reset SCI configuration.
			NaviData_.m_i16CommST_Navi = 1;

			NaviData_.m_i16WKST_X = 0;
			NaviData_.m_i16WKST_Y = 0;
			NaviData_.m_i16WKST_Z = 0;

			NaviData_.m_i16BGST_X = 1;
			NaviData_.m_i16BGST_Y = 1;
			NaviData_.m_i16BGST_Z = 1;

			sci_fifo_init(sci_a, 921600, 'E', TXFFIL_SCIA, RXFFIL_SCIA);  // Init SCI-A

		}
		else
		{
			// 1. Kick timer.
			ThreadQ[TKIN_SCIA].ResetTimer();
			// 2. Analyze frames.
			// analyze frame
			// 2.1 Get right frame.
			if (1 == GetPkg_Sci_A.Unpackage(&FifoBuffer_Sci_A))
			{
				// do sth
				TestSci_A.Right++;
				NaviData_.GetDataFromFrame(GetPkg_Sci_A.GetFrameBufferAddr());
				NaviData_.m_i16CommST_Navi = 0;
				// 信号量置位
				SMPHR[TKIN_SCIA].Set();

			}
			// 2.2 Get wrong frame
			else
			{
				// do sth
			    TestSci_A.Error++;
				NaviData_.m_i16CommST_Navi = 1;

				NaviData_.m_i16WKST_X = 0;
				NaviData_.m_i16WKST_Y = 0;
				NaviData_.m_i16WKST_Z = 0;

				NaviData_.m_i16BGST_X = 1;
				NaviData_.m_i16BGST_Y = 1;
				NaviData_.m_i16BGST_Z = 1;
			}

		}
		// 启动发送任务
		ThreadQ[TKIN_TX_FR_FRAME].SetState(READY);
		// 刷新任务流
		OSSchedule();
		ThreadQ[TKIN_SCIA].SetState(WAIT);
		/* And we loop. */
	}

	/* All protothread functions must end with PT_END() which takes a
	pointer to a struct pt. */
	PT_END(pt);
}

int Task_SCIB(struct pt *pt)
{
	/* A protothread function must begin with PT_BEGIN() which takes a
	pointer to a struct pt. */
	PT_BEGIN(pt);

	/* We loop forever here. */
	while (1)
	{
		/* Wait until the SCIA receive interrupt has set its flag. */
		PT_WAIT_UNTIL(pt, ((FifoBuffer_Sci_B.GetDataLen() >= GetPkg_Sci_B.GetFrameLen()) || ThreadQ[TKIN_SCIB].GetState()));
		// Time-Out for SCI receive data.
		if (TIMEOUT == ThreadQ[TKIN_SCIB].GetState())
		{
			// Reset SCI configuration.
			AcceSensorData_.m_i16BGCode = 1;
			AcceSensorData_.m_i16WKST = 0;
			sci_fifo_init(sci_b, 921600, 'N', TXFFIL_SCIB, RXFFIL_SCIB);  // Init SCI-B
			TestSci_B.TimeOut++;
		}
		else
		{
			// 1. Kick timer.
			ThreadQ[TKIN_SCIB].ResetTimer();
			// 2. Analyze frames.
			if (1 == GetPkg_Sci_B.Unpackage(&FifoBuffer_Sci_B))
			{
				// do sth
				TestSci_B.Right++;
				AcceSensorData_.GetDataFromFrame(GetPkg_Sci_B.GetFrameBufferAddr(), accType);  // 0 重庆 1 江苏
				AcceSensorData_.m_i16BGCode = 0;
				AcceSensorData_.m_i16WKST = 1;
				// 信号量置位
				SMPHR[TKIN_SCIB].Set();

			}
			else
			{
				// do sth
				TestSci_B.Error++;
				AcceSensorData_.m_i16BGCode = 1;
				AcceSensorData_.m_i16WKST = 0;
			}
			/* And we loop. */
		}
		// 启动发送任务
        ThreadQ[TKIN_TX_FR_FRAME].SetState(READY);
		
		ThreadQ[TKIN_SCIB].SetState(WAIT);
		/* And we loop. */
	}

	/* All protothread functions must end with PT_END() which takes a
	pointer to a struct pt. */
	PT_END(pt);
}

int Task_SCIC(struct pt *pt)
{
	/* A protothread function must begin with PT_BEGIN() which takes a
	pointer to a struct pt. */
	PT_BEGIN(pt);

	/* We loop forever here. */
	while (1)
	{
		/* Wait until the SCIA receive interrupt has set its flag. */
		PT_WAIT_UNTIL(pt, ((FifoBuffer_Sci_C.GetDataLen() >= GetPkg_Sci_C.GetFrameLen()) || ThreadQ[TKIN_SCIC].GetState()));
		// Time-Out for SCI receive data.
		if (TIMEOUT == ThreadQ[TKIN_SCIC].GetState())
		{
			// Reset SCI configuration.
			TestSci_C.TimeOut++;			
			GyroSensorData_.m_i16BGCode = 1;
			GyroSensorData_.m_i16WKST = 0;
			sci_fifo_init(sci_c, 921600, 'N', TXFFIL_SCIC, RXFFIL_SCIC);  // Init SCI-C
		}
		else
		{
			// 1. Kick timer.
			ThreadQ[TKIN_SCIC].ResetTimer();
			// 2. Analyze frames.
			if (1 == GetPkg_Sci_C.Unpackage(&FifoBuffer_Sci_C))
			{
				// do sth
				TestSci_C.Right++;
				GyroSensorData_.m_i16BGCode = 0;
				GyroSensorData_.m_i16WKST = 1;
				GyroSensorData_.GetDataFromFrame(GetPkg_Sci_C.GetFrameBufferAddr());
				// 信号量置位
				SMPHR[TKIN_SCIC].Set();

			}
			else
			{
				// do sth
				TestSci_C.Error++;
				GyroSensorData_.m_i16BGCode = 1;
				GyroSensorData_.m_i16WKST = 0;
			}
		}
		// 启动发送任务
        ThreadQ[TKIN_TX_FR_FRAME].SetState(READY);
		// 刷新任务流
		OSSchedule();
		ThreadQ[TKIN_SCIC].SetState(WAIT);
		/* And we loop. */
	}

	/* All protothread functions must end with PT_END() which takes a
	pointer to a struct pt. */
	PT_END(pt);
}

int Task_CANA(struct pt *pt)
{
	/* A protothread function must begin with PT_BEGIN() which takes a
	pointer to a struct pt. */
	PT_BEGIN(pt);

	/* We loop forever here. */
	while (1)
	{
		/* Wait until the SCIA receive interrupt has set its flag. */
		PT_WAIT_UNTIL(pt, ThreadQ[TKIN_CANA].GetState());
		// Time-Out for SCI receive data.
		if (TIMEOUT == ThreadQ[TKIN_CANA].GetState())
		{
			TestCan_A.TimeOut++;
			// Reset CAN configuration.
		    CheGyroData_.m_BGCode = 1;
		    CheGyroData_.m_WkST = 0;
			InitCan_Customed(eCan_a);
		}
		else
		{
			if (ECanaMboxes.MBOX31.MSGID.bit.STDMSGID == 0x1A8)
			{
				TestCan_A.Right++;
			    // 1. Kick timer.
			    ThreadQ[TKIN_CANA].ResetTimer();
			    // 2. Analyze frames.
			    CheGyroData_.m_BGCode = 0;
			    CheGyroData_.m_WkST = 1;
				CheGyroData_.GetDataFromFrame(&ECanaMboxes.MBOX31);
				// 3.Set semaphore.
				SMPHR[TKIN_CANA].Set();
				// 4.Start bus-transmit task.
				ThreadQ[TKIN_TX_FR_FRAME].SetState(READY);
			}
			else
			{
				// do sth
				TestCan_A.Error++;
			}
		}
		// 启动发送任务
        ThreadQ[TKIN_TX_FR_FRAME].SetState(READY);
		ThreadQ[TKIN_CANA].SetState(WAIT);
		/* And we loop. */
	}

	/* All protothread functions must end with PT_END() which takes a
	pointer to a struct pt. */
	PT_END(pt);
}
// 线缆只有6脚7脚canA的数据，故取消canB路线程
//int Task_CANB(struct pt *pt)
//{
//	/* A protothread function must begin with PT_BEGIN() which takes a
//	pointer to a struct pt. */
//	PT_BEGIN(pt);
//
//	/* We loop forever here. */
//	while (1)
//	{
//		/* Wait until the SCIA receive interrupt has set its flag. */
//		PT_WAIT_UNTIL(pt, ThreadQ[TKIN_CANB].GetState());
//		// Time-Out for SCI receive data.
//		if (TIMEOUT == ThreadQ[TKIN_CANB].GetState())
//		{
//		    CheGyroData_.m_BGCode = 1;
//		    CheGyroData_.m_WkST = 0;
//			// Reset CAN configuration.
//			InitCan_Customed(eCan_b);
//		}
//		else
//		{
//			if (ECanbMboxes.MBOX31.MSGID.bit.STDMSGID == 0x1A8)
//			{
//			    // 1. Kick timer.
//			    ThreadQ[TKIN_CANB].ResetTimer();
//			    CheGyroData_.m_BGCode = 0;
//			    CheGyroData_.m_WkST = 1;
//			    // 2. Analyze frames.
//				CheGyroData_.GetDataFromFrame(&ECanbMboxes.MBOX31);
//			}
//			else
//			{
//				// do sth
//			}
//		}
//		ThreadQ[TKIN_CANB].SetState(WAIT);
//		/* And we loop. */
//	}
//
//	/* All protothread functions must end with PT_END() which takes a
//	pointer to a struct pt. */
//	PT_END(pt);
//}

int Task_FR_Guarantee(struct pt *pt)
{
	/* A protothread function must begin with PT_BEGIN() which takes a
	pointer to a struct pt. */
	PT_BEGIN(pt);

	/* We loop forever here. */
	while (1)
	{
		/* Wait until the timer on. */
		PT_WAIT_UNTIL(pt, ThreadQ[TKIN_FR_GUARANTEE].GetState());

		if (FR_RET != 0)
		{
		    digitalWrite(RESET4310, LOW);   //输出低电平
			ThreadQ[TKIN_FR_GUARANTEE].SLEEP_MS(1);
            PT_WAIT_UNTIL(pt, ThreadQ[TKIN_FR_GUARANTEE].IsAwake_MS());

            digitalWrite(RESET4310, HIGH);     //输出高电平
			ThreadQ[TKIN_FR_GUARANTEE].SLEEP_MS(1);
            PT_WAIT_UNTIL(pt, ThreadQ[TKIN_FR_GUARANTEE].IsAwake_MS());
			FR_RET = vfnFlexRay_Init();
		}

		if ((Fr_get_POC_state() == FR_POCSTATE_HALT)
			|| (Fr_get_POC_state() == FR_POCSTATE_NORMAL_PASSIVE)
			|| (Fr_get_POC_state() == FR_POCSTATE_STARTUP)
//			|| (FR_RET != 0)
		    )
		{
		    Fr_CC_reg_ptr[FrPOCR] = ((Fr_CC_reg_ptr[FrPOCR] & 0xFFFB) | 0x0004);
			ThreadQ[TKIN_FR_GUARANTEE].SLEEP_MS(1);
			PT_WAIT_UNTIL(pt, ThreadQ[TKIN_FR_GUARANTEE].IsAwake_MS());
//		    for (int i = 0; i < 1000; i++) { ; }
		    FR_RET = vfnFlexRay_Init();

			Fr_CC_reg_ptr[FrPOCR] = ((Fr_CC_reg_ptr[FrPOCR] & 0xFFF0) | 0x0004);
			ThreadQ[TKIN_FR_GUARANTEE].SLEEP_MS(1);
			PT_WAIT_UNTIL(pt, ThreadQ[TKIN_FR_GUARANTEE].IsAwake_MS());
//			for (int i = 0; i < 1000; i++) { ; }
			FR_RET = vfnFlexRay_Init();
		}
		ThreadQ[TKIN_FR_GUARANTEE].SetState(WAIT);
		/* And we loop. */
	}

	/* All protothread functions must end with PT_END() which takes a
	pointer to a struct pt. */
	PT_END(pt);
}


//===========================================================================
// No more.
//===========================================================================

