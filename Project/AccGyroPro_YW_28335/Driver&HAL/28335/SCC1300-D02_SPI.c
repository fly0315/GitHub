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
union GYRO_REG_RATEX
{
	uint16_t all;
	struct 
	{
		unsigned int oddParity : 1;
		unsigned int S_OK : 1;
		unsigned int RateX14 : 14;
	}bits;
};
#define GYRO_IC_ID 0x07
#define GYRO_STATUS 0x08
typedef union
{
	uint16_t all;
	struct 
	{
		unsigned int oddParity : 1;
		unsigned int Reserved : 8;
		unsigned int Parity_OK : 1;
		unsigned int Reserved01 : 6;
	}bits;
}GYRO_REG_STATUS;
#define GYRO_TEMP 0x0A
#define GYRO_DUMMY 0x00
 
typedef union
{
	uint16_t all;
	struct
	{
		unsigned int ParOdd : 1;
		unsigned int fixed0 : 1;
		unsigned int RW : 1;
		unsigned int addr : 13;
	}bits;
}GYRO_MOSI_OPC;
typedef union
{
	uint16_t all;
	struct
	{
		unsigned int Par_Odd : 1;	// 0
		unsigned int S_Ok : 1;	// 1
		unsigned int D14 : 14;	// 2~15
	}bits;
}GYRO_MISO_DATA;
typedef struct
{
	GYRO_MOSI_OPC opc;
	GYRO_MOSI_OPC data;
}GYRO_MOSI;
typedef struct
{
	GYRO_MISO_DATA status;
	GYRO_MISO_DATA data;
}GYRO_MISO;
typedef struct 
{
	GYRO_MOSI_OPC rateX_OPC;
	GYRO_MOSI_OPC Temp_OPC;
	GYRO_MOSI_OPC DUMMY;
}GYRO_MOSI_MIX;
typedef struct
{
	GYRO_MISO_DATA status;
	GYRO_MISO_DATA rateX;
	GYRO_MISO_DATA temperature;
}GYRO_MISO_MIX;

typedef union
{
    uint16_t data;
    struct
    {
        unsigned L: 8;
        unsigned H: 8;
    }bit;
}W2C;
static unsigned int ParityEven_16b(uint16_t v)
{
	static const unsigned int ParityTableEven_8b[256] =
	{
#   define P2(n) n^1, n, n, n^1
#   define P4(n) P2(n), P2(n^1), P2(n^1), P2(n)
#   define P6(n) P4(n), P4(n^1), P4(n^1), P4(n)
		P6(0), P6(1), P6(1), P6(0)
	};
//	unsigned char* p = (unsigned char *)&v;
	W2C *p = (W2C*)&v;
	return ParityTableEven_8b[p->bit.L ^ p->bit.H];

}

static unsigned int ParityCheck_u16(uint16_t data)
{
	return (ParityEven_16b(data & (~1)) == (data & 1));
}

static GYRO_MOSI Gyro_RateX, Gyro_Status, Gyro_Temp, Gyro_IC_ID, Gyro_Reset;
static GYRO_MISO GD_RateX, GD_Status, GD_Temp, GD_IC_ID, GD_Reset;
static GYRO_MOSI_MIX Gyro_mix;
static GYRO_MISO_MIX GD_mix;
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
static void InitData();
static void InitSpi_Gyro();
static void InitSpi_Acc();
static void InitGpio_Spi_Gyro();
static void InitGpio_Spi_Acc();
static void spi_init();
static void spi_fifo_init();
static void spi_mcbspB_Acc();
__interrupt void spiTxFifoIsr(void);
__interrupt void spiRxFifoIsr(void);

unsigned int SCC1300_Init
(
// 	PIN CSB_G, PIN SCK_G, PIN MOSI_G, PIN MISO_G,
// 	PIN CSB_A, PIN SCK_A, PIN MOSI_A, PIN MISO_A,
// 	unsigned int Baud_G, unsigned int Baud_A
)
{
	InitData();

	InitSpi_Gyro();

	/*

	// Disable and clear all CPU interrupts
	DINT;
	IER = 0x0000;
	IFR = 0x0000;

	EALLOW;	// This is needed to write to EALLOW protected registers
	PieVectTable.SPIRXINTA = &spiRxFifoIsr;
	PieVectTable.SPITXINTA = &spiTxFifoIsr;
	EDIS;   // This is needed to disable write to EALLOW protected registers

	//
	// Enable interrupts required for this example
	//
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
	PieCtrlRegs.PIEIER6.bit.INTx1 = 1;     // Enable PIE Group 6, INT 1
	PieCtrlRegs.PIEIER6.bit.INTx2 = 1;     // Enable PIE Group 6, INT 2
	IER |= (M_INT6);					 // Enable CPU INT6
	EINT;                                // Enable Global Interrupts
	*/
	return 1;

}

void InitData()
{
	Gyro_RateX.opc.all = GenGyroOpc(GYRO_RATE_X, READ);
	Gyro_RateX.data.all = GenGyroOpc(GYRO_DUMMY, READ);

	Gyro_Status.opc.all = GenGyroOpc(GYRO_STATUS, READ);
	Gyro_Status.data.all = GenGyroOpc(GYRO_DUMMY, READ);

	Gyro_Temp.opc.all = GenGyroOpc(GYRO_TEMP, READ);
	Gyro_Temp.data.all = GenGyroOpc(GYRO_DUMMY, READ);

	Gyro_IC_ID.opc.all = GenGyroOpc(GYRO_IC_ID, READ);
	Gyro_IC_ID.data.all = GenGyroOpc(GYRO_DUMMY, READ);

	Gyro_Reset.opc.all = GenGyroOpc(GYRO_IC_ID, WRITE);
	Gyro_Reset.data.all = 0x04;

	Gyro_mix.rateX_OPC.all = GenGyroOpc(GYRO_RATE_X, READ);
	Gyro_mix.Temp_OPC.all = GenGyroOpc(GYRO_TEMP, READ);
	Gyro_mix.DUMMY.all = GenGyroOpc(GYRO_DUMMY, READ);
}

void InitSpi_Gyro()
{
	InitGpio_Spi_Gyro();

	spi_fifo_init();
	spi_init();
}

void InitSpi_Acc()
{
	InitGpio_Spi_Acc();
}

void InitGpio_Spi_Gyro()
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
//	GpioCtrlRegs.GPBQSEL2.bit.GPIO57 = 0; // Asynch input GPIO57 (SPISTEA GPIO MODE)
	GpioCtrlRegs.GPBQSEL2.bit.GPIO57 = 3;
	//
	// Configure SPI-A pins using GPIO regs
	// This specifies which of the possible GPIO pins will be SPI
	// functional pins.
	// Comment out other unwanted lines.
	GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 1; // Configure GPIO54 as SPISIMOA
	GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 1; // Configure GPIO55 as SPISOMIA
	GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 1; // Configure GPIO56 as SPICLKA
//	GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0; // Configure GPIO57 as GPIO (CS)
	GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 1;

//	GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1;  // Set as output
//	GpioDataRegs.GPBDAT.bit.GPIO57 = 1;  // Set value 1


	EDIS;
}

void spi_init()
{
	SpiaRegs.SPICCR.bit.SPISWRESET = 0;				// bit7. Reset on, run into initial mode
	SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;			// bit6. 0/1: rising/falling edge output
	SpiaRegs.SPICCR.bit.SPILBK = 0;					// bit4. Loop back	0:1: disable/enable
	SpiaRegs.SPICCR.bit.SPICHAR = 0x0F;				// bit3~0. 16-bit

	SpiaRegs.SPICTL.bit.OVERRUNINTENA = 0;			// bit4. Overrun INT	0/1: disable/enable
	SpiaRegs.SPICTL.bit.CLK_PHASE = 1;				// bit3. Delay phase	0/1: off/on
	SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;			// bit2. 0/1: slave/master mode; 
	SpiaRegs.SPICTL.bit.TALK = 1;					// bit1. Enable talk
	SpiaRegs.SPICTL.bit.SPIINTENA = 0;				// bit0. SPI interrupt 0/1: disable/enable

	SpiaRegs.SPISTS.all = 0x0000;					// FLAGs 0/1: Operate Null/Clear
	SpiaRegs.SPIBRR = 9;							// SPICLK=LSPCLK/(BRR+1)  @LSPCLK = 37.5MHz
	SpiaRegs.SPICCR.bit.SPISWRESET = 1;				// Relinquish SPI from Reset, exit from initial mode
	SpiaRegs.SPIPRI.bit.FREE = 1;					// Set so breakpoints don't disturb mission

}

void spi_fifo_init()
{
	// Initialize SPI FIFO registers
// 	SpiaRegs.SPIFFTX.all = 0x6040;              //  发送FIFO使能，中断禁止
// 	SpiaRegs.SPIFFTX.all = 0xE040;
// 	SpiaRegs.SPIFFRX.all = 0x204f;
// 	SpiaRegs.SPIFFCT.all = 0x0;
	//////////////////////////////////////////////////////////////////////////

	SpiaRegs.SPIFFTX.bit.SPIRST = 0;	// bit15.
	SpiaRegs.SPIFFTX.bit.SPIRST = 1;	// 0: reset SPI register, none for SPIFIFO register
										// 1: none for SPI register, reset SPIFIFO register
	SpiaRegs.SPIFFTX.bit.SPIFFENA = 1;	// bit14. 0/1: disable/enable FIFO's
	SpiaRegs.SPIFFTX.bit.TXFIFO = 0;	// bit13.
										// 0: reset FIFO pointer to 0 and keep in reset mode
										// 1: enable TX-FIFO
	SpiaRegs.SPIFFTX.bit.TXFFST = 0;	// bit12~8. read only. word number in TXBUFFER
	SpiaRegs.SPIFFTX.bit.TXFFINT = 0;	// bit7. read only. 0/1: interrupt flag no/yes 
	SpiaRegs.SPIFFTX.bit.TXFFINTCLR = 1;// bit6. write only. 0/1: null/clear TXFIFINT flag
	SpiaRegs.SPIFFTX.bit.TXFFIENA = 0;	// bit5. 0/1: disable/enable FIFO interrupts
	SpiaRegs.SPIFFTX.bit.TXFFIL = 0;	// bit4~0. 

	SpiaRegs.SPIFFTX.bit.TXFIFO = 1;

//	SpiaRegs.SPIFFTX.all = 0x6040;
//	SpiaRegs.SPIFFTX.all = 0xE040;
	//////////////////////////////////////////////////////////////////////////
	SpiaRegs.SPIFFRX.bit.RXFFOVF = 0;	// bit15. read only.
	SpiaRegs.SPIFFRX.bit.RXFFOVFCLR = 0;// bit14. write only. 0/1: none/clear RXFFOVF flag
	SpiaRegs.SPIFFRX.bit.RXFIFORESET = 1;// bit13. 0/1: reset FIFO RX-POINTER/enable RX FIFO
	SpiaRegs.SPIFFRX.bit.RXFFST = 0;	// bit12~8. read only. word number int RXBUFFER
	SpiaRegs.SPIFFRX.bit.RXFFINT = 0;	// bit7. read only. 0/1: interrupt flag no/yes
	SpiaRegs.SPIFFRX.bit.RXFFINTCLR = 1;// bit6. write only. 0/1: null/ clear RXFFINT flag
	SpiaRegs.SPIFFRX.bit.RXFFIENA = 0;	// bit5. 0/1: disable/enable RXFIFO interrupt
	SpiaRegs.SPIFFRX.bit.RXFFIL = 0x0F;	// bit4~0.

	SpiaRegs.SPIFFRX.bit.RXFIFORESET = 1;

//	SpiaRegs.SPIFFRX.all = 0x204f;
	//////////////////////////////////////////////////////////////////////////
	SpiaRegs.SPIFFCT.bit.TXDLY = 0x00;	// bit7~0.

}

void InitGpio_Spi_Acc()
{
	EALLOW;

	//
	// Configure McBSP-B pins using GPIO regs

	GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 3;        // GPIO24 is MDXB pin
	GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 3;        // GPIO25 is MDRB pin
	GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 3;        // GPIO26 is MCLKXB pin
	GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 1;      // GPIO60 is MCLKRB pin
	GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 3;        // GPIO27 is MFSXB pin
	GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 1;      // GPIO61 is MFSRB pin

	//
	// Enable internal pull-up for the selected pins
	// Pull-ups can be enabled or disabled by the user.
	// This will enable the pullups for the specified pins.
	// Comment out other unwanted lines.
	//
	GpioCtrlRegs.GPAPUD.bit.GPIO24 = 0;     //Enable pull-up on GPIO24 (MDXB)
	GpioCtrlRegs.GPAPUD.bit.GPIO25 = 0;     //Enable pull-up on GPIO25 (MDRB)
	GpioCtrlRegs.GPAPUD.bit.GPIO26 = 0;     //Enable pull-up on GPIO26 (MCLKXB)
	GpioCtrlRegs.GPBPUD.bit.GPIO60 = 0;   //Enable pull-up on GPIO60 (MCLKRB)
	GpioCtrlRegs.GPAPUD.bit.GPIO27 = 0;     //Enable pull-up on GPIO27 (MFSXB)
	GpioCtrlRegs.GPBPUD.bit.GPIO61 = 0;   //Enable pull-up on GPIO61 (MFSRB)

	//
	// Set qualification for selected input pins to asynch only
	// This will select asynch (no qualification) for the selected pins.
	// Comment out other unwanted lines.
	//
	GpioCtrlRegs.GPAQSEL2.bit.GPIO25 = 3;   // Asynch input GPIO25 (MDRB)
	GpioCtrlRegs.GPAQSEL2.bit.GPIO26 = 3;   // Asynch input GPIO26(MCLKXB)
	GpioCtrlRegs.GPBQSEL2.bit.GPIO60 = 3; // Asynch input GPIO60 (MCLKRB)
	GpioCtrlRegs.GPAQSEL2.bit.GPIO27 = 3;   // Asynch input GPIO27 (MFSXB)
	GpioCtrlRegs.GPBQSEL2.bit.GPIO61 = 3; // Asynch input GPIO61 (MFSRB)

	EDIS;
}

void spi_mcbspB_Acc()
{
	//
	// McBSP-B register settings
	//

	//
	// Reset Receiver, Right justify word, Digital loopback dis.
	//
	McbspbRegs.SPCR1.all = 0x0000;
	//
	// Reset FS generator, sample rate generator & transmitter
	//
	McbspbRegs.SPCR2.all = 0x0000;

	McbspbRegs.SPCR1.bit.DLB = 0;	// 0/1: loop back mode disable/enable
	McbspbRegs.SPCR1.bit.RJUST = 0;	// 0: Right alignment, MSBs fill 0
									// 1: Right alignment, MSBs fill symbol bits
									// 2: left alignment, LSBs file 0
									// 3: reserved
	McbspbRegs.SPCR1.bit.CLKSTP = 3;// 时钟停止模式标志位。
									// 0~1: 禁止时钟停止模式
									// 2: 使能该模式，无时钟延时
									// 3: 使能该模式，延时半个时钟。
	McbspbRegs.SPCR1.bit.DXENA = 0;	// 外部DX引脚延时使能标志位
									// 0: 延时禁止
									// 1：延时使能
	McbspbRegs.SPCR1.bit.RINTM = 0;	// 接收中断模式标志位，用来决定何种事件触发McBSP接收中断请求（RINT）
									// 0: RRDY标志位为1，McBSP向CPU发送RINT
									// 1: 

	


	//
	// (CLKXM=CLKRM=FSXM=FSRM= 1, FSXP = 1)
	//
	McbspbRegs.PCR.all = 0x0F08;

	McbspbRegs.SPCR1.bit.DLB = 1;

	//
	// Together with CLKXP/CLKRP determines clocking scheme
	//
	McbspbRegs.SPCR1.bit.CLKSTP = 2;

	McbspbRegs.PCR.bit.CLKXP = 0;	 // CPOL = 0, CPHA = 0 rising edge no delay
	McbspbRegs.PCR.bit.CLKRP = 0;

	//
	// FSX setup time 1 in master mode. 0 for slave mode (Receive)
	//
	McbspbRegs.RCR2.bit.RDATDLY = 01;

	//
	// FSX setup time 1 in master mode. 0 for slave mode (Transmit)
	//
	McbspbRegs.XCR2.bit.XDATDLY = 01;

	McbspbRegs.RCR1.bit.RWDLEN1 = 5;   // 32-bit word
	McbspbRegs.XCR1.bit.XWDLEN1 = 5;   // 32-bit word

	McbspbRegs.SRGR2.all = 0x2000; 	 // CLKSM=1, FPER = 1 CLKG periods
	McbspbRegs.SRGR1.all = 0x000F;	 // Frame Width = 1 CLKG period, CLKGDV=16

	McbspbRegs.SPCR2.bit.GRST = 1;     // Enable the sample rate generator
	delay_loop();                    // Wait at least 2 SRG clock cycles
	McbspbRegs.SPCR2.bit.XRST = 1;     // Release TX from Reset
	McbspbRegs.SPCR1.bit.RRST = 1;     // Release RX from Reset
	McbspbRegs.SPCR2.bit.FRST = 1;     // Frame Sync Generator reset
}

unsigned int GYRO_SPI(const GYRO_MOSI OP, GYRO_MISO *DATA)
{
//	Select_Gyro();
	while (SpiaRegs.SPIFFRX.bit.RXFFST)				// Read RX buffer just to clear interrupt flag
	{
		DATA->data.all = SpiaRegs.SPIRXBUF;
	}
	while (SpiaRegs.SPIFFTX.bit.TXFFST == 15);		// Move on only if TX FIFO not full

	SpiaRegs.SPITXBUF = OP.opc.all;					// Write Operate code
	SpiaRegs.SPITXBUF = OP.data.all;				// Write data to TX buffer;
	while (SpiaRegs.SPIFFRX.bit.RXFFST != 2);			// Wait until transmission done
	DATA->status.all = SpiaRegs.SPIRXBUF;               // Read RX buffer (status word)
	DATA->data.all = SpiaRegs.SPIRXBUF;             // Read RX buffer(data word)

//	Deselect_Gyro();
	return 1;
}

void GyroPowerup()

{
	// Wait 800 ms
	// always delay in application.
    int i;
    for (i = 0; i < 1000; i++)
    {
        DELAY_US(800);
    }

    // Read status register twice to clear error flags
	GYRO_SPI(Gyro_Status, &GD_Status);
	GYRO_SPI(Gyro_Status, &GD_Status);

	// If gyro is ok start reading the sensors
	while (!(GD_Status.data.bits.S_Ok))
	{
		// Gyro error
		GYRO_SPI(Gyro_Status, &GD_Status);				// Read status register to clear error flags
		for (i = 0; i < 100; i++)
        {
            DELAY_US(600);
        }
		GYRO_SPI(Gyro_Status, &GD_Status);				// Read status register to clear flags again
		if (!(((GYRO_REG_STATUS)GD_Status.data.bits.D14).bits.oddParity))
		{
			// If LOOPF still fails, reset gyroscope
		    GYRO_SPI(Gyro_Reset, &GD_Reset);            // Perform soft reset
		    GyroPowerup();
		}

	}
}

int16_t GetGyroRateX()
{
	GYRO_SPI(Gyro_RateX, &GD_RateX);
	return ((int16_t)GD_RateX.data.all >> 2);
}

unsigned int GetGyroMixAccess(int16_t RateX,int16_t Temp)
{
	while (SpiaRegs.SPIFFRX.bit.RXFFST)				// Read RX buffer just to clear interrupt flag
	{
		Temp = SpiaRegs.SPIRXBUF;
	}
	while (SpiaRegs.SPIFFTX.bit.TXFFST == 13);		// Move on only if TX FIFO not full

	SpiaRegs.SPITXBUF = Gyro_mix.rateX_OPC.all;				// Write rate-X address
	SpiaRegs.SPITXBUF = Gyro_mix.Temp_OPC.all;				// Write temperature address
	SpiaRegs.SPITXBUF = Gyro_mix.DUMMY.all;					// Zero Vector
	while (SpiaRegs.SPIFFRX.bit.RXFFST != 3);			// Wait until transmission done
	GD_mix.status.all = SpiaRegs.SPIRXBUF;               // Read RX buffer (status word)
	GD_mix.rateX.all = SpiaRegs.SPIRXBUF;
	GD_mix.temperature.all = SpiaRegs.SPIRXBUF;             // Read RX buffer(data word)
	return 1;
}

//
// spiTxFifoIsr -
//
__interrupt void
spiTxFifoIsr(void)
{
	Uint16 i;
	for (i = 0; i < 16; i++)
	{
		SpiaRegs.SPITXBUF = i;      // Send data
	}



	SpiaRegs.SPIFFTX.bit.TXFFINTCLR = 1;  // Clear Interrupt flag
	PieCtrlRegs.PIEACK.all |= 0x20;  		// Issue PIE ACK
}

//
// spiRxFifoIsr - 
//
__interrupt void
spiRxFifoIsr(void)
{
	Uint16 i, rdata[16];
	for (i = 0; i < SpiaRegs.SPIFFRX.bit.RXFFST; i++)
	{
		rdata[i] = SpiaRegs.SPIRXBUF;		// Read data
	}

	



	SpiaRegs.SPIFFRX.bit.RXFFOVFCLR = 1;  // Clear Overflow flag
	SpiaRegs.SPIFFRX.bit.RXFFINTCLR = 1; 	// Clear Interrupt flag
	PieCtrlRegs.PIEACK.all |= 0x20;       // Issue PIE ack
}
