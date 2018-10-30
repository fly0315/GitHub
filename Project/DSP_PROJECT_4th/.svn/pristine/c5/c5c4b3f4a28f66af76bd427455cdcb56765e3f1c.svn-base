#ifndef _DEBUG
    #define DEBUG 1
#endif

 /* {@*********************************************************************/
/*
 *      *******   操纵台的软件功能如下  ********
 *  1.串口接收两路光电编码器信号，用中断接收帧放入缓冲区，当缓冲区中数据大于帧长
 *  度，启动数据解析线程解析成瞄准信号。SCIA对应方位向，SCIB对应高低向。
 *  2.1ms定时置位IO信号采样线程，完成包括开关、按钮和写零位状态采集。
 *  对开关按钮进行16ms滑动窗口采样滤波，实现消抖功能
 *  3.100ms喂狗 IO3 输出一个低电平，持续几个毫秒；待实现？？？？？
 *  4.零位功能：分为正常模式和写零模式，写零模式又分为在写和写完状态。
 *  以LED灯闪烁作为工作模式区分标志：
 *  正常模式：2s慢闪，在写模式：150ms快闪，写完模式：熄灭
 *  零位功能在
 *  /
/* @}*********************************************************************/
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include <string.h>
#include "FlexRayInterface.h"
#include "Fr_UNIFIED.h"
#include "GpioConfig.h"
#include "SerialPortConfig.h"

#include "CSerialPortDataAnalyzer.h"
#include "DataInterFaceCZT.h"

#include "InterruptsCZT.h"
#include "PtThreadEx.h"
#include "ThreadDataCZT.h"

#include "TestData.h"

static const unsigned int IO_SampleWindow_ms = 16;
static const unsigned int TXFFIL_SCIA = 0;
static const unsigned int RXFFIL_SCIA = 4;
static const unsigned int TXFFIL_SCIB = 0;
static const unsigned int RXFFIL_SCIB = 4;
typedef enum _PERIOD_LED_TWINKLE_MS
{
	PERIOD_LED_NORMAL = 1000,
	PERIOD_LED_WRITE_OK =500,
	PERIOD_LED_WRITING = 100,
}PERIOD_LED_TWINKLE_MS;

// Prototype statements for functions found within this file.
// initial system
void InitGpio_Used();
void InitGpio_FrBus(void);
void InitGpio_IO(void);
void Init_Xintf_FrBus(void);
void attachInterrupts(void);
void initInterrupts(void);
void initData(void);

inline void EnablePwrSupply(unsigned int bEnable) { digitalWrite(PWREN, bEnable); }

void error(void);

// serial port data
CSerialPortDataAnalyzer GetPkg_Sci_A(4,0x80,3,2,1);
CSerialPortDataAnalyzer GetPkg_Sci_B(4,0x80,3,2,1);

UnlockFifoBuffer FifoBuffer_Sci_A(32);
UnlockFifoBuffer FifoBuffer_Sci_B(32);
// Interface data
extern SciCztMp CztMp_Hor_;
extern SciCztMp CztMp_Ver_;
extern IoCztBtnSw CztBtnSw_;
extern SpiCzt93C56 Czt93C56_;
extern CztDataBase CztData_;

// flexray port
extern volatile uint16 * FR_REG_FAR Fr_CC_reg_ptr;
void reset_FlexRay();
extern "C"
int vfnFlexRay_Init();
extern "C"
Fr_POC_state_type Fr_get_POC_state();

extern FlexRayTxData FrTxData_CZT_Slot31;
int FR_RET = 0;
/*{@************************************************/
extern  PtThreadEx ThreadQ[];
unsigned long PERIOD_MS_Q[] =
{
	2,		// TKIN_TX_FR_FRAME,
	0,		// TKIN_WRITE_ZERO,
	1,		// TKIN_SCAN_IO,
	100,	// TKIN_SCIA,
	100,	// TKIN_SCIB,
	1,		// TKIN_LAMP_H,
	1,		// TKIN_LAMP_V,
	1000,	// TKIN_LED_TWINKLE,
	1000,	// TKIN_FR_GUARANTEE,
};
static int Task_Tx_Fr_Frame(struct pt *pt);
static int Task_Write_Zero(struct pt *pt);
static int Task_IO(struct pt *pt);
static int Task_SCIA(struct pt *pt);
static int Task_SCIB(struct pt *pt);
static int Task_Lamp_1(struct pt *pt);
static int Task_Lamp_2(struct pt *pt);
static int Task_Led(struct pt *pt);
static int Task_Fr_Guarantee(struct pt *pt);
static TASK TaskArray[] = {
	Task_Tx_Fr_Frame,
	Task_Write_Zero,
	Task_IO,
	Task_SCIA,
	Task_SCIB,
	Task_Lamp_1,
	Task_Lamp_2,
	Task_Led,
	Task_Fr_Guarantee
};
/*@}************************************************/

/* {@*********************************************************************/
/*
 *  软件示波器
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
// just for sci receive frames test.
unsigned int error_cnt_SciFrame_A = 0, error_cnt_SciFrame_B = 0,
right_cnt_SciFrame_A = 0, right_cnt_SciFrame_B = 0;
TestDataCnt TestSci_A, TestSci_B;

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
    sci_fifo_init(sci_a, 115200, 'O', TXFFIL_SCIA, RXFFIL_SCIA);  // Init SCI-A
    sci_fifo_init(sci_b, 115200, 'O', TXFFIL_SCIB, RXFFIL_SCIB);  // Init SCI-B
	// init SPI.***Moved into initData();

    // init CPU timers
    InitCpuTimers();   // For this example, only initialize the Cpu Timers

    ConfigCpuTimer(&CpuTimer1,  CPU_FREQ*1E-6, PERIOD_TIMER1_US);
    ConfigCpuTimer(&CpuTimer2,  CPU_FREQ*1E-6, PERIOD_TIMER2_US);
	StartCpuTimer2();
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

// Read 93C56 Zero_X. 
	CztData_.ReloadZero_H(Czt93C56_.ReadZeroCode_H());
	CztData_.ReloadZero_V(Czt93C56_.ReadZeroCode_V());

//  Write protection will on for 10 times continuity writing so we reset write enable.
	Czt93C56_.ResetWriteEnable();
// FlexRay Config
// Enable Power Supply(12V/5VW).
    EnablePwrSupply(1);
    DELAY_US(300000L);
    reset_FlexRay();
    DELAY_US(300000L);
    FR_RET = vfnFlexRay_Init();
// Enable global Interrupts and higher priority real-time debug events:
    EINT;
    ERTM;
// Step 6. IDLE loop. Just sit and loop forever (optional):
	for (;;)
	{
		// In Scope-GPIO-Output test,  2~3 us were expended for one loop. LiuBin，20180301.
		// 每间隔300+us会出现一个6~7us的大执行周期，没搞清楚原理
	    // 注意：由于写零线程接受IO线程控制，因此写零线程应在IO扫描线程之后运行。
	    // 注意：LED闪烁线程中的LED闪烁周期是由IO采集线程和写零线程共同控制，所以要放到两个线程之后。这是设计缺陷！！！
	    // 0.SCI data handle.
		for (int i = 0; i < SCHED_MAX; i++)
		{
			ThreadQ[i].Schedule();
		}
// 		Task_SCIA(&pt_SCIA);
// 		Task_SCIB(&pt_SCIB); 
// 		// 1.IO sampling
// 		Task_IO(&pt_IO);
// 		// 2.write zero
// 		Task_Write_Zero(&pt_Write_zero);
// 		// 3.led twinkle
// 		Task_Led(&pt_Led_Twinkle);
// 		// 4.Transmit Flexray data.
// 		Task_Tx_Fr_Frame(&pt_Tx_Fr_Frame);
// 		// 5.SCI reset for communication timeout.
// 		Task_Reset_SCI(&pt_Reset_SCI);
// 		// 6.Flexray state check.
// //		Task_Fr_Guarantee(&pt_Fr_Guarantee);
// 		// 7.Set Lamp twinkle mode
// 		Task_Lamp_1(&pt_Lamp_1);
// 		Task_Lamp_2(&pt_Lamp_2);
//		digitalWrite(DSIN1, TOGGLE);
	}
}

void error(void)
{
    __asm("     ESTOP0"); // Test failed!! Stop!
    for (;;);
}
// 
// __interrupt void cpu_timer2_isr(void)
// {
//     EALLOW;
//     CpuTimer2.InterruptCount++;
//     
//     // The CPU acknowledges the interrupt.
// 	for (int i = 0; i < SCHED_MAX; i++)
// 	{
// 		ThreadQ[i].TimerRemarks();
// 	}
//     EDIS;
// }
// 
// //__interrupt void sciaTxFifoIsr(void)
// //{
// //    SciaRegs.SCIFFTX.bit.TXFFINTCLR = 1;  // Clear Interrupt flag
// //    PieCtrlRegs.PIEACK.all |= 0x100;      // Issue PIE ACK
// //}
// 
// __interrupt void sciaRxFifoIsr(void)
// {
//     // get data from ram buffer
// 	unsigned int rdata[16] = {0};
// 	unsigned int dataInFifoLen = SciaRegs.SCIFFRX.bit.RXFFST;
//     for (int i = 0; i < dataInFifoLen; i++)
//     {
//         rdata[i] = SciaRegs.SCIRXBUF.all;  // Read data
//     }
//     // Push data in circle buffer
//     FifoBuffer_Sci_A.Put((const unsigned int*)rdata, dataInFifoLen);
//     /**************************************************/
//     SciaRegs.SCIFFRX.bit.RXFFOVRCLR=1;   // Clear Overflow flag
//     SciaRegs.SCIFFRX.bit.RXFFINTCLR=1;   // Clear Interrupt flag
//     PieCtrlRegs.PIEACK.all|=0x100;       // Issue PIE ack
// }
// /*
// __interrupt void scibTxFifoIsr(void)
// {
// 
// 
//     ScibRegs.SCIFFTX.bit.TXFFINTCLR=1;  // Clear Interrupt flag
//     PieCtrlRegs.PIEACK.all|=0x100;      // Issue PIE ACK
// }
// */
// __interrupt void scibRxFifoIsr(void)
// {
//     // get data from ram buffer
//     unsigned int rdata[16] = { 0 };
// 	unsigned int dataInFifoLen = ScibRegs.SCIFFRX.bit.RXFFST;
//     for (int i = 0; i < dataInFifoLen; i++)
//     {
//         rdata[i] = ScibRegs.SCIRXBUF.all;  // Read data
//     }
//     // put in circle buffer
//     FifoBuffer_Sci_B.Put(rdata, dataInFifoLen);
//     /*************************************************/
//     ScibRegs.SCIFFRX.bit.RXFFOVRCLR=1;  // Clear Overflow flag
//     ScibRegs.SCIFFRX.bit.RXFFINTCLR=1;  // Clear Interrupt flag
//     PieCtrlRegs.PIEACK.all|=0x100;      // Issue PIE ack
// }
// /*
// __interrupt void scicTxFifoIsr(void)
// {
//     ScicRegs.SCIFFTX.bit.TXFFINTCLR = 1;  // Clear Interrupt flag
//     PieCtrlRegs.PIEACK.all |= 0x0080;      // Issue PIE ACK
// }
// 
// __interrupt void scicRxFifoIsr(void)
// {
// 
//     ScicRegs.SCIFFRX.bit.RXFFOVRCLR=1;   // Clear Overflow flag
//     ScicRegs.SCIFFRX.bit.RXFFINTCLR=1;   // Clear Interrupt flag
// 
//     PieCtrlRegs.PIEACK.all|=0x0080;       // Issue PIE ack
// }
//  */
// /************************************************************************/
// 触发任务:超时触发+事件触发，串口接收数据满足条件
int Task_SCIA(struct pt *pt)
{
  /* A protothread function must begin with PT_BEGIN() which takes a
     pointer to a struct pt. */
  PT_BEGIN(pt);

  /* We loop forever here. */
  while(1)
  {
    /* Wait until the SCIA receive interrupt has set its flag. */
    PT_WAIT_UNTIL(pt, ((FifoBuffer_Sci_A.GetDataLen() >= GetPkg_Sci_A.GetFrameLen()) || ThreadQ[TKIN_SCIA].GetState()));
	if (TIMEOUT == ThreadQ[TKIN_SCIA].GetState())
	{
		// Reset SCI configuration.
		InitGpio_SCI_A(29, 36);
		sci_fifo_init(sci_a, 115200, 'O', TXFFIL_SCIA, RXFFIL_SCIA);
		TestSci_A.TimeOut++;
	} 
	else
	{
		// 1. Kick timer.
		ThreadQ[TKIN_SCIA].ResetTimer();
		// 2. Analyze frames.
		// 2.1 Get right frame.
		if (1 == GetPkg_Sci_A.Unpackage(&FifoBuffer_Sci_A))
		{
			// do sth
			right_cnt_SciFrame_A++;
			TestSci_A.Right++;
			// get data from frames
			CztData_.Code2AimSignal_H(
				CztMp_Hor_.GetDataFromFrame(
					GetPkg_Sci_A.GetFrameBufferAddr()
				)
			);
			// Encoder data is available.
			CztMp_Hor_.m_bIsDataRcvd = 1;
		}
		// 2.2 Get wrong frame.
		else
		{
			// do sth
			error_cnt_SciFrame_A++;
			TestSci_A.Error++;
			CztMp_Hor_.m_bIsDataRcvd = 0;
		}

	}
	// Acknowledge the Reset-SCI timer.
	ThreadQ[TKIN_SCIA].SetState(WAIT);
    /* And we loop. */
  }

  /* All protothread functions must end with PT_END() which takes a
     pointer to a struct pt. */
  PT_END(pt);
}
// 触发任务:超时触发+事件触发，串口接收数据满足条件
int Task_SCIB(struct pt *pt)
{
  /* A protothread function must begin with PT_BEGIN() which takes a
     pointer to a struct pt. */
  PT_BEGIN(pt);

  /* We loop forever here. */
  while(1)
  {
    /* Wait until the SCIA receive interrupt has set its flag. */
    PT_WAIT_UNTIL(pt, ((FifoBuffer_Sci_B.GetDataLen() >= GetPkg_Sci_B.GetFrameLen()) || ThreadQ[TKIN_SCIB].GetState()));
	if (TIMEOUT == ThreadQ[TKIN_SCIB].GetState())
	{
		InitGpio_SCI_B(9, 11);
		sci_fifo_init(sci_b, 115200, 'O', TXFFIL_SCIB, RXFFIL_SCIB);
		TestSci_B.TimeOut++;
	} 
	else
	{
		// 1. Kick timer.
		ThreadQ[TKIN_SCIB].ResetTimer();
		// 2. Analyze frames.
		// 2.1 Get ritht frame.
		if (1 == GetPkg_Sci_B.Unpackage(&FifoBuffer_Sci_B))
		{
			// do sth
			right_cnt_SciFrame_B++;
			TestSci_B.Right++;
			// get data from frames
			CztData_.Code2AimSignal_V(
				CztMp_Ver_.GetDataFromFrame(
					GetPkg_Sci_B.GetFrameBufferAddr()
				)
			);
			// Decoder data is available.
			CztMp_Ver_.m_bIsDataRcvd = 1;
		}
		// 2.2 Get wrong frame.
		else
		{
			// do sth
			error_cnt_SciFrame_B++;
			TestSci_B.Error++;
			CztMp_Ver_.m_bIsDataRcvd = 0;
		}
	}

	// Acknowledge Reset-SCI timer.
	ThreadQ[TKIN_SCIB].SetState(WAIT);
    /* And we loop. */
  }

  /* All protothread functions must end with PT_END() which takes a
     pointer to a struct pt. */
  PT_END(pt);
}
// 定时任务，1ms
int Task_IO(struct pt *pt)
{
	/* A protothread function must begin with PT_BEGIN() which takes a
		pointer to a struct pt. */
	PT_BEGIN(pt);

	/* We loop forever here. */
	while(1)
	{
		/* Wait until the timer interrupt has set its flag. */
		PT_WAIT_UNTIL(pt, ThreadQ[TKIN_SCAN_IO].GetState());
		// Get IO state.
		unsigned long IOs = (unsigned long)GpioDataRegs.GPADAT.all;
		CztBtnSw_.BtnIOSampling(&GpioDataRegs.GPADAT.all);
		// put in a word. prepare for flexray frames.
		// The IO data with suffix '_L' means '0' represent 'ON' and '1' represent 'OFF'.
		// So the operator XOR reverse '0' to '1' and '1' to '0' is necessary. 
		CztData_.m_Fr_Word_IO.data.SA1 = (CztBtnSw_.m_Btn_Sw_IO.data.SA1_L ^ 1);
		CztData_.m_Fr_Word_IO.data.SA2 = (CztBtnSw_.m_Btn_Sw_IO.data.SA2_L ^ 1);
		CztData_.m_Fr_Word_IO.data.SC4 = (CztBtnSw_.m_Btn_Sw_IO.data.SC4_L ^ 1);
		CztData_.m_Fr_Word_IO.data.SC3 = (CztBtnSw_.m_Btn_Sw_IO.data.SC3_L1 ^ 1);
		CztData_.m_Fr_Word_IO.data.SC2 = (CztBtnSw_.m_Btn_Sw_IO.data.SC2_L ^ 1);
		CztData_.m_Fr_Word_IO.data.SC1 = (CztBtnSw_.m_Btn_Sw_IO.data.SC1_L ^ 1);
		CztData_.m_Fr_Word_IO.data.HSPEED_H = CztData_.m_bHighSpeed_H;
		CztData_.m_Fr_Word_IO.data.HSPEED_V = CztData_.m_bHighSpeed_V;

		if (0 == CztBtnSw_.m_Btn_Sw_IO.data.SB6_L1)
		{
			CztData_.m_Fr_Word_IO.data.SB6 = 1;
		}
		else if (0 == CztBtnSw_.m_Btn_Sw_IO.data.SB6_L2)
		{
			CztData_.m_Fr_Word_IO.data.SB6 = 2;
		}
		else
		{
			CztData_.m_Fr_Word_IO.data.SB6 = 0;
		}
		CztData_.m_Fr_Word_IO.data.SB5 = (CztBtnSw_.m_Btn_Sw_IO.data.SB5_L ^ 1);
		CztData_.m_Fr_Word_IO.data.SB4 = (CztBtnSw_.m_Btn_Sw_IO.data.SB4_L ^ 1);
		if (0 == CztBtnSw_.m_Btn_Sw_IO.data.SB3_L1)
		{
			CztData_.m_Fr_Word_IO.data.SB3 = 1;
		}
		else if (0 == CztBtnSw_.m_Btn_Sw_IO.data.SB3_L2)
		{
			CztData_.m_Fr_Word_IO.data.SB3 = 2;
		}
		else
		{
			CztData_.m_Fr_Word_IO.data.SB3 = 0;
		}
		CztData_.m_Fr_Word_IO.data.SB2 = (CztBtnSw_.m_Btn_Sw_IO.data.SB2_L ^ 1);
		CztData_.m_Fr_Word_IO.data.SB1 = (CztBtnSw_.m_Btn_Sw_IO.data.SB1_L ^ 1);

		// 模式判断，正常模式和写零模式，写零模式又分为在写模式和写完模式
		(HIGH == CztBtnSw_.m_Btn_Sw_IO.data.ZERO_SW) ?
			(CztData_.m_LedState = STATE_LED_NORMAL) :
			(CztData_.m_LedState = STATE_LED_WRITING);
		if ((STATE_LED_NORMAL != CztData_.m_LedState)			// 非正常模式+
				&& (1 == CztMp_Hor_.m_bIsDataRcvd)				// 进入写模式前要判断码盘数据是否准备好
				&& (1 == CztMp_Ver_.m_bIsDataRcvd)				// 准备好的条件是收到码盘数据，并解析正确
			)
		{
			// 启动写零线程
			ThreadQ[TKIN_WRITE_ZERO].SetState(READY);
			// 改变IO扫描周期
			ThreadQ[TKIN_SCAN_IO].SetPeriod(100);
		}
		else
		{
			// 正常模式停止写零线程。
			ThreadQ[TKIN_WRITE_ZERO].SetState(WAIT);
			// 改变IO扫描周期
			ThreadQ[TKIN_SCAN_IO].SetPeriod(1);
		}
		// Reset run flag.
		ThreadQ[TKIN_SCAN_IO].SetState(WAIT);
		/* And we loop. */
	}

	/* All protothread functions must end with PT_END() which takes a
		pointer to a struct pt. */
	PT_END(pt);
}
// 定时任务，周期自控可变
int Task_Led(struct pt *pt)
{
    /* A protothread function must begin with PT_BEGIN() which takes a
         pointer to a struct pt. */
	PT_BEGIN(pt);

	/* We loop forever here. */
	while(1)
	{
		/* Wait until the timer interrupt has set its flag. */
		PT_WAIT_UNTIL(pt, ThreadQ[TKIN_LED_TWINKLE].GetState());

		if (STATE_LED_NORMAL == CztData_.m_LedState)	
		{
			/* 进入正常模式 */
			digitalWrite(LED_D1, TOGGLE);
			ThreadQ[TKIN_LED_TWINKLE].SetPeriod(PERIOD_LED_NORMAL);
		}
		else 
		{
// 			/* 进入写零模式，该模式又分为在写模式和写完模式. */
// 			/* 写零模式模式判断 */
// 			CztData_.ReloadZero_H(Czt93C56_.ReadZeroCode_H());
// 			CztData_.ReloadZero_V(Czt93C56_.ReadZeroCode_V());
// 
// 			if (1 == CztData_.IsZeroValid_MPvsROM_H(CztMp_Hor_.GetCode()))
// 			{
// 				// 水平零位正确
// 				if (1 == CztData_.IsZeroValid_MPvsROM_V(CztMp_Ver_.GetCode()))
// 				{
// 					// 垂直零位正确
// 					CztData_.m_LedAndWriteState = STATE_LED_WRITE_OK;
// 				}
// 				else
// 				{
// 					// 垂直零位不正确
// 					CztData_.m_LedAndWriteState = STATE_LED_WRITING;
// 					Czt93C56_.WriteZeroCode_X(SpiCzt93C56::ENCODER_V, CztMp_Ver_.GetCode());
// 				}
// 				
// 			}
// 			else
// 			{
// 				// 水平零位不正确
// 				CztData_.m_LedAndWriteState = STATE_LED_WRITING;
// 				Czt93C56_.WriteZeroCode_X(SpiCzt93C56::ENCODER_H, CztMp_Hor_.GetCode());
// 			}
			/* 设置LED闪烁模式 */
			if (STATE_LED_WRITE_OK == CztData_.m_LedState)	// 写完模式 
			{
				// 只做定期检查，不闪灯
				digitalWrite(LED_D1, LOW);
				ThreadQ[TKIN_LED_TWINKLE].SetPeriod(PERIOD_LED_WRITE_OK);
			}
			else if (STATE_LED_WRITING == CztData_.m_LedState)	// 在写模式
			{
				// 快闪灯
				ThreadQ[TKIN_LED_TWINKLE].SetPeriod(PERIOD_LED_WRITING);
				digitalWrite(LED_D1, TOGGLE);
			}
		}
		// reset run flag.
		ThreadQ[TKIN_LED_TWINKLE].SetState(WAIT);
		/* And we loop. */
	}

      /* All protothread functions must end with PT_END() which takes a
         pointer to a struct pt. */
      PT_END(pt);
}
// 定时任务，2ms
int Task_Tx_Fr_Frame(struct pt *pt)
{
	/* A protothread function must begin with PT_BEGIN() which takes a
	pointer to a struct pt. */
	PT_BEGIN(pt);

	/* We loop forever here. */
	while (1)
	{
		/* Wait until the timer interrupt has set its flag. */
		PT_WAIT_UNTIL(pt, (ThreadQ[TKIN_TX_FR_FRAME].GetState() && (STATE_LED_NORMAL == CztData_.m_LedState)));
		// 1.Updata flexray frame data.
		FrTxData_CZT_Slot31.m_u16TxDataArry[1] = (unsigned int)CztData_.m_AimSignal_H;
		FrTxData_CZT_Slot31.m_u16TxDataArry[2] = (unsigned int)CztData_.m_AimSignal_V;
		FrTxData_CZT_Slot31.m_u16TxDataArry[3] = CztData_.m_Fr_Word_IO.All;
		FrTxData_CZT_Slot31.m_u16TxDataArry[5] &= 0x00FF;
		(0 == CztBtnSw_.m_Btn_Sw_IO.data.SB1_L) ?
			(FrTxData_CZT_Slot31.m_u16TxDataArry[5] += 0x5A00) : (FrTxData_CZT_Slot31.m_u16TxDataArry[5] += 0x0000);
		FrTxData_CZT_Slot31.m_u16TxDataArry[5] &= 0xFF00;
		(0 == CztBtnSw_.m_Btn_Sw_IO.data.SB5_L) ?
			(FrTxData_CZT_Slot31.m_u16TxDataArry[5] += 0x005A) : (FrTxData_CZT_Slot31.m_u16TxDataArry[5] += 0x0000);
		// 2.Transmit Flexray Frame.
		FrTxData_CZT_Slot31.txData();
		// reset run flag.
		ThreadQ[TKIN_TX_FR_FRAME].SetState(WAIT);
		/* And we loop. */
	}

	/* All protothread functions must end with PT_END() which takes a
	pointer to a struct pt. */
	PT_END(pt);
}
// 触发任务，受IO采集任务控制
int Task_Write_Zero(struct pt *pt)
{
	/* A protothread function must begin with PT_BEGIN() which takes a
	pointer to a struct pt. */
	PT_BEGIN(pt);

	/* We loop forever here. */
	while (1)
	{
		/* Wait until the timer interrupt has set its flag. */
		PT_WAIT_UNTIL(pt, ThreadQ[TKIN_WRITE_ZERO].GetState());// 节奏受IO采集线程控制
		/* 写零模式模式判断 */
		CztData_.ReloadZero_H(Czt93C56_.ReadZeroCode_H());
		CztData_.ReloadZero_V(Czt93C56_.ReadZeroCode_V());

		if (1 == CztData_.IsWriteZeroRight_MPvsROM_H(CztMp_Hor_.GetCode())) // 水平零位正确
		{
			if (1 == CztData_.IsWriteZeroRight_MPvsROM_V(CztMp_Ver_.GetCode())) // 垂直零位正确
			{
				CztData_.m_LedState = STATE_LED_WRITE_OK;
			}
			else   // 垂直零位不正确
			{
				CztData_.m_LedState = STATE_LED_WRITING;
				Czt93C56_.WriteZeroCode_X(SpiCzt93C56::ENCODER_V, CztMp_Ver_.GetCode());
			}
		}
		else   // 水平零位不正确
		{
			CztData_.m_LedState = STATE_LED_WRITING;
			Czt93C56_.WriteZeroCode_X(SpiCzt93C56::ENCODER_H, CztMp_Hor_.GetCode());
		}
		// reset run flag.
		ThreadQ[TKIN_WRITE_ZERO].SetState(WAIT);
		/* And we loop. */
	}
	/* All protothread functions must end with PT_END() which takes a
	pointer to a struct pt. */
	PT_END(pt);
}
// 定时任务
int Task_Fr_Guarantee(struct pt *pt)
{
	/* A protothread function must begin with PT_BEGIN() which takes a
	pointer to a struct pt. */
	PT_BEGIN(pt);

	/* We loop forever here. */
	while (1)
	{
		/* Wait until the timer interrupt has set its flag. */
		PT_WAIT_UNTIL(pt, ThreadQ[TKIN_FR_GUARANTEE].GetState());

		if (FR_RET != 0)
        {
           digitalWrite(FR_RST, LOW);   //输出低电平
		   ASLEEP == ThreadQ[TKIN_FR_GUARANTEE].SLEEP_MS(1);
           PT_WAIT_UNTIL(pt, ThreadQ[TKIN_FR_GUARANTEE].IsAwake_MS());
           digitalWrite(FR_RST, HIGH);     //输出高电平
		   ASLEEP == ThreadQ[TKIN_FR_GUARANTEE].SLEEP_MS(1);
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
//          for (int i = 0; i < 1000; i++) { ; }
            FR_RET = vfnFlexRay_Init();

            Fr_CC_reg_ptr[FrPOCR] = ((Fr_CC_reg_ptr[FrPOCR] & 0xFFF0) | 0x0004);
//			for (int i = 0; i < 100; ++i) { ; }
            ThreadQ[TKIN_FR_GUARANTEE].SLEEP_MS(1);
            PT_WAIT_UNTIL(pt, ThreadQ[TKIN_FR_GUARANTEE].IsAwake_MS());
            FR_RET = vfnFlexRay_Init();
			CztData_.m_Fr_Offline = STATE_FR_OFFLINE;
		}
		else
		{
			CztData_.m_Fr_Offline = STATE_LAMP_NONE;
		}

        // reset run flag.
		ThreadQ[TKIN_FR_GUARANTEE].SetState(WAIT);
        /* And we loop. */
	}

	/* All protothread functions must end with PT_END() which takes a
	pointer to a struct pt. */
	PT_END(pt);
}
// 定时任务，周期自控可变
int Task_Lamp_1(struct pt *pt)
{
	/* A protothread function must begin with PT_BEGIN() which takes a
	pointer to a struct pt. */
	PT_BEGIN(pt);

	/* We loop forever here. */
	while (1)
	{
		/* Wait until the timer interrupt has set its flag. */
		PT_WAIT_UNTIL(pt, ThreadQ[TKIN_LAMP_H].GetState());

		do 
		{
		    if (STATE_FR_OFFLINE == CztData_.m_Fr_Offline)
            {
                digitalWrite(LAMP1, TOGGLE);
                ThreadQ[TKIN_LAMP_H].SLEEP_MS(150);
                PT_WAIT_UNTIL(pt, ThreadQ[TKIN_LAMP_H].IsAwake_MS());
				CztData_.m_Fr_Offline = STATE_LAMP_NONE;
            }
			if (STATE_LAMP_NORMAL_H == CztData_.m_LampState_H)
			{
				// set Lamp state
				ThreadQ[TKIN_LAMP_H].SetPeriod(1);
				digitalWrite(LAMP1, CztBtnSw_.m_Btn_Sw_IO.data.SA1_L);
				break;
			}
			if (STATE_LAMP_ZERO_INIT_H == CztData_.m_LampState_H)
			{
				ThreadQ[TKIN_LAMP_H].SetPeriod(500);
				digitalWrite(LAMP1, TOGGLE);
				break;
			}
			if (STATE_LAMP_ZERO_ERROR_H == CztData_.m_LampState_H)
			{
				ThreadQ[TKIN_LAMP_H].SetPeriod(300);
				digitalWrite(LAMP1, TOGGLE);
				break;
			}

		} while (0);
		// reset run flag.
		ThreadQ[TKIN_LAMP_H].SetState(WAIT);
		/* And we loop. */
	}

	/* All protothread functions must end with PT_END() which takes a
	pointer to a struct pt. */
	PT_END(pt);
}
int Task_Lamp_2(struct pt *pt)
{
	/* A protothread function must begin with PT_BEGIN() which takes a
	pointer to a struct pt. */
	PT_BEGIN(pt);

	/* We loop forever here. */
	while (1)
	{
		/* Wait until the timer interrupt has set its flag. */
		PT_WAIT_UNTIL(pt, ThreadQ[TKIN_LAMP_V].GetState());

		do
		{
			if (STATE_LAMP_NORMAL_V == CztData_.m_LampState_V)
			{
				ThreadQ[TKIN_LAMP_V].SetPeriod(1);
				digitalWrite(LAMP2, CztBtnSw_.m_Btn_Sw_IO.data.SA2_L);
				break;
			}
			if (STATE_LAMP_ZERO_INIT_V == CztData_.m_LampState_V)
			{
				ThreadQ[TKIN_LAMP_V].SetPeriod(500);
				digitalWrite(LAMP2, TOGGLE);
			}
			if (STATE_LAMP_ZERO_ERROR_V == CztData_.m_LampState_V)
			{
				ThreadQ[TKIN_LAMP_V].SetPeriod(300);
				digitalWrite(LAMP2, TOGGLE);
				break;
			}
		} while (0);

		// reset run flag.
		ThreadQ[TKIN_LAMP_V].SetState(WAIT);
		/* And we loop. */
	}

	/* All protothread functions must end with PT_END() which takes a
	pointer to a struct pt. */
	PT_END(pt);
}
/************************************************************************/

void attachInterrupts()
{
    // ISR functions found within this file.
    EALLOW;  // This is needed to write to EALLOW protected registers
//    PieVectTable.TINT0 = &cpu_timer0_isr;
	PieVectTable.XINT13 = &cpu_timer1_isr;
	PieVectTable.TINT2 = &cpu_timer2_isr;

    PieVectTable.SCIRXINTA = &sciaRxFifoIsr;
//     PieVectTable.SCITXINTA = &sciaTxFifoIsr;
    PieVectTable.SCIRXINTB = &scibRxFifoIsr;
//     PieVectTable.SCITXINTB = &scibTxFifoIsr;
//     PieVectTable.SCIRXINTC = &scicRxFifoIsr;
//     PieVectTable.SCITXINTC = &scicTxFifoIsr;

    EDIS;   // This is needed to disable write to EALLOW protected registers
}

void initData(void)
{
    FifoBuffer_Sci_A.Initialize();
    FifoBuffer_Sci_B.Initialize();

	// Create threads
	for (int i = 0; i < SCHED_MAX; i++)
	{
		ThreadQ[i].CreateTask(TaskArray[i], PERIOD_MS_Q[i], WAIT, NULL);
	}

	CztBtnSw_.Init(IO_SampleWindow_ms  / PERIOD_TIMER2_MS);

	Czt93C56_.InitEEPROM();

	CztData_.m_PowerOnInitState_H = CztData_.m_PowerOnInitState_V = 1;
}

void initInterrupts(void)
{
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
    PieCtrlRegs.PIEIER9.bit.INTx1=1;     // PIE Group 9, INT1. SCIRXINTA
//    PieCtrlRegs.PIEIER9.bit.INTx2=1;     // PIE Group 9, INT2. SCITXINTA
    PieCtrlRegs.PIEIER9.bit.INTx3=1;     // PIE Group 9, INT3. SCIRXINTB
//    PieCtrlRegs.PIEIER9.bit.INTx4=1;     // PIE Group 9, INT4. SCITXINTB
//    PieCtrlRegs.PIEIER8.bit.INTx5=1;     // PIE Group 8, INT5. SCIRXINTC
//    PieCtrlRegs.PIEIER8.bit.INTx6=1;     // PIE Group 8, INT6. SCITXINTC
//    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;  // PIE Group 1, INT7. TINT0. Timer1 and Timer2 no PieGroup

//    PieCtrlRegs.PIEIER9.bit.INTx5 = 1;  //CAN0-A
//    PieCtrlRegs.PIEIER9.bit.INTx7 = 1;  //CAN0-B
//    PieCtrlRegs.PIEIER9.bit.INTx6 = 1;  //CAN1-A
//    PieCtrlRegs.PIEIER9.bit.INTx8 = 1;  //CAN1-B

    IER |= M_INT8;   // Enable CPU INT8 which is connected to SCI-C:
    IER |= M_INT9;   // Enable CPU INT8 which is connected to SCI-A and SCI-B

//    IER |= M_INT1;   // Enable CPU int1 which is connected to CPU-Timer 0
//    IER |= M_INT9;  // Enable CPU int9 which is connected to eCAN0/1
    IER |= M_INT13;   // Enable CPU int13 which is connected to CPU-Timer 1. Timer1 and Timer2 no PieGroup
    IER |= M_INT14;   // Enable CPU int14 which is connected to CPU-Timer 2. Timer1 and Timer2 no PieGroup
}

void reset_FlexRay()
{
    int i, j;//20170918

    //  GpioDataRegs.GPASET.bit.GPIO12 = 1;     //输出高电平
    GpioDataRegs.GPACLEAR.bit.GPIO12 = 1;   //输出低电平
    for (j = 0; j < 20; j++)
    {
        for (i = 0; i < 30000; i++) ;//延时  20170918
    }
    GpioDataRegs.GPASET.bit.GPIO12 = 1;     //输出高电平
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

void InitGpio_Used()
{
    InitGpio_Default();
	InitGpio_IO();
    InitGpio_SCI_A(29, 36);
    InitGpio_SCI_B(9, 11);

    InitGpio_FrBus();
}

void InitGpio_IO(void)
{
    pinMode(PWREN, OUTPUT);
	pinMode(ZERO, INPUT);
	pinMode(SA1_L, INPUT);
	pinMode(SA2_L, INPUT);
	pinMode(SA2_H, INPUT);
	for (int i = 16; i < 28; i++) { pinMode(i, INPUT); }
	pinMode(DSIN1, INPUT);
	pinMode(DSIN2, INPUT);
	for (int i = 48; i < 54; i++) { pinMode(i, INPUT); }
	pinMode(LED_D1, OUTPUT);
	pinMode(LAMP1, OUTPUT);
	pinMode(LAMP2, OUTPUT);
	pinMode(DOG, OUTPUT);
	pinMode(FR_RST, OUTPUT);
	pinMode(FR_INT, INPUT);
	// test
//	pinMode(DSIN1, OUTPUT);
}

//===========================================================================
// No more.
//===========================================================================

