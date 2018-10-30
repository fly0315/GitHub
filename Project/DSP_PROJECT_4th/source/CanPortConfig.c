/*
 * CanPortConfig.cpp
 *
 *  Created on: 2018年1月26日
 *      Author: LiuBin
 */

#include "CanPortConfig.h"

void InitECanGpio_Can_A_B()
{
    EALLOW;

    // init CAN A
    //
    // Enable internal pull-up for the selected CAN pins
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0; // Enable pull-up for GPIO18 (CANRXA)
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;  //Enable pull-up for GPIO19 (CANTXA)

    //
    // Set qualification for selected CAN pins to asynch only
    // Inputs are synchronized to SYSCLKOUT by default.
    // This will select asynch (no qualification) for the selected pins.
    //
    GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3;  // Asynch qual for GPIO18 (CANRXA)

    //
    // Configure eCAN-A pins using GPIO regs
    // This specifies which of the possible GPIO pins will be eCAN functional
    // pins.
    //
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 3;    // Configure GPIO18 for CANRXA
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 3;    // Configure GPIO19 for CANTXA

    // Init Can B

    //
    // Enable internal pull-up for the selected CAN pins
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //


    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;   //Enable pull-up for GPIO16(CANTXB)
    GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;  // Enable pull-up for GPIO17(CANRXB)


    //
    // Set qualification for selected CAN pins to asynch only
    // Inputs are synchronized to SYSCLKOUT by default.
    // This will select asynch (no qualification) for the selected pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3; // Asynch qual for GPIO17 (CANRXB)


    //
    // Configure eCAN-B pins using GPIO regs
    // This specifies which of the possible GPIO pins will be eCAN functional
    // pins.
    //
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 2;  // Configure GPIO16 for CANTXB
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 2;  // Configure GPIO17 for CANRXB


    EDIS;
}

void InitCan_Customed(ECAN_X eCan_x)
{

	volatile struct ECAN_REGS* ECanRegs_x;
	volatile struct ECAN_MBOXES* ECanMboxes_x;
	switch (eCan_x)
	{
	case eCan_a:
		ECanRegs_x = &ECanaRegs;
		ECanMboxes_x = &ECanaMboxes;
		break;
	case eCan_b:
		ECanRegs_x = &ECanbRegs;
		ECanMboxes_x = &ECanbMboxes;
		break;
	default:
		break;
	}


    ECanRegs_x->CANES.all = 0;  // disable all email-box before write MSGID

    struct ECAN_REGS ECanShadow_x;

    EALLOW;
    ECanShadow_x.CANMC.all = ECanRegs_x->CANMC.all;
    ECanShadow_x.CANMC.bit.STM = 0;      //  0 :normal mode; 1 :Configure CAN for self-test mode;
    ECanShadow_x.CANMC.bit.SCB = 1;      // 1 :eCan mode
                                        //  The data is received or transmitted least significant byte first.
                                        //    ECanaShadow.CANMC.bit.DBO = 1;
                                        // auto connect to can-bus.
    ECanShadow_x.CANMC.bit.ABO = 1;      // 检测到128*11个隐性位后，模块自动恢复总线连接
    ECanRegs_x->CANMC.all = ECanShadow_x.CANMC.all;
    EDIS;
    //////////////////////////////////////////////////////////////////////////
    EALLOW;
    // Mailboxes can be written to 16-bits or 32-bits at a time
    // Write to the MSGID field of TRANSMIT mailboxes MBOX0 - 15
    ECanMboxes_x->MBOX0.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX1.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX2.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX3.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX4.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX5.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX6.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX7.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX8.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX9.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX10.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX11.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX12.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX13.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX14.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX15.MSGID.all = 0x06A00000;

    // Write to the MSGID field of RECEIVE mailboxes MBOX16 - 31
    ECanMboxes_x->MBOX16.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX17.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX18.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX19.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX20.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX21.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX22.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX23.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX24.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX25.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX26.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX27.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX28.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX29.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX30.MSGID.all = 0x06A00000;
    ECanMboxes_x->MBOX31.MSGID.all = 0x06A00000;

    // Specify that 8 bits will be sent/received
    ECanMboxes_x->MBOX0.MSGCTRL.bit.DLC = 8;
    ECanMboxes_x->MBOX1.MSGCTRL.bit.DLC = 8;
    ECanMboxes_x->MBOX2.MSGCTRL.bit.DLC = 8;
    ECanMboxes_x->MBOX3.MSGCTRL.bit.DLC = 8;
    ECanMboxes_x->MBOX4.MSGCTRL.bit.DLC = 8;
    ECanMboxes_x->MBOX5.MSGCTRL.bit.DLC = 8;
    ECanMboxes_x->MBOX6.MSGCTRL.bit.DLC = 8;
    ECanMboxes_x->MBOX7.MSGCTRL.bit.DLC = 8;
    ECanMboxes_x->MBOX8.MSGCTRL.bit.DLC = 8;
    ECanMboxes_x->MBOX9.MSGCTRL.bit.DLC = 8;
    ECanMboxes_x->MBOX10.MSGCTRL.bit.DLC = 8;
    ECanMboxes_x->MBOX11.MSGCTRL.bit.DLC = 8;
    ECanMboxes_x->MBOX12.MSGCTRL.bit.DLC = 8;
    ECanMboxes_x->MBOX13.MSGCTRL.bit.DLC = 8;
    ECanMboxes_x->MBOX14.MSGCTRL.bit.DLC = 8;
    ECanMboxes_x->MBOX15.MSGCTRL.bit.DLC = 8;

    // Configure Mailboxes 0-31 as Rx
    // Since this write is to the entire register (instead of a bit
    // field) a shadow register is not required.
    ECanRegs_x->CANMD.all = 0xFFFFFFFF;
    // Enable all Mailboxes */
    // Since this write is to the entire register (instead of a bit
    // field) a shadow register is not required.
    ECanRegs_x->CANME.all = 0xFFFFFFFF;

    // Since this write is to the entire register (instead of a bit
    // field) a shadow register is not required.
    ECanRegs_x->CANMIM.all = 0xFFFFFFFF;
    // connect all email box ECAN1INT
    ECanRegs_x->CANMIL.all = 0xFFFFFFFF;
    // overwrite protect
    ECanRegs_x->CANOPC.all = 0;

    EDIS;
    //////////////////////////////////////////////////////////////////////////
    //
    // Mailboxes can be written to 16-bits or 32-bits at a time
    // Write to the MSGID field of TRANSMIT mailboxes MBOX0 - 15
    //
    EALLOW;
    /////////共1个接收邮箱///////////////////////////////////////
    //邮箱0接收车体陀螺信息
    ECanMboxes_x->MBOX0.MSGID.bit.IDE = 0;  // 标识符扩展位
    ECanMboxes_x->MBOX0.MSGID.bit.AME = 0;  // 接收屏蔽使能 0：不使能
    ECanMboxes_x->MBOX0.MSGID.bit.STDMSGID = 0x1A8; // standard frame ID
    ECanMboxes_x->MBOX0.MSGID.bit.EXTMSGID_L = 0;
    ECanMboxes_x->MBOX0.MSGID.bit.EXTMSGID_L = 0;
    ECanMboxes_x->MBOX0.MSGCTRL.bit.DLC = 0x8;  // data length 8 bytes

    ECanShadow_x.CANMD.all = ECanRegs_x->CANMD.all;
    ECanShadow_x.CANMD.bit.MD0 = 1;                 //  邮箱方向 1：接收 0：发送
    ECanRegs_x->CANMD.all = ECanShadow_x.CANMD.all;

    ECanShadow_x.CANME.all = ECanRegs_x->CANME.all;
    ECanShadow_x.CANME.bit.ME0 = 1;                 // 邮箱使能
    ECanRegs_x->CANME.all = ECanShadow_x.CANME.all;
    EDIS;

    EALLOW;
    ECanShadow_x.CANGIM.all = ECanRegs_x->CANGIM.all;
    ECanShadow_x.CANGIM.bit.I1EN = 1;            //
    ECanRegs_x->CANGIM.all = ECanShadow_x.CANGIM.all;

    ECanShadow_x.CANGIM.all = ECanRegs_x->CANGIM.all;
    ECanShadow_x.CANGIM.bit.GIL = 1;            //
    ECanRegs_x->CANGIM.all = ECanShadow_x.CANGIM.all;

    ECanShadow_x.CANMIM.all = ECanRegs_x->CANMIM.all;
    ECanShadow_x.CANMIM.bit.MIM0 = 1;     //邮箱中断被使能
    ECanShadow_x.CANMIM.bit.MIM1 = 1;
    ECanShadow_x.CANMIM.bit.MIM2 = 1;
    ECanShadow_x.CANMIM.bit.MIM3 = 1;
    ECanShadow_x.CANMIM.bit.MIM4 = 1;
    ECanRegs_x->CANMIM.all = ECanShadow_x.CANMIM.all;

    ECanShadow_x.CANMIL.all = ECanRegs_x->CANMIL.all;
    ECanShadow_x.CANMIL.bit.MIL0 = 1;    //邮箱中断产生在中断线1上
    ECanShadow_x.CANMIL.bit.MIL1 = 1;
    ECanShadow_x.CANMIL.bit.MIL2 = 1;
    ECanShadow_x.CANMIL.bit.MIL3 = 1;
    ECanShadow_x.CANMIL.bit.MIL4 = 1;
    ECanRegs_x->CANMIL.all = ECanShadow_x.CANMIL.all;

    EDIS;
}
/*
void InitCanBaudRate_A(void)
{
    //
    // Create a shadow register structure for the CAN control registers. This
    // is needed, since only 32-bit access is allowed to these registers.
    // 16-bit access to these registers could potentially corrupt the register
    // contents or return false data. This is especially true while writing
    // to/reading from a bit (or group of bits) among bits 16 - 31
    //
    struct ECAN_REGS ECanaShadow;
    //
    // Configure bit timing parameters for eCANA
    //
    EALLOW;
    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
    ECanaShadow.CANMC.bit.CCR = 1;            // Set CCR = 1
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

    ECanaShadow.CANES.all = ECanaRegs.CANES.all;

    do
    {
        ECanaShadow.CANES.all = ECanaRegs.CANES.all;
    } while (ECanaShadow.CANES.bit.CCE != 1);   // Wait for CCE bit to be set

    ECanaShadow.CANBTC.all = 0;

    //
    // CPU_FRQ_150MHz is defined in DSP2833x_Examples.h
    //
    //
    // The following block for all 150 MHz SYSCLKOUT
    // (75 MHz CAN clock) - default. Bit rate = 1 Mbps See Note at End of File
    //
    ECanaShadow.CANBTC.bit.BRPREG = 4;
    ECanaShadow.CANBTC.bit.TSEG2REG = 2;
    ECanaShadow.CANBTC.bit.TSEG1REG = 10;



    ECanaShadow.CANBTC.bit.SAM = 1;
    ECanaRegs.CANBTC.all = ECanaShadow.CANBTC.all;

    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
    ECanaShadow.CANMC.bit.CCR = 0;            // Set CCR = 0
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

    ECanaShadow.CANES.all = ECanaRegs.CANES.all;

    do
    {
        ECanaShadow.CANES.all = ECanaRegs.CANES.all;
    } while (ECanaShadow.CANES.bit.CCE != 0);// Wait for CCE bit to be  cleared
    EDIS;
}

void InitCanBaudRate_B(void)
{
    //
    // Create a shadow register structure for the CAN control registers. This
    // is needed, since only 32-bit access is allowed to these registers.
    // 16-bit access to these registers could potentially corrupt the register
    // contents or return false data. This is especially true while writing
    // to/reading from a bit (or group of bits) among bits 16 - 31
    //
    struct ECAN_REGS ECanbShadow;
    EALLOW;
    //
    // Configure bit timing parameters for eCANB
    //
    ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
    ECanbShadow.CANMC.bit.CCR = 1;            // Set CCR = 1
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

    ECanbShadow.CANES.all = ECanbRegs.CANES.all;

    do
    {
        ECanbShadow.CANES.all = ECanbRegs.CANES.all;
    } while (ECanbShadow.CANES.bit.CCE != 1); // Wait for CCE bit to be  cleared

    ECanbShadow.CANBTC.all = 0;

    //
    // CPU_FRQ_150MHz is defined in DSP2833x_Examples.h
    //

    //
    // The following block for all 150 MHz SYSCLKOUT
    // (75 MHz CAN clock) - default. Bit rate = 1 Mbps See Note at end of file
    //
    ECanbShadow.CANBTC.bit.BRPREG = 4;
    ECanbShadow.CANBTC.bit.TSEG2REG = 2;
    ECanbShadow.CANBTC.bit.TSEG1REG = 10;


    ECanbShadow.CANBTC.bit.SAM = 1;
    ECanbRegs.CANBTC.all = ECanbShadow.CANBTC.all;

    ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
    ECanbShadow.CANMC.bit.CCR = 0;            // Set CCR = 0
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

    ECanbShadow.CANES.all = ECanbRegs.CANES.all;

    do
    {
        ECanbShadow.CANES.all = ECanbRegs.CANES.all;
    } while (ECanbShadow.CANES.bit.CCE != 0);// Wait for CCE bit to be  cleared
    EDIS;
}
*/

