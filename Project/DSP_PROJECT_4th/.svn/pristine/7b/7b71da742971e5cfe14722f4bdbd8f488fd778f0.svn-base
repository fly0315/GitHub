/*
 * RxDataHandled.h
 *
 *  Created on: 2018年1月11日
 *      Author: ABC
 */

#ifndef RXDATA_INTERFACE
#define RXDATA_INTERFACE
#include "DSP28x_Project.h"
#include "AT93C56.h"


// 考虑修改成宏定义，效率更高
inline unsigned int MakeData_Word(unsigned int i16HiByte,unsigned int i16LoByte)
{
	return (unsigned int)
		(
			(((unsigned int)i16HiByte & 0xFF) << 8) 
				+ (((unsigned int)i16LoByte & 0xFF) << 0)
		);
}
// 考虑修改成宏定义，效率更高
inline unsigned long MakeData_DoubleWord(unsigned int i16HiWord, unsigned int i16LoWord)
{
	return (unsigned long)
		(
			(((unsigned long)i16HiWord & 0xFFFF) << 16)
				+ (((unsigned long)i16LoWord & 0xFFFF) << 0) 
		);
}

class SciGyroSensorData {
public:
    SciGyroSensorData();

private:
    signed long m_i32Speed;
public:
	unsigned int m_i16BGCode;
	unsigned int m_i16WKST;
    float m_f32Speed;
public:
    float GetDataFromFrame(unsigned int *pFrame);
	void InitData();
};

class SciAccelerometerSensorData {
public:
	SciAccelerometerSensorData();

private:
	int m_i16Acc;
	int m_i16Temperature;
public:
	unsigned int m_i16BGCode;
	unsigned int m_i16WKST;
	float m_f32Acc;
	float m_f32Temperature;
public:
	void InitData();
	void GetDataFromFrame(unsigned int *pFrame,unsigned int flag);	// flag=0: 重庆 flag=1: 江苏
};

class SciNaviData {
public:
	SciNaviData();

private:
	long m_i32Gyro_X;
	long m_i32Gyro_Y;
	long m_i32Gyro_Z;

public:
	unsigned int m_i16CommST_Navi;

	unsigned int m_i16WKST_X;
	unsigned int m_i16WKST_Y;
	unsigned int m_i16WKST_Z;

	unsigned int m_i16BGST_X;
	unsigned int m_i16BGST_Y;
	unsigned int m_i16BGST_Z;

	float m_f32Gyro_X;
	float m_f32Gyro_Y;
	float m_f32Gyro_Z;

public:
	void InitData();
	void GetDataFromFrame(unsigned int *pFrame);
};

class CanCheGyro {
public:
    CanCheGyro();
private:
    int m_i16Gyro;

public:
    float m_f32Gyro;
	unsigned int m_WkST;
	unsigned int m_BGCode;

public:
	void InitData();
//	void GetDataFromFrame(unsigned int *pFrame);
	void GetDataFromFrame(volatile MBOX* pMBox);


};
// 
// class SciCztMp {
// public:
//     SciCztMp();
// private:
//     unsigned int m_i16Code;
// 	unsigned int m_i16Zero;
// public:
//     int m_i16Position;
// public:
// 	void InitData();
// 	signed int GetRawCode() { return m_i16Code; };
// 	void SetZeroPosition(signed int Zero);
//     void GetDataFromFrame(unsigned int *pFrame);
// };
// 
// // IO Pins
 typedef enum _IO_PINS
 {
     PWREN = 3,     
	 DOG = 8,
	 RESET4310 = 12,
	 INT_CC4310 = 13,

 }IO_PINS;
// static const int PinCnt = 15;
// class IoCztBtnSw {
// public:
//     IoCztBtnSw();
// 
// 	union _BTN_SW
// 	{
// 		unsigned long All;
// 		struct GPADAT_BITS {           // bits   description
// 			unsigned int WDOG : 1;			//0			GPIO0
// 			unsigned int LED_TEST : 1;		// 1		GPIO1
// 			unsigned int ZERO_SW : 1;		// 2		GPIO2
// 			unsigned int LAMP_1 : 1;            // 3      GPIO3
// 			unsigned int LAMP_2 : 1;            // 4      GPIO4
// 			unsigned int SA1_L : 1;            // 5      GPIO5
// 			unsigned int SA2_L : 1;            // 6      GPIO6
// 			unsigned int SA2_H : 1;            // 7      GPIO7
// 			unsigned int rv1 : 8;            // 8      GPIO8~15
// 			unsigned int SB1_L : 1;           // 16     GPIO16
// 			unsigned int SB2_L : 1;           // 17     GPIO17
// 			unsigned int SB3_L1 : 1;           // 18     GPIO18
// 			unsigned int SB3_L2 : 1;           // 19     GPIO19
// 			unsigned int SB4_L : 1;           // 20     GPIO20
// 			unsigned int SB5_L : 1;           // 21     GPIO21
// 			unsigned int SB6_L1 : 1;           // 22     GPIO22
// 			unsigned int SB6_L2 : 1;           // 23     GPIO23
// 			unsigned int SB7_L : 1;           // 24     GPIO24
// 			unsigned int SB8_L : 1;           // 25     GPIO25
// 			unsigned int SB9_L1 : 1;           // 26     GPIO26
// 			unsigned int SB10_L : 1;           // 27     GPIO27
// 			unsigned int rv2 : 4;           // 28     GPIO28~31
// 
// 		}data;
// 	}m_Btn_Sw_IO;
// 
// 	void Sampling(unsigned long IOS);
// 	void Init(unsigned int SampleCnt);
// private:
// 	
// 	unsigned int m_Index;
// 	unsigned int m_SampleCnt;
// 	unsigned long* m_SampleBuffer_IO;
// 	unsigned long m_Mask;
// 	unsigned int m_SamplePin[PinCnt];
// 	void Debounce();
// 	inline bool Is_Power_Of_2(unsigned long n)
//     {
//         return (n != 0 && ((n & (n - 1)) == 0));
//     }
// 	unsigned long roundup_power_of_two(unsigned long val);
// };

/************************************************************************/
/*
** 93C56   address 0 is write H_zero location
** address 1 is write V_zero location
** one address have 16bits
** 需要把电路板的93C56的6脚置高或者悬空
*/
/************************************************************************/ 

// class SpiCzt93C56 {
// public:
//     SpiCzt93C56();
// 
// 	void InitEEPROM(void);
// private:
// 	AT93C56_SPI AT93C56;
// 	typedef enum
// 	{
// 		ENCODER_H = 0,
// 		ENCODER_V = 1,
// 	}ENCODER_X;	
// 	int m_WriteCNT;			// 写操作保护，存储器有擦写使用寿命，
// 							//为防止误操作使用(SetZeroCode_X())，
// 							//连续调用写函数，擦写操作最多执行10次，
// 							//下次使用写操作需要调用
// public:
// 	int m_i16Zero_H;
// 	int m_i16Zero_V;
// 	unsigned int m_SetZero_Ready_H;
// 	unsigned int m_SetZero_Ready_V;
// public:
// 	inline unsigned int IsZeroValid() { return (((m_i16Zero_H > 112) && (m_i16Zero_H < 8079)) && ((m_i16Zero_V > 112) && (m_i16Zero_V < 8079))); }
// 	inline unsigned int IsInitZero() {return GpioDataRegs.GPADAT.bit.GPIO2;}
// 	
// 	inline void ResetWriteEnable() { m_WriteCNT = 0; }
// 	inline unsigned int GetZeroCode_H() { return m_i16Zero_H = AT93C56.READ(ENCODER_H); }
// 	inline unsigned int GetZeroCode_V() { return m_i16Zero_V = AT93C56.READ(ENCODER_V); }
// 	void SetZeroCode_X(ENCODER_X Encoder, unsigned int Zero_H);
// 
// private:
// 
// };

#endif /* RXDATAHANDLED_H_ */
