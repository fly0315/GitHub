/*
 * GpioConfig.cpp
 *
 *  Created on: 2018年1月26日
 *      Author: LiuBin
 */

#include "GpioConfig.h"
#include <assert.h>
//---------------------------------------------------------------------------
// InitGpio:
//---------------------------------------------------------------------------
// This function initializes the Gpio to a known (default) state.
//
// For more details on configuring GPIO's as peripheral functions,
// refer to the individual peripheral examples and/or GPIO setup example.
void InitGpio_Default(void)
{
    EALLOW;

    // Each GPIO pin can be:
    // a) a GPIO input/output
    // b) peripheral function 1
    // c) peripheral function 2
    // d) peripheral function 3
    /* Configure SCI-C pins using GPIO regs*/
    // This specifies which of the possible GPIO pins will be SCI functional pins.

    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;     // Configure GPIO0 for IO operation
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;     // Configure GPIO1 for IO operation

    GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0;     // Configure GPIO7 for IO operation
    GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 0;     // Configure GPIO8 for IO operation
    GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0;     // Configure GPIO9 for IO operation
    GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 0;    // Configure GPIO10 for IO operation
    GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0;    // Configure GPIO11 for IO operation
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;    // Configure GPIO12 for IO operation
    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;    // Configure GPIO13 for IO operation

    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 0;  // Configure GPIO29 for IO operation

    asm("      nop");
    asm("      nop");
    GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 0;  // Configure GPIO32 for IO operation
    GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 0;  // Configure GPIO33 for IO operation

    GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 0;    // Configure GPIO48 for IO operation
    GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 0;    // Configure GPIO49 for IO operation
    GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 0;    // Configure GPIO50 for IO operation
    GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 0;    // Configure GPIO51 for IO operation
    GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 0;    // Configure GPIO52 for IO operation
    GpioCtrlRegs.GPBMUX2.bit.GPIO53 = 0;    // Configure GPIO53 for IO operation
    GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 0;    // Configure GPIO54 for IO operation
    GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 0;    // Configure GPIO55 for IO operation

    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 2;    // Configure GPIO16 for CANTXB operation
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 2;    // Configure GPIO17 for CANRXB operation
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 3;    // Configure GPIO18 for CANRXA operation
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 3;    // Configure GPIO19 for CANTXA operation

    GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 1;  // Configure GPIO20 for EQEP1A operation
    GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 1;  // Configure GPIO21 for EQEP1B operation
    GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 1;  // Configure GPIO22 for EQEP1S operation
    GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 1;  // Configure GPIO23 for EQEP1I operation

    GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 2;  // Configure GPIO24 for EQEP2A operation
    GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 2;  // Configure GPIO25 for EQEP2B operation
    GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 2;  // Configure GPIO26 for EQEP2I operation
    GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 2;  // Configure GPIO27 for EQEP2S operation

    GpioCtrlRegs.GPBMUX2.bit.GPIO62 = 1;   // Configure GPIO62 for SCIRXDA operation
    GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 1;   // Configure GPIO63 for SCITXDA operation

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

    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 3;  // XREADY
    GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 3;  // XR/W
    GpioCtrlRegs.GPBMUX1.bit.GPIO38 = 3;  // XWE0

    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 3;  // XZCS6

    // Enable internal pull-up for the selected pins
    // Pull-ups can be enabled or disabled disabled by the user.
    // This will enable the pullups for the specified pins.
    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;     // Enable pull-up for GPIO18 (CANRXA)
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;     // Enable pull-up for GPIO19 (CANTXA)

    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;   // Enable pull-up for GPIO16 (CANTXB)
    GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;   // Enable pull-up for GPIO17 (CANRXB)

    GpioCtrlRegs.GPBPUD.bit.GPIO62 = 0;    // Enable pull-up for GPIO62 (SCIRXDA)


    // Each input can have different qualification
    // a) input synchronized to SYSCLKOUT
    // b) input qualified by a sampling window
    // c) input sent asynchronously (valid for peripheral inputs only)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3;   // Asynch qual for GPIO18 (CANRXA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3; // Asynch qual for GPIO17 (CANRXB)

    GpioCtrlRegs.GPBQSEL2.bit.GPIO62 = 3;  // Asynch input GPIO62 (SCIRXDA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO55 = 3;  // Asynch input GPIO55 (SPISOMIA)

    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;      // GPIO0 is output
    GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;      // GPIO1 is output

    GpioCtrlRegs.GPADIR.bit.GPIO7 = 0;      // GPIO7 is input
    GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;      // GPIO8 is output
    GpioCtrlRegs.GPADIR.bit.GPIO9 = 0;      // GPIO9 is input
    GpioCtrlRegs.GPADIR.bit.GPIO10 = 1;      // GPIO10 is output
    GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;      // GPIO11 is output
    GpioCtrlRegs.GPADIR.bit.GPIO12 = 1;      // GPIO12 is output
    GpioCtrlRegs.GPADIR.bit.GPIO13 = 1;      // GPIO13 is output
    GpioCtrlRegs.GPADIR.bit.GPIO29 = 1;      // GPIO29 is output
    GpioCtrlRegs.GPBDIR.bit.GPIO32 = 0;      // GPIO29 is input
    GpioCtrlRegs.GPBDIR.bit.GPIO33 = 0;      // GPIO29 is input
    GpioCtrlRegs.GPBDIR.bit.GPIO48 = 0;      // GPIO48 is input
    GpioCtrlRegs.GPBDIR.bit.GPIO49 = 0;      // GPIO49 is input
    GpioCtrlRegs.GPBDIR.bit.GPIO50 = 0;      // GPIO50 is input
    GpioCtrlRegs.GPBDIR.bit.GPIO51 = 0;      // GPIO51 is input
    GpioCtrlRegs.GPBDIR.bit.GPIO52 = 0;      // GPIO52 is input
    GpioCtrlRegs.GPBDIR.bit.GPIO53 = 0;      // GPIO53 is input
    GpioCtrlRegs.GPBDIR.bit.GPIO54 = 0;      // GPIO54 is input
    GpioCtrlRegs.GPBDIR.bit.GPIO55 = 0;      // GPIO55 is input


    GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0;    // Enable pull-up for GPIO7
    GpioCtrlRegs.GPAPUD.bit.GPIO9 = 0;    // Enable pull-up for GPIO9
    GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;    // Enable pull-up for GPIO32
    GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;    // Enable pull-up for GPIO33
    GpioCtrlRegs.GPBPUD.bit.GPIO48 = 0;    // Enable pull-up for GPIO48
    GpioCtrlRegs.GPBPUD.bit.GPIO49 = 0;    // Enable pull-up for GPIO49
    GpioCtrlRegs.GPBPUD.bit.GPIO50 = 0;    // Enable pull-up for GPIO50
    GpioCtrlRegs.GPBPUD.bit.GPIO51 = 0;    // Enable pull-up for GPIO51
    GpioCtrlRegs.GPBPUD.bit.GPIO52 = 0;    // Enable pull-up for GPIO52
    GpioCtrlRegs.GPBPUD.bit.GPIO53 = 0;    // Enable pull-up for GPIO53
    GpioCtrlRegs.GPBPUD.bit.GPIO54 = 0;    // Enable pull-up for GPIO54
    GpioCtrlRegs.GPBPUD.bit.GPIO55 = 0;    // Enable pull-up for GPIO55

    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;    // Disable pull-up for GPIO0
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;    // Disable pull-up for GPIO1

    GpioCtrlRegs.GPAPUD.bit.GPIO8 = 1;    // Disable pull-up for GPIO8
    GpioCtrlRegs.GPAPUD.bit.GPIO10 = 1;    // Disable pull-up for GPIO10
    GpioCtrlRegs.GPAPUD.bit.GPIO11 = 1;    // Disable pull-up for GPIO11
    GpioCtrlRegs.GPAPUD.bit.GPIO12 = 1;    // Disable pull-up for GPIO12
    GpioCtrlRegs.GPAPUD.bit.GPIO13 = 1;    // Disable pull-up for GPIO13
    GpioCtrlRegs.GPAPUD.bit.GPIO29 = 1;    // Disable pull-up for GPIO29

    GpioDataRegs.GPASET.bit.GPIO10 = 1;     //输出高电平 让7V1灯开始是灭的状态
    GpioDataRegs.GPASET.bit.GPIO11 = 1;     //输出高电平 让7V2灯开始是灭的状态

//  GpioDataRegs.GPACLEAR.bit.GPIO10 = 1;   //输出低电平
//  GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;   //输出低电平

//  GpioDataRegs.GPASET.bit.GPIO29 = 1;
//  GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;   //输出低电平

    GpioDataRegs.GPADAT.bit.GPIO8 = 0;  //给12V 5V 模块不供电

    GpioDataRegs.GPADAT.bit.GPIO1 = 1;  //点亮D6指示灯

    EDIS;

}

void pinMode(unsigned int iGpio_x, unsigned int iMode)
{
	assert((iGpio_x < 63) && ((0 == iMode) || (1 == iMode)));
	int group = iGpio_x >> 5;	// x/32
	unsigned long bitMask = (unsigned long) 1 << (iGpio_x & 31);  // x%32 = x&31  两者相比，位操作指令大幅减少

	EALLOW;
    switch (group)
    {
    case GROUP_A:
        // set 0 as GPIO
        (iGpio_x < 16) ? 
            (GpioCtrlRegs.GPAMUX1.all &= ~bitMask):     // Group A 0~15
            (GpioCtrlRegs.GPAMUX2.all &= ~bitMask);     // Group A 16~31
	    // set 0 as Enable pull-up
	    GpioCtrlRegs.GPAPUD.all &= ~bitMask;  
        // set direction
        (iMode == OUTPUT) ?
            (GpioCtrlRegs.GPADIR.all |= bitMask):
            (GpioCtrlRegs.GPADIR.all &= ~bitMask);
		// GpioCtrlRegs.GPADIR.all &= ~bitMask;    // set 0 first
		// GpioCtrlRegs.GPADIR.all += bitValue;    // set direction
        break;
    case GROUP_B:
        // set 0 as GPIO 
        (iGpio_x < 48) ?                                
            (GpioCtrlRegs.GPBMUX1.all &= ~bitMask):    // 32~47 or 48~63
            (GpioCtrlRegs.GPBMUX2.all &= ~bitMask);
        // set 0 as Enable pull-up
        GpioCtrlRegs.GPBPUD.all &= ~bitMask;   
        // set direction
        (iMode == OUTPUT) ?
            (GpioCtrlRegs.GPBDIR.all |= bitMask):
            (GpioCtrlRegs.GPBDIR.all &= ~bitMask);

        // GpioCtrlRegs.GPBDIR.all &= ~bitMask;    // set 0 first
        // GpioCtrlRegs.GPBDIR.all += bitValue;    // set direction
        break;
    default:
        break;
    }
	EDIS;
}

void digitalWrite(unsigned int iGpio_x, unsigned int iValue)
{
 	assert((iGpio_x < 63) && ((HIGH == iValue) || (LOW == iValue) || (TOGGLE == iValue)));
	int group = iGpio_x >> 5;	// x/32
	unsigned long bitMask = (unsigned long)1 << (iGpio_x & 31);
//	unsigned long bitValue = (unsigned long)iValue << (iGpio_x & 31);
    switch (iValue)
    {
    case LOW:
        (group == GROUP_A)? 
            (GpioDataRegs.GPACLEAR.all = bitMask):
            (GpioDataRegs.GPBCLEAR.all = bitMask);
        break;
    case HIGH:
        (group == GROUP_A)?
            (GpioDataRegs.GPASET.all = bitMask):
            (GpioDataRegs.GPBSET.all = bitMask);
        break;
    case TOGGLE:
        (group == GROUP_A)?
            (GpioDataRegs.GPATOGGLE.all = bitMask):
            (GpioDataRegs.GPBTOGGLE.all = bitMask);
        break;
    default:
        break;
    }

}

int digitalRead(unsigned int iGpio_x)
{
    assert(iGpio_x < 63);
    int group = iGpio_x >> 5;   // x/32
    unsigned long bitMask = (unsigned long) 1 << (iGpio_x & 31);

    switch (group)
    {
    case GROUP_A:
        return ((GpioDataRegs.GPADAT.all & bitMask) == 0) ? 0 : 1;
        break;
    case GROUP_B:
        return ((GpioDataRegs.GPBDAT.all & bitMask) == 0) ? 0 : 1;
        break;
    default:
        break;
    }
    return 0;
}

