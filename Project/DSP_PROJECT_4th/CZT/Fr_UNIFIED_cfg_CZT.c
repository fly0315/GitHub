/******************************************************************************
* 4th_GG
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2005 Freescale Semiconductor, Inc.
* (c) Copyright 2001-2004 Motorola, Inc.
* ALL RIGHTS RESERVED.
*
***************************************************************************//*!
*
* @file      Fr_UNIFIED_cfg.c
*
* @author    rg003c
*
* @version   1.0.1.0
*
* @date      Apr-26-2007
*
* @brief     FlexRay High-Level Driver implementation.
*            This file contains structures with configuration data. An user may
*            modify these structures.
*
******************************************************************************/

#include "Fr_UNIFIED.h"

/******************************************************************************
* Global variables
******************************************************************************/

// Hardware configuration structure
// Number of MB in Segment 1:   11
// Number of MB in Segment 2:   8
// FIFO Depth:                  10
const Fr_HW_config_type Fr_HW_cfg_00 =//设置总通讯节点 20120310cwr
{
	0x100000,//0x200000,  //28335    XZCS7//20110218                                 //  0x000400,     // FlexRay module base address
	0x100400, //4310memory base address //20110218   //0x0FF000     // FlexRay memory base address (MB headers start at this address)
	FR_MFR4310,    // Type of Freescale FlexRay module          //FR_MC9S12XF,     // Type of Freescale FlexRay module
	FALSE,          // Synchronization filtering
					//FR_EXTERNAL_OSCILLATOR,
	FR_INTERNAL_SYSTEM_BUS_CLOCK,
	0,              // Prescaler value
	10,             // Data size - segment 1
	10,              // Data size - segment 2
	10,//20110413 33, // Last MB in segment 1 (Number of MB in Segment1 - 1),需修改
	10,             // Last individual MB (except FIFO); (Number of MB in Segment1 + Number of MB in Segment2 - 1),需修改
	11, //20110522  56            // Total number of used MB (Last_individual_MB + 1 + FIFO),需修改
	TRUE,           // Allow coldstart
	0,              // The value of the TIMEOUT bit field in the SYMATOR register - not implemented for all FlexRay modules
	0,              // Offset of the Sync Frame Table in the FlexRay memory
	FR_DUAL_CHANNEL_MODE    // Single channel mode disabled
};


// Transmit MB configuration structure
// Slot 1, payload length 16 Words, Double buffered MB, State transmission mode, 
// interrupt enabled from transmit side of double MB, channel AB, filtering disabled, streaming mode
const Fr_transmit_buffer_config_type Fr_tx_buffer_Frame_CZT_10ms_cfg =
{
	31,                           // Transmit frame ID
	0x5DD,         //2012                // Header CRC
	10,                          // Payload length
	FR_DOUBLE_TRANSMIT_BUFFER,   // Transmit MB buffering
	FR_STATE_TRANSMISSION_MODE,  // Transmission mode
	FR_IMMEDIATE_COMMIT_MODE,    // Transmission commit mode
	FR_CHANNEL_AB,               // Transmit channels
	FALSE,                       // Payload preamble
	TRUE,                      // Transmit cycle counter filter enable
	1,                           // Transmit cycle counter filter value
	1,                           // Transmit cycle counter filter mask
	FALSE,                       // Transmit MB interrupt enable
	FALSE                        // FALSE - interrupt is enabled at commit side, TRUE - interrupt is enabled at transmit side
};

// Configuration data for Shadow Message Buffers
// All shadow MB are configured
const Fr_receive_shadow_buffers_config_type Fr_rx_shadow_cfg =//shadow占用的单元不能使用 cwr20120310
{
	TRUE,       // Rx shadow buffer for channel A, seg 1 - enabled?
	FALSE,       // Rx shadow buffer for channel A, seg 2 - enabled?
	TRUE,       // Rx shadow buffer for channel B, seg 1 - enabled?
	FALSE,       // Rx shadow buffer for channel B, seg 2 - enabled?
	5,         // Ch A, seg 1 - the current index of the MB header field,需修改
	0,         // Ch A, seg 2 - the current index of the MB header field,需修改
	6,         // Ch B, seg 1 - the current index of the MB header field,需修改
	0          // Ch B, seg 2 - the current index of the MB header field,需修改
};
// Following array is used to determine which message buffers defined in Fr_buffer_cfg_xx structure
// will be used for the FlexRay CC configuration


const Fr_index_selector_type Fr_buffer_cfg_set_00[] =
{
	0,1, FR_LAST_MB
};


// Array of structures with message buffer configuration information
// The MBs 2, 4, 5, 6 and 7 will not be configured
const Fr_buffer_info_type Fr_buffer_cfg_00[] =//20120627还需要修改
{ // Buffer type         Configuration structure ptr         MB index    xx = configuration index used by Fr_buffer_cfg_set_xx
	{ FR_TRANSMIT_BUFFER, &Fr_tx_buffer_Frame_CZT_10ms_cfg,     0 },         // 00
    { FR_RECEIVE_SHADOW,  &Fr_rx_shadow_cfg,                    4 },         // 01
};

/*
// Configuration data for absolute timer T1
const Fr_timer_config_type Fr_timer_1_cfg =
{
FR_TIMER_T1, 						// Timer number (T1 or T2)
FR_ABSOLUTE, 						// Timer timebase
FR_REPETITIVE, 						// Timer repetition mode
2050, 								// Timer macrotick offset
0, 									// Timer cycle filter mask, only for absolute timer
0									// Timer cycle filter value, only for absolute timer
};

// Configuration data for relative timer T2
const Fr_timer_config_type Fr_timer_2_cfg =
{
FR_TIMER_T2, 						// Timer number (T1 or T2)
FR_RELATIVE, 						// Timer timebase
FR_REPETITIVE, 						// Timer repetition mode
1000000, 							// Timer macrotick offset
0, 									// Timer cycle filter mask, only for absolute timer
0									// Timer cycle filter value, only for absolute timer
};

// Array with timers configuration information
// Following array is used to determine which timers will be used for the FlexRay CC configuration
const Fr_timer_config_type * Fr_timers_cfg_00_ptr[] =
{
&Fr_timer_1_cfg,                    // Pointer to configuration structure for timer T1
&Fr_timer_2_cfg,                    // Pointer to configuration structure for timer T2
NULL
};
*/
/* Structure of this type contains configuration
information of the one low level parameters set */
/*炮控总线10M采集驱动参数设置，版本V1.0，
仅供测试用，非正式版，
write by zp，20120629 */
const Fr_low_level_config_type Fr_low_level_cfg_set_00 =
{
	10,         /*  gColdStartAttempts-----G_COLD_START_ATTEMPTS */
	7,         /*  gdActionPointOffset-----GD_ACTION_POINT_OFFSET */
	91,         /*  gdCASRxLowMax-----GD_CAS_RX_LOW_MAX */
	1,          /*  gdDynamicSlotIdlePhase-----GD_DYNAMIC_SLOT_IDLE_PHASE */
	10,         /*  gdMinislot-----GD_MINISLOT */
	5,          /*  gdMinislotActionPointOffset-----GD_MINI_SLOT_ACTION_POINT_OFFSET */
	50,        /*  gdStaticSlot-----GD_STATIC_SLOT */
	0,          /*  gdSymbolWindow-----GD_SYMBOL_WINDOW */
	11,          /*  gdTSSTransmitter-----GD_TSS_TRANSMITTER */
	59,         /*  gdWakeupSymbolRxIdle-----GD_WAKEUP_SYMBOL_RX_IDLE */
	50,         /*  gdWakeupSymbolRxLow-----GD_WAKEUP_SYMBOL_RX_LOW */
	301,         /*  gdWakeupSymbolRxWindow-----GD_WAKEUP_SYMBOL_RX_WINDOW */
	180,         /*  gdWakeupSymbolTxIdle-----GD_WAKEUP_SYMBOL_TX_IDLE */
	60,         /*  gdWakeupSymbolTxLow-----GD_WAKEUP_SYMBOL_TX_LOW */
	2,          /*  gListenNoise-----G_LISTEN_NOISE */
	5000,       /*  gdCycle-----G_MACRO_PER_CYCLE */
	8,          /*  gMaxWithoutClockCorrectionPassive-----G_MAX_WITHOUT_CLOCK_CORRECTION_PASSIVE */
	6,          /*  gMaxWithoutClockCorrectionFatal-----G_MAX_WITHOUT_CLOCK_CORRECTION_FATAL */
	90,          /*  gNumberOfMinislots-----G_NUMBER_OF_MINISLOTS */
	80,          /*  gNumberOfStaticSlots-----G_NUMBER_OF_STATIC_SLOTS */
	4985,        /*  gOffsetCorrectionStart-----G_OFFSET_CORRECTION_START */
	10,          /*  gPayloadLengthStatic-----G_PAYLOAD_LENGTH_STATIC */
	10,          /*  gSyncNodeMax-----G_SYNC_NODE_MAX */
	0,          /*  gNetworkManagementVectorLength-----G_NETWORK_MANAGEMENT_VECTOR_LENGTH */
	FALSE,      /*  -----G_ALLOW_HALT_DUE_TO_CLOCK */
	20,         /*  -----G_ALLOW_PASSIVE_TO_ACTIVE */
	FR_CHANNEL_AB,  /*  pChannels-----P_CHANNELS */
	212,        /*  pdAcceptedStartupRange-----PD_ACCEPTED_STARTUP_RANGE */
	2,          /*  pClusterDriftDamping-----P_CLUSTER_DRIFT_DAMPING */
	56,         /*  pDecodingCorrection-----P_DECODING_CORRECTION */
	1,          /*  pDelayCompensation[A]-----P_DELAY_COMPENSATION_A */
	1,          /*  pDelayCompensation[B]-----P_DELAY_COMPENSATION_B */
	401202,      /*  pdListenTimeout-----PD_LISTEN_TIMEOUT */
	601,         /*  pdMaxDrift-----PD_MAX_DRIFT */
	0,          /*  pExternOffsetCorrection-----P_EXTERN_OFFSET_CORRECTION */
	0,          /*  pExternRateCorrection-----P_EXTERN_RATE_CORRECTION */
	31,          /*  pKeySlotId-----P_KEY_SLOT_ID $$$$$$0$$$$$$$$$$$$$$$$$$$$$$$*/
	TRUE,       /*  pKeySlotUsedForStartup-----P_KEY_SLOT_USED_FOR_STARTUP $$$$$$$$$$$$$$$$$$$$*/
	TRUE,       /*  pKeySlotUsedForSync-----P_KEY_SLOT_USED_FOR_SYNC $$$$$$$$$$$$$$$$$$$$$$$$$*/
	0x5DD,      /*  -----P_KEY_SLOT_HEADER_CRC $$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
	16,          /*  pLatestTx-----P_LATEST_TX */
	9,         /*  pMacroInitialOffset[A]-----P_MACRO_INITIAL_OFFSET_A */
	9,         /*  pMacroInitialOffset[B]-----P_MACRO_INITIAL_OFFSET_B */
	23,          /*  pMicroInitialOffset[A]-----P_MICRO_INITIAL_OFFSET_A */
	23,          /*  pMicroInitialOffset[B]-----P_MICRO_INITIAL_OFFSET_B */
	200000,     /*  pMicroPerCycle-----P_MICRO_PER_CYCLE */
	141,         /*  pOffsetCorrectionOut-----P_OFFSET_CORRECTION_OUT */
	601,         /*  pRateCorrectionOut-----P_RATE_CORRECTION_OUT */
	FALSE,      /*  pSingleSlotEnabled-----P_SINGLE_SLOT_ENABLED */
	FR_CHANNEL_A,   /*  pWakeupChannel-----P_WAKEUP_CHANNEL $$$$$$$$$$$$$$$$$$$$$$$$$*/
	44,         /*  pWakeupPattern-----P_WAKEUP_PATTERN */
	40,         /*  pMicroPerMacroNom-----P_MICRO_PER_MACRO_NOM */
	16           /*  pPayloadLengthDynMax-----P_PAYLOAD_LENGTH_DYN_MAX */
};


