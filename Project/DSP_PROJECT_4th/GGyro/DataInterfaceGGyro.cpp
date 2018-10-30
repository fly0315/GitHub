/*
 * RxDataHandled.cpp
 *
 *  Created on: 2018年1月11日
 *      Author: ABC
 */
#include "DataInterfaceGGyro.h"
#include "DSP28x_Project.h"
#include <string.h>
SciGyroSensorData GyroSensorData_;
SciAccelerometerSensorData AcceSensorData_;
SciNaviData NaviData_;
CanCheGyro CheGyroData_;

//////////////////////////////////////////////////////////////////////////
// 光纤陀螺数据

SciGyroSensorData::SciGyroSensorData():
	m_i16BGCode(0),
	m_i16WKST(0),
	m_i32Speed(0),
	m_f32Speed(0)
{
    // TODO Auto-generated constructor stub

    
}
void SciGyroSensorData::InitData()
{
	m_i16BGCode = 0;
	m_i16WKST = 0;
	m_i32Speed = 0;
	m_f32Speed = 0;
}
float SciGyroSensorData::GetDataFromFrame(unsigned int *pFrame)
{
	m_i32Speed = MakeData_DoubleWord(
		MakeData_Word(pFrame[2], pFrame[3]),
		MakeData_Word(pFrame[4], pFrame[5])
	);

    m_f32Speed = (float)m_i32Speed * 6.6137566137566137566137566137566e-6;    // 标度因数42 LSB/度/小时。 除以(42*3600)得到度/秒
    return m_f32Speed;
}

//////////////////////////////////////////////////////////////////////////
//加速度计数据
SciAccelerometerSensorData::SciAccelerometerSensorData():
	m_i16BGCode(0),
	m_i16WKST(0),
	m_i16Acc(0),
	m_f32Acc(0)
{
	// TODO Auto-generated constructor stub

	
}

void SciAccelerometerSensorData::InitData()
{
	m_i16BGCode = 0;
	m_i16WKST = 0;
	m_i16Acc = 0;
	m_f32Acc = 0;
}

void SciAccelerometerSensorData::GetDataFromFrame(unsigned int *pFrame,unsigned int flag)
{
	m_i16Acc = MakeData_Word(pFrame[1], pFrame[2]);

	m_i16Temperature = MakeData_Word(pFrame[3], pFrame[4]);

	if (0==flag)
	{
		// 重庆加速度
		m_f32Acc = (float)m_i16Acc * 0.00299096;		        // 1LSB=0.3052mg (code * 0.3052 * 0.001 * 9.8) 计算结果单位为m/s^2
		m_f32Temperature = (float)m_i16Temperature * 0.005;	// 1LSB=0.005摄氏度
	}
	else if (1==flag)
	{
		// 江苏加速度
		m_f32Acc = (float)m_i16Acc * 0.0098;		        // 1LSB=0.001g (code * 9.8 * 0.001) 计算结果单位为m/s^2
		m_f32Temperature = (float)m_i16Temperature * 0.1;	//  摄氏度
	}
	
}
// 惯导数据
SciNaviData::SciNaviData():
	m_i16CommST_Navi(0),
	m_i16WKST_X(0),
	m_i16WKST_Y(0),
	m_i16WKST_Z(0),
	m_i16BGST_X(0),
	m_i16BGST_Y(0),
	m_i16BGST_Z(0),
	m_i32Gyro_X(0),
	m_i32Gyro_Y(0),
	m_i32Gyro_Z(0),
	m_f32Gyro_X(0),
	m_f32Gyro_Y(0),
	m_f32Gyro_Z(0)
{
	// TODO Auto-generated constructor stub

}

void SciNaviData::InitData()
{
	m_i16CommST_Navi = 0;
	m_i16WKST_X = 0;
	m_i16WKST_Y = 0;
	m_i16WKST_Z = 0;
	m_i16BGST_X = 0;
	m_i16BGST_Y = 0;
	m_i16BGST_Z = 0;
	m_i32Gyro_X = 0;
	m_i32Gyro_Y = 0;
	m_i32Gyro_Z = 0;
	m_f32Gyro_X = 0;
	m_f32Gyro_Y = 0;
	m_f32Gyro_Z = 0;
}

void SciNaviData::GetDataFromFrame(unsigned int *pFrame)
{
	unsigned int temp = 0;

	// X Aix Gyro Speed
	(pFrame[4] & 0x80) ? (temp = 0xFF) : (temp = 0);	// 符号判断

	m_i32Gyro_X = MakeData_DoubleWord(
		MakeData_Word(temp, pFrame[4]),
		MakeData_Word(pFrame[5], pFrame[6])
	);

	m_f32Gyro_X = (float)m_i32Gyro_X * 0.001;					// 俯仰，上为正，系数0.001 度/秒  

	// Y Aix Gyro Speed
	(pFrame[7] & 0x80) ? (temp = 0xFF) : (temp = 0);	// 符号判断

	m_i32Gyro_Y = MakeData_DoubleWord(
		MakeData_Word(temp, pFrame[7]),
		MakeData_Word(pFrame[8], pFrame[9])
	);

	m_f32Gyro_Y = (float)m_i32Gyro_Y * 0.001;					// 横滚，右倾为正，系数0.001 度/秒  

	// z Aix Gyro Speed
	(pFrame[10] & 0x80) ? (temp = 0xFF) : (temp = 0);	// 符号判断

	m_i32Gyro_Z = MakeData_DoubleWord(
			MakeData_Word(temp, pFrame[10]), 
			MakeData_Word(pFrame[11], pFrame[12])
		);

	m_f32Gyro_Z = (float)m_i32Gyro_Z * 0.001;					// 右转为正，系数0.001 度/秒  

	m_i16WKST_X = (pFrame[2] >> 0) & 01;
	m_i16WKST_Y = (pFrame[2] >> 1) & 01;
	m_i16WKST_Z = (pFrame[2] >> 2) & 01;

	m_i16BGST_X = (pFrame[3] >> 0) & 01;
	m_i16BGST_Y = (pFrame[3] >> 1) & 01;
	m_i16BGST_Z = (pFrame[3] >> 2) & 01;
}

// 车体陀螺数据
CanCheGyro::CanCheGyro() : 
	m_i16Gyro(0),  
	m_f32Gyro(0),
	m_WkST(0),
	m_BGCode(0)
	
{
	// TODO Auto-generated constructor stub

}

void CanCheGyro::InitData()
{
	m_i16Gyro = 0;
	m_f32Gyro = 0;
	m_WkST = 0;
	m_BGCode = 0;
}

//void CanCheGyro::GetDataFromFrame(unsigned int *pFrame)
//{
//	m_WkST = pFrame[0] & 0x03;
//	m_BGCode = pFrame[1] & 0x03;
//
//	m_i16Gyro = MakeData_Word(pFrame[2], pFrame[3]);
//	m_f32Gyro = (float)m_i16Gyro * 0.00333333333333333333333333333333;	// 300->1度/秒
//}

void CanCheGyro::GetDataFromFrame(volatile MBOX* pMBox)
{
    m_WkST =  pMBox->MDL.byte.BYTE0 & 03;
    m_BGCode =  pMBox->MDL.byte.BYTE1 & 03;

    m_i16Gyro = MakeData_Word( pMBox->MDL.byte.BYTE3, pMBox->MDL.byte.BYTE2 );	// [3]是高字节[2]是低字节。20180507修改
    m_f32Gyro = (float)m_i16Gyro * 0.00333333333333333333333333333333;  // 300->1度/秒

}
//////////////////////////////////////////////////////////////////////////
// 操纵台数据
// 
// SciCztMp::SciCztMp():
//         m_i16Code(0),
//         m_i16Position(0)
// {
//     // TODO Auto-generated constructor stub
// 
// 
// }
// 
// void SciCztMp::InitData()
// {
// 	m_i16Code = 0;
// 	m_i16Position = 0;
// }
// 
// void SciCztMp::SetZeroPosition(signed int Zero)
// {
// 	m_i16Zero = Zero;
// }
// 
// void SciCztMp::GetDataFromFrame(unsigned int *pFrame)
// {
// 	m_i16Code = ((pFrame[1] & 0x3F) << 7) + (pFrame[2] & 0x7F);
// 
// 	if ((m_i16Zero >= 0) && (m_i16Zero < 3591))  //零位在0-160°之间
// 	{
// 		if ((m_i16Code - m_i16Zero) > 4096)		//4096就是半圈
// 			m_i16Position = (m_i16Code - 0x1fff) - m_i16Zero;
// 		else
// 			m_i16Position = m_i16Code - m_i16Zero;
// 	}
// 
// 	if ((m_i16Zero >= 3591) && (m_i16Zero <= 4488)) //零位在160°-200°之间
// 	{
// 		m_i16Position = m_i16Code - m_i16Zero;
// 	}
// 
// 	if ((m_i16Zero > 4488) && (m_i16Zero < 8192))  //零位在200°-360°之间
// 	{
// 		if ((m_i16Code - m_i16Zero) < -4096) //4096就是半圈
// 			m_i16Position = (m_i16Code + 0x1fff) - m_i16Zero;
// 		else
// 			m_i16Position = m_i16Code - m_i16Zero;
// 	}
// 
// }
// 
// IoCztBtnSw::IoCztBtnSw()
// {
//     // TODO Auto-generated constructor stub
// 	m_Btn_Sw_IO.All = 0;
// 	m_Mask = 0x0FFF00E0;
// }
// 
// void IoCztBtnSw::Sampling(unsigned long IOs)
// {
//     // IOs
//     m_Btn_Sw_IO.data.ZERO_SW = (IOs & 0x00000004) >> 2;
// 
// 	// Buttons and Switches that need to be debounced.
// 	m_SampleBuffer_IO[m_Index] = IOs;
// 	(++m_Index) &= (m_SampleCnt - 1);
// 	if ((IOs & m_Mask) != (m_Btn_Sw_IO.All & m_Mask))
// 	{
// 		Debounce();
// 	}
// 
// }
// 
// unsigned long IoCztBtnSw::roundup_power_of_two(unsigned long val)
// {
//     if((val & (val-1)) == 0)
//         return val;
// 
//     unsigned long maxulong = (unsigned long)((unsigned long)~0);
//     unsigned long andv = ~(maxulong&(maxulong>>1));
//     while((andv & val) == 0)
//         andv = andv>>1;
// 
//     return andv<<1;
// }
// 
// void IoCztBtnSw::Init(unsigned int SampleCnt)
// {
//     m_SampleCnt = SampleCnt;
//     if (!Is_Power_Of_2(m_SampleCnt))
//     {
//         m_SampleCnt = roundup_power_of_two(m_SampleCnt);
//     }
//     m_Index = 0;
//     m_SampleBuffer_IO = new unsigned long[m_SampleCnt];
//     memset(m_SampleBuffer_IO, 0, m_SampleCnt * sizeof(unsigned long));
// 	unsigned int SamplePin[PinCnt] = { SA1_L, SA2_L, SA2_H, SB1_L, SB2_L,
// 		SB3_L1, SB3_L2, SB4_L, SB5_L, SB6_L1, SB6_L2, SB7_L, SB8_L, SB9_L1, SB10_L };
// 	for (int i = 0; i < PinCnt; ++i)
// 	{
// 		m_SamplePin[i] = SamplePin[i];
// 	}
// }
// 
// void IoCztBtnSw::Debounce()
// {
// 	unsigned int temp[PinCnt] = { 0 };
// 	for (int i = 0; i < PinCnt; ++i)
// 	{
// 		for (int j = 0; j < m_SampleCnt; ++j)
// 		{
// 			temp[i] += ((m_SampleBuffer_IO[j] >> m_SamplePin[i]) & 1);
// 		}
// 		// more-than-half judge.(>>1 means /2)
// 		(temp[i] > ((m_SampleCnt >> 1) + 1)) ?				
// 			CztBtnSw_.m_Btn_Sw_IO.All |= ((unsigned long)1 << m_SamplePin[i]) :
// 			CztBtnSw_.m_Btn_Sw_IO.All &= (~((unsigned long)1 << m_SamplePin[i]));
// 	}
// }
// 
// // 93C56数据
// SpiCzt93C56::SpiCzt93C56():AT93C56(0,0,0,0)
// {
// 	// TODO Auto-generated constructor stub
//     m_i16Zero_H = 0;
//     m_i16Zero_V = 0;
// 	m_WriteCNT = 0;
// 	m_SetZero_Ready_H = 0;
// 	m_SetZero_Ready_V = 0;
// }
// 
// void SpiCzt93C56::InitEEPROM(void)
// {
// 	AT93C56.Init();
// }
// 
// void SpiCzt93C56::SetZeroCode_X(ENCODER_X Encoder, unsigned int Zero_H)
// { 
// 	m_WriteCNT++;
// 	if (m_WriteCNT <= 10)
// 	{
// 		AT93C56.EWEN();
// 		AT93C56.WRITE(Zero_H, Encoder);
// 		AT93C56.EWDS();
// 	}
// 	
// }
// 
