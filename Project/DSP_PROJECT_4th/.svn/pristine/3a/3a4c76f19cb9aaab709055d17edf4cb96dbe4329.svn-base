/*
 * DataInterfaceCZT.h
 *
 *  Created on: 2018年2月28日
 *      Author: ABC
 */

#ifndef DATAINTERFACECZT_H_
#define DATAINTERFACECZT_H_
//#include "DSP28x_Project.h"
#include "AT93C56.h"
#include "UnlockFifoBuffer.h"
typedef enum _LED_AND_WZERO_STATE
{
	STATE_LED_NORMAL = 0,	// normal mode
	STATE_LED_WRITING,		// Set zero mode
	STATE_LED_WRITE_OK		// set zero complete mode
}LED_AND_WZERO_STATE;

typedef enum _LAMP_STATE
{
	STATE_LAMP_NONE = 0,
	STATE_LAMP_NORMAL_H = 1, 
	STATE_LAMP_NORMAL_V,
	STATE_LAMP_ZERO_INIT_H,
	STATE_LAMP_ZERO_INIT_V,
	STATE_LAMP_ZERO_ERROR_H,
	STATE_LAMP_ZERO_ERROR_V,
	STATE_FR_OFFLINE,
	
}LAMP_STATE;

class SciCztMp {
public:
    SciCztMp();
private:
    unsigned int m_i16Code;
public:
    int m_i16Position;
	// 用于判断是否正确接收到码盘数据
	int m_bIsDataRcvd;		// 0:初始值，或没有收到码盘数据，或收到错误帧数据
							// 1:收到正确数据
public:
    void InitData();
    unsigned int GetCode() { return m_i16Code; }
    inline unsigned int GetDataFromFrame(unsigned int *pFrame)
    {
        return m_i16Code = ((pFrame[1] & 0x3F) << 7) + (pFrame[2] & 0x7F);
    }
};

// IO Pins
/*
 * **********************************************************************
** IO采集执行机制说明
** 1.定时采集所有IO值，周期1ms。
** 2.每次采集到的IO数据保存至Ring Buffer结构数组
** 3.开关（按钮）需要消抖处理：
** a)m_Btn_Sw_IO为输出IO状态，m_SampleBuffer_IO为Ring Buffer结构数组，长度2^n。
** b)当检测到采集的开关（按钮）值IOs与输出状态m_Btn_Sw_IO不一致时，进行消抖处理。
** c)检查Ring Buffer中的开关（按钮）值，取占绝对优势的状态赋给输出值m_Btn_Sw_IO。
** d)如果没有哪一种状态占绝对优势，则输出值m_Btn_Sw_IO保持之前状态不变。
** e)占绝对优势阈值取为占比75%以上。
** f)采集得到的IO值不只包括开关（按钮）值，还有其他信号，因此需要m_Mask做过滤。
** g)由于消抖处理采用滑动窗口算法，相对耗时，因此只有检测到IO状态发生变化时才执行该算法。
** *******************************************************************************
*/
typedef enum _IO_PINS
{
    DOG = 0,
    LED_D1 = 1,
    ZERO = 2,
    LAMP1 = 3,
    LAMP2 = 4,
    SA1_L = 5,
    SA2_L = 6,
    SA2_H = 7,
    PWREN = 8,
    FR_RST = 12,
    FR_INT = 13,
    SB1_L = 16,
    SB2_L = 17,
    SB3_L1 = 18,
    SB3_L2 = 19,
    SB4_L = 20,
    SB5_L = 21,
    SB6_L1 = 22,
    SB6_L2 = 23,
    SC1 = 24,
    SC2 = 25,
    SC3 = 26,
    SC4 = 27,
    DSIN1 = 32,
    DSIN2 = 33,
    DSIN3 = 48,
    DSIN4 = 49,
    DSIN5 = 50,
    DSIN6 = 51,
    DSIN7 = 52,
    DSIN8 = 53
}IO_PINS;
static const int PinCnt_BTN_SW = 15;
// Button and Switch default state:
// 31,30,29,28|{27,26,25,24|23,22,21,20|19,18,17,16}|15,14,13,12|11,10,9,8|{7,6,5,4|3,2},1,0
static const unsigned long BTN_SW_INIT_STATE = 0x0FFF00FC;
// 
// 31,30,29,28|{27,26,25,24|23,22,21,20|19,18,17,16}|15,14,13,12|11,10,9,8|{7,6,5},4|3,2,1,0
static const unsigned long DEBOUNCE_MASK = 0x0FFF00E0;
#define PREPONDERANCE_IO 0.75f
class IoCztBtnSw {
public:
    IoCztBtnSw();
	~IoCztBtnSw();
    union _BTN_SW
    {
        unsigned long All;
        struct GPADAT_BITS {           // bits   description
            unsigned int WDOG       : 1;           // 0      GPIO0
            unsigned int LED_TEST   : 1;           // 1      GPIO1
            unsigned int ZERO_SW    : 1;           // 2      GPIO2
            unsigned int LAMP_1     : 1;           // 3      GPIO3
            unsigned int LAMP_2     : 1;           // 4      GPIO4
            unsigned int SA1_L      : 1;           // 5      GPIO5
            unsigned int SA2_L      : 1;           // 6      GPIO6
            unsigned int SA2_H      : 1;           // 7      GPIO7
            unsigned int rv1        : 8;           // 8~15   GPIO8~15
            unsigned int SB1_L      : 1;           // 16     GPIO16
            unsigned int SB2_L      : 1;           // 17     GPIO17
            unsigned int SB3_L1     : 1;           // 18     GPIO18
            unsigned int SB3_L2     : 1;           // 19     GPIO19
            unsigned int SB4_L      : 1;           // 20     GPIO20
            unsigned int SB5_L      : 1;           // 21     GPIO21
            unsigned int SB6_L1     : 1;           // 22     GPIO22
            unsigned int SB6_L2     : 1;           // 23     GPIO23
            unsigned int SC1_L      : 1;           // 24     GPIO24
            unsigned int SC2_L      : 1;           // 25     GPIO25
            unsigned int SC3_L1     : 1;           // 26     GPIO26
            unsigned int SC4_L      : 1;           // 27     GPIO27
            unsigned int rv2        : 4;           // 28~31  GPIO28~31

        }data;
    }m_Btn_Sw_IO;

    void BtnIOSampling(volatile unsigned long* pIOS);
    void Init(unsigned int SampleCnt);
private:

    unsigned int m_IOBufferIndex;
    unsigned int m_SampleCnt;
    unsigned long* m_SampleBuffer_IO;
    
    unsigned int m_SamplePin[PinCnt_BTN_SW];
    void Debounce();
	void DebounceEx(unsigned long ChangedBit);
    inline bool Is_Power_Of_2(unsigned long n)
    {
        return (n != 0 && ((n & (n - 1)) == 0));
    }
    unsigned long roundup_power_of_two(unsigned long val);
};

/************************************************************************/
/*
** 93C56   address 0 is write H_zero location
** address 1 is write V_zero location
** one address have 16bits
** 需要把电路板的93C56的6脚置高或者悬空
*/
/************************************************************************/

class SpiCzt93C56 {
public:
    SpiCzt93C56();
    typedef enum
    {
        ENCODER_H = 0,
        ENCODER_V = 1,
    }ENCODER_X;
    void InitEEPROM(void);
private:
    AT93C56_SPI AT93C56;

    int m_WriteCNT;         // 写操作保护，存储器有擦写使用寿命，
                            // 为防止误操作,使用(SetZeroCode_X())时，
                            // 连续调用写函数，擦写操作最多执行10次，
                            // 下次使用写操作需要调用ResetWriteEnable()
public:
    int m_i16Zero_H;
    int m_i16Zero_V;
public:
    inline void ResetWriteEnable() { m_WriteCNT = 0; }
    inline unsigned int ReadZeroCode_H() { return m_i16Zero_H = AT93C56.READ(ENCODER_H); }
    inline unsigned int ReadZeroCode_V() { return m_i16Zero_V = AT93C56.READ(ENCODER_V); }
    void WriteZeroCode_X(ENCODER_X Encoder_X, unsigned int Zero_Code);

private:

};
// H:0---2.5---25---27---30
#define ANGLE_ZERO_H 2.5f
#define ANGLE_HIGH_SPEED_H 20.5f
#define ANGLE_MAX_SPEED_H 23.0f
#define ANGLE_LIMIT_H 32.0f
// V:0---2.5---21---25---32 // 跟盖少凤沟通确认。20180424
#define ANGLE_ZERO_V 2.5f
#define ANGLE_HIGH_SPEED_V 21.0f
#define ANGLE_MAX_SPEED_V 25.0f
#define ANGLE_LIMIT_V 32.0f
// auto calculate
#define SCALE_AIM_SIGNAL_H (6000.0f/(ANGLE_HIGH_SPEED_H-ANGLE_ZERO_H))
#define SCALE_AIM_SIGNAL_V (6000.0f/(ANGLE_HIGH_SPEED_V-ANGLE_ZERO_V))

static const int E2PROM_ZERO_ERRO = 5;	// 铁电存储零位误差值 (取值范围0~8191)

class CztDataBase {
public:
	CztDataBase();
public:
	unsigned int m_PowerOnInitState_H;
	unsigned int m_PowerOnInitState_V;


	LED_AND_WZERO_STATE m_LedState;
	LAMP_STATE m_LampState_H;
	LAMP_STATE m_LampState_V;
	LAMP_STATE m_Fr_Offline;
	int m_AimSignal_H;
	int m_AimSignal_V;
	unsigned int m_bHighSpeed_H;
	unsigned int m_bHighSpeed_V;

	unsigned int IsWriteZeroRight_MPvsROM_H(unsigned int E2PROM_ZERO_H);
	unsigned int IsWriteZeroRight_MPvsROM_V(unsigned int E2PROM_ZERO_V);

	inline void ReloadZero_H(unsigned int Zero_H) { m_ZeroCode_H = Zero_H; }
	inline void ReloadZero_V(unsigned int Zero_V) { m_ZeroCode_V = Zero_V; }

	int Code2AimSignal_H(int Code_H);
	int Code2AimSignal_V(int Code_V);

	void MakeFrWord_IO(IoCztBtnSw *pBtnSw);

	union _FR_WORD_IO
	{
		unsigned int All;
		struct _DATA {           // bits   description
			unsigned int SB1 : 1;			// 0       
			unsigned int SB2 : 1;			// 1      
			unsigned int SB3 : 2;			// 2~3    
			unsigned int SB4 : 1;           // 4      
			unsigned int SB5 : 1;           // 5      
			unsigned int SB6 : 2;           // 6~7    
			unsigned int HSPEED_V : 1;      // 8      
			unsigned int HSPEED_H : 1;      // 9      
			unsigned int SC1 : 1;           // 10     
			unsigned int SC2 : 1;           // 11     
			unsigned int SC3 : 1;           // 12     
			unsigned int SC4 : 1;           // 13     
			unsigned int SA1 : 1;           // 14     
			unsigned int SA2 : 1;           // 15     
		}data;
	}m_Fr_Word_IO;
private:
	int m_ZeroCode_H;
	int m_ZeroCode_V;

	inline unsigned int RoundIn8191(unsigned int i16Value) { return (i16Value & 0x1FFF); }

	inline float Code2Angle(int Code) 
	{ return (float)((float)Code*0.0087890625); }	//Angle=Code*360/8192/5
	
	inline int CodeOnBaseline(int Code_X, int Baseline) 
	{
		return 
			((Code_X = RoundIn8191(Code_X - Baseline)) <= 4096) ?
			(Code_X) : 
			(Code_X -= 8192);

	}
// 	inline int CodeOnBaseline_V(int Code_V) 
// 	{
// 		return 
// 			((Code_V = RoundIn8191(Code_V - m_ZeroCode_V)) <= 4096) ? 
// 			(Code_V) : 
// 			(Code_V -= 8192);
// 
// 	}
	
public:

};

#endif /* DATAINTERFACECZT_H_ */
