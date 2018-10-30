/*
 * AT93C56.cpp
 *
 *  Created on: 2018年2月7日
 *      Author: LiuBin
 */

#include <AT93C56.h>



AT93C56_SPI::AT93C56_SPI
(
	volatile unsigned int* pCS,
	volatile unsigned int* pSK,
	volatile unsigned int* pDI,
	volatile unsigned int* pDO
) :m_pCS(pCS), m_pSK(pSK), m_pDI(pDI), m_pDO(pDO)
{
	
}

AT93C56_SPI::~AT93C56_SPI() {
    // TODO Auto-generated destructor stub
}
void AT93C56_SPI::Init()
{
	InitGpio_Spi_A();	
	spi_init();
	spi_fifo_init();
}
// Erase/Write Enable
void AT93C56_SPI::EWEN(void)
{
	WriteOPCodeAddr(0x9800, 0, 0);
	TCS();
}
// Erase/Write Disable
void AT93C56_SPI::EWDS(void)
{
	WriteOPCodeAddr(0x8000, 0, 0);
	TCS();
}
// Erase
void AT93C56_SPI::ERASE(unsigned int Addr)
{
	WriteOPCodeAddr(0x7000, Addr, 4);
	TCS();
}
// Erase All
void AT93C56_SPI::ERAL(void)
{
	WriteOPCodeAddr(0x9000, 0, 0);
	TCS();
}
// Write All
void AT93C56_SPI::WRAL(void)
{
	WriteOPCodeAddr(0x8800, 0, 0);
	TCS();
}
// Write
void AT93C56_SPI::WRITE(unsigned int txData, unsigned int Addr)
{
	WriteOPCodeAddr(0x5000, Addr, 4);
	SpiaRegs.SPICCR.bit.SPICHAR = 15;
	SpiaRegs.SPITXBUF = txData;
	TCS();

}

// Read
unsigned int AT93C56_SPI::READ(unsigned int Addr)
{

    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 0;   // 0,复位接收FIFO指针为0，且一直处于复位
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 1;   // 1,再次使能接收FIFO工作

	WriteOPCodeAddr(0xC000, Addr, 5);
	SpiaRegs.SPICCR.bit.SPICHAR = 15;
	SpiaRegs.SPITXBUF = 0;
	DELAY_US(20);
	CSEnable(DISABLE);
	return SpiaRegs.SPIRXBUF;
}

void AT93C56_SPI::WriteOPCodeAddr(unsigned int OPCode, unsigned int addr, unsigned int offset)
{
	unsigned int tx = 0;
	CSEnable(ENABLE);						
	DELAY_US(5);
	tx = ((addr & 0xFF) << offset) | OPCode;		// 发送数据左对齐，tx是16位数据，16位地址使用A7~A0八位数据，加1位起始位sp=1，加操作码2位，一共11位，低5位置空

	SpiaRegs.SPICCR.bit.SPICHAR = 11;		// 根据芯片手册和实际波形凑出来的。28335MOSI信号在CLK之前就已经准备好了，前一位对于93C56来说是无用的
	SpiaRegs.SPITXBUF = tx;
	DELAY_US(15);                           // 根据实测波形确定
	tx = SpiaRegs.SPIRXBUF;
}

// Generate TCS
void AT93C56_SPI::TCS()
{
	DELAY_US(20);
	CSEnable(DISABLE);
	DELAY_US(1);
	CSEnable(ENABLE);
	DELAY_US(1);
	CSEnable(DISABLE);
	DELAY_US(9000);
}

void AT93C56_SPI::spi_init()
{
	//	SpiaRegs.SPICCR.all = 0x000F;	             // Reset on, rising edge, 16-bit char bits
	SpiaRegs.SPICCR.bit.SPISWRESET = 0;
	SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;		 // rising edge output
	SpiaRegs.SPICCR.bit.SPILBK = 0;
	SpiaRegs.SPICCR.bit.SPICHAR = 15;

	//	SpiaRegs.SPICTL.all = 0x000E;    		     // Enable master mode, delay phase, // enable talk, and SPI int disabled.		
	SpiaRegs.SPICTL.bit.OVERRUNINTENA = 0;		 // disable overrun INT
	SpiaRegs.SPICTL.bit.CLK_PHASE = 1;			 // Delay phase
	SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;		 // master mode
	SpiaRegs.SPICTL.bit.TALK = 1;
	SpiaRegs.SPICTL.bit.SPIINTENA = 0;			 // disable spi interrupt		

	SpiaRegs.SPISTS.all = 0x0000;
	SpiaRegs.SPIBRR = 37;						 // SPICLK=LSPCLK/(BRR+1)  @LSPCLK = 37.5MHz
	SpiaRegs.SPICCR.all = 0x008F;		         // Relinquish SPI from Reset
	SpiaRegs.SPIPRI.bit.FREE = 1;                // Set so breakpoints don't disturb xmission
}

void AT93C56_SPI::spi_fifo_init()
{
	// Initialize SPI FIFO registers
	SpiaRegs.SPIFFTX.all = 0x6040;              //  发送FIFO使能，中断禁止
	SpiaRegs.SPIFFTX.all = 0xE040;
	SpiaRegs.SPIFFRX.all = 0x204f;
	SpiaRegs.SPIFFCT.all = 0x0;
}

void AT93C56_SPI::InitGpio_Spi_A()
{
	EALLOW;

	//
	// Enable internal pull-up for the selected pins
	// Pull-ups can be enabled or disabled by the user.
	// This will enable the pullups for the specified pins.
	// Comment out other unwanted lines.
	//
//    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;   //Enable pull-up on GPIO16 (SPISIMOA)
//    GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;   //Enable pull-up on GPIO17 (SPISOMIA)
//    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;   //Enable pull-up on GPIO18 (SPICLKA)
//    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;   //Enable pull-up on GPIO19 (SPISTEA)

	GpioCtrlRegs.GPBPUD.bit.GPIO54 = 0; //Enable pull-up on GPIO54 (SPISIMOA)
	GpioCtrlRegs.GPBPUD.bit.GPIO55 = 0; //Enable pull-up on GPIO55 (SPISOMIA)
	GpioCtrlRegs.GPBPUD.bit.GPIO56 = 0; //Enable pull-up on GPIO56 (SPICLKA)
	GpioCtrlRegs.GPBPUD.bit.GPIO57 = 0; //Enable pull-up on GPIO57 (SPISTEA)

	//
	// Set qualification for selected pins to asynch only
	// This will select asynch (no qualification) for the selected pins.
	// Comment out other unwanted lines.
	//
//    GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3; // Asynch input GPIO16 (SPISIMOA)
//    GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3; // Asynch input GPIO17 (SPISOMIA)
//    GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3; // Asynch input GPIO18 (SPICLKA)
//    GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3; // Asynch input GPIO19 (SPISTEA)

	GpioCtrlRegs.GPBQSEL2.bit.GPIO54 = 3; // Asynch input GPIO54 (SPISIMOA)
	GpioCtrlRegs.GPBQSEL2.bit.GPIO55 = 3; // Asynch input GPIO55 (SPISOMIA)
	GpioCtrlRegs.GPBQSEL2.bit.GPIO56 = 3; // Asynch input GPIO56 (SPICLKA)
	GpioCtrlRegs.GPBQSEL2.bit.GPIO57 = 0; // Asynch input GPIO57 (SPISTEA)

	//
	// Configure SPI-A pins using GPIO regs
	// This specifies which of the possible GPIO pins will be SPI
	// functional pins.
	// Comment out other unwanted lines.
	//
//    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1; // Configure GPIO16 as SPISIMOA
//    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1; // Configure GPIO17 as SPISOMIA
//    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1; // Configure GPIO18 as SPICLKA
//    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 1; // Configure GPIO19 as SPISTEA

	GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 1; // Configure GPIO54 as SPISIMOA
	GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 1; // Configure GPIO55 as SPISOMIA
	GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 1; // Configure GPIO56 as SPICLKA
	GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0; // Configure GPIO57 as GPIO (CS)

	GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1;  // Set as output
	GpioDataRegs.GPBDAT.bit.GPIO57 = 0;  // Set value 0

	EDIS;
}
