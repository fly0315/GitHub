/*
 * GpioConfig.cpp
 *
 *  Created on: 2018年11月05日
 *      Author: LiuBin
 */

#include "SCC1300-D02_SPI.h"
#include <assert.h>

 // SCC1300 version specific parameters, here SCC1300-D02
#define GYRO_SENSITIVITY 50 // LSB/DPS
#define ACC_SENSITIVITY 1800 // LSB/g
#define READ 0
#define WRITE 1
 // Accelerometer Registers
#define ACC_REVID 0x00
#define ACC_CTRL 0x01
#define ACC_STATUS 0x02
#define ACC_RESET 0x03
#define ACC_X_LSB 0x04
#define ACC_X_MSB 0x05
#define ACC_Y_LSB 0x06
#define ACC_Y_MSB 0x07
#define ACC_Z_LSB 0x08
#define ACC_Z_MSB 0x09
#define ACC_TEMP_LSB 0x12
#define ACC_TEMP_MSB 0x13
#define ACC_INT_STATUS 0x16
#define ACC_ID 0x27
 // Gyroscope Registers
#define GYRO_RATE_X 0x00
#define GYRO_IC_ID 0x07
#define GYRO_STATUS 0x08
#define GYRO_TEMP 0x0A
 // Gyroscope status and control bits
#define BIT_LOOPF_OK (1 << 2) // Loopfilter status flag
#define BIT_SOFTRESET_GYRO (1 << 1) // Gyro soft reset
 // SSP Status register
#define SSPSR_TFE (1 << 0)
#define SSPSR_TNF (1 << 1)
#define SSPSR_RNE (1 << 2)
#define SSPSR_RFF (1 << 3)
#define SSPSR_BSY (1 << 4)
 // GPIO ports
#define PORT0 0
#define PORT1 1
#define PORT2 2
#define PORT3 3
 // SPI read and write buffer size
#define FIFOSIZE 8
 // SSP CR0 register
#define SSPCR0_DSS (1 << 0)
#define SSPCR0_FRF (1 << 4)
#define SSPCR0_SPO (1 << 6)
#define SSPCR0_SPH (1 << 7)
#define SSPCR0_SCR (1 << 8)
 // SSP CR1 register
#define SSPCR1_LBM (1 << 0)
#define SSPCR1_SSE (1 << 1)
#define SSPCR1_MS (1 << 2)
#define SSPCR1_SOD (1 << 3)
 // SSP Interrupt Mask Set/Clear register
#define SSPIMSC_RORIM (1 << 0)
#define SSPIMSC_RTIM (1 << 1)
#define SSPIMSC_RXIM (1 << 2)
#define SSPIMSC_TXIM (1 << 3)
 // Pin definitions
#define PIN_CSB_GYRO (1 << 2)
#define PIN_CSB_ACC (1 << 0)
#define PIN_EXTRESN_GYRO (1 << 1)
static unsigned int ParityEven_16b(uint16_t v)
{
	static const bool ParityTableEven_8b[256] =
	{
#   define P2(n) n^1, n, n, n^1
#   define P4(n) P2(n), P2(n^1), P2(n^1), P2(n)
#   define P6(n) P4(n), P4(n^1), P4(n^1), P4(n)
		P6(0), P6(1), P6(1), P6(0)
	};
	unsigned char * p = (unsigned char *)&v;
	return ParityTableEven_8b[p[0] ^ p[1]];

}
static union GYRO_MISO_DATA
{
	uint16_t all;
	struct  
	{
		unsigned int Par_Odd	: 1;	// 0
		unsigned int S_Ok		: 1;	// 1
		unsigned int rsv		: 14;	// 2~15
	}bits;
}GD_Ratex, GD_ST, GD_Temp, GD_IC_ID;
static unsigned int GRDParityCheck(uint16_t data)
{
	return (ParityEven_16b(data & (~1)) == (data & 1))
}
static typedef union GYRO_MOSI_OPC
{
	uint16_t all;
	struct 
	{
		unsigned int ParOdd : 1;
		unsigned int fixed0 : 1;
		unsigned int RW		: 1;
		unsigned int addr	: 13;
	}bits;
}GOR_RateX, GOR_ST, GOR_Temp, GOR_IC_ID, GOW_Reset;
static uint16_t GenGyroOpc(unsigned int Addr, unsigned int RW)
{
	GYRO_MOSI_OPC OPC;
	OPC.all = 0;
	OPC.bits.addr = Addr;
	OPC.bits.fixed0 = 0;
	OPC.bits.RW = RW;
	OPC.bits.ParOdd = ParityEven_16b(OPC.all);
	return OPC.all;
}

unsigned int SCC1300_Init
(
	PIN CSB_G, PIN SCK_G, PIN MOSI_G, PIN MISO_G,
	PIN CSB_A, PIN SCK_A, PIN MOSI_A, PIN MISO_A,
	unsigned int Baud_G, unsigned int Baud_A
)
{
	GOR_RateX.all = GenGyroOpc(GYRO_RATE_X, READ);
	GOR_ST.all = GenGyroOpc(GYRO_STATUS, READ);
	GOR_Temp.all = GenGyroOpc(GYRO_TEMP, READ);
	GOR_IC_ID.all = GenGyroOpc(GYRO_IC_ID, READ);
	GOW_Reset.all = GenGyroOpc(GYRO_IC_ID, WRITE);
	InitGpio_Spi_A();
	spi_init();
	spi_fifo_init();

}

void InitGpio_Spi_A()
{
	EALLOW;

	//
	// Enable internal pull-up for the selected pins
	// Pull-ups can be enabled or disabled by the user.
	// This will enable the pullups for the specified pins.
	// Comment out other unwanted lines.
	GpioCtrlRegs.GPBPUD.bit.GPIO54 = 0; //Enable pull-up on GPIO54 (SPISIMOA)
	GpioCtrlRegs.GPBPUD.bit.GPIO55 = 0; //Enable pull-up on GPIO55 (SPISOMIA)
	GpioCtrlRegs.GPBPUD.bit.GPIO56 = 0; //Enable pull-up on GPIO56 (SPICLKA)
	GpioCtrlRegs.GPBPUD.bit.GPIO57 = 0; //Enable pull-up on GPIO57 (SPISTEA)

//
// Set qualification for selected pins to asynch only
// This will select asynch (no qualification) for the selected pins.
// Comment out other unwanted lines.
	GpioCtrlRegs.GPBQSEL2.bit.GPIO54 = 3; // Asynch input GPIO54 (SPISIMOA)
	GpioCtrlRegs.GPBQSEL2.bit.GPIO55 = 3; // Asynch input GPIO55 (SPISOMIA)
	GpioCtrlRegs.GPBQSEL2.bit.GPIO56 = 3; // Asynch input GPIO56 (SPICLKA)
	GpioCtrlRegs.GPBQSEL2.bit.GPIO57 = 0; // Asynch input GPIO57 (SPISTEA GPIO MODE)

	//
	// Configure SPI-A pins using GPIO regs
	// This specifies which of the possible GPIO pins will be SPI
	// functional pins.
	// Comment out other unwanted lines.
	GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 1; // Configure GPIO54 as SPISIMOA
	GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 1; // Configure GPIO55 as SPISOMIA
	GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 1; // Configure GPIO56 as SPICLKA
	GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0; // Configure GPIO57 as GPIO (CS)

	GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1;  // Set as output
	GpioDataRegs.GPBDAT.bit.GPIO57 = 1;  // Set value 1

	EDIS;
}

void spi_init()
{
	//	SpiaRegs.SPICCR.all = 0x000F;	             // Reset on, rising edge, 16-bit char bits
	SpiaRegs.SPICCR.bit.SPISWRESET = 0;				// reset on
	SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;			// rising edge output
	SpiaRegs.SPICCR.bit.SPILBK = 0;					// loop back disable
	SpiaRegs.SPICCR.bit.SPICHAR = 0x0F;				// 16-bit

	//	SpiaRegs.SPICTL.all = 0x000E;				// Enable master mode, delay phase, // enable talk, and SPI int disabled.		
	SpiaRegs.SPICTL.bit.OVERRUNINTENA = 0;			// disable overrun INT
	SpiaRegs.SPICTL.bit.CLK_PHASE = 0;				// no delay phase
	SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;			// master mode
	SpiaRegs.SPICTL.bit.TALK = 1;					// enable talk
	SpiaRegs.SPICTL.bit.SPIINTENA = 0;				// disable spi interrupt		

	SpiaRegs.SPISTS.all = 0x0000;
	SpiaRegs.SPIBRR = 9;							// SPICLK=LSPCLK/(BRR+1)  @LSPCLK = 37.5MHz
	SpiaRegs.SPICCR.all = 0x008F;					// Relinquish SPI from Reset
	SpiaRegs.SPIPRI.bit.FREE = 1;					// Set so breakpoints don't disturb xmission

}

void spi_fifo_init()
{
	// Initialize SPI FIFO registers
	SpiaRegs.SPIFFTX.all = 0x6040;              //  发送FIFO使能，中断禁止
	SpiaRegs.SPIFFTX.all = 0xE040;
	SpiaRegs.SPIFFRX.all = 0x204f;
	SpiaRegs.SPIFFCT.all = 0x0;
}

unsigned int GYRO_ReadRegister(uint16_t Address, uint16_t *Data)
{
	uint16_t Status;
	uint16_t ui16Address = Address;

	// Build address transfer frame
	ui16Address <<= 3; // Address to be shifted left by 3
					   // (and RW = 0)
	ui16Address += CalcParity(ui16Address); // Add parity bit
	LPC_GPIO0->DATA &= ~PIN_CSB_GYRO; // Select gyro sensor
	Status = LPC_SSP0->DR; // Read RX buffer just to clear
						   // interrupt flag
	while ((LPC_SSP0->SR & (SSPSR_TNF | SSPSR_BSY)) != SSPSR_TNF);	// Move on only if NOT busy and TX FIFO
																	// not full
	LPC_SSP0->DR = ui16Address >> 8; // Write address MSB to TX buffer
	while (LPC_SSP0->SR & SSPSR_BSY); // Wait until the Busy bit is cleared
	Status = LPC_SSP0->DR; // Read RX buffer (status byte MSB)
	Status <<= 8;
	LPC_SSP0->DR = ui16Address & 0x00FF; // Write address LSB to TX buffer
	while (LPC_SSP0->SR & SSPSR_BSY); // Wait until the Busy bit is cleared
	Status |= LPC_SSP0->DR; // Read RX buffer (status byte LSB)
	LPC_SSP0->DR = 0; // Write dummy data to TX buffer
	while (LPC_SSP0->SR & SSPSR_BSY); // Wait until the Busy bit is cleared
	*Data = LPC_SSP0->DR; // Read RX buffer (data byte MSB)
	*Data <<= 8;
	LPC_SSP0->DR = 0x01; // Write dummy data to TX buffer; NOTE:
						 // even if the data is dummy it still
						 // needs to have correct parity so
						 // force odd parity
						 // (TX 0x0000 -> 0x0001)
	while (LPC_SSP0->SR & SSPSR_BSY); // Wait until the Busy bit is cleared
	*Data |= LPC_SSP0->DR; // Read RX buffer (data byte LSB)
	LPC_GPIO0->DATA |= PIN_CSB_GYRO; // Deselect gyro sensor
	return Status;
}


void GyroPowerup()
{
	// Wait 800 ms
	// always delay in application.
// 	DELAY_US(800000);
	// Read status register twice to clear error flags
	GYRO_ReadRegister(GYRO_STATUS, &Status_Gyro.all);
	GYRO_ReadRegister(GYRO_STATUS, &Status_Gyro.all);
	// If gyro is ok start reading the sensors
	while (!(Status_Gyro.bits.S_Ok))
	{
		// Gyro error
		GYRO_ReadRegister(GYRO_STATUS, &Status_Gyro.all); // Read status register to clear error flags
		DELAY_US(60000);
		GYRO_ReadRegister(GYRO_STATUS, &Status_Gyro.all); // Read status register to clear flags again
		if (!(Status_Gyro.bits.Loopf_Ok))
		{
			// If LOOPF still fails, reset gyroscope
			Status_Gyro = GYRO_WriteRegister(GYRO_IC_ID, BIT_SOFTRESET_GYRO); // Perform soft reset
			DELAY_US(800000);
			// Read status register twice to clear error flags
			GYRO_ReadRegister(GYRO_STATUS, &Status_Gyro);
			GYRO_ReadRegister(GYRO_STATUS, &Status_Gyro);
		}

	}
}