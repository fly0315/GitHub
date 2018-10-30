/*
 * GpioConfig.cpp
 *
 *  Created on: 2018Äê1ÔÂ26ÈÕ
 *      Author: LiuBin
 */

#include "GpioConfig.h"
#include <assert.h>

//
// InitGpio - This function initializes the Gpio to a known (default) state.
//
// For more details on configuring GPIO's as peripheral functions,
// refer to the individual peripheral examples and/or GPIO setup example.
//
void InitGpio_Default(void)
{
    EALLOW;

    //
    // Each GPIO pin can be:
    // a) a GPIO input/output
    // b) peripheral function 1
    // c) peripheral function 2
    // d) peripheral function 3
    // By default, all are GPIO Inputs
    //
    GpioCtrlRegs.GPAMUX1.all = 0x0000;     // GPIO functionality GPIO0-GPIO15
    GpioCtrlRegs.GPAMUX2.all = 0x0000;     // GPIO functionality GPIO16-GPIO31
    GpioCtrlRegs.GPBMUX1.all = 0x0000;     // GPIO functionality GPIO32-GPIO39
    GpioCtrlRegs.GPBMUX2.all = 0x0000;     // GPIO functionality GPIO48-GPIO63
    GpioCtrlRegs.GPCMUX1.all = 0x0000;     // GPIO functionality GPIO64-GPIO79
    GpioCtrlRegs.GPCMUX2.all = 0x0000;     // GPIO functionality GPIO80-GPIO95

    GpioCtrlRegs.GPADIR.all = 0x0000;      // GPIO0-GPIO31 are inputs
    GpioCtrlRegs.GPBDIR.all = 0x0000;      // GPIO32-GPIO63 are inputs
    GpioCtrlRegs.GPCDIR.all = 0x0000;      // GPI064-GPIO95 are inputs

    //
    // Each input can have different qualification
    // a) input synchronized to SYSCLKOUT
    // b) input qualified by a sampling window
    // c) input sent asynchronously (valid for peripheral inputs only)
    //
    GpioCtrlRegs.GPAQSEL1.all = 0x0000;    // GPIO0-GPIO15 Synch to SYSCLKOUT
    GpioCtrlRegs.GPAQSEL2.all = 0x0000;    // GPIO16-GPIO31 Synch to SYSCLKOUT
    GpioCtrlRegs.GPBQSEL1.all = 0x0000;    // GPIO32-GPIO39 Synch to SYSCLKOUT
    GpioCtrlRegs.GPBQSEL2.all = 0x0000;    // GPIO48-GPIO63 Synch to SYSCLKOUT

    //
    // Pull-ups can be enabled or disabled
    //
    GpioCtrlRegs.GPAPUD.all = 0x0000;      // Pullup's enabled GPIO0-GPIO31
    GpioCtrlRegs.GPBPUD.all = 0x0000;      // Pullup's enabled GPIO32-GPIO63
    GpioCtrlRegs.GPCPUD.all = 0x0000;      // Pullup's enabled GPIO64-GPIO79
    //GpioCtrlRegs.GPAPUD.all = 0xFFFF;    // Pullup's disabled GPIO0-GPIO31
    //GpioCtrlRegs.GPBPUD.all = 0xFFFF;    // Pullup's disabled GPIO32-GPIO34
    //GpioCtrlRegs.GPCPUD.all = 0xFFFF;    // Pullup's disabled GPIO64-GPIO79

    EDIS;
}

void pinMode(unsigned int iGpio_x, unsigned int iMode)
{
	/** GPIO_X
	 * ---------------
	 * |6|5|4|3|2|1|0|
	 * ---------------
	 * |0|0|0|1|1|1|1|   00~15 Group_A_1
	 * |0|0|1|1|1|1|1|   16~31 Group_A_2
	 * |0|1|0|1|1|1|1|   32~47 Group_B_1
	 * |0|1|1|1|1|1|1|   48~63 Group_B_2
	 * |1|0|0|1|1|1|1|   64~79 Group_C_1
	 * |1|0|1|1|1|1|1|   80~95 Group_C_2
	 * ---------------
	 * (GPIO_X >> 5) & 0x03  == 0:Group_A_X;    1:Group_B_X    2:Group_C_X
	 * (GPIO_X >> 4) & 0x01  == 0:Group_X_1;    1:Group_X_2
	 */
    assert((iGpio_x < 87) && ((0 == iMode) || (1 == iMode)));
	int group = (iGpio_x >> 5) & 0x03;
	unsigned long bitMask_D = ~((unsigned long) 0x03 << ((iGpio_x & 15) * 2));
	unsigned long bitMask_S = (unsigned long)0x01 << (iGpio_x & 31);

	volatile Uint32* reg = 0x6F86 + ((iGpio_x & 0x20) * 8);
	EALLOW;

	// set 0 as GPIO
	*(reg + ((iGpio_x >> 4) & 0x01)) &= bitMask_D;
    // set 0 as Enable pull-up
	*(reg + 3) &= ~bitMask_S;
    // set direction
	(iMode == OUTPUT) ? (*(reg + 2) |= bitMask_S): (*(reg + 2) &= ~bitMask_S);

//
//    switch (group)
//    {
//    case GROUP_A:
//        // set 0 as GPIO
//        (iGpio_x & 0x10) ?
//            (GpioCtrlRegs.GPAMUX2.all &= bitMask_D):     // Group_A_2 16~31
//            (GpioCtrlRegs.GPAMUX1.all &= bitMask_D);     // Group_A_1  0~15
//	    // set 0 as Enable pull-up
//	    GpioCtrlRegs.GPAPUD.all &= ~bitMask_S;
//        // set direction
//        (iMode == OUTPUT) ?
//            (GpioCtrlRegs.GPADIR.all |= bitMask_S):
//            (GpioCtrlRegs.GPADIR.all &= ~bitMask_S);
//
//        break;
//    case GROUP_B:
//        // set 0 as GPIO
//        (iGpio_x & 0x10) ?
//            (GpioCtrlRegs.GPBMUX2.all &= bitMask_D):    // 32~47 or 48~63
//            (GpioCtrlRegs.GPBMUX1.all &= bitMask_D);
//        // set 0 as Enable pull-up
//        GpioCtrlRegs.GPBPUD.all &= ~bitMask_S;
//        // set direction
//        (iMode == OUTPUT) ?
//            (GpioCtrlRegs.GPBDIR.all |= bitMask_S):
//            (GpioCtrlRegs.GPBDIR.all &= ~bitMask_S);
//
//        break;
//    case GROUP_C:
//            // set 0 as GPIO
//            (iGpio_x & 0x10) ?
//                (GpioCtrlRegs.GPCMUX2.all &= bitMask_D):    // 32~47 or 48~63
//                (GpioCtrlRegs.GPCMUX1.all &= bitMask_D);
//            // set 0 as Enable pull-up
//            GpioCtrlRegs.GPCPUD.all &= ~bitMask_S;
//            // set direction
//            (iMode == OUTPUT) ?
//                (GpioCtrlRegs.GPCDIR.all |= bitMask_S):
//                (GpioCtrlRegs.GPCDIR.all &= ~bitMask_S);
//
//            break;
//    default:
//        break;
//    }
//
    EDIS;
}

void digitalWrite(unsigned int iGpio_x, unsigned int iValue)
{
 	assert((iGpio_x < 87) && ((HIGH == iValue) || (LOW == iValue) || (TOGGLE == iValue)));
//	int group = iGpio_x >> 5;	// x/32
	unsigned long bitMask = (unsigned long)1 << (iGpio_x & 31);//	unsigned long bitValue = (unsigned long)iValue << (iGpio_x & 31);
    volatile Uint32* reg = 0x6FC0 + ((iGpio_x & 0x20) * 4);
    switch (iValue)
    {
    case LOW:
        reg += 2;
        break;
    case HIGH:
        reg += 1;
        break;
    case TOGGLE:
        reg += 3;
        break;
    default:
        break;
    }
    *reg = bitMask;
//	switch (iValue)
//    {
//    case LOW:
//        do
//        {
//            if (GROUP_A == group){(GpioDataRegs.GPACLEAR.all = bitMask); break;};
//            if (GROUP_B == group){(GpioDataRegs.GPBCLEAR.all = bitMask); break;};
//            if (GROUP_C == group){(GpioDataRegs.GPCCLEAR.all = bitMask); break;};
//        }while(0);
//        break;
//    case HIGH:
//        do
//        {
//            if (GROUP_A == group){(GpioDataRegs.GPASET.all = bitMask); break;};
//            if (GROUP_B == group){(GpioDataRegs.GPBSET.all = bitMask); break;};
//            if (GROUP_C == group){(GpioDataRegs.GPCSET.all = bitMask); break;};
//        }while(0);
//        break;
//    case TOGGLE:
//        do
//        {
//            if (GROUP_A == group){(GpioDataRegs.GPATOGGLE.all = bitMask); break;};
//            if (GROUP_B == group){(GpioDataRegs.GPBTOGGLE.all = bitMask); break;};
//            if (GROUP_C == group){(GpioDataRegs.GPCTOGGLE.all = bitMask); break;};
//        }while(0);
//        break;
//    default:
//        break;
//    }

}

int digitalRead(unsigned int iGpio_x)
{
    assert(iGpio_x < 87);
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
    case GROUP_C:
        return ((GpioDataRegs.GPCDAT.all & bitMask) == 0) ? 0 : 1;
        break;
    default:
        break;
    }
    return 0;
}

