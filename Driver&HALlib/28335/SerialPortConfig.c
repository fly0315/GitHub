/*
 * CanPortConfig.cpp
 *
 *  Created on: 2018年1月26日
 *      Author: LiuBin
 */

#include "SerialPortConfig.h"

void sci_fifo_init(unsigned int sci_x, unsigned long BR, char Parity, unsigned int TXFFIL, unsigned int RXFFIL)
{
    volatile struct SCI_REGS* SciRegs_x;
	switch (sci_x)
	{
	case sci_a:
		SciRegs_x = &SciaRegs;
		break;
	case sci_b:
		SciRegs_x = &ScibRegs;
		break;
	case sci_c:
		SciRegs_x = &ScicRegs;
		break;
	default:
		break;
	}
// SciRegs_x->SCICCR.all =0x0067;   // 1 stop bit,  No loopback. 1,E,8, // async mode, idle-line protocol

	SciRegs_x->SCICCR.bit.STOPBITS = 0;

	unsigned int myParity = getParity(Parity);
	if (myParity > 1) { SciRegs_x->SCICCR.bit.PARITYENA = 0; }
	else
	{
		SciRegs_x->SCICCR.bit.PARITY = myParity;
		SciRegs_x->SCICCR.bit.PARITYENA = 1;
	}
	SciRegs_x->SCICCR.bit.LOOPBKENA = 0;
	SciRegs_x->SCICCR.bit.ADDRIDLE_MODE = 0;
	SciRegs_x->SCICCR.bit.SCICHAR = 7;

	SciRegs_x->SCICTL1.all = 0x0003;  // enable TX, RX, internal SCICLK,
									// Disable RX ERR, SLEEP, TXWAKE
	SciRegs_x->SCICTL2.bit.TXINTENA = 1;
	SciRegs_x->SCICTL2.bit.RXBKINTENA = 1;
	SciRegs_x->SCIHBAUD = 0x0000;
	SciRegs_x->SCILBAUD = getSciPrd(BR);
/********************************************************************/
/*
* SciRegs_x->SCIFFTX.all=0xC020;
*/
	SciRegs_x->SCIFFTX.bit.SCIRST = 1;
	SciRegs_x->SCIFFTX.bit.SCIFFENA = 1;
	SciRegs_x->SCIFFTX.bit.TXFIFOXRESET = 0;
	SciRegs_x->SCIFFTX.bit.TXFFST = 0;
	SciRegs_x->SCIFFTX.bit.TXFFINT = 0;
	SciRegs_x->SCIFFTX.bit.TXFFINTCLR = 0;
	SciRegs_x->SCIFFTX.bit.TXFFIENA = 1;
	SciRegs_x->SCIFFTX.bit.TXFFIL = TXFFIL;   // 小于等于0个字节触发发送中断
/********************************************************************/

/********************************************************************/
/*
*  SciRegs_x->SCIFFRX.all=0x0030;
*/
	SciRegs_x->SCIFFRX.bit.RXFFOVF = 0;
	SciRegs_x->SCIFFRX.bit.RXFFOVRCLR = 0;
	SciRegs_x->SCIFFRX.bit.RXFIFORESET = 0;
	SciRegs_x->SCIFFRX.bit.RXFFST = 0;
	SciRegs_x->SCIFFRX.bit.RXFFINT = 0;
	SciRegs_x->SCIFFRX.bit.RXFFINTCLR = 0;
	SciRegs_x->SCIFFRX.bit.RXFFIENA = 1;
	SciRegs_x->SCIFFRX.bit.RXFFIL = RXFFIL;  // 收到16个字节触发接收中断
	/********************************************************************/
	SciRegs_x->SCIFFCT.all = 0x00;
	/********************************************************************/
/*
*  SciRegs_x->SCICTL1.all =0x0023;     // Relinquish SCI from Reset
*/
	SciRegs_x->SCICTL1.bit.RXERRINTENA = 0;
	SciRegs_x->SCICTL1.bit.SWRESET = 1;
	SciRegs_x->SCICTL1.bit.TXWAKE = 0;
	SciRegs_x->SCICTL1.bit.SLEEP = 0;
	SciRegs_x->SCICTL1.bit.TXENA = 1;
	SciRegs_x->SCICTL1.bit.RXENA = 1;
	/********************************************************************/
	SciRegs_x->SCIFFTX.bit.TXFIFOXRESET = 1;
	SciRegs_x->SCIFFRX.bit.RXFIFORESET = 1;
}
/*
    SCIA(GPIO28/29)(GPIO35/36)；
    SCIB(O9/11)(GPIO14/15)(GPIO18/19)(GPIO22/23)
    SCIC(GPIO62/63)
*/
// InitScibGpio - This function initializes GPIO pins to function as SCI-A pins
void InitGpio_SCI_A(unsigned int TxPin, unsigned int RxPin)
{
    EALLOW;
    switch (TxPin)
    {
    case 29:
        GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;  // Enable pull-up for GPIO29 (SCITXDA) 
        GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;   // Configure GPIO29 to SCITXDA
        break;
    case 35:
        GpioCtrlRegs.GPBPUD.bit.GPIO35 = 0;  // Enable pull-up for GPIO29 (SCITXDA) 
        GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 1;   // Configure GPIO29 to SCITXDA
        break;
    default:
        break;
    }
    switch (RxPin)
    {
    case 28:
        GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;  // Enable pull-up for GPIO28 (SCIRXDA)
        GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;  // Asynch input GPIO28 (SCIRXDA)
        GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;   // Configure GPIO28 to SCIRXDA
        break;
    case 36:
        GpioCtrlRegs.GPBPUD.bit.GPIO36 = 0;  // Enable pull-up for GPIO28 (SCIRXDA)
        GpioCtrlRegs.GPBQSEL1.bit.GPIO36 = 3;  // Asynch input GPIO28 (SCIRXDA)
        GpioCtrlRegs.GPBMUX1.bit.GPIO36 = 1;   // Configure GPIO28 to SCIRXDA
        break;
    default:
        break;
    }
    EDIS;
}
// InitScibGpio - This function initializes GPIO pins to function as SCI-B pins
void InitGpio_SCI_B(unsigned int TxPin, unsigned int RxPin)
{
    EALLOW;
    switch (TxPin)
    {
    case 9:
        GpioCtrlRegs.GPAPUD.bit.GPIO9 = 0;  //Enable pull-up for GPIO9  (SCITXDB)
        GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 2;  //Configure GPIO9 to SCITXDB
        break;
    case 14:
        GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0; //Enable pull-up for GPIO14 (SCITXDB)
        GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 2; //Configure GPIO14 to SCITXDB
        break;
    case 18:
        GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;  //Enable pull-up for GPIO18 (SCITXDB)
        GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 2;  //Configure GPIO18 to SCITXDB
        break;
    case 22:
        GpioCtrlRegs.GPAPUD.bit.GPIO22 = 0; //Enable pull-up for GPIO22 (SCITXDB)
        GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 3; //Configure GPIO22 to SCITXDB
        break;
        default:
        break;
    }
    switch (RxPin)
    {
    case 11:
        GpioCtrlRegs.GPAPUD.bit.GPIO11 = 0; //Enable pull-up for GPIO11 (SCIRXDB)
        GpioCtrlRegs.GPAQSEL1.bit.GPIO11 = 3;  // Asynch input GPIO11 (SCIRXDB)
        GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 2;  //Configure GPIO11 for SCIRXDB
        break;
    case 15:
        GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0; //Enable pull-up for GPIO15 (SCIRXDB)
        GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3;  // Asynch input GPIO15 (SCIRXDB)
        GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 2;  //Configure GPIO15 for SCIRXDB
        break;
    case 19:
        GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;  //Enable pull-up for GPIO19 (SCIRXDB)
        GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3;  // Asynch input GPIO19 (SCIRXDB)
        GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 2;   //Configure GPIO19 for SCIRXDB
        break;
    case 23:
        GpioCtrlRegs.GPAPUD.bit.GPIO23 = 0; //Enable pull-up for GPIO23 (SCIRXDB)
        GpioCtrlRegs.GPAQSEL2.bit.GPIO23 = 3;  // Asynch input GPIO23 (SCIRXDB)
        GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 3;  //Configure GPIO23 for SCIRXDB
        break;
    default:
        break;
    }
    EDIS;
}
// InitScibGpio - This function initializes GPIO pins to function as SCI-C pins
void InitGpio_SCI_C(unsigned int TxPin, unsigned int RxPin)
{
    EALLOW;

    //
    // Enable internal pull-up for the selected pins
    // Pull-ups can be enabled or disabled disabled by the user.
    // This will enable the pullups for the specified pins.
    //
    GpioCtrlRegs.GPBPUD.bit.GPIO62 = 0;  // Enable pull-up for GPIO62 (SCIRXDC)
    GpioCtrlRegs.GPBPUD.bit.GPIO63 = 0;  // Enable pull-up for GPIO63 (SCITXDC)

    //
    // Set qualification for selected pins to asynch only
    // Inputs are synchronized to SYSCLKOUT by default.
    // This will select asynch (no qualification) for the selected pins.
    //
    GpioCtrlRegs.GPBQSEL2.bit.GPIO62 = 3;  // Asynch input GPIO62 (SCIRXDC)

    //
    // Configure SCI-C pins using GPIO regs
    // This specifies which of the possible GPIO pins will be SCI functional
    // pins.
    //
    GpioCtrlRegs.GPBMUX2.bit.GPIO62 = 1;   // Configure GPIO62 to SCIRXDC
    GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 1;   // Configure GPIO63 to SCITXDC

    EDIS;
}

/****************************************************************************/
/*  tolower                                                                 */
/****************************************************************************/
int tolower(int ch)
{
    /*
    This code depends on two assumptions: (1) all of the letters of the
    alphabet of a given case are contiguous, and (2) the lower and upper
    case forms of each letter are displaced from each other by the same
    constant value.
    */

    if (((unsigned int)ch - (unsigned int)'A') <=
        ((unsigned int)'Z' - (unsigned int)'A'))
    {
        ch += (int)'a' - (int)'A';
    }

    return ch;
}

/****************************************************************************/
/*  toupper                                                                 */
/****************************************************************************/
int toupper(int ch)
{
    /*
    This code depends on two assumptions: (1) all of the letters of the
    alphabet of a given case are contiguous, and (2) the lower and upper
    case forms of each letter are displaced from each other by the same
    constant value.
    */

    if (((unsigned int)ch - (unsigned int)'a') <=
        ((unsigned int)'z' - (unsigned int)'a'))
    {
        ch -= (int)'a' - (int)'A';
    }

    return ch;
}

unsigned int getSciPrd(unsigned long BR)
{
    return (unsigned int)(((float)LSPCLK_FREQ / (BR * 8)) - 1);
}

unsigned int getParity(unsigned char parity)
{
    switch (toupper(parity))
    {
    case 'N':
        return 0xFF;
        break;  // break is not necessary. Just for GJB.
    case 'O':
        return 0;
        break;
    case 'E':
        return 1;
        break;
    default:
        return 0xFF;
        break;
    }
}

